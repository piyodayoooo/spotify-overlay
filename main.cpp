#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define USE_MOCK_MODE 0

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <timeapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <regex>
#include <memory>
#include <d2d1effects.h>
#include <d2d1effects.h>
#include <dwmapi.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "windowsapp")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(linker, "/subsystem:windows")

#define CURL_STATICLIB

#include <avrt.h>
#include <thread>
#include <functional>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <cctype>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "UIWidgets.h"
#include "resource.h"
#include "LyricsManager.h"

#define ID_TRAY_TOGGLE_LYRICS_MGR 1006

using json = nlohmann::json;
using namespace Gdiplus;

std::string UrlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        }
        else {
            escaped << std::uppercase << '%' << std::setw(2) << int((unsigned char)c) << std::nouppercase;
        }
    }
    return escaped.str();
}

std::string UrlDecode(const std::string& str) {
    std::string ret;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            if (i + 2 < str.length()) {
                int ii;
                std::istringstream iss(str.substr(i + 1, 2));
                if (iss >> std::hex >> ii) { ret += static_cast<char>(ii); i += 2; }
            }
        }
        else if (str[i] == '+') ret += ' ';
        else ret += str[i];
    }
    return ret;
}

std::wstring Utf8ToWide(const std::string& utf8Str) {
    std::string decoded = UrlDecode(utf8Str);
    if (decoded.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &decoded[0], (int)decoded.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &decoded[0], (int)decoded.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string Base64Encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::wstring SanitizeFileName(std::wstring name) {
    std::wstring invalid = L"\\/:*?\"<>|";
    for (wchar_t& c : name) {
        if (invalid.find(c) != std::wstring::npos) c = L'_';
    }
    return name;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

std::string CLIENT_ID = "";
std::string CLIENT_SECRET = "";

std::wstring g_config_file;
std::wstring g_token_file;
std::wstring g_lyrics_dir;

bool g_is_3d_lyrics = false;
bool g_show_lyrics = true;
bool g_use_accent_color = true;
bool g_hide_on_pause = false;
int g_api_mode = 1;
float g_bg_alpha = 0.9f;
float g_album_alpha = 1.0f;
int g_uiStyle = 0;
int g_lyrics_glow_mode = 0;
bool g_enable_blur = false;
float g_blur_strength = 0.5f;
int g_color_trans_mode = 0;
float g_color_trans_rotation = 45.0f;
static float s_globalSweepStart = 0.0f;
static float s_globalSweepEnd = 0.0f;
static std::string s_lastSweepTrack = "";
static int s_render_glow_mode = 0;
static int s_targetGlowMode = 0;
static float s_targetSweepStart = 0.0f;
static float s_targetSweepEnd = 0.0f;
static float s_smoothSweepStart = 0.0f;
static float s_smoothSweepEnd = 0.0f;
static int s_lastSweepLineIndex = -1;

inline float EaseOutQuint(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float inv = 1.0f - t;
    return 1.0f - (inv * inv * inv * inv * inv);
}
inline float EaseInQuint(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * t * t * t;
}
inline float EaseOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return (t >= 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

inline float EaseIn(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return (t <= 0.0f) ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
}

enum AppState { STATE_INIT, STATE_INPUT_ID, STATE_INPUT_SECRET, STATE_RUNNING };
AppState g_app_state = STATE_INIT;
std::string g_input_buffer = "";
std::string g_error_message = "";

std::string g_refresh_token = "";
const std::string REDIRECT_URI = "http://127.0.0.1:8888/callback";

#define ID_TRAY_RESET_POS 1004
#define ID_TRAY_TOGGLE_MOVE_MODE 1005
#define ID_TRAY_TOGGLE_USE_ACCENT 1007

bool g_move_mode = false;
bool g_is_attracted = false;
int g_init_pos_x = -1;
int g_init_pos_y = -1;
float g_pos_x = 0.0f;
float g_pos_y = 0.0f;
float g_target_x = 0.0f;
float g_target_y = 0.0f;
bool g_is_moving = false;
int g_saved_pos_x = -9999;
int g_saved_pos_y = -9999;
int g_lyrics_pos_x = -9999;
int g_lyrics_pos_y = -9999;
float g_overlay_alpha = 0.0f;
float g_overlay_offset_y = 30.0f;
bool g_is_exiting = false;

const wchar_t* FontFileName = L"font.ttc";
const wchar_t* NormalFontFileName = L"Norfont.ttc";
const wchar_t* NumberFontFileName = L"Numfont.ttc";
const wchar_t* EnglishFontFileName = L"Engfont.ttc";

std::vector<char> g_image_bytes;
bool g_image_updated = false;
std::mutex g_image_mtx;

std::wstring g_last_state_file;
std::wstring g_album_art_file;
std::wstring g_debug_status = L"Initializing...";

std::atomic<int> g_current_request_id{ 0 };

struct HeaderUserData {
    int retry_after = -1;
};

size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t numbytes = size * nitems;
    std::string header(buffer, numbytes);
    size_t pos = header.find("Retry-After:");
    if (pos == std::string::npos) pos = header.find("retry-after:");
    if (pos != std::string::npos) {
        std::string val = header.substr(pos + 12);
        std::string digits = "";
        for (char c : val) {
            if (std::isdigit(static_cast<unsigned char>(c))) digits += c;
            else if (!digits.empty()) break;
        }
        if (!digits.empty()) {
            HeaderUserData* hd = static_cast<HeaderUserData*>(userdata);
            if (hd) hd->retry_after = std::stoi(digits);
        }
    }
    return numbytes;
}

void InitPaths() {
    wchar_t* appDataPath = nullptr;
    size_t len = 0;
    if (_wdupenv_s(&appDataPath, &len, L"APPDATA") == 0 && appDataPath != nullptr) {
        std::wstring baseDir = std::wstring(appDataPath) + L"\\Spotify_Overlay";
        CreateDirectoryW(baseDir.c_str(), NULL);
        g_config_file = baseDir + L"\\config.json";
        g_token_file = baseDir + L"\\spotify_token.txt";
        g_lyrics_dir = baseDir + L"\\lyrics";
        g_last_state_file = baseDir + L"\\last_state.json";
        g_album_art_file = baseDir + L"\\album_art.png";

        CreateDirectoryW(g_lyrics_dir.c_str(), NULL);
        free(appDataPath);
    }
    else {
        g_config_file = L"config.json";
        g_token_file = L"spotify_token.txt";
        g_lyrics_dir = L"lyrics";
        g_album_art_file = L"album_art.png";
        CreateDirectoryW(g_lyrics_dir.c_str(), NULL);
    }
}

void LoadConfig() {
    std::ifstream ifs(g_config_file);
    if (ifs) {
        try {
            json j; ifs >> j;
            CLIENT_ID = j.value("client_id", "");
            CLIENT_SECRET = j.value("client_secret", "");
            g_is_3d_lyrics = j.value("is_3d_lyrics", false);
            g_show_lyrics = j.value("show_lyrics", true);
            g_use_accent_color = j.value("use_accent_color", true);

            if (j.contains("api_mode")) g_api_mode = j.value("api_mode", 1);
            else g_api_mode = j.value("use_api", true) ? 1 : 0;

            g_lyrics_glow_mode = j.value("lyrics_glow_mode", 0);
            g_enable_blur = j.value("enable_blur", false);
            g_blur_strength = j.value("blur_strength", 0.5f);
            g_color_trans_mode = j.value("color_trans_mode", 0);
            g_color_trans_rotation = j.value("color_trans_rotation", 45.0f);

            g_saved_pos_x = j.value("pos_x", -9999);
            g_saved_pos_y = j.value("pos_y", -9999);
            g_lyrics_pos_x = j.value("lyrics_pos_x", -9999);
            g_lyrics_pos_y = j.value("lyrics_pos_y", -9999);
            g_bg_alpha = j.value("bg_alpha", 0.9f);
            g_album_alpha = j.value("album_alpha", 1.0f);
            g_uiStyle = j.value("uistyle", 1);
            g_hide_on_pause = j.value("hide_on_pause", false);
        }
        catch (...) {}
    }
}

void SaveConfig() {
    std::ifstream ifs(g_config_file);
    json j;
    if (ifs) { try { ifs >> j; } catch (...) {} }

    j["client_id"] = CLIENT_ID;
    j["client_secret"] = CLIENT_SECRET;
    j["is_3d_lyrics"] = g_is_3d_lyrics;
    j["show_lyrics"] = g_show_lyrics;
    j["use_accent_color"] = g_use_accent_color;
    j["api_mode"] = g_api_mode;

    j["lyrics_glow_mode"] = g_lyrics_glow_mode;
    j["enable_blur"] = g_enable_blur;
    j["blur_strength"] = g_blur_strength;
    j["color_trans_mode"] = g_color_trans_mode;
    j["color_trans_rotation"] = g_color_trans_rotation;

    j["pos_x"] = (int)g_pos_x;
    j["pos_y"] = (int)g_pos_y;
    j["lyrics_pos_x"] = g_lyrics_pos_x;
    j["lyrics_pos_y"] = g_lyrics_pos_y;
    j["uistyle"] = (int)g_uiStyle;
    j["bg_alpha"] = g_bg_alpha;
    j["album_alpha"] = g_album_alpha;
    j["hide_on_pause"] = g_hide_on_pause;

    std::ofstream ofs(g_config_file);
    ofs << j.dump(4);
}

struct SpotifyTrackInfo {
    std::string track_name = "";
    std::string artist_name = "";
    std::vector<std::string> artist_list;
    std::string image_url = "";
    std::string timestamp = "";
    bool is_spotify_api = false;
};

struct SpotifyOverlayState {
    std::chrono::steady_clock::time_point last_update;
    std::mutex mtx;
    SpotifyTrackInfo current_track;
    int progress_ms = 0;
    int duration_ms = 0;
    bool is_playing = false;
    bool track_changed = false;
    std::vector<SpotifyTrackInfo> track_history;
};

static SpotifyOverlayState g_spotify_overlay_state;

struct LrcLine { int time_ms; std::wstring text; };
struct LyricsData { bool has_lyrics = false; std::vector<LrcLine> lines; };

LyricsData g_current_lyrics;
std::mutex g_lyrics_mtx;
bool g_needs_lyrics_fetch = false;
bool g_is_fetching_lyrics = false;

void ParseLrc(const std::string& lrcStr, LyricsData& lyricsData, const std::string& trackName = "");

struct ColorInfo { BYTE r, g, b; float brightness; };
std::vector<ColorInfo> g_color_history;
std::mutex g_color_mtx;

class SmoothColor {
public:
    float r = 0, g = 0, b = 0;
    float tr = 0, tg = 0, tb = 0;
    float or_ = 0, og_ = 0, ob_ = 0;
    float speed = 0.04f;

    void setImmediate(BYTE R, BYTE G, BYTE B) {
        r = tr = or_ = R; g = tg = og_ = G; b = tb = ob_ = B;
    }

    void snapshot() {
        or_ = r; og_ = g; ob_ = b;
    }

    bool willChange(BYTE R, BYTE G, BYTE B) const {
        return (std::abs(tr - R) > 0.5f || std::abs(tg - G) > 0.5f || std::abs(tb - B) > 0.5f);
    }
    void setTarget(BYTE R, BYTE G, BYTE B) {
        tr = R; tg = G; tb = B;
    }
    void update(float dt = 0.0166f) {
        float factor = 1.0f - std::exp(-speed * 60.0f * dt);
        r += (tr - r) * factor;
        g += (tg - g) * factor;
        b += (tb - b) * factor;
    }
    Color getColor(BYTE alpha = 255) const {
        return Color(alpha, (BYTE)std::clamp(r, 0.0f, 255.0f), (BYTE)std::clamp(g, 0.0f, 255.0f), (BYTE)std::clamp(b, 0.0f, 255.0f));
    }
    Color getOldColor(BYTE alpha = 255) const {
        return Color(alpha, (BYTE)std::clamp(or_, 0.0f, 255.0f), (BYTE)std::clamp(og_, 0.0f, 255.0f), (BYTE)std::clamp(ob_, 0.0f, 255.0f));
    }
    Color getTargetColor(BYTE alpha = 255) const {
        return Color(alpha, (BYTE)std::clamp(tr, 0.0f, 255.0f), (BYTE)std::clamp(tg, 0.0f, 255.0f), (BYTE)std::clamp(tb, 0.0f, 255.0f));
    }
};

void BoostColorDensity(BYTE& r, BYTE& g, BYTE& b, float depthFactor) {
    int maxVal = std::max({ r, g, b }); int minVal = std::min({ r, g, b });
    int chroma = maxVal - minVal;
    if (chroma < 10) {
        r = static_cast<BYTE>(maxVal * 0.2f); g = static_cast<BYTE>(maxVal * 0.3f); b = static_cast<BYTE>(maxVal * 0.5f);
        return;
    }
    int avg = (r + g + b) / 3;
    float expandFactor = 2.5f;
    int nr = avg + static_cast<int>((r - avg) * expandFactor);
    int ng = avg + static_cast<int>((g - avg) * expandFactor);
    int nb = avg + static_cast<int>((b - avg) * expandFactor);
    r = static_cast<BYTE>(std::clamp(nr, 0, 255) * depthFactor);
    g = static_cast<BYTE>(std::clamp(ng, 0, 255) * depthFactor);
    b = static_cast<BYTE>(std::clamp(nb, 0, 255) * depthFactor);
}

void ExtractAccentColor(Bitmap* bmp, BYTE& outR, BYTE& outG, BYTE& outB, float& outBrightness) {
    if (!bmp) { outR = 29; outG = 185; outB = 84; outBrightness = 0.3f; return; }
    int w = bmp->GetWidth(); int h = bmp->GetHeight();

    struct ColorBucket { int count = 0; unsigned long long sumR = 0; unsigned long long sumG = 0; unsigned long long sumB = 0; int maxVal = 0; int maxChroma = 0; };
    std::vector<ColorBucket> buckets(32768);

    unsigned long long allSumR = 0, allSumG = 0, allSumB = 0;
    int allCount = 0; int vibrantCount = 0;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Color c;
            if (bmp->GetPixel(x, y, &c) == Ok) {
                BYTE r = c.GetR(); BYTE g = c.GetG(); BYTE b = c.GetB();
                allSumR += r; allSumG += g; allSumB += b; allCount++;

                int maxVal = std::max({ r, g, b }); int minVal = std::min({ r, g, b });
                int chroma = maxVal - minVal;

                if (chroma >= 45) {
                    vibrantCount++;
                    int qr = r >> 3; int qg = g >> 3; int qb = b >> 3;
                    int key = (qr << 10) | (qg << 5) | qb;
                    buckets[key].count++; buckets[key].sumR += r; buckets[key].sumG += g; buckets[key].sumB += b;
                    if (maxVal > buckets[key].maxVal) buckets[key].maxVal = maxVal;
                    if (chroma > buckets[key].maxChroma) buckets[key].maxChroma = chroma;
                }
            }
        }
    }

    float avgR = allCount > 0 ? static_cast<float>(allSumR) / allCount : 0.0f;
    float avgG = allCount > 0 ? static_cast<float>(allSumG) / allCount : 0.0f;
    float avgB = allCount > 0 ? static_cast<float>(allSumB) / allCount : 0.0f;
    float avgBrightness = (0.299f * avgR + 0.587f * avgG + 0.114f * avgB) / 255.0f;

    if (g_use_accent_color && (vibrantCount > (allCount * 0.005))) {
        int bestKey = -1; double maxScore = -1.0;
        for (int i = 0; i < 32768; ++i) {
            if (buckets[i].count > 0) {
                double score = static_cast<double>(buckets[i].count) * static_cast<double>(buckets[i].maxChroma);
                if (score > maxScore) { maxScore = score; bestKey = i; }
            }
        }
        if (bestKey != -1) {
            BYTE origR = static_cast<BYTE>(buckets[bestKey].sumR / buckets[bestKey].count);
            BYTE origG = static_cast<BYTE>(buckets[bestKey].sumG / buckets[bestKey].count);
            BYTE origB = static_cast<BYTE>(buckets[bestKey].sumB / buckets[bestKey].count);
            outBrightness = (0.299f * origR + 0.587f * origG + 0.114f * origB) / 255.0f;

            if (outBrightness > 0.85f) { outR = 29; outG = 150; outB = 70; outBrightness = 0.6f; return; }
            else if (outBrightness < 0.15f) { outR = 0; outG = 140; outB = 255; outBrightness = 0.3f; return; }

            outR = origR; outG = origG; outB = origB;
            BoostColorDensity(outR, outG, outB, 0.85f); return;
        }
    }

    if (avgBrightness < 0.5f) { outR = 29; outG = 185; outB = 84; outBrightness = 0.3f; }
    else { outR = 0; outG = 100; outB = 220; outBrightness = 0.7f; }
}

void SaveTrackToHistory(const SpotifyTrackInfo& track) {
    if (track.track_name.empty()) return;
    std::wstring filePath = g_config_file;
    wchar_t* appDataPath = nullptr; size_t len = 0;
    if (_wdupenv_s(&appDataPath, &len, L"APPDATA") == 0 && appDataPath != nullptr) {
        std::wstring dirPath = std::wstring(appDataPath) + L"\\Spotify_Overlay";
        filePath = dirPath + L"\\history.txt";
        free(appDataPath);
    }
    std::ofstream ofs(filePath, std::ios::app);
    if (ofs.is_open()) {
        ofs << "Track: " << track.track_name << " | Artist: " << track.artist_name << " | Time: " << track.timestamp << "\n";
        ofs.close();
    }
}

void AddRoundRect(GraphicsPath& path, float x, float y, float w, float h, float radius) {
    float d = radius * 2.0f;
    path.AddArc(x, y, d, d, 180, 90); path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90); path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

class RollingDigitAnimation {
public:
    float current = 0.0f; float target = 0.0f; float speed = 0.06f;
    void setTarget(float t) { target = t; }
    void setImmediate(float t) { current = target = t; }
    void update(float dt = 0.0166f) {
        float factor = 1.0f - std::exp(-speed * 60.0f * dt);
        current += (std::abs(target - current) > 10.0f) ? (target - current) : (target - current) * factor;
    }
};

class RollingTimeRenderer {
public:
    RollingDigitAnimation m1, m2, s1, s2; int last_total_seconds = -1;
    void update(int total_seconds, float dt = 0.0166f) {
        int cur_m1 = total_seconds / 600; int cur_m2 = (total_seconds / 60) % 10;
        int cur_s1 = (total_seconds % 60) / 10; int cur_s2 = total_seconds % 10;
        if (last_total_seconds == -1) {
            m1.setImmediate((float)cur_m1); m2.setImmediate((float)cur_m2);
            s1.setImmediate((float)cur_s1); s2.setImmediate((float)cur_s2);
        }
        else {
            m1.setTarget((float)cur_m1); m2.setTarget((float)cur_m2);
            s1.setTarget((float)cur_s1); s2.setTarget((float)cur_s2);
        }
        last_total_seconds = total_seconds;
        m1.update(dt); m2.update(dt); s1.update(dt); s2.update(dt);
    }
};

class RollingTextRenderer {
public:
    std::wstring currentText = L""; std::wstring targetText = L""; Animation rollAnim;
    RollingTextRenderer() { rollAnim.speed = 0.06f; rollAnim.setImmediate(1.0f); }
    void setText(const std::wstring& newText) {
        if (targetText != newText) {
            if (targetText.empty()) { currentText = targetText = newText; rollAnim.setImmediate(1.0f); }
            else { currentText = targetText; targetText = newText; rollAnim.setImmediate(0.0f); rollAnim.set(1.0f); }
        }
    }
    void update(float dt = 0.0166f) { rollAnim.update(dt); }
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct MemoryBuffer { std::vector<char> data; };
size_t ImageWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    MemoryBuffer* mem = (MemoryBuffer*)userp;
    mem->data.insert(mem->data.end(), (char*)contents, (char*)contents + total_size);
    return total_size;
}

std::vector<char> DownloadImageBytes(const std::string& url) {
    std::vector<char> buffer;
    if (url.empty()) return buffer;

    for (int retry = 0; retry < 3; ++retry) {
        CURL* curl = curl_easy_init();
        if (curl) {
            MemoryBuffer mem;
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ImageWriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode res = curl_easy_perform(curl);
            long response_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK && response_code == 200 && !mem.data.empty()) {
                buffer = std::move(mem.data);
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300 * (retry + 1)));
    }
    return buffer;
}
void ParseLrc(const std::string& lrcStr, LyricsData& lyricsData, const std::string& trackName) {
    lyricsData.lines.clear(); lyricsData.has_lyrics = false;
    std::istringstream iss(lrcStr); std::string line;
    std::wstring wTrackName = Utf8ToWide(trackName);
    std::transform(wTrackName.begin(), wTrackName.end(), wTrackName.begin(), ::towlower);
    bool in_metadata_header = true;

    std::vector<LrcLine> rawLines;

    while (std::getline(iss, line)) {
        if (line.size() >= 10 && line[0] == '[') {
            size_t endBracket = line.find(']');
            if (endBracket != std::string::npos && endBracket >= 9) {
                std::string timeStr = line.substr(1, endBracket - 1);
                std::string textStr = line.substr(endBracket + 1);
                int min = 0; float sec = 0.0f;
                if (sscanf(timeStr.c_str(), "%d:%f", &min, &sec) == 2) {
                    std::wstring wText = Utf8ToWide(textStr);
                    while (!wText.empty() && (wText.front() == L' ' || wText.front() == L'\t')) wText.erase(wText.begin());
                    while (!wText.empty() && (wText.back() == L' ' || wText.back() == L'\t' || wText.back() == L'\r' || wText.back() == L'\n')) wText.pop_back();
                    if (wText.empty()) continue;

                    if (in_metadata_header) {
                        std::wstring wTextLower = wText;
                        std::transform(wTextLower.begin(), wTextLower.end(), wTextLower.begin(), ::towlower);
                        bool is_metadata = false;
                        std::vector<std::wstring> keywords = { L"作词", L"作詞", L"作曲", L"编曲", L"編曲", L"填词", L"谱曲", L"演唱", L"歌手", L"lyric", L"composer", L"arranger", L"vocal", L"prod", L"by:" };
                        for (const auto& kw : keywords) { if (wTextLower.find(kw) != std::wstring::npos) { is_metadata = true; break; } }
                        if (!is_metadata && !wTrackName.empty() && wTextLower.find(wTrackName) != std::wstring::npos) is_metadata = true;
                        if (!is_metadata && (wTextLower.find(L":") != std::wstring::npos || wTextLower.find(L"：") != std::wstring::npos || wTextLower.find(L"|") != std::wstring::npos)) is_metadata = true;

                        if (is_metadata) continue;
                        else in_metadata_header = false;
                    }

                    LrcLine lrcLine;
                    lrcLine.time_ms = min * 60000 + static_cast<int>(sec * 1000);
                    lrcLine.text = wText;
                    rawLines.push_back(lrcLine);
                }
            }
        }
    }

    if (rawLines.empty()) return;

    std::sort(rawLines.begin(), rawLines.end(), [](const LrcLine& a, const LrcLine& b) {
        return a.time_ms < b.time_ms;
        });

    if (rawLines.front().time_ms >= 2000) {
        lyricsData.lines.push_back({ 0, L"前奏" });
    }

    for (size_t i = 0; i < rawLines.size(); ++i) {
        lyricsData.lines.push_back(rawLines[i]);
    }

    if (!lyricsData.lines.empty()) lyricsData.has_lyrics = true;
}

int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    int myRequestId = *static_cast<int*>(clientp);
    if (myRequestId != g_current_request_id.load()) return 1;
    return 0;
}

void FetchLyrics(const std::string& trackName, const std::string& allArtists, const std::vector<std::string>& artistList, int requestId, bool isSpotifyApi) {
    std::vector<std::string> searchArtists;
    if (!artistList.empty()) searchArtists.push_back(artistList[0]);
    if (std::find(searchArtists.begin(), searchArtists.end(), allArtists) == searchArtists.end()) searchArtists.push_back(allArtists);
    for (size_t i = 1; i < artistList.size(); ++i) {
        if (std::find(searchArtists.begin(), searchArtists.end(), artistList[i]) == searchArtists.end()) searchArtists.push_back(artistList[i]);
    }

    bool lyricsFound = false; bool explicitNotFound = false; bool hasNetworkError = false; LyricsData finalLyricsData;

    std::string apiTag = isSpotifyApi ? "[by: Spotify API]\r\n" : "[by: No Use API (GSMTC)]\r\n";

    for (const auto& artist : searchArtists) {
        if (requestId != g_current_request_id.load()) return;
        int retryCount = 0; int notFoundCount = 0;

        while (retryCount < 3) {
            if (requestId != g_current_request_id.load()) return;

            std::string url = "https://lrclib.net/api/get?track_name=" + UrlEncode(trackName) + "&artist_name=" + UrlEncode(artist);
            CURL* curl = curl_easy_init(); std::string response_string; HeaderUserData header_data;
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
                curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "SpotifyOverlayLyricsFetcher/1.0");
                curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback); curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &requestId); curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback); curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_data);

                CURLcode res = curl_easy_perform(curl);
                long response_code = 0; curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                curl_easy_cleanup(curl);

                if (requestId != g_current_request_id.load()) return;

                if (response_code == 429) {
                    int wait_sec = (header_data.retry_after > 0) ? header_data.retry_after : 2;
                    std::this_thread::sleep_for(std::chrono::seconds(wait_sec) + std::chrono::milliseconds(300));
                    retryCount++; continue;
                }

                if (res == CURLE_OK && response_code == 200) {
                    try {
                        auto js = json::parse(response_string);
                        if (js.contains("syncedLyrics") && !js["syncedLyrics"].is_null()) {
                            ParseLrc(js["syncedLyrics"].get<std::string>(), finalLyricsData, trackName);
                            if (finalLyricsData.has_lyrics) {
                                std::wstring safeArtist = SanitizeFileName(Utf8ToWide(allArtists)); std::wstring safeTrack = SanitizeFileName(Utf8ToWide(trackName));
                                std::wstring filePath = g_lyrics_dir + L"\\" + safeArtist + L" - " + safeTrack + L".lrc";
                                std::wstring noApiFilePath = g_lyrics_dir + L"\\NoAPI_" + safeArtist + L" - " + safeTrack + L".lrc";
                                if (GetFileAttributesW(noApiFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) filePath = noApiFilePath;

                                std::ofstream ofs(filePath, std::ios::binary);
                                if (ofs.is_open()) {
                                    std::string lrcText = apiTag + js["syncedLyrics"].get<std::string>();
                                    ofs.write(lrcText.c_str(), lrcText.size()); ofs.close();
                                }
                                lyricsFound = true;
                            }
                        }
                    }
                    catch (...) {}
                    break;
                }
                else if (response_code == 404) {
                    notFoundCount++;
                    if (notFoundCount < 2) { std::this_thread::sleep_for(std::chrono::milliseconds(300)); retryCount++; continue; }
                    explicitNotFound = true; break;
                }
                else if (res == CURLE_ABORTED_BY_CALLBACK) return;
                else {
                    hasNetworkError = true;
                    break;
                }
            }
            else break;
        }
        if (lyricsFound) break;
    }

    if (!lyricsFound) {
        if (requestId != g_current_request_id.load()) return;
        std::string mainArtist = artistList.empty() ? "" : artistList[0];
        std::string neteaseSearchUrl = "https://music.163.com/api/search/get/web?s=" + UrlEncode(trackName + " " + mainArtist) + "&type=1";

        CURL* curl = curl_easy_init(); std::string search_response;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_URL, neteaseSearchUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curl, CURLOPT_WRITEDATA, &search_response);
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"); curl_easy_setopt(curl, CURLOPT_REFERER, "https://music.163.com/");

            CURLcode res = curl_easy_perform(curl);
            long response_code = 0; curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl);

            if (requestId != g_current_request_id.load()) return;

            if (res == CURLE_OK && response_code == 200) {
                try {
                    auto js = json::parse(search_response); std::vector<std::string> songIds;
                    if (js.contains("trp") && js["trp"].contains("rules") && js["trp"]["rules"].is_array()) {
                        std::regex re(R"(::(\d+)::)");
                        for (const auto& rule : js["trp"]["rules"]) {
                            std::string ruleStr = rule.get<std::string>(); std::smatch match;
                            if (std::regex_search(ruleStr, match, re) && match.size() > 1) songIds.push_back(match[1].str());
                        }
                    }
                    auto StrToLower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); }); return s; };
                    std::string targetArtistLower = StrToLower(mainArtist); std::string targetTrackLower = StrToLower(trackName);
                    std::string finalLrcText = ""; LyricsData finalLrcData; bool foundExactMatch = false;

                    for (size_t idx = 0; idx < songIds.size(); ++idx) {
                        std::string songId = songIds[idx]; if (requestId != g_current_request_id.load()) return;
                        std::string detailUrl = "https://music.163.com/api/song/detail/?id=" + songId + "&ids=[" + songId + "]";
                        CURL* curlDetail = curl_easy_init(); std::string detail_response; bool titleVerified = false; std::string actualSongName = "";

                        if (curlDetail) {

                            curl_easy_setopt(curlDetail, CURLOPT_NOSIGNAL, 1L);
                            curl_easy_setopt(curlDetail, CURLOPT_TIMEOUT, 20L);
                            curl_easy_setopt(curlDetail, CURLOPT_CONNECTTIMEOUT, 10L);
                            curl_easy_setopt(curlDetail, CURLOPT_URL, detailUrl.c_str()); curl_easy_setopt(curlDetail, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curlDetail, CURLOPT_WRITEDATA, &detail_response);
                            curl_easy_setopt(curlDetail, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); curl_easy_setopt(curlDetail, CURLOPT_SSL_VERIFYPEER, 0L); curl_easy_setopt(curlDetail, CURLOPT_SSL_VERIFYHOST, 0L);
                            curl_easy_setopt(curlDetail, CURLOPT_USERAGENT, "Mozilla/5.0");
                            CURLcode resDetail = curl_easy_perform(curlDetail); curl_easy_cleanup(curlDetail);
                            if (resDetail == CURLE_OK) {
                                try {
                                    auto detailJs = json::parse(detail_response);
                                    if (detailJs.contains("songs") && detailJs["songs"].is_array() && !detailJs["songs"].empty()) {
                                        actualSongName = detailJs["songs"][0].contains("name") ? detailJs["songs"][0]["name"].get<std::string>() : "";
                                        std::string actualLower = StrToLower(actualSongName);
                                        if (actualLower.find(targetTrackLower) != std::string::npos || targetTrackLower.find(actualLower) != std::string::npos) titleVerified = true;
                                    }
                                }
                                catch (...) {}
                            }
                        }

                        if (!titleVerified) continue;

                        std::string lyricUrl = "https://music.163.com/api/song/lyric?id=" + songId + "&lv=1&kv=1&tv=-1";
                        CURL* curlLrc = curl_easy_init(); std::string lyric_response;
                        if (curlLrc) {
                            curl_easy_setopt(curlLrc, CURLOPT_NOSIGNAL, 1L);
                            curl_easy_setopt(curlLrc, CURLOPT_TIMEOUT, 20L);
                            curl_easy_setopt(curlLrc, CURLOPT_CONNECTTIMEOUT, 10L);
                            curl_easy_setopt(curlLrc, CURLOPT_URL, lyricUrl.c_str()); curl_easy_setopt(curlLrc, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curlLrc, CURLOPT_WRITEDATA, &lyric_response);
                            curl_easy_setopt(curlLrc, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); curl_easy_setopt(curlLrc, CURLOPT_SSL_VERIFYPEER, 0L); curl_easy_setopt(curlLrc, CURLOPT_SSL_VERIFYHOST, 0L);
                            curl_easy_setopt(curlLrc, CURLOPT_USERAGENT, "Mozilla/5.0"); curl_easy_setopt(curlLrc, CURLOPT_REFERER, "https://music.163.com/");
                            CURLcode resLrc = curl_easy_perform(curlLrc); long lrc_code = 0; curl_easy_getinfo(curlLrc, CURLINFO_RESPONSE_CODE, &lrc_code);
                            curl_easy_cleanup(curlLrc);
                            if (requestId != g_current_request_id.load()) return;

                            if (resLrc == CURLE_OK && lrc_code == 200) {
                                auto lrcJs = json::parse(lyric_response);
                                if (lrcJs.contains("lrc") && lrcJs["lrc"].contains("lyric") && !lrcJs["lrc"]["lyric"].is_null()) {
                                    std::string lrcText = lrcJs["lrc"]["lyric"].get<std::string>();
                                    if (!lrcText.empty()) {
                                        std::wstring wLrcText = Utf8ToWide(lrcText); std::wstring wTargetArtist = Utf8ToWide(targetArtistLower);
                                        std::transform(wLrcText.begin(), wLrcText.end(), wLrcText.begin(), ::towlower);
                                        std::transform(wTargetArtist.begin(), wTargetArtist.end(), wTargetArtist.begin(), ::towlower);

                                        if (wLrcText.find(L"纯音乐") != std::wstring::npos || wLrcText.find(L"请欣赏") != std::wstring::npos || wLrcText.find(L"instrumental") != std::wstring::npos) continue;

                                        std::wistringstream wiss(wLrcText); std::wstring wLine; size_t actual_lyrics_lines = 0; bool artist_found_in_credits = false;
                                        while (std::getline(wiss, wLine)) {
                                            size_t endBracket = wLine.find(L']'); std::wstring content = (endBracket != std::wstring::npos) ? wLine.substr(endBracket + 1) : wLine;
                                            while (!content.empty() && (content.front() == L' ' || content.front() == L'\t' || content.front() == L'\r' || content.front() == L'\n')) content.erase(content.begin());
                                            while (!content.empty() && (content.back() == L' ' || content.back() == L'\t' || content.back() == L'\r' || content.back() == L'\n')) content.pop_back();
                                            if (content.empty()) continue;

                                            bool is_credit_line = false;
                                            std::vector<std::wstring> credit_keywords = { L"作词", L"作詞", L"作曲", L"编曲", L"編曲", L"填词", L"谱曲", L"演唱", L"歌手", L"vocal", L"lyric", L"composer", L"arranger" };
                                            for (const auto& kw : credit_keywords) { if (content.find(kw) != std::wstring::npos) { is_credit_line = true; break; } }
                                            if (is_credit_line) { if (!wTargetArtist.empty() && content.find(wTargetArtist) != std::wstring::npos) artist_found_in_credits = true; }
                                            else actual_lyrics_lines++;
                                        }

                                        if (actual_lyrics_lines <= 3) continue;
                                        bool artistMatches = false;
                                        if (targetArtistLower.empty() || targetArtistLower == "unknown") artistMatches = true;
                                        else if (artist_found_in_credits) artistMatches = true;

                                        if (artistMatches) {
                                            LyricsData localData; ParseLrc(lrcText, localData, trackName);
                                            finalLrcText = lrcText; finalLrcData = localData; foundExactMatch = true; break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (foundExactMatch && !finalLrcText.empty()) {
                        std::wstring safeArtist = SanitizeFileName(Utf8ToWide(allArtists)); std::wstring safeTrack = SanitizeFileName(Utf8ToWide(trackName));
                        std::wstring filePath = g_lyrics_dir + L"\\" + safeArtist + L" - " + safeTrack + L".lrc";
                        std::wstring noApiFilePath = g_lyrics_dir + L"\\NoAPI_" + safeArtist + L" - " + safeTrack + L".lrc";
                        if (GetFileAttributesW(noApiFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) filePath = noApiFilePath;
                        std::ofstream ofs(filePath, std::ios::binary);
                        if (ofs.is_open()) {
                            std::string lrcTextWithApi = apiTag + finalLrcText;
                            ofs.write(lrcTextWithApi.c_str(), lrcTextWithApi.size()); ofs.close();
                        }
                        finalLyricsData = finalLrcData; lyricsFound = true;
                    }
                }
                catch (...) {}
            }
            else if (res == CURLE_ABORTED_BY_CALLBACK) {
                return;
            }
            else {

                hasNetworkError = true;
            }
        }
    }

    if (!lyricsFound && !hasNetworkError) {
        if (requestId == g_current_request_id.load()) {
            std::wstring unknownFilePath = g_lyrics_dir + L"\\unknowns.txt";
            std::string apiStr = isSpotifyApi ? "API: Yes" : "API: No";
            std::string targetLine = "Track: " + trackName + " | Artist: " + allArtists + " | " + apiStr;
            std::string searchLine = "Track: " + trackName + " | Artist: " + allArtists;
            bool alreadyExists = false;
            std::ifstream ifs(unknownFilePath);
            if (ifs.is_open()) {
                std::string line;
                while (std::getline(ifs, line)) { if (line.find(searchLine) != std::string::npos) { alreadyExists = true; break; } }
                ifs.close();
            }
            if (!alreadyExists) {
                std::ofstream ofs(unknownFilePath, std::ios::app);
                if (ofs.is_open()) { ofs << targetLine << "\n"; ofs.close(); }
            }
        }
    }

    std::lock_guard<std::mutex> lock(g_lyrics_mtx);
    if (requestId == g_current_request_id.load()) {
        if (lyricsFound && finalLyricsData.has_lyrics) {
            g_current_lyrics = finalLyricsData;
        }
        else {

            g_current_lyrics.has_lyrics = false;
            g_current_lyrics.lines.clear();
        }
        g_is_fetching_lyrics = false;
    }
    LyricsManager::Refresh();
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now(); auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss; ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S"); return ss.str();
}

void SaveRefreshToken(const std::string& token) { std::ofstream ofs(g_token_file); if (ofs) ofs << token; }
void LoadRefreshToken() { std::ifstream ifs(g_token_file); if (ifs) std::getline(ifs, g_refresh_token); }

bool AuthFlow() {
    g_debug_status = L"AuthFlow started...";
    WSADATA wsaData; WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr = { 0 }; serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); serverAddr.sin_port = htons(8888);
    bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)); listen(listenSocket, 1);

    std::string host_accounts = "accounts.spotify.com";
    std::string authUrl = "https://" + host_accounts + "/authorize?client_id=" + CLIENT_ID + "&response_type=code&redirect_uri=" + REDIRECT_URI + "&scope=user-read-currently-playing%20user-read-playback-state%20user-read-recently-played";
    ShellExecuteA(NULL, "open", authUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);

    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    char buffer[4096] = { 0 }; recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    std::string request(buffer); std::string code = ""; size_t codePos = request.find("code=");
    if (codePos != std::string::npos) {
        size_t endPos = request.find(' ', codePos); size_t ampPos = request.find('&', codePos);
        endPos = (std::min)(endPos, ampPos);
        if (endPos != std::string::npos) code = request.substr(codePos + 5, endPos - (codePos + 5));
    }

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body style='font-family:sans-serif; text-align:center; margin-top:50px;'><h2>Spotify Authentication Completed!</h2><p>You can close this window now.</p></body></html>";
    send(clientSocket, response.c_str(), (int)response.length(), 0);
    closesocket(clientSocket); closesocket(listenSocket); WSACleanup();

    if (code.empty()) { g_debug_status = L"[ERR] Auth code empty"; return false; }

    CURL* curl = curl_easy_init(); std::string response_string;
    if (curl) {
        std::string token_url = "https://" + host_accounts + "/api/token";
        curl_easy_setopt(curl, CURLOPT_URL, token_url.c_str());
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Basic " + Base64Encode(CLIENT_ID + ":" + CLIENT_SECRET);
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string post_data = "grant_type=authorization_code&code=" + code + "&redirect_uri=" + REDIRECT_URI;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers); curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            try {
                auto js = json::parse(response_string);
                if (js.contains("error") && js["error"] == "invalid_client") {
                    SaveRefreshToken(""); g_error_message = "Invalid Client ID or Secret!";
                    g_app_state = STATE_INPUT_ID; CLIENT_ID = ""; CLIENT_SECRET = ""; SaveConfig(); return false;
                }
                if (js.contains("refresh_token")) {
                    g_refresh_token = js["refresh_token"].get<std::string>();
                    SaveRefreshToken(g_refresh_token); g_debug_status = L"Auth success, token saved."; g_error_message = ""; return true;
                }
            }
            catch (...) {}
        }
    }
    g_debug_status = L"[ERR] Auth token exchange failed"; return false;
}

std::string getAccessToken() {
    if (g_refresh_token.empty()) return "";
    CURL* curl = curl_easy_init(); std::string response_string;
    if (curl) {
        std::string host_accounts = "accounts.spotify.com";
        std::string token_url = "https://" + host_accounts + "/api/token";
        curl_easy_setopt(curl, CURLOPT_URL, token_url.c_str());
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Basic " + Base64Encode(CLIENT_ID + ":" + CLIENT_SECRET);
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string post_data = "grant_type=refresh_token&refresh_token=" + g_refresh_token;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers); curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            try {
                auto js = json::parse(response_string);
                if (js.contains("error") && js["error"] == "invalid_client") {
                    g_error_message = "Invalid Client ID or Secret!";
                    g_app_state = STATE_INPUT_ID; CLIENT_ID = ""; CLIENT_SECRET = ""; SaveConfig(); return "";
                }
                if (js.contains("refresh_token")) { g_refresh_token = js["refresh_token"].get<std::string>(); SaveRefreshToken(g_refresh_token); }
                if (js.contains("access_token")) return js["access_token"].get<std::string>();
            }
            catch (...) {}
        }
    }
    return "";
}

bool updateSpotifyImageOnly(const std::string& access_token) {
    if (access_token.empty()) return false;
    CURL* curl = curl_easy_init(); std::string response_string; bool success = false;
    if (curl) {
        std::string play_url = "https://api.spotify.com/v1/me/player/currently-playing";
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, play_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        long response_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (res == CURLE_OK && response_code == 200) {
            try {
                auto js = json::parse(response_string);
                if (js.contains("item") && !js["item"].is_null()) {
                    auto item_obj = js["item"];
                    std::string image_url = "";
                    if (item_obj.contains("album") && item_obj["album"].is_object() && item_obj["album"].contains("images") && item_obj["album"]["images"].is_array() && !item_obj["album"]["images"].empty()) {
                        if (item_obj["album"]["images"][0].contains("url") && item_obj["album"]["images"][0]["url"].is_string()) {
                            image_url = item_obj["album"]["images"][0]["url"].get<std::string>();
                        }
                    }
                    if (!image_url.empty()) {
                        std::vector<char> downloaded_bytes = DownloadImageBytes(image_url);
                        {
                            std::lock_guard<std::mutex> img_lock(g_image_mtx);
                            g_image_bytes = std::move(downloaded_bytes);
                            g_image_updated = true;
                        }
                        success = true;
                    }
                }
            }
            catch (...) {}
        }
        curl_slist_free_all(headers); curl_easy_cleanup(curl);
    }
    return success;
}

void SaveLastState(const std::string& track_name, const std::string& artist_name, int progress_ms, int duration_ms, bool is_playing) {
    try {
        json j;
        j["track_name"] = track_name;
        j["artist_name"] = artist_name;
        j["progress_ms"] = progress_ms;
        j["duration_ms"] = duration_ms;
        j["is_playing"] = false;

        std::ofstream ofs(g_last_state_file);
        if (ofs.is_open()) {
            ofs << j.dump();
        }
    }
    catch (...) {}
}

bool updateGSMTCData(std::string& current_track_id) {
    static auto last_error_time = std::chrono::steady_clock::now() - std::chrono::seconds(10);
    bool in_cooldown = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_error_time).count() < 3000);

    static winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager s_manager = nullptr;

    if (!in_cooldown) {
        try {

            if (!s_manager) {
                auto managerOp = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
                if (managerOp) {
                    s_manager = managerOp.get();
                }
            }

            if (s_manager) {
                auto sessions = s_manager.GetSessions();
                if (sessions) {
                    bool spotifyFound = false;
                    for (auto session : sessions) {
                        try {
                            winrt::hstring appId = session.SourceAppUserModelId();
                            if (std::wstring_view(appId).find(L"Spotify.exe") != std::wstring_view::npos) {
                                spotifyFound = true;

                                auto mediaProps = session.TryGetMediaPropertiesAsync().get();
                                auto timeline = session.GetTimelineProperties();
                                auto playbackInfo = session.GetPlaybackInfo();

                                auto status = playbackInfo.PlaybackStatus();
                                bool is_playing = (status == winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);

                                auto WideToUtf8 = [](const std::wstring& wstr) -> std::string {
                                    if (wstr.empty()) return "";
                                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
                                    std::string strTo(size_needed, 0);
                                    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
                                    return strTo;
                                    };

                                std::string track_name = WideToUtf8(mediaProps.Title().c_str());
                                std::string artist_name = WideToUtf8(mediaProps.Artist().c_str());
                                std::string generated_id = track_name + "_" + artist_name;

                                if (track_name.empty()) continue;

                                auto basePos = timeline.Position();
                                auto lastUpdated = timeline.LastUpdatedTime();
                                auto now = winrt::clock::now();
                                auto actualPos = basePos;
                                if (is_playing && now > lastUpdated) actualPos = basePos + (now - lastUpdated);
                                if (actualPos > timeline.EndTime()) actualPos = timeline.EndTime();

                                auto currentSec = std::chrono::duration_cast<std::chrono::milliseconds>(actualPos).count();
                                auto endSec = std::chrono::duration_cast<std::chrono::milliseconds>(timeline.EndTime()).count();

                                {
                                    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                                    g_spotify_overlay_state.progress_ms = (int)currentSec;
                                    g_spotify_overlay_state.duration_ms = endSec > 0 ? (int)endSec : 100000;
                                    g_spotify_overlay_state.is_playing = is_playing;
                                    g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();

                                    SaveLastState(track_name, artist_name, (int)currentSec, g_spotify_overlay_state.duration_ms, is_playing);

                                    static bool s_gsmtc_needs_thumb = false;

                                    if (generated_id != current_track_id) {
                                        bool is_really_changed = true;

                                        if (g_spotify_overlay_state.current_track.is_spotify_api) {
                                            if (g_spotify_overlay_state.current_track.track_name == track_name) {
                                                if (g_spotify_overlay_state.current_track.artist_name.find(artist_name) != std::string::npos) {
                                                    is_really_changed = false;
                                                }
                                            }
                                        }
                                        else {
                                            if (g_spotify_overlay_state.current_track.track_name == track_name &&
                                                g_spotify_overlay_state.current_track.artist_name == artist_name) {
                                                is_really_changed = false;
                                            }
                                        }

                                        if (is_really_changed) {
                                            current_track_id = generated_id;
                                            g_current_request_id++;

                                            SpotifyTrackInfo new_track;
                                            new_track.track_name = track_name;
                                            new_track.artist_name = artist_name;
                                            new_track.artist_list = { artist_name };
                                            new_track.timestamp = getCurrentTimestamp();
                                            new_track.is_spotify_api = false;

                                            g_spotify_overlay_state.current_track = new_track;
                                            g_spotify_overlay_state.track_changed = true;
                                            s_gsmtc_needs_thumb = true;
                                        }
                                    }

                                    bool is_img_empty = false;
                                    {
                                        std::lock_guard<std::mutex> img_lock(g_image_mtx);
                                        is_img_empty = g_image_bytes.empty();
                                    }
                                    if (s_gsmtc_needs_thumb || is_img_empty) {
                                        auto thumbnail = mediaProps.Thumbnail();
                                        if (thumbnail) {
                                            try {
                                                auto streamRef = thumbnail.OpenReadAsync().get();
                                                auto size = streamRef.Size();
                                                if (size > 0) {
                                                    auto buffer = winrt::Windows::Storage::Streams::Buffer(static_cast<uint32_t>(size));
                                                    auto readOp = streamRef.ReadAsync(buffer, static_cast<uint32_t>(size), winrt::Windows::Storage::Streams::InputStreamOptions::None).get();
                                                    auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(readOp);
                                                    std::vector<char> imgData(reader.UnconsumedBufferLength());
                                                    reader.ReadBytes(winrt::array_view<uint8_t>(reinterpret_cast<uint8_t*>(imgData.data()), imgData.size()));

                                                    if (!imgData.empty()) {
                                                        std::lock_guard<std::mutex> img_lock(g_image_mtx);
                                                        g_image_bytes = std::move(imgData);
                                                        g_image_updated = true;
                                                        s_gsmtc_needs_thumb = false;
                                                    }
                                                }
                                            }
                                            catch (...) {}
                                        }
                                    }
                                }
                                g_debug_status = L"OK";
                                return true;
                            }
                        }
                        catch (const winrt::hresult_error&) {
                            continue;
                        }
                        catch (...) {
                            continue;
                        }
                    }
                    if (spotifyFound) return true;
                }
            }
        }
        catch (...) {
            last_error_time = std::chrono::steady_clock::now();
            s_manager = nullptr;
        }
    }

    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
    g_spotify_overlay_state.is_playing = false;

    g_debug_status = L"OK";

    if (g_spotify_overlay_state.current_track.track_name.empty()) {
        std::ifstream ifs(g_last_state_file);
        if (ifs.is_open()) {
            try {
                json j; ifs >> j;
                g_spotify_overlay_state.progress_ms = j.value("progress_ms", 0);
                g_spotify_overlay_state.duration_ms = j.value("duration_ms", 100000);
                g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();

                std::string t_name = j.value("track_name", "");
                std::string a_name = j.value("artist_name", "");
                if (!t_name.empty()) {
                    SpotifyTrackInfo cached_track;
                    cached_track.track_name = t_name;
                    cached_track.artist_name = a_name;
                    cached_track.artist_list = { a_name };
                    cached_track.timestamp = getCurrentTimestamp();
                    cached_track.is_spotify_api = false;
                    g_spotify_overlay_state.current_track = cached_track;

                    g_spotify_overlay_state.track_changed = true;
                    current_track_id = t_name + "_" + a_name;

                    std::ifstream img_ifs(g_album_art_file, std::ios::binary | std::ios::ate);
                    if (img_ifs.is_open()) {
                        std::streamsize size = img_ifs.tellg();
                        img_ifs.seekg(0, std::ios::beg);
                        std::vector<char> buffer(size);
                        if (img_ifs.read(buffer.data(), size)) {
                            std::lock_guard<std::mutex> img_lock(g_image_mtx);
                            g_image_bytes = std::move(buffer);
                            g_image_updated = true;
                        }
                    }
                }
            }
            catch (...) {}
        }
    }
    return false;
}

bool updateSpotifyData(const std::string& access_token, std::string& current_track_id, int& out_wait_ms) {
    out_wait_ms = 0;
    if (access_token.empty()) { g_debug_status = L"OK"; return false; }

    CURL* curl = curl_easy_init(); std::string response_string; bool success = false; HeaderUserData header_data;
    if (curl) {
        std::string host_api = "api.spotify.com"; std::string play_url = "https://" + host_api + "/v1/me/player/currently-playing";
        struct curl_slist* headers = nullptr; headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, play_url.c_str()); curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback); curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_data);

        long response_code = 0; CURLcode res = curl_easy_perform(curl); curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (res == CURLE_OK) {
            if (response_code == 429) {
                int wait_sec = (header_data.retry_after > 0) ? header_data.retry_after : 5; out_wait_ms = wait_sec * 1000 + 300;
                g_debug_status = L"Rate Limited (Waiting...)";
                curl_slist_free_all(headers); curl_easy_cleanup(curl); return false;
            }

            json js; bool is_recent = false;
            if (response_code == 200) {
                try { auto temp_js = json::parse(response_string); if (temp_js.contains("item") && !temp_js["item"].is_null()) js = temp_js; else response_code = 204; }
                catch (...) { response_code = 204; }
            }
            if (response_code == 204) {
                bool has_local_track = false;
                {
                    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                    has_local_track = !g_spotify_overlay_state.current_track.track_name.empty();
                }

                if (!has_local_track) {
                    response_string.clear(); std::string recent_url = "https://" + host_api + "/v1/me/player/recently-played?limit=1";
                    curl_easy_setopt(curl, CURLOPT_URL, recent_url.c_str());
                    res = curl_easy_perform(curl); curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                    if (res == CURLE_OK && response_code == 200) {
                        try { auto recent_js = json::parse(response_string); if (recent_js.contains("items") && !recent_js["items"].empty()) { js = recent_js["items"][0]; is_recent = true; } }
                        catch (...) {}
                    }
                }
            }

            if (!js.empty() && (is_recent ? (js.contains("track") && !js["track"].is_null()) : (js.contains("item") && !js["item"].is_null()))) {
                try {
                    success = true; g_debug_status = L"OK"; auto item_obj = is_recent ? js["track"] : js["item"];

                    std::string track_id = "";
                    if (item_obj.contains("id") && !item_obj["id"].is_null() && item_obj["id"].is_string()) track_id = item_obj["id"].get<std::string>();

                    std::string track_name = "Unknown Track";
                    if (item_obj.contains("name") && !item_obj["name"].is_null() && item_obj["name"].is_string()) track_name = item_obj["name"].get<std::string>();

                    if (track_id.empty()) track_id = "LOCAL_" + track_name;

                    std::string all_artists = ""; std::vector<std::string> artist_list;
                    if (item_obj.contains("artists") && item_obj["artists"].is_array()) {
                        for (size_t i = 0; i < item_obj["artists"].size(); ++i) {
                            if (item_obj["artists"][i].contains("name") && !item_obj["artists"][i]["name"].is_null() && item_obj["artists"][i]["name"].is_string()) {
                                std::string name = item_obj["artists"][i]["name"].get<std::string>(); artist_list.push_back(name);
                                if (i > 0) all_artists += ", "; all_artists += name;
                            }
                        }
                    }
                    if (all_artists.empty()) all_artists = "Unknown Artist";

                    {
                        std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);

                        int prev_duration_ms = g_spotify_overlay_state.duration_ms;

                        if (g_api_mode == 2 && (all_artists == "Unknown Artist" || all_artists.empty())) {
                            if (!g_spotify_overlay_state.current_track.artist_name.empty() &&
                                g_spotify_overlay_state.current_track.artist_name != "Unknown Artist") {
                                all_artists = g_spotify_overlay_state.current_track.artist_name;
                                artist_list = g_spotify_overlay_state.current_track.artist_list;
                            }
                        }

                        bool override_time = (g_api_mode == 1) ||
                            g_spotify_overlay_state.current_track.track_name.empty() ||
                            (g_spotify_overlay_state.current_track.track_name != track_name);

                        if (override_time) {
                            if (is_recent) {
                                g_spotify_overlay_state.progress_ms = 0;
                                g_spotify_overlay_state.is_playing = false;
                                g_spotify_overlay_state.duration_ms = (item_obj.contains("duration_ms") && item_obj["duration_ms"].is_number()) ? item_obj["duration_ms"].get<int>() : 100000;
                            }
                            else {
                                g_spotify_overlay_state.progress_ms = (js.contains("progress_ms") && js["progress_ms"].is_number()) ? js["progress_ms"].get<int>() : 0;
                                g_spotify_overlay_state.duration_ms = (item_obj.contains("duration_ms") && item_obj["duration_ms"].is_number()) ? item_obj["duration_ms"].get<int>() : 100000;
                                g_spotify_overlay_state.is_playing = (js.contains("is_playing") && js["is_playing"].is_boolean()) ? js["is_playing"].get<bool>() : false;
                            }
                        }

                        g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();

                        std::string image_url = "";
                        if (item_obj.contains("album") && item_obj["album"].is_object() && item_obj["album"].contains("images") && item_obj["album"]["images"].is_array() && !item_obj["album"]["images"].empty()) {
                            if (item_obj["album"]["images"][0].contains("url") && item_obj["album"]["images"][0]["url"].is_string()) {
                                image_url = item_obj["album"]["images"][0]["url"].get<std::string>();
                            }
                        }

                        static std::string s_last_downloaded_img_url = "";
                        bool need_image_download = false;

                        if (track_id != current_track_id) {
                            bool is_really_changed = true;
                            if (g_spotify_overlay_state.current_track.track_name == track_name &&
                                g_spotify_overlay_state.current_track.artist_name == all_artists) {
                                is_really_changed = false;
                            }
                            current_track_id = track_id;
                            need_image_download = true;
                            g_spotify_overlay_state.current_track.is_spotify_api = true;

                            if (is_really_changed) {
                                g_current_request_id++;

                                SpotifyTrackInfo new_track;
                                new_track.track_name = track_name;
                                new_track.artist_name = all_artists;
                                new_track.artist_list = artist_list;
                                new_track.image_url = image_url;
                                new_track.timestamp = getCurrentTimestamp();
                                new_track.is_spotify_api = true;

                                bool preserve_lyrics = false;
                                {
                                    std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                                    if (g_current_lyrics.has_lyrics && !g_spotify_overlay_state.current_track.track_name.empty()) {
                                        std::string old_t = g_spotify_overlay_state.current_track.track_name;
                                        auto toLower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); }); return s; };
                                        std::string lt1 = toLower(old_t), lt2 = toLower(track_name);
                                        bool name_match = (!lt1.empty() && !lt2.empty() && (lt1.find(lt2) != std::string::npos || lt2.find(lt1) != std::string::npos));

                                        int new_dur = g_spotify_overlay_state.duration_ms;
                                        bool dur_match = (prev_duration_ms > 0 && std::abs(prev_duration_ms - new_dur) < 4000);

                                        if (name_match && dur_match) {
                                            preserve_lyrics = true;
                                        }
                                    }
                                }

                                g_spotify_overlay_state.current_track = new_track;
                                g_spotify_overlay_state.track_changed = true;

                                std::wstring safeArtist = SanitizeFileName(Utf8ToWide(all_artists));
                                std::wstring safeTrack = SanitizeFileName(Utf8ToWide(track_name));
                                std::wstring filePath = g_lyrics_dir + L"\\" + safeArtist + L" - " + safeTrack + L".lrc";
                                std::wstring noApiFilePath = g_lyrics_dir + L"\\NoAPI_" + safeArtist + L" - " + safeTrack + L".lrc";
                                if (GetFileAttributesW(noApiFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) filePath = noApiFilePath;

                                std::ifstream ifs(filePath, std::ios::binary);
                                if (ifs.is_open()) {
                                    std::string lrcText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                                    ifs.close();
                                    LyricsData localData;

                                    ParseLrc(lrcText, localData, track_name);
                                    std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                                    g_current_lyrics = localData;
                                    g_is_fetching_lyrics = false;
                                    g_needs_lyrics_fetch = false;
                                }
                                else {
                                    bool is_unknown = false;
                                    std::ifstream unk_ifs(g_lyrics_dir + L"\\unknowns.txt");
                                    if (unk_ifs.is_open()) {
                                        std::string line;
                                        std::string targetLine = "Track: " + track_name + " | Artist: " + all_artists;
                                        while (std::getline(unk_ifs, line)) {
                                            if (line.find(targetLine) != std::string::npos) { is_unknown = true; break; }
                                        }
                                        unk_ifs.close();
                                    }

                                    std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                                    if (!preserve_lyrics) {
                                        g_current_lyrics.has_lyrics = false;
                                        g_current_lyrics.lines.clear();
                                        if (g_show_lyrics && !is_unknown) {
                                            g_is_fetching_lyrics = true;
                                            g_needs_lyrics_fetch = true;
                                        }
                                        else {
                                            g_is_fetching_lyrics = false;
                                            g_needs_lyrics_fetch = false;
                                        }
                                    }
                                    else {
                                        g_is_fetching_lyrics = false;
                                        g_needs_lyrics_fetch = false;
                                    }
                                }
                            }
                        }

                        {
                            std::lock_guard<std::mutex> img_lock(g_image_mtx);
                            if (g_image_bytes.empty()) need_image_download = true;
                        }
                        if ((need_image_download || image_url != s_last_downloaded_img_url) && !image_url.empty()) {
                            std::vector<char> downloaded_bytes = DownloadImageBytes(image_url);
                            if (!downloaded_bytes.empty()) {
                                std::lock_guard<std::mutex> img_lock(g_image_mtx);
                                g_image_bytes = std::move(downloaded_bytes);
                                g_image_updated = true;
                                s_last_downloaded_img_url = image_url;
                            }
                        }
                    }
                }
                catch (...) { success = false; }
            }
        }
        else g_debug_status = L"OK";
        curl_slist_free_all(headers); curl_easy_cleanup(curl);
    }
    return success;
}

void RunMockMode() {
    g_app_state = STATE_RUNNING; g_debug_status = L"OK (MOCK MODE)";
    std::string mock_track_id = "mock_track_1"; std::string current_mock_id = "";
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        {
            std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
            if (current_mock_id != mock_track_id) {
                current_mock_id = mock_track_id; g_current_request_id++;
                std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                if (mock_track_id == "mock_track_1") {
                    g_spotify_overlay_state.current_track.track_name = "Mock Track 1"; g_spotify_overlay_state.current_track.artist_name = "Artist Alpha, Artist Beta";
                    g_spotify_overlay_state.current_track.artist_list = { "Artist Alpha", "Artist Beta" }; g_spotify_overlay_state.duration_ms = 30000; g_spotify_overlay_state.progress_ms = 0; g_spotify_overlay_state.is_playing = true;
                    g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();
                    g_current_lyrics.has_lyrics = false; g_current_lyrics.lines.clear(); g_is_fetching_lyrics = true; g_needs_lyrics_fetch = true;
                }
                else {
                    g_spotify_overlay_state.current_track.track_name = "Mock Track 2"; g_spotify_overlay_state.current_track.artist_name = "Solo Artist";
                    g_spotify_overlay_state.current_track.artist_list = { "Solo Artist" }; g_spotify_overlay_state.duration_ms = 30000; g_spotify_overlay_state.progress_ms = 0; g_spotify_overlay_state.is_playing = true;
                    g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();
                    LyricsData mockData; mockData.has_lyrics = true;
                    for (int i = 0; i < 20; ++i) mockData.lines.push_back({ i * 2500, L"Mock Instant Cache Line " + std::to_wstring(i + 1) });
                    g_current_lyrics = mockData; g_is_fetching_lyrics = false; g_needs_lyrics_fetch = false;
                }
                g_spotify_overlay_state.track_changed = true;
            }
            if (g_spotify_overlay_state.is_playing) {
                g_spotify_overlay_state.progress_ms; g_spotify_overlay_state.last_update = std::chrono::steady_clock::now();
                if (g_spotify_overlay_state.progress_ms >= g_spotify_overlay_state.duration_ms) mock_track_id = mock_track_id == "mock_track_1" ? "mock_track_2" : "mock_track_1";
            }
        }
        if (g_needs_lyrics_fetch) {
            g_needs_lyrics_fetch = false; int reqId = g_current_request_id.load();
            std::thread([reqId]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (reqId != g_current_request_id.load()) return;
                LyricsData mockData; mockData.has_lyrics = true;
                for (int i = 0; i < 20; ++i) mockData.lines.push_back({ i * 2500, L"Mock Web Lyrics Line " + std::to_wstring(i + 1) });
                std::lock_guard<std::mutex> lock(g_lyrics_mtx);
                if (reqId == g_current_request_id.load()) { g_current_lyrics = mockData; g_is_fetching_lyrics = false; }
                }).detach();
        }
    }
}

void SpotifyTrackerThread() {
    winrt::init_apartment();

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    DWORD taskIndex = 0;
    AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);

    LoadConfig();

#if USE_MOCK_MODE == 1
    RunMockMode();
    return;
#endif

    if (g_api_mode != 0 && (CLIENT_ID.empty() || CLIENT_SECRET.empty())) {
        g_app_state = STATE_INPUT_ID;
        g_refresh_token = "";
        DeleteFileW(g_token_file.c_str());
    }
    else {
        g_app_state = STATE_RUNNING;
    }

    std::string current_id = "";
    std::string current_gsmtc_id = "";
    int last_api_mode = g_api_mode;

    while (true) {
        if (g_app_state != STATE_RUNNING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        if (last_api_mode != g_api_mode) {
            current_id = "";
            current_gsmtc_id = "";
            last_api_mode = g_api_mode;
            if (g_api_mode != 0 && (CLIENT_ID.empty() || CLIENT_SECRET.empty())) {
                g_app_state = STATE_INPUT_ID; g_input_buffer = ""; g_error_message = "";
                continue;
            }
        }

        bool gsmtc_success = (g_api_mode != 1) ? updateGSMTCData(current_gsmtc_id) : false;

        if (g_api_mode == 0) {
            if (g_needs_lyrics_fetch) {
                g_needs_lyrics_fetch = false;

                std::string tName, aName; std::vector<std::string> aList; int reqId = 0; bool isApi = false;
                {
                    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                    tName = g_spotify_overlay_state.current_track.track_name; aName = g_spotify_overlay_state.current_track.artist_name;
                    aList = g_spotify_overlay_state.current_track.artist_list; reqId = g_current_request_id.load();
                    isApi = g_spotify_overlay_state.current_track.is_spotify_api;
                }
                std::thread([tName, aName, aList, reqId, isApi]() { FetchLyrics(tName, aName, aList, reqId, isApi); }).detach();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            continue;
        }

        LoadRefreshToken();
        if (g_refresh_token.empty()) {
            if (!AuthFlow()) {
                if (g_app_state != STATE_RUNNING) continue;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }
        }

        std::string token = getAccessToken();
        if (token.empty() && g_app_state == STATE_RUNNING) {
            if (!AuthFlow()) continue;
            token = getAccessToken();
        }

        while (g_app_state == STATE_RUNNING && (g_api_mode == 1 || g_api_mode == 2)) {

            if (g_needs_lyrics_fetch) {
                g_needs_lyrics_fetch = false;

                std::string tName, aName; std::vector<std::string> aList; int reqId = 0; bool isApi = false;
                {
                    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                    tName = g_spotify_overlay_state.current_track.track_name; aName = g_spotify_overlay_state.current_track.artist_name;
                    aList = g_spotify_overlay_state.current_track.artist_list; reqId = g_current_request_id.load();
                    isApi = g_spotify_overlay_state.current_track.is_spotify_api;
                }
                std::thread([tName, aName, aList, reqId, isApi]() { FetchLyrics(tName, aName, aList, reqId, isApi); }).detach();
            }

            int api_wait_ms = 0;
            if (!updateSpotifyData(token, current_id, api_wait_ms)) {
                if (api_wait_ms > 0) {

                    int elapsed_wait = 0;
                    while (elapsed_wait < api_wait_ms && g_app_state == STATE_RUNNING) {
                        if (g_api_mode == 2) {
                            updateGSMTCData(current_gsmtc_id);
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        elapsed_wait += 200;
                    }
                    continue;
                }
                token = getAccessToken();
                if (token.empty()) { AuthFlow(); token = getAccessToken(); }
                std::this_thread::sleep_for(std::chrono::seconds(2)); continue;

            }

            int wait_ms = 1500;
            int elapsed_sleep = 0;
            while (elapsed_sleep < wait_ms && g_app_state == STATE_RUNNING && (g_api_mode == 1 || g_api_mode == 2)) {
                if (g_api_mode == 2) {
                    updateGSMTCData(current_gsmtc_id);
                }

                if (g_needs_lyrics_fetch) {
                    g_needs_lyrics_fetch = false;
                    std::string tName, aName; std::vector<std::string> aList; int reqId = 0; bool isApi = false;
                    {
                        std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                        tName = g_spotify_overlay_state.current_track.track_name; aName = g_spotify_overlay_state.current_track.artist_name;
                        aList = g_spotify_overlay_state.current_track.artist_list; reqId = g_current_request_id.load();
                        isApi = g_spotify_overlay_state.current_track.is_spotify_api;
                    }
                    std::thread([tName, aName, aList, reqId, isApi]() { FetchLyrics(tName, aName, aList, reqId, isApi); }).detach();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                elapsed_sleep += 200;
            }
        }
    }
}

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_TOGGLE_LYRICS 1002
#define ID_TRAY_TOGGLE_SHOW_LYRICS 1003
#define ID_TRAY_TOGGLE_HIDE_PAUSE 1008
#define ID_TRAY_TOGGLE_BLUR 1009

const int OVERLAY_W = 400;
const int OVERLAY_H = 120;
const int OVERLAY_MARGIN = 30;
const int OVERLAY_POS_X = -1;
const int OVERLAY_POS_Y = -1;

NOTIFYICONDATA nid = {};

struct LyricDrawInfo { std::wstring trackName; std::wstring text; float targetY; float scaleX; float scaleY; float alpha; float z3d; BYTE r, g, b; BYTE gr, gg, gb; float glowIntensity; float sweepStart; float sweepEnd; };
struct RenderItem { std::wstring text; float x, y, scaleX, scaleY; float alpha; BYTE r, g, b; BYTE gr, gg, gb; float glowIntensity; };
struct SceneState { std::vector<RenderItem> lyrics2D; std::vector<RenderItem> lyrics3D; int currentOverlayH; float progressBarPct; };

namespace MyOverlayMenu {
    constexpr int MENU_ITEM_COUNT = 17;
    constexpr int MENU_W = 220;
    constexpr int MENU_BUFFER_H = 1020;
    constexpr float MENU_PAD_X = 10.0f;
    constexpr float MENU_PAD_BOTTOM = 14.0f;

    struct MenuState {
        HWND hwnd = nullptr;
        bool isOpen = false;
        bool isClosing = false;

        float windowAlpha = 0.0f;
        float windowScale = 0.92f;
        float windowOffsetY = 14.0f;

        float dropdownAnim = 0.0f; bool dropdownExpanded = false;
        float styleDropdownAnim = 0.0f; bool styleDropdownExpanded = false;
        float apiDropdownAnim = 0.0f; bool apiDropdownExpanded = false;
        float glowDropdownAnim = 0.0f; bool glowDropdownExpanded = false;
        float colorTransDropdownAnim = 0.0f; bool colorTransDropdownExpanded = false;

        float blurStrengthRowAnim = 0.0f;
        float rotationRowAnim = 0.0f;
        float opacityRowsAnim = 1.0f;

        float currentH = 514.0f;
        float targetBgAlpha = 0.9f;
        float targetAlbumAlpha = 1.0f;
        float targetRotation = 45.0f;
        float targetBlurStrength = 0.5f;
        int activeSlider = 0;

        float animShowLyrics = 1.0f; float animUseAccent = 1.0f; float animMoveMode = 0.0f;
        float animHideOnPause = 0.0f; float animBlur = 0.0f;

        float animLyricsModeIdx = 0.0f;
        float animStyleIdx = 0.0f;
        float animApiIdx = 1.0f;
        float animGlowIdx = 0.0f;
        float animColorTransIdx = 0.0f;

        int hoveredItem = -1;
        float hoverAnim[MENU_ITEM_COUNT] = { 0.0f };
        float scaleAnim[MENU_ITEM_COUNT] = {
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
        };

        D2D1_COLOR_F bgColor = D2D1::ColorF(0.08f, 0.08f, 0.08f, 0.95f);
        D2D1_COLOR_F targetBgColor = D2D1::ColorF(0.08f, 0.08f, 0.08f, 0.95f);
        D2D1_COLOR_F textColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
        D2D1_COLOR_F targetTextColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
        D2D1_COLOR_F accentColor = D2D1::ColorF(0.11f, 0.73f, 0.33f, 1.0f);
        D2D1_COLOR_F targetAccentColor = D2D1::ColorF(0.11f, 0.73f, 0.33f, 1.0f);

        bool needsRedraw = false;
    };

    static MenuState g_menuState;
    static Microsoft::WRL::ComPtr<ID2D1Factory> g_pD2DFactory;
    static Microsoft::WRL::ComPtr<IDWriteFactory> g_pDWriteFactory;
    static Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> g_pDCRT;

    struct MenuGlowCacheItem {
        std::wstring text;
        float w = 0.0f, h = 0.0f;
        BYTE gr = 0, gg = 0, gb = 0;
        Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pBmpRT;
        Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap;
    };
    static std::vector<std::shared_ptr<MenuGlowCacheItem>> s_menuGlowCaches;
    static BYTE s_menuLastR = 0, s_menuLastG = 0, s_menuLastB = 0;

    inline float SmoothLerp(float current, float target, float speed = 0.2f) {
        if (std::abs(target - current) < 0.001f) return target;
        return current + (target - current) * speed;
    }

    inline float CalcExactMenuHeight() {
        return 448.0f
            + (33.0f * g_menuState.blurStrengthRowAnim)
            + (56.0f * g_menuState.dropdownAnim)
            + (84.0f * g_menuState.styleDropdownAnim)
            + (84.0f * g_menuState.apiDropdownAnim)
            + (84.0f * g_menuState.glowDropdownAnim)
            + (84.0f * g_menuState.colorTransDropdownAnim)
            + (33.0f * g_menuState.rotationRowAnim)
            + (66.0f * g_menuState.opacityRowsAnim);
    }

    inline float GetMenuBottomY() {
        return (float)MENU_BUFFER_H - MENU_PAD_BOTTOM;
    }

    inline float GetMenuTopY() {
        return GetMenuBottomY() - g_menuState.currentH;
    }

    inline D2D1_MATRIX_3X2_F GetMenuBaseTransform() {
        float cx = (float)MENU_W * 0.5f;
        float cy = GetMenuBottomY();
        return D2D1::Matrix3x2F::Scale(g_menuState.windowScale, g_menuState.windowScale, D2D1::Point2F(cx, cy))
            * D2D1::Matrix3x2F::Translation(0.0f, g_menuState.windowOffsetY);
    }

    inline void StartCloseMenu() {
        if (g_menuState.isOpen && !g_menuState.isClosing) {
            g_menuState.isClosing = true;
            g_menuState.activeSlider = 0;
            ReleaseCapture();
        }
    }

    inline void UpdateTheme(D2D1_COLOR_F bg, D2D1_COLOR_F text, D2D1_COLOR_F accent) {
        g_menuState.targetBgColor = bg;
        g_menuState.targetTextColor = text;
        g_menuState.targetAccentColor = accent;
        if (!g_menuState.isOpen) {
            g_menuState.bgColor = bg;
            g_menuState.textColor = text;
            g_menuState.accentColor = accent;
        }
    }

    inline void ApplyItemTransform(ID2D1DCRenderTarget* pDCRT, const D2D1_RECT_F& rect, float scale) {
        float cx = (rect.left + rect.right) * 0.5f;
        float cy = (rect.top + rect.bottom) * 0.5f;
        pDCRT->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(cx, cy)) * GetMenuBaseTransform());
    }

    inline D2D1_COLOR_F LerpColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t, float alphaOverride = -1.0f) {
        return D2D1::ColorF(
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            (alphaOverride >= 0.0f) ? alphaOverride : (a.a + (b.a - a.a) * t)
        );
    }

    inline ID2D1Bitmap* GetOrCreateMenuDeepGlowBitmap(
        ID2D1DCRenderTarget* target, IDWriteTextFormat* font,
        const std::wstring& text, float w, float h, BYTE gr, BYTE gg, BYTE gb)
    {
        if (std::abs((int)gr - (int)s_menuLastR) > 3 ||
            std::abs((int)gg - (int)s_menuLastG) > 3 ||
            std::abs((int)gb - (int)s_menuLastB) > 3) {
            s_menuGlowCaches.clear();
            s_menuLastR = gr; s_menuLastG = gg; s_menuLastB = gb;
        }

        for (auto& item : s_menuGlowCaches) {
            if (item->text == text && std::abs(item->w - w) < 1.0f && std::abs(item->h - h) < 1.0f) {
                return item->pBitmap.Get();
            }
        }

        if (s_menuGlowCaches.size() > 60) {
            s_menuGlowCaches.erase(s_menuGlowCaches.begin(), s_menuGlowCaches.begin() + 20);
        }

        auto newItem = std::make_shared<MenuGlowCacheItem>();
        newItem->text = text;
        newItem->w = w; newItem->h = h;
        newItem->gr = s_menuLastR; newItem->gg = s_menuLastG; newItem->gb = s_menuLastB;

        float pad = 32.0f;
        D2D1_SIZE_F rtSize = D2D1::SizeF(w + pad * 2.0f, h + pad * 2.0f);

        Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pBaseRT;
        if (FAILED(target->CreateCompatibleRenderTarget(rtSize, &pBaseRT)) || !pBaseRT) return nullptr;

        pBaseRT->BeginDraw();
        pBaseRT->Clear(D2D1::ColorF(0, 0, 0, 0));
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> baseBrush;
        pBaseRT->CreateSolidColorBrush(D2D1::ColorF(gr / 255.0f, gg / 255.0f, gb / 255.0f, 0.95f), &baseBrush);
        D2D1_RECT_F localRect = D2D1::RectF(pad, pad, pad + w, pad + h);
        pBaseRT->DrawText(text.c_str(), (UINT32)text.length(), font, &localRect, baseBrush.Get());
        pBaseRT->EndDraw();

        Microsoft::WRL::ComPtr<ID2D1Bitmap> pInputBmp;
        pBaseRT->GetBitmap(&pInputBmp);

        Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pFinalRT;
        target->CreateCompatibleRenderTarget(rtSize, &pFinalRT);
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> pFinalContext;
        if (pFinalRT) pFinalRT.As(&pFinalContext);

        if (pFinalContext && pInputBmp) {
            Microsoft::WRL::ComPtr<ID2D1Effect> blur1, blur2, blur3, comp1, comp2;
            pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur1);
            pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur2);
            pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur3);

            blur1->SetInput(0, pInputBmp.Get());
            blur1->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 2.0f);

            blur2->SetInput(0, pInputBmp.Get());
            blur2->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 8.0f);

            blur3->SetInput(0, pInputBmp.Get());
            blur3->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 24.0f);

            pFinalContext->CreateEffect(CLSID_D2D1Composite, &comp1);
            pFinalContext->CreateEffect(CLSID_D2D1Composite, &comp2);
            comp1->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_PLUS);
            comp2->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_PLUS);

            comp1->SetInputEffect(0, blur1.Get());
            comp1->SetInputEffect(1, blur2.Get());
            comp2->SetInputEffect(0, comp1.Get());
            comp2->SetInputEffect(1, blur3.Get());

            pFinalRT->BeginDraw();
            pFinalRT->Clear(D2D1::ColorF(0, 0, 0, 0));
            pFinalContext->DrawImage(comp2.Get());
            pFinalRT->EndDraw();

            pFinalRT->GetBitmap(newItem->pBitmap.GetAddressOf());
            newItem->pBmpRT = pFinalRT;
        }
        else {
            newItem->pBitmap = pInputBmp;
            newItem->pBmpRT = pBaseRT;
        }

        s_menuGlowCaches.push_back(newItem);
        return newItem->pBitmap.Get();
    }

    inline void DrawSimpleBoxGlow(ID2D1DCRenderTarget* pDCRT, const D2D1_RECT_F& rect, float radius, float intensity, D2D1_COLOR_F glowCol) {
        if (intensity <= 0.01f) return;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> gBrush;
        for (int i = 5; i >= 1; --i) {
            float offset = (float)i * 1.4f;
            D2D1_ROUNDED_RECT gRect = D2D1::RoundedRect(
                D2D1::RectF(rect.left - offset, rect.top - offset, rect.right + offset, rect.bottom + offset),
                radius + offset, radius + offset);
            float a = (6.0f - (float)i) * 0.03f * intensity;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(glowCol.r, glowCol.g, glowCol.b, a), &gBrush);
            pDCRT->FillRoundedRectangle(&gRect, gBrush.Get());
        }
    }

    inline void DrawModernGlowText(ID2D1DCRenderTarget* pDCRT, const std::wstring& text, IDWriteTextFormat* pFormat, const D2D1_RECT_F& rect, D2D1_COLOR_F baseTextCol, D2D1_COLOR_F glowCol, float glowIntensity, float alphaMult = 1.0f) {
        if (alphaMult <= 0.01f || text.empty()) return;
        float rw = rect.right - rect.left;
        float rh = rect.bottom - rect.top;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> dimBrush;
        float dimAlpha = baseTextCol.a * alphaMult * (1.0f - glowIntensity * 0.65f);
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(baseTextCol.r, baseTextCol.g, baseTextCol.b, dimAlpha), &dimBrush);
        pDCRT->DrawText(text.c_str(), (UINT32)text.length(), pFormat, &rect, dimBrush.Get());

        if (glowIntensity > 0.01f) {
            BYTE gr = static_cast<BYTE>(std::clamp(glowCol.r * 255.0f, 0.0f, 255.0f));
            BYTE gg = static_cast<BYTE>(std::clamp(glowCol.g * 255.0f, 0.0f, 255.0f));
            BYTE gb = static_cast<BYTE>(std::clamp(glowCol.b * 255.0f, 0.0f, 255.0f));

            float glowAlpha = std::clamp(alphaMult * glowIntensity, 0.0f, 1.0f);
            ID2D1Bitmap* pGlowBmp = GetOrCreateMenuDeepGlowBitmap(pDCRT, pFormat, text, rw, rh, gr, gg, gb);
            if (pGlowBmp) {
                float pad = 32.0f;
                D2D1_RECT_F destRect = D2D1::RectF(rect.left - pad, rect.top - pad, rect.right + pad, rect.bottom + pad);
                pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }

            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, glowAlpha), &brightWhiteBrush);
            pDCRT->DrawText(text.c_str(), (UINT32)text.length(), pFormat, &rect, brightWhiteBrush.Get());
        }
    }

    inline void DrawToggleButton(ID2D1DCRenderTarget* pDCRT, IDWriteTextFormat* pFormat, const std::wstring& name, float animVal, const D2D1_RECT_F& rect, D2D1_COLOR_F textColor, D2D1_COLOR_F accentColor, float hoverAlpha, float scale, bool enabled = true) {
        float alphaMult = enabled ? 1.0f : 0.35f;
        float effectiveAnim = enabled ? animVal : 0.0f;
        float effHover = enabled ? hoverAlpha : 0.0f;

        ApplyItemTransform(pDCRT, rect, enabled ? scale : 1.0f);
        DrawSimpleBoxGlow(pDCRT, rect, 6.0f, effHover * alphaMult, accentColor);

        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(rect, 6.0f, 6.0f);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush, glowBrush, borderBrush;
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(textColor.r, textColor.g, textColor.b, (0.05f + effectiveAnim * 0.04f) * alphaMult), &bgBrush);
        pDCRT->FillRoundedRectangle(&rRect, bgBrush.Get());

        if (effHover > 0.01f) {
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.14f * effHover * alphaMult), &glowBrush);
            pDCRT->FillRoundedRectangle(&rRect, glowBrush.Get());
        }

        D2D1_COLOR_F bCol = LerpColor(D2D1::ColorF(textColor.r, textColor.g, textColor.b, (0.14f + effectiveAnim * 0.12f) * alphaMult), accentColor, effHover, (0.14f + effHover * 0.7f) * alphaMult);
        pDCRT->CreateSolidColorBrush(bCol, &borderBrush);
        pDCRT->DrawRoundedRectangle(&rRect, borderBrush.Get(), 1.0f);

        D2D1_RECT_F textRect = D2D1::RectF(rect.left + 10.0f, rect.top, rect.right - 30.0f, rect.bottom);
        DrawModernGlowText(pDCRT, name, pFormat, textRect, textColor, accentColor, effHover, alphaMult);

        float dotX = rect.right - 15.0f; float dotY = rect.top + (rect.bottom - rect.top) / 2.0f; float dotRadius = 4.5f; D2D1_POINT_2F center = D2D1::Point2F(dotX, dotY);
        if (effectiveAnim > 0.01f) {
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> dotGlowBrush, dotBrush;
            D2D1_COLOR_F gColor = accentColor; gColor.a = 0.25f * effectiveAnim * alphaMult;
            pDCRT->CreateSolidColorBrush(gColor, &dotGlowBrush);
            pDCRT->FillEllipse(D2D1::Ellipse(center, dotRadius + 3.0f, dotRadius + 3.0f), dotGlowBrush.Get());
            D2D1_COLOR_F activeColor = D2D1::ColorF(0.4f + (accentColor.r - 0.4f) * effectiveAnim, 0.4f + (accentColor.g - 0.4f) * effectiveAnim, 0.4f + (accentColor.b - 0.4f) * effectiveAnim, alphaMult);
            pDCRT->CreateSolidColorBrush(activeColor, &dotBrush);
            pDCRT->FillEllipse(D2D1::Ellipse(center, dotRadius, dotRadius), dotBrush.Get());
        }
        else {
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> offBrush;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(textColor.r * 0.5f + 0.2f, textColor.g * 0.5f + 0.2f, textColor.b * 0.5f + 0.2f, 0.5f * alphaMult), &offBrush);
            pDCRT->FillEllipse(D2D1::Ellipse(center, dotRadius, dotRadius), offBrush.Get());
        }
        pDCRT->SetTransform(GetMenuBaseTransform());
    }

    inline void DrawSimpleButton(ID2D1DCRenderTarget* pDCRT, IDWriteTextFormat* pFormat, const std::wstring& label, const D2D1_RECT_F& rect, D2D1_COLOR_F textColor, D2D1_COLOR_F accentColor, float hoverAlpha, float scale, bool isExitBtn = false) {
        ApplyItemTransform(pDCRT, rect, scale);
        D2D1_COLOR_F targetHighlight = isExitBtn ? D2D1::ColorF(1.0f, 0.35f, 0.35f, 1.0f) : accentColor;

        DrawSimpleBoxGlow(pDCRT, rect, 6.0f, hoverAlpha, targetHighlight);

        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(rect, 6.0f, 6.0f);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush, glowBrush, borderBrush;
        D2D1_COLOR_F baseBg = isExitBtn ? D2D1::ColorF(0.4f, 0.1f, 0.1f, 0.25f) : D2D1::ColorF(textColor.r, textColor.g, textColor.b, 0.05f);
        pDCRT->CreateSolidColorBrush(baseBg, &bgBrush);
        pDCRT->FillRoundedRectangle(&rRect, bgBrush.Get());

        if (hoverAlpha > 0.01f) {
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(targetHighlight.r, targetHighlight.g, targetHighlight.b, 0.16f * hoverAlpha), &glowBrush);
            pDCRT->FillRoundedRectangle(&rRect, glowBrush.Get());
        }

        D2D1_COLOR_F bCol = LerpColor(D2D1::ColorF(textColor.r, textColor.g, textColor.b, 0.15f), targetHighlight, hoverAlpha, 0.15f + 0.7f * hoverAlpha);
        pDCRT->CreateSolidColorBrush(bCol, &borderBrush);
        pDCRT->DrawRoundedRectangle(&rRect, borderBrush.Get(), 1.0f);

        D2D1_RECT_F textRect = D2D1::RectF(rect.left + 10.0f, rect.top, rect.right - 10.0f, rect.bottom);
        DrawModernGlowText(pDCRT, label, pFormat, textRect, textColor, targetHighlight, hoverAlpha, 1.0f);
        pDCRT->SetTransform(GetMenuBaseTransform());
    }

    inline void DrawOptDropdown(ID2D1DCRenderTarget* pDCRT, IDWriteTextFormat* pFormat, const std::wstring& name, const std::wstring& currentState, const std::wstring* opts, int optCount, int selectedIdx, float animSelectedIdx, bool isExpanded, float expandRatio, const D2D1_RECT_F& baseRect, D2D1_COLOR_F textColor, D2D1_COLOR_F accentColor, float hoverAlpha, float scale) {
        float baseH = 28.0f;
        float expandedH = 28.0f * (float)optCount;
        float currentH = baseH + (expandedH * expandRatio);
        D2D1_RECT_F totalRect = D2D1::RectF(baseRect.left, baseRect.top, baseRect.right, baseRect.top + currentH);

        ApplyItemTransform(pDCRT, totalRect, scale);
        DrawSimpleBoxGlow(pDCRT, totalRect, 6.0f, hoverAlpha, accentColor);

        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(totalRect, 6.0f, 6.0f);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush, glowBrush, borderBrush, activeBrush, activeBorderBrush;
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(textColor.r, textColor.g, textColor.b, 0.07f), &bgBrush);
        pDCRT->FillRoundedRectangle(&rRect, bgBrush.Get());

        if (hoverAlpha > 0.01f) {
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.14f * hoverAlpha), &glowBrush);
            pDCRT->FillRoundedRectangle(&rRect, glowBrush.Get());
        }

        D2D1_COLOR_F bCol = LerpColor(D2D1::ColorF(textColor.r, textColor.g, textColor.b, 0.2f), accentColor, hoverAlpha, 0.2f + 0.65f * hoverAlpha);
        pDCRT->CreateSolidColorBrush(bCol, &borderBrush);
        pDCRT->DrawRoundedRectangle(&rRect, borderBrush.Get(), 1.0f);

        std::wstring displayText = name + L" : " + currentState + (isExpanded ? L"  ▲" : L"  ▼");
        D2D1_RECT_F textRect = D2D1::RectF(baseRect.left + 10.0f, baseRect.top, baseRect.right - 10.0f, baseRect.top + baseH);
        DrawModernGlowText(pDCRT, displayText, pFormat, textRect, textColor, accentColor, hoverAlpha, 1.0f);

        if (expandRatio > 0.01f) {

            D2D1_RECT_F childClip = D2D1::RectF(
                baseRect.left - 12.0f,
                baseRect.top + baseH - (10.0f * expandRatio),
                baseRect.right + 12.0f,
                std::max(baseRect.top + baseH, totalRect.bottom - 1.0f)
            );
            pDCRT->PushAxisAlignedClip(&childClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            const float optRowH = 25.0f;
            const float topPad = 2.0f;

            float selY = baseRect.top + baseH + topPad + (animSelectedIdx * optRowH);
            D2D1_RECT_F selRect = D2D1::RectF(baseRect.left + 4.0f, selY + 1.0f, baseRect.right - 4.0f, selY + optRowH - 1.0f);
            D2D1_ROUNDED_RECT selRounded = D2D1::RoundedRect(selRect, 4.0f, 4.0f);

            DrawSimpleBoxGlow(pDCRT, selRect, 4.0f, 0.55f * expandRatio, accentColor);
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.22f * expandRatio), &activeBrush);
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.75f * expandRatio), &activeBorderBrush);
            pDCRT->FillRoundedRectangle(&selRounded, activeBrush.Get());
            pDCRT->DrawRoundedRectangle(&selRounded, activeBorderBrush.Get(), 1.0f);

            for (int i = 0; i < optCount; ++i) {
                float itemY = baseRect.top + baseH + topPad + (i * optRowH);
                D2D1_RECT_F textOpt = D2D1::RectF(baseRect.left + 15.0f, itemY + 2.0f, baseRect.right - 15.0f, itemY + optRowH - 2.0f);
                std::wstring label = (selectedIdx == i ? L"・ " : L"   ") + opts[i];
                float itemGlow = std::clamp(1.0f - std::abs((float)i - animSelectedIdx), 0.0f, 1.0f);
                DrawModernGlowText(pDCRT, label, pFormat, textOpt, textColor, accentColor, itemGlow, expandRatio);
            }
            pDCRT->PopAxisAlignedClip();
        }
        pDCRT->SetTransform(GetMenuBaseTransform());
    }

    inline void DrawGlowSlider(ID2D1DCRenderTarget* pDCRT, IDWriteTextFormat* pFormat, const std::wstring& label, float normValue, const D2D1_RECT_F& rect, D2D1_COLOR_F textColor, D2D1_COLOR_F accentColor, float hoverAlpha, float scale, float alphaMult = 1.0f) {
        if (alphaMult <= 0.01f) return;
        ApplyItemTransform(pDCRT, rect, scale);

        D2D1_COLOR_F aCol = accentColor; aCol.a *= alphaMult;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> railBrush, activeBrush;
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(textColor.r, textColor.g, textColor.b, 0.18f * alphaMult), &railBrush);
        pDCRT->CreateSolidColorBrush(aCol, &activeBrush);

        D2D1_RECT_F textRect = D2D1::RectF(rect.left + 2.0f, rect.top, rect.right, rect.top + 14.0f);
        DrawModernGlowText(pDCRT, label, pFormat, textRect, textColor, accentColor, hoverAlpha, alphaMult);

        float barY = rect.top + 18.0f; float barH = 5.0f; float barW = rect.right - rect.left - 4.0f; float barX = rect.left + 2.0f;
        D2D1_ROUNDED_RECT railRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + barW, barY + barH), 2.5f, 2.5f);
        pDCRT->FillRoundedRectangle(&railRect, railBrush.Get());
        float activeW = barW * std::clamp(normValue, 0.0f, 1.0f);
        if (activeW > 0.0f) {
            for (int i = 4; i >= 1; --i) {
                float offset = (float)i * 1.5f;
                D2D1_ROUNDED_RECT gRect = D2D1::RoundedRect(D2D1::RectF(barX - offset, barY - offset, barX + activeW + offset, barY + barH + offset), 2.5f + offset, 2.5f + offset);
                D2D1_COLOR_F gColor = accentColor; gColor.a = ((0.15f + 0.15f * hoverAlpha) / (float)i) * alphaMult;
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> glowBrush; pDCRT->CreateSolidColorBrush(gColor, &glowBrush);
                pDCRT->FillRoundedRectangle(&gRect, glowBrush.Get());
            }
            D2D1_ROUNDED_RECT activeRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + activeW, barY + barH), 2.5f, 2.5f);
            pDCRT->FillRoundedRectangle(&activeRect, activeBrush.Get());
        }
        pDCRT->SetTransform(GetMenuBaseTransform());
    }

    inline int GetItemIndexAtPoint(POINT pt) {
        float topY = GetMenuTopY();
        float bottomY = GetMenuBottomY();
        float itemLeft = MENU_PAD_X + 10.0f;
        float itemRight = (float)MENU_W - MENU_PAD_X - 10.0f;
        if (pt.x < itemLeft || pt.x > itemRight || pt.y < topY || pt.y > bottomY) return -1;

        float curY = topY + 12.0f; float itemH = 28.0f; float gap = 5.0f;

        for (int i = 0; i <= 4; ++i) {
            if (pt.y >= curY && pt.y <= curY + itemH) return i;
            curY += itemH + gap;
        }

        if (g_menuState.blurStrengthRowAnim > 0.01f) {
            float bsH = (28.0f + gap) * g_menuState.blurStrengthRowAnim;
            if (g_enable_blur && g_uiStyle == 0 && pt.y >= curY && pt.y <= curY + 28.0f) return 5;
            curY += bsH;
        }

        float h6 = itemH + (56.0f * g_menuState.dropdownAnim);
        if (pt.y >= curY && pt.y <= curY + h6) return 6;
        curY += h6 + gap;

        float h7 = itemH + (84.0f * g_menuState.styleDropdownAnim);
        if (pt.y >= curY && pt.y <= curY + h7) return 7;
        curY += h7 + gap;

        float h8 = itemH + (84.0f * g_menuState.apiDropdownAnim);
        if (pt.y >= curY && pt.y <= curY + h8) return 8;
        curY += h8 + gap;

        float h9 = itemH + (84.0f * g_menuState.glowDropdownAnim);
        if (pt.y >= curY && pt.y <= curY + h9) return 9;
        curY += h9 + gap;

        float h10 = itemH + (84.0f * g_menuState.colorTransDropdownAnim);
        if (pt.y >= curY && pt.y <= curY + h10) return 10;
        curY += h10 + gap;

        if (g_menuState.rotationRowAnim > 0.01f) {
            float rotH = (28.0f + gap) * g_menuState.rotationRowAnim;
            if (g_color_trans_mode == 2 && pt.y >= curY && pt.y <= curY + 28.0f) return 11;
            curY += rotH;
        }

        if (g_menuState.opacityRowsAnim > 0.01f) {
            float opRowH = (28.0f + gap) * g_menuState.opacityRowsAnim;
            if (g_uiStyle != 1 && pt.y >= curY && pt.y <= curY + 28.0f) return 12;
            curY += opRowH;
            if (g_uiStyle != 1 && pt.y >= curY && pt.y <= curY + 28.0f) return 13;
            curY += opRowH;
        }

        for (int i = 14; i <= 16; ++i) {
            if (pt.y >= curY && pt.y <= curY + itemH) return i;
            curY += itemH + gap;
        }
        return -1;
    }

    inline void RenderScene(HWND hwnd, ID2D1DCRenderTarget* pDCRT, IDWriteFactory* pDWriteFactory) {
        if (!pDCRT || !pDWriteFactory) return;
        pDCRT->BeginDraw(); pDCRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        pDCRT->SetTransform(GetMenuBaseTransform());

        float cardLeft = MENU_PAD_X;
        float cardRight = (float)MENU_W - MENU_PAD_X;
        float topY = GetMenuTopY();
        float bottomY = GetMenuBottomY();

        D2D1_ROUNDED_RECT bgCard = D2D1::RoundedRect(D2D1::RectF(cardLeft, topY, cardRight, bottomY), 10.0f, 10.0f);

        D2D1_COLOR_F bgColor = g_menuState.bgColor;
        D2D1_COLOR_F textColor = g_menuState.textColor;
        D2D1_COLOR_F accentColor = g_menuState.accentColor;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush, borderBrush;
        pDCRT->CreateSolidColorBrush(bgColor, &bgBrush);
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.45f), &borderBrush);
        pDCRT->FillRoundedRectangle(&bgCard, bgBrush.Get());
        pDCRT->DrawRoundedRectangle(&bgCard, borderBrush.Get(), 1.0f);

        Microsoft::WRL::ComPtr<IDWriteTextFormat> pFormat;
        pDWriteFactory->CreateTextFormat(L"Yu Gothic UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f, L"ja-jp", &pFormat);
        if (pFormat) pFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        float itemLeft = cardLeft + 10.0f;
        float itemRight = cardRight - 10.0f;
        float curY = topY + 12.0f; float itemH = 28.0f; float gap = 5.0f;

        DrawToggleButton(pDCRT, pFormat.Get(), L"Show Lyrics", g_menuState.animShowLyrics, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[0], g_menuState.scaleAnim[0]); curY += itemH + gap;
        DrawToggleButton(pDCRT, pFormat.Get(), L"Accent Color", g_menuState.animUseAccent, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[1], g_menuState.scaleAnim[1]); curY += itemH + gap;
        DrawToggleButton(pDCRT, pFormat.Get(), L"Move Mode", g_menuState.animMoveMode, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[2], g_menuState.scaleAnim[2]); curY += itemH + gap;
        DrawToggleButton(pDCRT, pFormat.Get(), L"Hide on Pause", g_menuState.animHideOnPause, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[3], g_menuState.scaleAnim[3]); curY += itemH + gap;

        bool isBlurAvailable = (g_uiStyle == 0);
        std::wstring blurLabel = isBlurAvailable ? L"Blur" : L"Blur (Modern Only)";
        DrawToggleButton(pDCRT, pFormat.Get(), blurLabel, g_menuState.animBlur, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[4], g_menuState.scaleAnim[4], isBlurAvailable); curY += itemH + gap;

        if (g_menuState.blurStrengthRowAnim > 0.01f) {
            float bsH = (28.0f + gap) * g_menuState.blurStrengthRowAnim;
            D2D1_RECT_F bsClip = D2D1::RectF(0.0f, curY - 28.0f, (float)MENU_W, curY + bsH + 12.0f * g_menuState.blurStrengthRowAnim);
            pDCRT->PushAxisAlignedClip(&bsClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            std::wstring bsLabel = L"Blur Strength (" + std::to_wstring((int)std::round(::g_blur_strength * 100.0f)) + L"%)";
            DrawGlowSlider(pDCRT, pFormat.Get(), bsLabel, ::g_blur_strength, D2D1::RectF(itemLeft, curY, itemRight, curY + 28.0f), textColor, accentColor, g_menuState.hoverAnim[5], g_menuState.scaleAnim[5], g_menuState.blurStrengthRowAnim);
            pDCRT->PopAxisAlignedClip();
            curY += bsH;
        }

        const std::wstring modeOpts[2] = { L"2D Mode", L"3D Mode" };
        int modeIdx = g_is_3d_lyrics ? 1 : 0;
        std::wstring modeStr = g_is_3d_lyrics ? L"3D" : L"2D";
        DrawOptDropdown(pDCRT, pFormat.Get(), L"Lyrics Mode", modeStr, modeOpts, 2, modeIdx, g_menuState.animLyricsModeIdx, g_menuState.dropdownExpanded, g_menuState.dropdownAnim, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[6], g_menuState.scaleAnim[6]);
        curY += itemH + (56.0f * g_menuState.dropdownAnim) + gap;

        const std::wstring styleOpts[3] = { L"Modern", L"Clean", L"Retro" };
        DrawOptDropdown(pDCRT, pFormat.Get(), L"UI Style", styleOpts[g_uiStyle], styleOpts, 3, g_uiStyle, g_menuState.animStyleIdx, g_menuState.styleDropdownExpanded, g_menuState.styleDropdownAnim, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[7], g_menuState.scaleAnim[7]);
        curY += itemH + (84.0f * g_menuState.styleDropdownAnim) + gap;

        const std::wstring apiOpts[3] = { L"No Use API", L"Spotify API", L"Both API" };
        DrawOptDropdown(pDCRT, pFormat.Get(), L"API Mode", apiOpts[g_api_mode], apiOpts, 3, g_api_mode, g_menuState.animApiIdx, g_menuState.apiDropdownExpanded, g_menuState.apiDropdownAnim, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[8], g_menuState.scaleAnim[8]);
        curY += itemH + (84.0f * g_menuState.apiDropdownAnim) + gap;

        const std::wstring glowOpts[3] = { L"Default", L"Fixed Sweep", L"Line Sweep" };
        DrawOptDropdown(pDCRT, pFormat.Get(), L"Glow Mode", glowOpts[g_lyrics_glow_mode], glowOpts, 3, g_lyrics_glow_mode, g_menuState.animGlowIdx, g_menuState.glowDropdownExpanded, g_menuState.glowDropdownAnim, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[9], g_menuState.scaleAnim[9]);
        curY += itemH + (84.0f * g_menuState.glowDropdownAnim) + gap;

        const std::wstring transOpts[3] = { L"Default", L"Radial", L"Direction" };
        DrawOptDropdown(pDCRT, pFormat.Get(), L"Color Trans", transOpts[g_color_trans_mode], transOpts, 3, g_color_trans_mode, g_menuState.animColorTransIdx, g_menuState.colorTransDropdownExpanded, g_menuState.colorTransDropdownAnim, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[10], g_menuState.scaleAnim[10]);
        curY += itemH + (84.0f * g_menuState.colorTransDropdownAnim) + gap;

        if (g_menuState.rotationRowAnim > 0.01f) {
            float rotH = (28.0f + gap) * g_menuState.rotationRowAnim;
            D2D1_RECT_F rotClip = D2D1::RectF(0.0f, curY - 28.0f, (float)MENU_W, curY + rotH + 12.0f * g_menuState.rotationRowAnim);
            pDCRT->PushAxisAlignedClip(&rotClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            std::wstring rotLabel = L"Rotation (" + std::to_wstring((int)std::round(::g_color_trans_rotation)) + L"°)";
            DrawGlowSlider(pDCRT, pFormat.Get(), rotLabel, ::g_color_trans_rotation / 360.0f, D2D1::RectF(itemLeft, curY, itemRight, curY + 28.0f), textColor, accentColor, g_menuState.hoverAnim[11], g_menuState.scaleAnim[11], g_menuState.rotationRowAnim);
            pDCRT->PopAxisAlignedClip();
            curY += rotH;
        }

        if (g_menuState.opacityRowsAnim > 0.01f) {
            float opRowH = (28.0f + gap) * g_menuState.opacityRowsAnim;
            float totalOpH = opRowH * 2.0f;
            D2D1_RECT_F opClip = D2D1::RectF(0.0f, curY - 28.0f, (float)MENU_W, curY + totalOpH + 12.0f * g_menuState.opacityRowsAnim);
            pDCRT->PushAxisAlignedClip(&opClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            std::wstring bgLabel = L"Background Opacity (" + std::to_wstring((int)std::round(::g_bg_alpha * 100.0f)) + L"%)";
            DrawGlowSlider(pDCRT, pFormat.Get(), bgLabel, ::g_bg_alpha, D2D1::RectF(itemLeft, curY, itemRight, curY + 28.0f), textColor, accentColor, g_menuState.hoverAnim[12], g_menuState.scaleAnim[12], g_menuState.opacityRowsAnim);
            curY += opRowH;

            std::wstring albLabel = L"Album Opacity (" + std::to_wstring((int)std::round(::g_album_alpha * 100.0f)) + L"%)";
            DrawGlowSlider(pDCRT, pFormat.Get(), albLabel, ::g_album_alpha, D2D1::RectF(itemLeft, curY, itemRight, curY + 28.0f), textColor, accentColor, g_menuState.hoverAnim[13], g_menuState.scaleAnim[13], g_menuState.opacityRowsAnim);
            curY += opRowH;

            pDCRT->PopAxisAlignedClip();
        }

        std::wstring mgrText = LyricsManager::IsOpen() ? L"Lyrics List: Open" : L"Lyrics List: Closed";
        DrawSimpleButton(pDCRT, pFormat.Get(), mgrText, D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[14], g_menuState.scaleAnim[14]); curY += itemH + gap;
        DrawSimpleButton(pDCRT, pFormat.Get(), L"Reset Position", D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), textColor, accentColor, g_menuState.hoverAnim[15], g_menuState.scaleAnim[15]); curY += itemH + gap;
        DrawSimpleButton(pDCRT, pFormat.Get(), L"Exit App", D2D1::RectF(itemLeft, curY, itemRight, curY + itemH), D2D1::ColorF(1.0f, 0.4f, 0.4f), accentColor, g_menuState.hoverAnim[16], g_menuState.scaleAnim[16], true);

        pDCRT->SetTransform(D2D1::Matrix3x2F::Identity());
        pDCRT->EndDraw();
    }

    inline LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_SETCURSOR: SetCursor(LoadCursor(NULL, IDC_ARROW)); return TRUE;
        case WM_MOUSEMOVE: {
            if (g_menuState.activeSlider > 0 && !g_menuState.isClosing) {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                float barX = MENU_PAD_X + 12.0f;
                float barW = (float)MENU_W - (MENU_PAD_X + 12.0f) * 2.0f;
                if (g_menuState.activeSlider == 1) {
                    g_menuState.targetBgAlpha = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                }
                else if (g_menuState.activeSlider == 2) {
                    g_menuState.targetAlbumAlpha = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                }
                else if (g_menuState.activeSlider == 3) {
                    g_menuState.targetRotation = std::clamp((pt.x - barX) / barW, 0.0f, 1.0f) * 360.0f;
                }
                else if (g_menuState.activeSlider == 4) {
                    g_menuState.targetBlurStrength = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                }
                g_menuState.needsRedraw = true;
            }
            break;
        }
        case WM_LBUTTONUP:
            if (g_menuState.activeSlider > 0) {
                if (g_menuState.activeSlider == 1) ::g_bg_alpha = g_menuState.targetBgAlpha;
                else if (g_menuState.activeSlider == 2) ::g_album_alpha = g_menuState.targetAlbumAlpha;
                else if (g_menuState.activeSlider == 3) ::g_color_trans_rotation = g_menuState.targetRotation;
                else if (g_menuState.activeSlider == 4) ::g_blur_strength = g_menuState.targetBlurStrength;
                g_menuState.activeSlider = 0; ReleaseCapture(); SaveConfig();
            }
            break;
        case WM_CREATE: SetTimer(hwnd, 1, 16, NULL); break;
        case WM_TIMER: {
            bool animating = false;
            auto UpdateAnim = [&](float& val, float target, float speed = 0.2f) {
                if (std::abs(target - val) > 0.001f) { val = SmoothLerp(val, target, speed); animating = true; }
                else { val = target; }
                };

            if (g_menuState.isClosing) {
                UpdateAnim(g_menuState.windowAlpha, 0.0f, 0.25f);
                UpdateAnim(g_menuState.windowScale, 0.92f, 0.25f);
                UpdateAnim(g_menuState.windowOffsetY, 14.0f, 0.25f);
                if (g_menuState.windowAlpha < 0.02f) {
                    KillTimer(hwnd, 1);
                    DestroyWindow(hwnd);
                    return 0;
                }
            }
            else {
                UpdateAnim(g_menuState.windowAlpha, 1.0f, 0.20f);
                UpdateAnim(g_menuState.windowScale, 1.0f, 0.22f);
                UpdateAnim(g_menuState.windowOffsetY, 0.0f, 0.22f);
            }

            UpdateAnim(g_menuState.bgColor.r, g_menuState.targetBgColor.r, 0.15f);
            UpdateAnim(g_menuState.bgColor.g, g_menuState.targetBgColor.g, 0.15f);
            UpdateAnim(g_menuState.bgColor.b, g_menuState.targetBgColor.b, 0.15f);
            UpdateAnim(g_menuState.textColor.r, g_menuState.targetTextColor.r, 0.15f);
            UpdateAnim(g_menuState.textColor.g, g_menuState.targetTextColor.g, 0.15f);
            UpdateAnim(g_menuState.textColor.b, g_menuState.targetTextColor.b, 0.15f);
            UpdateAnim(g_menuState.accentColor.r, g_menuState.targetAccentColor.r, 0.15f);
            UpdateAnim(g_menuState.accentColor.g, g_menuState.targetAccentColor.g, 0.15f);
            UpdateAnim(g_menuState.accentColor.b, g_menuState.targetAccentColor.b, 0.15f);

            UpdateAnim(g_menuState.dropdownAnim, g_menuState.dropdownExpanded ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.styleDropdownAnim, g_menuState.styleDropdownExpanded ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.apiDropdownAnim, g_menuState.apiDropdownExpanded ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.glowDropdownAnim, g_menuState.glowDropdownExpanded ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.colorTransDropdownAnim, g_menuState.colorTransDropdownExpanded ? 1.0f : 0.0f, 0.22f);

            UpdateAnim(g_menuState.blurStrengthRowAnim, (::g_enable_blur && ::g_uiStyle == 0) ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.rotationRowAnim, (::g_color_trans_mode == 2) ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.opacityRowsAnim, (::g_uiStyle != 1) ? 1.0f : 0.0f, 0.22f);

            g_menuState.currentH = CalcExactMenuHeight();

            UpdateAnim(g_menuState.animLyricsModeIdx, ::g_is_3d_lyrics ? 1.0f : 0.0f, 0.22f);
            UpdateAnim(g_menuState.animStyleIdx, (float)::g_uiStyle, 0.22f);
            UpdateAnim(g_menuState.animApiIdx, (float)::g_api_mode, 0.22f);
            UpdateAnim(g_menuState.animGlowIdx, (float)::g_lyrics_glow_mode, 0.22f);
            UpdateAnim(g_menuState.animColorTransIdx, (float)::g_color_trans_mode, 0.22f);

            if (!g_menuState.isClosing) {
                POINT cursorPt;
                GetCursorPos(&cursorPt);
                ScreenToClient(hwnd, &cursorPt);
                g_menuState.hoveredItem = GetItemIndexAtPoint(cursorPt);
            }
            else {
                g_menuState.hoveredItem = -1;
            }

            for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
                bool isHov = (g_menuState.hoveredItem == i);
                if (i == 4 && ::g_uiStyle != 0) isHov = false;
                UpdateAnim(g_menuState.hoverAnim[i], isHov ? 1.0f : 0.0f, 0.2f);
                UpdateAnim(g_menuState.scaleAnim[i], isHov ? 1.045f : 1.0f, 0.25f);
            }

            UpdateAnim(::g_bg_alpha, g_menuState.targetBgAlpha, 0.25f);
            UpdateAnim(::g_album_alpha, g_menuState.targetAlbumAlpha, 0.25f);
            UpdateAnim(::g_color_trans_rotation, g_menuState.targetRotation, 0.25f);
            UpdateAnim(::g_blur_strength, g_menuState.targetBlurStrength, 0.25f);

            UpdateAnim(g_menuState.animShowLyrics, ::g_show_lyrics ? 1.0f : 0.0f);
            UpdateAnim(g_menuState.animUseAccent, ::g_use_accent_color ? 1.0f : 0.0f);
            UpdateAnim(g_menuState.animMoveMode, ::g_move_mode ? 1.0f : 0.0f);
            UpdateAnim(g_menuState.animHideOnPause, ::g_hide_on_pause ? 1.0f : 0.0f);
            UpdateAnim(g_menuState.animBlur, (::g_enable_blur && ::g_uiStyle == 0) ? 1.0f : 0.0f);

            if (animating || g_menuState.needsRedraw) { g_menuState.needsRedraw = false; InvalidateRect(hwnd, NULL, FALSE); }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc); int w = rc.right - rc.left; int h = rc.bottom - rc.top;
            HDC hdcMem = CreateCompatibleDC(hdc); HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h); HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

            if (!g_pD2DFactory) D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_pD2DFactory.GetAddressOf());
            if (!g_pDWriteFactory) DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(g_pDWriteFactory.GetAddressOf()));
            if (!g_pDCRT && g_pD2DFactory) {
                D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
                g_pD2DFactory->CreateDCRenderTarget(&props, g_pDCRT.GetAddressOf());
            }

            if (g_pDCRT && g_pDWriteFactory) { g_pDCRT->BindDC(hdcMem, &rc); RenderScene(hwnd, g_pDCRT.Get(), g_pDWriteFactory.Get()); }

            BYTE frameAlpha = static_cast<BYTE>(std::clamp(g_menuState.windowAlpha * 255.0f, 0.0f, 255.0f));
            POINT ptDst = { 0, 0 }; RECT wRect; GetWindowRect(hwnd, &wRect); ptDst.x = wRect.left; ptDst.y = wRect.top;
            SIZE sz = { w, h }; POINT ptSrc = { 0, 0 }; BLENDFUNCTION blend = { AC_SRC_OVER, 0, frameAlpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(hwnd, hdc, &ptDst, &sz, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

            SelectObject(hdcMem, hOld); DeleteObject(hBmp); DeleteDC(hdcMem); EndPaint(hwnd, &ps); return 0;
        }
        case WM_LBUTTONDOWN: {
            if (g_menuState.isClosing) return 0;

            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            float topY = GetMenuTopY();
            float bottomY = GetMenuBottomY();
            if (pt.y < topY || pt.y > bottomY || pt.x < MENU_PAD_X || pt.x > MENU_W - MENU_PAD_X) {
                StartCloseMenu();
                return 0;
            }

            float curY = topY + 12.0f; float itemH = 28.0f; float gap = 5.0f;
            float barX = MENU_PAD_X + 12.0f;
            float barW = (float)MENU_W - (MENU_PAD_X + 12.0f) * 2.0f;

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_SHOW_LYRICS, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_USE_ACCENT, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_MOVE_MODE, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_HIDE_PAUSE, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                if (g_uiStyle == 0) {
                    PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_BLUR, 0);
                    g_menuState.needsRedraw = true;
                }
                return 0;
            }
            curY += itemH + gap;

            if (g_menuState.blurStrengthRowAnim > 0.01f) {
                float bsH = (28.0f + gap) * g_menuState.blurStrengthRowAnim;
                if (g_enable_blur && g_uiStyle == 0 && pt.y >= curY && pt.y <= curY + 28.0f) {
                    g_menuState.activeSlider = 4;
                    g_menuState.targetBlurStrength = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                    SetCapture(hwnd); g_menuState.needsRedraw = true; return 0;
                }
                curY += bsH;
            }

            if (pt.y >= curY && pt.y <= curY + itemH) {
                g_menuState.dropdownExpanded = !g_menuState.dropdownExpanded;
                g_menuState.needsRedraw = true; return 0;
            }
            else if (g_menuState.dropdownExpanded && pt.y > curY + itemH && pt.y <= curY + itemH + 56.0f) {
                PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_LYRICS, 0);
                g_menuState.needsRedraw = true; return 0;
            }
            curY += itemH + (56.0f * g_menuState.dropdownAnim) + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                g_menuState.styleDropdownExpanded = !g_menuState.styleDropdownExpanded;
                g_menuState.needsRedraw = true; return 0;
            }
            else if (g_menuState.styleDropdownExpanded && pt.y > curY + itemH && pt.y <= curY + itemH + 84.0f) {
                int selectedIndex = std::clamp((int)((pt.y - (curY + itemH + 2.0f)) / 25.0f), 0, 2);
                g_uiStyle = selectedIndex; SaveConfig();
                g_menuState.needsRedraw = true; return 0;
            }
            curY += itemH + (84.0f * g_menuState.styleDropdownAnim) + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                g_menuState.apiDropdownExpanded = !g_menuState.apiDropdownExpanded;
                g_menuState.needsRedraw = true; return 0;
            }
            else if (g_menuState.apiDropdownExpanded && pt.y > curY + itemH && pt.y <= curY + itemH + 84.0f) {
                int selectedIndex = std::clamp((int)((pt.y - (curY + itemH + 2.0f)) / 25.0f), 0, 2);
                g_api_mode = selectedIndex; SaveConfig();
                g_menuState.needsRedraw = true; return 0;
            }
            curY += itemH + (84.0f * g_menuState.apiDropdownAnim) + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                g_menuState.glowDropdownExpanded = !g_menuState.glowDropdownExpanded;
                g_menuState.needsRedraw = true; return 0;
            }
            else if (g_menuState.glowDropdownExpanded && pt.y > curY + itemH && pt.y <= curY + itemH + 84.0f) {
                int selectedIndex = std::clamp((int)((pt.y - (curY + itemH + 2.0f)) / 25.0f), 0, 2);
                g_lyrics_glow_mode = selectedIndex; SaveConfig();
                g_menuState.needsRedraw = true; return 0;
            }
            curY += itemH + (84.0f * g_menuState.glowDropdownAnim) + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                g_menuState.colorTransDropdownExpanded = !g_menuState.colorTransDropdownExpanded;
                g_menuState.needsRedraw = true; return 0;
            }
            else if (g_menuState.colorTransDropdownExpanded && pt.y > curY + itemH && pt.y <= curY + itemH + 84.0f) {
                int selectedIndex = std::clamp((int)((pt.y - (curY + itemH + 2.0f)) / 25.0f), 0, 2);
                g_color_trans_mode = selectedIndex; SaveConfig();
                g_menuState.needsRedraw = true; return 0;
            }
            curY += itemH + (84.0f * g_menuState.colorTransDropdownAnim) + gap;

            if (g_menuState.rotationRowAnim > 0.01f) {
                float rotH = (28.0f + gap) * g_menuState.rotationRowAnim;
                if (g_color_trans_mode == 2 && pt.y >= curY && pt.y <= curY + 28.0f) {
                    g_menuState.activeSlider = 3;
                    g_menuState.targetRotation = std::clamp((pt.x - barX) / barW, 0.0f, 1.0f) * 360.0f;
                    SetCapture(hwnd); g_menuState.needsRedraw = true; return 0;
                }
                curY += rotH;
            }

            if (g_menuState.opacityRowsAnim > 0.01f) {
                float opRowH = (28.0f + gap) * g_menuState.opacityRowsAnim;
                if (g_uiStyle != 1 && pt.y >= curY && pt.y <= curY + 28.0f) {
                    g_menuState.activeSlider = 1;
                    g_menuState.targetBgAlpha = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                    SetCapture(hwnd); g_menuState.needsRedraw = true; return 0;
                }
                curY += opRowH;

                if (g_uiStyle != 1 && pt.y >= curY && pt.y <= curY + 28.0f) {
                    g_menuState.activeSlider = 2;
                    g_menuState.targetAlbumAlpha = std::clamp((pt.x - barX) / barW, 0.05f, 1.0f);
                    SetCapture(hwnd); g_menuState.needsRedraw = true; return 0;
                }
                curY += opRowH;
            }

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_TOGGLE_LYRICS_MGR, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) { PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_RESET_POS, 0); g_menuState.needsRedraw = true; return 0; }
            curY += itemH + gap;

            if (pt.y >= curY && pt.y <= curY + itemH) {
                PostMessage(GetParent(hwnd), WM_COMMAND, ID_TRAY_EXIT, 0);
                StartCloseMenu();
                return 0;
            }

            g_menuState.needsRedraw = true;
            break;
        }
        case WM_KILLFOCUS:
            StartCloseMenu();
            break;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            g_menuState.hwnd = nullptr;
            g_menuState.isOpen = false;
            g_menuState.isClosing = false;
            break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void ShowCustomMenu(HWND hParent, int mouseX, int mouseY) {
    if (MyOverlayMenu::g_menuState.isOpen && MyOverlayMenu::g_menuState.hwnd) {
        MyOverlayMenu::StartCloseMenu();
        return;
    }
    static bool registered = false;
    if (!registered) {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = MyOverlayMenu::MenuWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"SpotifyOverlayCustomMenuUnique";
        RegisterClass(&wc);
        registered = true;
    }

    int menuW = MyOverlayMenu::MENU_W;
    int maxMenuH = MyOverlayMenu::MENU_BUFFER_H;

    MyOverlayMenu::g_menuState.blurStrengthRowAnim = (::g_enable_blur && ::g_uiStyle == 0) ? 1.0f : 0.0f;
    MyOverlayMenu::g_menuState.rotationRowAnim = (::g_color_trans_mode == 2) ? 1.0f : 0.0f;
    MyOverlayMenu::g_menuState.opacityRowsAnim = (::g_uiStyle != 1) ? 1.0f : 0.0f;
    MyOverlayMenu::g_menuState.currentH = MyOverlayMenu::CalcExactMenuHeight();

    MyOverlayMenu::g_menuState.animLyricsModeIdx = ::g_is_3d_lyrics ? 1.0f : 0.0f;
    MyOverlayMenu::g_menuState.animStyleIdx = (float)::g_uiStyle;
    MyOverlayMenu::g_menuState.animApiIdx = (float)::g_api_mode;
    MyOverlayMenu::g_menuState.animGlowIdx = (float)::g_lyrics_glow_mode;
    MyOverlayMenu::g_menuState.animColorTransIdx = (float)::g_color_trans_mode;

    POINT mousePt = { mouseX, mouseY };
    HMONITOR hMon = MonitorFromPoint(mousePt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    RECT workArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    if (GetMonitorInfo(hMon, &mi)) {
        workArea = mi.rcWork;
    }

    int posX = mouseX - menuW / 2;
    if (posX < workArea.left + 8) posX = workArea.left + 8;
    if (posX + menuW > workArea.right - 8) posX = workArea.right - menuW - 8;

    int initH = (int)std::ceil(MyOverlayMenu::g_menuState.currentH);
    int desiredCardBottom = mouseY - 8;

    if (desiredCardBottom > workArea.bottom - 8) {
        desiredCardBottom = workArea.bottom - 8;
    }
    if (desiredCardBottom - initH < workArea.top + 8) {
        desiredCardBottom = std::min((int)workArea.bottom - 8, (int)workArea.top + initH + 8);
    }

    int posY = desiredCardBottom - (maxMenuH - (int)MyOverlayMenu::MENU_PAD_BOTTOM);

    MyOverlayMenu::g_menuState.hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"SpotifyOverlayCustomMenuUnique", L"", WS_POPUP,
        posX, posY, menuW, maxMenuH, hParent, NULL, GetModuleHandle(NULL), NULL);

    if (MyOverlayMenu::g_menuState.hwnd) {
        MyOverlayMenu::g_menuState.isOpen = true;
        MyOverlayMenu::g_menuState.isClosing = false;

        MyOverlayMenu::g_menuState.windowAlpha = 0.0f;
        MyOverlayMenu::g_menuState.windowScale = 0.92f;
        MyOverlayMenu::g_menuState.windowOffsetY = 14.0f;

        MyOverlayMenu::g_menuState.hoveredItem = -1;
        for (int i = 0; i < MyOverlayMenu::MENU_ITEM_COUNT; ++i) {
            MyOverlayMenu::g_menuState.hoverAnim[i] = 0.0f;
            MyOverlayMenu::g_menuState.scaleAnim[i] = 1.0f;
        }
        MyOverlayMenu::g_menuState.targetBgAlpha = ::g_bg_alpha;
        MyOverlayMenu::g_menuState.targetAlbumAlpha = ::g_album_alpha;
        MyOverlayMenu::g_menuState.targetRotation = ::g_color_trans_rotation;
        MyOverlayMenu::g_menuState.targetBlurStrength = ::g_blur_strength;
        MyOverlayMenu::g_menuState.needsRedraw = true;
        ShowWindow(MyOverlayMenu::g_menuState.hwnd, SW_SHOW);
        SetForegroundWindow(MyOverlayMenu::g_menuState.hwnd);
    }
}
SceneState g_currentScene;

struct LyricGlowCacheItem {
    std::wstring trackName;
    std::wstring text;
    BYTE gr, gg, gb;
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pBmpRT;
    Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap;
};

static std::vector<std::shared_ptr<LyricGlowCacheItem>> s_glowCaches;
static BYTE s_lastTargetR = 0, s_lastTargetG = 0, s_lastTargetB = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT && g_app_state != STATE_RUNNING) {
            POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam); ScreenToClient(hwnd, &pt);
            if (pt.x >= (OVERLAY_W - 80) && pt.x <= (OVERLAY_W - 20) && pt.y >= 62 && pt.y <= 97) return HTCLIENT;
            return HTCAPTION;
        }
        return hit;
    }
    case WM_LBUTTONDOWN:
        if (g_app_state == STATE_INPUT_ID || g_app_state == STATE_INPUT_SECRET) {
            POINT pt; GetCursorPos(&pt); ScreenToClient(hwnd, &pt);
            if (pt.x >= (OVERLAY_W - 80) && pt.x <= (OVERLAY_W - 20) && pt.y >= 62 && pt.y <= 97) {
                g_api_mode = 0; SaveConfig();
                g_app_state = STATE_RUNNING; g_error_message = ""; g_input_buffer = "";
                return 0;
            }
        }
        if (g_move_mode) { g_is_attracted = true; SetCapture(hwnd); }
        break;
    case WM_LBUTTONUP:
        if (g_is_attracted) {
            g_is_attracted = false; ReleaseCapture();
            OutputDebugStringW(L"[SpotifyOverlay] Window Drag Stopped\n");
            SaveConfig();
        }
        break;
    case WM_EXITSIZEMOVE: {
        RECT rc; GetWindowRect(hwnd, &rc);
        g_pos_x = (float)rc.left; g_pos_y = (float)rc.top; g_target_x = g_pos_x; g_target_y = g_pos_y;
        SaveConfig();
        break;
    }
    case WM_KEYDOWN:
        if (g_app_state == STATE_INPUT_ID || g_app_state == STATE_INPUT_SECRET) {
            if (wParam == VK_BACK && !g_input_buffer.empty()) g_input_buffer.pop_back();
            else if (wParam == 'V' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                if (OpenClipboard(hwnd)) {
                    HANDLE hData = GetClipboardData(CF_TEXT);
                    if (hData) { char* pszText = static_cast<char*>(GlobalLock(hData)); if (pszText) g_input_buffer += pszText; GlobalUnlock(hData); }
                    CloseClipboard();
                }
            }
            else if (wParam == VK_RETURN) {
                if (g_app_state == STATE_INPUT_ID) { CLIENT_ID = g_input_buffer; g_input_buffer = ""; g_app_state = STATE_INPUT_SECRET; g_error_message = ""; }
                else if (g_app_state == STATE_INPUT_SECRET) { CLIENT_SECRET = g_input_buffer; g_input_buffer = ""; SaveConfig(); g_app_state = STATE_RUNNING; g_error_message = "Connecting..."; }
            }
        }
        break;
    case WM_CHAR:
        if (g_app_state == STATE_INPUT_ID || g_app_state == STATE_INPUT_SECRET) {
            if (wParam >= 32 && wParam <= 126) g_input_buffer += static_cast<char>(wParam);
        }
        break;
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) { POINT pt; GetCursorPos(&pt); ShowCustomMenu(hwnd, pt.x, pt.y); }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_TRAY_TOGGLE_SHOW_LYRICS) {
            g_show_lyrics = !g_show_lyrics; SaveConfig();
            if (g_show_lyrics) {
                std::string track_name, all_artists;
                {
                    std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                    track_name = g_spotify_overlay_state.current_track.track_name;
                    all_artists = g_spotify_overlay_state.current_track.artist_name;
                }
                if (!track_name.empty()) {
                    std::wstring safeArtist = SanitizeFileName(Utf8ToWide(all_artists));
                    std::wstring safeTrack = SanitizeFileName(Utf8ToWide(track_name));
                    std::wstring filePath = g_lyrics_dir + L"\\" + safeArtist + L" - " + safeTrack + L".lrc";
                    std::ifstream ifs(filePath, std::ios::binary);
                    if (ifs.is_open()) {
                        std::string lrcText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                        ifs.close();
                        LyricsData localData; ParseLrc(lrcText, localData, track_name);
                        std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                        g_current_lyrics = localData; g_is_fetching_lyrics = false; g_needs_lyrics_fetch = false;
                    }
                    else {
                        bool is_unknown = false; std::ifstream unk_ifs(g_lyrics_dir + L"\\unknowns.txt");
                        if (unk_ifs.is_open()) {
                            std::string line; std::string targetLine = "Track: " + track_name + " | Artist: " + all_artists;
                            while (std::getline(unk_ifs, line)) { if (line.find(targetLine) != std::string::npos) { is_unknown = true; break; } }
                            unk_ifs.close();
                        }
                        if (!is_unknown) {
                            std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                            g_current_lyrics.has_lyrics = false; g_current_lyrics.lines.clear();
                            g_is_fetching_lyrics = true; g_needs_lyrics_fetch = true;
                        }
                    }
                }
            }
        }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_LYRICS_MGR) { LyricsManager::ToggleWindow(); }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_LYRICS) { g_is_3d_lyrics = !g_is_3d_lyrics; SaveConfig(); }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_MOVE_MODE) {
            g_move_mode = !g_move_mode; LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            if (g_move_mode) { SetWindowLong(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT); }
            else { g_is_attracted = false; ReleaseCapture(); SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT); }
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_HIDE_PAUSE) {
            g_hide_on_pause = !g_hide_on_pause;
            SaveConfig();

            s_lastTargetR = s_lastTargetG = s_lastTargetB = 0;
            s_glowCaches.clear();
            std::lock_guard<std::mutex> img_lock(g_image_mtx);
            g_image_updated = true;
        }
        else if (LOWORD(wParam) == ID_TRAY_RESET_POS) {
            g_pos_x = (float)g_init_pos_x; g_pos_y = (float)g_init_pos_y; g_target_x = (float)g_init_pos_x; g_target_y = (float)g_init_pos_y; g_is_moving = false;
            SetWindowPos(hwnd, HWND_TOPMOST, g_init_pos_x, g_init_pos_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            SaveConfig();
        }
        else if (LOWORD(wParam) == ID_TRAY_EXIT) { g_is_exiting = true; }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_USE_ACCENT) {
            g_use_accent_color = !g_use_accent_color; SaveConfig();

            s_lastTargetR = s_lastTargetG = s_lastTargetB = 0;
            s_glowCaches.clear();
            std::lock_guard<std::mutex> img_lock(g_image_mtx); g_image_updated = true;
        }
        else if (LOWORD(wParam) == ID_TRAY_TOGGLE_BLUR) {
            if (g_uiStyle == 0) {
                g_enable_blur = !g_enable_blur;
                SaveConfig();
            }
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid); PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA); nid.hWnd = hwnd; nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wcscpy_s(nid.szTip, L"Spotify Overlay");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowFirstRunNotification(HWND hwnd) {
    NOTIFYICONDATA nidNotify = { 0 }; nidNotify.cbSize = sizeof(NOTIFYICONDATA); nidNotify.hWnd = hwnd; nidNotify.uID = 1;
    nidNotify.uFlags = NIF_INFO; nidNotify.dwInfoFlags = NIIF_INFO;
    wcscpy_s(nidNotify.szInfoTitle, L"Spotify App Required");
    wcscpy_s(nidNotify.szInfo, L"please create an app at:\nhttps://developer.spotify.com/dashboard");
    Shell_NotifyIcon(NIM_MODIFY, &nidNotify);
}

struct FrameData {
    int currentOverlayH;
    int progress_ms;
    int duration_ms;
    bool hasLyrics;
    bool isFetching;
    float extensionRatio;
    float lyricsChangeProgress;
    bool isEcoDraw;
    std::vector<LyricDrawInfo> list2D;
    std::vector<LyricDrawInfo> list3D;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    timeBeginPeriod(1);
    InitPaths();
    LoadConfig();

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    DWORD mmcssTaskIndex = 0;
    HANDLE hMmTask = AvSetMmThreadCharacteristicsW(L"Games", &mmcssTaskIndex);

    GdiplusStartupInput gdiplusStartupInput; ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    PrivateFontCollection collTrack;  collTrack.AddFontFile(FontFileName);
    PrivateFontCollection collNormal; collNormal.AddFontFile(NormalFontFileName);
    PrivateFontCollection collNum;    collNum.AddFontFile(NumberFontFileName);
    PrivateFontCollection collEng;    collEng.AddFontFile(EnglishFontFileName);

    auto GetFamilyFromColl = [](PrivateFontCollection& coll) -> FontFamily* {
        int count = coll.GetFamilyCount();
        if (count > 0) {
            std::vector<FontFamily> families(count); int numFound = 0;
            coll.GetFamilies(count, families.data(), &numFound);
            if (numFound > 0) return families[0].Clone();
        }
        return new FontFamily(L"Segoe UI");
        };

    FontFamily* famTrack = GetFamilyFromColl(collTrack);
    FontFamily* famNormal = GetFamilyFromColl(collNormal);
    FontFamily* famNum = GetFamilyFromColl(collNum);
    FontFamily* famEng = GetFamilyFromColl(collEng);

    WNDCLASS wc = { 0 }; wc.lpfnWndProc = WndProc; wc.hInstance = hInstance; wc.lpszClassName = L"SpotifyModernOverlay"; RegisterClass(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN); int screenH = GetSystemMetrics(SM_CYSCREEN);
    int overlayW = OVERLAY_W; int overlayH = OVERLAY_H;
    int posX = screenW - overlayW - OVERLAY_MARGIN; int posY = (screenH - overlayH) / 2;

    HWND hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        L"SpotifyModernOverlay", L"Spotify Overlay", WS_POPUP, posX, posY, overlayW, overlayH, nullptr, nullptr, hInstance, nullptr);

    if (g_saved_pos_x != -9999 && g_saved_pos_y != -9999) {
        g_pos_x = (float)g_saved_pos_x; g_pos_y = (float)g_saved_pos_y;
        SetWindowPos(hwnd, HWND_TOPMOST, g_saved_pos_x, g_saved_pos_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else {
        g_pos_x = (float)posX; g_pos_y = (float)posY;
    }
    g_init_pos_x = posX; g_init_pos_y = posY;
    g_target_x = g_pos_x; g_target_y = g_pos_y;
    LyricsManager::Initialize(hInstance, hwnd, g_lyrics_dir);

    ShowWindow(hwnd, SW_SHOW); CreateTrayIcon(hwnd);

#if USE_MOCK_MODE == 0
    if (g_api_mode != 0 && (CLIENT_ID.empty() || CLIENT_SECRET.empty())) ShowFirstRunNotification(hwnd);
#endif

    std::thread bgWorker(SpotifyTrackerThread); bgWorker.detach();

    Animation smoothProgress; smoothProgress.setImmediate(0.0f); smoothProgress.speed = 0.08f;
    Animation albumScaleAnim; albumScaleAnim.setImmediate(1.0f); albumScaleAnim.speed = 0.02f;
    Animation textScaleAnim;  textScaleAnim.setImmediate(1.0f);  textScaleAnim.speed = 0.02f;
    Animation textAlphaAnim;  textAlphaAnim.setImmediate(1.0f);  textAlphaAnim.speed = 0.02f;
    Animation progressBarAnim;progressBarAnim.setImmediate(0.0f);progressBarAnim.speed = 0.015f;
    Animation overlayHeightAnim; overlayHeightAnim.setImmediate((float)OVERLAY_H); overlayHeightAnim.speed = 0.08f;
    Animation lyricsScrollAnim;  lyricsScrollAnim.setImmediate(0.0f); lyricsScrollAnim.speed = 0.04f;
    Animation fetchOutAnim; fetchOutAnim.setImmediate(1.0f); fetchOutAnim.speed = 0.06f;
    Animation modeAnim; modeAnim.setImmediate(g_is_3d_lyrics ? 1.0f : 0.0f); modeAnim.speed = 0.08f;
    Animation oldAlbumAlpha; oldAlbumAlpha.setImmediate(0.0f);
    Animation lyricsChangeAnim; lyricsChangeAnim.setImmediate(1.0f); lyricsChangeAnim.speed = 0.04f;
    Animation oldLyricsAlphaAnim; oldLyricsAlphaAnim.setImmediate(0.0f); oldLyricsAlphaAnim.speed = 0.04f;
    Animation colorTransAnim; colorTransAnim.setImmediate(1.0f); colorTransAnim.speed = 0.025f;

    std::vector<LrcLine> g_old_lyrics_lines;
    float g_old_lyrics_scroll = 0.0f;
    bool g_old_lyrics_has = false;
    bool g_old_lyrics_is_3d = false;
    Color g_old_txt_color(255, 255, 255), g_old_acc_color(29, 185, 84);
    std::wstring g_old_track_name = L"";

    RollingTimeRenderer progressTimer; RollingTimeRenderer durationTimer; RollingTimeRenderer sysClockTimer;
    SmoothColor accentColorAnim; accentColorAnim.setImmediate(29, 185, 84);
    SmoothColor bgColorAnim;     bgColorAnim.setImmediate(15, 15, 15);
    SmoothColor textColorAnim;  textColorAnim.setTarget(255, 255, 255);
    SmoothColor blurColorAnim;  blurColorAnim.setImmediate(255, 255, 255);
    float targetBrightness = 0.3f;

    RollingTextRenderer trackRoll; RollingTextRenderer artistRoll;
    Bitmap* g_album_art = nullptr; Bitmap* g_old_album_art = nullptr;

    const int TARGET_FPS = 120; const double MS_PER_FRAME = 1000.0 / TARGET_FPS;
    AppState lastState = STATE_INIT;
    MSG msg;

    auto UpdateScene = [&](FrameData& frame, float dt) {
        if (g_app_state != lastState) {
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            if (g_app_state == STATE_RUNNING) SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
            else { SetWindowLong(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT); SetForegroundWindow(hwnd); }
            lastState = g_app_state;
        }

        float targetMode = g_is_3d_lyrics ? 1.0f : 0.0f;
        if (modeAnim.target != targetMode) modeAnim.set(targetMode);
        modeAnim.update(dt);

        frame.progress_ms = 0;
        frame.duration_ms = 100000;

        {
            std::lock_guard<std::mutex> img_lock(g_image_mtx);
            if (g_image_updated) {
                if (!g_image_bytes.empty()) {
                    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, g_image_bytes.size());
                    if (hGlob) {
                        void* pData = GlobalLock(hGlob);
                        if (pData) {
                            memcpy(pData, g_image_bytes.data(), g_image_bytes.size());
                            GlobalUnlock(hGlob);
                            IStream* pStream = nullptr;
                            if (CreateStreamOnHGlobal(hGlob, TRUE, &pStream) == S_OK) {
                                Gdiplus::Bitmap* tempBmp = Gdiplus::Bitmap::FromStream(pStream);
                                if (tempBmp && tempBmp->GetLastStatus() == Gdiplus::Ok && tempBmp->GetWidth() > 0) {
                                    if (g_album_art) {
                                        if (g_old_album_art) delete g_old_album_art;
                                        g_old_album_art = g_album_art;
                                        oldAlbumAlpha.setImmediate(1.0f); oldAlbumAlpha.set(0.0f);
                                    }

                                    g_album_art = tempBmp->Clone(0, 0, tempBmp->GetWidth(), tempBmp->GetHeight(), PixelFormat32bppARGB);
                                }
                                if (tempBmp) delete tempBmp;
                                pStream->Release();
                            }
                        }
                        else GlobalFree(hGlob);
                    }
                }
                if (g_album_art) {
                    BYTE r = 29, g = 185, b = 84; float bright = 0.3f;
                    ExtractAccentColor(g_album_art, r, g, b, bright);

                    BYTE nextBg = (bright >= 0.5f) ? 240 : 15;
                    BYTE nextTxt = (bright >= 0.5f) ? 40 : 250;
                    BYTE nextBlur = (bright >= 0.5f) ? 0 : 255;
                    if (accentColorAnim.willChange(r, g, b) || bgColorAnim.willChange(nextBg, nextBg, nextBg)) {
                        accentColorAnim.snapshot();
                        bgColorAnim.snapshot();
                        textColorAnim.snapshot();
                        blurColorAnim.snapshot();
                        colorTransAnim.setImmediate(0.0f);
                        colorTransAnim.set(1.0f);
                    }
                    accentColorAnim.setTarget(r, g, b);
                    targetBrightness = bright;

                    CLSID pngClsid;
                    if (GetEncoderClsid(L"image/png", &pngClsid) != -1) {

                        g_album_art->Save(g_album_art_file.c_str(), &pngClsid, NULL);
                    }

                }
                g_image_updated = false;
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - g_spotify_overlay_state.last_update).count();
            frame.progress_ms = g_spotify_overlay_state.is_playing ? g_spotify_overlay_state.progress_ms + elapsed : g_spotify_overlay_state.progress_ms;
            frame.duration_ms = g_spotify_overlay_state.duration_ms > 0 ? g_spotify_overlay_state.duration_ms : 100000;

            frame.progress_ms = std::min(frame.progress_ms, frame.duration_ms);

            if (g_spotify_overlay_state.track_changed) {

                static std::string s_prev_track_name = "";
                bool is_minor_update = false;

                auto toLower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); }); return s; };
                std::string lt1 = toLower(s_prev_track_name);
                std::string lt2 = toLower(g_spotify_overlay_state.current_track.track_name);
                if (!lt1.empty() && !lt2.empty() && (lt1.find(lt2) != std::string::npos || lt2.find(lt1) != std::string::npos)) {
                    is_minor_update = true;
                }
                s_prev_track_name = g_spotify_overlay_state.current_track.track_name;

                s_lastTargetR = s_lastTargetG = s_lastTargetB = 0;
                s_glowCaches.clear();

                if (g_album_art) {
                    if (g_old_album_art) delete g_old_album_art;
                    g_old_album_art = g_album_art->Clone(0, 0, g_album_art->GetWidth(), g_album_art->GetHeight(), PixelFormat32bppARGB);
                    oldAlbumAlpha.setImmediate(1.0f); oldAlbumAlpha.set(0.0f);
                }
                else {
                    if (g_old_album_art) { delete g_old_album_art; g_old_album_art = nullptr; }
                    oldAlbumAlpha.setImmediate(0.0f);
                }
                albumScaleAnim.setImmediate(0.9f); albumScaleAnim.set(1.0f);
                trackRoll.setText(Utf8ToWide(g_spotify_overlay_state.current_track.track_name));
                artistRoll.setText(Utf8ToWide(g_spotify_overlay_state.current_track.artist_name));

                if (!is_minor_update) {
                    progressBarAnim.set(0.0f);
                    lyricsScrollAnim.setImmediate(0.0f);

                    bool fetchingNow = false;
                    {
                        std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                        fetchingNow = g_is_fetching_lyrics;
                    }
                    fetchOutAnim.setImmediate(fetchingNow ? 0.0f : 1.0f);
                }
                g_spotify_overlay_state.track_changed = false;
            }
        }

        std::vector<LrcLine> currentLines;
        {
            std::lock_guard<std::mutex> lock(g_lyrics_mtx);
            frame.hasLyrics = g_current_lyrics.has_lyrics && g_show_lyrics;
            if (frame.hasLyrics) currentLines = g_current_lyrics.lines;
            frame.isFetching = g_is_fetching_lyrics && g_show_lyrics;
        }

        if (frame.isFetching) fetchOutAnim.setImmediate(0.0f);
        else if (fetchOutAnim.target == 0.0f) fetchOutAnim.set(1.0f);
        fetchOutAnim.update(dt);

        float targetH = (float)OVERLAY_H;
        if (g_show_lyrics) {
            if (frame.hasLyrics) targetH = (float)OVERLAY_H + 100.0f;
            else if (frame.isFetching || fetchOutAnim.current < 0.99f) targetH = (float)OVERLAY_H + 35.0f;
        }
        overlayHeightAnim.set(targetH);
        overlayHeightAnim.update(dt);
        frame.currentOverlayH = static_cast<int>(overlayHeightAnim.current);

        int currentLrcIndex = 0;
        if (frame.hasLyrics && !currentLines.empty()) {
            for (int i = 0; i < (int)currentLines.size(); ++i) {
                if (frame.progress_ms >= currentLines[i].time_ms) currentLrcIndex = i;
                else break;
            }
        }

        static std::string lastRawTrackName = "";
        static std::wstring cachedTrackNameW = L"";
        std::string rawTrackName;
        {
            std::lock_guard<std::mutex> spot_lock(g_spotify_overlay_state.mtx);
            rawTrackName = g_spotify_overlay_state.current_track.track_name;
        }
        if (rawTrackName != lastRawTrackName) {
            cachedTrackNameW = Utf8ToWide(rawTrackName);
            lastRawTrackName = rawTrackName;
        }
        std::wstring currentTrackName = cachedTrackNameW;
        static std::wstring lastTrackName = L"";

        BYTE nextBg = (targetBrightness >= 0.5f) ? 240 : 15;
        BYTE nextTxt = (targetBrightness >= 0.5f) ? 40 : 250;
        BYTE nextBlur = (targetBrightness >= 0.5f) ? 0 : 255;

        if (bgColorAnim.willChange(nextBg, nextBg, nextBg)) {
            accentColorAnim.snapshot();
            bgColorAnim.snapshot();
            textColorAnim.snapshot();
            blurColorAnim.snapshot();
            colorTransAnim.setImmediate(0.0f);
            colorTransAnim.set(1.0f);
        }

        bgColorAnim.setTarget(nextBg, nextBg, nextBg);
        textColorAnim.setTarget(nextTxt, nextTxt, nextTxt);
        blurColorAnim.setTarget(nextBlur, nextBlur, nextBlur);

        accentColorAnim.update(dt);
        bgColorAnim.update(dt);
        textColorAnim.update(dt);
        blurColorAnim.update(dt);
        colorTransAnim.update(dt);

        Gdiplus::Color txt = textColorAnim.getColor();
        Gdiplus::Color acc = accentColorAnim.getColor();
        Gdiplus::Color bg = bgColorAnim.getColor();

        BYTE tgtR = static_cast<BYTE>(accentColorAnim.tr);
        BYTE tgtG = static_cast<BYTE>(accentColorAnim.tg);
        BYTE tgtB = static_cast<BYTE>(accentColorAnim.tb);

        D2D1_COLOR_F themeBg = D2D1::ColorF(bg.GetR() / 255.0f, bg.GetG() / 255.0f, bg.GetB() / 255.0f, 0.95f);
        D2D1_COLOR_F themeTxt = D2D1::ColorF(txt.GetR() / 255.0f, txt.GetG() / 255.0f, txt.GetB() / 255.0f, 1.0f);
        D2D1_COLOR_F themeAcc = D2D1::ColorF(acc.GetR() / 255.0f, acc.GetG() / 255.0f, acc.GetB() / 255.0f, 1.0f);
        LyricsManager::UpdateTheme(themeBg, themeTxt, themeAcc);
        MyOverlayMenu::UpdateTheme(themeBg, themeTxt, themeAcc);

        static std::vector<LrcLine> lastFrameLines;
        static float lastFrameScroll = 0.0f;
        static bool lastFrameHasLyrics = false;
        static bool lastFrameIs3D = false;
        static Gdiplus::Color lastFrameTxtColor(255, 255, 255), lastFrameAccColor(29, 185, 84);

        static bool lastShowLyrics = true;
        if (g_show_lyrics != lastShowLyrics) {
            if (!g_show_lyrics) {
                if (lastFrameHasLyrics && !lastFrameLines.empty()) {
                    g_old_lyrics_lines = lastFrameLines;
                    g_old_lyrics_scroll = lastFrameScroll;
                    g_old_lyrics_has = true;
                    g_old_lyrics_is_3d = lastFrameIs3D;
                    g_old_txt_color = lastFrameTxtColor;
                    g_old_acc_color = lastFrameAccColor;
                    g_old_track_name = lastTrackName;
                    oldLyricsAlphaAnim.setImmediate(1.0f);
                    oldLyricsAlphaAnim.set(0.0f);
                }
            }
            lastShowLyrics = g_show_lyrics;
        }

        if (currentTrackName != lastTrackName) {
            if (lastFrameHasLyrics && !lastFrameLines.empty()) {
                g_old_lyrics_lines = lastFrameLines;
                g_old_lyrics_scroll = lastFrameScroll;
                g_old_lyrics_has = true;
                g_old_lyrics_is_3d = lastFrameIs3D;
                g_old_txt_color = lastFrameTxtColor;
                g_old_acc_color = lastFrameAccColor;
                g_old_track_name = lastTrackName;
                oldLyricsAlphaAnim.setImmediate(1.0f);
                oldLyricsAlphaAnim.set(0.0f);
            }
            else {
                g_old_lyrics_has = false;
                oldLyricsAlphaAnim.setImmediate(0.0f);
            }

            lyricsChangeAnim.setImmediate(0.0f);
            lyricsChangeAnim.set(1.0f);
            lyricsScrollAnim.setImmediate((float)currentLrcIndex);
            lastTrackName = currentTrackName;
        }
        else {
            lyricsScrollAnim.set((float)currentLrcIndex);
        }

        lyricsScrollAnim.update(dt); oldLyricsAlphaAnim.update(dt); lyricsChangeAnim.update(dt);
        frame.lyricsChangeProgress = lyricsChangeAnim.current;
        albumScaleAnim.update(dt); textScaleAnim.update(dt); textAlphaAnim.update(dt); oldAlbumAlpha.update(dt);

        smoothProgress.set((float)frame.progress_ms);
        smoothProgress.update(dt);
        int displayProgressMs = (int)smoothProgress.current;

        float targetPct = std::clamp((float)displayProgressMs / (float)frame.duration_ms, 0.0f, 1.0f);
        progressBarAnim.set(targetPct); progressBarAnim.update(dt);

        progressTimer.update(displayProgressMs / 1000, dt);
        durationTimer.update(frame.duration_ms / 1000, dt);

        auto now = std::chrono::system_clock::now(); auto in_time_t = std::chrono::system_clock::to_time_t(now);
        struct tm sys_tm; localtime_s(&sys_tm, &in_time_t);
        sysClockTimer.update(sys_tm.tm_hour * 60 + sys_tm.tm_min, dt);

        trackRoll.update(dt); artistRoll.update(dt);

        if (g_app_state == STATE_RUNNING) {
            if (g_is_attracted) {
                POINT mousePt; GetCursorPos(&mousePt);
                float t_x = (float)(mousePt.x - overlayW / 2);
                float t_y = (float)(mousePt.y - frame.currentOverlayH / 2);

                HMONITOR hMonitor = MonitorFromPoint(mousePt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(MONITORINFO) };
                if (GetMonitorInfo(hMonitor, &mi)) {
                    if (t_x < mi.rcWork.left) t_x = (float)mi.rcWork.left;
                    if (t_x + overlayW > mi.rcWork.right) t_x = (float)(mi.rcWork.right - overlayW);
                    if (t_y < mi.rcWork.top) t_y = (float)mi.rcWork.top;
                    if (t_y + frame.currentOverlayH > mi.rcWork.bottom) t_y = (float)(mi.rcWork.bottom - frame.currentOverlayH);
                }
                g_target_x = t_x; g_target_y = t_y; g_is_moving = true;
            }

            if (g_is_moving) {

                float factor = 1.0f - std::exp(-0.07f * 60.0f * dt);
                g_pos_x += (g_target_x - g_pos_x) * factor;
                g_pos_y += (g_target_y - g_pos_y) * factor;
                SetWindowPos(hwnd, HWND_TOPMOST, (int)g_pos_x, (int)g_pos_y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

                if (std::abs(g_target_x - g_pos_x) < 0.5f && std::abs(g_target_y - g_pos_y) < 0.5f) {
                    g_pos_x = g_target_x; g_pos_y = g_target_y; g_is_moving = false; SaveConfig();
                }
            }
        }

        bool isLyricsActive = frame.hasLyrics || (g_old_lyrics_has && oldLyricsAlphaAnim.current > 0.01f);
        float maxTargetAddH = isLyricsActive ? 100.0f : 35.0f;
        float extensionHeight = (float)frame.currentOverlayH - (float)OVERLAY_H;
        frame.extensionRatio = std::clamp(extensionHeight / maxTargetAddH, 0.0f, 1.0f);

        frame.list2D.clear(); frame.list3D.clear();

        float targetSweepStart = 0.0f;
        float targetSweepEnd = 1.0f;

        if (s_render_glow_mode != 0 && frame.hasLyrics && !currentLines.empty()) {
            int i = currentLrcIndex;
            int lineStartMs = currentLines[i].time_ms;
            int lineNextMs = (i + 1 < (int)currentLines.size()) ? currentLines[i + 1].time_ms : (lineStartMs + 4000);
            int lineDuration = std::max(400, lineNextMs - lineStartMs);
            int elapsedLineMs = std::max(0, frame.progress_ms - lineStartMs);
            float lineProgress = std::clamp((float)elapsedLineMs / (float)lineDuration, 0.0f, 1.0f);

            if (s_render_glow_mode == 1) {
                float inFixedDuration = 1000.0f;
                float tIn = std::clamp((float)elapsedLineMs / inFixedDuration, 0.0f, 1.0f);
                targetSweepEnd = EaseOut(tIn);
                targetSweepStart = (lineProgress < 0.85f) ? 0.0f : EaseIn((lineProgress - 0.85f) / 0.15f);
            }
            else if (s_render_glow_mode == 2) {
                if (lineProgress <= 0.20f) {
                    targetSweepEnd = EaseOutQuint(lineProgress / 0.20f);
                    targetSweepStart = 0.0f;
                }
                else if (lineProgress >= 0.50f) {
                    targetSweepEnd = 1.0f;
                    targetSweepStart = EaseIn((lineProgress - 0.50f) / 0.50f);
                }
                else {
                    targetSweepEnd = 1.0f;
                    targetSweepStart = 0.0f;
                }
            }
            if (targetSweepStart >= targetSweepEnd) targetSweepStart = targetSweepEnd;
        }

        float globalTargetStart = (float)currentLrcIndex + targetSweepStart;
        float globalTargetEnd = (float)currentLrcIndex + targetSweepEnd;

        if (rawTrackName != s_lastSweepTrack || currentLines.empty()) {
            s_globalSweepStart = globalTargetStart;
            s_globalSweepEnd = globalTargetEnd;
            s_lastSweepTrack = rawTrackName;
        }
        else {
            float sweepFactor = 1.0f - std::exp(-0.12f * 60.0f * dt);
            s_globalSweepStart += (globalTargetStart - s_globalSweepStart) * sweepFactor;
            s_globalSweepEnd += (globalTargetEnd - s_globalSweepEnd) * sweepFactor;
        }

        if (s_render_glow_mode != g_lyrics_glow_mode) {
            bool canSwitch = false;
            if (s_render_glow_mode == 0 || !frame.hasLyrics || currentLines.empty()) {
                canSwitch = true;
            }
            else {
                if (targetSweepEnd >= 0.999f) canSwitch = true;
            }
            if (canSwitch) s_render_glow_mode = g_lyrics_glow_mode;
        }

        if (frame.currentOverlayH > OVERLAY_H + 5) {
            float lyricsAreaY = OVERLAY_H - 2.0f + 5.0f;
            float lyricsAreaH = frame.currentOverlayH - lyricsAreaY - 2.0f;
            float centerY = lyricsAreaY + (lyricsAreaH / 2.0f) + 4.0f;

            Graphics tempG(hwnd);
            Font tempFont(famNormal, 16.0f, FontStyleBold, UnitPixel);

            if (oldLyricsAlphaAnim.current > 0.01f && g_old_lyrics_has && !g_old_lyrics_lines.empty()) {
                auto generateOldLyricsList = [&](std::vector<LyricDrawInfo>& list, bool is3D) {
                    if (is3D) {
                        float cylinderRadius = lyricsAreaH / 2.0f - 2.0f;
                        float cameraZ = cylinderRadius * 2.8f;
                        for (int i = 0; i < (int)g_old_lyrics_lines.size(); ++i) {
                            float diff = (float)i - g_old_lyrics_scroll; const float PI = 3.14159265f; float theta = diff * (PI / 6.0f);
                            float rotOffset = -(1.0f - oldLyricsAlphaAnim.current) * 1.2f; theta += rotOffset;
                            if (std::abs(theta) >= PI / 2.0f) continue;
                            float y3d = cylinderRadius * std::sin(theta); float z3d = cylinderRadius * std::cos(theta);
                            float distance = cameraZ + cylinderRadius - z3d; float perspectiveScale = cameraZ / distance;
                            float targetY = centerY + (y3d * perspectiveScale); float scaleX = perspectiveScale * 1.0f; float scaleY = scaleX * std::cos(theta);
                            float alpha = std::clamp(((z3d + cylinderRadius) / (cylinderRadius * 2.0f) - 0.5f) / 0.5f, 0.0f, 1.0f);
                            alpha *= oldLyricsAlphaAnim.current; if (alpha <= 0.01f) continue;
                            RectF boundingBox; tempG.MeasureString(g_old_lyrics_lines[i].text.c_str(), -1, &tempFont, PointF(0, 0), &boundingBox);
                            float textWidth = boundingBox.Width * scaleX; float maxAvailableWidth = (float)overlayW - 120.0f;
                            if (textWidth > maxAvailableWidth) { float shrink = maxAvailableWidth / textWidth; scaleX *= shrink; scaleY *= shrink; }
                            float glowIntensity = std::max(0.0f, (std::cos(theta) - 0.86f) / 0.14f);
                            float coreRatio = 0.7f;
                            BYTE tr = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetR() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tg = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetG() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tb = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetB() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE r = static_cast<BYTE>(std::clamp(g_old_txt_color.GetR() * (1.0f - glowIntensity) + tr * glowIntensity, 0.0f, 255.0f));
                            BYTE g_c = static_cast<BYTE>(std::clamp(g_old_txt_color.GetG() * (1.0f - glowIntensity) + tg * glowIntensity, 0.0f, 255.0f));
                            BYTE b = static_cast<BYTE>(std::clamp(g_old_txt_color.GetB() * (1.0f - glowIntensity) + tb * glowIntensity, 0.0f, 255.0f));

                            list.push_back({ g_old_track_name, g_old_lyrics_lines[i].text, targetY, scaleX, scaleY, alpha, z3d, r, g_c, b, g_old_acc_color.GetR(), g_old_acc_color.GetG(), g_old_acc_color.GetB(), glowIntensity, 0.0f, 1.0f });
                        }
                    }
                    else {
                        for (int i = 0; i < (int)g_old_lyrics_lines.size(); ++i) {
                            float diff = (float)i - g_old_lyrics_scroll; if (std::abs(diff) > 2.5f) continue;
                            float targetY = centerY + (diff * 32.0f); float scale = 0.85f + std::max(0.0f, 1.0f - std::abs(diff)) * 0.15f;
                            float exitProgress = 1.0f - oldLyricsAlphaAnim.current; scale *= (1.0f + 0.3f * exitProgress);
                            float alpha = std::clamp(1.0f - std::abs(diff) * 0.5f, 0.0f, 1.0f); alpha *= oldLyricsAlphaAnim.current; if (alpha <= 0.01f) continue;
                            float glowIntensity = std::max(0.0f, 1.0f - std::abs(diff));
                            float coreRatio = 0.7f;
                            BYTE tr = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetR() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tg = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetG() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tb = static_cast<BYTE>(std::clamp(255.0f * coreRatio + g_old_acc_color.GetB() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE r = static_cast<BYTE>(std::clamp(g_old_txt_color.GetR() * (1.0f - glowIntensity) + tr * glowIntensity, 0.0f, 255.0f));
                            BYTE g_c = static_cast<BYTE>(std::clamp(g_old_txt_color.GetG() * (1.0f - glowIntensity) + tg * glowIntensity, 0.0f, 255.0f));
                            BYTE b = static_cast<BYTE>(std::clamp(g_old_txt_color.GetB() * (1.0f - glowIntensity) + tb * glowIntensity, 0.0f, 255.0f));

                            list.push_back({ g_old_track_name, g_old_lyrics_lines[i].text, targetY, scale, scale, alpha, 0.0f, r, g_c, b, g_old_acc_color.GetR(), g_old_acc_color.GetG(), g_old_acc_color.GetB(), glowIntensity, 0.0f, 1.0f });
                        }
                    }
                    };
                if (modeAnim.current < 0.999f) generateOldLyricsList(frame.list2D, false);
                if (modeAnim.current > 0.001f) generateOldLyricsList(frame.list3D, true);
            }

            if (frame.hasLyrics) {
                auto generateLyricsList = [&](std::vector<LyricDrawInfo>& list, bool is3D) {
                    if (is3D) {
                        float cylinderRadius = lyricsAreaH / 2.0f - 2.0f;
                        float cameraZ = cylinderRadius * 2.8f;
                        for (int i = 0; i < (int)currentLines.size(); ++i) {
                            float diff = (float)i - lyricsScrollAnim.current; const float PI = 3.14159265f; float theta = diff * (PI / 6.0f);
                            float rotOffset = (1.0f - frame.lyricsChangeProgress) * 1.2f; theta += rotOffset;
                            if (std::abs(theta) >= PI / 2.0f) continue;
                            float y3d = cylinderRadius * std::sin(theta); float z3d = cylinderRadius * std::cos(theta);
                            float distance = cameraZ + cylinderRadius - z3d; float perspectiveScale = cameraZ / distance;
                            float targetY = centerY + (y3d * perspectiveScale); float scaleX = perspectiveScale * 1.0f; float scaleY = scaleX * std::cos(theta);
                            float alpha = std::clamp(((z3d + cylinderRadius) / (cylinderRadius * 2.0f) - 0.5f) / 0.5f, 0.0f, 1.0f);
                            alpha *= frame.lyricsChangeProgress; if (alpha <= 0.01f) continue;
                            RectF boundingBox; tempG.MeasureString(currentLines[i].text.c_str(), -1, &tempFont, PointF(0, 0), &boundingBox);
                            float textWidth = boundingBox.Width * scaleX; float maxAvailableWidth = (float)overlayW - 120.0f;
                            if (textWidth > maxAvailableWidth) { float shrink = maxAvailableWidth / textWidth; scaleX *= shrink; scaleY *= shrink; }
                            float glowIntensity = std::max(0.0f, (std::cos(theta) - 0.86f) / 0.14f);
                            float coreRatio = 0.7f;
                            BYTE tr = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetR() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tg = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetG() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tb = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetB() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE r = static_cast<BYTE>(std::clamp(txt.GetR() * (1.0f - glowIntensity) + tr * glowIntensity, 0.0f, 255.0f));
                            BYTE g_c = static_cast<BYTE>(std::clamp(txt.GetG() * (1.0f - glowIntensity) + tg * glowIntensity, 0.0f, 255.0f));
                            BYTE b = static_cast<BYTE>(std::clamp(txt.GetB() * (1.0f - glowIntensity) + tb * glowIntensity, 0.0f, 255.0f));

                            float sweepStart = 0.0f;
                            float sweepEnd = 1.0f;

                            if (s_render_glow_mode != 0) {
                                sweepStart = std::clamp(s_globalSweepStart - (float)i, 0.0f, 1.0f);
                                sweepEnd = std::clamp(s_globalSweepEnd - (float)i, 0.0f, 1.0f);
                                if (sweepStart >= sweepEnd) sweepStart = sweepEnd;
                            }

                            list.push_back({ currentTrackName, currentLines[i].text, targetY, scaleX, scaleY, alpha, z3d, r, g_c, b, tgtR, tgtG, tgtB, glowIntensity, sweepStart, sweepEnd });
                        }
                    }
                    else {
                        for (int i = 0; i < (int)currentLines.size(); ++i) {
                            float diff = (float)i - lyricsScrollAnim.current; if (std::abs(diff) > 2.5f) continue;
                            float targetY = centerY + (diff * 32.0f); float scale = 0.85f + std::max(0.0f, 1.0f - std::abs(diff)) * 0.15f;
                            scale *= (0.6f + 0.4f * frame.lyricsChangeProgress);
                            RectF boundingBox; tempG.MeasureString(currentLines[i].text.c_str(), -1, &tempFont, PointF(0, 0), &boundingBox);
                            float textWidth = boundingBox.Width * scale; float maxAvailableWidth = (float)overlayW - 60.0f;
                            if (textWidth > maxAvailableWidth) scale *= (maxAvailableWidth / textWidth);
                            float alpha = std::clamp(1.0f - std::abs(diff) * 0.5f, 0.0f, 1.0f); alpha *= frame.lyricsChangeProgress; if (alpha <= 0.01f) continue;
                            float glowIntensity = std::max(0.0f, 1.0f - std::abs(diff));
                            float coreRatio = 0.7f;
                            BYTE tr = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetR() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tg = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetG() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE tb = static_cast<BYTE>(std::clamp(255.0f * coreRatio + acc.GetB() * (1.0f - coreRatio), 0.0f, 255.0f));
                            BYTE r = static_cast<BYTE>(std::clamp(txt.GetR() * (1.0f - glowIntensity) + tr * glowIntensity, 0.0f, 255.0f));
                            BYTE g_c = static_cast<BYTE>(std::clamp(txt.GetG() * (1.0f - glowIntensity) + tg * glowIntensity, 0.0f, 255.0f));
                            BYTE b = static_cast<BYTE>(std::clamp(txt.GetB() * (1.0f - glowIntensity) + tb * glowIntensity, 0.0f, 255.0f));

                            float sweepStart = 0.0f;
                            float sweepEnd = 1.0f;

                            if (s_render_glow_mode != 0) {
                                sweepStart = std::clamp(s_globalSweepStart - (float)i, 0.0f, 1.0f);
                                sweepEnd = std::clamp(s_globalSweepEnd - (float)i, 0.0f, 1.0f);
                                if (sweepStart >= sweepEnd) sweepStart = sweepEnd;
                            }

                            list.push_back({ currentTrackName, currentLines[i].text, targetY, scale, scale, alpha, 0.0f, r, g_c, b, tgtR, tgtG, tgtB, glowIntensity, sweepStart, sweepEnd });
                        }
                    }
                    };
                if (modeAnim.current < 0.999f) generateLyricsList(frame.list2D, false);
                if (modeAnim.current > 0.001f) generateLyricsList(frame.list3D, true);
            }
            std::sort(frame.list3D.begin(), frame.list3D.end(), [](const LyricDrawInfo& a, const LyricDrawInfo& b) { return a.z3d < b.z3d; });
        }

        lastFrameLines = currentLines;
        lastFrameScroll = lyricsScrollAnim.current;
        lastFrameHasLyrics = frame.hasLyrics;
        lastFrameIs3D = g_is_3d_lyrics;
        lastFrameTxtColor = txt;
        lastFrameAccColor = acc;
        };

    using Microsoft::WRL::ComPtr;
    ComPtr<ID2D1Factory> pD2DFactory;
    ComPtr<IDWriteFactory> pDWriteFactory;
    ComPtr<ID2D1DCRenderTarget> pDCRT;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pD2DFactory.GetAddressOf());
    if (FAILED(hr)) { OutputDebugStringW(L"[Direct2D] Failed to create Factory\n"); return 0; }

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(pDWriteFactory.GetAddressOf()));
    if (FAILED(hr)) { OutputDebugStringW(L"[DirectWrite] Failed to create Factory\n"); return 0; }

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0.0f, 0.0f,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
        D2D1_FEATURE_LEVEL_DEFAULT
    );

    hr = pD2DFactory->CreateDCRenderTarget(&props, pDCRT.GetAddressOf());
    if (FAILED(hr)) { OutputDebugStringW(L"[Direct2D] Failed to create DCRenderTarget\n"); return 0; }

    Microsoft::WRL::ComPtr<IDWriteTextFormat> g_pFontTrack, g_pFontArtist, g_pFontTime, g_pFontClock, g_pFontNormal, g_pFontLyrics;
    pDWriteFactory->CreateTextFormat(L"Yu Gothic", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.5f, L"ja-jp", &g_pFontTrack);
    pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"ja-jp", &g_pFontArtist);
    pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 9.0f, L"ja-jp", &g_pFontTime);
    pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 9.0f, L"ja-jp", &g_pFontClock);
    pDWriteFactory->CreateTextFormat(L"Yu Gothic", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 9.0f, L"ja-jp", &g_pFontNormal);
    pDWriteFactory->CreateTextFormat(L"Yu Gothic", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"ja-jp", &g_pFontLyrics);

    g_pFontTrack->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    g_pFontArtist->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    DWRITE_TRIMMING dwriteTrimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
    g_pFontTrack->SetTrimming(&dwriteTrimming, NULL);
    g_pFontArtist->SetTrimming(&dwriteTrimming, NULL);

    g_pFontLyrics->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    g_pFontLyrics->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    g_pFontLyrics->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    Microsoft::WRL::ComPtr<IDWriteTextFormat> g_pFontHeaderClean, g_pFontTimeClean;
    pDWriteFactory->CreateTextFormat(L"Yu Gothic", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.5f, L"ja-jp", &g_pFontHeaderClean);
    pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &g_pFontTimeClean);
    g_pFontHeaderClean->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_pFontHeaderClean->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    g_pFontTimeClean->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    auto GetOrCreateLyricGlowBitmap = [&](ID2D1DCRenderTarget* target, const std::wstring& trackName, const std::wstring& text, BYTE gr, BYTE gg, BYTE gb) -> ID2D1Bitmap* {
        for (auto& item : s_glowCaches) {
            if (item->trackName == trackName && item->text == text && item->gr == gr && item->gg == gg && item->gb == gb) {
                return item->pBitmap.Get();
            }
        }

        if (s_glowCaches.size() > 100) {
            s_glowCaches.erase(s_glowCaches.begin(), s_glowCaches.begin() + 30);
        }

        auto newItem = std::make_shared<LyricGlowCacheItem>();
        newItem->trackName = trackName;
        newItem->text = text;
        newItem->gr = gr;
        newItem->gg = gg;
        newItem->gb = gb;

        float cacheW = 800.0f;
        float cacheH = 100.0f;
        D2D1_SIZE_F rtSize = D2D1::SizeF(cacheW, cacheH);

        HRESULT hrBmp = target->CreateCompatibleRenderTarget(rtSize, newItem->pBmpRT.GetAddressOf());

        if (SUCCEEDED(hrBmp) && newItem->pBmpRT) {
            Microsoft::WRL::ComPtr<ID2D1DeviceContext> pContext;
            newItem->pBmpRT.As(&pContext);

            if (pContext) {

                newItem->pBmpRT->BeginDraw();
                newItem->pBmpRT->Clear(D2D1::ColorF(0, 0, 0, 0));

                Microsoft::WRL::ComPtr<IDWriteTextLayout> pLayout;
                pDWriteFactory->CreateTextLayout(text.c_str(), (UINT32)text.length(), g_pFontLyrics.Get(), cacheW, cacheH, &pLayout);
                if (pLayout) {
                    pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    pLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

                    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> baseBrush;
                    newItem->pBmpRT->CreateSolidColorBrush(D2D1::ColorF(gr / 255.0f, gg / 255.0f, gb / 255.0f, 0.9f), &baseBrush);
                    newItem->pBmpRT->DrawTextLayout(D2D1::Point2F(0, 0), pLayout.Get(), baseBrush.Get());
                }
                newItem->pBmpRT->EndDraw();

                Microsoft::WRL::ComPtr<ID2D1Bitmap> pInputBmp;
                newItem->pBmpRT->GetBitmap(&pInputBmp);

                Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pFinalRT;
                target->CreateCompatibleRenderTarget(rtSize, &pFinalRT);

                Microsoft::WRL::ComPtr<ID2D1DeviceContext> pFinalContext;
                pFinalRT.As(&pFinalContext);

                if (pFinalContext) {

                    Microsoft::WRL::ComPtr<ID2D1Effect> blur1, blur2, blur3;
                    pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur1);
                    pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur2);
                    pFinalContext->CreateEffect(CLSID_D2D1GaussianBlur, &blur3);

                    blur1->SetInput(0, pInputBmp.Get());
                    blur1->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 2.0f);

                    blur2->SetInput(0, pInputBmp.Get());
                    blur2->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 8.0f);

                    blur3->SetInput(0, pInputBmp.Get());
                    blur3->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 24.0f);

                    Microsoft::WRL::ComPtr<ID2D1Effect> comp1, comp2;
                    pFinalContext->CreateEffect(CLSID_D2D1Composite, &comp1);
                    pFinalContext->CreateEffect(CLSID_D2D1Composite, &comp2);

                    comp1->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_PLUS);
                    comp2->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_PLUS);

                    comp1->SetInputEffect(0, blur1.Get());
                    comp1->SetInputEffect(1, blur2.Get());

                    comp2->SetInputEffect(0, comp1.Get());
                    comp2->SetInputEffect(1, blur3.Get());

                    pFinalRT->BeginDraw();
                    pFinalRT->Clear(D2D1::ColorF(0, 0, 0, 0));
                    pFinalContext->DrawImage(comp2.Get());
                    pFinalRT->EndDraw();

                    pFinalRT->GetBitmap(newItem->pBitmap.GetAddressOf());
                    newItem->pBmpRT = pFinalRT;
                }
            }
            else {

                newItem->pBmpRT->BeginDraw();
                newItem->pBmpRT->Clear(D2D1::ColorF(0, 0, 0, 0));
                Microsoft::WRL::ComPtr<IDWriteTextLayout> pLayout;
                pDWriteFactory->CreateTextLayout(text.c_str(), (UINT32)text.length(), g_pFontLyrics.Get(), cacheW, cacheH, &pLayout);
                if (pLayout) {
                    pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    pLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

                    int layers = 4;
                    for (int i = layers; i >= 1; --i) {
                        float r = (float)i * 3.5f;
                        float gAlpha = (1.0f - (float)i / (layers + 1)) * 0.08f;
                        if (gAlpha <= 0.002f) continue;

                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> gBrush;
                        newItem->pBmpRT->CreateSolidColorBrush(D2D1::ColorF(gr / 255.0f, gg / 255.0f, gb / 255.0f, gAlpha), &gBrush);

                        int numDirs = 8 + (i * 2);
                        for (int d = 0; d < numDirs; ++d) {
                            float angle = (3.14159265f * 2.0f * d) / numDirs;
                            newItem->pBmpRT->DrawTextLayout(D2D1::Point2F(std::cos(angle) * r, std::sin(angle) * r), pLayout.Get(), gBrush.Get());
                        }
                    }
                }
                newItem->pBmpRT->EndDraw();
                newItem->pBmpRT->GetBitmap(newItem->pBitmap.GetAddressOf());
            }

            ID2D1Bitmap* pResult = newItem->pBitmap.Get();
            s_glowCaches.push_back(newItem);
            return pResult;
        }
        return nullptr;
        };

        auto IsSpatialColorTransActive = [&]() -> bool {
            return (g_color_trans_mode == 1 || g_color_trans_mode == 2) && (colorTransAnim.current < 0.999f);
            };

        auto CreateColorTransMaskBrush = [&](ID2D1DCRenderTarget* target, float w, float h, ID2D1Brush** outBrush) {
            const float feather = 0.35f;
            float p = EaseOut(colorTransAnim.current) * (1.0f + feather);
            float stop1 = std::clamp(p - feather, 0.0f, 1.0f);
            float stop2 = std::clamp(p, 0.0001f, 1.0f);
            if (stop1 >= stop2) stop1 = std::max(0.0f, stop2 - 0.0001f);

            D2D1_GRADIENT_STOP stops[3] = {
                { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                { stop1, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                { stop2, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
            };
            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pStops;
            target->CreateGradientStopCollection(stops, 3, &pStops);

            float cx = w / 2.0f; float cy = h / 2.0f;
            if (g_color_trans_mode == 1) {
                float maxR = std::hypot(cx, cy);
                Microsoft::WRL::ComPtr<ID2D1RadialGradientBrush> pRad;
                target->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy), D2D1::Point2F(0, 0), maxR, maxR),
                    pStops.Get(), &pRad);
                *outBrush = pRad.Detach();
            }
            else {
                float rad = g_color_trans_rotation * 3.14159265f / 180.0f;
                float dx = std::cos(rad); float dy = std::sin(rad);
                float halfLen = std::max(1.0f, cx * std::abs(dx) + cy * std::abs(dy));
                D2D1_POINT_2F startPt = D2D1::Point2F(cx - dx * halfLen, cy - dy * halfLen);
                D2D1_POINT_2F endPt = D2D1::Point2F(cx + dx * halfLen, cy + dy * halfLen);
                Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> pLin;
                target->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(startPt, endPt),
                    pStops.Get(), &pLin);
                *outBrush = pLin.Detach();
            }
            };

        auto CreateColorTransBrush = [&](ID2D1DCRenderTarget* target, const SmoothColor& sc, BYTE alpha, float w, float h, ID2D1Brush** outBrush) {
            auto toD2D = [](const Gdiplus::Color& c) { return D2D1::ColorF(c.GetR() / 255.0f, c.GetG() / 255.0f, c.GetB() / 255.0f, c.GetA() / 255.0f); };
            if (!IsSpatialColorTransActive()) {
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pSolid;
                target->CreateSolidColorBrush(toD2D(sc.getColor(alpha)), &pSolid);
                *outBrush = pSolid.Detach();
                return;
            }
            const float feather = 0.35f;
            float p = EaseOut(colorTransAnim.current) * (1.0f + feather);
            float stop1 = std::clamp(p - feather, 0.0f, 1.0f);
            float stop2 = std::clamp(p, 0.0001f, 1.0f);
            if (stop1 >= stop2) stop1 = std::max(0.0f, stop2 - 0.0001f);

            D2D1_COLOR_F newCol = toD2D(sc.getTargetColor(alpha));
            D2D1_COLOR_F oldCol = toD2D(sc.getOldColor(alpha));
            D2D1_GRADIENT_STOP stops[3] = { { 0.0f, newCol }, { stop1, newCol }, { stop2, oldCol } };
            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pStops;
            target->CreateGradientStopCollection(stops, 3, &pStops);

            float cx = w / 2.0f; float cy = h / 2.0f;
            if (g_color_trans_mode == 1) {
                float maxR = std::hypot(cx, cy);
                Microsoft::WRL::ComPtr<ID2D1RadialGradientBrush> pRad;
                target->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy), D2D1::Point2F(0, 0), maxR, maxR),
                    pStops.Get(), &pRad);
                *outBrush = pRad.Detach();
            }
            else {
                float rad = g_color_trans_rotation * 3.14159265f / 180.0f;
                float dx = std::cos(rad); float dy = std::sin(rad);
                float halfLen = std::max(1.0f, cx * std::abs(dx) + cy * std::abs(dy));
                Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> pLin;
                target->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(cx - dx * halfLen, cy - dy * halfLen), D2D1::Point2F(cx + dx * halfLen, cy + dy * halfLen)),
                    pStops.Get(), &pLin);
                *outBrush = pLin.Detach();
            }
            };

    auto RenderScene1 = [&](ID2D1DCRenderTarget* pDCRT, IDWriteFactory* pDWriteFactory, const FrameData& frame) {
        float barGlowBrightness = 0.35f;
        float lineGlowBrightness = 1.0f;
        float albumBlurSizePulse = 0.2f;
        float albumBlurOpacity = 0.5f;

        pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        auto GetD2DColor = [](const Gdiplus::Color& c, BYTE alpha = 255) { return D2D1::ColorF(c.GetR() / 255.0f, c.GetG() / 255.0f, c.GetB() / 255.0f, (c.GetA() == 255 ? alpha : c.GetA()) / 255.0f); };
        auto GetD2DColorFromRGB = [](BYTE r, BYTE g, BYTE b, BYTE a = 255) { return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); };

        auto CreateD2DBitmap = [](ID2D1DCRenderTarget* target, Gdiplus::Bitmap* gdiBmp, ID2D1Bitmap** d2dBmp) -> HRESULT {
            if (!gdiBmp) return E_POINTER;
            UINT w = gdiBmp->GetWidth(); UINT h = gdiBmp->GetHeight();
            Gdiplus::Rect rect(0, 0, w, h); Gdiplus::BitmapData bmpData;
            if (gdiBmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData) != Gdiplus::Ok) return E_FAIL;
            D2D1_SIZE_U size = D2D1::SizeU(w, h);
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            HRESULT hr = target->CreateBitmap(size, bmpData.Scan0, bmpData.Stride, &props, d2dBmp);
            gdiBmp->UnlockBits(&bmpData); return hr;
            };

        auto DrawRollingText = [&](RollingTextRenderer& roller, IDWriteTextFormat* pFormat, ID2D1Brush* pBrush, const D2D1_RECT_F& rect) {
            if (roller.rollAnim.current >= 0.999f) { pDCRT->DrawText(roller.targetText.c_str(), (UINT32)roller.targetText.length(), pFormat, &rect, pBrush); return; }
            pDCRT->PushAxisAlignedClip(&rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            float h = rect.bottom - rect.top; float yOffset = roller.rollAnim.current * h;
            D2D1_RECT_F oldRect = D2D1::RectF(rect.left, rect.top - yOffset, rect.right, rect.bottom - yOffset);
            pDCRT->DrawText(roller.currentText.c_str(), (UINT32)roller.currentText.length(), pFormat, &oldRect, pBrush);
            D2D1_RECT_F newRect = D2D1::RectF(rect.left, rect.top + (h - yOffset), rect.right, rect.bottom + (h - yOffset));
            pDCRT->DrawText(roller.targetText.c_str(), (UINT32)roller.targetText.length(), pFormat, &newRect, pBrush);
            pDCRT->PopAxisAlignedClip();
            };

        auto DrawRollingTime = [&](RollingTimeRenderer& timer, IDWriteTextFormat* pFormat, ID2D1Brush* pBrush, float startX, float startY, float digitW, float digitH, bool rightAlign = false) {
            float totalW = 4.5f * digitW; float x = rightAlign ? (startX - totalW) : startX;
            auto renderDigit = [&](RollingDigitAnimation& anim, float dx, int base) {
                float val = anim.current; int num1 = static_cast<int>(floor(val)) % base; if (num1 < 0) num1 += base; int num2 = (num1 + 1) % base; float off = val - floor(val);
                D2D1_RECT_F digitRect = D2D1::RectF(dx, startY, dx + digitW, startY + digitH); pDCRT->PushAxisAlignedClip(&digitRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                std::wstring n1 = std::to_wstring(num1); std::wstring n2 = std::to_wstring(num2);
                D2D1_RECT_F r1 = D2D1::RectF(dx, startY - off * digitH, dx + digitW, startY + digitH - off * digitH); pDCRT->DrawText(n1.c_str(), (UINT32)n1.length(), pFormat, &r1, pBrush);
                D2D1_RECT_F r2 = D2D1::RectF(dx, startY + digitH - off * digitH, dx + digitW, startY + digitH * 2.0f - off * digitH); pDCRT->DrawText(n2.c_str(), (UINT32)n2.length(), pFormat, &r2, pBrush);
                pDCRT->PopAxisAlignedClip();
                };
            renderDigit(timer.m1, x, 10); renderDigit(timer.m2, x + digitW, 10);
            D2D1_RECT_F colonRect = D2D1::RectF(x + digitW * 2.0f, startY, x + digitW * 2.5f, startY + digitH); pDCRT->DrawText(L":", 1, pFormat, &colonRect, pBrush);
            renderDigit(timer.s1, x + digitW * 2.5f, 6); renderDigit(timer.s2, x + digitW * 3.5f, 10);
            };

        float cardRadius = 18.0f; D2D1_RECT_F cardRect = D2D1::RectF(1.0f, 1.0f, (float)OVERLAY_W - 1.0f, (float)frame.currentOverlayH - 1.0f);
        D2D1_ROUNDED_RECT roundedCard = D2D1::RoundedRect(cardRect, cardRadius, cardRadius);

        float imgSize = 80.0f; float imgX = 20.0f; float imgY = (OVERLAY_H - imgSize) / 2.0f; float imgRadius = 10.0f;
        D2D1_RECT_F imgRect = D2D1::RectF(imgX, imgY, imgX + imgSize, imgY + imgSize); D2D1_ROUNDED_RECT imgRoundedRect = D2D1::RoundedRect(imgRect, imgRadius, imgRadius);

        float glowPulse = (sinf((float)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) * 3.14159265f / 1750.0f) + 1.0f) / 2.0f;

        float colorTintFactor = g_enable_blur ? 0.65f : 1.0f;
        BYTE bgAlpha = static_cast<BYTE>(std::clamp(220.0f * g_bg_alpha, 0.0f, 255.0f));
        BYTE glowAlpha = static_cast<BYTE>(std::clamp((40.0f + glowPulse * 55.0f) * g_bg_alpha, 0.0f, 255.0f));if (g_enable_blur && g_overlay_alpha > 0.01f) {
            static HDC s_hdcBlurCap = nullptr;
            static HBITMAP s_hBmpBlurCap = nullptr;
            static HBITMAP s_hOldBlurCap = nullptr;
            static void* s_pBlurBits = nullptr;
            static int s_blurCapW = 0, s_blurCapH = 0;

            const int pad = 32;
            int capW = OVERLAY_W + pad * 2;
            int capH = std::max(1, frame.currentOverlayH) + pad * 2;

            if (!s_hdcBlurCap) {
                HDC hdcScr = GetDC(nullptr);
                s_hdcBlurCap = CreateCompatibleDC(hdcScr);
                ReleaseDC(nullptr, hdcScr);
            }
            if (s_blurCapW != capW || s_blurCapH != capH || !s_hBmpBlurCap) {
                if (s_hBmpBlurCap) {
                    SelectObject(s_hdcBlurCap, s_hOldBlurCap);
                    DeleteObject(s_hBmpBlurCap);
                }
                BITMAPINFO bmi = { 0 };
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = capW;
                bmi.bmiHeader.biHeight = -capH;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                s_hBmpBlurCap = CreateDIBSection(s_hdcBlurCap, &bmi, DIB_RGB_COLORS, &s_pBlurBits, NULL, 0);
                s_hOldBlurCap = (HBITMAP)SelectObject(s_hdcBlurCap, s_hBmpBlurCap);
                s_blurCapW = capW;
                s_blurCapH = capH;
            }

            if (s_hBmpBlurCap && s_pBlurBits) {
                HDC hdcScr = GetDC(nullptr);
                int winX = (int)g_pos_x - pad;
                int winY = (int)g_pos_y + (int)g_overlay_offset_y - pad;
                BitBlt(s_hdcBlurCap, 0, 0, capW, capH, hdcScr, winX, winY, SRCCOPY);
                ReleaseDC(nullptr, hdcScr);

                D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
                Microsoft::WRL::ComPtr<ID2D1Bitmap> pBgScreenBmp;
                if (SUCCEEDED(pDCRT->CreateBitmap(D2D1::SizeU(capW, capH), s_pBlurBits, capW * 4, &bmpProps, &pBgScreenBmp))) {
                    Microsoft::WRL::ComPtr<ID2D1DeviceContext> pDC;
                    pDCRT->QueryInterface(IID_PPV_ARGS(&pDC));
                    if (pDC) {
                        Microsoft::WRL::ComPtr<ID2D1Effect> blurEffect;
                        if (SUCCEEDED(pDC->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect))) {
                            blurEffect->SetInput(0, pBgScreenBmp.Get());
                            blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 14.0f * g_blur_strength);
                            blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);

                            Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> pCardGeom;
                            pD2DFactory->CreateRoundedRectangleGeometry(roundedCard, &pCardGeom);
                            Microsoft::WRL::ComPtr<ID2D1Layer> pBlurClipLayer;
                            if (SUCCEEDED(pDCRT->CreateLayer(NULL, &pBlurClipLayer))) {

                                D2D1_LAYER_PARAMETERS blurLp = D2D1::LayerParameters(
                                    cardRect, pCardGeom.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                    D2D1::IdentityMatrix(), 1.0f);
                                pDCRT->PushLayer(&blurLp, pBlurClipLayer.Get());

                                D2D1_POINT_2F targetOffset = D2D1::Point2F(0.0f, 0.0f);
                                D2D1_RECT_F srcRect = D2D1::RectF((float)pad, (float)pad, (float)(pad + OVERLAY_W), (float)(pad + frame.currentOverlayH));
                                pDC->DrawImage(blurEffect.Get(), &targetOffset, &srcRect, D2D1_INTERPOLATION_MODE_LINEAR);

                                pDCRT->PopLayer();
                            }
                        }
                    }
                }
            }
        }

        Microsoft::WRL::ComPtr<ID2D1Brush> cardBgBrush;
        CreateColorTransBrush(pDCRT, bgColorAnim, bgAlpha, (float)OVERLAY_W, (float)frame.currentOverlayH, &cardBgBrush);
        if (cardBgBrush) {
            pDCRT->FillRoundedRectangle(&roundedCard, cardBgBrush.Get());
        }

        auto DrawAccentGlow = [&](const Gdiplus::Color& accCol) {
            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pGlowStops;
            D2D1_GRADIENT_STOP glowStops[3] = { { 0.0f, D2D1::ColorF(0,0,0,0) }, { 0.4f, D2D1::ColorF(0,0,0,0) }, { 1.0f, GetD2DColor(accCol) } };
            pDCRT->CreateGradientStopCollection(glowStops, 3, &pGlowStops);
            Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> accentGlowBrush;
            pDCRT->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, (float)frame.currentOverlayH)), pGlowStops.Get(), &accentGlowBrush);
            pDCRT->FillRoundedRectangle(&roundedCard, accentGlowBrush.Get());
            };

        if (!IsSpatialColorTransActive()) {
            DrawAccentGlow(accentColorAnim.getColor(glowAlpha));
        }
        else {

            DrawAccentGlow(accentColorAnim.getOldColor(static_cast<BYTE>(glowAlpha * (1.0f - colorTransAnim.current))));
            Microsoft::WRL::ComPtr<ID2D1Brush> pTransMask;
            CreateColorTransMaskBrush(pDCRT, (float)OVERLAY_W, (float)frame.currentOverlayH, &pTransMask);
            Microsoft::WRL::ComPtr<ID2D1Layer> pTransLayer;
            if (pTransMask && SUCCEEDED(pDCRT->CreateLayer(NULL, &pTransLayer))) {
                D2D1_LAYER_PARAMETERS lp = D2D1::LayerParameters();
                lp.contentBounds = cardRect;
                lp.opacityBrush = pTransMask.Get();
                pDCRT->PushLayer(&lp, pTransLayer.Get());
                DrawAccentGlow(accentColorAnim.getTargetColor(glowAlpha));
                pDCRT->PopLayer();
            }
        }

        Microsoft::WRL::ComPtr<ID2D1Brush> borderBrush;
        CreateColorTransBrush(pDCRT, accentColorAnim, static_cast<BYTE>(90 * g_bg_alpha), (float)OVERLAY_W, (float)frame.currentOverlayH, &borderBrush);
        if (borderBrush) {
            pDCRT->DrawRoundedRectangle(&roundedCard, borderBrush.Get(), 1.0f);
        }

        float shadowSizePulse = glowPulse * 8.0f * albumBlurSizePulse;
        for (int i = 8; i >= 1; --i) {
            float offset = (float)i * 1.5f + shadowSizePulse;
            D2D1_ROUNDED_RECT shadowRect = D2D1::RoundedRect(D2D1::RectF(imgX - offset, imgY - offset, imgX + imgSize + offset, imgY + imgSize + offset), imgRadius + offset, imgRadius + offset);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> sBrush; float baseAlpha = (10.0f - (float)i) * 0.015f * albumBlurOpacity * g_album_alpha;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(blurColorAnim.r / 255.0f, blurColorAnim.g / 255.0f, blurColorAnim.b / 255.0f, baseAlpha), &sBrush);
            pDCRT->FillRoundedRectangle(&shadowRect, sBrush.Get());
        }
        for (int i = 8; i >= 1; --i) {
            float offset = (float)i * 1.5f + shadowSizePulse; D2D1_ROUNDED_RECT shadowRect = D2D1::RoundedRect(D2D1::RectF(imgX - offset, imgY - offset, imgX + imgSize + offset, imgY + imgSize + offset), imgRadius + offset, imgRadius + offset);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> sBrush; float baseAlpha = (10.0f - (float)i) * 0.015f * albumBlurOpacity * g_album_alpha;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(blurColorAnim.r / 255.0f, blurColorAnim.g / 255.0f, blurColorAnim.b / 255.0f, baseAlpha), &sBrush);
            pDCRT->FillRoundedRectangle(&shadowRect, sBrush.Get());
        }

        D2D1_MATRIX_3X2_F orgTransform; pDCRT->GetTransform(&orgTransform);
        float imgCX = imgX + imgSize / 2.0f; float imgCY = imgY + imgSize / 2.0f;
        D2D1_MATRIX_3X2_F imgScale = D2D1::Matrix3x2F::Scale(albumScaleAnim.current, albumScaleAnim.current, D2D1::Point2F(imgCX, imgCY));
        pDCRT->SetTransform(imgScale * orgTransform);

        auto DrawRoundedImage = [&](Gdiplus::Bitmap* gdiBmp, float opacity) {
            if (!gdiBmp || g_album_alpha <= 0.001f) return;
            Microsoft::WRL::ComPtr<ID2D1Bitmap> pD2DBmp;
            if (SUCCEEDED(CreateD2DBitmap(pDCRT, gdiBmp, &pD2DBmp))) {
                Microsoft::WRL::ComPtr<ID2D1BitmapBrush> pBmpBrush; D2D1_BITMAP_BRUSH_PROPERTIES bmpBrushProps = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP);
                pDCRT->CreateBitmapBrush(pD2DBmp.Get(), bmpBrushProps, &pBmpBrush);
                D2D1_MATRIX_3X2_F brushMat = D2D1::Matrix3x2F::Scale(imgSize / (float)gdiBmp->GetWidth(), imgSize / (float)gdiBmp->GetHeight()) * D2D1::Matrix3x2F::Translation(imgX, imgY);
                pBmpBrush->SetTransform(brushMat); pBmpBrush->SetOpacity(opacity * g_album_alpha); pDCRT->FillRoundedRectangle(&imgRoundedRect, pBmpBrush.Get());
            }
            };

        if (g_old_album_art) DrawRoundedImage(g_old_album_art, oldAlbumAlpha.current);
        if (g_album_art) DrawRoundedImage(g_album_art, 1.0f - oldAlbumAlpha.current);
        pDCRT->SetTransform(orgTransform);

        float textX = imgX + imgSize + 16.0f; float textW = overlayW - textX - 20.0f; float textCX = textX + textW / 2.0f; float textCY = 45.0f;
        D2D1_MATRIX_3X2_F textScale = D2D1::Matrix3x2F::Scale(textScaleAnim.current, textScaleAnim.current, D2D1::Point2F(textCX, textCY));
        pDCRT->SetTransform(textScale * orgTransform);

        float subBaseAlpha = 200.0f + (1.0f - g_bg_alpha) * 50.0f;
        BYTE alphaText = static_cast<BYTE>(std::clamp(textAlphaAnim.current * 255.0f, 0.0f, 255.0f));
        BYTE alphaSub = static_cast<BYTE>(std::clamp(textAlphaAnim.current * subBaseAlpha, 0.0f, 255.0f));

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush, artistBrush, timerBrush, sysTimeBrush;
        Microsoft::WRL::ComPtr<ID2D1Brush> accentBrush;
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaText), alphaText), &textBrush);
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &artistBrush);
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &timerBrush);
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &sysTimeBrush);
        CreateColorTransBrush(pDCRT, accentColorAnim, 255, (float)OVERLAY_W, (float)frame.currentOverlayH, &accentBrush);

        D2D1_RECT_F trackRect = D2D1::RectF(textX, 20.0f, textX + textW - 55.0f, 42.0f); D2D1_RECT_F artistRect = D2D1::RectF(textX, 44.0f, textX + textW, 62.0f);

        if (g_debug_status != L"OK" && g_debug_status != L"OK (MOCK MODE)") {
            DrawRollingText(trackRoll, g_pFontTrack.Get(), textBrush.Get(), trackRect);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> debugBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.4f, 0.4f, 1.0f), &debugBrush);
            pDCRT->DrawText(g_debug_status.c_str(), (UINT32)g_debug_status.length(), g_pFontNormal.Get(), &artistRect, debugBrush.Get());
        }
        else {
            DrawRollingText(trackRoll, g_pFontTrack.Get(), textBrush.Get(), trackRect);
            DrawRollingText(artistRoll, g_pFontArtist.Get(), artistBrush.Get(), artistRect);
        }
        pDCRT->SetTransform(orgTransform);

        g_pFontClock->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        DrawRollingTime(sysClockTimer, g_pFontClock.Get(), sysTimeBrush.Get(), (float)overlayW - 20.0f, 20.0f, 8.0f, 14.0f, true);

        float barX = textX; float barY = 74.0f; float barW = (float)overlayW - barX - 20.0f; float barH = 4.0f; float barRadius = 2.0f;
        D2D1_ROUNDED_RECT railRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + barW, barY + barH), barRadius, barRadius);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> railBrush; pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(80), 80), &railBrush);
        pDCRT->FillRoundedRectangle(&railRect, railBrush.Get());

        float activeBarW = barW * progressBarAnim.current;
        if (activeBarW > 0) {
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> activeGlowBrush;
            for (int i = 6; i >= 1; --i) {
                float offset = (float)i * 1.5f; D2D1_ROUNDED_RECT gRect = D2D1::RoundedRect(D2D1::RectF(barX - offset, barY - offset, barX + activeBarW + offset, barY + barH + offset), barRadius + offset, barRadius + offset);
                float alphaFactor = (7.0f - (float)i) * 0.05f * barGlowBrightness;
                pDCRT->CreateSolidColorBrush(GetD2DColor(accentColorAnim.getColor(), static_cast<BYTE>(std::clamp(alphaFactor * 255.0f, 0.0f, 255.0f))), &activeGlowBrush);
                pDCRT->FillRoundedRectangle(&gRect, activeGlowBrush.Get());
            }
            D2D1_ROUNDED_RECT activeRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + activeBarW, barY + barH), barRadius, barRadius);
            pDCRT->FillRoundedRectangle(&activeRect, accentBrush.Get());
        }

        g_pFontTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        DrawRollingTime(progressTimer, g_pFontTime.Get(), timerBrush.Get(), barX, barY + 6.0f, 7.5f, 14.0f);
        g_pFontTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        DrawRollingTime(durationTimer, g_pFontTime.Get(), timerBrush.Get(), barX + barW, barY + 6.0f, 7.5f, 14.0f, true);

        if (fetchOutAnim.current < 0.99f) {
            float currentScale = 1.0f + fetchOutAnim.current * 0.4f; BYTE alpha = static_cast<BYTE>(std::clamp(180.0f * (1.0f - fetchOutAnim.current), 0.0f, 255.0f));
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fetchBrush; pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alpha)), &fetchBrush);
            D2D1_RECT_F fetchRect = D2D1::RectF(0.0f, (float)OVERLAY_H + 4.0f, (float)overlayW, (float)OVERLAY_H + 24.0f);
            float cx = fetchRect.left + (fetchRect.right - fetchRect.left) / 2.0f; float cy = fetchRect.top + (fetchRect.bottom - fetchRect.top) / 2.0f;
            D2D1_MATRIX_3X2_F fetchMat = D2D1::Matrix3x2F::Scale(currentScale, currentScale, D2D1::Point2F(cx, cy)); pDCRT->SetTransform(fetchMat * orgTransform);
            g_pFontNormal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pDCRT->DrawText(L"Fetching lyrics...", 18, g_pFontNormal.Get(), fetchRect, fetchBrush.Get());
            pDCRT->SetTransform(orgTransform);
        }

        bool isLineVisible = frame.hasLyrics || (g_old_lyrics_has && oldLyricsAlphaAnim.current > 0.01f);
        if (isLineVisible && frame.extensionRatio > 0.01f && fetchOutAnim.current > 0.01f) {
            float lineW = (overlayW - 60.0f) * frame.extensionRatio * fetchOutAnim.current;
            float lineH_thick = 3.0f; float lineX_pos = (overlayW - lineW) / 2.0f; float lineY_pos = OVERLAY_H - 2.0f; float lineRad = 1.5f;
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> lineGlowBrush;
            for (int i = 6; i >= 1; --i) {
                float offset = (float)i * 1.5f; D2D1_ROUNDED_RECT gLineRect = D2D1::RoundedRect(D2D1::RectF(lineX_pos - offset, lineY_pos - offset, lineX_pos + lineW + offset, lineY_pos + lineH_thick + offset), lineRad + offset, lineRad + offset);
                float baseAlpha = std::max(0.0f, (15.0f - (float)i * 2.0f) / 255.0f); float finalAlpha = baseAlpha * frame.extensionRatio * fetchOutAnim.current * lineGlowBrightness;
                pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(), static_cast<BYTE>(std::clamp(finalAlpha * 255.0f, 0.0f, 255.0f))), &lineGlowBrush);
                pDCRT->FillRoundedRectangle(&gLineRect, lineGlowBrush.Get());
            }
            D2D1_ROUNDED_RECT coreLine = D2D1::RoundedRect(D2D1::RectF(lineX_pos, lineY_pos, lineX_pos + lineW, lineY_pos + lineH_thick), lineRad, lineRad);
            BYTE lineC = static_cast<BYTE>((textColorAnim.getColor().GetR() > 128) ? 200 : 50); Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> lineBrush;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(lineC / 255.0f, lineC / 255.0f, lineC / 255.0f, 140.0f * frame.extensionRatio * fetchOutAnim.current / 255.0f), &lineBrush);
            pDCRT->FillRoundedRectangle(&coreLine, lineBrush.Get());
        }

        auto DrawD2DList = [&](const std::vector<LyricDrawInfo>& list, float offsetX, float clipY, float clipH) {
            D2D1_RECT_F clipRect = D2D1::RectF(0.0f, clipY, (float)OVERLAY_W, clipY + clipH);
            pDCRT->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            Microsoft::WRL::ComPtr<ID2D1Layer> pSweepLayer;
            pDCRT->CreateLayer(NULL, &pSweepLayer);

            for (const auto& item : list) {
                if (item.alpha <= 0.01f) continue;
                float cx = (float)OVERLAY_W / 2.0f;
                D2D1_MATRIX_3X2_F itemMat = D2D1::Matrix3x2F::Scale(item.scaleX, item.scaleY, D2D1::Point2F(cx, item.targetY)) * D2D1::Matrix3x2F::Translation(offsetX, 0.0f) * orgTransform;
                pDCRT->SetTransform(itemMat);

                D2D1_RECT_F textRect = D2D1::RectF(10.0f, item.targetY - 15.0f, (float)OVERLAY_W - 10.0f, item.targetY + 15.0f);

                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> dimBrush;
                pDCRT->CreateSolidColorBrush(D2D1::ColorF(textColorAnim.getColor().GetR() / 255.0f, textColorAnim.getColor().GetG() / 255.0f, textColorAnim.getColor().GetB() / 255.0f, item.alpha * 0.35f), &dimBrush);
                pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, dimBrush.Get());

                if (s_render_glow_mode == 0) {
                    if (item.glowIntensity > 0.01f) {
                        float glowAlpha = std::clamp(item.alpha * item.glowIntensity, 0.0f, 1.0f);
                        ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                        if (pGlowBmp) {
                            float cacheW = 800.0f; float cacheH = 100.0f;
                            D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                            pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }

                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                        pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, item.alpha * item.glowIntensity), &brightWhiteBrush);
                        pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());
                    }
                }
                else {
                    if ((item.sweepEnd - item.sweepStart) > 0.001f && pSweepLayer) {

                        float invScale = 1.0f / item.scaleX;
                        float localW = OVERLAY_W * invScale;
                        float textLeft = cx - (localW / 2.0f) - (120.0f * invScale);
                        float textRight = cx + (localW / 2.0f) + (120.0f * invScale);
                        float totalW = textRight - textLeft;

                        float feather = 80.0f * invScale;
                        float xStart = textLeft + totalW * item.sweepStart - feather;
                        float xEnd = textLeft + totalW * item.sweepEnd + feather;

                        if (xEnd > xStart) {
                            float sweepLen = xEnd - xStart;
                            float fRatio = std::clamp(feather / sweepLen, 0.01f, 0.49f);

                            D2D1_GRADIENT_STOP stops[4] = {
                                { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) },
                                { fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f - fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
                            };

                            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pStops;
                            pDCRT->CreateGradientStopCollection(stops, 4, &pStops);

                            Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> pMaskBrush;
                            pDCRT->CreateLinearGradientBrush(
                                D2D1::LinearGradientBrushProperties(D2D1::Point2F(xStart, item.targetY), D2D1::Point2F(xEnd, item.targetY)),
                                pStops.Get(), &pMaskBrush
                            );

                            D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                            layerParams.contentBounds = D2D1::InfiniteRect();
                            layerParams.opacityBrush = pMaskBrush.Get();

                            pDCRT->PushLayer(&layerParams, pSweepLayer.Get());

                            ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                            if (pGlowBmp) {
                                float glowAlpha = std::clamp(item.alpha * item.glowIntensity, 0.0f, 1.0f);
                                float cacheW = 800.0f; float cacheH = 100.0f;
                                D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                                pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                            }

                            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                            pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, item.alpha), &brightWhiteBrush);
                            pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());

                            pDCRT->PopLayer();
                        }
                    }
                }

                pDCRT->SetTransform(orgTransform);
            }
            pDCRT->PopAxisAlignedClip();
            };

        if (frame.currentOverlayH > OVERLAY_H + 5) {
            float lyricsAreaY = OVERLAY_H - 2.0f + 5.0f; float lyricsAreaH = frame.currentOverlayH - lyricsAreaY - 2.0f; float modeVal = modeAnim.current;
            if (modeVal < 0.999f) DrawD2DList(frame.list2D, -(modeVal * overlayW), lyricsAreaY, lyricsAreaH);
            if (modeVal > 0.001f) DrawD2DList(frame.list3D, (1.0f - modeVal) * overlayW, lyricsAreaY, lyricsAreaH);
        }
        };

    auto RenderScene2 = [&](ID2D1DCRenderTarget* pDCRT, IDWriteFactory* pDWriteFactory, const FrameData& frame) {
        pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        float globalScale = 0.85f;
        D2D1_MATRIX_3X2_F orgTransform = D2D1::Matrix3x2F::Scale(globalScale, globalScale, D2D1::Point2F((float)OVERLAY_W / 2.0f, (float)OVERLAY_H / 2.0f));

        Gdiplus::Color txtCol = textColorAnim.getColor();
        float txtR = txtCol.GetR() / 255.0f; float txtG = txtCol.GetG() / 255.0f; float txtB = txtCol.GetB() / 255.0f;

        Gdiplus::Color accGdi = accentColorAnim.getColor();
        float accR = accGdi.GetR() / 255.0f; float accG = accGdi.GetG() / 255.0f; float accB = accGdi.GetB() / 255.0f;

        bool isTextBlack = (txtCol.GetR() < 100 && txtCol.GetG() < 100 && txtCol.GetB() < 100);
        float shadowR = isTextBlack ? 1.0f : 0.0f;
        float shadowG = isTextBlack ? 1.0f : 0.0f;
        float shadowB = isTextBlack ? 1.0f : 0.0f;
        BYTE shadowByte = isTextBlack ? 255 : 0;

        float uiShadowAlpha = isTextBlack ? 0.35f : 0.8f;
        float uiGlowAlpha = isTextBlack ? 0.5f : 1.0f;
        float lrcShadowMult = isTextBlack ? 0.5f : 1.5f;
        float lrcGlowMult = isTextBlack ? 0.6f : 1.0f;

        auto measureScale = [&](const std::wstring& text, IDWriteTextFormat* format) {
            Microsoft::WRL::ComPtr<IDWriteTextLayout> pLayout; pDWriteFactory->CreateTextLayout(text.c_str(), (UINT32)text.length(), format, 10000.0f, 100.0f, &pLayout);
            if (!pLayout) return 1.0f; DWRITE_TEXT_METRICS metrics; pLayout->GetMetrics(&metrics);
            float maxW = (float)OVERLAY_W - 20.0f;
            return (metrics.width > maxW) ? (maxW / metrics.width) : 1.0f;
            };

        auto DrawAllUI = [&](ID2D1RenderTarget* rt, D2D1_COLOR_F baseColor) {
            auto DrawFadingRollingText = [&](const std::wstring& currentText, const std::wstring& targetText, float animProgress, IDWriteTextFormat* format, const D2D1_RECT_F& rect, float baseAlpha) {
                if (baseAlpha <= 0.01f) return;
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * baseAlpha), &brush);
                if (animProgress >= 0.999f) { rt->DrawText(targetText.c_str(), (UINT32)targetText.length(), format, &rect, brush.Get()); return; }

                float slideDist = (rect.bottom - rect.top) * 1.0f; float oldAlpha = baseAlpha * (1.0f - animProgress);
                if (oldAlpha > 0.01f) {
                    float oldY = animProgress * slideDist; D2D1_RECT_F oldRect = D2D1::RectF(rect.left, rect.top - oldY, rect.right, rect.bottom - oldY);
                    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> oBrush; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * oldAlpha), &oBrush);
                    rt->DrawText(currentText.c_str(), (UINT32)currentText.length(), format, &oldRect, oBrush.Get());
                }
                float newAlpha = baseAlpha * animProgress;
                if (newAlpha > 0.01f) {
                    float newY = (1.0f - animProgress) * slideDist; D2D1_RECT_F newRect = D2D1::RectF(rect.left, rect.top + newY, rect.right, rect.bottom + newY);
                    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> nBrush; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * newAlpha), &nBrush);
                    rt->DrawText(targetText.c_str(), (UINT32)targetText.length(), format, &newRect, nBrush.Get());
                }
                };

            auto DrawFadingRollingTime = [&](RollingTimeRenderer& timer, IDWriteTextFormat* format, float startX, float startY, float digitW, float digitH, float alpha, bool rightAlign = false) {
                float totalW = 4.5f * digitW; float x = rightAlign ? (startX - totalW) : startX;
                auto renderDigit = [&](RollingDigitAnimation& anim, float dx, int base) {
                    float val = anim.current; int num1 = static_cast<int>(floor(val)) % base; if (num1 < 0) num1 += base; int num2 = (num1 + 1) % base; float off = val - floor(val);
                    std::wstring n1 = std::to_wstring(num1); std::wstring n2 = std::to_wstring(num2);
                    g_pFontTimeClean->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); float slideDist = digitH * 1.0f;
                    float alpha1 = alpha * (1.0f - off);
                    if (alpha1 > 0.01f) {
                        D2D1_RECT_F r1 = D2D1::RectF(dx, startY - off * slideDist, dx + digitW, startY + digitH - off * slideDist);
                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> b1; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * alpha1), &b1);
                        rt->DrawText(n1.c_str(), (UINT32)n1.length(), format, &r1, b1.Get());
                    }
                    float alpha2 = alpha * off;
                    if (alpha2 > 0.01f) {
                        D2D1_RECT_F r2 = D2D1::RectF(dx, startY + slideDist - off * slideDist, dx + digitW, startY + digitH + slideDist - off * slideDist);
                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> b2; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * alpha2), &b2);
                        rt->DrawText(n2.c_str(), (UINT32)n2.length(), format, &r2, b2.Get());
                    }
                    };

                rt->PushAxisAlignedClip(D2D1::RectF(x - 30.0f, startY, x + totalW + 30.0f, startY + digitH), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                renderDigit(timer.m1, x, 10); renderDigit(timer.m2, x + digitW, 10);
                g_pFontTimeClean->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                D2D1_RECT_F colonRect = D2D1::RectF(x + digitW * 2.0f, startY, x + digitW * 2.5f, startY + digitH);
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bC; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * alpha), &bC);
                rt->DrawText(L":", 1, format, &colonRect, bC.Get());
                renderDigit(timer.s1, x + digitW * 2.5f, 6); renderDigit(timer.s2, x + digitW * 3.5f, 10);
                rt->PopAxisAlignedClip();
                };

            float cx = (float)OVERLAY_W / 2.0f;
            std::wstring curTrackArtist = trackRoll.currentText + L" - " + artistRoll.currentText;
            std::wstring tgtTrackArtist = trackRoll.targetText + L" - " + artistRoll.targetText;

            D2D1_RECT_F headerRect = D2D1::RectF(0.0f, 20.0f, (float)OVERLAY_W, 50.0f);
            float headerScale = measureScale(tgtTrackArtist, g_pFontHeaderClean.Get()) * textScaleAnim.current;

            rt->SetTransform(D2D1::Matrix3x2F::Scale(headerScale, headerScale, D2D1::Point2F(cx, 35.0f)) * orgTransform);
            DrawFadingRollingText(curTrackArtist, tgtTrackArtist, trackRoll.rollAnim.current, g_pFontHeaderClean.Get(), headerRect, textAlphaAnim.current);

            rt->SetTransform(D2D1::Matrix3x2F::Scale(textScaleAnim.current, textScaleAnim.current, D2D1::Point2F(cx, 65.0f)) * orgTransform);
            float timeY = 55.0f; float digitW = 7.5f; float digitH = 15.0f;
            DrawFadingRollingTime(progressTimer, g_pFontTimeClean.Get(), cx - 12.0f, timeY, digitW, digitH, textAlphaAnim.current, true);
            g_pFontTimeClean->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bSlash; rt->CreateSolidColorBrush(D2D1::ColorF(baseColor.r, baseColor.g, baseColor.b, baseColor.a * textAlphaAnim.current * 0.6f), &bSlash);
            rt->DrawText(L"/", 1, g_pFontTimeClean.Get(), D2D1::RectF(cx - 12.0f, timeY, cx + 12.0f, timeY + digitH), bSlash.Get());

            DrawFadingRollingTime(durationTimer, g_pFontTimeClean.Get(), cx + 12.0f, timeY, digitW, digitH, textAlphaAnim.current, false);
            rt->SetTransform(orgTransform);
            };

        struct UICacheState {
            float trackAnim, progM1, progM2, progS1, progS2;
            float durM1, durM2, durS1, durS2, alpha, scale;
            BYTE tR, tG, tB, aR, aG, aB;
            float sR, sG, sB, shadowAlpha, glowAlpha;
            std::wstring curTrack, tgtTrack, curArtist, tgtArtist;

            bool operator!=(const UICacheState& o) const {
                return std::abs(trackAnim - o.trackAnim) > 0.001f ||
                    std::abs(progM1 - o.progM1) > 0.001f || std::abs(progM2 - o.progM2) > 0.001f ||
                    std::abs(progS1 - o.progS1) > 0.001f || std::abs(progS2 - o.progS2) > 0.001f ||
                    std::abs(durM1 - o.durM1) > 0.001f || std::abs(durM2 - o.durM2) > 0.001f ||
                    std::abs(durS1 - o.durS1) > 0.001f || std::abs(durS2 - o.durS2) > 0.001f ||
                    std::abs(alpha - o.alpha) > 0.001f || std::abs(scale - o.scale) > 0.001f ||
                    tR != o.tR || tG != o.tG || tB != o.tB || aR != o.aR || aG != o.aG || aB != o.aB ||
                    std::abs(sR - o.sR) > 0.001f || std::abs(sG - o.sG) > 0.001f || std::abs(sB - o.sB) > 0.001f ||
                    std::abs(shadowAlpha - o.shadowAlpha) > 0.001f || std::abs(glowAlpha - o.glowAlpha) > 0.001f ||
                    curTrack != o.curTrack || tgtTrack != o.tgtTrack ||
                    curArtist != o.curArtist || tgtArtist != o.tgtArtist;
            }
        };

        static Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> s_uiShadowRT;
        static Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> s_uiGlowRT;
        static Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> s_uiCompRT;
        static int s_lastH = 0;
        static UICacheState s_lastUIState = {};

        bool rtRecreated = false;
        if (!s_uiShadowRT || s_lastH != frame.currentOverlayH) {
            D2D1_SIZE_F size = D2D1::SizeF((float)OVERLAY_W, (float)frame.currentOverlayH);
            pDCRT->CreateCompatibleRenderTarget(size, &s_uiShadowRT);
            pDCRT->CreateCompatibleRenderTarget(size, &s_uiGlowRT);
            pDCRT->CreateCompatibleRenderTarget(size, &s_uiCompRT);
            s_lastH = frame.currentOverlayH;
            rtRecreated = true;
        }

        UICacheState currentState = {
            trackRoll.rollAnim.current, progressTimer.m1.current, progressTimer.m2.current, progressTimer.s1.current, progressTimer.s2.current,
            durationTimer.m1.current, durationTimer.m2.current, durationTimer.s1.current, durationTimer.s2.current, textAlphaAnim.current, textScaleAnim.current,
            static_cast<BYTE>(txtR * 255.0f), static_cast<BYTE>(txtG * 255.0f), static_cast<BYTE>(txtB * 255.0f),
            static_cast<BYTE>(accR * 255.0f), static_cast<BYTE>(accG * 255.0f), static_cast<BYTE>(accB * 255.0f),
            shadowR, shadowG, shadowB, uiShadowAlpha, uiGlowAlpha,
            trackRoll.currentText, trackRoll.targetText, artistRoll.currentText, artistRoll.targetText
        };

        bool needsUIUpdate = rtRecreated || (currentState != s_lastUIState);

        if (s_uiShadowRT && s_uiGlowRT && s_uiCompRT) {
            Microsoft::WRL::ComPtr<ID2D1DeviceContext> pCompCtx;
            s_uiCompRT.As(&pCompCtx);

            if (pCompCtx) {

                if (needsUIUpdate) {
                    s_uiShadowRT->BeginDraw();
                    s_uiShadowRT->Clear(D2D1::ColorF(0, 0, 0, 0));
                    DrawAllUI(s_uiShadowRT.Get(), D2D1::ColorF(shadowR, shadowG, shadowB, uiShadowAlpha));
                    s_uiShadowRT->EndDraw();

                    s_uiGlowRT->BeginDraw();
                    s_uiGlowRT->Clear(D2D1::ColorF(0, 0, 0, 0));
                    DrawAllUI(s_uiGlowRT.Get(), D2D1::ColorF(accR, accG, accB, uiGlowAlpha));
                    s_uiGlowRT->EndDraw();

                    pCompCtx->BeginDraw();
                    pCompCtx->Clear(D2D1::ColorF(0, 0, 0, 0));
                    pCompCtx->SetTransform(D2D1::Matrix3x2F::Identity());

                    Microsoft::WRL::ComPtr<ID2D1Effect> shadowBlur;
                    pCompCtx->CreateEffect(CLSID_D2D1GaussianBlur, &shadowBlur);
                    Microsoft::WRL::ComPtr<ID2D1Bitmap> shadowBmp;
                    s_uiShadowRT->GetBitmap(&shadowBmp);
                    shadowBlur->SetInput(0, shadowBmp.Get());
                    shadowBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 3.5f);

                    Microsoft::WRL::ComPtr<ID2D1Effect> glowBlur;
                    pCompCtx->CreateEffect(CLSID_D2D1GaussianBlur, &glowBlur);
                    Microsoft::WRL::ComPtr<ID2D1Bitmap> glowBmp;
                    s_uiGlowRT->GetBitmap(&glowBmp);
                    glowBlur->SetInput(0, glowBmp.Get());
                    glowBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 6.0f);

                    pCompCtx->DrawImage(shadowBlur.Get());
                    pCompCtx->DrawImage(glowBlur.Get(), D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_PLUS);
                    pCompCtx->EndDraw();

                    s_lastUIState = currentState;
                }

                pDCRT->SetTransform(D2D1::Matrix3x2F::Identity());
                Microsoft::WRL::ComPtr<ID2D1Bitmap> finalCompBmp;
                s_uiCompRT->GetBitmap(&finalCompBmp);
                pDCRT->DrawBitmap(finalCompBmp.Get(), D2D1::RectF(0, 0, (float)OVERLAY_W, (float)frame.currentOverlayH));
            }
        }

        DrawAllUI(pDCRT, D2D1::ColorF(txtR, txtG, txtB, 1.0f));

        auto DrawLyrics = [&](const std::vector<LyricDrawInfo>& list, float offsetX, float globalAlpha) {
            if (globalAlpha <= 0.001f) return;

            float clipY = OVERLAY_H - 45.0f;
            float clipH = frame.currentOverlayH - clipY;
            if (clipH <= 5.0f) return;

            D2D1_RECT_F clipRect = D2D1::RectF(-2000.0f, clipY, (float)OVERLAY_W + 2000.0f, clipY + clipH);
            pDCRT->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            float lyricShiftY = -35.0f;
            Microsoft::WRL::ComPtr<ID2D1Layer> pSweepLayer;
            pDCRT->CreateLayer(NULL, &pSweepLayer);

            for (const auto& item : list) {
                float finalAlpha = item.alpha * globalAlpha;
                if (finalAlpha <= 0.01f) continue;

                float cx = (float)OVERLAY_W / 2.0f;
                pDCRT->SetTransform(D2D1::Matrix3x2F::Scale(item.scaleX, item.scaleY, D2D1::Point2F(cx, item.targetY)) * D2D1::Matrix3x2F::Translation(offsetX, lyricShiftY) * orgTransform);
                D2D1_RECT_F textRect = D2D1::RectF(10.0f, item.targetY - 15.0f, (float)OVERLAY_W - 10.0f, item.targetY + 15.0f);

                ID2D1Bitmap* pShadowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, shadowByte, shadowByte, shadowByte);
                if (pShadowBmp) {
                    float cacheW = 800.0f; float cacheH = 100.0f;
                    D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                    pDCRT->DrawBitmap(pShadowBmp, destRect, std::clamp(finalAlpha * lrcShadowMult, 0.0f, 1.0f), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }

                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> dimBrush;
                pDCRT->CreateSolidColorBrush(D2D1::ColorF(txtR, txtG, txtB, finalAlpha * 0.35f), &dimBrush);
                pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, dimBrush.Get());

                if (s_render_glow_mode == 0) {
                    if (item.glowIntensity > 0.01f) {
                        float glowAlpha = std::clamp(finalAlpha * item.glowIntensity * lrcGlowMult, 0.0f, 1.0f);
                        ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                        if (pGlowBmp) {
                            float cacheW = 800.0f; float cacheH = 100.0f;
                            D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                            pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }
                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                        pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, finalAlpha * item.glowIntensity), &brightWhiteBrush);
                        pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());
                    }
                }
                else {
                    if (item.sweepEnd - item.sweepStart > 0.001f && pSweepLayer) {
                        float invScale = 1.0f / item.scaleX;
                        float localW = OVERLAY_W * invScale;
                        float textLeft = cx - (localW / 2.0f) - (120.0f * invScale);
                        float textRight = cx + (localW / 2.0f) + (120.0f * invScale);
                        float totalW = textRight - textLeft;
                        float feather = 80.0f * invScale;
                        float xStart = textLeft + totalW * item.sweepStart - feather;
                        float xEnd = textLeft + totalW * item.sweepEnd + feather;

                        if (xEnd > xStart) {
                            float sweepLen = xEnd - xStart;
                            float fRatio = std::clamp(feather / sweepLen, 0.01f, 0.49f);
                            D2D1_GRADIENT_STOP stops[4] = {
                                { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) },
                                { fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f - fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
                            };
                            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pStops;
                            pDCRT->CreateGradientStopCollection(stops, 4, &pStops);
                            Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> pMaskBrush;
                            pDCRT->CreateLinearGradientBrush(
                                D2D1::LinearGradientBrushProperties(D2D1::Point2F(xStart, item.targetY), D2D1::Point2F(xEnd, item.targetY)),
                                pStops.Get(), &pMaskBrush
                            );
                            D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                            layerParams.contentBounds = D2D1::InfiniteRect();
                            layerParams.opacityBrush = pMaskBrush.Get();

                            pDCRT->PushLayer(&layerParams, pSweepLayer.Get());
                            ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                            if (pGlowBmp) {
                                float glowAlpha = std::clamp(finalAlpha * item.glowIntensity * lrcGlowMult, 0.0f, 1.0f);
                                float cacheW = 800.0f; float cacheH = 100.0f;
                                D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                                pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                            }
                            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                            pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, finalAlpha), &brightWhiteBrush);
                            pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());
                            pDCRT->PopLayer();
                        }
                    }
                }
                pDCRT->SetTransform(orgTransform);
            }
            pDCRT->PopAxisAlignedClip();
            };

        if (frame.currentOverlayH > OVERLAY_H + 5) {
            float modeVal = modeAnim.current;
            if (modeVal < 0.999f) DrawLyrics(frame.list2D, -(modeVal * (float)OVERLAY_W * 1.5f), 1.0f - modeVal);
            if (modeVal > 0.001f) DrawLyrics(frame.list3D, (1.0f - modeVal) * (float)OVERLAY_W * 1.5f, modeVal);
        }

        pDCRT->SetTransform(D2D1::Matrix3x2F::Identity());
        };

    auto RenderScene3 = [&](ID2D1DCRenderTarget* pDCRT, IDWriteFactory* pDWriteFactory, const FrameData& frame) {
        Microsoft::WRL::ComPtr<ID2D1Factory> pD2DFactory; if (pDCRT) pDCRT->GetFactory(&pD2DFactory);
        pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

        static Gdiplus::Bitmap* lastGdiPtr = nullptr; static Microsoft::WRL::ComPtr<ID2D1Bitmap> currentD2DBitmap; static Microsoft::WRL::ComPtr<ID2D1Bitmap> oldD2DBitmap;
        auto GetOrCreateD2DBitmap = [&](Gdiplus::Bitmap* gdiBmp, Microsoft::WRL::ComPtr<ID2D1Bitmap>& outBmp) {
            if (!gdiBmp) return; UINT w = gdiBmp->GetWidth(); UINT h = gdiBmp->GetHeight();
            Gdiplus::Rect rect(0, 0, w, h); Gdiplus::BitmapData bmpData;
            if (gdiBmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok) {
                D2D1_SIZE_U size = D2D1::SizeU(w, h); D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                pDCRT->CreateBitmap(size, bmpData.Scan0, bmpData.Stride, &props, &outBmp); gdiBmp->UnlockBits(&bmpData);
            }
            };

        if (g_album_art != lastGdiPtr) { lastGdiPtr = g_album_art; currentD2DBitmap.Reset(); GetOrCreateD2DBitmap(g_album_art, currentD2DBitmap); }

        Gdiplus::Color txtCol = textColorAnim.getColor(); Gdiplus::Color accCol = accentColorAnim.getColor(); Gdiplus::Color bgCol = bgColorAnim.getColor();
        float txtR = txtCol.GetR() / 255.0f; float txtG = txtCol.GetG() / 255.0f; float txtB = txtCol.GetB() / 255.0f;
        float accR = accCol.GetR() / 255.0f; float accG = accCol.GetG() / 255.0f; float accB = accCol.GetB() / 255.0f;
        float bgR = bgCol.GetR() / 255.0f;   float bgG = bgCol.GetG() / 255.0f;   float bgB = bgCol.GetB() / 255.0f;

        static auto lastTime = std::chrono::steady_clock::now(); auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count(); lastTime = now; if (dt > 0.1f) dt = 0.016f;

        static float currentRotation = 0.0f; static float rotationSpeed = 0.0f; static float armLift = 1.0f;
        static float recordSwapProgress = 1.0f; static bool isSwapping = false; static std::wstring lastTrackName = L"";
        static int lastProgress = frame.progress_ms;

        bool playing = g_spotify_overlay_state.is_playing;
        std::wstring curTrack = Utf8ToWide(g_spotify_overlay_state.current_track.track_name);
        if (!curTrack.empty() && curTrack != lastTrackName) { lastTrackName = curTrack; isSwapping = true; recordSwapProgress = 0.0f; oldD2DBitmap = currentD2DBitmap; }
        int progressDiff = frame.progress_ms - lastProgress; if (std::abs(progressDiff) > 1000) { currentRotation += (progressDiff / 10000.0f) * 360.0f; } lastProgress = frame.progress_ms;

        if (isSwapping) {
            armLift += (1.0f - armLift) * 8.0f * dt;
            if (armLift > 0.8f) { recordSwapProgress += dt * 0.5f; if (recordSwapProgress >= 1.0f) { recordSwapProgress = 1.0f; isSwapping = false; } }
        }
        else { float targetArmLift = playing ? 0.0f : 1.0f; armLift += (targetArmLift - armLift) * 6.0f * dt; }

        float targetSpeed = (playing && !isSwapping) ? 36.0f : 0.0f; rotationSpeed += (targetSpeed - rotationSpeed) * 3.0f * dt; currentRotation += rotationSpeed * dt;

        auto GetD2DColor = [](const Gdiplus::Color& c, BYTE alpha = 255) { return D2D1::ColorF(c.GetR() / 255.0f, c.GetG() / 255.0f, c.GetB() / 255.0f, (c.GetA() == 255 ? alpha : c.GetA()) / 255.0f); };

        auto DrawShadowText = [&](const std::wstring& text, UINT32 len, IDWriteTextFormat* format, const D2D1_RECT_F& rect, ID2D1SolidColorBrush* brush) {
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> shadowBrush; float baseAlpha = brush->GetOpacity();
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, baseAlpha * 0.5f), &shadowBrush);
            D2D1_RECT_F sRect = D2D1::RectF(rect.left - 1.5f, rect.top + 1.5f, rect.right - 1.5f, rect.bottom + 1.5f);
            pDCRT->DrawText(text.c_str(), len, format, &sRect, shadowBrush.Get()); pDCRT->DrawText(text.c_str(), len, format, &rect, brush);
            };

        auto DrawRollingText = [&](RollingTextRenderer& roller, IDWriteTextFormat* pFormat, ID2D1SolidColorBrush* pBrush, const D2D1_RECT_F& rect) {
            if (roller.rollAnim.current >= 0.999f) { DrawShadowText(roller.targetText, (UINT32)roller.targetText.length(), pFormat, rect, pBrush); return; }
            pDCRT->PushAxisAlignedClip(&rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            float h = rect.bottom - rect.top; float yOffset = roller.rollAnim.current * h;
            D2D1_RECT_F oldRect = D2D1::RectF(rect.left, rect.top - yOffset, rect.right, rect.bottom - yOffset);
            DrawShadowText(roller.currentText, (UINT32)roller.currentText.length(), pFormat, oldRect, pBrush);
            D2D1_RECT_F newRect = D2D1::RectF(rect.left, rect.top + (h - yOffset), rect.right, rect.bottom + (h - yOffset));
            DrawShadowText(roller.targetText, (UINT32)roller.targetText.length(), pFormat, newRect, pBrush);
            pDCRT->PopAxisAlignedClip();
            };

        auto DrawRollingTime = [&](RollingTimeRenderer& timer, IDWriteTextFormat* pFormat, ID2D1SolidColorBrush* pBrush, float startX, float startY, float digitW, float digitH, bool rightAlign = false) {
            float totalW = 4.5f * digitW; float x = rightAlign ? (startX - totalW) : startX;
            auto renderDigit = [&](RollingDigitAnimation& anim, float dx, int base) {
                float val = anim.current; int num1 = static_cast<int>(floor(val)) % base; if (num1 < 0) num1 += base; int num2 = (num1 + 1) % base; float off = val - floor(val);
                D2D1_RECT_F digitRect = D2D1::RectF(dx, startY, dx + digitW, startY + digitH); pDCRT->PushAxisAlignedClip(&digitRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                std::wstring n1 = std::to_wstring(num1); std::wstring n2 = std::to_wstring(num2);
                D2D1_RECT_F r1 = D2D1::RectF(dx, startY - off * digitH, dx + digitW, startY + digitH - off * digitH); DrawShadowText(n1, (UINT32)n1.length(), pFormat, r1, pBrush);
                D2D1_RECT_F r2 = D2D1::RectF(dx, startY + digitH - off * digitH, dx + digitW, startY + digitH * 2.0f - off * digitH); DrawShadowText(n2, (UINT32)n2.length(), pFormat, r2, pBrush);
                pDCRT->PopAxisAlignedClip();
                };
            renderDigit(timer.m1, x, 10); renderDigit(timer.m2, x + digitW, 10);
            D2D1_RECT_F colonRect = D2D1::RectF(x + digitW * 2.0f, startY, x + digitW * 2.5f, startY + digitH); DrawShadowText(L":", 1, pFormat, colonRect, pBrush);
            renderDigit(timer.s1, x + digitW * 2.5f, 6); renderDigit(timer.s2, x + digitW * 3.5f, 10);
            };

        D2D1_MATRIX_3X2_F orgTransform; pDCRT->GetTransform(&orgTransform);

        float cardRadius = 18.0f; D2D1_RECT_F cardRect = D2D1::RectF(1.0f, 1.0f, (float)OVERLAY_W - 1.0f, (float)frame.currentOverlayH - 1.0f);
        D2D1_ROUNDED_RECT roundedCard = D2D1::RoundedRect(cardRect, cardRadius, cardRadius);

        float imgSize = 80.0f; float imgX = 20.0f; float imgY = (OVERLAY_H - imgSize) / 2.0f; float imgCX = imgX + imgSize / 2.0f; float imgCY = imgY + imgSize / 2.0f;

        Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> fullCardGeom; pD2DFactory->CreateRoundedRectangleGeometry(&roundedCard, &fullCardGeom);
        Microsoft::WRL::ComPtr<ID2D1EllipseGeometry> outerCircle, innerCircle;
        pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(imgCX, imgCY), imgSize / 2.0f, imgSize / 2.0f), &outerCircle);
        pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(imgCX, imgCY), imgSize / 8.0f, imgSize / 8.0f), &innerCircle);

        Microsoft::WRL::ComPtr<ID2D1PathGeometry> recordGeom; pD2DFactory->CreatePathGeometry(&recordGeom);
        Microsoft::WRL::ComPtr<ID2D1GeometrySink> recSink; recordGeom->Open(&recSink);
        outerCircle->CombineWithGeometry(innerCircle.Get(), D2D1_COMBINE_MODE_EXCLUDE, NULL, recSink.Get()); recSink->Close();

        BYTE bgAlpha1 = static_cast<BYTE>(std::clamp(230.0f * g_bg_alpha, 0.0f, 255.0f)); BYTE bgAlpha2 = static_cast<BYTE>(std::clamp(200.0f * g_bg_alpha, 0.0f, 255.0f));
        Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pBgStops; D2D1_GRADIENT_STOP bgStops[2] = { { 0.0f, GetD2DColor(bgCol, bgAlpha1) }, { 1.0f, GetD2DColor(bgCol, bgAlpha2) } };
        pDCRT->CreateGradientStopCollection(bgStops, 2, &pBgStops); Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> cardBgBrush;
        pDCRT->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, (float)frame.currentOverlayH)), pBgStops.Get(), &cardBgBrush);
        pDCRT->FillRoundedRectangle(&roundedCard, cardBgBrush.Get());

        auto nowForNoise = std::chrono::steady_clock::now().time_since_epoch(); unsigned int noiseSeed = (unsigned int)(std::chrono::duration_cast<std::chrono::milliseconds>(nowForNoise).count() / 60); srand(noiseSeed);
        std::vector<Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> noiseBrushes;
        for (int i = 0; i < 16; ++i) {
            float mix = (rand() % 20 + 10) / 100.0f; float diff = ((rand() % 100) / 100.0f - 0.5f) * 0.12f;
            float r = std::clamp(bgR * (1.0f - mix) + accR * mix + diff, 0.0f, 1.0f); float g = std::clamp(bgG * (1.0f - mix) + accG * mix + diff, 0.0f, 1.0f); float b = std::clamp(bgB * (1.0f - mix) + accB * mix + diff, 0.0f, 1.0f);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(r, g, b, 0.3f * g_bg_alpha), &bBrush); noiseBrushes.push_back(bBrush);
        }

        Microsoft::WRL::ComPtr<ID2D1Layer> pLayer; pDCRT->CreateLayer(NULL, &pLayer);
        pDCRT->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), fullCardGeom.Get()), pLayer.Get());
        for (int i = 0; i < 1200; ++i) {
            float nx = (float)(rand() % OVERLAY_W); float ny = (float)(rand() % frame.currentOverlayH); int bIdx = rand() % noiseBrushes.size();
            D2D1_RECT_F dotRect = D2D1::RectF(nx, ny, nx + 1.5f, ny + 1.5f); pDCRT->FillRectangle(&dotRect, noiseBrushes[bIdx].Get());
        }
        pDCRT->PopLayer();

        Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pBorderStops; D2D1_GRADIENT_STOP borderStops[2] = { { 0.0f, GetD2DColor(txtCol, static_cast<BYTE>(60 * g_bg_alpha)) }, { 1.0f, GetD2DColor(accCol, static_cast<BYTE>(100 * g_bg_alpha)) } };
        pDCRT->CreateGradientStopCollection(borderStops, 2, &pBorderStops); Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> borderBrush;
        pDCRT->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, (float)frame.currentOverlayH)), pBorderStops.Get(), &borderBrush);
        pDCRT->DrawRoundedRectangle(&roundedCard, borderBrush.Get(), 1.0f);

        float recOffX = 0.0f; float recOffY = 0.0f; float recScale = 1.0f; float recAlpha = 1.0f; Microsoft::WRL::ComPtr<ID2D1Bitmap> targetBmp = currentD2DBitmap;
        if (recordSwapProgress < 1.0f) {
            if (recordSwapProgress < 0.5f) { float p = recordSwapProgress * 2.0f; float ep = p * p; recOffX = -ep * 120.0f; recOffY = -ep * 25.0f; recScale = 1.0f + ep * 0.2f; recAlpha = 1.0f - ep; targetBmp = oldD2DBitmap; }
            else { float p = (recordSwapProgress - 0.5f) * 2.0f; float ep = 1.0f - (1.0f - p) * (1.0f - p); recOffX = (1.0f - ep) * 120.0f; recOffY = -(1.0f - ep) * 25.0f; recScale = 1.2f - ep * 0.2f; recAlpha = ep; targetBmp = currentD2DBitmap; }
        }

        D2D1_MATRIX_3X2_F recordTrans = D2D1::Matrix3x2F::Scale(recScale, recScale, D2D1::Point2F(imgCX, imgCY)) * D2D1::Matrix3x2F::Translation(recOffX, recOffY);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> albShadowBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f * g_album_alpha * recAlpha), &albShadowBrush);
        D2D1_MATRIX_3X2_F shadowTrans = recordTrans * D2D1::Matrix3x2F::Translation(-2.5f, 3.5f); Microsoft::WRL::ComPtr<ID2D1TransformedGeometry> shadowGeom;
        pD2DFactory->CreateTransformedGeometry(recordGeom.Get(), shadowTrans, &shadowGeom); pDCRT->FillGeometry(shadowGeom.Get(), albShadowBrush.Get());

        auto DrawRecordImage = [&](Microsoft::WRL::ComPtr<ID2D1Bitmap> bmp, float opacity) {
            if (!bmp || g_album_alpha <= 0.001f || opacity <= 0.001f) return;
            Microsoft::WRL::ComPtr<ID2D1BitmapBrush> pBmpBrush; D2D1_BITMAP_BRUSH_PROPERTIES bmpBrushProps = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP);
            pDCRT->CreateBitmapBrush(bmp.Get(), bmpBrushProps, &pBmpBrush);
            D2D1_SIZE_F size = bmp->GetSize(); D2D1_MATRIX_3X2_F brushScale = D2D1::Matrix3x2F::Scale(imgSize / size.width, imgSize / size.height);
            D2D1_MATRIX_3X2_F brushTrans = D2D1::Matrix3x2F::Translation(imgX, imgY); D2D1_MATRIX_3X2_F brushRot = D2D1::Matrix3x2F::Rotation(currentRotation, D2D1::Point2F(imgCX, imgCY));
            pBmpBrush->SetTransform(brushScale * brushTrans * brushRot * recordTrans); pBmpBrush->SetOpacity(opacity * g_album_alpha);
            Microsoft::WRL::ComPtr<ID2D1TransformedGeometry> transRecGeom; pD2DFactory->CreateTransformedGeometry(recordGeom.Get(), recordTrans, &transRecGeom);
            pDCRT->FillGeometry(transRecGeom.Get(), pBmpBrush.Get());
            };
        DrawRecordImage(targetBmp, recAlpha);

        float pivotX = imgCX + imgSize * 0.45f; float pivotY = imgY - 8.0f; float armTargetX = imgCX - imgSize * 0.05f; float armTargetY = imgCY + imgSize * 0.1f; float bounce = 0.0f;
        if (playing && rotationSpeed > 10.0f && armLift < 0.1f) { srand((unsigned int)(frame.progress_ms / 120)); if (rand() % 100 > 85) bounce = -(rand() % 10) * 0.25f; }

        float armAngle = armLift * 25.0f; float arm3DScale = 1.0f + armLift * 0.35f; D2D1_MATRIX_3X2_F curTrans; pDCRT->GetTransform(&curTrans);
        D2D1_MATRIX_3X2_F armMat = D2D1::Matrix3x2F::Rotation(armAngle, D2D1::Point2F(pivotX, pivotY)) * D2D1::Matrix3x2F::Scale(arm3DScale, arm3DScale, D2D1::Point2F(pivotX, pivotY));
        float shadowDistX = -2.0f - armLift * 4.0f; float shadowDistY = 3.0f + armLift * 6.0f; D2D1_MATRIX_3X2_F shadowMat = D2D1::Matrix3x2F::Rotation(armAngle, D2D1::Point2F(pivotX, pivotY)) * D2D1::Matrix3x2F::Translation(shadowDistX, shadowDistY);

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> armBrush, armShadowBrush;
        pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.85f, 0.85f, 1.0f * g_album_alpha), &armBrush); pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, (0.5f - armLift * 0.15f) * g_album_alpha), &armShadowBrush);

        D2D1_POINT_2F p1 = D2D1::Point2F(pivotX, pivotY); D2D1_POINT_2F p2 = D2D1::Point2F(armTargetX, armTargetY + bounce);
        pDCRT->SetTransform(shadowMat * curTrans); pDCRT->DrawLine(p1, p2, armShadowBrush.Get(), 2.0f); pDCRT->FillEllipse(D2D1::Ellipse(p2, 4.0f, 6.0f), armShadowBrush.Get()); pDCRT->FillEllipse(D2D1::Ellipse(p1, 6.0f, 6.0f), armShadowBrush.Get());
        pDCRT->SetTransform(armMat * curTrans); pDCRT->DrawLine(p1, p2, armBrush.Get(), 2.0f); pDCRT->FillEllipse(D2D1::Ellipse(p2, 4.0f, 6.0f), armBrush.Get()); pDCRT->FillEllipse(D2D1::Ellipse(p1, 6.0f, 6.0f), armBrush.Get());
        pDCRT->DrawLine(p2, D2D1::Point2F(p2.x - 2.5f, p2.y + 5.0f), armBrush.Get(), 1.5f); pDCRT->SetTransform(curTrans);

        float textX = imgX + imgSize + 16.0f; float textW = OVERLAY_W - textX - 20.0f; float textCX = textX + textW / 2.0f; float textCY = 45.0f;
        D2D1_MATRIX_3X2_F textScale = D2D1::Matrix3x2F::Scale(textScaleAnim.current, textScaleAnim.current, D2D1::Point2F(textCX, textCY)); pDCRT->SetTransform(textScale * orgTransform);

        BYTE alphaText = static_cast<BYTE>(std::clamp(textAlphaAnim.current * 255.0f, 0.0f, 255.0f)); BYTE alphaSub = static_cast<BYTE>(std::clamp(textAlphaAnim.current * 200.0f, 0.0f, 255.0f));

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush, artistBrush, timerBrush, sysTimeBrush, accentBrush;
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaText), alphaText), &textBrush); pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &artistBrush);
        pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &timerBrush); pDCRT->CreateSolidColorBrush(GetD2DColor(textColorAnim.getColor(alphaSub), alphaSub), &sysTimeBrush); pDCRT->CreateSolidColorBrush(GetD2DColor(accCol, 255), &accentBrush);

        D2D1_RECT_F trackRect = D2D1::RectF(textX, 20.0f, textX + textW - 55.0f, 42.0f); D2D1_RECT_F artistRect = D2D1::RectF(textX, 44.0f, textX + textW, 62.0f);

        if (g_debug_status != L"OK" && g_debug_status != L"OK (MOCK MODE)") {
            DrawRollingText(trackRoll, g_pFontTrack.Get(), textBrush.Get(), trackRect);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> debugBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.4f, 0.4f), &debugBrush);
            DrawShadowText(g_debug_status, (UINT32)g_debug_status.length(), g_pFontNormal.Get(), artistRect, debugBrush.Get());
        }
        else {
            DrawRollingText(trackRoll, g_pFontTrack.Get(), textBrush.Get(), trackRect); DrawRollingText(artistRoll, g_pFontArtist.Get(), artistBrush.Get(), artistRect);
        }
        pDCRT->SetTransform(orgTransform);

        g_pFontClock->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        DrawRollingTime(sysClockTimer, g_pFontClock.Get(), sysTimeBrush.Get(), (float)OVERLAY_W - 20.0f, 20.0f, 8.0f, 14.0f, true);

        float barX = textX; float barY = 74.0f; float barW = (float)OVERLAY_W - barX - 20.0f; float barH = 4.0f; float barRadius = 2.0f;
        D2D1_ROUNDED_RECT railRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + barW, barY + barH), barRadius, barRadius);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> railBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(txtR, txtG, txtB, 35.0f / 255.0f), &railBrush);
        pDCRT->FillRoundedRectangle(&railRect, railBrush.Get());

        float activeBarW = barW * progressBarAnim.current;
        if (activeBarW > 0) {
            D2D1_ROUNDED_RECT activeRect = D2D1::RoundedRect(D2D1::RectF(barX, barY, barX + activeBarW, barY + barH), barRadius, barRadius);
            pDCRT->FillRoundedRectangle(&activeRect, accentBrush.Get());
        }

        float nx = barX + activeBarW; float ny = barY - 1.0f;
        Microsoft::WRL::ComPtr<ID2D1PathGeometry> needleGeom; pD2DFactory->CreatePathGeometry(&needleGeom);
        Microsoft::WRL::ComPtr<ID2D1GeometrySink> nSink; needleGeom->Open(&nSink);
        nSink->BeginFigure(D2D1::Point2F(nx, ny), D2D1_FIGURE_BEGIN_FILLED);
        nSink->AddLine(D2D1::Point2F(nx - 3.5f, ny - 5.0f)); nSink->AddLine(D2D1::Point2F(nx + 3.5f, ny - 5.0f)); nSink->EndFigure(D2D1_FIGURE_END_CLOSED); nSink->Close();

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pinShadowBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.6f * g_album_alpha), &pinShadowBrush);
        Microsoft::WRL::ComPtr<ID2D1TransformedGeometry> nShadowGeom; D2D1_MATRIX_3X2_F nShadowTrans = D2D1::Matrix3x2F::Translation(-1.5f, 1.5f);
        pD2DFactory->CreateTransformedGeometry(needleGeom.Get(), nShadowTrans, &nShadowGeom);
        pDCRT->FillGeometry(nShadowGeom.Get(), pinShadowBrush.Get()); pDCRT->FillGeometry(needleGeom.Get(), accentBrush.Get());

        g_pFontTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        DrawRollingTime(progressTimer, g_pFontTime.Get(), timerBrush.Get(), barX, barY + 6.0f, 7.5f, 14.0f);
        g_pFontTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        DrawRollingTime(durationTimer, g_pFontTime.Get(), timerBrush.Get(), barX + barW, barY + 6.0f, 7.5f, 14.0f, true);

        if (fetchOutAnim.current < 0.99f) {
            float currentScale = 1.0f + fetchOutAnim.current * 0.4f; BYTE alpha = static_cast<BYTE>(std::clamp(180.0f * (1.0f - fetchOutAnim.current), 0.0f, 255.0f));
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> fetchBrush; pDCRT->CreateSolidColorBrush(D2D1::ColorF(txtR, txtG, txtB, alpha / 255.0f), &fetchBrush);
            D2D1_RECT_F fetchRect = D2D1::RectF(0.0f, (float)OVERLAY_H + 4.0f, (float)OVERLAY_W, (float)OVERLAY_H + 24.0f);
            float cx = fetchRect.left + (fetchRect.right - fetchRect.left) / 2.0f; float cy = fetchRect.top + (fetchRect.bottom - fetchRect.top) / 2.0f;
            D2D1_MATRIX_3X2_F fetchMat = D2D1::Matrix3x2F::Scale(currentScale, currentScale, D2D1::Point2F(cx, cy)); pDCRT->SetTransform(fetchMat * orgTransform);
            g_pFontNormal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); DrawShadowText(L"Fetching lyrics...", 18, g_pFontNormal.Get(), fetchRect, fetchBrush.Get());
            pDCRT->SetTransform(orgTransform);
        }

        bool isLineVisible = frame.hasLyrics || (g_old_lyrics_has && oldLyricsAlphaAnim.current > 0.01f);
        if (isLineVisible && frame.extensionRatio > 0.01f && fetchOutAnim.current > 0.01f) {
            float lineW = (OVERLAY_W - 60.0f) * frame.extensionRatio * fetchOutAnim.current;
            float lineH_thick = 2.0f; float lineX_pos = (OVERLAY_W - lineW) / 2.0f; float lineY_pos = OVERLAY_H - 2.0f; float lineRad = 1.0f;
            D2D1_ROUNDED_RECT coreLine = D2D1::RoundedRect(D2D1::RectF(lineX_pos, lineY_pos, lineX_pos + lineW, lineY_pos + lineH_thick), lineRad, lineRad);
            BYTE lineC = static_cast<BYTE>((txtCol.GetR() > 128) ? 200 : 50);
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> lineBrush, lineShadowBrush;
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(lineC / 255.0f, lineC / 255.0f, lineC / 255.0f, 140.0f * frame.extensionRatio * fetchOutAnim.current / 255.0f), &lineBrush);
            pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 100.0f * frame.extensionRatio * fetchOutAnim.current / 255.0f), &lineShadowBrush);
            D2D1_ROUNDED_RECT sLine = D2D1::RoundedRect(D2D1::RectF(lineX_pos - 1.0f, lineY_pos + 1.0f, lineX_pos + lineW - 1.0f, lineY_pos + lineH_thick + 1.0f), lineRad, lineRad);
            pDCRT->FillRoundedRectangle(&sLine, lineShadowBrush.Get()); pDCRT->FillRoundedRectangle(&coreLine, lineBrush.Get());
        }

        auto DrawD2DList = [&](const std::vector<LyricDrawInfo>& list, float offsetX, float clipY, float clipH) {
            D2D1_RECT_F clipRect = D2D1::RectF(0.0f, clipY, (float)OVERLAY_W, clipY + clipH);
            pDCRT->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            Microsoft::WRL::ComPtr<ID2D1Layer> pSweepLayer;
            pDCRT->CreateLayer(NULL, &pSweepLayer);

            for (const auto& item : list) {
                if (item.alpha <= 0.01f) continue;
                float cx = (float)OVERLAY_W / 2.0f;
                D2D1_MATRIX_3X2_F itemMat = D2D1::Matrix3x2F::Scale(item.scaleX, item.scaleY, D2D1::Point2F(cx, item.targetY)) * D2D1::Matrix3x2F::Translation(offsetX, 0.0f) * orgTransform;
                pDCRT->SetTransform(itemMat);

                D2D1_RECT_F textRect = D2D1::RectF(10.0f, item.targetY - 15.0f, (float)OVERLAY_W - 10.0f, item.targetY + 15.0f);
                D2D1_RECT_F sRectLrc = D2D1::RectF(textRect.left - 1.5f, textRect.top + 1.5f, textRect.right - 1.5f, textRect.bottom + 1.5f);

                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> shadowBrush, dimLrcBrush;
                pDCRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, item.alpha * 0.3f), &shadowBrush);
                pDCRT->CreateSolidColorBrush(D2D1::ColorF(txtR, txtG, txtB, item.alpha * 0.35f), &dimLrcBrush);

                pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &sRectLrc, shadowBrush.Get());
                pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, dimLrcBrush.Get());

                if (s_render_glow_mode == 0) {
                    if (item.glowIntensity > 0.01f) {
                        float glowAlpha = std::clamp(item.alpha * item.glowIntensity, 0.0f, 1.0f);

                        ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                        if (pGlowBmp) {
                            float cacheW = 800.0f; float cacheH = 100.0f;
                            D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                            pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }

                        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                        pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, item.alpha * item.glowIntensity), &brightWhiteBrush);
                        pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());
                    }
                }
                else {
                    if (item.sweepEnd - item.sweepStart > 0.001f && pSweepLayer) {
                        float invScale = 1.0f / item.scaleX;
                        float localW = OVERLAY_W * invScale;
                        float textLeft = cx - (localW / 2.0f) - (120.0f * invScale);
                        float textRight = cx + (localW / 2.0f) + (120.0f * invScale);
                        float totalW = textRight - textLeft;

                        float feather = 80.0f * invScale;
                        float xStart = textLeft + totalW * item.sweepStart - feather;
                        float xEnd = textLeft + totalW * item.sweepEnd + feather;

                        if (xEnd > xStart) {
                            float sweepLen = xEnd - xStart;
                            float fRatio = std::clamp(feather / sweepLen, 0.01f, 0.49f);

                            D2D1_GRADIENT_STOP stops[4] = {
                                { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) },
                                { fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f - fRatio, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) },
                                { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
                            };

                            Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> pStops;
                            pDCRT->CreateGradientStopCollection(stops, 4, &pStops);

                            Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> pMaskBrush;
                            pDCRT->CreateLinearGradientBrush(
                                D2D1::LinearGradientBrushProperties(D2D1::Point2F(xStart, item.targetY), D2D1::Point2F(xEnd, item.targetY)),
                                pStops.Get(), &pMaskBrush
                            );

                            D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                            layerParams.contentBounds = D2D1::InfiniteRect();
                            layerParams.opacityBrush = pMaskBrush.Get();

                            pDCRT->PushLayer(&layerParams, pSweepLayer.Get());

                            ID2D1Bitmap* pGlowBmp = GetOrCreateLyricGlowBitmap(pDCRT, item.trackName, item.text, item.gr, item.gg, item.gb);
                            if (pGlowBmp) {
                                float glowAlpha = std::clamp(item.alpha * item.glowIntensity, 0.0f, 1.0f);
                                float cacheW = 800.0f; float cacheH = 100.0f;
                                D2D1_RECT_F destRect = D2D1::RectF((OVERLAY_W - cacheW) / 2.0f, item.targetY - cacheH / 2.0f, (OVERLAY_W + cacheW) / 2.0f, item.targetY + cacheH / 2.0f);
                                pDCRT->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                            }

                            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
                            pDCRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, item.alpha), &brightWhiteBrush);
                            pDCRT->DrawText(item.text.c_str(), (UINT32)item.text.length(), g_pFontLyrics.Get(), &textRect, brightWhiteBrush.Get());

                            pDCRT->PopLayer();
                        }
                    }
                }

                pDCRT->SetTransform(orgTransform);
            }
            pDCRT->PopAxisAlignedClip();
            };

        if (frame.currentOverlayH > OVERLAY_H + 5) {
            float lyricsAreaY = OVERLAY_H - 2.0f + 5.0f; float lyricsAreaH = frame.currentOverlayH - lyricsAreaY - 2.0f; float modeVal = modeAnim.current;
            if (modeVal < 0.999f) DrawD2DList(frame.list2D, -(modeVal * OVERLAY_W), lyricsAreaY, lyricsAreaH);
            if (modeVal > 0.001f) DrawD2DList(frame.list3D, (1.0f - modeVal) * OVERLAY_W, lyricsAreaY, lyricsAreaH);
        }
        };

    auto RenderScene = [&](ID2D1DCRenderTarget* pDCRT, IDWriteFactory* pDWriteFactory, const FrameData& frame) {
        switch (g_uiStyle) {
        case 0: RenderScene1(pDCRT, pDWriteFactory, frame); break;
        case 1: RenderScene2(pDCRT, pDWriteFactory, frame); break;
        case 2: RenderScene3(pDCRT, pDWriteFactory, frame); break;
        default: RenderScene1(pDCRT, pDWriteFactory, frame); break;
        }
        };

    FrameData frameData;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = nullptr;
    HBITMAP hOldBitmap = nullptr;
    int lastCreatedH = 0;

    bool isHighResTimerActive = true;
    timeBeginPeriod(1);

    auto lastFrameTime = std::chrono::steady_clock::now();
    float pause_timer = 0.0f;

    while (true) {

        auto frameStart = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(frameStart - lastFrameTime).count();
        lastFrameTime = frameStart;

        if (dt > 0.1f) dt = 0.016f;

        std::wstring refetchTrack, refetchArtist;
        bool refetchIsUnknown = false;
        if (LyricsManager::PopRefetchRequest(refetchTrack, refetchArtist, refetchIsUnknown)) {
            auto WideToUtf8 = [](const std::wstring& wstr) -> std::string {
                if (wstr.empty()) return "";
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
                std::string strTo(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
                return strTo;
                };

            std::string tName = WideToUtf8(refetchTrack);
            std::string aName = WideToUtf8(refetchArtist);

            g_current_request_id++;
            int reqId = g_current_request_id.load();
            std::vector<std::string> aList = { aName };
            bool isApi = false;

            {
                std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                std::lock_guard<std::mutex> spot_lock(g_spotify_overlay_state.mtx);
                if (Utf8ToWide(g_spotify_overlay_state.current_track.track_name) == refetchTrack) {
                    g_current_lyrics.has_lyrics = false;
                    g_current_lyrics.lines.clear();
                    g_is_fetching_lyrics = true;
                    g_needs_lyrics_fetch = false;
                    isApi = g_spotify_overlay_state.current_track.is_spotify_api;
                }
            }

            std::thread([tName, aName, aList, reqId, isApi]() {
                FetchLyrics(tName, aName, aList, reqId, isApi);
                }).detach();
        }

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto EXIT_MAIN_LOOP;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        bool is_playing_now = false;
        bool has_track = false;
        std::string tName = "";
        std::string aName = "";
        int current_dur_ms = 0;
        {
            std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
            is_playing_now = g_spotify_overlay_state.is_playing;
            has_track = !g_spotify_overlay_state.current_track.track_name.empty();
            if (has_track) {
                tName = g_spotify_overlay_state.current_track.track_name;
                aName = g_spotify_overlay_state.current_track.artist_name;
                current_dur_ms = g_spotify_overlay_state.duration_ms;
            }
        }

        if (!is_playing_now && has_track) { pause_timer += dt; }
        else { pause_timer = 0.0f; }
        frameData.isEcoDraw = (pause_timer > 10.0f);

        static std::string lastLoadedTrack = "";
        static std::string lastLoadedArtist = "";
        static int lastLoadedDuration = 0;

        if (has_track && (tName != lastLoadedTrack || aName != lastLoadedArtist)) {
            bool preserve_lyrics = false;
            {
                std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                if (g_current_lyrics.has_lyrics && !lastLoadedTrack.empty()) {
                    bool artist_improved = (lastLoadedArtist == "Unknown Artist" && aName != "Unknown Artist");

                    if (!artist_improved) {
                        auto toLower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); }); return s; };
                        std::string lt1 = toLower(lastLoadedTrack), lt2 = toLower(tName);
                        bool name_match = (!lt1.empty() && !lt2.empty() && (lt1.find(lt2) != std::string::npos || lt2.find(lt1) != std::string::npos));
                        bool dur_match = (lastLoadedDuration > 0 && std::abs(lastLoadedDuration - current_dur_ms) < 4000);

                        if (name_match && dur_match) {
                            preserve_lyrics = true;
                        }
                    }
                }
            }

            lastLoadedTrack = tName;
            lastLoadedArtist = aName;
            lastLoadedDuration = current_dur_ms;

            std::wstring safeArtist = SanitizeFileName(Utf8ToWide(aName));
            std::wstring safeTrack = SanitizeFileName(Utf8ToWide(tName));
            std::wstring filePath = g_lyrics_dir + L"\\" + safeArtist + L" - " + safeTrack + L".lrc";

            std::wstring noApiFilePath = g_lyrics_dir + L"\\NoAPI_" + safeArtist + L" - " + safeTrack + L".lrc";
            if (GetFileAttributesW(noApiFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                filePath = noApiFilePath;
            }

            std::ifstream ifs(filePath, std::ios::binary);
            if (ifs.is_open()) {
                std::string lrcText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();
                LyricsData localData;
                ParseLrc(lrcText, localData, tName);
                std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                g_current_lyrics = localData;
                g_is_fetching_lyrics = false;
                g_needs_lyrics_fetch = false;
            }
            else {
                bool is_unknown = false;
                std::ifstream unk_ifs(g_lyrics_dir + L"\\unknowns.txt");
                if (unk_ifs.is_open()) {
                    std::string line;
                    std::string targetLine = "Track: " + tName + " | Artist: " + aName;
                    while (std::getline(unk_ifs, line)) {
                        if (line.find(targetLine) != std::string::npos) { is_unknown = true; break; }
                    }
                    unk_ifs.close();
                }

                std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
                if (!preserve_lyrics) {
                    g_current_lyrics.has_lyrics = false;
                    g_current_lyrics.lines.clear();
                    if (g_show_lyrics && !is_unknown) {
                        g_is_fetching_lyrics = true;
                        g_needs_lyrics_fetch = true;
                    }
                    else {
                        g_is_fetching_lyrics = false;
                        g_needs_lyrics_fetch = false;
                    }
                }
                else {
                    g_is_fetching_lyrics = false;
                    g_needs_lyrics_fetch = false;
                }
            }
        }

        if (g_is_exiting || (!has_track) || (g_hide_on_pause && !is_playing_now)) {
            float fadeFactor = 1.0f - std::exp(-0.15f * 60.0f * dt);
            g_overlay_alpha += (0.0f - g_overlay_alpha) * fadeFactor;
            g_overlay_offset_y += (30.0f - g_overlay_offset_y) * fadeFactor;

            if (g_is_exiting && g_overlay_alpha < 0.02f) break;
        }
        else {
            float fadeFactor = 1.0f - std::exp(-0.08f * 60.0f * dt);
            g_overlay_alpha += (1.0f - g_overlay_alpha) * fadeFactor;
            g_overlay_offset_y += (0.0f - g_overlay_offset_y) * fadeFactor;
        }

        frameData.isEcoDraw = (pause_timer > 10.0f) && (lyricsChangeAnim.current >= 0.999f) && (lyricsScrollAnim.current == lyricsScrollAnim.target);
        UpdateScene(frameData, dt);

        {
            std::string rawTrackName;
            {
                std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
                rawTrackName = g_spotify_overlay_state.current_track.track_name;
            }
            static std::string gcLastTrack = "";
            BYTE tr = static_cast<BYTE>(accentColorAnim.tr);
            BYTE tg = static_cast<BYTE>(accentColorAnim.tg);
            BYTE tb = static_cast<BYTE>(accentColorAnim.tb);

            if (rawTrackName != gcLastTrack || s_lastTargetR != tr || s_lastTargetG != tg || s_lastTargetB != tb) {
                s_glowCaches.clear();
                s_glowCaches.shrink_to_fit();
                frameData.list2D.shrink_to_fit();
                frameData.list3D.shrink_to_fit();
                gcLastTrack = rawTrackName;
                s_lastTargetR = tr; s_lastTargetG = tg; s_lastTargetB = tb;
            }
        }

        if (lastCreatedH != frameData.currentOverlayH || !hBitmap) {
            if (hBitmap) {
                SelectObject(hdcMem, hOldBitmap);
                DeleteObject(hBitmap);
            }
            hBitmap = CreateCompatibleBitmap(hdcScreen, overlayW, frameData.currentOverlayH);
            hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
            lastCreatedH = frameData.currentOverlayH;
        }

        {
            static bool s_lastAffinityBlur = false;
            bool wantBlur = g_enable_blur && (g_uiStyle == 0);
            if (wantBlur != s_lastAffinityBlur) {
                SetWindowDisplayAffinity(hwnd, wantBlur ? 0x00000011 : 0x00000000);
                s_lastAffinityBlur = wantBlur;
            }
        }

        if (pDCRT) {
            RECT rc = { 0, 0, overlayW, frameData.currentOverlayH };
            hr = pDCRT->BindDC(hdcMem, &rc);

            if (SUCCEEDED(hr)) {
                pDCRT->BeginDraw();
                pDCRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
                RenderScene(pDCRT.Get(), pDWriteFactory.Get(), frameData);
                hr = pDCRT->EndDraw();

                if (hr == D2DERR_RECREATE_TARGET) {
                    pDCRT.Reset();
                    s_glowCaches.clear();
                    pD2DFactory->CreateDCRenderTarget(&props, pDCRT.GetAddressOf());
                }
            }
        }

        BYTE currentAlpha = static_cast<BYTE>(std::clamp(g_overlay_alpha * 255.0f, 0.0f, 255.0f));
        POINT ptWinPos = { (int)g_pos_x, (int)g_pos_y + (int)g_overlay_offset_y };
        SIZE szWin = { overlayW, frameData.currentOverlayH };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, currentAlpha, AC_SRC_ALPHA };

        UpdateLayeredWindow(hwnd, hdcScreen, &ptWinPos, &szWin, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

        bool isPlayingNow = false;
        bool isFetchingNow = false;
        {
            std::lock_guard<std::mutex> lock(g_spotify_overlay_state.mtx);
            isPlayingNow = g_spotify_overlay_state.is_playing;
        }
        {
            std::lock_guard<std::mutex> lrc_lock(g_lyrics_mtx);
            isFetchingNow = g_is_fetching_lyrics;
        }

        bool isFading = (g_overlay_alpha > 0.01f && g_overlay_alpha < 0.99f);

        bool isColorChanging = (colorTransAnim.current < 0.999f) ||
            (std::abs(accentColorAnim.r - accentColorAnim.tr) > 0.5f) ||
            (std::abs(accentColorAnim.g - accentColorAnim.tg) > 0.5f) ||
            (std::abs(accentColorAnim.b - accentColorAnim.tb) > 0.5f) ||
            (std::abs(bgColorAnim.r - bgColorAnim.tr) > 0.5f);

        bool needsHighFPS = isPlayingNow || isFading || g_is_moving || g_is_attracted ||
            isFetchingNow || isColorChanging || lyricsChangeAnim.current < 0.98f || modeAnim.current > 0.01f;

        if (needsHighFPS) {
            if (!isHighResTimerActive) {
                timeBeginPeriod(1);
                isHighResTimerActive = true;
            }
            double frameTargetMs = MS_PER_FRAME;
            auto frameEnd = std::chrono::steady_clock::now();
            double elapsedMs = std::chrono::duration<double, std::milli>(frameEnd - frameStart).count();

            if (elapsedMs < frameTargetMs) {
                double remain = frameTargetMs - elapsedMs;

                if (remain >= 2.0) {
                    Sleep(static_cast<DWORD>(remain - 1.0));
                }

                while (true) {
                    auto now = std::chrono::steady_clock::now();
                    double el = std::chrono::duration<double, std::milli>(now - frameStart).count();
                    if (el >= frameTargetMs) break;
                    std::this_thread::yield();
                }
            }
        }
        else {
            if (isHighResTimerActive) {
                timeEndPeriod(1);
                isHighResTimerActive = false;
            }
            Sleep(33);
        }
    }

EXIT_MAIN_LOOP:
    if (isHighResTimerActive) {
        timeEndPeriod(1);
    }
    s_glowCaches.clear();
    s_glowCaches.shrink_to_fit();

    if (hBitmap) { SelectObject(hdcMem, hOldBitmap); DeleteObject(hBitmap); }
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    if (g_album_art) delete g_album_art;
    ::GdiplusShutdown(gdiplusToken);
    return 0;
}
