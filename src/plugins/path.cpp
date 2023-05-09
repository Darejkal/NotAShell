#include <iostream>
#include "../lib/shellSetting.h"
#include <windows.h>
#include <vector>
#include <string>
void printHelp(){
    std::wcout<<"path.exe\nINPUT: path.exe\n add [SPACE SEPERATED PARAM(S)]\n del [SPACE SEPERATED PARAM(S)]\n cur\nOUTPUT: Modify user's PATH or Show current PATH config";
}

int wmain(int argc, wchar_t* argv[]){
    //No argument options:
    if(argc==1){
        printHelp();
        return 0;
    }
    std::wstring action(argv[1]);
    bool (*func)(LPCWSTR,std::wstring)=NULL;
    if(!action.compare(L"cur")){
        WCHAR temp[ms::settingBuff];
        ms::readConfig(L"PATH_CONFIG",temp);
        std::wstring t(temp);
        std::wcout<<L"(BEGIN OF CONFIG)\n"<<t<<L"\n(END OF CONFIG)\n";
        return 0;
    }
    //Other options:
    if(argc==2){
        printHelp();
        return 0;
    }
    if(!action.compare(L"add")){
        func=ms::addConfig;
    } else if(!action.compare(L"del")){
        func=ms::removeConfig;
    } 
    for(int i=2;i<argc;i++){
        std::wstring temp(argv[i]);
        std::wcout<<temp<<L"\n";
        (*func)(L"PATH_CONFIG",temp);
    }
    return 0;
}
