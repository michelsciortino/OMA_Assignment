#include "tabu.h"
#include "manipulateArrays.h"

#include <iostream>
#include <stdlib.h>

extern unsigned long long numConfChecks;

using namespace std;

int tabu(Graph &g, vector<int> &t, int k, int maxIterations, int verbose, int **neighbors)
{
	int ** nodesByTimeSlot; // Arrays of nodes for each timeslot
	int * nbcPosition;   // Position of each node in the above array
	int ** conflicts;   // Number of conflicts for each timeslot and node
	int ** tabuStatus;  // Tabu status for each node and timeslot
	int *nodesInConflict = new int [g.n+1];
	int *confPosition = new int [g.n];

	long totalIterations = 0;
	int incVerbose = 0;
	int totalConflicts = 0;

	if (verbose == 1) incVerbose=10000;
	if (verbose == 2) incVerbose=100;
	if (verbose > 2) incVerbose=1;
	int nextVerbose = incVerbose;

	int tabuTenure = 5; //This is effetively a random choice

	initializeArrays(nodesByTimeSlot, conflicts, tabuStatus, nbcPosition, g, t, k);
	// Count the number of conflicts and set up the list nodesInConflict
	// with the associated list confPosition
	nodesInConflict[0]=0;
	for (int i=0; i<g.n; i++) {
		numConfChecks++;
		if (conflicts[t[i]][i] > 0) {
			totalConflicts += conflicts[t[i]][i];
			nodesInConflict[ (confPosition[i]=++nodesInConflict[0]) ] = i;
		}
	}
	totalConflicts /=2;
	if (verbose>1) cout << "Initialized the arrays. #Conflicts = " << totalConflicts << endl;

	int bestSolutionValue = totalConflicts; // Number of conflicts

	// Just in case we already have an admissible k-timeslotting
	if (bestSolutionValue == 0) {
		return 0;
	}

	//int minSolutionValue = g.n;
	//int maxSolutionValue = 0;

	//Main TABU LOOP
	while (totalIterations < maxIterations) {
		totalIterations++;
		int nc = nodesInConflict[0];

		int bestNode=-1, bestTimeSlot=-1, bestValue=g.n*g.n;
		int numBest=0;

		// Try for every node in conflict
		for (int iNode=1; iNode <= nodesInConflict[0]; iNode++) {
			int node = nodesInConflict[iNode];
			// to move it to every timeslot except its existing one
			for (int timeslot=1; timeslot<=k; timeslot++) {
				if (timeslot != t[node]) {
					numConfChecks+=2;
					int newValue = totalConflicts + conflicts[timeslot][node] - conflicts[t[node]][node];
					if (newValue <= bestValue && timeslot != t[node]) {
						if (newValue < bestValue) {
							numBest=0;
						}
						// Only consider the move if it is not tabu or leads to a new very best solution seen globally.
						if (tabuStatus[node][timeslot] < totalIterations || (newValue < bestSolutionValue)) {
							// Select the nth move with probability 1/n
							if ( (rand()%(numBest+1)) == 0 ) {//r.getInt(0,numBest)==0) {
								//we will move node "bestNode" to the new timeslot "bestTimeSlot"
								bestNode = node;
								bestTimeSlot = timeslot;
								bestValue = newValue;
							}
							numBest++;  // Count the number of considered moves
						}
					}
				}
			}
		}

		// If no non tabu moves have been found, take any random move
		if (bestNode == -1) {
			bestNode = rand()%g.n;
			while ((bestTimeSlot = (rand()%k)+1) != t[bestNode]);{
				bestValue = totalConflicts + conflicts[bestTimeSlot][bestNode] - conflicts[t[bestNode]][bestNode];
				numConfChecks+=2;
			}
		}

		// Now execute the move
		if (verbose>2) {
			cout << "Will move node " << bestNode << " to timeslot " << bestTimeSlot << " with value " << bestValue << " oldconf = " << conflicts[t[bestNode]][bestNode]			<< " newconf = " << conflicts[bestTimeSlot][bestNode] << " totalConflicts = " << totalConflicts<< endl;
		}
		int tTenure = tabuTenure;
		moveNodeToAssignForTabu(bestNode, bestTimeSlot, g, t, nodesByTimeSlot, conflicts, nbcPosition, neighbors, nodesInConflict, confPosition, tabuStatus, totalIterations, tTenure);
		totalConflicts = bestValue;

		//Now update the tabu tenure
		tabuTenure = (int)(0.6*nc) + (rand()%10);

		// check: have we a new globally best solution?
		if (totalConflicts < bestSolutionValue) {
			bestSolutionValue = totalConflicts;

			// If all nodes are assigned to a timeslot we report success and stop iterating
			if (bestSolutionValue == 0) {
				//We have found a feasible solution with k timeslots, so we jump out of the tabu loop
				break;
			}
			// Otherwise reinitialize some values
			//minSolutionValue = g.n*g.n;
			//maxSolutionValue = 0;
			nextVerbose = totalIterations;
		}

		//Do some output if needed
		if (totalIterations == nextVerbose && incVerbose) {
			cout << totalIterations << "   obj =" << totalConflicts << "   best =" << bestSolutionValue << "   tenure =" << tabuTenure << endl;
			nextVerbose += incVerbose;
		}

	}// END OF TABU LOOP

	freeArrays(nodesByTimeSlot, conflicts, tabuStatus, nbcPosition, k, g.n);
	delete [] nodesInConflict;
	delete [] confPosition;


	return totalConflicts;

}
