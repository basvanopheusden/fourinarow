#ifndef BFSOLD_H_INCLUDED
#define BFSOLD_H_INCLUDED

#include "../board.h"
#include <vector>
#include <fstream>
#include <iomanip>
#include <random>
using namespace std;

#define BLACK_WINS 4*BOARD_WIDTH
#define WHITE_WINS -4*BOARD_WIDTH
#define DRAW 0

namespace old{
class node{
  private:
    node** child;
    node* parent;
    node* best;
    unsigned int Nchildren;
    uint64 m;
  public:
    static mt19937_64 engine;
    board b;
    double val;
    int pess;
    int opt;
    bool player;
    int depth;
    node(){
      val=0.0;
      child=NULL;
      parent=NULL;
      best=NULL;
      Nchildren=0;
      depth=1;
      m=0;
      pess=0;
      opt=0;
      //game_status=UNDETERMINED;
    }
    node(board bstart,double v,bool p,int d){
      b=bstart;
      depth=d;
      player=p;
      child=NULL;
      parent=NULL;
      best=NULL;
      Nchildren=0;
      m=0;
      if(b.black_has_won())
        pess=opt=BLACK_WINS-depth,val=10000.0;
      else if(b.white_has_won())
        pess=opt=WHITE_WINS+depth,val=-10000.0;
      else if(b.is_full())
        pess=opt=0,val=0.0;
      else val=v,pess=WHITE_WINS+depth+1,opt=BLACK_WINS-depth-1;
    }
    bool winning(){
      if(player==BLACK)
        return pess>DRAW;
      else return opt<DRAW;
    }
    void check(ostream& out){
      double oldval=val;
      int oldstat=opt;
      node* oldbest=best;
      if(!determined())
        get_val();
      if(oldbest!=best || val-oldval>0.001 || oldval-val>0.001){
        write(out);
        out<<"real best"<<endl;
        if(best)
          best->write(out);
        else out<<best<<"\t"<<val<<endl;
        out<<"old best"<<endl;
        if(oldbest)
          oldbest->write(out);
        else out<<oldbest<<"\t"<<oldval<<endl;
        cin.get();
      }
      val=oldval;
      opt=oldstat;
      best=oldbest;
      for(unsigned int i=0;i<Nchildren;i++)
        child[i]->check(out);
    }
    ~node(){
      if(child){
        for(unsigned int i=0;i<Nchildren;i++)
          delete child[i];
        delete[] child;
      }
      parent=NULL;
      best=NULL;
    }
    void seed_engine(mt19937_64 engine_for_seed){
      engine.seed(engine_for_seed());
    }
    void write_principal(ostream& out){
      node* n=this;
      while(n->best!=NULL){
        for(unsigned int i=0;i<Nchildren;i++)
          if(n->best==n->child[i]){
            out<<i<<" ";
            break;
          }
        n=n->best;
      }
      out<<endl;
    }
    int total_depth(){
      int temp,d=1;
      for(unsigned int i=0;i<Nchildren;i++){
        temp=child[i]->total_depth()+1;
        if(temp>d)
          d=temp;
      }
      return d;
    }
    int Nleaves(){
      int s=0;
      if(!Nchildren)
        return 1;
      for(unsigned int i=0;i<Nchildren;i++)
        s+=child[i]->Nleaves();
      return s;
    }
    void write_tree(ostream& out,bool principal=true){
      bool color=false;
      if(!parent){
        out<<"\\documentclass[varwidth=true]{standalone}\n"
           <<"\\usepackage{fancybox}\n"
           <<"\\usepackage{tikz}\n"
           <<"\\usepackage{tikz-qtree}\n"
           <<"\\begin{document}\n\n"
           <<"\\Ovalbox{\n"
           <<"\\begin{tikzpicture}[grow'=right,edge from parent path=(\\tikzparentnode\\tikzparentanchor)\n"
           <<" -- (\\tikzchildnode\\tikzchildanchor)]\n"
           <<"\\tikzset{level distance=50pt}\n"
           <<"\\tikzstyle{edge from parent}=[draw=none]\n"
           <<"\\tikzstyle{every node}=[draw=none]\n"
           <<"\\Tree";
        for(int i=0;i<total_depth();i++)
          out<<"[."<<((player+i)%2==BLACK?"Max":"Min")<<" ";
        for(int i=0;i<total_depth();i++)
          out<<"] ";
      out<<"\n"
         <<"\\begin{scope}[yshift=-"<<Nleaves()*20<<"pt]\n"
         <<"\\tikzstyle{every node}=[circle,draw]\n"
         <<"\\tikzstyle{edge from parent}=[draw,->]\n"
         <<"\\Tree";
      }
      else{
        if(parent->best==this)
          color=true;
        else principal=false;
        out<<endl;
        for(int i=0;i<depth-1;i++)
          out<<"\t";
      }
      if(principal && parent)
        out<<"\\edge[thick,color=red]; ";
      else if(color)
        out<<"\\edge[thick,color=green]; ";
      if(Nchildren)
        out<<"[.";
      if(principal)
        out<<"\\node[color=red]{";
      else if(color)
        out<<"\\node[color=green]{";
      if(determined())
        out<<"$"<<fixed<<setprecision(1)<<val<<"$";
      else if(opt>DRAW)
        out<<"$\\infty$";
      else if(opt<DRAW)
        out<<"$-\\infty$";
      else out<<"$0$";
      if(color || principal)
        out<<"};";
      if(Nchildren){
        child[0]->write_tree(out,principal);
        for(unsigned int i=1;i<Nchildren;i++){
          child[i]->write_tree(out,principal);
        }
        out<<endl;
        for(int i=0;i<depth-1;i++)
          out<<"\t";
        out<<"]";
      }
      if(!parent)
        out<<"\n"
           <<"\\end{scope}\n"
           <<"\\end{tikzpicture}\n"
           <<"}\n"
           <<"\\end{document}\n";
    }
    void write(ostream& out){
      b.write();
      out<<val<<"\t"<<opt<<"\t"<<pess<<"\t"<<(player==BLACK?"BLACK":"WHITE")<<"\t"<<Nchildren<<"\t"<<depth<<endl;
    }
    void expand(vector<zet> candidate){
      Nchildren=candidate.size();
      //if(game_status!=UNDETERMINED)
      //  browse();
      if(Nchildren>0){
        child=new node*[Nchildren];
        for(unsigned int i=0;i<Nchildren;i++){
          if(player==BLACK)
            child[i]=new node(b+candidate[i],val+candidate[i].val,!player,depth+1);
          else child[i]=new node(b+candidate[i],val-candidate[i].val,!player,depth+1);
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
    void backpropagate(node* changed){
      if(!update_opt(changed))
        get_opt();
      if(!update_pess(changed))
        get_pess();
      if(!changed->determined() && update_val(changed))
        best=changed;
      else get_val();
      if(parent)
        parent->backpropagate(this);
    }
    inline bool determined(){
      return opt==pess;
    }
    inline bool is_winning(){
      return (player==BLACK?(pess>DRAW):(opt<DRAW));
    }
    inline bool active(){
      if(!parent)
        return true;
      if(opt>=parent->pess)
        return true;
      return false;
    }
    inline bool update_val(node* c){
      if((player==BLACK && c->val>val)||(player==WHITE && c->val<val)){
        val=c->val;
        return true;
      }
      return false;
    }
    inline bool update_opt(node* c){
      if((player==BLACK && c->opt>opt)||(player==WHITE && c->opt<opt)){
        opt=c->opt;
        return true;
      }
      return false;
    }
    inline bool update_pess(node* c){
      if((player==BLACK && c->pess>pess)||(player==WHITE && c->pess<pess)){
        pess=c->pess;
        return true;
      }
      return false;
    }
    inline void get_opt(){
      opt=(player==BLACK?WHITE_WINS:BLACK_WINS);
      for(unsigned int i=0;i<Nchildren;i++)
        update_opt(child[i]);
    }
    inline void get_pess(){
      pess=(player==BLACK?WHITE_WINS:BLACK_WINS);
      for(unsigned int i=0;i<Nchildren;i++)
        update_pess(child[i]);
    }
    void get_best_determined(){
      vector<node*> candidate;
      if(player==BLACK){
        for(unsigned int i=0;i<Nchildren;i++)
          if(child[i]->pess==pess)
            candidate.push_back(child[i]);
      }
      else for(unsigned int i=0;i<Nchildren;i++)
        if(child[i]->opt==opt)
          candidate.push_back(child[i]);
      if(candidate.size()==1)
        best=candidate[0];
      else best=candidate[uniform_int_distribution<int>{0,(int) candidate.size()-1}(engine)];
    }
    void get_val(){
      val=(player==BLACK?-20000.0:20000.0);
      for(unsigned int i=0;i<Nchildren;i++)
        if(!child[i]->determined() && update_val(child[i]))
          best=child[i];
      for(unsigned int i=0;i<Nchildren;i++)
        if(child[i]->determined())
          update_val(child[i]);
    }
    node* select(){
      if(best)
        return best->select();
      return this;
    }
    zet bestmove(){
      if(!best)
        return zet(0,val,player);
      if(is_winning())
        get_best_determined();
      return zet(best->m,val,player);
    }
    void browse(){
      write(cout);
      string s;
      unsigned int i;
      node* n=this;
      cin>>s;
      if(s=="parent"){
        if(parent)
          n=parent;
        else cout<<"This is the root"<<endl;
      }
      else if(s=="best"){
        if(best){
          for(i=0;i<Nchildren;i++)
            if(child && child[i]==best)
              cout<<"Best is child "<<i<<endl;
          n=best;
        }
        else cout<<"This is a leaf"<<endl;
      }
      else if(s=="child"){
        cin>>i;
        if(child && i<Nchildren && child[i])
          n=child[i];
        else cout<<"Child does not exist"<<endl;
      }
      else if(s=="getval")
        get_val();
      else if(s=="select")
        n=select();
      else if(s=="done")
        n=NULL;
      else if(s=="root")
        while(n->parent)
          n=n->parent;
      else cout<<"Unknown input"<<endl;
      if(n)
        n->browse();
    }
};
}
#endif // BFS_H_INCLUDED
