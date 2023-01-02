#ifndef HEURISTIC_DROP_H_INCLUDED
#define HEURISTIC_DROP_H_INCLUDED

#include "board.h"
#include "heuristic.h"
using namespace std;

class heuristic_drop: public heuristic{
    public:
        zet makemove_bfs(board,bool,bool=false, bool=false);
		vector<zet> get_moves(board&, bool, bool=false);
};

#endif // HEURISTIC_DROP_H_INCLUDED
