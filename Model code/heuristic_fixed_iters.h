#ifndef HEURISTIC_FIXED_ITERS_H_INCLUDED
#define HEURISTIC_FIXED_ITERS_H_INCLUDED

#include "board.h"
#include "heuristic.h"
using namespace std;

class heuristic_fixed_iters: public heuristic{
    public:
        zet makemove_bfs(board,bool,bool=false, bool=false);
};

#endif // HEURISTIC_FIXED_ITERS_H_INCLUDED
