#include "../heuristic.h"
#include "../board_list.h"
#include "../data_struct.h"
#include "mex.h"
#include "matrix.h"

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
    if(success)
      cout<<i<<"\t"<<board_list->Nunsolved<<"\t"<<board_list->get_Ltot()<<endl;
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

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]){
  heuristic h;
  random_device rd;
  data_struct dat;
  mt19937_64 global_generator(rd());
  const mwSize *dims = mxGetDimensions(prhs[0]);
  const unsigned int N = dims[1];
  board b;
  bool player;
  uint64 m;
  double* paramptr=mxGetPr(prhs[1]);
  int* times = mxGetInt32s(prhs[2]);

  //cout<<N<<endl;

  h.get_params_from_array(paramptr);
  h.seed_generator(global_generator);

  plhs[0] = mxCreateDoubleMatrix(1, N, mxREAL);
  double* output = (double*) mxGetData(plhs[0]);
  todolist board_list(N,times);

  for(unsigned int i=0;i<N;i++){
    //cout<<i<<endl;
    b.pieces[BLACK] = *mxGetUint64s(mxGetCell(prhs[0],i*4));
    b.pieces[WHITE] = *mxGetUint64s(mxGetCell(prhs[0],i*4+1));
    player = *mxGetLogicals(mxGetCell(prhs[0],i*4+2));
    m = *mxGetUint64s(mxGetCell(prhs[0],i*4+3));
    dat.add(b,m,player,0.0,0,0);
  }
  compute_loglik_threads(h,dat,board_list,global_generator);
  for(int i=0;i<N;i++){
    output[i] = board_list.data[i].L;
  }
}
