#include "board.h"
#include "data_struct.h"

#include <algorithm>
#include <fstream>
using namespace std;

void data_struct::add(board b, uint64 m, bool player, double t=0.0, unsigned int p=0){
  alltrials.emplace_back(b,m,player,t,p);
  if(p>=Nplayers)
    Nplayers=p;
  Nboards++;
}

void data_struct::add_2afc(board b, uint64 m, uint64 m1, uint64 m2, bool player, int v1, int v2, double t=0.0, unsigned int p=0){
  alltrials_2afc.emplace_back(b,m,m1,m2,player,v1,v2,t,p);
  if(p>=Nplayers)
    Nplayers=p;
  Nboards_2afc++;
}

void data_struct::add_eval(board b, int score, int opt_score, bool player, double t=0.0, unsigned int p=0){
  alltrials_eval.emplace_back(b,score,opt_score,player,t,p);
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

void data_struct::load_board_file(char* filename){
  ifstream input(filename,ios::in);
  uint64 pblack,pwhite,m;
  string s;
  while(!input.eof()){
    input>>s;
    input>>pblack>>pwhite;
    input>>m;
    add(board(pblack,pwhite),m,(s=="Black")?BLACK:WHITE);
  }
}

void data_struct::save_board_file(char* filename){
  ofstream output2(filename,ios::out);
  board b;
  uint64 m;
  bool player;
  for(unsigned int i=0;i<Nboards;i++){
    b=alltrials[i].b;
    m=alltrials[i].m;
    player=alltrials[i].player;
    output2<<(player==BLACK?"Black\t":"White\t")<<b.pieces[BLACK]<<"\t"<<b.pieces[WHITE]<<"\t"<<m<<endl;
  }
}

void data_struct::load_board_file_ionatan(char* filename){
  ifstream input(filename,ios::in);
  uint64 pblack,pwhite;
  int m;
  int rt;
  string s="",sold="";
  int pid=-1;
  cout<<input.getline()<<endl;
  while(!input.eof()){
    input>>pblack>>pwhite;
    input>>m;
    input>>rt;
    input>>s;
    if(s!=sold && s!=""){
        pid++;
    }
    sold = s;
    add(board(pblack,pwhite),tiletouint64(35-m),BLACK,rt/1000.0f,pid);
    cout<<pblack<<endl;
  }
}

void data_struct::make_groups(int Ngroups, mt19937_64 engine){
  vector<int> board_by_player[Nplayers];
  unsigned int N,k;
  for(unsigned int j=0;j<Nboards;j++)
    board_by_player[alltrials[j].player_id].push_back(j);
  for(unsigned int p=0;p<Nplayers;p++){
    N=board_by_player[p].size();
    for(unsigned int i=0;i<N;i++){
      k=uniform_int_distribution<unsigned int> {i,N-1}(engine);
      alltrials[board_by_player[p][k]].group=(Ngroups*i)/N+1;
      board_by_player[p][k]=board_by_player[p][i];
    }
  }
  for(unsigned int p=0;p<Nplayers;p++)
    board_by_player[p].clear();
  for(unsigned int j=0;j<Nboards_2afc;j++)
    board_by_player[alltrials_2afc[j].player_id].push_back(j);
  for(unsigned int p=0;p<Nplayers;p++){
    N=board_by_player[p].size();
    for(unsigned int i=0;i<N;i++){
      k=uniform_int_distribution<unsigned int> {i,N-1}(engine);
      alltrials_2afc[board_by_player[p][k]].group=(Ngroups*i)/N+1;
      board_by_player[p][k]=board_by_player[p][i];
    }
  }
  for(unsigned int p=0;p<Nplayers;p++)
    board_by_player[p].clear();
  for(unsigned int j=0;j<Nboards_eval;j++)
    board_by_player[alltrials_eval[j].player_id].push_back(j);
  for(unsigned int p=0;p<Nplayers;p++){
    N=board_by_player[p].size();
    for(unsigned int i=0;i<N;i++){
      k=uniform_int_distribution<unsigned int> {i,N-1}(engine);
      alltrials_eval[board_by_player[p][k]].group=(Ngroups*i)/N+1;
      board_by_player[p][k]=board_by_player[p][i];
    }
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
