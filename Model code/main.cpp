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
#include "heuristic_drop.h"

#include <fstream>
#include <ctime>
#include <sstream>

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
    board_list_mutex.unlock();
    m=h.makemove_bfs(dat->alltrials[i].b,dat->alltrials[i].player).zet_id;
    success=(m==dat->alltrials[i].m);
    board_list_mutex.lock();
    //if(success)
    //  cout<<i<<"\t"<<board_list->Nunsolved<<"\t"<<board_list->get_Ltot()<<endl;
  }
  board_list_mutex.unlock();
}

void compute_loglik_threads(heuristic& h,data_struct& dat,todolist& board_list,mt19937_64 &global_generator){
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

/*void compute_move_distribution(heuristic& h,const char* input_filename, const char* output_filename,int N){
  int hist[36];
  data_struct dat;
  ifstream input(input_filename,ios::in);
  ofstream output(output_filename,ios::out);
  uint64 bp,wp;
  string s1,s2;
  string line;
  board b;
  if(input.is_open()){
    while(!input.eof()){
      getline(input,line);
      if(line.size()>0){
        stringstream(line)>>s1>>s2;
        b=board(binstringtouint64(s1),binstringtouint64(s2));
        dat.add(b,0,b.active_player(),0.0,0,0);
      }
    }
    input.close();
  }
  else{
    cout<<"file not found: "<<input_filename<<endl;
  }
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int i=0;i<36;i++)
      hist[i]=0;
    //h.get_params_from_file(param_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    for(int n=0;n<N;n++){
      hist[uint64totile(h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player).zet_id)]++;
    }
    for(int i=0;i<36;i++)
      output<<((double) hist[i])/N<<"\t";
    output<<endl;
  }
  output.close();
}*/

void compute_model_prediction(heuristic& h,data_struct& dat, const char* param_filename, const char* output_filename, int N=1){
  ofstream output(output_filename,ios::out);
  for(int i=0;i<40;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,i,g);
      cout<<i<<"\t"<<g<<endl;
      for(unsigned int j=0;j<dat.Nboards;j++){
        //h.get_params_from_file(param_filename,dat.alltrials[j].player_id,dat.alltrials[j].group);
        for(int n=0;n<N;n++){
          output<< uint64totile(h.makemove_bfs(dat.alltrials[j].b,dat.alltrials[j].player).zet_id)<<"\t";
        }

      }
      output<<endl;
    }
  }
  output.close();
}

void compute_turing_logliks(heuristic& h,data_struct& dat, const char* param_filename, const char* output_filename, mt19937_64& global_generator){
  todolist* board_list;
  for(int i=0;i<40;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,i,g);
      cout<<i<<"\t"<<g<<endl;
      board_list = new todolist(dat.Nboards);
      board_list->set_output(output_filename);
      compute_loglik_threads(h,dat,*board_list,global_generator);
      cout<<board_list->get_Ltot()<<endl;
      delete board_list;
    }
  }
}



/*void compute_model_prediction(heuristic& h,data_struct& dat, const char* param_filename, const char* output_filename,int N){
  ofstream output(output_filename,ios::out);
  for(unsigned int j=0;j<dat.Nboards;j++){
    cout<<j<<endl;
    h.get_params_from_file(param_filename,dat.alltrials[j].player_id,dat.alltrials[j].group);
    for(int n=0;n<N;n++){
      while(h.makemove_bfs(dat.alltrials[j].b,dat.alltrials[j].player,true).zet_id!=dat.alltrials[j].m){
        //cout<<dat.alltrials[j].m<<"\t"<<h.game_tree->bestmove().zet_id<<endl;

        delete(h.game_tree);
        h.game_tree = NULL;
      };
      cout<<n<<endl;
      output<<h.game_tree->get_depth_of_pv()<<"\t"<<std::flush;
      delete(h.game_tree);
      h.game_tree = NULL;
    }
    //for(int n=0;n<N;n++){
    //  output<<uint64totile(h.makemove_bfs(dat.alltrials[j].b,dat.alltrials[j].player).zet_id)<<"\t";
    //}
    output<<endl;
  }
  output.close();
}*/
template<typename T>
void print_queue(T q) { // NB: pass by value so the print uses a copy
    while(!q.empty()) {
        std::cout << q.top().i << ' ';
        q.pop();
    }
    std::cout << '\n';
}


int main(int argc, char* argv[]){
  data_struct dat;
  heuristic h;
  mt19937_64 global_generator;
  global_generator.seed(unsigned(time(0)));
  h.seed_generator(global_generator);
  //const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Peak/peak_vs_experiment_agents.txt";
  //int ranks[10][2]={{5,10},{20,40},{50,70},{80,100},{110,130},{140,160},{150,165},{170,199},{175,185},{180,199}};
  //calculate_peak_ratings(output_filename,ranks,10,10);

  //test_nhp_agents(200,1);
  //const char* direc = "C:/Users/svo/Documents/fmri/splits/";
  //const char* param_filename1 = "C:/Users/svo/Google Drive/Bas Games/Analysis/Params/params_hvh_final.txt";
  const char* param_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/params_drop.txt";
  //const char* param_filename = "C:/Users/svo/Documents/fmri/params_fmri_final.txt";
  const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Model prediction/prediction_eye_final.txt";
  //const char* input_filename = "C:/Users/svo/Documents/peak/splits/1/1.csv";
  //dat.load_board_file(input_filename);
  //const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Ratings/tournament_results_short.txt";
  //const char* param_filename = "C:/Users/svo/Documents/Sourcetree repos/fourinarow/params_sorted_by_elo.txt";
  const char* board_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/data_hvh.txt";
  //const char* fmri_inds_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/fmri_board_inds.txt";
  //const char* output_filename1 = "C:/Users/svo/Google Drive/Bas Games/Analysis/Loglik/move_dist_tree.txt";
  //const char* output_filename = "C:/Users/svo/Google Drive/Bas Games/Analysis/Turing/Turing_logliks_random.txt";
  //test_agents(param_filename,output_filename,200,1);
  //ofstream output(output_filename);
  dat.load_board_file(board_filename);
  //cout<<dat.Nboards<<endl;
  //compute_turing_logliks(h,dat,param_filename,output_filename,global_generator);
  /*board b;
  int x,y;
  while(!b.game_has_ended()){
    b.write();
    b.add(h.makemove_bfs(b,BLACK));
    b.write();
    do{
        cin>>x>>y;
    } while (!b.isempty(zet(x,y,0.0,WHITE)));
    b.add(x,y,WHITE);
    b.write();
  }*/
  heuristic_drop h_drop;
  int j =5250;
  h_drop.get_params_from_file(param_filename,dat.alltrials[j].player_id,dat.alltrials[j].group);
  cout<<h_drop.center_weight<<endl;
  while(true){
    dat.alltrials[j].b.write(h_drop.makemove_bfs(dat.alltrials[j].b,dat.alltrials[j].player).zet_id);
    cin.get();
  }



  //compute_move_distribution(h,dat,param_filename1,output_filename1,ind,10,1000);
  //for(unsigned int j=0;j<dat.Nboards;j++){
  //  h.get_params_from_sliders(1.0,1.0,1.0,1.0,0.5);
  //  dat.alltrials[j].b.write(h.makemove_bfs(dat.alltrials[j].b,dat.alltrials[j].player).zet_id);
  //  cin.get();
  //}

  //compute_model_prediction(h,dat,param_filename,output_filename,10);
  /*todolist* board_list = new todolist(100);
  int i=-1;
  bool success=false;
  while(board_list->get_next(i,success)){
    success=bernoulli_distribution{1.0/sqrt(i)}(global_generator);
    if(success || board_list->iterations %100000000000000==0){
      cout<<i<<"\t"<<board_list->Nunsolved<<"\t"<<board_list->get_Ltot()<<"\t"<<board_list->tasklist.size()<<endl;
    }
    if(board_list->tasklist.size()==NTHREADS)
      print_queue(board_list->tasklist);
  }*/

  /*int j;
  ifstream input(fmri_inds_filename,ios::in);
  ofstream output(output_filename,ios::out);
  for(int i=0;i<250;i++){
    input>>j;
    bool player=dat.alltrials[j].b.active_player();
    double vself=0.0,vopp=0.0;
    for(uint64 m=1;m!=boardend;m<<=1){
      if(dat.alltrials[j].b.contains(m,player))
        vself+=h.vtile[m];
      if(dat.alltrials[j].b.contains(m,!player))
        vopp+=h.vtile[m];
    }
    output<<vself<<"\t"<<vopp<<endl;
  }
  input.close();
  output.close();*/

  //const char* input_filename = "C:/Users/svo/Documents/FourinarowData/Data/jiawen/splits/1/boards_e.csv";
  //const char* output_filename = "C:/Users/svo/Documents/FourinarowData/Data/jiawen/splits/1/test.csv";
  //compute_move_distribution(h,input_filename,output_filename,100);
  //dat.load_board_file("C:/Users/svo/Documents/fmri/invalid_boards.csv",-1);
  //compute_fmri_values_entropy(h,dat,param_filename,output_filename,100);

  //ofstream output("eye_trace_model.txt",ios::out);
  //write_eye_trace(&h.game_tree,output);
  //output.close();
  return 0;
}
