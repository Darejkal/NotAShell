#include <windows.h>
#include <windowsx.h>
#include <cstdio>
#include <string>
#define TEXT_FONT_HEIGHT 16
const TCHAR parentClassName[] = TEXT("myWindowClass");
RECT windowRect={0,0,10,10},staticRect={0,0,0,0},editRect={0,0,0,0};
HWND hwnd,hwndStatic,hwndEdit;
WNDPROC WPA;
RECT getStringBorder(std::string text, HFONT font) {
    HDC dc = GetDC(NULL);
    if(font!=NULL){
        SelectObject(dc, font);
    }

    RECT rect = { 0, 0, 0, 0 };
    DrawText(dc, text.c_str(), text.size(), &rect, DT_CALCRECT | DT_NOPREFIX);

    ReleaseDC(NULL,dc);
    return rect;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result=0;
    RECT tempRect;
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            result= DefWindowProc(hwnd, msg, wParam, lParam);
            if(wParam==SIZE_MINIMIZED||hwndEdit==NULL){
                printf("%i\n",wParam==SIZE_MAXIMIZED);
                break;
            }
            printf("SIZED\n");
            AdjustWindowRectEx(&tempRect,WS_OVERLAPPEDWINDOW,TRUE,WS_EX_CLIENTEDGE);
            GetWindowRect(hwnd,&windowRect);
            windowRect.right=windowRect.right-windowRect.left;
            windowRect.bottom=windowRect.bottom-windowRect.top;
            windowRect.left=0;
            windowRect.top=0;
            editRect.right=windowRect.right;
            // GetWindowRect(hwndEdit,&editRect);
            // editRect.bottom=windowRect.bottom;
            printf("%i %i %i %i\n",windowRect.left,windowRect.top,windowRect.right,windowRect.bottom);
            printf("-> %i %i %i %i\n",editRect.left,editRect.top,editRect.right,editRect.bottom);
            MoveWindow(hwndEdit,editRect.left,editRect.top,editRect.right,editRect.bottom,TRUE);
            break;
        default:
            result= DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }
    return result;
}
LRESULT CALLBACK Edit_Prc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
  if(msg==WM_CHAR&&wParam==1){SendMessage(hwnd,EM_SETSEL,0,-1); return 1;}
  else return CallWindowProc(WPA,hwnd,msg,wParam,lParam);
}
// void writeOnWindow(HWND hWnd,LPCTSTR message)
// {
//     RECT  rect;
//     PAINTSTRUCT ps;
//     HDC hdc = BeginPaint(hWnd, &ps);

//     GetClientRect(hWnd, &rect);
//     SetTextColor(hdc, RGB(0,0,0));
//     SetBkMode(hdc, TRANSPARENT);
//     rect.left = 0;
//     rect.top = 0;
//     DrawText(hdc, message, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
//     EndPaint(hWnd, &ps);
// }
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc={};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = (HICON)LoadImage(NULL,TEXT(".\\src\\img\\logo.ico"),IMAGE_ICON,0,0,LR_LOADFROMFILE);
    if(wc.hIcon==NULL){
        printf("Load Image Error: %x\n",GetLastError());
    }
    wc.hIconSm = wc.hIcon;

    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0,255,255));
    //TODO:
    wc.lpszMenuName = NULL;
    wc.lpszClassName = parentClassName;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("myShell"),
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        parentClassName,
        TEXT("myShell"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);


    ShowWindow(hwnd, nCmdShow);
    HFONT staticFont = CreateFont (TEXT_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    std::string staticContent="myShell>";
    staticRect = getStringBorder(staticContent,NULL); 
    hwndStatic = CreateWindow("Edit",
                        staticContent.c_str(),  
                        WS_OVERLAPPED|WS_CHILD | WS_VISIBLE |ES_READONLY|ES_MULTILINE,
                        staticRect.left,staticRect.top,staticRect.right, staticRect.bottom,
                        hwnd,
                        0,
                        hInstance,NULL);
    editRect={staticRect.right,staticRect.top,1000, staticRect.bottom};
    hwndEdit = CreateWindow("Edit",
                        TEXT("ohhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"),  
                        WS_OVERLAPPED|WS_CHILD | WS_VISIBLE|ES_AUTOHSCROLL|WS_HSCROLL,
                        // LPtoDP((HDC)hwnd,staticSize.cx),LPtoDP((HDC)hwnd,staticSize.cy)-TEXT_FONT_HEIGHT,1000, 1000,
                        staticRect.right,staticRect.top,1000, staticRect.bottom,
                        hwnd,
                        0,
                        hInstance,NULL);
    ShowScrollBar(hwndEdit,SB_BOTH,FALSE);
    WPA = (WNDPROC)SetWindowLongPtr(hwndEdit,GWLP_WNDPROC, (LONG_PTR)Edit_Prc);

    printf("????\n");
    


    
    bool isUnderlined=false;
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        isUnderlined=!isUnderlined;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}