#include "heuristic.h"
#include "heuristic_nhp.h"
#include "data_struct.h"
#include "bfs.h"

#include <fstream>
#include <ctime>

bool play_nhp_game(heuristic_nhp& h_black, heuristic_nhp& h_white, bool verbose = false){
  board b;
  zet m;
  vector<zet> candidates;
  while(!b.is_full()){
      m = h_black.makemove_bfs(b,BLACK);
      b.add(m);
      if(verbose)
        b.write();
      if(b.black_has_won())
        break;
      m = h_white.makemove_bfs(b,WHITE);
      b.add(m);
      if(verbose)
        b.write();
  }
  return b.black_has_won();
}

void test_nhp_agents(int N,int k){
  ofstream output("../nhp_tournament_results_with_lapse.txt",ios::out);
  heuristic_nhp h_black, h_white;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  h_black.seed_generator(global_generator);
  h_white.seed_generator(global_generator);
  const char* param_filename = "../params.txt";
  int i,j;
  for(int n=0;n<N*N;n+=k){
    i=n/N;
    j=n%N;
    h_black.get_params_from_file(param_filename,i);
    h_black.lapse_rate = (i%20==0?1:(i%20==1?0.5+0.5*h_black.lapse_rate:h_black.lapse_rate));
    h_black.update();
    h_black.c_self = 2.0;
    h_black.c_opp = 0.0;
    h_white.get_params_from_file(param_filename,j);
    h_white.lapse_rate = (j%20==0?1:(j%20==1?0.5+0.5*h_white.lapse_rate:h_white.lapse_rate));
    h_white.update();
    h_white.c_self = 0.0;
    h_white.c_opp = 2.0;
    output<<i<<"\t"<<j<<"\t"<<play_nhp_game(h_black,h_white,false)<<endl;
    cout<<i<<"\t"<<j<<endl;
  }
  output.close();
}


int main(int argc, char* argv[]){
  data_struct dat;
  heuristic h;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  //test_nhp_agents(1650,23);
  /*const char* direc = "C:/Users/svo/Documents/fmri/splits/";
  const char* board_filename = "C:/Users/svo/Documents/fmri/splits/boards_by_group.txt";
  dat.load_data_from_directory(direc,39);
  dat.save_board_file(board_filename);*/
  //compute_fmri_values(h,dat,200);

  //dat.load_board_file("C:/Users/svo/Documents/fmri/invalid_boards.csv",-1);
  //compute_fmri_values_invalid(h,dat,40);

  //ofstream output("eye_trace_model.txt",ios::out);
  //write_eye_trace(&h.game_tree,output);
  //output.close();
  return 0;
}
