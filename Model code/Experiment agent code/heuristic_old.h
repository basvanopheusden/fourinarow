#ifndef HEURISTICOLD_H_INCLUDED
#define HEURISTICOLD_H_INCLUDED
//#define DEFAULT_WEIGHTS

#include "../board.h"
#include <random>
#include <vector>
using namespace std;

namespace old{
struct pattern{
  uint64 pieces;
  uint64 pieces_empty;
  int n;
  double weight_act;
  double weight_pass;
  int weightind_act;
  int weightind_pass;
  pattern(uint64,uint64,int,double*,int,int);
  bool is_active(board&);
  bool contained(board&);
  bool contained(board&,bool);
  bool just_active(board&);
  bool isempty(uint64);
  uint64 missing_pieces(board&,bool);
  void write(ostream&);
  double diff_act_pass();
  void update(double*);
};

class heuristic{
  public:
    static const unsigned int Nweights=17;
    double weight[Nweights];
    unsigned int Nfeatures;
    double D0,K0;
    double gamma,delta,lapse_rate;
    double vert_scale,diag_scale,opp_scale;
    bool self;
    int depth;
    unsigned int K;
    int iterations;
    vector<pattern> feature;
    vector<pattern> feature_backup;
    mt19937_64 engine;
    geometric_distribution<int> iter_dist;
    normal_distribution<double> noise;
    bernoulli_distribution attention;
    bernoulli_distribution lapse;
    bernoulli_distribution K_dist;
    bernoulli_distribution D_dist;
    heuristic();
    void get_params_from_file(char*,int);
    void get_params_from_matlab(double*);
    void get_features_from_file(char*);
    void addfeature(uint64, int , int );
    void addfeature(uint64, uint64, int , int ,int);
    void update();
    void seed_generator(mt19937_64);
    void perturb_weights(double);
    void update_weights();
    double evaluate(board, zet);
    double evaluate(board);
    void write(ostream&);
    void write_to_header(char*);
    vector<zet> get_pruned_moves(board&, bool);
    vector<zet> get_moves(board&, bool, bool);
    void check_moves(vector<zet>,board);
    double likelihood(board, zet);
    zet makerandommove(board,bool);
    zet makemove(board,bool);
    zet makemove_bfs(board,bool);
    zet makemove_notree(board,bool);
    zet makemove_notree_noatt(board,bool);
    void remove_features();
    void restore_features();
    void scale_lines();
    void eliminate_noise();
    double logprior();
    double loglik(board,zet);
};

class superheuristic{
  public:
    heuristic* h;
    int N;
    mt19937_64 engine;
  public:
    void seed_generator(mt19937_64 generator){
      engine.seed(generator());
    }
    void init(heuristic* subject,int Nplayers){
      h=subject;
      N=Nplayers;
    }
    zet makemove_bfs(board, bool);
};
}
#endif // HEURISTICOLD_H_INCLUDED
