#ifndef MANIPULATEARRAYS_H_INCLUDED
#define MANIPULATEARRAYS_H_INCLUDED

#include <iostream>
#include <vector>
#include "Graph.h"

using namespace std;

void initializeArrays(int ** & nodesByColor, int ** & conflicts, int ** & tabuStatus,
		      int * & nbcPosition, Graph & g, vector<int> &c, int k);
void moveNodeToColorForTabu(int bestNode, int bestColor, Graph & g, vector<int> &c, int ** nodesByColor, int ** conflicts,
			    int * nbcPosition, int ** neighbors, int * nodesInConflict, int * confPosition,
			    int ** tabuStatus,  long totalIterations, int tabuTenure);
void freeArrays(int ** & nodesByColor, int ** & conflicts, int ** & tabuStatus, int * & nbcPosition, int k, int n);

#endif // MANIPULATEARRAYS_H_INCLUDED
