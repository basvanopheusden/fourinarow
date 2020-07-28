#ifndef DATA_STRUCT_H_INCLUDED
#define DATA_STRUCT_H_INCLUDED

#include "board.h"
#include <vector>
#include <random>

#define AFC2 101
#define EVAL 102

struct trial{
  board b;
  uint64 m;
  bool player;
  double thinking_time;
  int player_id;
  int group;
  trial(board b_val, uint64 m_val, bool player_val, double t_val, unsigned int p_val, int g_val=0){
    b=b_val;
    m=m_val;
    player=player_val;
    thinking_time=t_val;
    player_id=p_val;
    group=g_val;
  }
};

struct trial_eval{
  board b;
  bool player;
  double thinking_time;
  int player_id;
  int score;
  int opt_score;
  int group;
  trial_eval(board b_val, int s_val, int s_opt_val, bool c_val, double t_val, unsigned int p_val, int g_val=0){
    b=b_val;
    thinking_time=t_val;
    player=c_val;
    score=s_val;
    opt_score=s_opt_val;
    player_id=p_val;
    group=g_val;
  }
};

struct trial_2afc{
  board b;
  bool player;
  uint64 m,m1,m2;
  int v1,v2;
  double thinking_time;
  int player_id;
  int group;
  trial_2afc(board b_val, uint64 m_val, uint64 m1_val, uint64 m2_val, bool player_val, int v1_val, int v2_val, double t_val, unsigned int p_val, int g_val=0){
    b=b_val;
    m=m_val;
    m1=m1_val;
    m2=m2_val;
    player=player_val;
    v1=v1_val;
    v2=v2_val;
    thinking_time=t_val;
    player_id=p_val;
    group=g_val;
  }
};

struct data_struct{
  unsigned int Nplayers;
  unsigned int Nboards, Nboards_2afc, Nboards_eval;
  vector<trial> alltrials;
  vector<trial_2afc> alltrials_2afc;
  vector<trial_eval> alltrials_eval;
  data_struct(): Nplayers(0), Nboards(0){};
  void add(board, uint64, bool, double, unsigned int,int);
  void add_2afc(board, uint64, uint64, uint64, bool, int, int, double, unsigned int,int);
  void add_eval(board, int, int, bool, double, unsigned int,int);
  void clear();
  void make_groups(int, mt19937_64);
  vector<unsigned int> select_boards(int,int);
  vector<unsigned int> select_boards(int,int,int);
  void load_data_from_directory(const char*,int);
  void save_board_file(const char*);
  void load_board_file(const char*,int=-1);
};

#endif // DATA_STRUCT_H_INCLUDED
