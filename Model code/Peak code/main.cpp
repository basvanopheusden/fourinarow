#include "board.h"
#include "heuristic.h"
#include "bfs.h"
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <fstream>
using namespace std;
using namespace peak;

const int N_opps = 200;

struct moveResponse
{
    int index;
    int wait_time;
};

enum gameEndResponse
{
    gameEndResponseBlackWin,
    gameEndResponseWhiteWin,
    gameEndResponseDraw,
    gameEndResponseIncomplete
};

extern "C" {

    moveResponse makemoveresponse(int seed, string bp, string wp, bool player, int opp_num) {

        moveResponse output;
        heuristic h;
        zet m;
        mt19937_64 generator;
        board b(binstringtouint64(bp),binstringtouint64(wp));
        double base_time;

        generator.seed(seed);
        h.seed_generator(generator);
        if(opp_num>=0 && opp_num<N_opps)
            h.get_params(params[opp_num]);
        m = h.makemove_bfs(b,player);

        cout<<bp<<"\t"<<wp<<endl;

        base_time = 3000.0 * sqrt(h.iterations * h.gamma ) + 500.0;

        output.wait_time = (h.iterations==0) ? 2000.0 : (8000.0-log(1.0+exp((8000.0-1.2*base_time)/2000.0))*2000.0);

        b.write(m);
        b.add(m);

        output.index = uint64totile(m.zet_id);
        return output;
    }

    gameEndResponse evaluateGameEnd(string bp, string wp)
    {
        board b (binstringtouint64(bp), binstringtouint64(wp));
        cout<<uint64tobinstring(b.pieces[BLACK])<<endl<<uint64tobinstring(b.pieces[WHITE])<<endl;

        if (is_win(b.pieces[BLACK]))
        {
            cout<<"win"<<endl;
            return gameEndResponseBlackWin;
        }
        else if (is_win(b.pieces[WHITE]))
        {
            cout<<"win"<<endl;
            return gameEndResponseWhiteWin;
        }
        else if(b.is_full())
        {
            cout<<"draw"<<endl;
            return gameEndResponseDraw;
        }

        return gameEndResponseIncomplete;
    }

    int makemove(int seed, string bp, string wp, bool player, int opp_num) {
        return makemoveresponse(seed, bp, wp, player, opp_num).index;
    }
}

