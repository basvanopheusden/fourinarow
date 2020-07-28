#include "heuristic_nhp.h"
#include "bfs_nhp.h"

bool play_nhp_game(heuristic_nhp& h_black, heuristic_nhp& h_white, bool verbose = false){
  board b;
  zet m;
  vector<zet> candidates;
  while(!b.is_full()){
      m = h_black.makemove_bfs(b,BLACK);
      b.add(m);
      if(verbose)
        b.write();
      if(b.black_has_won())
        break;
      m = h_white.makemove_bfs(b,WHITE);
      b.add(m);
      if(verbose)
        b.write();
  }
  return b.black_has_won();
}

void test_nhp_agents(int N,int k){
  ofstream output("../nhp_tournament_results_with_lapse.txt",ios::out);
  heuristic_nhp h_black, h_white;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  h_black.seed_generator(global_generator);
  h_white.seed_generator(global_generator);
  const char* param_filename = "../params.txt";
  int i,j;
  for(int n=0;n<N*N;n+=k){
    i=n/N;
    j=n%N;
    h_black.get_params_from_file(param_filename,i);
    h_black.lapse_rate = (i%20==0?1:(i%20==1?0.5+0.5*h_black.lapse_rate:h_black.lapse_rate));
    h_black.update();
    h_black.c_self = 2.0;
    h_black.c_opp = 0.0;
    h_white.get_params_from_file(param_filename,j);
    h_white.lapse_rate = (j%20==0?1:(j%20==1?0.5+0.5*h_white.lapse_rate:h_white.lapse_rate));
    h_white.update();
    h_white.c_self = 0.0;
    h_white.c_opp = 2.0;
    output<<i<<"\t"<<j<<"\t"<<play_nhp_game(h_black,h_white,false)<<endl;
    cout<<i<<"\t"<<j<<endl;
  }
  output.close();
}

zet heuristic_nhp::makemove_bfs(board b,bool player,bool save_tree){
  game_tree = new bfs::nhp_node(b,evaluate(b),player,1);
  bfs::node *n=game_tree;
  zet bestmove;
  uint64 mold,m=0x0ULL;
  int t=0,tmax=stopping_thresh;
  vector<zet> candidates;
  int max_iterations=iter_dist(engine)+1;
  int iterations=0;
  if(lapse(engine))
    returk_chose_utilityn makerandommove(b,player);
  remove_features();
  self=player;
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
