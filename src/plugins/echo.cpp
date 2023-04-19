#include <iostream>
int main(int argc, char* argv[]){
    for(int i=0;i<argc-1;i++){
        std::wcout<<argv[i];
    }
    std::wcout<<argv[argc-1];
}
