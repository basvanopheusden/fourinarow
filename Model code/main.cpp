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

int main(int argc, char* argv[]){
  data_struct dat;
  heuristic h;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  h.seed_generator(global_generator);
  zet m;
  char* direc = "C:/Users/Bas/Downloads/splits";
  char* params_filename = "C:/Users/Bas/Google Drive/Bas Games/Analysis/Params/params_peak_final.txt";
  dat.load_data_from_directory(direc,10);
  ofstream output("eye_trace_model.txt",ios::out);
  unsigned int i = 92;
  h.get_params_from_file(params_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
  m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player);
  dat.alltrials[i].b.write(m);
  write_eye_trace(&h.game_tree,output);
  output.close();
}
