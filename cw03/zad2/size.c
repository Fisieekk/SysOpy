#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>


int main(){
    long long whole_size=0;
    printf("%lld|n", whole_size);
    long size;
    struct dirent *dir;
    DIR *d = opendir(".");
    struct stat buf;
    if (d){
        while ((dir = readdir(d)) != NULL){
            if (stat(dir->d_name, &buf) != -1){
                if (!S_ISDIR(buf.st_mode)){
                    size = buf.st_size;
                    printf("%ld %s\n", size, dir->d_name);
                    whole_size += size;
                }
            }
        }
        closedir(d);
    }
    printf("Whole Size: %lld\n", whole_size);
    return 0;
}