#include "heuristic.h"
#include "data_struct.h"

int main(int argc, char* argv[]){
  data_struct dat;
  heuristic h;
  random_device rd;
  mt19937_64 global_generator;
  global_generator.seed(rd());
  h.seed_generator(global_generator);
  zet m;
  char* direc = "C:/Users/Bas/Downloads/splits";
  char* params_filename = "C:/Users/Bas/Google Drive/Bas Games/Analysis/Params/params_peak_final.txt";
  dat.load_data_from_directory(direc,10);
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(params_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player);
    dat.alltrials[i].b.write(m);
  }
}
