#ifndef XOVER_H_INCLUDED
#define XOVER_H_INCLUDED

#include "Graph.h"
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <float.h>
#include <vector>

using namespace std;

void doCrossover(vector<int> &osp, vector<int> &parents, Graph &g, int k, vector< vector<int> > &population);

#endif // XOVER_H_INCLUDED
