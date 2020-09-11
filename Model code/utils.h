#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "board.h"
#include "data_struct.h"
#include "heuristic.h"

double compute_entropy(heuristic& ,board ,bool ,int );
void compute_fmri_values_entropy(heuristic& , data_struct& , const char* , const char* , int );
void compute_fmri_iters(heuristic& , data_struct& , const char* , const char* , int );
void compute_fmri_values_pv_depth(heuristic& , data_struct& , const char* , const char* , int );
void compute_fmri_values(heuristic& , data_struct& , const char* , const char* , int );
void compute_fmri_values_myopic(heuristic& , data_struct& , const char* , const char* , int );


#endif // UTILS_H_INCLUDED
