#include "board.h"
#include "data_struct.h"

#include <algorithm>
#include <fstream>
using namespace std;

void data_struct::add(board b, uint64 m, bool player, double t, unsigned int p,int g){
  alltrials.emplace_back(b,m,player,t,p,g);
  if(p>=Nplayers)
    Nplayers=p;
  Nboards++;
}

void data_struct::add_2afc(board b, uint64 m, uint64 m1, uint64 m2, bool player, int v1, int v2, double t, unsigned int p,int g){
  alltrials_2afc.emplace_back(b,m,m1,m2,player,v1,v2,t,p,g);
  if(p>=Nplayers)
    Nplayers=p;
  Nboards_2afc++;
}

void data_struct::add_eval(board b, int score, int opt_score, bool player, double t, unsigned int p,int g){
  alltrials_eval.emplace_back(b,score,opt_score,player,t,p,g);
  if(p>=Nplayers)
    Nplayers=p;
  Nboards_eval++;
}

void data_struct::clear(){
  alltrials.clear();
  alltrials_2afc.clear();
  alltrials_eval.clear();
  Nboards=Nboards_2afc=Nboards_eval=0;
  Nplayers=0;
}

void data_struct::load_data_from_directory(char* dir_name,int Nparticipants){
  char filename[128];
  for(int i=0;i<Nparticipants;i++){
    for(int g=1;g<=5;g++){
      sprintf(filename,"%s/%d/%d.csv",dir_name,i+1,g);
      load_board_file(filename,i,g);
    }
  }
}

void data_struct::load_board_file(char* filename,int player_id, int group){
  ifstream input(filename,ios::in);
  uint64 bp,wp,m;
  string s;
  double t;
  int g;
  if(input.is_open()){
    while(!input.eof()){
      input>>bp>>wp;
      input>>s;
      input>>m;
      input>>t;
      input>>g;
      add(board(bp,wp),m,(s=="Black")?BLACK:WHITE,t,player_id,g);
    }
  }
}

void data_struct::save_board_file(char* filename){
  ofstream output(filename,ios::out);
  board b;
  uint64 m;
  bool player;
  for(unsigned int i=0;i<Nboards;i++){
    b=alltrials[i].b;
    m=alltrials[i].m;
    player=alltrials[i].player;
    output<<(player==BLACK?"Black\t":"White\t")<<b.pieces[BLACK]<<"\t"<<b.pieces[WHITE]<<"\t"<<m<<endl;
  }
}

vector<unsigned int> data_struct::select_boards(int subject, int group){
  return select_boards(subject,group,0);
}

vector<unsigned int> data_struct::select_boards(int subject, int group, int type){
  vector<unsigned int> boards;
  if(type==AFC2){
    for(unsigned int i=0;i<Nboards_2afc;i++)
      if((subject==-1 || alltrials_2afc[i].player_id==subject) && (group==0 || alltrials_2afc[i].group==group || (group<0 && alltrials_2afc[i].group!=-group)))
        boards.push_back(i);
  }
  else if(type==EVAL){
    for(unsigned int i=0;i<Nboards_eval;i++)
      if((subject==-1 || alltrials_eval[i].player_id==subject) && (group==0 || alltrials_eval[i].group==group || (group<0 && alltrials_eval[i].group!=-group)))
        boards.push_back(i);
  }
  else for(unsigned int i=0;i<Nboards;i++)
    if((subject==-1 || alltrials[i].player_id==subject) && (group==0 || alltrials[i].group==group || (group<0 && alltrials[i].group!=-group)))
      boards.push_back(i);
  return boards;
}
