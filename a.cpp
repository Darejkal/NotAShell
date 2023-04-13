#include <windows.h>
#include <windowsx.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
const TCHAR parentClassName[] = TEXT("myWindowClass");
int LINE_SIZE = 0;
RECT windowRect = {0, 0, 10, 10}, staticRect = {0, 0, 0, 0};
// editRect={0,0,0,0}
HWND hwndMain, hwndStatic, hwndEdit;
std::string staticContent = "", editContent = "", currentDirectoryText = "myShell>";
std::wstring CURRENT_DIRECTORY;
WNDPROC WPA;
HFILE fileSetting;
RECT getStringBorder(std::string text, HFONT font) {
    HDC dc = GetDC(NULL);
    if (font != NULL) {
        SelectObject(dc, font);
    }

    RECT rect = {0, 0, 0, 0};
    DrawText(dc, text.c_str(), text.size(), &rect, DT_CALCRECT | DT_NOPREFIX);

    ReleaseDC(NULL, dc);
    return rect;
}
std::wstring getCurrentPath() {
    std::vector<wchar_t> pathBuf;
    DWORD copied = 0;
    do {
        pathBuf.resize(pathBuf.size() + MAX_PATH);
        copied = GetModuleFileNameW(0, &pathBuf.at(0), pathBuf.size());
    } while (copied >= pathBuf.size());

    pathBuf.resize(copied);

    std::wstring path(pathBuf.begin(), pathBuf.end());
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
            // std::cout << "SIZED\n";
            result = DefWindowProc(hwnd, msg, wParam, lParam);
            if (wParam == SIZE_MINIMIZED || hwndEdit == NULL) {
                // printf("%i\n", wParam == SIZE_MAXIMIZED);
                break;
            }
            GetWindowRect(hwnd, &windowRect);
            windowRect.right = windowRect.right - windowRect.left;
            windowRect.bottom = windowRect.bottom - windowRect.top;
            windowRect.left = 0;
            windowRect.top = 0;
            staticRect = getStringBorder(staticContent, NULL);
            // printf("---> staticRect %i,%i,%i,%i\n", staticRect.left, staticRect.top, staticRect.right, staticRect.bottom);
            // GetWindowRect(hwndStatic,&staticRect);
            // staticRect.right=staticRect.right-staticRect.left;
            // staticRect.bottom=staticRect.bottom-staticRect.top;
            // staticRect.left=0;
            // staticRect.top=0;
            // editRect.right=windowRect.right;
            // GetWindowRect(hwndEdit,&editRect);
            // editRect.bottom=windowRect.bottom;
            // printf("%i %i %i %i\n",windowRect.left,windowRect.top,windowRect.right,windowRect.bottom);
            // printf("-> %i %i %i %i\n",editRect.left,editRect.top,editRect.right,editRect.bottom);
            // MoveWindow(hwndMain,windowRect.right,windowRect.bottom,windowRect.right,windowRect.bottom,TRUE);
            MoveWindow(hwndStatic, staticRect.left, staticRect.top, staticRect.right - staticRect.left, staticRect.bottom - staticRect.top, TRUE);
            // std::cout<<"--"<<staticRect.right<<","<<staticRect.bottom - LINE_SIZE<<","<<windowRect.right-staticRect.right<<","<<LINE_SIZE<<"\n";
            MoveWindow(hwndEdit, staticRect.right, staticRect.bottom - LINE_SIZE, windowRect.right - staticRect.right, LINE_SIZE, TRUE);
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

bool getWindowText(HWND hwnd, std::string& text) {
    editContent.resize(GetWindowTextLength(hwnd));
    editContent.resize(GetWindowText(hwnd, (LPSTR)editContent.data(), editContent.size() + 1));
    return 1;
}
LRESULT CALLBACK Edit_Prc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // if(msg==WM_CHAR){
    //     system("cls");
    //     printf("%i|%i %i|%i\n",msg,msg==WM_CHAR,wParam,wParam==1);

    // }
    CURRENT_DIRECTORY=getCurrentPath().c_str();
    if (msg == WM_CHAR) {
        switch (wParam) {
            case 1:  // Ctrl+A
                printf("-\n");
                SendMessage(hwnd, EM_SETSEL, 0, -1);
                return 1;
                return 1;
            case 13:  // Enter
                getWindowText(hwndEdit, editContent);
                staticContent.append(editContent).append("\r\n").append(currentDirectoryText);
                editContent = "";
                SetWindowText(hwndStatic, staticContent.c_str());
                SetWindowText(hwndEdit, editContent.c_str());
                SendMessage(hwndMain, WM_SIZE, SIZE_RESTORED, 0);
                return 1;
        }
    }
    return CallWindowProc(WPA, hwnd, msg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {};
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
    wc.lpszClassName = parentClassName;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("myShell"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwndMain = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        parentClassName,
        TEXT("myShell"),
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwndMain, nCmdShow);
    // HFONT staticFont = CreateFont(TEXT_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
    //                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
    //                               DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    staticContent = currentDirectoryText;
    staticRect = getStringBorder(staticContent, NULL);
    LINE_SIZE = staticRect.bottom - staticRect.top;
    hwndStatic = CreateWindow("Edit",
                              staticContent.c_str(),
                              WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE,
                              staticRect.left, staticRect.top, staticRect.right, staticRect.bottom,
                              hwndMain,
                              0,
                              hInstance, NULL);
    // editRect={staticRect.right,staticRect.top,1000, staticRect.bottom};
    editContent = "";
    hwndEdit = CreateWindow("Edit",
                            editContent.c_str(),
                            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_HSCROLL,
                            // LPtoDP((HDC)hwnd,staticSize.cx),LPtoDP((HDC)hwnd,staticSize.cy)-TEXT_FONT_HEIGHT,1000, 1000,
                            staticRect.right, staticRect.top, windowRect.right, staticRect.bottom,
                            hwndMain,
                            0,
                            hInstance, NULL);
    ShowScrollBar(hwndEdit, SB_BOTH, FALSE);
    WPA = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)Edit_Prc);
    OFSTRUCT os_temp;
    fileSetting = OpenFile(TEXT("\\def\\src\\settings.txt"), &os_temp, OF_EXIST);
    if (fileSetting == HFILE_ERROR) {
        MessageBox(
            NULL,
            TEXT("Setting File not found.\r\n Will create a new one."),
            TEXT("Warning"),
            MB_ICONEXCLAMATION | MB_OK);
        // fileSetting = OpenFile(TEXT("\\def\\src\\settings.txt"), &os_temp, OF_CREATE | OF_WRITE);
    } else {
        //OpenFile doesnot suppot unicode
        fileSetting = OpenFile(TEXT(CURRENT_DIRECTORY+"\\def\\src\\settings.txt"), &os_temp, OF_READ);
    }
    CloseHandle(&fileSetting);
    bool isUnderlined = false;
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        isUnderlined = !isUnderlined;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}