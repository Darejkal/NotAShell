#include <windows.h>
#include <windowsx.h>
#include <cstdio>
#include <string>
#define TEXT_FONT_HEIGHT 16
const TCHAR parentClassName[] = TEXT("myWindowClass");
// const TCHAR mainViewClassName[] = TEXT("myWindowClassMain");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
// void writeOnWindow(HWND hWnd,LPCTSTR message)
// {
//     RECT  rect;
//     PAINTSTRUCT ps;
//     HDC hdc = BeginPaint(hWnd, &ps);

//     GetClientRect(hWnd, &rect);
//     SetTextColor(hdc, RGB(255,255,255));
//     SetBkMode(hdc, TRANSPARENT);
//     rect.left = 0;
//     rect.top = 0;
//     DrawText(hdc, message, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
//     EndPaint(hWnd, &ps);
// }
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc={};
    //If incremented, decremented: Window Registration Failed
    //Usually is just a sizeof(...)
    wc.cbSize = sizeof(WNDCLASSEX);

    //I have no use for this.
    wc.style = 0;
    //Handle messages.
    wc.lpfnWndProc = WndProc;
    //Useless for now
    wc.cbClsExtra = 0;
    // DLGWINDOWEXTRA something
    wc.cbWndExtra = 0;
    //like Handle
    wc.hInstance = hInstance;
    //icon
    wc.hIcon = (HICON)LoadImage(NULL,TEXT(".\\src\\img\\logo.ico"),IMAGE_ICON,0,0,LR_LOADFROMFILE);
    if(wc.hIcon==NULL){
        printf("Load Image Error: %x\n",GetLastError());
    }
    wc.hIconSm = wc.hIcon;

    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //CreateSolidBrush create a HBRUSH used for coloring.
    wc.hbrBackground = CreateSolidBrush(RGB(0,0,0));
    //TODO:
    wc.lpszMenuName = NULL;
    wc.lpszClassName = parentClassName;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("myShell"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    // WNDCLASSEX wcMain={};
    //     wcMain.cbSize = sizeof(WNDCLASSEX);
    //     wcMain.style = 0;
    //     wcMain.lpfnWndProc = WndProc;
    //     wcMain.cbClsExtra = 0;
    //     wcMain.cbWndExtra = 0;
    //     wcMain.hInstance = hInstance;
    //     wcMain.hCursor = LoadCursor(NULL, IDC_ARROW);
    //     wcMain.hbrBackground = CreateSolidBrush(RGB(255,255,255));
    //     wcMain.lpszMenuName = NULL;
    //     wcMain.lpszClassName = mainViewClassName;
    // if (!RegisterClassEx(&wcMain)) {
    //     MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("myShell"),
    //                MB_ICONEXCLAMATION | MB_OK);
    //     return 0;
    // }

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        parentClassName,
        TEXT("myShell"),
        WS_OVERLAPPEDWINDOW| WS_HSCROLL |
                                WS_VSCROLL | ES_LEFT | ES_MULTILINE |
                                ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    std::string staticContent="myShell>";
    HFONT staticFont = CreateFont (TEXT_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    HWND hwndStatic = CreateWindow("Edit",
                                staticContent.c_str(),  
                                WS_OVERLAPPED|WS_CHILD | WS_VISIBLE |ES_READONLY|ES_MULTILINE,
                                0, 0,1000, 1000,
                                hwnd,
                                0,
                                hInstance,NULL);
    SendMessage(hwndStatic, WM_SETFONT, (WPARAM)staticFont, TRUE);
    SIZE staticSize;
    GetTextExtentPoint32(GetDC(hwndStatic), staticContent.c_str(), staticContent.size(), &staticSize);
    MoveWindow(hwndStatic,0,0,staticSize.cx,staticSize.cy,TRUE);
    HWND hwndEdit = CreateWindow("Edit",
                                TEXT("oh"),  
                                WS_OVERLAPPED|WS_CHILD | WS_VISIBLE|ES_MULTILINE,
                                staticSize.cx,staticSize.cy-TEXT_FONT_HEIGHT,1000, 1000,
                                hwnd,
                                0,
                                hInstance,NULL);
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)staticFont, TRUE);
    
    if (hwnd == NULL||hwndEdit == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("myShell"),
                   MB_ICONEXCLAMATION | MB_OK);
        printf("Error: %x",GetLastError());
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    
    bool isUnderlined=false;
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        isUnderlined=!isUnderlined;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}