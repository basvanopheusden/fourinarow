#ifndef BFS_H_INCLUDED
#define BFS_H_INCLUDED

#include "board.h"
#include <vector>
using namespace std;

namespace bfs{

class node{
  public:
    node** child;
    node* parent;
    node* best;
    unsigned int Nchildren;
    uint64 m;
    board b;
    double val;
    int pess;
    int opt;
    bool player;
    int depth;
    int iteration;
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
      iteration=-1;
    }
    virtual ~node(){
      if(child){
        for(unsigned int i=0;i<Nchildren;i++)
          delete child[i];
        delete[] child;
      }
      parent=NULL;
      best=NULL;
    }

    node(board,double,bool,int);
    virtual void expand(vector<zet>,int);
    void backpropagate(node*);
    bool determined();
    bool update_val(node*);
    bool update_opt(node*);
    bool update_pess(node*);
    virtual void get_opt();
    virtual void get_pess();
    void get_best_determined();
    void get_val();
    node* select();
    zet bestmove();
};

}

#endif // BFS_H_INCLUDED
