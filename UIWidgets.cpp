#include "UIWidgets.h"
#include <cmath>
#include <algorithm>

CharPopAnim::CharPopAnim(wchar_t c) : ch(c) {
    scale.setImmediate(0.0f);
    scale.set(1.0f);
    scale.speed = 0.25f;
}

void CharPopAnim::update(float dt) {
    scale.update(dt);
}

UIInputField::UIInputField(std::wstring lb, float px, float py, float pw, float ph, std::wstring& tx)
    : label(lb), x(px), y(py), w(pw), h(ph), textRef(tx)
{
    hoverAnim.setImmediate(0.0f);
    focusAnim.setImmediate(0.0f);
    cursorAlphaAnim.setImmediate(0.0f);
    cursorXAnim.setImmediate(0.0f);
    selectionWidthAnim.setImmediate(0.0f);
}

void UIInputField::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    bool isHover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
    hoverAnim.set(isHover ? 1.0f : 0.0f);
    hoverAnim.update(dt);

    if (clicked) {
        isFocused = isHover;
        if (!isFocused) isAllSelected = false;
    }
    focusAnim.set(isFocused ? 1.0f : 0.0f);
    focusAnim.update(dt);

    cursorTime += dt;
    cursorAlphaAnim.set((std::sin(cursorTime * 10.0f) > 0.0f && isFocused) ? 1.0f : 0.0f);
    cursorAlphaAnim.update(dt);
}

void UIInputField::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* bgBrush = nullptr;
    ID2D1SolidColorBrush* borderBrush = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;

    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha * (25.0f / 255.0f)), &bgBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(100.0f / 255.0f, 150.0f / 255.0f, 1.0f, layerAlpha * ((100.0f + focusAnim.current * 155.0f) / 255.0f)), &borderBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(240.0f / 255.0f, 240.0f / 255.0f, 240.0f / 255.0f, layerAlpha), &textBrush);

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + w, y + h);

    if (bgBrush) target->FillRectangle(rect, bgBrush);
    if (borderBrush) target->DrawRectangle(rect, borderBrush, 1.5f);

    if (textBrush) {

        D2D1_RECT_F labelRect = D2D1::RectF(x + 10.0f, y - 20.0f, x + w, y);
        if (fontBold) target->DrawTextW(label.c_str(), (UINT32)label.length(), fontBold, labelRect, textBrush);

        D2D1_RECT_F textRect = D2D1::RectF(x + 10.0f, y + (h - 20.0f) / 2.0f, x + w, y + h);
        if (fontReg) target->DrawTextW(textRef.c_str(), (UINT32)textRef.length(), fontReg, textRect, textBrush);
    }

    if (bgBrush) bgBrush->Release();
    if (borderBrush) borderBrush->Release();
    if (textBrush) textBrush->Release();
}

UIScrollingNumber::UIScrollingNumber(std::wstring lb, float px, float py, int& val)
    : label(lb), x(px), y(py), targetValue(val) {
}

void UIScrollingNumber::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    updateDigits(dt);
}

void UIScrollingNumber::updateDigits(float dt) {
    std::wstring numStr = std::to_wstring(targetValue);
    if (digitAnims.size() != numStr.length()) {
        digitAnims.resize(numStr.length());
        for (size_t i = 0; i < digitAnims.size(); ++i) {
            digitAnims[i].setImmediate(0.0f);
        }
    }
    for (size_t i = 0; i < digitAnims.size(); ++i) {
        digitAnims[i].set(1.0f);
        digitAnims[i].update(dt);
    }
}

void UIScrollingNumber::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* labelBrush = nullptr;
    ID2D1SolidColorBrush* numBrush = nullptr;

    target->CreateSolidColorBrush(D2D1::ColorF(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, layerAlpha * (180.0f / 255.0f)), &labelBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &numBrush);

    if (labelBrush && fontReg) {
        D2D1_RECT_F labelRect = D2D1::RectF(x, y, x + 100.0f, y + 30.0f);
        target->DrawTextW(label.c_str(), (UINT32)label.length(), fontReg, labelRect, labelBrush);
    }

    if (numBrush && fontBold) {
        std::wstring numStr = std::to_wstring(targetValue);
        float curX = x + 100.0f;
        for (size_t i = 0; i < numStr.length(); ++i) {
            float offset = (1.0f - digitAnims[i].current) * 15.0f;
            D2D1_RECT_F digitRect = D2D1::RectF(curX, y - offset, curX + 20.0f, y - offset + 30.0f);
            target->DrawTextW(&numStr[i], 1, fontBold, digitRect, numBrush);
            curX += 14.0f;
        }
    }

    if (labelBrush) labelBrush->Release();
    if (numBrush) numBrush->Release();
}

UISlider::UISlider(std::wstring text, float px, float py, float pw, float ph, int minV, int maxV, int& valueRef)
    : label(text), x(px), y(py), w(pw), h(ph), minVal(minV), maxVal(maxV), targetValue(valueRef)
{
    handleAnim.setImmediate(0.0f);
    hoverAnim.setImmediate(0.0f);
}

void UISlider::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    bool isHover = (mx >= x && mx <= x + w && my >= y - 10 && my <= y + h + 10);
    hoverAnim.set(isHover ? 1.0f : 0.0f);
    hoverAnim.update(dt);

    if (clicked && isHover) isSliding = true;
    if (!isDragging) isSliding = false;

    if (isSliding) {
        float pct = (mx - x) / w;
        pct = std::max(0.0f, std::min(1.0f, pct));
        targetValue = minVal + static_cast<int>(pct * (maxVal - minVal));
    }

    float currentPct = static_cast<float>(targetValue - minVal) / (maxVal - minVal);
    handleAnim.set(currentPct);
    handleAnim.update(dt);
}

void UISlider::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* railBg = nullptr;
    ID2D1SolidColorBrush* railActive = nullptr;
    ID2D1SolidColorBrush* handleBrush = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;

    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha * (40.0f / 255.0f)), &railBg);
    target->CreateSolidColorBrush(D2D1::ColorF(80.0f / 255.0f, 160.0f / 255.0f, 240.0f / 255.0f, layerAlpha), &railActive);
    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &handleBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, layerAlpha), &textBrush);

    float railY = y + h / 2.0f - 3.0f;
    if (railBg) target->FillRectangle(D2D1::RectF(x, railY, x + w, railY + 6.0f), railBg);
    if (railActive) target->FillRectangle(D2D1::RectF(x, railY, x + w * handleAnim.current, railY + 6.0f), railActive);

    float handleX = x + w * handleAnim.current;
    float handleRadius = 8.0f + hoverAnim.current * 3.0f;
    if (handleBrush) {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(handleX, y + h / 2.0f), handleRadius, handleRadius);
        target->FillEllipse(ellipse, handleBrush);
    }

    if (textBrush && fontBold) {
        std::wstring valStr = std::to_wstring(targetValue);
        D2D1_RECT_F textRect = D2D1::RectF(x + w + 15.0f, y - 2.0f, x + w + 100.0f, y + 30.0f);
        target->DrawTextW(valStr.c_str(), (UINT32)valStr.length(), fontBold, textRect, textBrush);
    }

    if (railBg) railBg->Release();
    if (railActive) railActive->Release();
    if (handleBrush) handleBrush->Release();
    if (textBrush) textBrush->Release();
}

UISliderCard::UISliderCard(std::wstring displayLabel, float px, float py, float pw, float ph, int minV, int maxV, int& valueRef)
    : x(px), y(py), w(pw), h(ph)
{
    scrollNum = std::make_shared<UIScrollingNumber>(displayLabel, px + 15.0f, py + 15.0f, valueRef);
    innerSlider = std::make_shared<UISlider>(L"", px + 15.0f, py + 45.0f, pw - 80.0f, 20.0f, minV, maxV, valueRef);
}

void UISliderCard::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    scrollNum->update(mx, my, clicked, isDragging, dt);
    innerSlider->update(mx, my, clicked, isDragging, dt);
}

void UISliderCard::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* cardBg = nullptr;
    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha * (15.0f / 255.0f)), &cardBg);

    if (cardBg) {
        target->FillRectangle(D2D1::RectF(x, y, x + w, y + h), cardBg);
        cardBg->Release();
    }

    scrollNum->render(target, writeFactory, fontBold, fontReg, fontLarge, layerAlpha);
    innerSlider->render(target, writeFactory, fontBold, fontReg, fontLarge, layerAlpha);
}

UIButton::UIButton(std::wstring text, float px, float py, float pw, float ph)
    : label(text), x(px), y(py), w(pw), h(ph)
{
    hoverAnim.setImmediate(0.0f);
}

void UIButton::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    bool isHover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
    hoverAnim.set(isHover ? 1.0f : 0.0f);
    hoverAnim.update(dt);
    wasClicked = (isHover && clicked);
}

void UIButton::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* btnBrush = nullptr;
    ID2D1SolidColorBrush* borderPen = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;

    float alphaBtn = layerAlpha * ((40.0f + hoverAnim.current * 40.0f) / 255.0f);
    target->CreateSolidColorBrush(D2D1::ColorF(100.0f / 255.0f, 150.0f / 255.0f, 1.0f, alphaBtn), &btnBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(100.0f / 255.0f, 150.0f / 255.0f, 1.0f, layerAlpha * (150.0f / 255.0f)), &borderPen);
    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &textBrush);

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + w, y + h);

    if (btnBrush) target->FillRectangle(rect, btnBrush);
    if (borderPen) target->DrawRectangle(rect, borderPen, 1.0f);

    if (textBrush && fontBold) {

        target->DrawTextW(label.c_str(), (UINT32)label.length(), fontBold, rect, textBrush);
    }

    if (btnBrush) btnBrush->Release();
    if (borderPen) borderPen->Release();
    if (textBrush) textBrush->Release();
}

UIToggle::UIToggle(std::wstring text, float px, float py, float pw, float ph, bool& valueRef)
    : label(text), x(px), y(py), w(pw), h(ph), targetValue(valueRef)
{
    hoverAnim.setImmediate(0.0f);
    toggleAnim.setImmediate(targetValue ? 1.0f : 0.0f);
}

void UIToggle::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    bool isHover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
    hoverAnim.set(isHover ? 1.0f : 0.0f);
    hoverAnim.update(dt);

    if (clicked && isHover && !wasClickedLastFrame) {
        targetValue = !targetValue;
        wasClickedLastFrame = true;
    }
    if (!clicked) {
        wasClickedLastFrame = false;
    }

    toggleAnim.set(targetValue ? 1.0f : 0.0f);
    toggleAnim.update(dt);
}

void UIToggle::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    float swW = 44.0f;
    float swH = 22.0f;
    float swX = x + w - swW - 10.0f;
    float swY = y + (h - swH) / 2.0f;

    float r = (50.0f + toggleAnim.current * 30.0f) / 255.0f;
    float g_ch = (50.0f + toggleAnim.current * 110.0f) / 255.0f;
    float b = (50.0f + toggleAnim.current * 205.0f) / 255.0f;

    ID2D1SolidColorBrush* switchBg = nullptr;
    ID2D1SolidColorBrush* btnBrush = nullptr;
    ID2D1SolidColorBrush* textBrush = nullptr;

    target->CreateSolidColorBrush(D2D1::ColorF(r, g_ch, b, layerAlpha), &switchBg);
    target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &btnBrush);
    target->CreateSolidColorBrush(D2D1::ColorF(240.0f / 255.0f, 240.0f / 255.0f, 240.0f / 255.0f, layerAlpha), &textBrush);

    if (switchBg) {
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(swX, swY, swX + swW, swY + swH), swH / 2.0f, swH / 2.0f);
        target->FillRoundedRectangle(rr, switchBg);
    }

    if (btnBrush) {
        float btnRadius = 8.0f;
        float btnX = swX + 11.0f + toggleAnim.current * (swW - 22.0f);
        float btnY = swY + swH / 2.0f;
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(btnX, btnY), btnRadius, btnRadius);
        target->FillEllipse(ellipse, btnBrush);
    }

    if (textBrush && fontBold) {
        D2D1_RECT_F textRect = D2D1::RectF(x + 10.0f, y + (h - 18.0f) / 2.0f, swX, y + h);
        target->DrawTextW(label.c_str(), (UINT32)label.length(), fontBold, textRect, textBrush);
    }

    if (switchBg) switchBg->Release();
    if (btnBrush) btnBrush->Release();
    if (textBrush) textBrush->Release();
}

UIHeader::UIHeader(float px, float py, float pw, float ph) : x(px), y(py), w(pw), h(ph) {}

void UIHeader::update(float dt) {
    timeCount += dt * 2.0f;
}

void UIHeader::render(ID2D1RenderTarget* target) {
    float animW = w * 0.4f + std::sin(timeCount) * 50.0f;

    ID2D1SolidColorBrush* barBrush = nullptr;
    target->CreateSolidColorBrush(D2D1::ColorF(80.0f / 255.0f, 160.0f / 255.0f, 240.0f / 255.0f, 1.0f), &barBrush);

    if (barBrush) {
        target->FillRectangle(D2D1::RectF(x, y + h - 4.0f, x + animW, y + h), barBrush);
        barBrush->Release();
    }
}

UIImagePreview::UIImagePreview(float px, float py, float pw, float ph)
    : x(px), y(py), w(pw), h(ph)
{
    fadeAnim.speed = 0.18f;
    gaugeAnim.speed = 0.32f;
    fadeAnim.setImmediate(1.0f);
    gaugeAnim.setImmediate(0.0f);

    bgColor = D2D1::ColorF(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f);
    accentColor = D2D1::ColorF(230.0f / 255.0f, 230.0f / 255.0f, 230.0f / 255.0f, 1.0f);
}

UIImagePreview::~UIImagePreview() {
    if (currentBmp) currentBmp->Release();
    if (nextBmp) nextBmp->Release();
}

void UIImagePreview::calculateDynamicColors(IWICImagingFactory* wicFactory, IWICBitmapSource* bmpSource) {
    if (!wicFactory || !bmpSource) return;

    UINT w = 0, h = 0;
    bmpSource->GetSize(&w, &h);
    if (w == 0 || h == 0) return;

    IWICBitmapScaler* scaler = nullptr;
    if (SUCCEEDED(wicFactory->CreateBitmapScaler(&scaler))) {
        if (SUCCEEDED(scaler->Initialize(bmpSource, 16, 16, WICBitmapInterpolationModeFant))) {
            WICRect rc = { 0, 0, 16, 16 };
            std::vector<BYTE> buffer(16 * 16 * 4);

            if (SUCCEEDED(scaler->CopyPixels(&rc, 16 * 4, (UINT)buffer.size(), buffer.data()))) {
                long long rSum = 0, gSum = 0, bSum = 0, count = 0;
                for (size_t i = 0; i < buffer.size(); i += 4) {
                    BYTE b = buffer[i];
                    BYTE g_val = buffer[i + 1];
                    BYTE r = buffer[i + 2];
                    BYTE a = buffer[i + 3];
                    if (a > 50) {
                        rSum += r; gSum += g_val; bSum += b; count++;
                    }
                }

                if (count > 0) {
                    float rAvg = (rSum / count) / 255.0f;
                    float gAvg = (gSum / count) / 255.0f;
                    float bAvg = (bSum / count) / 255.0f;

                    bgColor = D2D1::ColorF(1.0f - rAvg, 1.0f - gAvg, 1.0f - bAvg, 1.0f);
                    accentColor = D2D1::ColorF(rAvg, gAvg, bAvg, 1.0f);
                }
            }
        }
        scaler->Release();
    }
}

void UIImagePreview::setImage(ID2D1RenderTarget* target, IWICImagingFactory* wicFactory, const std::wstring& imagePath) {
    if (!target || !wicFactory) return;

    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    ID2D1Bitmap* newD2DBmp = nullptr;

    HRESULT hr = wicFactory->CreateDecoderFromFilename(
        imagePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

    if (SUCCEEDED(hr)) hr = decoder->GetFrame(0, &frame);
    if (SUCCEEDED(hr)) hr = wicFactory->CreateFormatConverter(&converter);

    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(
            frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
            NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
    }

    if (SUCCEEDED(hr)) {
        hr = target->CreateBitmapFromWicBitmap(converter, NULL, &newD2DBmp);

        calculateDynamicColors(wicFactory, converter);
    }

    if (decoder) decoder->Release();
    if (frame) frame->Release();
    if (converter) converter->Release();

    if (!newD2DBmp) return;

    if (!currentBmp) {
        currentBmp = newD2DBmp;
        fadeAnim.setImmediate(1.0f);
    }
    else {
        if (nextBmp) nextBmp->Release();
        nextBmp = newD2DBmp;
        fadeAnim.set(0.0f);
    }
}

void UIImagePreview::setGaugeValue(float value) {
    gaugeAnim.set(std::max(0.0f, std::min(1.0f, value)));
}

void UIImagePreview::update(float mx, float my, bool clicked, bool isDragging, float dt) {
    fadeAnim.update(dt);
    gaugeAnim.update(dt);

    if (nextBmp && fadeAnim.current <= 0.01f) {
        if (currentBmp) currentBmp->Release();
        currentBmp = nextBmp;
        nextBmp = nullptr;
        fadeAnim.set(1.0f);
    }
}

void UIImagePreview::render(ID2D1RenderTarget* target, IDWriteFactory* writeFactory,
    IDWriteTextFormat* fontBold, IDWriteTextFormat* fontReg, IDWriteTextFormat* fontLarge, float layerAlpha)
{
    ID2D1SolidColorBrush* bgBrush = nullptr;
    D2D1_COLOR_F currentBg = bgColor;
    currentBg.a = layerAlpha;
    target->CreateSolidColorBrush(currentBg, &bgBrush);

    if (bgBrush) {
        target->FillRectangle(D2D1::RectF(x, y, x + w, y + h), bgBrush);
        bgBrush->Release();
    }

    float imgW = w * 0.7f;
    float imgH = h * 0.5f;
    float imgX = x + (w - imgW) / 2.0f;
    float imgY = y + (h - imgH) / 2.8f;

    if (currentBmp) {
        ID2D1SolidColorBrush* blackFrame = nullptr;
        ID2D1SolidColorBrush* whiteFrame = nullptr;
        target->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, layerAlpha * (180.0f / 255.0f)), &blackFrame);
        target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &whiteFrame);

        if (blackFrame) {
            target->FillRectangle(D2D1::RectF(imgX - 6.0f, imgY - 6.0f, imgX + imgW + 6.0f, imgY + imgH + 6.0f), blackFrame);
            blackFrame->Release();
        }
        if (whiteFrame) {
            target->FillRectangle(D2D1::RectF(imgX - 3.0f, imgY - 3.0f, imgX + imgW + 3.0f, imgY + imgH + 3.0f), whiteFrame);
            whiteFrame->Release();
        }

        float alphaVal = fadeAnim.current * layerAlpha;
        target->DrawBitmap(
            currentBmp,
            D2D1::RectF(imgX, imgY, imgX + imgW, imgY + imgH),
            alphaVal,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );
    }

    float gaugeY = imgY + imgH + 25.0f;
    float gaugeW = imgW;
    float gaugeH_size = 10.0f;
    float gaugeX = imgX;

    ID2D1SolidColorBrush* gaugeChassisBrush = nullptr;
    ID2D1SolidColorBrush* gaugeFillBrush = nullptr;

    D2D1_COLOR_F accChassis = accentColor; accChassis.a = layerAlpha * (50.0f / 255.0f);
    D2D1_COLOR_F accFill = accentColor; accFill.a = layerAlpha;

    target->CreateSolidColorBrush(accChassis, &gaugeChassisBrush);
    target->CreateSolidColorBrush(accFill, &gaugeFillBrush);

    if (gaugeChassisBrush) {
        target->FillRectangle(D2D1::RectF(gaugeX, gaugeY, gaugeX + gaugeW, gaugeY + gaugeH_size), gaugeChassisBrush);
        gaugeChassisBrush->Release();
    }
    if (gaugeFillBrush) {
        target->FillRectangle(D2D1::RectF(gaugeX, gaugeY, gaugeX + gaugeW * gaugeAnim.current, gaugeY + gaugeH_size), gaugeFillBrush);
        gaugeFillBrush->Release();
    }

    if (fontBold) {
        ID2D1SolidColorBrush* textBrush = nullptr;
        target->CreateSolidColorBrush(accFill, &textBrush);
        if (textBrush) {
            int percent = static_cast<int>(std::round(gaugeAnim.current * 100.0f));
            std::wstring percentText = std::to_wstring(percent) + L" %";
            D2D1_RECT_F textRect = D2D1::RectF(gaugeX + gaugeW - 70.0f, gaugeY + gaugeH_size + 10.0f, x + w, y + h);

            target->DrawTextW(percentText.c_str(), (UINT32)percentText.length(), fontBold, textRect, textBrush);
            textBrush->Release();
        }
    }

    float brightness = (bgColor.r * 0.299f) + (bgColor.g * 0.587f) + (bgColor.b * 0.114f);
    ID2D1SolidColorBrush* lineBrush = nullptr;

    if (brightness < 0.5f) {
        target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, layerAlpha), &lineBrush);
    }
    else {
        target->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, layerAlpha), &lineBrush);
    }

    if (lineBrush) {
        float lineY = gaugeY + gaugeH_size + 5.0f;
        float lineX = imgX + 15.0f;
        float lineW = imgW - 30.0f;
        float lineH_thickness = 3.0f;
        target->FillRectangle(D2D1::RectF(lineX, lineY, lineX + lineW, lineY + lineH_thickness), lineBrush);
        lineBrush->Release();
    }
}
