using namespace std;
#include "graph.h"
#include<iostream>
#include <fstream>
#include <string>
#include<cstdio>
#include <utility>
#include <algorithm>
/*
THIS AUXILLARY PROGRAM COMPUTES THE N50 VALUE OF AN
ASSEMBLY.  IT TAKES IN A FILE ON THE COMMAND LINE ARGUMENT
THAT IS THE LIST OF ALL CONTIG LENGTHS GREATER THAN 100BP.
IT FIRST DETERMINES THE SUM OF ALL CONTIG LENGTHS. IT THEN
MAKES AN ARRAY, WHERE EACH CONTIG LENGTH IS COPIED INTO IT
THE SAME NUMBER OF TIMES AS ITS VALUE.  THE MEDIAN OF THIS
ARRAY IS THEN RETURNED SINCE IT REPRESENT THE N50 VALUE
*/


int main( int argc, char *argv[] ){

  string current = "";
  int i, addition,place = 0;  
  int sum = 0;
  
  ifstream file(argv[1]);
  
  file >> current;
  
  while (!file.eof()){
          addition = stoi( current );
          sum = sum + addition ;
          i++;    
          file >> current;
  }
          addition = stoi( current );
          sum = sum + addition ;
  cout<<endl<<sum;

  int array[sum];
  
  ifstream file2(argv[1]);  
  
  
  while (!file2.eof()){
          file2 >> current;
          addition = stoi( current );
          for(int j = 0; j < addition ; j++){
                  
                  array[j+ place] = addition;                 
          }
          place = place + addition;           
  }
  cout<<endl;
  int elements = sizeof(array) / sizeof(array[0]); 
  std::sort(array, array + elements);


  if (elements%2 != 0){
          cout<<"MEDIAN OF SORTED ARRAY IS  "<< array[ elements/2 + 1]<<endl;
  }
  else 
          cout<<"MEDIAN OF SORTED ARRAY IS  "<< array[ elements/2 ]<<endl;
  
  
  

  return 0;
}