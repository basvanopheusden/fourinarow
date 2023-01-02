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
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,dat.alltrials[i].player_id,g);
      output<<compute_entropy(h,dat.alltrials[i].b,dat.alltrials[i].player,N)<<"\t";
      cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
    }
    output<<endl;
  }
  output.close();
}

void compute_fmri_iters(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
  ofstream output(output_filename,ios::out);
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,dat.alltrials[i].player_id,g);
      h.stopping_thresh=50.5;
      n=0;
      while(n<N){
        m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player,true);
        if(m.zet_id==dat.alltrials[i].m){
          output<<h.game_tree->get_num_internal_nodes()<<"\t";
          n++;
        }
        delete(h.game_tree);
      }
      cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
    }
    output<<endl;
  }
  output.close();
}

void compute_fmri_values_pv_depth(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
  ofstream output(output_filename,ios::out);
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,dat.alltrials[i].player_id,g);
      h.stopping_thresh=50.5;
      n=0;
      while(n<N){
        m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player,true);
        if(m.zet_id==dat.alltrials[i].m){
          output<<h.game_tree->get_depth_of_pv()<<"\t";
          n++;
        }
        delete(h.game_tree);
      }
      cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
    }
    output<<endl;
  }
  output.close();
}

void compute_fmri_values(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
ofstream output(output_filename,ios::out);
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,dat.alltrials[i].player_id,g);
      n=0;
      while(n<N){
        m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player);
        if(m.zet_id==dat.alltrials[i].m){
          output<<m.val<<"\t";
          n++;
        }
      }
      cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
    }
    output<<endl;
  }
  output.close();
}

void compute_fmri_values_myopic(heuristic& h, data_struct& dat, const char* param_filename, const char* output_filename, int N){
  ofstream output(output_filename,ios::out);
  zet m;
  int n;
  for(unsigned int i=0;i<dat.Nboards;i++){
    for(int g=1;g<=5;g++){
      h.get_params_from_file(param_filename,dat.alltrials[i].player_id,g);
      n=0;
      while(n<N){
        m=h.makemove_bfs(dat.alltrials[i].b,dat.alltrials[i].player,false,true);
        if(m.zet_id==dat.alltrials[i].m){
          output<<h.evaluate(dat.alltrials[i].b)<<"\t";
          n++;
        }
        h.restore_features();
      }
      cout<<i<<"\t"<<dat.alltrials[i].player_id<<endl;
    }
    output<<endl;
  }
  output.close();
}