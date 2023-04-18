#include <stdio.h>
int main(int argc, char* argv[]){
    for(int i=0;i<argc-1;i++){
        printf("%s\n",argv[i]);
    }
    printf("%s",argv[argc-1]);
}
