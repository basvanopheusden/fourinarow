#include "heuristic.h"
#include "heuristic_nhp.h"
#include "data_struct.h"
#include "bfs.h"
#include "utils.h"
#include "board_list.h"
#include "Peak code/heuristic.h"
#include "Experiment agent code/heuristic_old.h"
#include "heuristic_fixed_branch.h"
#include "heuristic_fixed_iters.h"

#include <fstream>
#include <ctime>


#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
using namespace std;

mutex board_list_mutex;

void worker_thread(heuristic h,data_struct* dat,todolist* board_list, int seed){
  int i=-1;
  bool success=false;
  uint64 m;
  board_list_mutex.lock();
  h.engine.seed(seed);
  while(board_list->get_next(i,success)){
    if(i<0 || i>=213){
      cout<<"wtf"<<endl;
      cin.get();
    }
    board_list_mutex.unlock();
    m=h.makemove_bfs(dat->alltrials[i].b,dat->alltrials[i].player).zet_id;
    success=(m==dat->alltrials[i].m);
    board_list_mutex.lock();
    //if(success)
    //  cout<<i<<"\t"<<board_list->Nunsolved<<"\t"<<board_list->get_Ltot()<<endl;
  }
  board_list_mutex.unlock();
}

void compute_loglik_threads(heuristic h,data_struct& dat,todolist& board_list,mt19937_64 &global_generator){
  thread t[NTHREADS];
  for(int i=0;i<NTHREADS;i++)
    t[i]=thread(worker_thread,h,&dat,&board_list,global_generator());
  for(int i=0;i<NTHREADS;i++)
    t[i].join();
}

void calculate_peak_ratings(const char* output_filename, int ranks[][2], const int Nranks, const int N){
  peak::heuristic hpeak;
  old::heuristic hold;
  random_device rd;  //Will be used to obtain a seed for the random number engine
  mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  board b;
  zet m;
  bool player;
  int opp;
  ofstream output(output_filename,ios::out);
  for(int k=0;k<N;k++){
  for(int i=0;i<Nranks;i++){
    for(int j=0;j<30;j++){
      hold.get_params_from_file("C:/Users/svo/Documents/Sourcetree repos/fourinarow/Model code/Experiment agent code/params_bfs_new.txt",j);
      b.reset();
      player=BLACK;
      while(!b.game_has_ended()){
        if(((k%2==0) && (player==BLACK)) || ((k%2==1) && (player==WHITE))){
          m=hold.makemove(b,player);
        }
        else{
          opp=std::uniform_int_distribution<int>{ranks[i][0],ranks[i][1]}(gen);
          hpeak.get_params(peak::params[opp]);
          m = hpeak.makemove_bfs(b,player);
        }
        b=b+m;
        player=!player;
      }
      if(k%2==0)
        output<<"comp0"<<(j<10?"0":"")<<j<<"\tpeak_opp_"<<ranks[i][0]<<"_"<<ranks[i][1]<<"\t"<<(b.black_has_won()?1:(b.white_has_won()?-1:0))<<endl;
      else
        output<<"peak_opp_"<<ranks[i][0]<<"_"<<ranks[i][1]<<"\tcomp0"<<(j<10?"0":"")<<j<<"\t"<<(b.black_has_won()?1:(b.white_has_won()?-1:0))<<endl;
      cout<<i<<"\t"<<j<<"\t"<<k<<endl;
    }
  }
  }
  output.close();
}

int play_game(heuristic& h_black, heuristic& h_white, bool verbose = false){
  board b;
  zet m;
  bool player = BLACK;
  while(!b.game_has_ended()){
      if(player==BLACK)
        m = h_black.makemove_bfs(b,BLACK);
      else
        m = h_white.makemove_bfs(b,WHITE);
      player=!player;
      b.add(m);
      if(verbose)
        b.write();
  }
  if(b.black_has_won())
    return 1;
  else if(b.is_full())
    return 0;
  return -1;
}

void test_agents(const char* param_filename, const char* output_filename, int N,int k){
  ofstream output(output_filename,ios::out);
  heuristic h_black, h_white;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  h_black.seed_generator(global_generator);
  h_white.seed_generator(global_generator);
  int i,j;
  for(int n=0;n<N*N;n+=k){
    i=n/N;
    j=n%N;
    h_black.get_params_from_file(param_filename,i);
    h_white.get_params_from_file(param_filename,j);
    output<<i<<"\t"<<j<<"\t"<<play_game(h_black,h_white,false)<<endl;
    cout<<i<<"\t"<<j<<endl;
  }
  output.close();
}


int main(int argc, char* argv[]){
  data_struct dat;
  heuristic_fixed_iters h;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  //const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Peak/peak_vs_experiment_agents.txt";
  //int ranks[10][2]={{5,10},{20,40},{50,70},{80,100},{110,130},{140,160},{150,165},{170,199},{175,185},{180,199}};
  //calculate_peak_ratings(output_filename,ranks,10,10);

  //test_nhp_agents(1650,23);
  //const char* direc = "C:/Users/svo/Documents/fmri/splits/";
  const char* param_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Params/params_learn_final_fixed_branch.txt";
  //const char* param_filename = "C:/Users/svo/Documents/fmri/params_fmri_final.txt";
  //const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Model prediction/prediction_hvh.txt";
  //const char* input_filename = "C:/Users/svo/Documents/peak/splits/1/1.csv";
  //dat.load_board_file(input_filename);
  const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Ratings/tournament_results_short.txt";
  //const char* param_filename = "C:/Users/svo/Documents/Sourcetree repos/fourinarow/params_sorted_by_elo.txt";
  const char* board_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/data_hvh.txt";
  //test_agents(param_filename,output_filename,200,1);
  //ofstream output(output_filename);
  dat.load_board_file(board_filename);
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(param_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    for(int n=0;n<100;n++){
      cout<<uint64totile(h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player).zet_id)<<"\t";
    }
    //output<<endl;
    cout<<i<<endl;
  }
  //dat.load_board_file("C:/Users/svo/Documents/fmri/invalid_boards.csv",-1);
  //compute_fmri_values_entropy(h,dat,param_filename,output_filename,100);

  //ofstream output("eye_trace_model.txt",ios::out);
  //write_eye_trace(&h.game_tree,output);
  //output.close();
  return 0;
}
