#include<windows.h>
#include<iostream>
// #include<string>
#include<vector>
void printHelp(){
    std::wcout<<L"date\nINPUT: date [PATH]\nOUTPUT: Print system current date";
}
int wmain(int argc, wchar_t* argv[]){
    if(argc>1){
        printHelp();
        return 0;
    }
    SYSTEMTIME time;
    GetSystemTime(&time);
    std::wcout<<(L"Current date: ")
    <<(std::to_wstring(time.wDay))<<(L"/")
    <<(std::to_wstring(time.wMonth))<<(L"/")
    <<(std::to_wstring(time.wYear));
    return 1;

}