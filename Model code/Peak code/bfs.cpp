#include "bfs.h"
#include "heuristic.h"

using namespace peak;

zet heuristic::makemove_bfs(board b,bool player){
  node game_tree(b,evaluate(b),player,1);
  node *n=&game_tree;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  iterations=0;
  if(lapse(engine))
    return makerandommove(b,player);
  remove_features();
  self=player;
  while(iterations<max_iterations && t<tmax && !game_tree.determined()){
    candidates=get_pruned_moves(n->b,n->player);
    n->expand(candidates);
    n=game_tree.select();
    mold=m;
    m=game_tree.bestmove().zet_id;
    if(mold==m)
      t++;
    else t=0;
    iterations++;
  }
  restore_features();
  return game_tree.bestmove();
}
