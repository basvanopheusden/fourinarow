#include "heuristic_fixed_branch.h"

void heuristic_fixed_branch::get_params_from_array(double* input){
  stopping_thresh=input[0];
  K0=input[1];
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

vector<zet> heuristic_fixed_branch::get_pruned_moves(board& b, bool player){
  unsigned int K=K_dist(engine)+K0;
  vector<zet> candidate=get_moves(b,player,false);
  if(candidate.size()>K)
    candidate.erase(candidate.begin()+K,candidate.end());
  return candidate;
}
