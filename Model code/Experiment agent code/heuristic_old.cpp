#include "heuristic_old.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <iomanip>

namespace old{

pattern::pattern(uint64 p,uint64 pe,int n_val,double* w,int i,int j){
  pieces=p;
  pieces_empty=pe;
  n=n_val;
  weight_act=w[i];
  weightind_act=i;
  weight_pass=w[j];
  weightind_pass=j;
}

void pattern::update(double* w){
  weight_act=w[weightind_act];
  weight_pass=w[weightind_pass];
}

void pattern::write(ostream& out){
  board(pieces,pieces_empty).write();
  out<<hex<<setw(16)<<internal<<showbase<<setfill('0')<<pieces<<endl
     <<hex<<setw(16)<<internal<<showbase<<setfill('0')<<pieces_empty<<endl;
  out<<dec<<n<<"\t"<<weight_act<<"\t"<<weight_pass<<"\t"<<weightind_act<<"\t"<<weightind_pass<<endl;
  cin.get();
}

inline double pattern::diff_act_pass(){
  return weight_act-weight_pass;
}

inline bool pattern::contained(board& b){
  return b.contains(pieces,BLACK) || b.contains(pieces,WHITE);
}

inline bool pattern::contained(board& b,bool player){
  return b.contains(pieces,player);
}

inline bool pattern::is_active(board& b){
  return b.Nempty(pieces_empty)>=n;
}

inline bool pattern::just_active(board& b){
  return b.Nempty(pieces_empty)==n;
}

inline uint64 pattern::missing_pieces(board& b, bool player){
  return pieces & (~b.pieces[player]);
}

inline bool pattern::isempty(uint64 m){
  return m & pieces_empty;
}

void heuristic::get_params_from_file(char* filename,int subject=0){
  ifstream input(filename,ios::in);
  string s;
  for(int i=0;i<subject;i++)
    getline(input,s);
  input>>D0>>K0>>gamma>>delta>>lapse_rate>>vert_scale>>diag_scale>>opp_scale;
  for(int i=0;i<6;i++)
    input>>weight[i];
  input>>weight[16];
  input.close();
  update();
}

void heuristic::get_params_from_matlab(double* input){
  D0=input[0];
  K0=input[1];
  gamma=input[2];
  delta=input[3];
  lapse_rate=input[4];
  vert_scale=input[5];
  diag_scale=input[6];
  opp_scale=input[7];
  for(int i=8;i<14;i++)
    weight[i]=input[i];
  weight[16]=input[14];
  update();
}

void heuristic::get_features_from_file(char* filename){
  uint64 c,e;
  int i,j,n;
  ifstream input(filename,ios::in);
  scale_lines();
  if(input.is_open())
    while(!input.eof()){
      input>>hex>>c>>e;
      input>>dec>>i>>j>>n;
      addfeature(c,e,i,j,n);
    }
  input.close();
}

void heuristic::addfeature(uint64 config, int i, int j){
  int shiftc=colmax(config);
  int shiftr=rowmax(config);
  for(int col=0;col<shiftc;col++)
    for(int row=0;row<shiftr;row++)
      feature.push_back(pattern(shift(config,row,col),0,0,weight,i,j));
  Nfeatures=feature.size();
}

void heuristic::addfeature(uint64 config, uint64 confempty, int i, int j, int n){
  int cmin=colmin(config,confempty,n);
  int cmax=colmax(config,confempty,n);
  int rmin=rowmin(config,confempty,n);
  int rmax=rowmax(config,confempty,n);
  board b;
  for(int col=cmin;col<cmax;col++)
    for(int row=rmin;row<rmax;row++)
      if(num_bits(shift(confempty,row,col))>=n)
        feature.push_back(pattern(shift(config,row,col),shift(confempty,row,col),n,weight,i,j));
  Nfeatures=feature.size();
}

void heuristic::update(){
  iter_dist=geometric_distribution<int>(gamma);
  K_dist=bernoulli_distribution(fmod(K0,1.0));
  D_dist=bernoulli_distribution(fmod(D0,1.0));
  noise=normal_distribution<double>(0.0,1.0);
  lapse=bernoulli_distribution(lapse_rate);
  attention=bernoulli_distribution(delta);
  scale_lines();
  update_weights();
}

void heuristic::update_weights(){
  for(unsigned int i=0;i<Nfeatures;i++)
    feature[i].update(weight);
}

void heuristic::scale_lines(){
  for(int i=1;i<=5;i++){
    weight[i+5]=weight[i]*vert_scale;
    weight[i+10]=weight[i]*diag_scale;
  }
}

void heuristic::eliminate_noise(){
  noise=normal_distribution<double> (0.0,0.0);
  lapse=bernoulli_distribution(0.0);
  attention=bernoulli_distribution(0.0);
  delta=0.0;
  lapse_rate=0.0;
}

void heuristic::seed_generator(mt19937_64 generator){
  engine.seed(generator());
}

void heuristic::perturb_weights(double sigma_pert=0.1){
  for(unsigned int i=0;i<Nweights;i++)
    weight[i]+=sigma_pert*noise(engine);
  update_weights();
}

double heuristic::evaluate(board b, zet m){
  return evaluate(b+m);
}

double heuristic::evaluate(board b){
  bool player=b.active_player();
  double temp=weight[0]*(b.Nfull(center,player)-b.Nfull(center,!player));
  for(unsigned int i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      if(feature[i].contained(b,player))
        temp+=feature[i].weight_act;
      else if(feature[i].contained(b,!player))
        temp-=feature[i].weight_pass;
  }
  return player==BLACK?temp:-temp;
}

void heuristic::write(ostream& out){
  out<<D0<<"\t"<<K0<<"\t"<<gamma<<"\t"<<delta<<"\t"
  <<lapse_rate<<"\t"<<vert_scale<<"\t"<<diag_scale<<"\t"<<opp_scale<<"\t";
  for(unsigned int i=0;i<6;i++)
    out<<weight[i]<<"\t";
  out<<weight[16]<<endl;
  //for(unsigned int i=0;i<Nfeatures;i++)
  //  feature[i].write(out);
}

void heuristic::remove_features(){
  unsigned int i=0;
  while(i<Nfeatures){
    if(attention(engine)){
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

double heuristic::logprior(){
  return (log(vert_scale)+0.5*(log(vert_scale)-1.0)*(log(vert_scale)-1.0))
        +(log(diag_scale)+0.5*(log(diag_scale)-1.0)*(log(diag_scale)-1.0))
        +(log(opp_scale)+0.5*(log(opp_scale)-1.0)*(log(opp_scale)-1.0));
}

void heuristic::write_to_header(char* filename){
  ofstream headout(filename,ios::out);
  headout<<"#include \"board.h\"\n"
  <<"#include \"heuristic.h\"\n\n"
  <<"#ifdef DEFAULT_WEIGHTS\n"
  <<"heuristic::heuristic(): weight {0.8,1.0,0.3,6.0,2.5,10.0,1.0,0.3,6.0,2.5,10.0,1.0,0.3,6.0,2.5,10.0,1.2},\n"
  <<"Nfeatures(0), D0(6.0), K0(5.0), gamma(0.025), delta(0.2), lapse_rate(0.01),\n"
  <<"vert_scale(1.8), diag_scale(1.2), opp_scale(1.5){\n"
  <<"\tupdate();\n}\n"
  <<"#else\n"
  <<"heuristic::heuristic(): weight{";
  for(unsigned int i=0;i<Nweights-1;i++)
    headout<<weight[i]<<",";
  headout<<weight[Nweights-1]<<"},\n"
  <<"Nfeatures("<<Nfeatures<<"), D0("<<D0<<"), K0("<<K0<<"),gamma("<<gamma<<"), delta("<<delta<<"), \n"
  <<"lapse_rate("<<lapse_rate<<"), vert_scale("<<vert_scale<<"), diag_scale("<<diag_scale<<"), opp_scale("<<opp_scale<<"),\n"
  <<"feature{";
  for(unsigned int i=0;i<Nfeatures-1;i++)
    headout<<"{0x"<<hex<<feature[i].pieces<<"ULL,0x"<<feature[i].pieces_empty<<dec<<"ULL,"<<feature[i].n
    <<",weight,"<<feature[i].weightind_act<<","<<feature[i].weightind_pass<<"},\n";
  headout<<"{0x"<<hex<<feature[Nfeatures-1].pieces<<"ULL,0x"<<feature[Nfeatures-1].pieces_empty<<dec<<"ULL,"
  <<feature[Nfeatures-1].n<<",weight,"<<feature[Nfeatures-1].weightind_act<<","
  <<feature[Nfeatures-1].weightind_pass<<"}}{\n\tupdate();\n}\n";
  headout<<"#endif";
  headout.close();
}

vector<zet> heuristic::get_moves(board& b, bool player, bool nosort=false){
  vector<zet> candidate;
  unsigned int i;
  uint64 m,m1,m2;
  map<uint64,int> lookup;
  double deltaL=0.0;
  double c_act,c_pass;
  if(player==self){
    c_act=2.0*opp_scale/(1.0+opp_scale);
    c_pass=2.0/(1.0+opp_scale);
  }
  else{
    c_act=2.0/(1.0+opp_scale);
    c_pass=2.0*opp_scale/(1.0+opp_scale);
  }
  for(i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      if(feature[i].contained(b,player))
        deltaL-=c_pass*feature[i].diff_act_pass();
      else if(feature[i].contained(b,!player))
        deltaL-=c_act*feature[i].diff_act_pass();
    }
  for(i=1,m=1;m!=boardend;m<<=1){
    if(b.isempty(m)){
      if(m & center)
        candidate.push_back(zet(m,deltaL+c_pass*weight[0]+noise(engine),player));
      else candidate.push_back(zet(m,deltaL+noise(engine),player));
      lookup[m]=i;
      i++;
    }
  }
  for(i=0;i<Nfeatures;i++)
    if(feature[i].is_active(b)){
      m1=feature[i].missing_pieces(b,player);
      m2=feature[i].missing_pieces(b,!player);
      if((m1 & m2) && has_one_bit(m1))
        candidate[lookup[m1]-1].val+=c_pass*feature[i].weight_pass;
      if(m1==0 && feature[i].just_active(b))
        for(m=1;m!=boardend;m<<=1)
          if(b.isempty(m) && feature[i].isempty(m))
            candidate[lookup[m]-1].val-=c_pass*feature[i].weight_pass;
      if(m2==0 && feature[i].just_active(b))
        for(m=1;m!=boardend;m<<=1)
          if(b.isempty(m) && feature[i].isempty(m))
            candidate[lookup[m]-1].val+=c_act*feature[i].weight_act;
    }
  if(nosort)
    return candidate;
  sort(candidate.begin(),candidate.end(),compare);
  return candidate;
}

vector<zet> heuristic::get_pruned_moves(board& b, bool player){
  vector<zet> candidate=get_moves(b,player);
  if(candidate.size()>K)
    candidate.erase(candidate.begin()+K,candidate.end());
  return candidate;
}

void heuristic::check_moves(vector<zet> candidates,board b){
  zet m;
  for(unsigned int i=0;i<candidates.size();i++){
    m=candidates[i];
    if(abs(evaluate(b+m)-evaluate(b)-(m.player==BLACK?m.val:-m.val))>0.01){
      b.write(m);
      (b+m).write();
      cout<<evaluate(b)<<"\t"<<evaluate(b,m)<<"\t"<<(m.player==BLACK?m.val:-m.val)<<endl;
      cin.get();
    }
  }
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

zet heuristic::makemove(board bstart,bool player){
  depth=D_dist(engine)+D0;
  K=K_dist(engine)+K0;
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
  iterations=0;
  if(lapse(engine))
    return makerandommove(bstart,player);
  remove_features();
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

double heuristic::loglik(board b,zet m){
  double L=0.0;
  int n=1;
  while(makemove_bfs(b,m.player).zet_id!=m.zet_id){
    L-=1.0/n;
    n++;
  }
  return L;
}

zet heuristic::makemove_notree(board b,bool player){
  vector<zet> candidate=get_moves(b,player);
  zet m(0,0.0,player);
  if(lapse(engine))
    return makerandommove(b,player);
  remove_features();
  if(candidate.size()>0)
    m=candidate[0];
  restore_features();
  return m;
}

zet heuristic::makemove_notree_noatt(board b,bool player){
  vector<zet> candidate=get_moves(b,player);
  zet m(0,0.0,player);
  if(lapse(engine))
    return makerandommove(b,player);
  if(candidate.size()>0)
    m=candidate[0];
  return m;
}

zet superheuristic::makemove_bfs(board b, bool player){
  uniform_int_distribution<int> dist(0,N-1);
  int subject=dist(engine);
  return h[subject].makemove_bfs(b,player);
}
}
