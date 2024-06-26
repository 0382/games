#pragma once
#ifndef GAMES_WINDOW_HPP
#define GAMES_WINDOW_HPP

#ifndef UNICODE
#define UNICODE
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <mutex>

class Application
{
  private:
    bool m_debug;

  public:
    Application(bool debug = false)
    {
        m_debug = debug;
        if (m_debug)
        {
            ::AllocConsole();
            std::FILE *pfStdOut = nullptr;
            std::FILE *pfStdIn = nullptr;
            ::freopen_s(&pfStdOut, "CONOUT$", "w", stdout);
            ::freopen_s(&pfStdIn, "CONIN$", "r", stdin);
        }
    }
    ~Application()
    {
        if (m_debug)
        {
            ::FreeConsole();
        }
    }

    void exec()
    {
        MSG msg = {};
        while (::GetMessageW(&msg, NULL, 0, 0))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }
};

class RawCanvas
{
  private:
    BITMAPINFO m_bmi;
    uint8_t *m_bits;
    uint8_t *m_bits2;
    int m_width;
    int m_height;
    std::mutex m_mtx;

  public:
    RawCanvas(int width, int height) : m_width(width), m_height(height)
    {
        m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        m_bmi.bmiHeader.biWidth = width;
        m_bmi.bmiHeader.biHeight = -height;
        m_bmi.bmiHeader.biPlanes = 1;
        m_bmi.bmiHeader.biBitCount = 24;
        m_bmi.bmiHeader.biCompression = BI_RGB;
        m_bmi.bmiHeader.biSizeImage = 0;
        m_bmi.bmiHeader.biXPelsPerMeter = 0;
        m_bmi.bmiHeader.biYPelsPerMeter = 0;
        m_bmi.bmiHeader.biClrUsed = 0;
        m_bmi.bmiHeader.biClrImportant = 0;
        m_bits = new uint8_t[3 * width * height];
        m_bits2 = new uint8_t[3 * width * height];
    }

    ~RawCanvas()
    {
        delete[] m_bits;
        delete[] m_bits2;
    }

    void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        auto p = m_bits + 3 * (y * m_width + x);
        p[0] = b;
        p[1] = g;
        p[2] = r;
    }

    void clear(uint8_t r, uint8_t g, uint8_t b)
    {
        for (int i = 0; i < m_width * m_height; ++i)
        {
            auto p = m_bits + 3 * i;
            p[0] = b;
            p[1] = g;
            p[2] = r;
        }
    }

    int width() const { return m_width; }
    int height() const { return m_height; }

    void beginpaint() {}

    void endpaint()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        std::memcpy(m_bits2, m_bits, 3 * m_width * m_height);
    }

    void to_dc(HDC hdc, int width, int height)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        ::StretchDIBits(hdc, 0, 0, width, height, 0, 0, m_width, m_height, m_bits2, &m_bmi, DIB_RGB_COLORS, SRCCOPY);
    }
};

/* modified from Microsoft's Direct2D tutorial code
 * url:
 * https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/begin/LearnWin32/Direct2DCircle/cpp/basewin.h
 */
template <typename Child>
class BaseWindow
{
  public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        Child *pThis = nullptr;
        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
            pThis = (Child *)pCreate->lpCreateParams;
            ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (Child *)::GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->handle_message(uMsg, wParam, lParam);
        }
        else
        {
            LPARAM lParam = 0;
            return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL) {}
    HWND Window() const { return m_hwnd; }

    BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = 0, HMENU hMenu = 0)
    {
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = Child::WindowProc;
        wc.hInstance = ::GetModuleHandleW(NULL);
        wc.lpszClassName = classname();
        ::RegisterClassW(&wc);

        m_hwnd = ::CreateWindowExW(dwExStyle, classname(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
                                   hMenu, ::GetModuleHandleW(NULL), this);

        return (m_hwnd ? TRUE : FALSE);
    }

  protected:
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CREATE: return static_cast<Child *>(this)->on_create();
            case WM_DESTROY: return static_cast<Child *>(this)->on_destroy();
            case WM_TIMER: return static_cast<Child *>(this)->on_timer();
            case WM_PAINT: return static_cast<Child *>(this)->on_paint();
            case WM_SIZE: return static_cast<Child *>(this)->on_resize();
            case WM_SIZING: return static_cast<Child *>(this)->on_resizing(wParam, (RECT *)lParam);
            default: return ::DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
        }
    }

    virtual PCWSTR classname() const = 0;
    HWND m_hwnd;
};

struct SizePolicy
{
    int m_minWidth;
    int m_maxWidth;
    int m_minHeight;
    int m_maxHeight;
    double m_aspectRatio;
    static SizePolicy fixed(int width, int height) { return {width, width, height, height, 0.0}; }
    static SizePolicy ratio(double aspectRatio) { return {0, INT_MAX, 0, INT_MAX, aspectRatio}; }
    static SizePolicy minmax(int minWidth, int maxWidth, int minHeight, int maxHeight)
    {
        return {minWidth, maxWidth, minHeight, maxHeight, 0.0};
    }
    static SizePolicy any() { return {0, INT_MAX, 0, INT_MAX, 0.0}; }
    bool compatible(int width, int height) const
    {
        if (m_aspectRatio > 0.0)
        {
            return width >= m_minWidth && width <= m_maxWidth && height >= m_minHeight && height <= m_maxHeight &&
                   (double)width / height == m_aspectRatio;
        }
        else
        {
            return width >= m_minWidth && width <= m_maxWidth && height >= m_minHeight && height <= m_maxHeight;
        }
    }
};

class MainWindow : public BaseWindow<MainWindow>
{
  private:
    SizePolicy m_size_policy;
    RawCanvas &m_canvas;
    int m_fps = 0;

    BOOL invalidate() { return ::InvalidateRect(m_hwnd, nullptr, FALSE); }

  public:
    MainWindow(SizePolicy policy, RawCanvas &canvas, int fps) : m_size_policy(policy), m_canvas(canvas), m_fps(fps) {}
    bool init(const wchar_t *title, int width, int height)
    {
        if (m_size_policy.compatible(width, height))
            return Create(title, WS_OVERLAPPEDWINDOW, 0, CW_USEDEFAULT, CW_USEDEFAULT, width, height);
        return false;
    }
    PCWSTR classname() const override { return L"MainWindow"; }
    BOOL show(int nCmdShow) { return ::ShowWindow(m_hwnd, nCmdShow); }

    LRESULT on_create()
    {
        if (m_fps > 0)
        {
            ::SetTimer(m_hwnd, 1, 1000 / m_fps, nullptr);
        }
        return 0;
    }

    LRESULT on_destroy()
    {
        ::PostQuitMessage(0);
        return 0;
    }

    LRESULT on_timer() { return m_fps > 0 ? !invalidate() : 0; }
    LRESULT on_resize() { return !invalidate(); }
    LRESULT on_resizing(WPARAM wParam, RECT *rc)
    {
        RECT wrc;
        GetWindowRect(m_hwnd, &wrc);
        int w_width = wrc.right - wrc.left;
        int w_height = wrc.bottom - wrc.top;
        int width = rc->right - rc->left;
        int height = rc->bottom - rc->top;
        width = std::clamp(width, m_size_policy.m_minWidth, m_size_policy.m_maxWidth);
        height = std::clamp(height, m_size_policy.m_minHeight, m_size_policy.m_maxHeight);
        if (m_size_policy.m_aspectRatio > 0)
        {
            int w = height * m_size_policy.m_aspectRatio;
            int h = width / m_size_policy.m_aspectRatio;
            if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT)
            {
                height = h;
            }
            else if (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM)
            {
                width = w;
            }
            else
            {
                bool outer = width > w_width && height > w_height;
                if (outer ^ (width < w))
                    height = h;
                else
                    width = w;
            }
        }
        if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT)
        {
            int h_center = rc->top + (rc->bottom - rc->top) / 2;
            rc->top = h_center - height / 2;
            rc->bottom = h_center + height / 2;
            if (wParam == WMSZ_LEFT)
                rc->left = rc->right - width;
            else
                rc->right = rc->left + width;
        }
        else if (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM)
        {
            int w_center = rc->left + (rc->right - rc->left) / 2;
            rc->left = w_center - width / 2;
            rc->right = w_center + width / 2;
            if (wParam == WMSZ_TOP)
                rc->top = rc->bottom - height;
            else
                rc->bottom = rc->top + height;
        }
        else if (wParam == WMSZ_TOPLEFT)
        {
            rc->left = rc->right - width;
            rc->top = rc->bottom - height;
        }
        else if (wParam == WMSZ_TOPRIGHT)
        {
            rc->right = rc->left + width;
            rc->top = rc->bottom - height;
        }
        else if (wParam == WMSZ_BOTTOMLEFT)
        {
            rc->left = rc->right - width;
            rc->bottom = rc->top + height;
        }
        else if (wParam == WMSZ_BOTTOMRIGHT)
        {
            rc->right = rc->left + width;
            rc->bottom = rc->top + height;
        }
        return 0;
    }

    LRESULT on_paint()
    {
        PAINTSTRUCT ps;
        auto hdc = ::BeginPaint(m_hwnd, &ps);
        RECT rc;
        ::GetClientRect(m_hwnd, &rc);
        m_canvas.to_dc(hdc, rc.right, rc.bottom);
        ::EndPaint(m_hwnd, &ps);
        return 0;
    }
};

#endif // GAMES_WINDOW_HPP