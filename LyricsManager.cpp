#include "LyricsManager.h"
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

extern void SaveConfig();
extern int g_lyrics_pos_x;
extern int g_lyrics_pos_y;

static float s_currentX = 0.0f;
static float s_currentY = 0.0f;
static float s_targetX = 0.0f;
static float s_targetY = 0.0f;

static bool s_themeDirty = false;
static float s_listVisualAlpha = 1.0f;
static float s_listTargetAlpha = 1.0f;
static float s_targetXOffset = 0.0f;
static float s_listXOffset = 0.0f;
static std::vector<ManagedTrack> s_nextFilteredTracks;
static bool s_isListSwitching = false;

static float s_placeholderVisualAlpha = 0.40f;
static float s_placeholderYOffset = 0.0f;
static float s_searchBgVisualAlpha = 0.10f;

HWND LyricsManager::s_hwnd = nullptr;
HWND LyricsManager::s_hParent = nullptr;
HINSTANCE LyricsManager::s_hInstance = nullptr;
std::wstring LyricsManager::s_lyricsDir = L"";
std::vector<ManagedTrack> LyricsManager::s_tracks;
std::vector<ManagedTrack> LyricsManager::s_filteredTracks;

int LyricsManager::s_selectedIndex = 0;
float LyricsManager::s_scrollOffset = 0.0f;
float LyricsManager::s_targetScrollOffset = 0.0f;
float LyricsManager::s_marqueeTime = 0.0f;

TrackSource LyricsManager::s_currentTab = TrackSource::All;
D2D1_RECT_F LyricsManager::s_tabAllRect = { 20, 15, 80, 40 };
D2D1_RECT_F LyricsManager::s_tabApiRect = { 85, 15, 175, 40 };
D2D1_RECT_F LyricsManager::s_tabNoApiRect = { 180, 15, 275, 40 };

std::wstring LyricsManager::s_searchQuery = L"";
bool LyricsManager::s_isSearchFocused = false;
D2D1_RECT_F LyricsManager::s_searchBoxRect = { 290, 15, 440, 40 };

std::vector<CustomButton> LyricsManager::s_buttons;
bool LyricsManager::s_isDragging = false;
POINT LyricsManager::s_dragStartPos = { 0, 0 };

int LyricsManager::s_windowX = 0;
int LyricsManager::s_windowY = 0;

float LyricsManager::s_windowAlpha = 0.0f;
float LyricsManager::s_windowScale = 1.0f;
float LyricsManager::s_windowOffsetY = 20.0f;
bool LyricsManager::s_isClosing = false;

std::wstring LyricsManager::s_statusMessage = L"Ready.";
float LyricsManager::s_statusAlpha = 1.0f;

D2D1_COLOR_F LyricsManager::s_bgColor = D2D1::ColorF(0.08f, 0.08f, 0.08f, 0.95f);
D2D1_COLOR_F LyricsManager::s_textColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
D2D1_COLOR_F LyricsManager::s_accentColor = D2D1::ColorF(29.0f / 255.0f, 185.0f / 255.0f, 84.0f / 255.0f, 1.0f);
D2D1_COLOR_F LyricsManager::s_targetAccentColor = D2D1::ColorF(29.0f / 255.0f, 185.0f / 255.0f, 84.0f / 255.0f, 1.0f);
D2D1_COLOR_F LyricsManager::s_currentAccentColor = D2D1::ColorF(29.0f / 255.0f, 185.0f / 255.0f, 84.0f / 255.0f, 1.0f);

bool LyricsManager::s_isEditing = false;
float LyricsManager::s_editModeAlpha = 0.0f;
float LyricsManager::s_editScrollOffset = 0.0f;
float LyricsManager::s_targetEditScrollOffset = 0.0f;
std::vector<EditableLrcLine> LyricsManager::s_editLines;
int LyricsManager::s_selectedEditLine = 0;
int LyricsManager::s_totalDurationMs = 180000;
bool LyricsManager::s_isSliderDragging = false;
bool LyricsManager::s_isDurationSliderDragging = false;

D2D1_RECT_F LyricsManager::s_btnTimeMinusRect = { 25, 350, 75, 380 };
D2D1_RECT_F LyricsManager::s_sliderTrackRect = { 135, 360, 220, 370 };
D2D1_RECT_F LyricsManager::s_btnTimePlusRect = { 230, 350, 280, 380 };

D2D1_RECT_F LyricsManager::s_durationSliderRect = { 80, 400, 200, 410 };
D2D1_RECT_F LyricsManager::s_addLineEditRect = { 270, 390, 340, 420 };
D2D1_RECT_F LyricsManager::s_removeLineEditRect = { 350, 390, 420, 420 };

MgrRollingTimeRenderer LyricsManager::s_lineTimeRenderer;
MgrRollingTimeRenderer LyricsManager::s_totalTimeRenderer;

Microsoft::WRL::ComPtr<ID2D1Factory> LyricsManager::s_pD2DFactory;
Microsoft::WRL::ComPtr<IDWriteFactory> LyricsManager::s_pDWriteFactory;
Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> LyricsManager::s_pRenderTarget;

Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontList;
Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontListBold;
Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontBtn;
Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontStatus;
Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontTab;
Microsoft::WRL::ComPtr<IDWriteTextFormat> LyricsManager::s_pFontDigit;

bool LyricsManager::s_hasRefetchRequest = false;
std::wstring LyricsManager::s_refetchTrackName = L"";
std::wstring LyricsManager::s_refetchArtistName = L"";
bool LyricsManager::s_refetchIsUnknown = false;

HDC LyricsManager::s_hdcMem = nullptr;
HBITMAP LyricsManager::s_hBitmap = nullptr;
HBITMAP LyricsManager::s_hOldBitmap = nullptr;

#define BTN_ADD_EDIT 0
#define BTN_REFETCH 1
#define BTN_DELETE 2
#define BTN_DELETE_ALL 3
#define BTN_CLOSE 4
#define BTN_TIME_MINUS 5
#define BTN_TIME_PLUS 6
#define BTN_ADD_LINE 7
#define BTN_REMOVE_LINE 8
#define BTN_TOGGLE_API 9

bool LyricsManager::PopRefetchRequest(std::wstring& outTrackName, std::wstring& outArtistName, bool& outIsUnknown) {
    if (!s_hasRefetchRequest) return false;
    outTrackName = s_refetchTrackName;
    outArtistName = s_refetchArtistName;
    outIsUnknown = s_refetchIsUnknown;
    s_hasRefetchRequest = false;
    return true;
}

void LyricsManager::Refresh() {
    if (s_hwnd) {
        LoadTrackList();
        UpdateFilteredTracks();
    }
}

void LyricsManager::Initialize(HINSTANCE hInstance, HWND hParent, const std::wstring& lyricsDir) {
    s_hInstance = hInstance; s_hParent = hParent; s_lyricsDir = lyricsDir;
    WNDCLASS wc = { 0 }; wc.lpfnWndProc = WndProc; wc.hInstance = hInstance;
    wc.lpszClassName = L"SpotifyLyricsManager"; wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wc);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, s_pD2DFactory.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(s_pDWriteFactory.GetAddressOf()));

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
    s_pD2DFactory->CreateDCRenderTarget(&props, s_pRenderTarget.GetAddressOf());

    s_pDWriteFactory->CreateTextFormat(L"Yu Gothic UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"ja-jp", &s_pFontList);
    s_pDWriteFactory->CreateTextFormat(L"Yu Gothic UI", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"ja-jp", &s_pFontListBold);
    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-us", &s_pFontBtn);
    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontStatus);

    s_pDWriteFactory->CreateTextFormat(L"Yu Gothic UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"ja-jp", &s_pFontTab);
    s_pFontTab->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    s_pFontTab->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontDigit);
    s_pFontDigit->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    s_pFontDigit->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontList->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    s_pFontListBold->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    s_pFontBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    s_pFontBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    s_pFontStatus->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
}

void LyricsManager::UpdateTheme(D2D1_COLOR_F bg, D2D1_COLOR_F text, D2D1_COLOR_F accent) {
    if (std::abs(s_bgColor.r - bg.r) > 0.001f || std::abs(s_bgColor.g - bg.g) > 0.001f || std::abs(s_bgColor.b - bg.b) > 0.001f ||
        std::abs(s_textColor.r - text.r) > 0.001f || std::abs(s_textColor.g - text.g) > 0.001f || std::abs(s_textColor.b - text.b) > 0.001f ||
        std::abs(s_targetAccentColor.r - accent.r) > 0.001f || std::abs(s_targetAccentColor.g - accent.g) > 0.001f || std::abs(s_targetAccentColor.b - accent.b) > 0.001f) {
        s_themeDirty = true;
    }
    s_bgColor = bg;
    s_textColor = text;
    s_targetAccentColor = accent;
}

void LyricsManager::SetStatus(const std::wstring& msg) {
    s_statusMessage = msg; s_statusAlpha = 1.0f;
}

void LyricsManager::ToggleWindow() {
    if (s_hwnd) {
        s_isClosing = true;
        return;
    }

    timeBeginPeriod(1);

    s_isClosing = false;
    s_isEditing = false;
    s_editModeAlpha = 0.0f;
    s_windowAlpha = 0.0f;
    s_windowScale = 0.95f;

    int w = 700; int h = 450;
    int sw = GetSystemMetrics(SM_CXSCREEN); int sh = GetSystemMetrics(SM_CYSCREEN);

    if (g_lyrics_pos_x != -9999 && g_lyrics_pos_y != -9999) {
        s_windowX = g_lyrics_pos_x;
        s_windowY = g_lyrics_pos_y;
    }
    else {
        s_windowX = (sw - w) / 2;
        s_windowY = (sh - h) / 2;
    }
    s_windowOffsetY = sh / 2.0f;

    s_currentX = (float)s_windowX;
    s_currentY = (float)s_windowY;
    s_targetX = (float)s_windowX;
    s_targetY = (float)s_windowY;

    s_listVisualAlpha = 1.0f;
    s_listTargetAlpha = 1.0f;
    s_listXOffset = 0.0f;
    s_targetXOffset = 0.0f;
    s_isListSwitching = false;
    s_placeholderVisualAlpha = 0.40f;
    s_placeholderYOffset = 0.0f;
    s_searchBgVisualAlpha = 0.10f;

    HDC hdcScreen = GetDC(nullptr);
    s_hdcMem = CreateCompatibleDC(hdcScreen);
    s_hBitmap = CreateCompatibleBitmap(hdcScreen, w, h);
    s_hOldBitmap = (HBITMAP)SelectObject(s_hdcMem, s_hBitmap);
    ReleaseDC(nullptr, hdcScreen);

    s_hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"SpotifyLyricsManager", L"",
        WS_POPUP, s_windowX, s_windowY, w, h, s_hParent, nullptr, s_hInstance, nullptr);

    s_buttons.clear();
    s_buttons.push_back({ D2D1::RectF(460, 65, 640, 105), L"Add", false, 0.0f, 1.0f, BTN_ADD_EDIT, true });
    s_buttons.push_back({ D2D1::RectF(460, 120, 640, 160), L"Re-fetch", false, 0.0f, 1.0f, BTN_REFETCH, true });
    s_buttons.push_back({ D2D1::RectF(460, 175, 640, 215), L"Delete", false, 0.0f, 1.0f, BTN_DELETE, true });
    s_buttons.push_back({ D2D1::RectF(460, 230, 640, 270), L"Remove All", false, 0.0f, 1.0f, BTN_DELETE_ALL, true });
    s_buttons.push_back({ D2D1::RectF(650, 15, 685, 50), L"", false, 0.0f, 1.0f, BTN_CLOSE, true });

    s_buttons.push_back({ s_btnTimeMinusRect, L"-0.1s", false, 0.0f, 1.0f, BTN_TIME_MINUS, true });
    s_buttons.push_back({ s_btnTimePlusRect, L"+0.1s", false, 0.0f, 1.0f, BTN_TIME_PLUS, true });
    s_buttons.push_back({ s_addLineEditRect, L"+ Line", false, 0.0f, 1.0f, BTN_ADD_LINE, true });
    s_buttons.push_back({ s_removeLineEditRect, L"- Line", false, 0.0f, 1.0f, BTN_REMOVE_LINE, true });
    s_buttons.push_back({ D2D1::RectF(290, 350, 420, 380), L"Type: API", false, 0.0f, 1.0f, BTN_TOGGLE_API, true });

    LoadTrackList();
    s_searchQuery = L"";
    s_filteredTracks = s_tracks;

    s_selectedIndex = 0; s_scrollOffset = 0.0f; s_targetScrollOffset = 0.0f;
    s_editScrollOffset = 0.0f; s_targetEditScrollOffset = 0.0f;
    s_marqueeTime = 0.0f;

    ShowWindow(s_hwnd, SW_SHOW);
    SetTimer(s_hwnd, 1, 10, nullptr);
}

bool LyricsManager::IsOpen() { return s_hwnd != nullptr && !s_isClosing; }

void LyricsManager::LoadTrackList() {
    s_tracks.clear();

    for (const auto& entry : std::filesystem::directory_iterator(s_lyricsDir)) {
        if (entry.path().extension() == L".lrc") {
            std::wstring filePath = entry.path().wstring();
            std::wstring trackName = entry.path().stem().wstring();

            TrackSource src = TrackSource::All;
            std::ifstream ifs(filePath);
            if (ifs.is_open()) {
                std::string line;

                for (int i = 0; i < 5 && std::getline(ifs, line); ++i) {
                    if (line.find("Spotify API") != std::string::npos) {
                        src = TrackSource::SpotifyApi;
                        break;
                    }
                    else if (line.find("No Use API") != std::string::npos) {
                        src = TrackSource::NoApi;
                        break;
                    }
                }
            }

            if (trackName.find(L"NoAPI_") == 0) {
                if (src == TrackSource::All) src = TrackSource::NoApi;
                trackName = trackName.substr(6);
            }

            s_tracks.push_back({ trackName, L"", false, src, filePath });
        }
    }

    std::ifstream ifs(s_lyricsDir + L"\\unknowns.txt");
    if (ifs.is_open()) {
        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;

            TrackSource src = TrackSource::All;
            if (line.find("API: Yes") != std::string::npos) {
                src = TrackSource::SpotifyApi;
            }
            else if (line.find("API: No") != std::string::npos) {
                src = TrackSource::NoApi;
            }

            std::string cleanLine = line;
            size_t apiPos = cleanLine.find(" | API:");
            if (apiPos != std::string::npos) {
                cleanLine = cleanLine.substr(0, apiPos);
            }

            int size = MultiByteToWideChar(CP_UTF8, 0, cleanLine.c_str(), (int)cleanLine.size(), NULL, 0);
            std::wstring wline(size, 0);
            MultiByteToWideChar(CP_UTF8, 0, cleanLine.c_str(), (int)cleanLine.size(), &wline[0], size);

            s_tracks.push_back({ wline, L"", true, src, L"" });
        }
    }
}

void LyricsManager::UpdateFilteredTracks() {
    s_nextFilteredTracks.clear();
    std::wstring queryLower = s_searchQuery;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::towlower);

    for (const auto& t : s_tracks) {
        if (s_currentTab == TrackSource::SpotifyApi && t.source != TrackSource::SpotifyApi) continue;
        if (s_currentTab == TrackSource::NoApi && t.source != TrackSource::NoApi) continue;

        if (!queryLower.empty()) {
            std::wstring nameLower = t.trackName;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::towlower);
            if (nameLower.find(queryLower) == std::wstring::npos) continue;
        }
        s_nextFilteredTracks.push_back(t);
    }

    bool isSame = (s_filteredTracks.size() == s_nextFilteredTracks.size());
    if (isSame) {
        for (size_t i = 0; i < s_filteredTracks.size(); ++i) {
            if (s_filteredTracks[i].trackName != s_nextFilteredTracks[i].trackName ||
                s_filteredTracks[i].isUnknown != s_nextFilteredTracks[i].isUnknown) {
                isSame = false; break;
            }
        }
    }
    if (isSame) return;

    s_filteredTracks = s_nextFilteredTracks;
    s_selectedIndex = 0;
    s_scrollOffset = 0.0f;
    s_targetScrollOffset = 0.0f;
    s_marqueeTime = 0.0f;

    s_listVisualAlpha = 0.0f;
    s_listTargetAlpha = 1.0f;
    s_listXOffset = -30.0f;
    s_targetXOffset = 0.0f;
    s_isListSwitching = false;
}

void LyricsManager::LoadLrcForEditing(const std::wstring& filePath) {
    s_editLines.clear();
    s_totalDurationMs = 0;

    if (filePath.empty() || !std::filesystem::exists(filePath)) {
        s_editLines.push_back({ 0, L"New Lyrics Line 1" });
        s_selectedEditLine = 0;
        s_totalDurationMs = 180000;
        s_editScrollOffset = 0.0f;
        s_targetEditScrollOffset = 0.0f;
        return;
    }

    std::ifstream ifs(filePath);
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.find("[length:") == 0) {
            size_t endB = line.find(']');
            if (endB != std::string::npos) {
                int min = 0; float sec = 0.0f;
                if (sscanf_s(line.substr(8, endB - 8).c_str(), "%d:%f", &min, &sec) == 2) {
                    s_totalDurationMs = min * 60000 + static_cast<int>(sec * 1000);
                }
            }
            continue;
        }

        if (line.size() >= 10 && line[0] == '[') {
            size_t endB = line.find(']');
            if (endB != std::string::npos && endB >= 9) {
                std::string timeStr = line.substr(1, endB - 1);
                std::string textStr = line.substr(endB + 1);
                int min = 0; float sec = 0.0f;
                if (sscanf_s(timeStr.c_str(), "%d:%f", &min, &sec) == 2) {
                    int size = MultiByteToWideChar(CP_UTF8, 0, textStr.c_str(), -1, NULL, 0);
                    std::wstring wtext(size - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, textStr.c_str(), -1, &wtext[0], size);
                    s_editLines.push_back({ min * 60000 + static_cast<int>(sec * 1000), wtext });
                }
            }
        }
    }

    if (s_totalDurationMs <= 0) {
        if (!s_editLines.empty()) {
            s_totalDurationMs = s_editLines.back().timeMs + 5000;
        }
        else {
            s_totalDurationMs = 180000;
        }
    }

    if (s_editLines.empty()) {
        s_editLines.push_back({ 0, L"Sample Lyrics Line" });
    }
    s_selectedEditLine = 0;
    s_editScrollOffset = 0.0f;
    s_targetEditScrollOffset = 0.0f;
}

void LyricsManager::SaveLrcFromEditing(const ManagedTrack& track) {
    std::wstring savePath = track.filePath;
    if (savePath.empty()) {
        std::wstring cleanName = track.trackName;

        if (track.isUnknown) {
            size_t tPos = cleanName.find(L"Track: ");
            size_t aPos = cleanName.find(L" | Artist: ");
            if (tPos != std::wstring::npos && aPos != std::wstring::npos) {
                std::wstring tPart = cleanName.substr(tPos + 7, aPos - (tPos + 7));
                std::wstring aPart = cleanName.substr(aPos + 11);
                cleanName = aPart + L" - " + tPart;
            }
        }

        if (track.source == TrackSource::NoApi) {
            cleanName = L"NoAPI_" + cleanName;
        }
        std::wstring invalid = L"\\/:*?\"<>|";
        for (wchar_t& c : cleanName) if (invalid.find(c) != std::wstring::npos) c = L'_';
        savePath = s_lyricsDir + L"\\" + cleanName + L".lrc";
    }

    std::sort(s_editLines.begin(), s_editLines.end(), [](const EditableLrcLine& a, const EditableLrcLine& b) {
        return a.timeMs < b.timeMs;
        });

    if (!s_editLines.empty() && s_totalDurationMs < s_editLines.back().timeMs) {
        s_totalDurationMs = s_editLines.back().timeMs;
    }
    if (s_totalDurationMs <= 0) s_totalDurationMs = 180000;

    std::ofstream ofs(savePath, std::ios::binary | std::ios::trunc);
    if (ofs.is_open()) {
        std::string srcStr = (track.source == TrackSource::NoApi) ? "No Use API (GSMTC)" : "Spotify API";
        ofs << "[by: " << srcStr << "]\r\n";

        int totSec = s_totalDurationMs / 1000;
        int tMin = totSec / 60;
        int tSec = totSec % 60;
        int tMs10 = (s_totalDurationMs % 1000) / 10;
        std::ostringstream ssTot;
        ssTot << "[length: " << std::setfill('0') << std::setw(2) << tMin << ":"
            << std::setfill('0') << std::setw(2) << tSec << "."
            << std::setfill('0') << std::setw(2) << tMs10 << "]\r\n";
        ofs << ssTot.str();

        for (const auto& line : s_editLines) {
            int totalSec = line.timeMs / 1000;
            int min = totalSec / 60;
            int sec = totalSec % 60;
            int ms10 = (line.timeMs % 1000) / 10;

            std::ostringstream ss;
            ss << "[" << std::setfill('0') << std::setw(2) << min << ":"
                << std::setfill('0') << std::setw(2) << sec << "."
                << std::setfill('0') << std::setw(2) << ms10 << "]";

            int size = WideCharToMultiByte(CP_UTF8, 0, line.text.c_str(), -1, NULL, 0, NULL, NULL);
            std::string utf8Text(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, line.text.c_str(), -1, &utf8Text[0], size, NULL, NULL);

            std::string fullLine = ss.str() + utf8Text + "\r\n";
            ofs.write(fullLine.c_str(), fullLine.size());
        }
        ofs.close();
    }

    if (track.isUnknown) {
        std::wstring unknownFilePath = s_lyricsDir + L"\\unknowns.txt";
        std::vector<std::string> lines;
        std::ifstream ifs(unknownFilePath);
        if (ifs.is_open()) {
            std::string line;
            int size = WideCharToMultiByte(CP_UTF8, 0, track.trackName.c_str(), -1, NULL, 0, NULL, NULL);
            std::string targetLineUtf8(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, track.trackName.c_str(), -1, &targetLineUtf8[0], size, NULL, NULL);
            while (std::getline(ifs, line)) {

                if (line.find(targetLineUtf8) == std::string::npos && !line.empty()) {
                    lines.push_back(line);
                }
            }
            ifs.close();
        }
        std::ofstream uofs(unknownFilePath, std::ios::trunc);
        if (uofs.is_open()) {
            for (const auto& l : lines) uofs << l << "\n";
            uofs.close();
        }
    }
}

void LyricsManager::DrawRollingTime(MgrRollingTimeRenderer& timer, ID2D1Brush* pBrush, float startX, float startY, float digitW, float digitH) {
    auto renderDigit = [&](MgrRollingDigitAnimation& anim, float dx, int base) {
        float val = anim.current;
        int num1 = static_cast<int>(floor(val)) % base; if (num1 < 0) num1 += base;
        int num2 = (num1 + 1) % base;
        float off = val - floor(val);

        D2D1_RECT_F digitRect = D2D1::RectF(dx, startY, dx + digitW, startY + digitH);
        s_pRenderTarget->PushAxisAlignedClip(&digitRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        std::wstring n1 = std::to_wstring(num1);
        std::wstring n2 = std::to_wstring(num2);

        D2D1_RECT_F r1 = D2D1::RectF(dx, startY - off * digitH, dx + digitW, startY + digitH - off * digitH);
        s_pRenderTarget->DrawTextW(n1.c_str(), (UINT32)n1.length(), s_pFontDigit.Get(), &r1, pBrush);

        if (off > 0.005f) {
            D2D1_RECT_F r2 = D2D1::RectF(dx, startY + digitH - off * digitH, dx + digitW, startY + digitH * 2.0f - off * digitH);
            s_pRenderTarget->DrawTextW(n2.c_str(), (UINT32)n2.length(), s_pFontDigit.Get(), &r2, pBrush);
        }

        s_pRenderTarget->PopAxisAlignedClip();
        };

    renderDigit(timer.m1, startX, 10);
    renderDigit(timer.m2, startX + digitW, 10);

    D2D1_RECT_F colonRect = D2D1::RectF(startX + digitW * 2.0f, startY, startX + digitW * 2.6f, startY + digitH);
    s_pRenderTarget->DrawTextW(L":", 1, s_pFontDigit.Get(), &colonRect, pBrush);

    renderDigit(timer.s1, startX + digitW * 2.6f, 6);
    renderDigit(timer.s2, startX + digitW * 3.6f, 10);
}

namespace {
    struct MgrGlowCacheItem {
        std::wstring text;
        IDWriteTextFormat* font = nullptr;
        float w = 0.0f, h = 0.0f;
        BYTE gr = 0, gg = 0, gb = 0;
        Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pBmpRT;
        Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap;
    };
    static std::vector<std::shared_ptr<MgrGlowCacheItem>> s_mgrGlowCaches;
    static BYTE s_mgrLastR = 0, s_mgrLastG = 0, s_mgrLastB = 0;

    ID2D1Bitmap* GetOrCreateMgrDeepGlowBitmap(
        ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        const std::wstring& text, IDWriteTextFormat* font,
        float w, float h, BYTE gr, BYTE gg, BYTE gb)
    {
        if (std::abs((int)gr - (int)s_mgrLastR) > 3 ||
            std::abs((int)gg - (int)s_mgrLastG) > 3 ||
            std::abs((int)gb - (int)s_mgrLastB) > 3) {
            s_mgrGlowCaches.clear();
            s_mgrLastR = gr; s_mgrLastG = gg; s_mgrLastB = gb;
        }

        for (auto& item : s_mgrGlowCaches) {
            if (item->font == font && item->text == text &&
                std::abs(item->w - w) < 1.0f && std::abs(item->h - h) < 1.0f) {
                return item->pBitmap.Get();
            }
        }

        if (s_mgrGlowCaches.size() > 120) {
            s_mgrGlowCaches.erase(s_mgrGlowCaches.begin(), s_mgrGlowCaches.begin() + 40);
        }

        auto newItem = std::make_shared<MgrGlowCacheItem>();
        newItem->text = text;
        newItem->font = font;
        newItem->w = w;
        newItem->h = h;
        newItem->gr = s_mgrLastR;
        newItem->gg = s_mgrLastG;
        newItem->gb = s_mgrLastB;

        float pad = 48.0f;
        float cacheW = w + pad * 2.0f;
        float cacheH = h + pad * 2.0f;
        D2D1_SIZE_F rtSize = D2D1::SizeF(cacheW, cacheH);

        Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pBaseRT;
        if (FAILED(target->CreateCompatibleRenderTarget(rtSize, &pBaseRT)) || !pBaseRT) return nullptr;

        pBaseRT->BeginDraw();
        pBaseRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        Microsoft::WRL::ComPtr<IDWriteTextLayout> pLayout;
        writeFactory->CreateTextLayout(text.c_str(), (UINT32)text.length(), font, w, h, &pLayout);
        if (pLayout) {
            pLayout->SetTextAlignment(font->GetTextAlignment());
            pLayout->SetParagraphAlignment(font->GetParagraphAlignment());
            pLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> baseBrush;
            pBaseRT->CreateSolidColorBrush(D2D1::ColorF(gr / 255.0f, gg / 255.0f, gb / 255.0f, 0.95f), &baseBrush);
            pBaseRT->DrawTextLayout(D2D1::Point2F(pad, pad), pLayout.Get(), baseBrush.Get());
        }
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

        s_mgrGlowCaches.push_back(newItem);
        return newItem->pBitmap.Get();
    }
}

void LyricsManager::Render(HWND hwnd) {
    if (!s_pRenderTarget || !s_hdcMem) return;
    int w = 700; int h = 450;

    static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBtnGlowBrush;
    static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBtnBorderBrush;
    static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBtnTextBrush;

    HDC hdcScreen = GetDC(nullptr);

    RECT bindRc = { 0, 0, w, h };
    s_pRenderTarget->BindDC(s_hdcMem, &bindRc);
    s_pRenderTarget->BeginDraw();
    s_pRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

    s_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    s_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    D2D1_MATRIX_3X2_F windowMat = D2D1::Matrix3x2F::Scale(s_windowScale, s_windowScale, D2D1::Point2F(w / 2.0f, h / 2.0f));
    s_pRenderTarget->SetTransform(windowMat);

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBgBrush, pTextBrush, pAccentBrush, pDisabledBrush;
    s_pRenderTarget->CreateSolidColorBrush(s_bgColor, &pBgBrush);
    s_pRenderTarget->CreateSolidColorBrush(s_textColor, &pTextBrush);
    s_pRenderTarget->CreateSolidColorBrush(s_currentAccentColor, &pAccentBrush);
    s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_textColor.r, s_textColor.g, s_textColor.b, 0.25f), &pDisabledBrush);

    BYTE accR = static_cast<BYTE>(std::clamp(s_currentAccentColor.r * 255.0f, 0.0f, 255.0f));
    BYTE accG = static_cast<BYTE>(std::clamp(s_currentAccentColor.g * 255.0f, 0.0f, 255.0f));
    BYTE accB = static_cast<BYTE>(std::clamp(s_currentAccentColor.b * 255.0f, 0.0f, 255.0f));

    auto DrawSimpleBoxGlow = [&](const D2D1_RECT_F& rect, float radius, float intensity, D2D1_COLOR_F glowCol) {
        if (intensity <= 0.01f) return;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> gBrush;
        for (int i = 6; i >= 1; --i) {
            float offset = (float)i * 1.5f;
            D2D1_ROUNDED_RECT gRect = D2D1::RoundedRect(
                D2D1::RectF(rect.left - offset, rect.top - offset, rect.right + offset, rect.bottom + offset),
                radius + offset, radius + offset);
            float a = (7.0f - (float)i) * 0.025f * intensity;
            s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(glowCol.r, glowCol.g, glowCol.b, a), &gBrush);
            s_pRenderTarget->FillRoundedRectangle(&gRect, gBrush.Get());
        }
        };

    auto DrawModernLyricsStyleText = [&](const std::wstring& text, IDWriteTextFormat* font, const D2D1_RECT_F& rect, float glowIntensity, float baseAlpha) {
        if (baseAlpha <= 0.01f || text.empty()) return;
        float rw = std::min(420.0f, rect.right - rect.left);
        float rh = rect.bottom - rect.top;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> dimBrush;
        float dimAlpha = baseAlpha * (1.0f - glowIntensity * 0.65f);
        s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_textColor.r, s_textColor.g, s_textColor.b, dimAlpha), &dimBrush);
        s_pRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), font, &rect, dimBrush.Get());

        if (glowIntensity > 0.01f) {
            float glowAlpha = std::clamp(baseAlpha * glowIntensity, 0.0f, 1.0f);
            ID2D1Bitmap* pGlowBmp = GetOrCreateMgrDeepGlowBitmap(s_pRenderTarget.Get(), s_pDWriteFactory.Get(), text, font, rw, rh, accR, accG, accB);
            if (pGlowBmp) {
                float pad = 48.0f;
                D2D1_RECT_F destRect = D2D1::RectF(rect.left - pad, rect.top - pad, rect.left + rw + pad, rect.top + rh + pad);
                s_pRenderTarget->DrawBitmap(pGlowBmp, destRect, glowAlpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }

            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brightWhiteBrush;
            s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, glowAlpha), &brightWhiteBrush);
            s_pRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), font, &rect, brightWhiteBrush.Get());
        }
        };

    D2D1_ROUNDED_RECT roundedWindow = D2D1::RoundedRect(D2D1::RectF(0, 0, (float)w, (float)h), 12.0f, 12.0f);
    s_pRenderTarget->FillRoundedRectangle(&roundedWindow, pBgBrush.Get());

    float uiAlpha = s_isEditing ? 0.3f : 1.0f;

    auto drawTab = [&](D2D1_RECT_F rect, const wchar_t* label, TrackSource src) {
        D2D1_ROUNDED_RECT rRect = D2D1::RoundedRect(rect, 4.0f, 4.0f);
        bool isActive = (s_currentTab == src);
        float glowAmt = isActive ? (1.0f * uiAlpha) : 0.0f;
        DrawSimpleBoxGlow(rect, 4.0f, glowAmt * 0.7f, s_currentAccentColor);

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> tBgBrush;
        s_pRenderTarget->CreateSolidColorBrush(isActive ? D2D1::ColorF(s_currentAccentColor.r, s_currentAccentColor.g, s_currentAccentColor.b, 0.22f * uiAlpha) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.05f * uiAlpha), &tBgBrush);
        s_pRenderTarget->FillRoundedRectangle(&rRect, tBgBrush.Get());
        if (isActive) s_pRenderTarget->DrawRoundedRectangle(&rRect, pAccentBrush.Get(), 1.0f);

        DrawModernLyricsStyleText(label, s_pFontTab.Get(), rect, glowAmt, isActive ? uiAlpha : (0.6f * uiAlpha));
        };
    drawTab(s_tabAllRect, L"All", TrackSource::All);
    drawTab(s_tabApiRect, L"Spotify API", TrackSource::SpotifyApi);
    drawTab(s_tabNoApiRect, L"No Use API", TrackSource::NoApi);

    D2D1_ROUNDED_RECT searchBg = D2D1::RoundedRect(s_searchBoxRect, 6.0f, 6.0f);
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pSearchBgBrush;
    s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_textColor.r, s_textColor.g, s_textColor.b, s_searchBgVisualAlpha * uiAlpha), &pSearchBgBrush);
    s_pRenderTarget->FillRoundedRectangle(&searchBg, pSearchBgBrush.Get());

    if (s_isSearchFocused && !s_isEditing) {
        DrawSimpleBoxGlow(s_searchBoxRect, 6.0f, 0.8f, s_currentAccentColor);
        s_pRenderTarget->DrawRoundedRectangle(&searchBg, pAccentBrush.Get(), 1.5f);
    }

    D2D1_RECT_F textRectSearch = D2D1::RectF(s_searchBoxRect.left + 10.0f, s_searchBoxRect.top + 2.0f + s_placeholderYOffset, s_searchBoxRect.right - 10.0f, s_searchBoxRect.bottom);
    if (s_placeholderVisualAlpha > 0.005f) {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pPlaceholderBrush;
        s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_textColor.r, s_textColor.g, s_textColor.b, s_placeholderVisualAlpha * uiAlpha), &pPlaceholderBrush);
        s_pRenderTarget->DrawTextW(L"Search...", 9, s_pFontStatus.Get(), &textRectSearch, pPlaceholderBrush.Get());
    }

    if (!s_searchQuery.empty() || s_isSearchFocused) {
        std::wstring dispQuery = s_searchQuery;
        if (s_isSearchFocused && std::fmod(s_marqueeTime, 1.0f) < 0.5f) dispQuery += L"|";
        D2D1_RECT_F normalInputRect = D2D1::RectF(s_searchBoxRect.left + 10.0f, s_searchBoxRect.top + 2.0f, s_searchBoxRect.right - 10.0f, s_searchBoxRect.bottom);
        s_pRenderTarget->DrawTextW(dispQuery.c_str(), dispQuery.length(), s_pFontStatus.Get(), &normalInputRect, pTextBrush.Get());
    }

    float listWidth = 410.0f;

    if (!s_isEditing) {
        float startY = (h / 2.0f) - (s_scrollOffset * 45.0f);
        s_pRenderTarget->PushAxisAlignedClip(D2D1::RectF(15, 50, listWidth + 25, h - 20), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        D2D1_MATRIX_3X2_F listAnimMat = D2D1::Matrix3x2F::Translation(s_listXOffset, 0.0f);

        for (int i = 0; i < (int)s_filteredTracks.size(); ++i) {
            float diff = (float)i - s_scrollOffset;
            float absDiff = std::abs(diff);
            float y = startY + (i * 45.0f);
            if (y < -50.0f || y > h + 50.0f) continue;

            float highlightRatio = std::max(0.0f, 1.0f - absDiff);
            float itemScale = std::max(0.85f, 1.0f - (absDiff * 0.03f));
            float itemAlpha = std::max(0.15f, 1.0f - (absDiff * 0.25f));

            float cx = 35.0f + listWidth / 2.0f; float cy = y + 20.0f;
            s_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(itemScale, itemScale, D2D1::Point2F(cx, cy)) * listAnimMat * windowMat);

            float finalAlpha = (itemAlpha + (1.0f - itemAlpha) * highlightRatio) * s_listVisualAlpha;

            if (highlightRatio > 0.01f) {
                D2D1_RECT_F itemBox = D2D1::RectF(25.0f, y - 2.0f, listWidth, y + 42.0f);
                DrawSimpleBoxGlow(itemBox, 6.0f, highlightRatio * s_listVisualAlpha * 0.65f, s_currentAccentColor);

                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pItemBgBrush, pAccentLineBrush;
                s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.05f * highlightRatio * s_listVisualAlpha), &pItemBgBrush);
                D2D1_ROUNDED_RECT itemBgRect = D2D1::RoundedRect(itemBox, 6.0f, 6.0f);
                s_pRenderTarget->FillRoundedRectangle(&itemBgRect, pItemBgBrush.Get());

                s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_currentAccentColor.r, s_currentAccentColor.g, s_currentAccentColor.b, highlightRatio * s_listVisualAlpha), &pAccentLineBrush);
                s_pRenderTarget->DrawRoundedRectangle(&itemBgRect, pAccentLineBrush.Get(), 1.0f);

                D2D1_ROUNDED_RECT lineRect = D2D1::RoundedRect(D2D1::RectF(25.0f, y + 8.0f, 28.5f, y + 32.0f), 1.5f, 1.5f);
                s_pRenderTarget->FillRoundedRectangle(&lineRect, pAccentLineBrush.Get());
            }

            auto fontToUse = (highlightRatio > 0.5f) ? s_pFontListBold.Get() : s_pFontList.Get();
            D2D1_RECT_F textRect = D2D1::RectF(40.0f, y + 8.0f, listWidth - 10.0f, y + 34.0f);

            s_pRenderTarget->PushAxisAlignedClip(D2D1::RectF(32.0f, y - 10.0f, listWidth - 8.0f, y + 50.0f), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            DrawModernLyricsStyleText(s_filteredTracks[i].trackName, fontToUse, textRect, highlightRatio, finalAlpha);
            s_pRenderTarget->PopAxisAlignedClip();
        }
        s_pRenderTarget->PopAxisAlignedClip();
    }
    else {

        s_pRenderTarget->SetTransform(windowMat);

        D2D1_ROUNDED_RECT editAreaBg = D2D1::RoundedRect(D2D1::RectF(20, 50, listWidth + 20, 335), 8.0f, 8.0f);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pAreaBgBrush;
        s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.4f * s_editModeAlpha), &pAreaBgBrush);
        s_pRenderTarget->FillRoundedRectangle(&editAreaBg, pAreaBgBrush.Get());

        s_pRenderTarget->PushAxisAlignedClip(D2D1::RectF(25, 55, listWidth + 15, 330), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        float startY = 60.0f - (s_editScrollOffset * 36.0f);
        for (int i = 0; i < (int)s_editLines.size(); ++i) {
            float lineY = startY + (i * 36.0f);
            if (lineY < 10.0f || lineY > 370.0f) continue;

            bool isSelected = (i == s_selectedEditLine);
            D2D1_RECT_F rowRect = D2D1::RectF(30, lineY, listWidth + 10, lineY + 32);

            if (isSelected) {
                DrawSimpleBoxGlow(rowRect, 4.0f, 0.7f * s_editModeAlpha, s_currentAccentColor);
                D2D1_ROUNDED_RECT selR = D2D1::RoundedRect(rowRect, 4.0f, 4.0f);
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pSelBrush;
                s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_currentAccentColor.r, s_currentAccentColor.g, s_currentAccentColor.b, 0.2f * s_editModeAlpha), &pSelBrush);
                s_pRenderTarget->FillRoundedRectangle(&selR, pSelBrush.Get());
                s_pRenderTarget->DrawRoundedRectangle(&selR, pAccentBrush.Get(), 1.0f);
            }

            int totalSec = s_editLines[i].timeMs / 1000;
            int min = totalSec / 60;
            int sec = totalSec % 60;
            int ms10 = (s_editLines[i].timeMs % 1000) / 10;
            wchar_t timeBuf[32];
            swprintf_s(timeBuf, L"[%02d:%02d.%02d]", min, sec, ms10);

            D2D1_RECT_F tRect = D2D1::RectF(35, lineY + 4, 110, lineY + 28);
            s_pRenderTarget->DrawTextW(timeBuf, (UINT32)wcslen(timeBuf), s_pFontStatus.Get(), &tRect, isSelected ? pAccentBrush.Get() : pDisabledBrush.Get());

            std::wstring lineText = s_editLines[i].text;
            if (isSelected && std::fmod(s_marqueeTime, 1.0f) < 0.5f) lineText += L"|";
            D2D1_RECT_F txtRect = D2D1::RectF(115, lineY + 4, listWidth, lineY + 28);
            DrawModernLyricsStyleText(lineText, s_pFontStatus.Get(), txtRect, isSelected ? 1.0f : 0.0f, s_editModeAlpha);
        }
        s_pRenderTarget->PopAxisAlignedClip();

        if (s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
            s_pRenderTarget->DrawLine(
                D2D1::Point2F(s_sliderTrackRect.left, s_sliderTrackRect.top + 5),
                D2D1::Point2F(s_sliderTrackRect.right, s_sliderTrackRect.top + 5), pDisabledBrush.Get(), 4.0f
            );

            float maxMs = (s_totalDurationMs > 0) ? (float)s_totalDurationMs : 180000.0f;
            float currentMs = (float)s_editLines[s_selectedEditLine].timeMs;
            float ratio = std::clamp(currentMs / maxMs, 0.0f, 1.0f);
            float thumbX = s_sliderTrackRect.left + (s_sliderTrackRect.right - s_sliderTrackRect.left) * ratio;

            D2D1_ELLIPSE thumb = D2D1::Ellipse(D2D1::Point2F(thumbX, s_sliderTrackRect.top + 5), 6.0f, 6.0f);
            s_pRenderTarget->FillEllipse(&thumb, pAccentBrush.Get());

            s_lineTimeRenderer.update(s_editLines[s_selectedEditLine].timeMs / 1000);
            DrawRollingTime(s_lineTimeRenderer, pTextBrush.Get(), 80.0f, s_sliderTrackRect.top - 3.0f, 10.0f, 16.0f);

            D2D1_RECT_F durLabelRect = { 30, 395, 75, 415 };
            s_pRenderTarget->DrawTextW(L"Total:", 6, s_pFontStatus.Get(), &durLabelRect, pDisabledBrush.Get());

            s_pRenderTarget->DrawLine(
                D2D1::Point2F(s_durationSliderRect.left, s_durationSliderRect.top + 5),
                D2D1::Point2F(s_durationSliderRect.right, s_durationSliderRect.top + 5), pDisabledBrush.Get(), 3.0f
            );
            float durRatio = std::clamp((float)s_totalDurationMs / 600000.0f, 0.0f, 1.0f);
            float durThumbX = s_durationSliderRect.left + (s_durationSliderRect.right - s_durationSliderRect.left) * durRatio;
            D2D1_ELLIPSE durThumb = D2D1::Ellipse(D2D1::Point2F(durThumbX, s_durationSliderRect.top + 5), 5.0f, 5.0f);
            s_pRenderTarget->FillEllipse(&durThumb, pAccentBrush.Get());

            s_totalTimeRenderer.update(s_totalDurationMs / 1000);
            DrawRollingTime(s_totalTimeRenderer, pAccentBrush.Get(), 210.0f, 395.0f, 10.0f, 16.0f);
        }
    }

    s_pRenderTarget->SetTransform(windowMat);

    if (!s_isEditing && s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
        s_buttons[BTN_ADD_EDIT].text = s_filteredTracks[s_selectedIndex].isUnknown ? L"Add" : L"Edit";
    }

    for (auto& btn : s_buttons) {
        bool isSubBtn = (btn.id >= BTN_TIME_MINUS);
        if (isSubBtn && (!s_isEditing || s_selectedEditLine < 0 || s_selectedEditLine >= (int)s_editLines.size())) {
            continue;
        }

        if (btn.id == BTN_TOGGLE_API && s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
            btn.text = (s_filteredTracks[s_selectedIndex].source == TrackSource::NoApi) ? L"Type: NoAPI" : L"Type: API";
        }

        float cx = btn.rect.left + (btn.rect.right - btn.rect.left) / 2.0f;
        float cy = btn.rect.top + (btn.rect.bottom - btn.rect.top) / 2.0f;

        if (btn.id == BTN_CLOSE) {
            float rot = btn.hoverAlpha * 90.0f;
            s_pRenderTarget->SetTransform(
                D2D1::Matrix3x2F::Rotation(rot, D2D1::Point2F(cx, cy)) * D2D1::Matrix3x2F::Scale(btn.currentScale, btn.currentScale, D2D1::Point2F(cx, cy)) * windowMat
            );
        }
        else {
            s_pRenderTarget->SetTransform(
                D2D1::Matrix3x2F::Scale(btn.currentScale, btn.currentScale, D2D1::Point2F(cx, cy)) * windowMat
            );
        }

        D2D1_ROUNDED_RECT btnRect = D2D1::RoundedRect(btn.rect, 6.0f, 6.0f);
        bool btnEnabled = btn.enabled && (!s_isEditing || btn.id == BTN_ADD_EDIT || btn.id == BTN_CLOSE || isSubBtn);
        float currentBtnAlpha = btnEnabled ? 1.0f : 0.25f;
        float effHover = btnEnabled ? btn.hoverAlpha : 0.0f;

        if (effHover > 0.01f) {
            DrawSimpleBoxGlow(btn.rect, 6.0f, effHover, s_currentAccentColor);

            D2D1_COLOR_F glowColor = D2D1::ColorF(s_currentAccentColor.r, s_currentAccentColor.g, s_currentAccentColor.b, 0.14f * effHover);
            if (!pBtnGlowBrush) s_pRenderTarget->CreateSolidColorBrush(glowColor, &pBtnGlowBrush);
            else pBtnGlowBrush->SetColor(glowColor);
            s_pRenderTarget->FillRoundedRectangle(&btnRect, pBtnGlowBrush.Get());
        }

        D2D1_COLOR_F borderColor = D2D1::ColorF(
            s_textColor.r + (s_currentAccentColor.r - s_textColor.r) * effHover,
            s_textColor.g + (s_currentAccentColor.g - s_textColor.g) * effHover,
            s_textColor.b + (s_currentAccentColor.b - s_textColor.b) * effHover,
            (0.15f + 0.75f * effHover) * currentBtnAlpha
        );
        if (!pBtnBorderBrush) s_pRenderTarget->CreateSolidColorBrush(borderColor, &pBtnBorderBrush);
        else pBtnBorderBrush->SetColor(borderColor);
        s_pRenderTarget->DrawRoundedRectangle(&btnRect, pBtnBorderBrush.Get(), 1.0f);

        if (btn.id == BTN_CLOSE) {
            D2D1_COLOR_F closeCol = D2D1::ColorF(
                s_textColor.r + (1.0f - s_textColor.r) * effHover,
                s_textColor.g + (1.0f - s_textColor.g) * effHover,
                s_textColor.b + (1.0f - s_textColor.b) * effHover,
                currentBtnAlpha
            );
            if (!pBtnTextBrush) s_pRenderTarget->CreateSolidColorBrush(closeCol, &pBtnTextBrush);
            else pBtnTextBrush->SetColor(closeCol);

            float p = 12.0f;
            s_pRenderTarget->DrawLine(D2D1::Point2F(btn.rect.left + p, btn.rect.top + p), D2D1::Point2F(btn.rect.right - p, btn.rect.bottom - p), pBtnTextBrush.Get(), 1.8f);
            s_pRenderTarget->DrawLine(D2D1::Point2F(btn.rect.right - p, btn.rect.top + p), D2D1::Point2F(btn.rect.left + p, btn.rect.bottom - p), pBtnTextBrush.Get(), 1.8f);
        }
        else {
            auto fontToUse = isSubBtn ? s_pFontTab.Get() : s_pFontBtn.Get();
            DrawModernLyricsStyleText(btn.text, fontToUse, btn.rect, effHover, currentBtnAlpha);
        }
    }

    s_pRenderTarget->SetTransform(windowMat);

    if (s_statusAlpha > 0.01f) {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pStatusBrush;
        s_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(s_textColor.r, s_textColor.g, s_textColor.b, s_statusAlpha * 0.6f), &pStatusBrush);
        D2D1_RECT_F statRect = D2D1::RectF(w - 250.0f, h - 30.0f, w - 20.0f, h - 10.0f);
        s_pRenderTarget->DrawTextW(s_statusMessage.c_str(), s_statusMessage.length(), s_pFontStatus.Get(), &statRect, pStatusBrush.Get());
    }

    HRESULT hr = s_pRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET) {
        s_pRenderTarget.Reset();
        s_mgrGlowCaches.clear();
        pBtnGlowBrush.Reset();
        pBtnBorderBrush.Reset();
        pBtnTextBrush.Reset();

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
        s_pD2DFactory->CreateDCRenderTarget(&props, s_pRenderTarget.GetAddressOf());
    }

    BYTE globalAlpha = static_cast<BYTE>(std::clamp(s_windowAlpha * 255.0f, 0.0f, 255.0f));

    POINT ptSrc = { 0, 0 };
    POINT ptWinPos = { s_windowX, s_windowY + (int)s_windowOffsetY };
    SIZE szWin = { w, h };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, globalAlpha, AC_SRC_ALPHA };

    UpdateLayeredWindow(hwnd, hdcScreen, &ptWinPos, &szWin, s_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(nullptr, hdcScreen);
}

LRESULT CALLBACK LyricsManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER: {
        bool needRender = false;

        if (s_themeDirty) {
            needRender = true;
            s_themeDirty = false;
        }

        if (std::abs(s_targetAccentColor.r - s_currentAccentColor.r) > 0.001f ||
            std::abs(s_targetAccentColor.g - s_currentAccentColor.g) > 0.001f ||
            std::abs(s_targetAccentColor.b - s_currentAccentColor.b) > 0.001f) {
            s_currentAccentColor.r += (s_targetAccentColor.r - s_currentAccentColor.r) * 0.06f;
            s_currentAccentColor.g += (s_targetAccentColor.g - s_currentAccentColor.g) * 0.06f;
            s_currentAccentColor.b += (s_targetAccentColor.b - s_currentAccentColor.b) * 0.06f;
            needRender = true;
        }
        else {
            s_currentAccentColor = s_targetAccentColor;
        }

        float targetEditAlpha = s_isEditing ? 1.0f : 0.0f;
        if (std::abs(targetEditAlpha - s_editModeAlpha) > 0.001f) {
            s_editModeAlpha += (targetEditAlpha - s_editModeAlpha) * 0.08f;
            needRender = true;
        }

        if (s_isClosing) {
            float prevAlpha = s_windowAlpha;
            s_windowAlpha += (0.0f - s_windowAlpha) * 0.10f;
            s_windowScale += (0.95f - s_windowScale) * 0.10f;
            s_windowOffsetY += (50.0f - s_windowOffsetY) * 0.10f;
            if (std::abs(s_windowAlpha - prevAlpha) > 0.001f) needRender = true;

            if (s_windowAlpha < 0.02f) {
                KillTimer(hwnd, 1);
                DestroyWindow(hwnd);
                return 0;
            }
        }
        else {
            float prevAlpha = s_windowAlpha;
            s_windowAlpha += (1.0f - s_windowAlpha) * 0.08f;
            s_windowScale += (1.0f - s_windowScale) * 0.08f;
            s_windowOffsetY += (0.0f - s_windowOffsetY) * 0.07f;
            if (std::abs(s_windowAlpha - prevAlpha) > 0.001f) needRender = true;
        }

        if (s_isDragging || std::abs(s_targetX - s_currentX) > 0.5f || std::abs(s_targetY - s_currentY) > 0.5f) {
            s_currentX += (s_targetX - s_currentX) * 0.08f;
            s_currentY += (s_targetY - s_currentY) * 0.08f;
            s_windowX = (int)std::round(s_currentX);
            s_windowY = (int)std::round(s_currentY);
            needRender = true;
        }

        if (std::abs(s_targetScrollOffset - s_scrollOffset) > 0.001f) {
            s_scrollOffset += (s_targetScrollOffset - s_scrollOffset) * 0.08f;
            needRender = true;
        }

        if (std::abs(s_targetEditScrollOffset - s_editScrollOffset) > 0.001f) {
            s_editScrollOffset += (s_targetEditScrollOffset - s_editScrollOffset) * 0.09f;
            needRender = true;
        }

        float prevListAlpha = s_listVisualAlpha;
        float prevListX = s_listXOffset;

        s_listVisualAlpha += (s_listTargetAlpha - s_listVisualAlpha) * 0.08f;
        s_listXOffset += (s_targetXOffset - s_listXOffset) * 0.08f;

        if (std::abs(s_listVisualAlpha - prevListAlpha) > 0.001f || std::abs(s_listXOffset - prevListX) > 0.001f) {
            needRender = true;
        }

        float targetPlaceholderAlpha = (s_searchQuery.empty() && !s_isSearchFocused) ? 0.40f : 0.0f;
        float targetPlaceholderY = (s_searchQuery.empty() && !s_isSearchFocused) ? 0.0f : -15.0f;

        float prevPlaceholderAlpha = s_placeholderVisualAlpha;
        float prevPlaceholderY = s_placeholderYOffset;

        s_placeholderVisualAlpha += (targetPlaceholderAlpha - s_placeholderVisualAlpha) * 0.09f;
        s_placeholderYOffset += (targetPlaceholderY - s_placeholderYOffset) * 0.09f;

        if (std::abs(s_placeholderVisualAlpha - prevPlaceholderAlpha) > 0.001f || std::abs(s_placeholderYOffset - prevPlaceholderY) > 0.001f) {
            needRender = true;
        }

        float targetBgAlpha = s_isSearchFocused ? 0.15f : 0.10f;
        float prevBgAlpha = s_searchBgVisualAlpha;
        s_searchBgVisualAlpha += (targetBgAlpha - s_searchBgVisualAlpha) * 0.08f;
        if (std::abs(s_searchBgVisualAlpha - prevBgAlpha) > 0.001f) needRender = true;

        s_marqueeTime += 0.015f;
        bool isScrolling = std::abs(s_targetScrollOffset - s_scrollOffset) > 0.05f;
        if (s_isSearchFocused || s_isEditing || (!isScrolling && s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size() && s_filteredTracks[s_selectedIndex].trackName.length() > 25)) {
            needRender = true;
        }

        for (auto& btn : s_buttons) {
            float targetAlpha = btn.isHovered ? 1.0f : 0.0f;
            float targetScale = btn.isHovered ? 1.05f : 1.0f;
            float prevHA = btn.hoverAlpha;
            float prevCS = btn.currentScale;
            btn.hoverAlpha += (targetAlpha - btn.hoverAlpha) * 0.08f;
            btn.currentScale += (targetScale - btn.currentScale) * 0.09f;
            if (std::abs(btn.hoverAlpha - prevHA) > 0.001f || std::abs(btn.currentScale - prevCS) > 0.001f) {
                needRender = true;
            }
        }

        if (s_statusAlpha > 0.0f) {
            s_statusAlpha -= 0.004f;
            needRender = true;
        }

        if (needRender) {
            Render(hwnd);
        }
        break;
    }
    case WM_CHAR: {
        if (s_isEditing && s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
            if (wParam == VK_BACK) {
                if (!s_editLines[s_selectedEditLine].text.empty()) s_editLines[s_selectedEditLine].text.pop_back();
            }
            else if (wParam >= 32) {
                s_editLines[s_selectedEditLine].text += (wchar_t)wParam;
            }
            SendMessage(hwnd, WM_TIMER, 1, 0);
            return 0;
        }

        if (s_isSearchFocused && !s_isEditing) {
            if (wParam == VK_BACK) {
                if (!s_searchQuery.empty()) s_searchQuery.pop_back();
            }
            else if (wParam >= 32) {
                s_searchQuery += (wchar_t)wParam;
            }
            UpdateFilteredTracks();
            SendMessage(hwnd, WM_TIMER, 1, 0);
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (s_isEditing) {
            if (zDelta > 0) s_targetEditScrollOffset = std::max(0.0f, s_targetEditScrollOffset - 1.5f);
            else s_targetEditScrollOffset = std::min(std::max(0.0f, (float)s_editLines.size() - 6.0f), s_targetEditScrollOffset + 1.5f);
            SendMessage(hwnd, WM_TIMER, 1, 0);
            break;
        }

        if (s_filteredTracks.empty()) break;
        if (zDelta > 0) s_selectedIndex = std::max(0, s_selectedIndex - 1);
        else s_selectedIndex = std::min((int)s_filteredTracks.size() - 1, s_selectedIndex + 1);
        s_targetScrollOffset = (float)s_selectedIndex;
        s_marqueeTime = 0.0f;
        SendMessage(hwnd, WM_TIMER, 1, 0);
        break;
    }
    case WM_LBUTTONDOWN: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (!s_isEditing) {
            auto checkTab = [&](D2D1_RECT_F r, TrackSource src) {
                if (pt.x >= r.left && pt.x <= r.right && pt.y >= r.top && pt.y <= r.bottom) {
                    s_currentTab = src;
                    UpdateFilteredTracks();
                    return true;
                }
                return false;
                };
            if (checkTab(s_tabAllRect, TrackSource::All) ||
                checkTab(s_tabApiRect, TrackSource::SpotifyApi) ||
                checkTab(s_tabNoApiRect, TrackSource::NoApi)) {
                SendMessage(hwnd, WM_TIMER, 1, 0);
                return 0;
            }

            s_isSearchFocused = (pt.x >= s_searchBoxRect.left && pt.x <= s_searchBoxRect.right && pt.y >= s_searchBoxRect.top && pt.y <= s_searchBoxRect.bottom);
        }

        for (const auto& btn : s_buttons) {
            if (btn.isHovered) {
                if (s_isEditing && btn.id != BTN_ADD_EDIT && btn.id != BTN_CLOSE && btn.id < BTN_TIME_MINUS) continue;

                if (btn.id == BTN_CLOSE) {
                    ToggleWindow();
                }
                else if (btn.id == BTN_ADD_EDIT) {
                    if (!s_isEditing) {
                        if (s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
                            s_isEditing = true;
                            s_buttons[BTN_ADD_EDIT].text = L"Save";
                            LoadLrcForEditing(s_filteredTracks[s_selectedIndex].filePath);
                            SetStatus(L"Editing lyrics mode.");
                        }
                    }
                    else {
                        if (s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
                            SaveLrcFromEditing(s_filteredTracks[s_selectedIndex]);
                            s_isEditing = false;
                            s_buttons[BTN_ADD_EDIT].text = L"Edit";
                            SetStatus(L"Lyrics saved successfully.");
                            LoadTrackList(); UpdateFilteredTracks();
                        }
                    }
                }
                else if (btn.id == BTN_REFETCH) {
                    if (s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
                        ManagedTrack target = s_filteredTracks[s_selectedIndex];
                        std::wstring track, artist;
                        if (!target.isUnknown) {
                            size_t pos = target.trackName.find(L" - ");
                            if (pos != std::wstring::npos) {
                                artist = target.trackName.substr(0, pos);
                                track = target.trackName.substr(pos + 3);
                            }
                            else track = target.trackName;
                            if (!target.filePath.empty() && std::filesystem::exists(target.filePath)) std::filesystem::remove(target.filePath);
                        }
                        else {
                            size_t tPos = target.trackName.find(L"Track: ");
                            size_t aPos = target.trackName.find(L" | Artist: ");
                            if (tPos != std::wstring::npos && aPos != std::wstring::npos) {
                                track = target.trackName.substr(tPos + 7, aPos - (tPos + 7));
                                artist = target.trackName.substr(aPos + 11);
                            }
                        }
                        if (!track.empty()) {
                            s_refetchTrackName = track; s_refetchArtistName = artist; s_refetchIsUnknown = target.isUnknown; s_hasRefetchRequest = true;
                            SetStatus(L"Refetching: " + track);
                        }
                        LoadTrackList(); UpdateFilteredTracks();
                    }
                }
                else if (btn.id == BTN_DELETE) {
                    if (s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
                        ManagedTrack target = s_filteredTracks[s_selectedIndex];

                        if (!target.isUnknown && !target.filePath.empty() && std::filesystem::exists(target.filePath)) {
                            std::filesystem::remove(target.filePath);
                        }

                        else if (target.isUnknown) {
                            std::wstring unknownFilePath = s_lyricsDir + L"\\unknowns.txt";
                            std::vector<std::string> lines;
                            std::ifstream ifs(unknownFilePath);
                            if (ifs.is_open()) {
                                std::string line;
                                int size = WideCharToMultiByte(CP_UTF8, 0, target.trackName.c_str(), -1, NULL, 0, NULL, NULL);
                                std::string targetLineUtf8(size - 1, 0);
                                WideCharToMultiByte(CP_UTF8, 0, target.trackName.c_str(), -1, &targetLineUtf8[0], size, NULL, NULL);

                                while (std::getline(ifs, line)) {

                                    if (line.find(targetLineUtf8) == std::string::npos && !line.empty()) {
                                        lines.push_back(line);
                                    }
                                }
                                ifs.close();
                            }

                            std::ofstream uofs(unknownFilePath, std::ios::trunc);
                            if (uofs.is_open()) {
                                for (const auto& l : lines) uofs << l << "\n";
                                uofs.close();
                            }
                        }
                        SetStatus(L"Deleted selected track.");
                        LoadTrackList();
                        UpdateFilteredTracks();
                    }
                }
                else if (btn.id == BTN_DELETE_ALL) {
                    for (const auto& entry : std::filesystem::directory_iterator(s_lyricsDir)) {
                        if (entry.path().extension() == L".lrc") std::filesystem::remove(entry.path());
                    }

                    std::wstring unknownFilePath = s_lyricsDir + L"\\unknowns.txt";
                    if (std::filesystem::exists(unknownFilePath)) {
                        std::filesystem::remove(unknownFilePath);
                    }

                    SetStatus(L"Cleared all data.");
                    LoadTrackList();
                    UpdateFilteredTracks();
                }

                else if (btn.id == BTN_TIME_MINUS) {
                    if (s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
                        s_editLines[s_selectedEditLine].timeMs = std::max(0, s_editLines[s_selectedEditLine].timeMs - 100);
                    }
                }
                else if (btn.id == BTN_TIME_PLUS) {
                    if (s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
                        s_editLines[s_selectedEditLine].timeMs += 100;
                    }
                }
                else if (btn.id == BTN_ADD_LINE) {
                    int newTime = s_editLines.empty() ? 0 : s_editLines.back().timeMs + 3000;
                    s_editLines.push_back({ newTime, L"New Line" });
                    s_selectedEditLine = (int)s_editLines.size() - 1;
                }
                else if (btn.id == BTN_REMOVE_LINE) {
                    if (!s_editLines.empty() && s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
                        s_editLines.erase(s_editLines.begin() + s_selectedEditLine);
                        if (s_selectedEditLine >= (int)s_editLines.size()) s_selectedEditLine = std::max(0, (int)s_editLines.size() - 1);
                    }
                }
                else if (btn.id == BTN_TOGGLE_API) {
                    if (s_selectedIndex >= 0 && s_selectedIndex < (int)s_filteredTracks.size()) {
                        auto& track = s_filteredTracks[s_selectedIndex];
                        track.source = (track.source == TrackSource::NoApi) ? TrackSource::SpotifyApi : TrackSource::NoApi;

                        if (!track.filePath.empty()) {
                            if (std::filesystem::exists(track.filePath)) {
                                std::filesystem::remove(track.filePath);
                            }
                            track.filePath = L"";
                        }
                    }
                }

                SendMessage(hwnd, WM_TIMER, 1, 0);
                return 0;
            }
        }

        if (s_isEditing) {
            if (pt.x >= 25 && pt.x <= 425 && pt.y >= 55 && pt.y <= 335) {
                float startY = 60.0f - (s_editScrollOffset * 36.0f);
                int index = (pt.y - startY) / 36;
                if (index >= 0 && index < (int)s_editLines.size()) {
                    s_selectedEditLine = index;
                }
            }
            else if (pt.x >= s_sliderTrackRect.left && pt.x <= s_sliderTrackRect.right && pt.y >= s_sliderTrackRect.top - 10 && pt.y <= s_sliderTrackRect.bottom + 10) {
                s_isSliderDragging = true;
                SetCapture(hwnd);
            }
            else if (pt.x >= s_durationSliderRect.left && pt.x <= s_durationSliderRect.right && pt.y >= s_durationSliderRect.top - 10 && pt.y <= s_durationSliderRect.bottom + 10) {
                s_isDurationSliderDragging = true;
                SetCapture(hwnd);
            }
        }

        if (!s_isSearchFocused && !s_isEditing) {
            s_isDragging = true;
            GetCursorPos(&s_dragStartPos);
            SetCapture(hwnd);
        }
        SendMessage(hwnd, WM_TIMER, 1, 0);
        break;
    }
    case WM_MOUSEMOVE: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        for (auto& btn : s_buttons) {
            bool isSubBtn = (btn.id >= BTN_TIME_MINUS);
            if (isSubBtn && (!s_isEditing || s_selectedEditLine < 0 || s_selectedEditLine >= (int)s_editLines.size())) {
                btn.isHovered = false;
                continue;
            }
            btn.isHovered = (pt.x >= btn.rect.left && pt.x <= btn.rect.right && pt.y >= btn.rect.top && btn.rect.bottom >= pt.y);
        }

        if (s_isSliderDragging && s_selectedEditLine >= 0 && s_selectedEditLine < (int)s_editLines.size()) {
            float ratio = std::clamp((float)(pt.x - s_sliderTrackRect.left) / (s_sliderTrackRect.right - s_sliderTrackRect.left), 0.0f, 1.0f);
            float maxMs = (s_totalDurationMs > 0) ? (float)s_totalDurationMs : 180000.0f;
            int newMs = static_cast<int>(ratio * maxMs);
            newMs = (newMs / 100) * 100;
            s_editLines[s_selectedEditLine].timeMs = newMs;
        }

        if (s_isDurationSliderDragging) {
            float ratio = std::clamp((float)(pt.x - s_durationSliderRect.left) / (s_durationSliderRect.right - s_durationSliderRect.left), 0.0f, 1.0f);
            int newDurMs = static_cast<int>(ratio * 600000.0f);
            s_totalDurationMs = std::max(10000, (newDurMs / 1000) * 1000);
        }

        if (s_isDragging) {
            POINT screenPt;
            GetCursorPos(&screenPt);
            s_targetX += (screenPt.x - s_dragStartPos.x);
            s_targetY += (screenPt.y - s_dragStartPos.y);
            s_dragStartPos = screenPt;
        }
        SendMessage(hwnd, WM_TIMER, 1, 0);
        break;
    }
    case WM_LBUTTONUP: {
        s_isDragging = false;
        s_isSliderDragging = false;
        s_isDurationSliderDragging = false;
        ReleaseCapture();

        g_lyrics_pos_x = s_windowX;
        g_lyrics_pos_y = s_windowY;
        SaveConfig();

        SendMessage(hwnd, WM_TIMER, 1, 0);
        break;
    }
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        if (s_hdcMem) {
            SelectObject(s_hdcMem, s_hOldBitmap); DeleteObject(s_hBitmap); DeleteDC(s_hdcMem);
            s_hdcMem = nullptr; s_hBitmap = nullptr; s_hOldBitmap = nullptr;
        }
        s_hwnd = nullptr; timeEndPeriod(1);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
