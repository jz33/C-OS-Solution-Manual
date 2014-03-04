#include <iostream>
#include <memory>
#include <queue>
using namespace std;

/*
Operating System Concepts Edition 9th Chapter 7 Project 1
Case source: http://www.cs.jhu.edu/~yairamir/cs418/os4/sld026.htm
Compile: clang++ -std=c++11 -stdlib=libc++ 7_1.cpp
Author: jz33, junzhengrice@gmail.com
*/

#define N 5 //# of processes
#define M 3 //# of resources

int main(int argc, char* argv[])
{
	bool exec = 0;//If current process can allocate needed resources
	bool safe = 0;//If current state is safe
	int i,j,counter=0;//Iterators
	queue<int> sequ; //Sequence of valid execution
	unique_ptr<int[]> fnsh(new int[N]); //If process i finished
	unique_ptr<int[]> avai(new int[M]); //Current resource available
	unique_ptr<int[][M]> maxi(new int[N][M]); //Maximum a process requires
	unique_ptr<int[][M]> allc(new int[N][M]); //Current resources allocated
	unique_ptr<int[][M]> need(new int[N][M]); //Current resources needed
	
	for(i=0;i<N;i++) fnsh[i] = 0;
	
	avai[0] = 3; avai[1] = 3; avai[2] = 2;
	
	allc[0][0] = 0; allc[0][1] = 1; allc[0][2] = 0;
	allc[1][0] = 2; allc[1][1] = 0; allc[1][2] = 0;
	allc[2][0] = 3; allc[2][1] = 0; allc[2][2] = 2;
	allc[3][0] = 2; allc[3][1] = 1; allc[3][2] = 1;
	allc[4][0] = 0; allc[4][1] = 0; allc[4][2] = 2;
	
	maxi[0][0] = 7; maxi[0][1] = 5; maxi[0][2] = 3;
	maxi[1][0] = 3; maxi[1][1] = 2; maxi[1][2] = 2;
	maxi[2][0] = 9; maxi[2][1] = 0; maxi[2][2] = 2;
	maxi[3][0] = 2; maxi[3][1] = 2; maxi[3][2] = 2;
	maxi[4][0] = 4; maxi[4][1] = 3; maxi[4][2] = 3;
	
	for(i=0;i<N;i++)
		for(j=0;j<M;j++)
			need[i][j] = maxi[i][j]-allc[i][j];
	
    while(counter < N){
	     safe = 0;
	     for(i=0;i<N;i++){
		     if(fnsh[i] == 1) continue;
	         exec = 1;
			 for(j=0;j<M;j++){
			     if(need[i][j]>avai[j]){
				     exec = 0;
				     break;
				 }
			 }
			 if(exec){
			     safe = 1;
			     fnsh[i] = 1;
				 counter++;
			     sequ.push(i);
				 
				 cout<<"At i="<<i<<", avai is: "<<endl;
				 for(j=0;j<M;j++) {
				     avai[j] += allc[i][j];
					 cout<<avai[j]<<" ";
				 }
				 cout<<endl;
				 break;
			 }
	     }
		 if(!safe) break;
	}
	
	if(!safe) cout<<"Unsafe"<<endl;
	else cout<<"Safe"<<endl;
	
    while(!sequ.empty()){
    	cout<<sequ.front()<<" ";
		sequ.pop();
	}
    return 0;
}