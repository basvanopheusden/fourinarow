#ifndef BFS_NHP_H_INCLUDED
#define BFS_NHP_H_INCLUDED

#include "board.h"
#include "bfs.h"
using namespace std;

namespace bfs{

class nhp_node: public node {
    public:
        nhp_node(board,double,bool,int);
        void get_opt();
        void get_pess();
        void expand(vector<zet> ,int );
};

}


#endif // BFS_NHP_H_INCLUDED
