#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <string>
#include <vector>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

class MgrRollingDigitAnimation {
public:
    float current = 0.0f;
    float target = 0.0f;
    float speed = 0.06f;
    void setTarget(float t) { target = t; }
    void setImmediate(float t) { current = target = t; }
    void update() { current += (std::abs(target - current) > 10.0f) ? (target - current) : (target - current) * speed; }
};

class MgrRollingTimeRenderer {
public:
    MgrRollingDigitAnimation m1, m2, s1, s2;
    int last_total_seconds = -1;
    void update(int total_seconds) {
        int cur_m1 = total_seconds / 600;
        int cur_m2 = (total_seconds / 60) % 10;
        int cur_s1 = (total_seconds % 60) / 10;
        int cur_s2 = total_seconds % 10;
        if (last_total_seconds == -1) {
            m1.setImmediate((float)cur_m1); m2.setImmediate((float)cur_m2);
            s1.setImmediate((float)cur_s1); s2.setImmediate((float)cur_s2);
        }
        else {
            m1.setTarget((float)cur_m1); m2.setTarget((float)cur_m2);
            s1.setTarget((float)cur_s1); s2.setTarget((float)cur_s2);
        }
        last_total_seconds = total_seconds;
        m1.update(); m2.update(); s1.update(); s2.update();
    }
};

enum class TrackSource {
    All,
    SpotifyApi,
    NoApi
};

struct ManagedTrack {
    std::wstring trackName;
    std::wstring artistName;
    bool isUnknown;
    TrackSource source;
    std::wstring filePath;
};

struct EditableLrcLine {
    int timeMs;
    std::wstring text;
};

struct CustomButton {
    D2D1_RECT_F rect;
    std::wstring text;
    bool isHovered = false;
    float hoverAlpha = 0.0f;
    float currentScale = 1.0f;
    int id;
    bool enabled = true;
};

class LyricsManager {
public:
    static void Initialize(HINSTANCE hInstance, HWND hParent, const std::wstring& lyricsDir);
    static void ToggleWindow();
    static bool IsOpen();
    static void UpdateTheme(D2D1_COLOR_F bg, D2D1_COLOR_F text, D2D1_COLOR_F accent);

    static bool PopRefetchRequest(std::wstring& outTrackName, std::wstring& outArtistName, bool& outIsUnknown);
    static void Refresh();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void LoadTrackList();
    static void UpdateFilteredTracks();
    static void Render(HWND hwnd);
    static void SetStatus(const std::wstring& msg);

    static void LoadLrcForEditing(const std::wstring& filePath);
    static void SaveLrcFromEditing(const ManagedTrack& track);
    static void DrawRollingTime(MgrRollingTimeRenderer& timer, ID2D1Brush* pBrush, float startX, float startY, float digitW, float digitH);

    static HWND s_hwnd;
    static HWND s_hParent;
    static HINSTANCE s_hInstance;
    static std::wstring s_lyricsDir;

    static std::vector<ManagedTrack> s_tracks;
    static std::vector<ManagedTrack> s_filteredTracks;

    static int s_selectedIndex;
    static float s_scrollOffset;
    static float s_targetScrollOffset;

    static float s_marqueeTime;

    static TrackSource s_currentTab;
    static D2D1_RECT_F s_tabAllRect;
    static D2D1_RECT_F s_tabApiRect;
    static D2D1_RECT_F s_tabNoApiRect;

    static std::wstring s_searchQuery;
    static bool s_isSearchFocused;
    static D2D1_RECT_F s_searchBoxRect;

    static std::vector<CustomButton> s_buttons;
    static bool s_isDragging;
    static POINT s_dragStartPos;

    static int s_windowX;
    static int s_windowY;

    static float s_windowAlpha;
    static float s_windowScale;
    static float s_windowOffsetY;
    static bool s_isClosing;

    static std::wstring s_statusMessage;
    static float s_statusAlpha;

    static D2D1_COLOR_F s_bgColor;
    static D2D1_COLOR_F s_textColor;
    static D2D1_COLOR_F s_accentColor;
    static D2D1_COLOR_F s_targetAccentColor;
    static D2D1_COLOR_F s_currentAccentColor;

    static bool s_isEditing;
    static float s_editModeAlpha;
    static float s_editScrollOffset;
    static float s_targetEditScrollOffset;
    static std::vector<EditableLrcLine> s_editLines;
    static int s_selectedEditLine;
    static int s_totalDurationMs;
    static bool s_isSliderDragging;
    static bool s_isDurationSliderDragging;
    static D2D1_RECT_F s_sliderTrackRect;
    static D2D1_RECT_F s_durationSliderRect;
    static D2D1_RECT_F s_addLineEditRect;
    static D2D1_RECT_F s_removeLineEditRect;
    static D2D1_RECT_F s_btnTimeMinusRect;
    static D2D1_RECT_F s_btnTimePlusRect;

    static MgrRollingTimeRenderer s_lineTimeRenderer;
    static MgrRollingTimeRenderer s_totalTimeRenderer;

    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontSettingsTab;
    static Microsoft::WRL::ComPtr<ID2D1Factory> s_pD2DFactory;
    static Microsoft::WRL::ComPtr<IDWriteFactory> s_pDWriteFactory;
    static Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> s_pRenderTarget;

    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontList;
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontListBold;
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontBtn;
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontStatus;
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontTab;
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> s_pFontDigit;

    static bool s_hasRefetchRequest;
    static std::wstring s_refetchTrackName;
    static std::wstring s_refetchArtistName;
    static bool s_refetchIsUnknown;

    static HDC s_hdcMem;
    static HBITMAP s_hBitmap;
    static HBITMAP s_hOldBitmap;
};
