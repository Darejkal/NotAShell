#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include "shellCommand.h"
namespace ms {
const WCHAR parentClassName[] = L"myWindowClass";
int LINE_SIZE = 0;
RECT windowRect = {0, 0, 10, 10}, staticRect = {0, 0, 0, 0};
HWND hwndMain, hwndStatic, hwndEdit;
HANDLE fileSetting;
std::wstring staticContent = L"", editContent = L"";

WNDPROC WPA;
RECT getStringBorderW(std::wstring text, HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    HFONT hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    RECT rect = {0, 0, 0, 0};
    DrawTextW(hdc, text.c_str(), text.size(), &rect, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX);

    TEXTMETRIC v;
    GetTextMetrics(hdc, &v);
    rect.right = rect.right + (v.tmAveCharWidth / 2);
    rect.bottom = rect.bottom + (v.tmAveCharWidth / 2);

    // Clean the Font DC
    ReleaseDC(hwnd, hdc);
    DeleteFont(hOldFont);
    DeleteFont(hFont);

    // Getting the actual styles
    LONG s = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    LONG g = GetWindowLongPtr(hwnd, GWL_STYLE);

    // change the rectangle size for borders
    AdjustWindowRectEx(&rect, g, FALSE, s);
    return rect;
}
std::wstring getCurrentPath() {
    int bufferSize = GetCurrentDirectory(0, NULL);
    std::vector<TCHAR> pathBuf(bufferSize);
    GetCurrentDirectory(bufferSize, &pathBuf.at(0));
    std::wstring path(pathBuf.begin(), pathBuf.end() - 1);
    return path;
}
LRESULT CALLBACK hwndMainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (msg) {
        case WM_CREATE:
            GetWindowRect(hwnd, &windowRect);
            windowRect.right = windowRect.right - windowRect.left;
            windowRect.bottom = windowRect.bottom - windowRect.top;
            windowRect.left = 0;
            windowRect.top = 0;
            result = DefWindowProcW(hwnd, msg, wParam, lParam);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            result = DefWindowProcW(hwnd, msg, wParam, lParam);
            if (wParam == SIZE_MINIMIZED || hwndEdit == NULL) {
                break;
            }
            GetWindowRect(hwnd, &windowRect);
            windowRect.right = windowRect.right - windowRect.left;
            windowRect.bottom = windowRect.bottom - windowRect.top;
            windowRect.left = 0;
            windowRect.top = 0;
            staticRect = getStringBorderW(staticContent, hwndStatic);
            RECT directRect;
            directRect = getStringBorderW(currentDirectoryText, hwndStatic);
            SetWindowPos(hwndStatic, HWND_BOTTOM, staticRect.left, staticRect.top,
                         staticRect.right - staticRect.left, staticRect.bottom - staticRect.top, SWP_NOMOVE);
            SetWindowPos(hwndEdit, HWND_TOP, directRect.right,
                         staticRect.bottom - LINE_SIZE, windowRect.right - directRect.right, LINE_SIZE, SWP_SHOWWINDOW);
            break;
        case WM_KEYDOWN: {
            WORD wScrollNotify = 0xFFFF;
            switch (wParam) {
                case VK_UP:
                    wScrollNotify = SB_LINEUP;
                    break;

                case VK_PRIOR:
                    wScrollNotify = SB_PAGEUP;
                    break;

                case VK_NEXT:
                    wScrollNotify = SB_PAGEDOWN;
                    break;

                case VK_DOWN:
                    wScrollNotify = SB_LINEDOWN;
                    break;

                case VK_HOME:
                    wScrollNotify = SB_TOP;
                    break;

                case VK_END:
                    wScrollNotify = SB_BOTTOM;
                    break;
            }

            if (wScrollNotify != -1)
                SendMessage(hwnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L);

            break;
        }
        case WM_VSCROLL:
            // Get all the vertical scroll bar information
            SCROLLINFO si;
            int iVertPos;
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            // Save the position for comparison later on

            iVertPos = si.nPos;

            switch (LOWORD(wParam)) {
                case SB_TOP:
                    si.nPos = si.nMin;
                    break;

                case SB_BOTTOM:
                    si.nPos = si.nMax;
                    break;

                case SB_LINEUP:
                    si.nPos -= 1;
                    break;

                case SB_LINEDOWN:
                    si.nPos += 1;
                    break;

                case SB_PAGEUP:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGEDOWN:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBTRACK:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }
            // Set the position and then retrieve it.  Due to adjustments
            //   by Windows it may not be the same as the value set.

            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            // If the position has changed, scroll the window and update it

            if (si.nPos != iVertPos) {
                ScrollWindow(hwnd, 0, LINE_SIZE * (iVertPos - si.nPos),
                             NULL, NULL);
                UpdateWindow(hwnd);
            }
            return 0;
        default:
            result = DefWindowProcW(hwnd, msg, wParam, lParam);
            break;
    }
    return result;
}

LRESULT CALLBACK hwndEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CHAR) {
        switch (wParam) {
            case 1:  // Ctrl+A
                SendMessage(hwnd, EM_SETSEL, 0, -1);
                return 1;
            case 5:  // Ctrl+E
                SendMessage(hwndMain, WM_CLOSE, 0, 0);
                return 1;
            case 13:  // Enter
                //Get content of Edit window
                editContent.resize(GetWindowTextLength(hwnd));
                editContent.resize(GetWindowTextW(hwnd, (LPWSTR)editContent.data(), editContent.size() + 1));
                //Append content to Static window
                staticContent.append(editContent).append(L"\r\n");  // NewLine OK
                scExecuteCommand(hwndMain,staticContent,editContent,PathFileDirectory,PathCurrentDirectory);
                staticContent.append(currentDirectoryText);  // No NewLine OK

                editContent = L"";

                SetWindowTextW(hwndStatic, staticContent.c_str());

                SetWindowTextW(hwndEdit, editContent.c_str());

                SendMessage(hwndMain, WM_SIZE, SIZE_RESTORED, 0);
                return 1;
        }
    }
    return CallWindowProc(WPA, hwnd, msg, wParam, lParam);
}
int createWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    PathFileDirectory = getCurrentPath().append(L"\\");
    PathCurrentDirectory = PathFileDirectory;
    currentDirectoryText=shellName;
    currentDirectoryText.append(PathCurrentDirectory).append(L">");
    // FreeConsole();
    WNDCLASSEXW wc = {};
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = hwndMainProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = (HICON)LoadImage(NULL, TEXT(".\\src\\img\\logo.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    if (wc.hIcon == NULL) {
        printf("Load Image Error: %x\n", GetLastError());
    }
    wc.hIconSm = wc.hIcon;

    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 255, 255));
    // TODO:
    wc.lpszMenuName = NULL;
    wc.lpszClassName = &parentClassName[0];

    if (!RegisterClassExW(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("myShell"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwndMain = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        parentClassName,
        L"myShell",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);
    ShowWindow(hwndMain, nCmdShow);

    staticContent = currentDirectoryText;
    staticRect = getStringBorderW(staticContent, NULL);
    LINE_SIZE = staticRect.bottom - staticRect.top;
    hwndStatic = CreateWindowW(L"Edit",
                               (LPWSTR)staticContent.data(),
                               WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_CLIPSIBLINGS,
                               staticRect.left, staticRect.top, staticRect.right, staticRect.bottom,
                               hwndMain,
                               0,
                               hInstance, NULL);
    hwndEdit = CreateWindowW(L"Edit",
                             (LPWSTR)editContent.data(),
                             WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS,
                             staticRect.right, staticRect.top, windowRect.right, staticRect.bottom,
                             hwndMain,
                             0,
                             hInstance, NULL);
    ShowScrollBar(hwndEdit, SB_BOTH, FALSE);
    WPA = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)hwndEditProc);

    bool isUnderlined = false;
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        isUnderlined = !isUnderlined;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
}  // namespace ms