#include "bfs_old.h"
#include "heuristic_old.h"
using namespace old;

mt19937_64 node::engine;

zet heuristic::makemove_bfs(board b,bool player){
  node game_tree(b,evaluate(b),player,1);
  node *n=&game_tree;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  game_tree.seed_engine(engine);
  iterations=0;
  if(lapse(engine))
    return makerandommove(b,player);
  K=K_dist(engine)+K0;
  remove_features();
  self=player;
  while(iterations<max_iterations && !game_tree.determined()){
    candidates=get_pruned_moves(n->b,n->player);
    n->expand(candidates);
    n=game_tree.select();
    iterations++;
  }
  restore_features();
  return game_tree.bestmove();
}
