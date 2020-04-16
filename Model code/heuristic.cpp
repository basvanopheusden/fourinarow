#include "heuristic.h"
#include "bfs.h"

#include <algorithm>
#include <fstream>

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

double pattern::diff_act_pass(){
  return weight_act-weight_pass;
}

bool pattern::contained(board& b){
  return b.contains(pieces,BLACK) || b.contains(pieces,WHITE);
}

bool pattern::contained(board& b,bool player){
  return b.contains(pieces,player);
}

bool pattern::is_active(board& b){
  return b.Nempty(pieces_empty)>=n;
}

bool pattern::just_active(board& b){
  return b.Nempty(pieces_empty)==n;
}

uint64 pattern::missing_pieces(board& b, bool player){
  return pieces & (~b.pieces[player]);
}

bool pattern::isempty(uint64 m){
  return m & pieces_empty;
}

void heuristic::get_params_from_file(const char* filename,int subject=0,int group=0){
  ifstream input(filename,ios::in);
  string s;
  double params[3*Nweights+7];
  if(!input)
    cout<<"could not open input"<<endl;
  for(int i=0;i<5*subject+group-1;i++)
    getline(input,s);
  for(unsigned int i=0;i<3*Nweights+7;i++)
    input>>params[i];
  get_params_from_array(params);
  input.close();
}

void heuristic::get_params_from_array(double* input){
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

void heuristic::get_features_from_file(const char* filename){
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
  for(int col=0;col<shiftc;col++)
    for(int row=0;row<shiftr;row++)
      feature.push_back(pattern(shift(config,row,col),0,0,w_act,w_pass,delta,i));
  Nfeatures=feature.size();
}

void heuristic::addfeature(uint64 config, uint64 confempty, int i, int n){
  int cmin=colmin(config,confempty,n);
  int cmax=colmax(config,confempty,n);
  int rmin=rowmin(config,confempty,n);
  int rmax=rowmax(config,confempty,n);
  board b;
  for(int col=cmin;col<cmax;col++)
    for(int row=rmin;row<rmax;row++)
      if(num_bits(shift(confempty,row,col))>=n)
        feature.push_back(pattern(shift(config,row,col),shift(confempty,row,col),n,w_act,w_pass,delta,i));
  Nfeatures=feature.size();
}

void heuristic::update(){
  iter_dist=geometric_distribution<int>(gamma);
  K_dist=bernoulli_distribution(fmod(K0,1.0));
  D_dist=bernoulli_distribution(fmod(D0,1.0));
  noise=normal_distribution<double>(0.0,1.0);
  lapse=bernoulli_distribution(lapse_rate);
  for(uint64 m=1;m!=boardend;m<<=1)
    vtile[m]=1.0/sqrt(pow(uint64totile(m)/BOARD_WIDTH-1.5,2) + pow(uint64totile(m)%BOARD_WIDTH-4.0,2));
  c_self = 2.0*opp_scale/(1.0+opp_scale);
  c_opp = 2.0/(1.0+opp_scale);
  update_weights();
}

void heuristic::update_weights(){
  for(unsigned int i=0;i<Nfeatures;i++)
    feature[i].update(w_act,w_pass,delta);
}

void heuristic::eliminate_noise(){
  lapse_rate=0.0;
  for(unsigned int i=0;i<Nweights;i++)
    delta[i]=0.0;
  for(unsigned int i=0;i<Nfeatures;i++)
    feature[i].drop_rate=0.0;
  noise=normal_distribution<double> (0.0,0.0);
  lapse=bernoulli_distribution(0.0);
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
  double c_act=(player==self)?c_self:c_opp;
  double c_pass=(player==self)?c_opp:c_self;
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

vector<zet> heuristic::get_moves(board& b, uint64 m1,uint64 m2, bool player){
  vector<zet> candidate=get_moves(b,player,true);
  vector<zet> Retval;
  for(vector<zet>::iterator it=candidate.begin();it!=candidate.end();++it)
    if(it->zet_id==m1 || it->zet_id==m2)
      Retval.push_back(*it);
  return Retval;
}

zet heuristic::makerandommove(board bstart, bool player){
  vector<uint64> options;
  int Noptions;
  for(uint64 m=1;m!=boardend;m<<=1)
    if(bstart.isempty(m))
      options.push_back(m);
  Noptions=options.size();
  if(Noptions>0)
    return zet(options[uniform_int_distribution<int>{0,Noptions-1}(engine)],0.0,player);
  return zet(0,0.0,player);
}

zet heuristic::makerandommove(board b, uint64 m1,uint64 m2,bool player){
  if(bernoulli_distribution{0.5}(engine))
    return zet(m1,0.0,player);
  return zet(m2,0.0,player);
}

zet heuristic::makemove_bfs(board b,bool player,bool save_tree){
  game_tree = new bfs::node(b,evaluate(b),player,1);
  bfs::node *n=game_tree;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  int iterations=0;
  if(lapse(engine))
    return makerandommove(b,player);
  remove_features();
  self=player;
  zet bestmove;
  while(iterations<max_iterations && t<tmax && !game_tree->determined()){
    candidates=get_pruned_moves(n->b,n->player);
    n->expand(candidates,iterations);
    n=game_tree->select();
    mold=m;
    m=game_tree->bestmove().zet_id;
    if(mold==m)
      t++;
    else t=0;
    iterations++;
  }
  restore_features();
  bestmove = game_tree->bestmove();
  if(!save_tree){
    delete(game_tree);
    game_tree = NULL;
  }
  return bestmove;
}

zet heuristic::makemove_bfs(board b, uint64 m1, uint64 m2, bool player, bool save_tree){
  game_tree = new bfs::node(b,evaluate(b),player,1);
  bfs::node *n=game_tree;
  zet bestmove;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  if(lapse(engine))
    return makerandommove(b,m1,m2,player);
  remove_features();
  self=player;
  game_tree->expand(get_moves(b,m1,m2,player),0);
  n=game_tree->select();
  m=game_tree->bestmove().zet_id;
  int iterations=1;
  while(iterations<max_iterations && t<tmax && !game_tree->determined()){
    candidates=get_pruned_moves(n->b,n->player);
    n->expand(candidates,iterations);
    n=game_tree->select();
    mold=m;
    m=game_tree->bestmove().zet_id;
    if(mold==m)
      t++;
    else t=0;
    iterations++;
  }
  restore_features();
  bestmove = game_tree->bestmove();
  if(!save_tree){
    delete(game_tree);
    game_tree = NULL;
  }
  return bestmove;
}
