#include "heuristic_fixed_iters.h"

zet heuristic_fixed_iters::makemove_bfs(board b,bool player,bool save_tree, bool save_features_dropped){
  game_tree = new bfs::node(b,evaluate(b),player,1);
  bfs::node *n=game_tree;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=1.0/gamma;
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
  bestmove = game_tree->bestmove();
  if(!save_tree){
    delete(game_tree);
    game_tree = NULL;
  }
  if(!save_features_dropped){
    restore_features();
  }
  return bestmove;
}
