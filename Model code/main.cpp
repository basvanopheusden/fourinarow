#include "heuristic.h"
#include "data_struct.h"

int main(int argc, char* argv[]){
  data_struct dat;
  heuristic h;
  random_device rd;
  mt19937_64 global_generator;
  global_generator.seed(rd());
  h.seed_generator(global_generator);
  char* data_filename = "C:/Users/Bas/Documents/Google Drive/Bas Games/peak_moves.csv";
  char* params_filename = "C:/Users/Bas/Documents/Google Drive/Bas Games/Analysis/params_peak_final.csv";
  dat.load_board_file_ionatan(data_filename);
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(params_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    cout<<h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player).zet_id<<endl;
  }
}
