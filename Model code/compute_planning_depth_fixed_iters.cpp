#include "heuristic.h"
#include "heuristic_fixed_iters.h"
#include "data_struct.h"
#include "utils.h"

#include <fstream>
#include <ctime>

void compute_planning_depth(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int player, int group, int N){
  ofstream output(output_filename,ios::out);
  h.get_params_from_file(param_filename,player,group);
  h.noise=normal_distribution<double> (0.0,0.0);
  double s;
  for(unsigned int i=0;i<dat.Nboards;i++){
    s=0.0;
    for(int n=0;n<N;n++){
      h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player,true,false);
      s+=h.game_tree->get_depth_of_pv();
      delete(h.game_tree);
    }
    output<<s/N<<endl;    
    cout<<i<<"\t"<<s/N<<endl;
  }
  output.close();
}

int main(int argc, const char* argv[]){
  data_struct dat;
  heuristic_fixed_iters h;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  const char* input_filename = argv[1];
  const char* param_filename = argv[2];
  int player = atoi(argv[3]);
  int group = atoi(argv[4]);
  const char* output_filename = argv[5];
  int N = atoi(argv[6]);
  dat.load_board_file(input_filename);
  cout<<dat.Nboards<<endl;
  compute_planning_depth(h,dat,param_filename,output_filename,player,group,N);
  return 0;
}
