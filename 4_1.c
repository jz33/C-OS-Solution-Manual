#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef unsigned int uint;
typedef unsigned int unit;

/*
Operating System Concepts Edition 9th Project 4.1
Test on Macintosh 10.8.4
Auther: junzhengrice@gmail.com
*/

#define DIM 9 
#define BLK 3
// uint ARR[DIM]= {1,2,3,4,5,6,7,8,9};
static uint SAM[DIM][DIM] = {
    {1,2,3,4,5,6,7,8,9},
    {4,5,6,7,8,9,1,2,3},
    {7,8,9,1,2,3,4,5,6},
    {2,1,4,3,6,5,8,9,7},
    {3,6,5,8,9,7,2,1,4},
    {8,9,7,2,1,4,3,6,5},
    {5,3,1,6,4,2,9,7,8},
    {6,4,2,9,7,8,5,3,1},
    {9,7,8,5,3,1,6,4,2}
};

uint* initArr(void);
uint** initSud(void);
void printArr(uint*);
void printSud(uint**);
void printBlk(uint**, int, int);

void prop_inr(uint**, int, int, int);
void prop_blk(uint**, int, int, int);
void prop_dig(uint**, int, int);
void prop_rol(uint**);

int searchRow(uint**, int, uint);

int main(int argc, char* argv[]){
    uint* arr= initArr();
      
    uint** sam = initSud();printSud(sam); 
    
    prop_rol(sam);printSud(sam);
    prop_inr(sam,0,3,4);printSud(sam);
    prop_blk(sam,0,1,2);printSud(sam);
    prop_dig(sam,2,3);printSud(sam);
    
return 0;
}

//Init 1 - 9 array
uint* initArr(void){
    uint* arr = (uint*)malloc(sizeof(uint)*DIM);
    for(int i=0;i<DIM;i++) arr[i] = i+1;
    return arr;
}

//Init sudoku as SAM
uint** initSud(void){
    uint** sud = (unit**)malloc(sizeof(unit*)*DIM);
    for(int i=0;i<DIM;i++) sud[i] = (uint*)malloc(sizeof(uint)*DIM);
    for(int i=0;i<DIM;i++)
        for(int j=0;j<DIM;j++)
            sud[i][j]=SAM[i][j];
    return sud;
}

void printArr(uint* arr){
    for(int i=0;i<DIM;i++) printf("%u ",arr[i]);
    printf("\n\n");
}

void printSud(uint** sud){
    for(int i=0;i<DIM;i++){
        for(int j=0;j<DIM;j++) printf("%u ",sud[i][j]);
        printf("\n");
    }
    printf("\n");
}

void printBlk(uint** sud, int row, int col){
    if(row % 3 !=0 || col % 3 !=0 ||
       row<0 || col <0 || row>6 || col >6){
          printf("row & col must be in {0,3,6}\n");
          return;
    }
    for(int i=row;i<BLK+row;i++){
        for(int j=col;j<BLK+col;j++) printf("%u ",sud[i][j]);
        printf("\n");
    }
    printf("\n");
}

//Switch cols/rows inside block col/row 
void prop_inr(uint** ori, int flag, int x, int y){
    if(flag != 0 && flag != 1){
        printf("flag must be either 1(row) or 0 (col)\n");
        return;
    }
    if(x/BLK != y/BLK){
        printf("x & y must be in same block row/col\n");
        return;
    }
    if(x<0 || x>DIM-1 || y<0 || y>DIM-1){
        printf("x & y must be in [0-8]\n");
        return;
    }
    if(x == y){
        printf("x & y must be different\n");
        return;
    }
    
    uint temp = 0;
    if(flag == 0){ //col
       for(int i=0;i<DIM;i++){
           temp = ori[i][x];
           ori[i][x] = ori[i][y];
           ori[i][y] = temp;
       }
    }
    else if(flag ==1){ //row
       for(int i=0;i<DIM;i++){
           temp = ori[x][i];
           ori[x][i] = ori[y][i];
           ori[y][i] = temp;
       }
    }
}

//Switch blocks at row/col
void prop_blk(uint** ori, int flag, int x, int y){
    if(flag != 0 && flag != 1){
        printf("flag must be either 1(row) or 0 (col)\n");
        return;
    }
    if(x != 0 && x != 1 && x !=2){
        printf("x must be in [0,1,2]\n");
        return;
    }
    if(y != 0 && y != 1 && y !=2){
        printf("y must be in [0,1,2]\n");
        return;
    }
    if(x == y){
        printf("x & y must be different\n");
        return;
    }
    
    uint temp = 0;
    int fr = 0;
    int to = 0;
    if(flag == 0){ //col
       for(int i=0;i<BLK;i++)
           for(int j=0;j<DIM;j++){
              fr = x*BLK+i;
              to = y*BLK+i;
              temp = ori[j][fr];
              ori[j][fr] = ori[j][to];
              ori[j][to] = temp;
           }
    }
    else if(flag ==1){ //row
       for(int i=0;i<BLK;i++)
           for(int j=0;j<DIM;j++){
              fr = x*BLK+i;
              to = y*BLK+i;
              temp = ori[fr][j];
              ori[fr][j] = ori[to][j];
              ori[to][j] = temp;
           }
    }
}

//Switch 2 digits 9 times. There is no difference via col or row
void prop_dig(uint** ori, int x, int y){
    if(x<0 || x>DIM-1 || y<0 || y>DIM-1){
        printf("x & y must be in [0-8]\n");
        return;
    }
    if(x == y){
        printf("x & y must be different\n");
        return;
    }
    
    int ind_x =0;
    int ind_y =0;
    for(int i=0;i<DIM;i++){
        ind_x = searchRow(ori,i,x);
        ind_y = searchRow(ori,i,y);
        ori[i][ind_x]=y;
        ori[i][ind_y]=x;
    }
}

//Roll the sudoku by 90 degree
void prop_rol(uint** ori){
    uint** sud = (unit**)malloc(sizeof(unit*)*DIM);
    for(int i=0;i<DIM;i++) sud[i] = (uint*)malloc(sizeof(uint)*DIM);
    for(int i=0;i<DIM;i++)
        for(int j=0;j<DIM;j++)
            sud[i][j] = ori[DIM-1-j][i];
    for(int i=0;i<DIM;i++)
        for(int j=0;j<DIM;j++)
            ori[i][j] = sud[i][j];
    for(int i=0;i<DIM;i++) free(sud[i]);
    free(sud); 
}

//Search e's index of a row, return index
int searchRow(uint** sud, int row, uint e){
    for(int i=0;i<DIM;i++)
        if(sud[row][i]==e) return i;
    return -1;
}