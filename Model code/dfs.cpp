#include "heuristic.h"

zet heuristic::makemove_dfs(board bstart,bool player){
  int depth=D_dist(engine)+D0;
  double alpha[depth+1];
  double beta[depth+1];
  board b[depth+1];
  double L[depth+1];
  unsigned int i[depth+1];
  vector<zet> candidate[depth+1];
  int level=0;
  int ibest=-1;
  bool onreturn=false;
  zet m;
  double retval;
  alpha[0]=-10000.0;
  beta[0]=10000.0;
  L[0]=evaluate(bstart);
  b[0]=bstart;
  int iterations=0;
  if(lapse(engine))
    return makerandommove(bstart,player);
  remove_features();
  self=player;
  while(level!=-1){
    if(onreturn){
      level--;
      if(level!=-1){
        if((player+level)%2==BLACK && retval>alpha[level]){
          alpha[level]=retval;
          if(level==0){
            ibest=i[0];
            candidate[0][ibest].val=retval;
          }
        }
        if((player+level)%2==WHITE && retval<beta[level]){
          beta[level]=retval;
          if(level==0){
            ibest=i[0];
            candidate[0][ibest].val=retval;
          }
        }
        i[level]++;
        onreturn=false;
      }
    }
    else if(b[level].black_has_won())
      retval=10000.0-level-0.1*noise(engine),onreturn=true;
    else if(b[level].white_has_won())
      retval=-10000.0+level+0.1*noise(engine),onreturn=true;
    else if(b[level].is_full())
      retval=0.0,onreturn=true;
    else if(level==depth){
      retval=L[level];
      onreturn=true;
    }
    else if(candidate[level].empty()){
      iterations++;
      candidate[level]=get_pruned_moves(b[level],(player+level)%2);
      i[level]=0;
    }
    else if((player+level)%2==WHITE && (i[level]==candidate[level].size() || beta[level]<=alpha[level]))
      retval=beta[level],onreturn=true;
    else if((player+level)%2==BLACK && (i[level]==candidate[level].size() || alpha[level]>=beta[level]))
      retval=alpha[level],onreturn=true;
    else {
      m=candidate[level][i[level]];
      if((player+level)%2==BLACK)
        L[level+1]=L[level]+m.val;
      else L[level+1]=L[level]-m.val;
      b[level+1]=b[level]+m;
      alpha[level+1]=alpha[level];
      beta[level+1]=beta[level];
      candidate[level+1].clear();
      level++;
    }
  }
  restore_features();
  if(ibest==-1)
    return zet(0,0.0,player);
  return candidate[0][ibest];
}

zet heuristic::makemove_dfs(board bstart,uint64 m1, uint64 m2, bool player){
  int depth=D_dist(engine)+D0;
  double alpha[depth+1];
  double beta[depth+1];
  board b[depth+1];
  double L[depth+1];
  unsigned int i[depth+1];
  vector<zet> candidate[depth+1];
  int level=0;
  int ibest=-1;
  bool onreturn=false;
  zet m;
  double retval;
  alpha[0]=-10000.0;
  beta[0]=10000.0;
  L[0]=evaluate(bstart);
  b[0]=bstart;
  int iterations=0;
  if(lapse(engine))
    return makerandommove(bstart,player);
  remove_features();
  self=player;
  while(level!=-1){
    if(onreturn){
      level--;
      if(level!=-1){
        if((player+level)%2==BLACK && retval>alpha[level]){
          alpha[level]=retval;
          if(level==0){
            ibest=i[0];
            candidate[0][ibest].val=retval;
          }
        }
        if((player+level)%2==WHITE && retval<beta[level]){
          beta[level]=retval;
          if(level==0){
            ibest=i[0];
            candidate[0][ibest].val=retval;
          }
        }
        i[level]++;
        onreturn=false;
      }
    }
    else if(b[level].black_has_won())
      retval=10000.0-level-0.1*noise(engine),onreturn=true;
    else if(b[level].white_has_won())
      retval=-10000.0+level+0.1*noise(engine),onreturn=true;
    else if(b[level].is_full())
      retval=0.0,onreturn=true;
    else if(level==depth){
      retval=L[level];
      onreturn=true;
    }
    else if(candidate[level].empty()){
      iterations++;
      if(level==0)
        candidate[level]=get_moves(b[level],m1,m2,(player+level)%2);
      else candidate[level]=get_pruned_moves(b[level],(player+level)%2);
      i[level]=0;
    }
    else if((player+level)%2==WHITE && (i[level]==candidate[level].size() || beta[level]<=alpha[level]))
      retval=beta[level],onreturn=true;
    else if((player+level)%2==BLACK && (i[level]==candidate[level].size() || alpha[level]>=beta[level]))
      retval=alpha[level],onreturn=true;
    else {
      m=candidate[level][i[level]];
      if((player+level)%2==BLACK)
        L[level+1]=L[level]+m.val;
      else L[level+1]=L[level]-m.val;
      b[level+1]=b[level]+m;
      alpha[level+1]=alpha[level];
      beta[level+1]=beta[level];
      candidate[level+1].clear();
      level++;
    }
  }
  restore_features();
  if(ibest==-1)
    return zet(0,0.0,player);
  return candidate[0][ibest];
}
