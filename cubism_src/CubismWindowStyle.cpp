#include "CubismWindowStyle.h"
#include <dwmapi.h>
#include "../global.h"

WINSTYLE CubismWindowStyle::ws= WINSTYLE::WinTop;
WINSTYLE CubismWindowStyle::winStyle= WINSTYLE::WinTop;
bool CubismWindowStyle::canTrans=true;

void CubismWindowStyle::SetWindowTopApha(HWND hwnd, bool isTop, bool isApha)
{
    int intExTemp = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (isTop)
    {
        intExTemp |= WS_EX_TOPMOST;
        if (!isApha) intExTemp &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
        else intExTemp |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
    }
    else
    {
        intExTemp &= ~WS_EX_TOPMOST;
        if (isApha) intExTemp |= (WS_EX_LAYERED | WS_EX_TRANSPARENT);
        else intExTemp &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
    }
    SetWindowLong(hwnd, GWL_EXSTYLE, intExTemp);
    if (isTop) SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
    else SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

BOOL CubismWindowStyle::GetPixelRGBA(HWND hwnd, int x, int y, BYTE& r, BYTE& g, BYTE& b, BYTE& a)
{
    // 获取窗口设备上下文
    HDC hdcWindow = GetDC(hwnd);
    if (!hdcWindow) {
        return FALSE;
    }
    // 创建内存DC和位图
    HDC hdcMem = CreateCompatibleDC(hdcWindow);
    if (!hdcMem) {
        ReleaseDC(hwnd, hdcWindow);
        return FALSE;
    }
    HBITMAP hbm = CreateCompatibleBitmap(hdcWindow, 1, 1);
    if (!hbm) {
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdcWindow);
        return FALSE;
    }
    HGDIOBJ hOldBitmap = SelectObject(hdcMem, hbm);
    // 将目标像素区域复制到内存DC
    if (!BitBlt(hdcMem, 0, 0, 1, 1, hdcWindow, x, y, SRCCOPY)) {
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hbm);
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdcWindow);
        return FALSE;
    }
    // 准备位图信息
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 1;
    bi.bmiHeader.biHeight = -1;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    // 读取像素数据
    DWORD pixelData = 0;
    if (!GetDIBits(hdcMem, hbm, 0, 1, &pixelData, &bi, DIB_RGB_COLORS)) {
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hbm);
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdcWindow);
        return FALSE;
    }
    // 提取RGBA
    BYTE* color = reinterpret_cast<BYTE*>(&pixelData);
    b = color[0]; // 蓝色分量
    g = color[1]; // 绿色分量
    r = color[2]; // 红色分量
    a = color[3]; // Alpha分量
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcWindow);
    return TRUE;
}

void CubismWindowStyle::WindowStyleUpdate(HWND hwnd)
{
    if (CubismWindowStyle::canTrans)
    {
        if (winStyle == WINSTYLE::WinTop)
            SetWindowTopApha(hwnd, true, true);
        else if (winStyle == WINSTYLE::WinApha)
            SetWindowTopApha(hwnd, false, true);
        else if (winStyle == WINSTYLE::WinTopApha)
            SetWindowTopApha(hwnd, true, true);
        else if (winStyle == WINSTYLE::WinNoTopNoApha)
            SetWindowTopApha(hwnd, false, true);
    }
    else
    {
        if (winStyle == WINSTYLE::WinTop)
            SetWindowTopApha(hwnd, true, false);
        else if (winStyle == WINSTYLE::WinApha)
            SetWindowTopApha(hwnd, false, true);
        else if (winStyle == WINSTYLE::WinTopApha)
            SetWindowTopApha(hwnd, true, true);
        else if (winStyle == WINSTYLE::WinNoTopNoApha)
            SetWindowTopApha(hwnd, false, false);
    }
}

void CubismWindowStyle::Update(HWND hwnd)
{
    ws = static_cast<WINSTYLE>(GlobalConfig::getInstance()->window_cubism_style_id);
    if (ws != winStyle) {
        winStyle = ws;
    }
    POINT Mouse_X_Y;	//存放鼠标坐标结构体
    //获取鼠标坐标
    if (FALSE == GetCursorPos(&Mouse_X_Y)) {
        //printf("error：无法获取鼠标指针位于屏幕的坐标值");
    }
    else {
        //printf("鼠标的X坐标：%d,鼠标的Y坐标：%d\n", Mouse_X_Y.x, Mouse_X_Y.y);
        BYTE r, g, b, a;
        CubismWindowStyle::GetPixelRGBA(hwnd, Mouse_X_Y.x, Mouse_X_Y.y, r, g, b, a);
        //printf("%d %d %d %d\n", r, g, b, a);
        if (r == 0 && g == 0 && b == 0 && a == 0)CubismWindowStyle::canTrans = true;
        else CubismWindowStyle::canTrans = false;
    }
    CubismWindowStyle::WindowStyleUpdate(hwnd);
}

void CubismWindowStyle::SetWinStyle(const WINSTYLE& ws)
{
    CubismWindowStyle::winStyle = ws;
}
WINSTYLE CubismWindowStyle::GetWinStyle()
{
    return CubismWindowStyle::winStyle;
}
