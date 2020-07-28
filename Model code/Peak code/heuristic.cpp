#include "heuristic.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <iomanip>
using namespace peak;

pattern::pattern(uint64 p,uint64 pe,int n_val,double* w_act,double* w_pass,double* delta,int i){
  pieces=p;
  pieces_empty=pe;
  n=n_val;
  ind=i;
  weight_act=w_act[i];
  weight_pass=w_pass[i];
  drop_rate=delta[i];
}

void pattern::update(double* w_act, double* w_pass, double* delta){
  weight_act=w_act[ind];
  weight_pass=w_pass[ind];
  drop_rate=delta[ind];
}

inline double pattern::diff_act_pass(){
  return weight_act-weight_pass;
}

inline bool pattern::contained(board& b){
  return b.contains(pieces,BLACK) || b.contains(pieces,WHITE);
}

inline bool pattern::contained(board& b,bool player){
  return b.contains(pieces,player);
}

inline bool pattern::is_active(board& b){
  return b.Nempty(pieces_empty)>=n;
}

inline bool pattern::just_active(board& b){
  return b.Nempty(pieces_empty)==n;
}

inline uint64 pattern::missing_pieces(board& b, bool player){
  return pieces & (~b.pieces[player]);
}

inline bool pattern::isempty(uint64 m){
  return m & pieces_empty;
}

void heuristic::get_params_from_file(char* filename,int subject=0){
  get_params_from_file(filename,subject,0);
}

void heuristic::get_params(const double* params){
  stopping_thresh=10000.0;
  pruning_thresh=params[0];
  gamma=params[1];
  opp_scale=1.0;
  exploration_constant=1.0;
  for(unsigned int i=0;i<Nweights;i++)
    delta[i]=params[2];
  lapse_rate=params[3];
  center_weight=params[5];
  for(unsigned int i=0;i<Nweights-1;i++){
    w_act[i]=params[i%4+6];
    w_pass[i]=params[i%4+6]*params[4];
  }
  w_act[Nweights-1]=0.0;
  w_pass[Nweights-1]=0.0;
  update();
}

void heuristic::get_params_from_file(char* filename,int subject=0,int group=0){
  ifstream input(filename,ios::in);
  string s;
  if(!input)
    cout<<"could not open input"<<endl;
  for(int i=0;i<5*subject+group-1;i++)
    getline(input,s);
  input>>stopping_thresh>>pruning_thresh>>gamma>>lapse_rate>>opp_scale>>exploration_constant>>center_weight;
  for(unsigned int i=0;i<Nweights;i++)
    input>>w_act[i];
  for(unsigned int i=0;i<Nweights;i++)
    input>>w_pass[i];
  for(unsigned int i=0;i<Nweights;i++)
    input>>delta[i];
  input.close();
  update();
}

void heuristic::get_params_from_matlab(double* input){
  stopping_thresh=input[0];
  pruning_thresh=input[1];
  gamma=input[2];
  lapse_rate=input[3];
  opp_scale=input[4];
  exploration_constant=input[5];
  center_weight=input[6];
  for(unsigned int i=0;i<Nweights;i++)
    w_act[i]=input[i+7];
  for(unsigned int i=0;i<Nweights;i++)
    w_pass[i]=input[i+Nweights+7];
  for(unsigned int i=0;i<Nweights;i++)
    delta[i]=input[i+2*Nweights+7];
  update();
}

void heuristic::get_features_from_file(char* filename){
  uint64 c,e;
  int i,n;
  ifstream input(filename,ios::in);
  if(input.is_open())
    while(!input.eof()){
      input>>hex>>c>>e;
      input>>dec>>i>>n;
      addfeature(c,e,i,n);
    }
  input.close();
}

void heuristic::addfeature(uint64 config, int i){
  int shiftc=colmax(config);
  int shiftr=rowmax(config);
  for (int col=0; col<shiftc; col++)
  {
      for(int row=0;row<shiftr;row++)
      {
          feature.push_back(pattern(shift(config,row,col),0,0,w_act,w_pass,delta,i));
      }
  }
  Nfeatures = (unsigned int)feature.size();
}

void heuristic::addfeature(uint64 config, uint64 confempty, int i, int n){
  int cmin=colmin(config,confempty,n);
  int cmax=colmax(config,confempty,n);
  int rmin=rowmin(config,confempty,n);
  int rmax=rowmax(config,confempty,n);
  board b;
  for (int col=cmin;col<cmax;col++)
  {
      for (int row=rmin; row<rmax; row++)
      {
          if(num_bits(shift(confempty,row,col))>=n)
          {
              feature.push_back(pattern(shift(config,row,col),shift(confempty,row,col),n,w_act,w_pass,delta,i));
          }
      }
  }
  Nfeatures = (unsigned int)feature.size();
}

void heuristic::update(){
  iter_dist=geometric_distribution<int>(gamma);
  noise=normal_distribution<double>(0.0,1.0);
  lapse=bernoulli_distribution(lapse_rate);
  for(uint64 m=1;m!=boardend;m<<=1)
    vtile[m]=1.0/sqrt(pow(uint64totile(m)/BOARD_WIDTH-1.5,2) + pow(uint64totile(m)%BOARD_WIDTH-4.0,2));
  update_weights();
}

void heuristic::update_weights(){
  for(unsigned int i=0;i<Nfeatures;i++)
    feature[i].update(w_act,w_pass,delta);
}

void heuristic::seed_generator(mt19937_64 generator){
  engine.seed(generator());
}

double heuristic::evaluate(board b, zet m){
  return evaluate(b+m);
}

double heuristic::evaluate(board b){
  bool player=b.active_player();
  double temp=0.0;
  for(uint64 m=1;m!=boardend;m<<=1){
    if(b.contains(m,player))
      temp+=center_weight*vtile[m];
    if(b.contains(m,!player))
      temp-=center_weight*vtile[m];
  }
  for(unsigned int i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      if(feature[i].contained(b,player))
        temp+=feature[i].weight_act;
      else if(feature[i].contained(b,!player))
        temp-=feature[i].weight_pass;
  }
  return player==BLACK?temp:-temp;
}

void heuristic::remove_features(){
  unsigned int i=0;
  while(i<Nfeatures){
    if(bernoulli_distribution{feature[i].drop_rate}(engine)){
      feature_backup.push_back(feature[i]);
      feature.erase(feature.begin()+i);
      Nfeatures--;
    }
    else i++;
  }
}

void heuristic::restore_features(){
  for(unsigned int i=0;i<feature_backup.size();i++){
    feature.push_back(feature_backup[i]);
    Nfeatures++;
  }
  feature_backup.clear();
}

vector<zet> heuristic::get_moves(board& b, bool player, bool nosort=false){
  vector<zet> candidate;
  unsigned int i;
  uint64 m,m1,m2;
  map<uint64,int> lookup;
  double deltaL=0.0;
  double c_act,c_pass;
  if(player==self){
    c_act=2.0*opp_scale/(1.0+opp_scale);
    c_pass=2.0/(1.0+opp_scale);
  }
  else{
    c_act=2.0/(1.0+opp_scale);
    c_pass=2.0*opp_scale/(1.0+opp_scale);
  }
  for(i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      if(feature[i].contained(b,player))
        deltaL-=c_pass*feature[i].diff_act_pass();
      else if(feature[i].contained(b,!player))
        deltaL-=c_act*feature[i].diff_act_pass();
    }
  for(i=1,m=1;m!=boardend;m<<=1){
    if(b.isempty(m)){
      candidate.push_back(zet(m,deltaL+center_weight*vtile[m]+noise(engine),player));
      lookup[m]=i;
      i++;
    }
  }
  for(i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      m1=feature[i].missing_pieces(b,player);
      m2=feature[i].missing_pieces(b,!player);
      if((m1 & m2) && has_one_bit(m1))
        if(lookup[m1])
          candidate[lookup[m1]-1].val+=c_pass*feature[i].weight_pass;
      if(m1==0 && feature[i].just_active(b))
        for(m=1;m!=boardend;m<<=1)
          if(b.isempty(m) && feature[i].isempty(m))
            if(lookup[m])
              candidate[lookup[m]-1].val-=c_pass*feature[i].weight_pass;
      if(m2==0 && feature[i].just_active(b))
        for(m=1;m!=boardend;m<<=1)
          if(b.isempty(m) && feature[i].isempty(m))
            if(lookup[m])
              candidate[lookup[m]-1].val+=c_act*feature[i].weight_act;
    }
  if(nosort)
    return candidate;
  sort(candidate.begin(),candidate.end(),compare);
  return candidate;
}

vector<zet> heuristic::get_pruned_moves(board& b, bool player){
  vector<zet> candidate=get_moves(b,player);
  unsigned int i=1;
  while(i<candidate.size() && abs(candidate[0].val-candidate[i].val)<pruning_thresh)
    i++;
  if(i<candidate.size())
    candidate.erase(candidate.begin()+i,candidate.end());
  return candidate;
}

zet heuristic::makerandommove(board bstart, bool player){
  vector<uint64> options;
  for(uint64 m=1; m!=boardend; m<<=1)
  {
      if(bstart.isempty(m))
      {
          options.push_back(m);
      }
  }

  int Noptions = (int)options.size();

  if(Noptions > 0)
  {
    return zet(options[uniform_int_distribution<int>{0,Noptions-1}(engine)],0.0,player);
  }
  return zet(0,0.0,player);
}
