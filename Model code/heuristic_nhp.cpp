#include "heuristic_nhp.h"
#include "bfs_nhp.h"

zet heuristic_nhp::makemove_bfs(board b,bool player,bool save_tree){
  game_tree = new bfs::nhp_node(b,evaluate(b),player,1);
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

zet heuristic_nhp::makemove_bfs(board b, uint64 m1, uint64 m2, bool player, bool save_tree){
  game_tree = new bfs::nhp_node(b,evaluate(b),player,1);
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
