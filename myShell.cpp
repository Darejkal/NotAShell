#include <windows.h>
#include <windowsx.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
const std::wstring PATH_DEFAULT(L"src\\def\\settings.ini");
const WCHAR parentClassName[] = L"myWindowClass";
int LINE_SIZE = 0;
RECT windowRect = {0, 0, 10, 10}, staticRect = {0, 0, 0, 0};
HWND hwndMain, hwndStatic, hwndEdit;
HANDLE fileSetting;
HANDLE childOutRead, childOutWrite, childInRead, childInWrite;
std::wstring staticContent = L"", editContent = L"", currentDirectoryText = L"myShell>";
std::wstring PathCurrentDirectory, PathFileDirectory, PathDefault = L"src\\plugins\\", PathConfig;
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (msg) {
        case WM_CREATE:
            GetWindowRect(hwnd, &windowRect);
            windowRect.right = windowRect.right - windowRect.left;
            windowRect.bottom = windowRect.bottom - windowRect.top;
            windowRect.left = 0;
            windowRect.top = 0;
            result = DefWindowProc(hwnd, msg, wParam, lParam);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            result = DefWindowProc(hwnd, msg, wParam, lParam);
            if (wParam == SIZE_MINIMIZED || hwndEdit == NULL) {
                // printf("-----Maximized ??? %i\n", wParam == SIZE_MAXIMIZED);
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
            SetWindowPos(hwndStatic, HWND_BOTTOM, staticRect.left, staticRect.top, staticRect.right - staticRect.left, staticRect.bottom - staticRect.top, SWP_NOMOVE);
            SetWindowPos(hwndEdit, HWND_TOP, directRect.right, staticRect.bottom - LINE_SIZE, windowRect.right - directRect.right, LINE_SIZE, SWP_SHOWWINDOW);
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
            result = DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }
    return result;
}
std::vector<std::wstring> splitW(std::wstring s, std::wstring delimiter) {
    size_t pos_start = 0, pos_end, s_len = s.length(), delim_len = delimiter.length();
    std::wstring token;
    std::vector<std::wstring> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::wstring::npos) {
        for (int i = pos_start; i < pos_end; i++) {
            if (s[i] != delimiter[i - pos_start]) {
                goto notADelimiter;
            }
        }
        pos_start = pos_end + delim_len;
        continue;
    notADelimiter:
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}
bool executeCommand(std::wstring command) {
    return 1;
    // std::wcout << command << " *****\n";
    std::vector<std::wstring> cmdArr = splitW(command, L" ");
    // for (auto x : cmdArr) {
    //     std::wcout << x << "\n";
    // }
    std::wstring temp = PathFileDirectory;
    temp.append(PathDefault).append(cmdArr[0]);
    HANDLE f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // std::wcout << temp << "\n";
    if (f == INVALID_HANDLE_VALUE) {
        temp = PathFileDirectory;
        temp.append(PathConfig).append(cmdArr[0]);
        // std::wcout << temp << "\n";
        CloseHandle(f);
        f = CreateFileW(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        // std::wcout << temp;
        if (f == INVALID_HANDLE_VALUE) {
            staticContent.append(cmdArr[0]).append(L" is not a valid or known command.\r\n");  // NewLine OK
            CloseHandle(f);
            return 0;
        }
    }
    CloseHandle(f);
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    CreatePipe(&childOutRead, &childOutWrite, &saAttr, 0);
    SetHandleInformation(childOutRead, HANDLE_FLAG_INHERIT, 0);
    PROCESS_INFORMATION cmdProcess;
    ZeroMemory(&cmdProcess, sizeof(cmdProcess));
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = L"Winsta0\\default";
    si.hStdOutput = childOutWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;
    CreateProcessW(cmdArr[0].c_str(), &command[0], NULL, NULL, TRUE,
                   0, NULL, NULL, &si, &cmdProcess);

    int bufferSize = 100;
    wchar_t buffer[bufferSize];
    DWORD readed;
    OVERLAPPED overlapped;
    // staticContent.append(L"\r\n");
    std::wcout << "BeginRead\n-\n";
    // while(ReadFile(childOutRead, buffer, bufferSize, &readed, NULL)!=0&&readed!=0){
    //     staticContent.append(buffer);
    // }
    ReadFile(childOutRead, buffer, bufferSize, &readed, NULL);
    std::wcout << "ReadOnce\n-\n";

    for (;;) {
        if (!ReadFile(childOutRead, buffer, bufferSize, &readed, NULL) || readed == 0) {
            std::wcout << "reading\n";
        }
    }
    std::wcout << "EndRead\n-\n";
    SetWindowTextW(hwndStatic, staticContent.c_str());
    // staticContent.append(cmdArr[0]).append(L" is a valid or known command.\r\n");
    SendMessage(hwndMain, WM_SIZE, SIZE_RESTORED, 0);
    CloseHandle(childOutRead);
    CloseHandle(childOutWrite);
    CloseHandle(childInRead);
    CloseHandle(childInWrite);
    return 1;
}
LRESULT CALLBACK Edit_Prc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CHAR) {
        switch (wParam) {
            case 1:  // Ctrl+A
                SendMessage(hwnd, EM_SETSEL, 0, -1);
                return 1;
            case 3:
                SendMessage(hwndMain, WM_CLOSE, 0, 0);
            case 5:
                CloseWindow(hwndMain);
                return 1;
            case 13:  // Enter
                editContent.resize(GetWindowTextLength(hwnd));

                editContent.resize(GetWindowTextW(hwnd, (LPWSTR)editContent.data(), editContent.size() + 1));
                staticContent.append(editContent).append(L"\r\n");  // NewLine OK
                executeCommand(editContent);
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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    // TODO:
    PathFileDirectory = getCurrentPath().append(L"\\");
    PathCurrentDirectory = PathFileDirectory;
    // FreeConsole();
    WNDCLASSEXW wc = {};
    // ZeroMemory(&wc,sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
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
    // HFONT staticFont = CreateFont(TEXT_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
    //                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
    //                               DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
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
    // editContent = L"";
    hwndEdit = CreateWindowW(L"Edit",
                             (LPWSTR)editContent.data(),
                             WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS,
                             // LPtoDP((HDC)hwnd,staticSize.cx),LPtoDP((HDC)hwnd,staticSize.cy)-TEXT_FONT_HEIGHT,1000, 1000,
                             staticRect.right, staticRect.top, windowRect.right, staticRect.bottom,
                             hwndMain,
                             0,
                             hInstance, NULL);
    ShowScrollBar(hwndEdit, SB_BOTH, FALSE);
    WPA = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)Edit_Prc);
    OFSTRUCT os_temp;
    std::wstring settingPath(PathCurrentDirectory);
    settingPath.append(PATH_DEFAULT);
    std::wcout << settingPath << "\n";
    fileSetting = CreateFileW(settingPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fileSetting == INVALID_HANDLE_VALUE) {
        MessageBox(
            NULL,
            TEXT("Setting File not found.\r\n Will create a new one."),
            TEXT("Warning"),
            MB_ICONEXCLAMATION | MB_OK);
        CloseHandle(fileSetting);
        fileSetting = CreateFileW(settingPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    CloseHandle(fileSetting);
    bool isUnderlined = false;
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        isUnderlined = !isUnderlined;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}