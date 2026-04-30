#pragma once
#include "pch.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "Application.h"


#define DEFAULT_WINDOW_CLASS "MARKFRAMEWORK2"
#define DEFAULT_WINDOW_TITLE "MarkFramework2 Player"

static constexpr DWORD DEFAULT_WINDOW_WIDTH = 1280;
static constexpr DWORD DEFAULT_WINDOW_HEIGHT = 720;

HBRUSH hBackgroundBrush = nullptr;
Application* g_pApp = nullptr;

HRESULT __stdcall WMCreateMainWindow(
    const char* szWindowTitle,
    HINSTANCE hInstance,
    LPSTR lpCmdLine,
    int nCmdShow,
    int xPos,
    int yPos,
    int Width,
    int Height,
    UINT32 BackgroundColor,
    HWND* ppWindowHandle
);

LRESULT __stdcall OnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    HWND hWnd = nullptr;
    if (FAILED(WMCreateMainWindow(
        DEFAULT_WINDOW_TITLE,
        hInstance,
        lpCmdLine,
        nCmdShow,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        0xFFFFFFFFu,
        &hWnd
    )))
    {
        return -1;
    }

    ::ShowWindow(hWnd, SW_SHOW);

    g_pApp = new Application();
    if (!g_pApp->OnInit(hWnd, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT))
    {
        CHECK_DELETE(g_pApp);
        return -1;
    }

    MSG msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, nullptr, 0U, 0U, PM_NOREMOVE);

    while (WM_QUIT != msg.message)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            if (g_pApp)
                g_pApp->OnUpdate();
        }
    }

    CHECK_DELETE(g_pApp);

    return 0;
}

HRESULT __stdcall WMCreateMainWindow(
    const char* szWindowTitle,
    HINSTANCE hInstance,
    LPSTR lpCmdLine,
    int nCmdShow,
    int xPos,
    int yPos,
    int Width,
    int Height,
    UINT32 BackgroundColor,
    HWND* ppWindowHandle
)
{

    unsigned bg = BackgroundColor;
    unsigned r = (bg & 0xff000000) >> 24;
    unsigned g = (bg & 0x00ff0000) >> 16;
    unsigned b = (bg & 0x0000ff00) >> 8;
    hBackgroundBrush = CreateSolidBrush(RGB(r, g, b));

    char szWindowClass[256] = { '\0' };
    sprintf_s(szWindowClass, 256, "WINDOW_CLASS_%s", szWindowTitle);

    WNDCLASS WindowClass = { 0 };
    //WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = OnMsgProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = WS_EX_NOPARENTNOTIFY;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = hBackgroundBrush;
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = szWindowClass;
    //WindowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassA(&WindowClass))
        return E_FAIL;

    DWORD dwExStyle = 0;
    DWORD dwStyle = 0;

    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    RECT rc;
    SetRect(&rc, 0, 0, Width, Height);
    AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

    /*HWND hWnd = CreateWindowEx(dwExStyle, szWindowClass, szWindowTitle, dwStyle,
                               xPos, yPos, rc.right - rc.left, rc.bottom - rc.top,
                               NULL, NULL, hInstance, NULL);*/
    HWND hWnd = CreateWindow(szWindowClass, szWindowTitle, dwStyle,
                             xPos, yPos, rc.right - rc.left, rc.bottom - rc.top,
                             NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        DWORD dwError = GetLastError();
        *ppWindowHandle = nullptr;
        return HRESULT_FROM_WIN32(dwError);
    }

    *ppWindowHandle = hWnd;

    return S_OK;
}

LRESULT __stdcall OnMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




