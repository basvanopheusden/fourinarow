#ifndef HEURISTIC_NHP_H_INCLUDED
#define HEURISTIC_NHP_H_INCLUDED

#include "board.h"
#include "heuristic.h"
#include "bfs_nhp.h"
using namespace std;

class heuristic_nhp: public heuristic{
    public:
        zet makemove_bfs(board,bool,bool=false);
        zet makemove_bfs(board,uint64,uint64,bool,bool=false);

};

#endif // HEURISTIC_H_INCLUDED
