#include "heuristic.h"
#include "data_struct.h"
#include "bfs.h"

#include <fstream>
#include <ctime>

void add_to_expansion_vector(bfs::node* n,vector<bfs::node*>& expansion_order){
  if(n->iteration>=0){
    if(expansion_order.size()<n->iteration+1)
      expansion_order.resize(n->iteration+1);
    expansion_order[n->iteration]=n;
  }
  for(int j=0;j<n->Nchildren;j++)
    add_to_expansion_vector(n->child[j],expansion_order);
}

void write_eye_trace(bfs::node* tree,ostream& out){
  vector<bfs::node*> expansion_order;
  vector<int> trace;
  add_to_expansion_vector(tree,expansion_order);
  string s;
  out<<uint64tobinstring(tree->b.pieces[BLACK])<<"\t"<<uint64tobinstring(tree->b.pieces[WHITE])<<endl;
  for(int i=0;i<expansion_order.size();i++){
    trace.clear();
    for(bfs::node* n=expansion_order[i];n->parent!=NULL;n=n->parent)
      trace.insert(trace.begin(),uint64totile(n->m));
    for(unsigned int j=0;j<trace.size();j++)
      out<<trace[j]<<"\t";
    out<<endl;
  }
}
