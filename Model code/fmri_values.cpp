#include "heuristic.h"
#include "data_struct.h"

#include <fstream>
#include <ctime>

double compute_entropy(heuristic& h,board b,bool player,int N){
  double s=0.0;
  uint64 m;
  int k;
  for(int i=0;i<N;i++){
    m=h.makemove_bfs(b,player).zet_id;
    k=1;
    while(h.makemove_bfs(b,player).zet_id!=m){
      s+=1.0/(k*N);
      k++;
    }
  }
  return s;
}

void compute_fmri_values_entropy(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
  ofstream output(output_filename,ios::out);
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(param_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    output<<compute_entropy(h,dat.alltrials[i].b,dat.alltrials[i].player,N)<<endl;
    cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
  }
  output.close();
}

void compute_fmri_values_pv_depth(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
  ofstream output(output_filename,ios::out);
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(param_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    h.stopping_thresh=50.5;
    n=0;
    while(n<N){
      m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player,true);
      if(m.zet_id==dat.alltrials[i].m){
        output<<h.game_tree->get_depth_of_pv()<<(n==N-1?"\n":"\t");
        n++;
      }
      delete(h.game_tree);
    }
    cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
  }
  output.close();
}

void compute_fmri_values(heuristic& h, data_struct& dat, int N){
  ofstream output("C:/Users/svo/Documents/fmri/fmri_board_values_conditioned.txt",ios::out);
  const char* param_filename = "C:/Users/svo/Documents/fmri/params_fmri_final.txt";
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    h.get_params_from_file(param_filename,dat.alltrials[i].player_id,dat.alltrials[i].group);
    n=0;
    while(n<N){
      m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player);
      if(m.zet_id==dat.alltrials[i].m){
        output<<m.val<<(n==N-1?"\n":"\t");
        n++;
      }
    }
    cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
  }
  output.close();
}

void compute_fmri_values_invalid(heuristic& h, data_struct& dat, int N){
  ofstream output("C:/Users/svo/Documents/fmri/fmri_board_values_invalid_notree.txt",ios::out);
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int n=0;n<N;n++){
        h.get_params_from_file("C:/Users/svo/Documents/fmri/params_fmri_final.txt",dat.alltrials[i].player_id,(5*n)/N);
        h.gamma = 1.0;
        h.update();
        output<<h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player).val<<(n==N-1?"\n":"\t");
    }
    cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
  }
  output.close();
}
