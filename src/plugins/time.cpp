#include<windows.h>
#include<iostream>
// #include<string>
#include<vector>
void printHelp(){
    std::wcout<<L"time\nINPUT: time [PATH]\nOUTPUT: Print system current time";
}
int wmain(int argc, wchar_t* argv[]){
    if(argc>1){
        printHelp();
        return 0;
    }
    SYSTEMTIME time;
    GetSystemTime(&time);
    std::wcout<<(L"Current time: ")
    <<(std::to_wstring(time.wHour))<<(L":")
    <<(std::to_wstring(time.wMinute))<<(L":")
    <<(std::to_wstring(time.wMilliseconds))<<(L":")
    <<(std::to_wstring(time.wSecond))<<(L" ");
    return 1;

}