#ifndef BOARD_LIST_H_INCLUDED
#define BOARD_LIST_H_INCLUDED

#include <vector>
#include <queue>
#include <map>
#include <fstream>

#ifndef _WIN64
#define NTHREADS 20
#else
#define NTHREADS 8
#endif

struct task{
  int i;
  unsigned int priority;
  task(int i_val, unsigned int p_val): i(i_val), priority(p_val){}
  bool operator< (const task& other) const {return priority>other.priority;}
};

struct task_data{
  double L;
  unsigned int tries;
  unsigned int times;
  unsigned int times_left;
  bool done;
  task_data(unsigned int t=1): L(0.0), tries(1), times(t), times_left(t), done(false){}
  void update_success(){
    times_left--;
    if(!times_left)
      done=true;
    else tries=1;
  }
  void update_fail(){
    L+=1.0/(tries*times);
    tries++;
  }
};

struct todolist{
  unsigned int N;
  unsigned int Nunsolved;
  unsigned int iterations;
  const double cutoff=3.5;
  const double expt_factor=1.0;
  double Lexpt;
  bool verbose;
  priority_queue<task> tasklist;
  map<int,task_data> data;
  ofstream output;
  todolist(): N(0), Nunsolved(0), iterations(0), Lexpt(0.0), verbose(false){}
  todolist(const int N_val): N(N_val), Nunsolved(N), iterations(0), Lexpt(N*expt_factor), verbose(false){
    for(unsigned int i=0;i<N;i++){
      make_task(i,1);
      add_task(i);
    }
  }
  todolist(const int N_val,int* times): N(N_val), Nunsolved(0),
           iterations(0), Lexpt(N*expt_factor), verbose(false){
    for(unsigned int i=0;i<N;i++){
      make_task(i,(unsigned int) times[i]);
      add_task(i);
      Nunsolved+=times[i];
    }
  }
  void set_output(char* filename){
    output.open(filename,ios::out | ios::app);
    verbose=true;
  }
  ~todolist(){
    if(output.is_open())
      output.close();
  }
  void make_task(int i,unsigned int times=1){
    data[i]=task_data(times);
  }
  void add_task(int i){
    tasklist.emplace(i,data[i].tries);
  }
  bool get_task(int& i){
    while(!tasklist.empty()){
      i=tasklist.top().i;
      tasklist.pop();
      if(!data[i].done)
        return true;
    }
    return false;
  }
  void task_completed(int i,bool success){
    int tries;
    if(success){
      Nunsolved--;
      Lexpt-=expt_factor/data[i].times;
      tries=data[i].tries;
      data[i].update_success();
      if(verbose){
        if(output.is_open())
          output<<i<<"\t"<<iterations<<"\t"<<tries<<"\t"<<Lexpt/N<<"\t"<<Nunsolved<<endl;
        else cout<<i<<"\t"<<iterations<<"\t"<<tries<<"\t"<<Lexpt/N<<"\t"<<Nunsolved<<endl;
      }
    }
    else {
      Lexpt+=expt_factor/(data[i].times*data[i].tries);
      data[i].update_fail();
    }
    if(!data[i].done)
      add_task(i);
  }
  bool get_next(int& i,bool success){
    iterations++;
    if(i!=-1 && !data[i].done)
      task_completed(i,success);
    if(stopping_time()){
      //cout<<"cutoff reached"<<endl;
      return false;
    }
    if(!get_task(i))
      return false;
    if(Nunsolved<=NTHREADS)
      add_task(i);
    return true;
  }
  bool stopping_time(){
    return (Lexpt>cutoff*N);
  }
  void write_L(){
    for(map<int,task_data>::iterator it=data.begin();it!=data.end();++it)
      if(it->second.done)
        output<<it->first<<"\t"<<it->second.L<<endl;
  }
  double get_Ltot(){
    double Ltot=0.0;
    for(map<int,task_data>::iterator it=data.begin();it!=data.end();++it){
      if(!it->second.done)
        return Lexpt/N;
      Ltot+=it->second.L;
    }
    return Ltot/N;
  }
};


#endif // BOARD_LIST_H_INCLUDED
