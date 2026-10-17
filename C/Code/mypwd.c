#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

ino_t get_inode(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror("stat failed");
        exit(EXIT_FAILURE);
    }
    return statbuf.st_ino;
}

int main(void){
    int i = 0;
    char names[20][200];
    while(get_inode(".") != get_inode("..") ){
    ino_t current = get_inode(".");
    chdir("..");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
    if(entry -> d_ino == current){
    strcpy(names[i],entry->d_name);
    i ++;
    break;
    }
    }
    closedir(dir);
    }
    if(i == 0)
        printf("/");
    else{
        while(i > 0){
            printf("/%s",names[i - 1]);
            i--;
    }
    }
}