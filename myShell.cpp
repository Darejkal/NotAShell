#include "./src/lib/shellWindow.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    ms::initPathConfig();
    return ms::createWindow(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
}