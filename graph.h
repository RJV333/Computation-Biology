#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

//REPRESENTS A NODE OF OUR DEBRUIJN GRAPH
//CONTAINS ALL PERAMETERS SUGGESTED IN ASSIGNMENT

struct node{
  node();
  //WHAT IS THE KMER STRING OF THIS NODE?
  string kmer;
  //HAS NODE BEEN MARKED?
  bool marker;
  //DOES NODE BRANCH?
  bool branch;
  //HOW MANY TIMES HAS THIS NODE BEEN READ?
  int reads;
  //VECTORS ARE USED TO STORE COLLECTION OF INCOMING
  // AND OUTGOING NODES
  std::vector<node*> incoming;
  std::vector<node*> outgoing;

};
