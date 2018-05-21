#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(){
    int c1, c2, c3, p1 = -1, p2 = -1;
    c1 = mkfifo("/tmp/canal-1", 0600);
    c2 = mkfifo("/tmp/canal-2", 0600);
    c3 = mkfifo("/tmp/canal-3", 0600);

    
    p1 = fork();
    
    if(p1 > 0){
        p2 = fork();
        if(p2 > 0){
            // Parinte
            int f1 = open("date.txt", O_RDONLY);
            int f2 = open("/tmp/canal-1", O_WRONLY);
            int f3 = open("/tmp/canal-3", O_RDONLY);
            
            char x;
            int dist;
            while(read(f1, &x, 1)){
                write(f2, &x, 1);
            }
            close(f1);
            close(f2);
            wait(NULL);
            wait(NULL);
            
            read(f3, &dist, 4);
            
            printf("Distinct chars: %d\n", dist);
            close(f3);
        }else{
            
            // Fiu 1
            int f1 = open("/tmp/canal-1", O_RDONLY);
            int f2 = open("/tmp/canal-2", O_WRONLY);
            char x;
            while(read(f1, &x, 1)){
                if(x >= 'a' && x <= 'z')
                    write(f2, &x, 1);
            }
            close(f1);
            close(f2);
            return 0;
        }
    }else{
        // Fiu 2
        int f1 = open("/tmp/canal-2", O_RDONLY);
        int f2 = open("/tmp/canal-3", O_WRONLY);
        FILE* f3 = fopen("statistica.txt", "w");
        int i, distinct = 0;
        char x;
        char chlist[30];
        
        for(i=0;i<30;i++) chlist[i] = 0;
        
        while(read(f1, &x, 1)){
            if(chlist[x-'a'] == 0) distinct++;
            chlist[x-'a']++;
        }
        for(i=0;i<30;i++){
            if(chlist[i])
                fprintf(f3, "%c: %d\n", 'a'+i, chlist[i]);
        }
        write(f2, &distinct, 4);
        close(f1);
        close(f2);
        fclose(f3);
        return 0;
    }
}
