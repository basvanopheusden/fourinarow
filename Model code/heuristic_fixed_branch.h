#ifndef HEURISTIC_FIXED_BRANCH_H_INCLUDED
#define HEURISTIC_FIXED_BRANCH_H_INCLUDED

#include "board.h"
#include "heuristic.h"
using namespace std;

class heuristic_fixed_branch: public heuristic{
    public:
        void get_params_from_array(double*);
        vector<zet> get_pruned_moves(board&, bool);
};

#endif // HEURISTIC_FIXED_BRANCH_H_INCLUDED
