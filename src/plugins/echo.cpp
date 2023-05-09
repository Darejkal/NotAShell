#include <iostream>
void printHelp(){
    std::wcout<<"echo.exe\nINPUT: echo.exe [SPACE TERMINATED STRING]\nOUTPUT: Output [SPACE TERMINATED STRING] to shell";
}
int wmain(int argc, wchar_t* argv[]){
    if(argc<=1){
        printHelp();
        return 0;
    }
    for(int i=1;i<argc-1;i++){
        std::wcout<<argv[i]<<" ";
    }
    std::wcout<<argv[argc-1];
    return 0;
}
