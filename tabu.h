#ifndef TABU_H_INCLUDED
#define TABU_H_INCLUDED

#include <vector>
#include "Graph.h"

using namespace std;

int tabu(Graph &g, vector<int> &c, int k, int maxIterations, int verbose, int **neighbors);

#endif // TABU_H_INCLUDED
