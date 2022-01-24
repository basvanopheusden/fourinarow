#include "heuristic_drop.h"
#include <algorithm>


vector<zet> heuristic_drop::get_moves(board& b, bool player, bool nosort){
  vector<zet> candidate;
  unsigned int i;
  uint64 m,m1,m2;
  map<uint64,int> lookup;
  double deltaL=0.0;
  double c_act=(player==self)?c_self:c_opp;
  double c_pass=(player==self)?c_opp:c_self;
  for(i=1,m=1;m!=boardend;m<<=1){
    if(b.isempty(m)){
      candidate.push_back(zet(m,center_weight*vtile[m]+noise(engine),player));
      lookup[m]=i;
      i++;
    }
  }
  for(i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b) && bernoulli_distribution{feature[i].drop_rate}(engine)){
      if(feature[i].contained(b,player))
        deltaL-=c_pass*feature[i].diff_act_pass();
      else if(feature[i].contained(b,!player))
        deltaL-=c_act*feature[i].diff_act_pass();
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
  for(i=1,m=1;m!=boardend;m<<=1){
    if(b.isempty(m)){
      candidate[lookup[m]-1].val+=deltaL;
    }
  }
  if(nosort)
    return candidate;
  sort(candidate.begin(),candidate.end(),compare);
  return candidate;
}

zet heuristic_drop::makemove_bfs(board b,bool player,bool save_tree, bool save_features_dropped){
  bfs::node *n;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  int iterations=0;
  if(lapse(engine)){
    cout<<"lapse"<<endl;
    return makerandommove(b,player);
  }
  game_tree = new bfs::node(b,evaluate(b),player,1);
  n = game_tree;
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
  bestmove = game_tree->bestmove();
  if(!save_tree){
    delete(game_tree);
    game_tree = NULL;
  }
  return bestmove;
}
