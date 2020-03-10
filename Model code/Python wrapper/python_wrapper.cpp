#include "../board.h"
#include "../heuristic.h"
#include "../data_struct.h"
#include <fstream>

extern "C" {

int makemove(int seed, char* bp, char* wp, bool player, double* params){
  heuristic h;
  zet m;
  mt19937_64 generator;
  ofstream output("c++_output.txt",ios::out);
  cout<<"hi"<<endl;
  board b(binstringtouint64(bp),binstringtouint64(wp));
  output<<seed<<"\t"<<bp<<"\t"<<wp<<"\t"<<player<<endl;
  generator.seed(seed);
  h.seed_generator(generator);
  h.get_params_from_array(params);
  m=h.makemove_bfs(b,player);
  output.close();
  return uint64totile(m.zet_id);
}

}
