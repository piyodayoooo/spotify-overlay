#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <cmath>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace UIConfig {
    const int WINDOW_WIDTH = 400;
    const int WINDOW_HEIGHT = 530;
    const wchar_t* const WINDOW_TITLE = L"Spotify Overlay";
    const wchar_t* const FONT_FILE = L"font.ttc";
}

class Animation {
public:
    float current = 0.0f;
    float target = 0.0f;
    float speed = 0.04f;

    void set(float t) { target = t; }
    void setImmediate(float t) { current = target = t; }

    void update(float dt = 0.0166f) {
        float factor = 1.0f - std::exp(-speed * 60.0f * dt);
        current += (target - current) * factor;
        if (std::abs(target - current) < 0.001f) current = target;
    }
};

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) = 0;
    virtual void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) = 0;
};

class CharPopAnim {
public:
    wchar_t ch;
    Animation scale;
    bool isRemoving = false;
    CharPopAnim(wchar_t c);
    void update(float dt = 0.0166f);
};

class UIInputField : public Widget {
private:
    float x, y, w, h;
    std::wstring label;
    bool isFocused = false;
    bool isAllSelected = false;
    Animation hoverAnim;
    Animation focusAnim;
    Animation cursorAlphaAnim;
    Animation cursorXAnim;
    Animation selectionWidthAnim;
    float cursorTime = 0.0f;
    std::wstring& textRef;
    std::vector<CharPopAnim> charAnims;

public:
    UIInputField(std::wstring lb, float px, float py, float pw, float ph, std::wstring& tx);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
    void clearSelection();
    void handleCharInput(wchar_t c);
    void selectAll();
    void pasteText(const std::wstring& pastedText);
    bool focused() const;
    void setFocused(bool f);
};

class UIScrollingNumber : public Widget {
private:
    float x, y;
    int& targetValue;
    std::wstring label;
    std::vector<Animation> digitAnims;
public:
    UIScrollingNumber(std::wstring lb, float px, float py, int& val);
    void updateDigits(float dt = 0.0166f);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
};

class UISlider : public Widget {
private:
    std::wstring label;
    int& targetValue;
    int minVal, maxVal;
    float x, y, w, h;
    Animation handleAnim;
    Animation hoverAnim;
    bool isSliding = false;
public:
    UISlider(std::wstring text, float x, float y, float w, float h, int minV, int maxV, int& valueRef);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
};

class UISliderCard : public Widget {
private:
    float x, y, w, h;
    std::shared_ptr<UIScrollingNumber> scrollNum;
    std::shared_ptr<UISlider> innerSlider;
public:
    UISliderCard(std::wstring displayLabel, float px, float py, float pw, float ph, int minV, int maxV, int& valueRef);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
};

class UIButton : public Widget {
private:
    float x, y, w, h;
    std::wstring label;
    Animation hoverAnim;
    bool wasClicked = false;
public:
    UIButton(std::wstring text, float px, float py, float pw, float ph);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
    bool clicked() const;
};

class UIToggle : public Widget {
private:
    std::wstring label;
    bool& targetValue;
    float x, y, w, h;
    Animation hoverAnim;
    Animation toggleAnim;
    bool wasClickedLastFrame = false;
public:
    UIToggle(std::wstring text, float x, float y, float w, float h, bool& valueRef);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
};

class UIHeader {
private:
    float x, y, w, h;
    float timeCount = 0.0f;
public:
    UIHeader(float x, float y, float w, float h);
    void update(float dt = 0.0166f);
    void render(ID2D1RenderTarget* target);
};

struct TrackInfo {
    std::string track_name;
    std::string artist_name;
    std::string image_url;
    std::string timestamp;
};

class OverlayState {
public:
    std::mutex mtx;
    TrackInfo current_track;
    std::vector<TrackInfo> track_history;
    bool track_changed = false;
    int progress_sec = 0;
    int duration_sec = 0;
};

extern OverlayState g_overlay_state;

class UIGlowText {
private:
    const int GLOW_ITERATIONS = 5;
public:
    void renderGlow(ID2D1RenderTarget* target, const std::wstring& text, IDWriteTextFormat* font,
        D2D1_POINT_2F origin, D2D1_COLOR_F glowColor, float intensity);
};

class UIImagePreview : public Widget {
private:
    float x, y, w, h;
    ID2D1Bitmap* currentBmp = nullptr;
    ID2D1Bitmap* nextBmp = nullptr;
    Animation fadeAnim;
    Animation gaugeAnim;
    D2D1_COLOR_F bgColor;
    D2D1_COLOR_F accentColor;
    void calculateDynamicColors(IWICImagingFactory* wicFactory, IWICBitmapSource* bmpSource);

public:
    UIImagePreview(float px, float py, float pw, float ph);
    virtual ~UIImagePreview() override;
    void setImage(ID2D1RenderTarget* target, IWICImagingFactory* wicFactory, const std::wstring& imagePath);
    void setGaugeValue(float value);
    void update(float mx, float my, bool clicked, bool isDragging, float dt = 0.0166f) override;
    void render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
        IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha) override;
};
