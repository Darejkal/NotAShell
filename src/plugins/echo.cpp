#include <iostream>
int main(int argc, char* argv[]){
    for(int i=1;i<argc-1;i++){
        std::wcout<<argv[i]<<" ";
    }
    std::wcout<<argv[argc-1];
    return 0;
}
