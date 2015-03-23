using namespace std;
#include "graph.h"
#include<iostream>
#include <fstream>      
#include<cstdio>
#include <unordered_map>
#include <utility>


//KSIZE IS STORED IN GLOBAL VARIABLE
int ksize = 5;
std::unordered_map<int, int> test;

//AN UNORDERED MAP IS USED TO CREATE OUR HASH TABLE
//ITS KEY VALUE IS A K-MER STRING
//THE ASSOCIATED VALUE IS A NODE POINTER
typedef unordered_map<string,node*> nodemap;

//PAIR THAT IS USED FOR INSERTION IN HASH
typedef pair<string, node*> insertionpair;


//HELPER FUNCTIONS
void add(string x, nodemap* table);
void updatemap(string bef, string y, string aft, nodemap* table);
void ynotinmap(string bef, string y, string aft, nodemap* table);
void yinmap(string bef, string y, string aft, nodemap* table);

node* beforenode(string bef, nodemap* table);
node* afternode(string aft, nodemap* table);

void cleantableFIX(char *argv[], nodemap* table);

void markbranchnodes(nodemap* table);
void findcontigs(char *argv[], nodemap* table2);

void startnodes(char *argv[], nodemap* table2, std::ofstream& file2, std::ofstream& fileX);
void branchchildren(char* argv[], nodemap* table2, std::ofstream& file2, std::ofstream& fileX);
void walkit(char *argv[], node* path, std::ofstream& file2, std::ofstream& fileX);

void lengthoutput(char *argv[], std::ifstream& contigs);

//NODE CONSTRUCTOR
node::node()
{
        kmer = "";
        marker = false;
        branch = false;
        reads = 0;
        
}

//MAIN()::
int main( int argc, char *argv[] ){
  string x;
  nodemap* table = new nodemap;


     ifstream file(argv[1]);
     //CREATE INITIAL MAP OF CONTIGS
     while ( ! file.eof() ){  
     getline(file, x);
     add(x, table);
  }
    //PERFORM ERROR DETECTION HERE
    //OUTPUTS TO GOOD_READS
    cleantableFIX(argv, table);
    
    nodemap* table2 = new nodemap;    
    file.close();

    ifstream file2( argv[2] ) ;
     //RECONSTRUCTS A SECOND MAP WITH GOOD_READS
     while ( ! file2.eof() ){
        getline(file2, x);
        add(x, table2);
  }
      //HERE WE FIND AND MARK BRANCHED NODES
      markbranchnodes(table2);
      //THEN WE FIND ALL VALID CONTIGS AND THEIR LENGTHS
      findcontigs(argv, table2);

  return 0;
}
//ADD():
//ADDS A GIVEN STRING TO OUR NODEMAP TABLE
//IT CREATES THREE K-SIZE STRINGS AND USES THEM TO UPDATE
//THE NODEMAP TABLE WITH UPDATEMAP().  ONE STRING, Y, IS THE PRINCIPLE
//NODE BEING INSERTED, WITH BEF AND AFT AS THE PRECEDING AND
//FOLLOWING NODES.  
void add(string x, nodemap* table){
  int length = x.length();
  string y, bef, aft = "";
  for(int i = 0; i < (length - (ksize-1)); i++){
        bef = y;
	aft ="";
        y = "";
      for (int j = 0; j < ksize ; j++){
	y = y + x.at(j + i);
	if ( i < (length - ksize))
	        aft = aft + x.at(j + i+ 1);
        }
     updatemap(bef, y, aft, table);
}
}
//UPDATEMATE():
//UPDATES OUR MAP OF KMER NODES
void updatemap(string bef, string y, string aft, nodemap* table){

      //CASE 1: y is not in map
      if(!table->count(y)){     
         ynotinmap(bef, y, aft, table);
      }
      //CASE 2: y is already in map
      else{
        yinmap(bef, y, aft, table);
      }
}
//YNOTINMAP();
//IF OUR CURRENT NODE, Y, IS NOT ALREADY IN THE HASH TABLE
//IT IS INSERTED USING THIS METHOD
void ynotinmap(string bef, string y, string aft, nodemap* table){

     insertionpair t;           
     node* ynd = new node;
     node* bnd = NULL;
     node* aftnd= NULL;
     ynd->kmer = y;
     ynd->reads = 1;
     bool prevB = false;
     bool prevA = false;
     
        //make pair for middle node and insert
        t = make_pair(y, ynd);
        table->insert(t);
        //retrieve pointer for incoming node
        if ( bef != "" ){
                bnd = beforenode(bef, table);
       }
       //retrieve pointer for outgoing node
        if (aft != "")
           aftnd = afternode(aft, table);
        //RETRIEVES KEY, VALUE PAIR FROM TABLE FOR KEY Y
       nodemap::const_iterator got = table->find(y);
        
       vector<node*>::iterator bit = got->second->incoming.begin();
       
       if( bnd != NULL){
                //CHECK FOR DUPLICATE INCOMING NODE*S TO AVOID REPEATES
                for (int i = 0; i != got->second->incoming.size(); i++){
                               if (bef == got->second->incoming.at(i)->kmer)
                                        prevB= true;
                                }                                
                if (prevB == false)
                //INSERT NODE* INTO INCOMING VECTOR 
                bit = got->second->incoming.insert( bit, bnd);
        }

       vector<node*>::iterator ait = got->second->outgoing.begin();
       if (aftnd !=NULL){
               //CHECK FOR DUPLICATE OUTGOING NODE*S TO AVOID REPEATES
                for (int i = 0; i != got->second->outgoing.size(); i++){
                               if (aft == got->second->outgoing.at(i)->kmer)
                                        prevA= true;
                                } 
                if (prevA == false)
                 //INSERT NODE* INTO OUTGOING VECTOR 
                 ait = got->second->outgoing.insert( ait, aftnd);       
        }
}
//YINMAP();
//IF OUR CURRENT NODE, Y, IS ALREADY IN THE HASH TABLE
//IT IS INSERTED USING THIS METHOD
void yinmap(string bef, string y, string aft, nodemap* table){

     node* bnd = NULL;
     node* aftnd= NULL;
     insertionpair t;   
     bool prevA, prevB = false;

        nodemap::const_iterator got = table->find(y);
        got->second->reads++;     

        if ( bef != "" ){
                bnd = beforenode(bef, table);
       }

       if (aft != "")
           aftnd = afternode(aft, table);
 
       vector<node*>::iterator bit = got->second->incoming.begin();
       if( bnd != NULL){
                for (int i = 0; i != got->second->incoming.size(); i++){
                               if (bef == got->second->incoming.at(i)->kmer)
                                        prevB = true;
                                }                                
                if (prevB == false)
                bit = got->second->incoming.insert( bit, bnd);
        }


       vector<node*>::iterator ait = got->second->outgoing.begin();
       if (aftnd !=NULL){
                for (int i = 0; i != got->second->outgoing.size(); i++){
                               if (aft == got->second->outgoing.at(i)->kmer)
                                        prevA = true;
                                } 
                if (prevA == false)
                 ait = got->second->outgoing.insert( ait, aftnd);       
        }

      

}
//BEFORENODE():
//RETRIEVES THE INCOMING NODE POINTER 
//SHOULD ALWAYS EXIST ALREADY IN TABLE.
node* beforenode(string bef, nodemap* table){
               if (!table->count(bef)){
                        exit;
                }
               return table->at(bef);
}


//AFTERNODE():
//RETRIEVES THE OUTGOING NODE POINTER
node* afternode(string aft, nodemap* table){
        insertionpair t;

       if ( table->count(aft) )
                //aft node already in table
                return table->at(aft) ;

       if(!table->count(aft) ){
                node* aftnd = new node;
                aftnd->kmer = aft;
                //make pair for after node and insert
                insertionpair t;
                t = make_pair(aft, aftnd);
                table->insert(t);         
                return table->at(aft);
       }

}
//CLEANTABLEFIX():
//WE ITERATE THROUGH OUR CURRENT TABLE AND 
//OUTPUT ALL STRINGS THAT HAVE BEEN READ MORE THAN ONCE
//TO THE SECOND COMMAND LINE ARGUMENT GOOD_READS
void cleantableFIX(char *argv[], nodemap* table){

        string current_string, y = "";
        nodemap::const_iterator got;
        bool rottenword = false;        
        ifstream file(argv[1]);
        ofstream file2(argv[2]);

        //WE AGAIN READ THROUGH OUR SEQUENCE READS
        while ( ! file.eof() ){   
            getline(file, current_string);
                rottenword = false;
                int length = current_string.length();

                  //WE AGAIN LOOK AT ALL KMERS BUT USE OUR BOOL 
                  // 'ROTTEN WORDS' TO DETERMINE IF A GIVEN SUBSEQUENCE
                  //  OF A STRING OCCURS ONLY ONCE
                  for(int i = 0; i < (length - (ksize-1)); i++){
                        if (rottenword == true)
                                break;
                        y = "";
                      for (int j = 0; j < ksize ; j++){
                                y = y + current_string.at(j + i);
                        }
                        //RETRIEVE THE CORRESPOND NODE* FROM OUR TABLE
                        got = table->find(y);
                        if (got->second->reads == 1){
                                 rottenword = true;
                                }
                }
        //IF THE CURRENT STRING HAS NO UNIQUE SUBSEQUENCES
        //WE OUTPUT TO THE SECOND COMMAN LINE ARGUMENT GOOD_READS
        if (rottenword == false){
        file2<<current_string<<endl;
        }


     }

}

//MARKBRANCHNODES():
//READ THROUGH OUR NODEMAP TABLE AND MARK ALL
//NODES THAT BRANCH OUT
void markbranchnodes(nodemap* table){

        for (nodemap::iterator it = table->begin(); it!= table->end(); ++it ){
                  if ( (it->second->incoming.size() > 1) || (it->second->outgoing.size() > 1)  ){
     
                        it->second->branch = true;
                }
        }
}

//FINDCONTIGS():
//WE PERFORM THE WALKING ALGO ON ALL NODES NO INCOMING NODES
//THEN ON THOSE NODES COMING FROM BRANCHED NODES
//WE THEN OUTPUT THE CONTIG LENGTHS
void findcontigs(char *argv[], nodemap* table2){

        ofstream file2(argv[3]);
        
        ofstream fileX(argv[5]);
        
        ifstream file3(argv[3]);
        
        startnodes(argv, table2, file2, fileX);
        branchchildren(argv, table2, file2, fileX);
        //notice that this function takes the file 
        //previously used to store output and uses it
        //as an input stream to determine lengths
        lengthoutput(argv, file3);
        

}
//STARTNODES():
//first we trace paths from nodes with indegrees of '0'
void startnodes(char *argv[], nodemap* table, std::ofstream& file2, std::ofstream& fileX){

        for (nodemap::iterator it = table->begin(); it!= table->end(); ++it ){
                  if (it->second->incoming.size() == 0 ) 
                        walkit(argv, it->second, file2, fileX);
        }

}
//BRANCHCHILDREN():
//WE THEN PERFORM THE WALKING ALGO ON THOSE UNMARKED NODES WHOSE
//SINGLE INCOMING NODE IS A BRANCH
void branchchildren(char* argv[], nodemap* table, std::ofstream& file2, std::ofstream& fileX){
        
        
        for (nodemap::iterator it = table->begin(); it!=table->end(); ++it){              
                if ( ( it->second->marker == false) && (it->second->incoming.size() == 1) ){
                        if ( (it->second->incoming.front())->branch == true)
                                if (it->second != NULL){  
                                walkit(argv, it->second, file2, fileX);                                       
                                }
                     }
        }                                                                     
                        
}

//WALKIT()
//PERFORMS THE WALKING ALGORITHM DESCRIBED IN ASSIGNMENT
void walkit(char *argv[], node* walker, std::ofstream& file2, std::ofstream& fileX){
        
        if (walker->marker == true)
                return;
        
        string y = walker->kmer;
        walker->marker = true;
        
        //CORNER CASE: ONE NODE AND DONE: NO OUTGOING NODES
        if (walker->outgoing.size() == 0){
                if ( y.length() > 100)
                file2<<y<<endl; 
                return;                
        }        
        //WALK THE DOG BABY UNTIL HITTING A MARKED NODE OR BRANCH
        while( (walker->branch == false )  ){
                walker = walker->outgoing.front();
                   if (walker->marker==true)
                        return;
                //AUGMENTS STRING OF OUR PATH
                y = y + walker->kmer.at(ksize - 1);
                walker->marker = true;
                
                  if (walker->outgoing.size() == 0)
                        break;
        }
        //OUTPUTS THE CONSTRUCTED STRING TO OUTPUT_CONTIGS
        if ( y.length() > 1)
                file2<<y<<endl;
        
        fileX<<y<<endl;
        return;        
}

//LENGTHOUTPUT()
//OUTPUTS THE LENGTH OF THE ALL THE OUTPUT_CONTIGS
//TO THE FILE CONTIG_LENGTHS
void lengthoutput(char *argv[], std::ifstream& contigs){
        
        string current="";
        //WE'LL USE THIS FILE TO OUTPUT THE LENGTHS
        ofstream contig_lengths(argv[4]);
        contigs >> current;        
        if (current=="")
              return;
        int chaff = 0;
        int con= 0;
        
        
        while(!contigs.eof()){
                contig_lengths<<current.length()<<endl;
                contigs >> current;
        }
        
        return;
        
        
}
