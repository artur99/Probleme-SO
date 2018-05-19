/* Cerință:
4. Scrieti un program C care va crea 2 procese fii si va citi de la tastatura o secventa de numere terminata cu
CTRL+D. Numerele pozitive vor fi trimise primului fiu, iar numerele negative vor fi trimise celui de-al doilea
fiu. Fiecare fiu va calcula suma numerelor primite si va afisa in final identitatea lui si secventa de numere
primita, iar suma secventei o va trimite parintelui. Procesul initial va afisa la final cele doua sume primite
ce la fii si suma celor doua sume. */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

// Pipe 1: Parent -> Child1
#define RD_FR_PARENT_P1 0   // Read from parent (in child 1)
#define WR_TO_P1        1
// Pipe 1: Parent -> Child2
#define RD_FR_PARENT_P2 2
#define WR_TO_P2        3
// Pipe 1: Child1 -> Parent
#define RD_FR_P1        4
#define WR_TO_PARENT_P1 5
// Pipe 1: Child2 -> Parent
#define RD_FR_P2        6
#define WR_TO_PARENT_P2 7

int main(){
    int proc[2] = {-2, -2};
    int pipes[8], pipeS, i;
    
    for(i = 0; i < 8; i+=2){
        if(pipe(&pipes[i]) == -1){
            printf("Error creating Pipe %d", i/2+1);
            exit(1);
        }
    }
    
    
    
    proc[0] = fork();
    if(proc[0] == -1){
        printf("Error creating Child 1");
        exit(3);
    }
    
    if(proc[0] > 0) proc[1] = fork();
    if(proc[1] == -1){
        printf("Error creating Child 1");
        exit(3);
    }
    
    if(proc[0] != 0 && proc[1] != 0){
        // Parent
        int nr, sum1, sum2;
        
        for(i = 0; i < 8; i++){
            if(i == WR_TO_P1 || i == WR_TO_P2 || i == RD_FR_P1 || i == RD_FR_P2) continue;
            close(pipes[i]);
        }
        
        printf("Type some numbers to sum, then press Ctrl+D\n");
        
        while(1){
            if(scanf("%d", &nr) == EOF) break;
            if(nr > 0){
                write(pipes[WR_TO_P1], &nr, 4);
            }else{
                write(pipes[WR_TO_P2], &nr, 4);
            }
        }
        close(pipes[WR_TO_P1]);
        close(pipes[WR_TO_P2]);
        
        wait(NULL);
        wait(NULL);
        
        read(pipes[RD_FR_P1], &sum1, 4);
        read(pipes[RD_FR_P2], &sum2, 4);
        
        printf("Recieved sum: %d from Child 1\n", sum1);
        printf("Recieved sum: %d from Child 2\n", sum2);
        printf("Final sum: %d\n", sum1+sum2);
        
    }else if(proc[1] == 0){
        // Child 1
        int nr, sum = 0;
        
        for(i = 0; i < 8; i++){
            if(i == RD_FR_PARENT_P1 || i == WR_TO_PARENT_P1) continue;
            close(pipes[i]);
        }
        
        while(read(pipes[RD_FR_PARENT_P1], &nr, 4)){
            sum += nr;
        }
        close(pipes[RD_FR_PARENT_P1]);
        
        write(pipes[WR_TO_PARENT_P1], &sum, 4);
        close(pipes[WR_TO_PARENT_P1]);
        
    }else if(proc[0] == 0){
        // Child 2
        int nr, sum = 0;
        
        for(i = 0; i < 8; i++){
            if(i == RD_FR_PARENT_P2 || i == WR_TO_PARENT_P2) continue;
            close(pipes[i]);
        }
        
        while(read(pipes[RD_FR_PARENT_P2], &nr, 4)){
            sum += nr;
        }
        close(pipes[RD_FR_PARENT_P2]);
        
        write(pipes[WR_TO_PARENT_P2], &sum, 4);
        close(pipes[WR_TO_PARENT_P2]);
        
        return 0;
    }
}
