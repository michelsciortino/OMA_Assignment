#ifndef MANIPULATEARRAYS_H_INCLUDED
#define MANIPULATEARRAYS_H_INCLUDED

#include <iostream>
#include <vector>
#include "Graph.h"

using namespace std;

void initializeArrays(int ** & nodesByTimeSlot, int ** & conflicts, int ** & tabuStatus,
		      int * & nbcPosition, Graph & g, vector<int> &t, int k);
void moveNodeToAssignForTabu(int bestNode, int bestTimeSlot, Graph & g, vector<int> &t, int ** nodesByTimeSlot, int ** conflicts,
			    int * nbcPosition, int ** neighbors, int * nodesInConflict, int * confPosition,
			    int ** tabuStatus,  long totalIterations, int tabuTenure);
void freeArrays(int ** & nodesByTimeSlot, int ** & conflicts, int ** & tabuStatus, int * & nbcPosition, int k, int n);

#endif // MANIPULATEARRAYS_H_INCLUDED
