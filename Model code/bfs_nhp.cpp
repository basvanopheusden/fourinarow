#include "bfs_nhp.h"

#define BLACK_WINS 4*BOARD_WIDTH
#define WHITE_WINS -4*BOARD_WIDTH
#define DRAW 0

using namespace bfs;

nhp_node::nhp_node(board bstart,double v,bool p,int d){
  b=bstart;
  depth=d;
  player=p;
  child=NULL;
  parent=NULL;
  best=NULL;
  Nchildren=0;
  iteration=-1;
  m=0;
  if(b.black_has_won())
    pess=opt=BLACK_WINS-depth,val=10000.0;
  else if(b.is_full())
    pess=opt=0,val=0.0;
  else val=v,pess=0,opt=BLACK_WINS-depth-1;
}

void nhp_node::get_opt(){
  opt=(player==BLACK?0:BLACK_WINS);
  for(unsigned int i=0;i<Nchildren;i++)
    update_opt(child[i]);
}

void nhp_node::get_pess(){
  pess=(player==BLACK?0:BLACK_WINS);
  for(unsigned int i=0;i<Nchildren;i++)
    update_pess(child[i]);
}

void nhp_node::expand(vector<zet> candidate,int n){
  Nchildren=candidate.size();
  if(Nchildren>0){
    iteration = n;
    child=new node*[Nchildren];
    for(unsigned int i=0;i<Nchildren;i++){
      if(player==BLACK)
        child[i]=new nhp_node(b+candidate[i],val+candidate[i].val,!player,depth+1);
      else child[i]=new nhp_node(b+candidate[i],val-candidate[i].val,!player,depth+1);
      child[i]->parent=this;
      child[i]->m=candidate[i].zet_id;
    }
    get_opt();
    get_pess();
    get_val();
    if(determined())
      get_best_determined();
    if(parent)
      parent->backpropagate(this);
  }
}
