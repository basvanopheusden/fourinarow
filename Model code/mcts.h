#include "board.h"
#include "heuristic.h"
#include <vector>

#define WHITE_WINS_GAME -1
#define BLACK_WINS_GAME 1
#define GAME_DRAWN 0

namespace MCTS{

class node{
  private:
    unsigned int Nvisits,Nwins,Ndraws;
    node* parent;
    node** child;
    unsigned int Nchildren;
  public:
    board b;
    bool player;
    uint64 m;
    node* select(double);
    void expand(vector<zet>);
    void backpropagate(int);
    double score();
    zet bestmove();
    node(board,bool);
    ~node();
};

}
