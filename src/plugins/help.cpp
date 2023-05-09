#include <iostream>
void printHelp(){
    std::wcout<<"Usable default commands: help, echo, path";
}
int wmain(int argc, wchar_t* argv[]){
    printHelp();
    return 0;
}
