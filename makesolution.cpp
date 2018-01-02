#include "makesolution.h"

int ASSIGNED = INT_MIN;
extern unsigned long long numConfChecks;

inline bool timeSlotIsFeasible(int v, vector< vector<int> > &sol, int t, vector<int> &Exams, vector< vector<int> > &adjMatrix, Graph &g)
{
	//Checks to see whether vertex (exam) v can be feasibly inserted into timeslot t in sol.
	int i;
	numConfChecks++;
	if(sol[t].size() > adjMatrix[v].size()){
		//check if any neighbours of v are currently in timeslot t
		for(i=0; i<adjMatrix[v].size(); i++){
			numConfChecks++;
			if(Exams[adjMatrix[v][i]] == t) return false;
		}
		return true;
	}
	else {
		//check if any vertices in timeslot t are adjacent to v
		for(i=0; i<sol[t].size(); i++){
			numConfChecks++;
			if(g[v][sol[t][i]]) return false;
		}
		return true;
	}
}

inline void assignATimeSlot(bool &foundTimeSlot, vector< vector<int> > &TimeSlots, vector<int> &permutation, int examPos, vector<int> &conflictDegrees, Graph &g, vector<int> &Exams, vector< vector<int> > &adjMatrix)
{
	int i, j, t=0, v=permutation[examPos];
	bool alreadyAdj;

	while(t < TimeSlots.size() && !foundTimeSlot){
		//check if timeslot t is feasible for vertex (exam) v
		if(timeSlotIsFeasible(v, TimeSlots, t, Exams, adjMatrix, g)){
			//v can be added to this timeslot
			foundTimeSlot = true;
			TimeSlots[t].push_back(v);
			Exams[v] = t;
			//We now need to update conflictDegrees. To do this we identify the not assigned exams i that are adjacent to
			//this newly assigned exam v to timeslot t. If it is already adjacent to an exam in timeslot t we do nothing,
			//otherwise its conflict degree is increased...
			for(i=0; i<conflictDegrees.size(); i++){
				numConfChecks++;
				if(g[v][permutation[i]]){
					alreadyAdj = false;
					j=0;
					while(j<TimeSlots[t].size()-1 && !alreadyAdj){
						numConfChecks++;
						if(g[TimeSlots[t][j]][permutation[i]]) alreadyAdj = true;
						j++;
					}
					if (!alreadyAdj)
						conflictDegrees[i]++;
				}
			}
		}
		t++;
	}
}

inline void FillTimeSlots(vector< vector<int> > &TimeSlots, vector<int> &Exams, Graph &g, vector< vector<int> > &adjMatrix)
{
	int i, j, r;
	bool foundTimeSlot;

	//Make a vector representing all the exams
	vector<int> permutation(g.n);
	for (i=0;i<g.n;i++)permutation[i]=i;
	//Randomly permute the nodes, and then arrange by increasing order of degree
	//(this allows more than 1 possible outcome from the sort procedure)
	for(i=permutation.size()-1; i>=0; i--){
		r = rand()%(i+1);
		swap(permutation[i],permutation[r]);
	}
	//Bubble sort is used here. This could be made more efficent
	for(i=(permutation.size()-1); i>=0; i--){
		for(j=1; j<=i; j++){
			numConfChecks+=2;
			if(adjMatrix[permutation[j-1]].size() > adjMatrix[permutation[j]].size()){
				swap(permutation[j-1],permutation[j]);
			}
		}
	}

	//We also have a vector to hold the conflict degrees of each node
	vector<int> conflictDegrees(permutation.size(), 0);

	//Initializing TimeSlots and Exams
	TimeSlots.clear();
	TimeSlots.push_back(vector<int>());
	for(i=0; i<Exams.size(); i++) Exams[i] = INT_MIN;

	//Assigning a timeslot to the rightmost node first (it has the highest degree), and remove it from the permutation
	TimeSlots[0].push_back(permutation.back());
	Exams[permutation.back()] = 0;
	permutation.pop_back();
	//..and update the conflict degrees array
	conflictDegrees.pop_back();
	for(i=0; i<conflictDegrees.size(); i++){
		numConfChecks++;
		if(g[TimeSlots[0][0]][permutation[i]]){
			conflictDegrees[i]++;
		}
	}

	//Now assign a timeslot to the remaining nodes.
	int examPos = 0, maxConflict;
	while(!permutation.empty()){
		//choose the node to be assigned next (the rightmost node that has maximal conflictDegree)
		maxConflict = INT_MIN;
		for(i=0; i<conflictDegrees.size(); i++){
			if(conflictDegrees[i] >= maxConflict){
				maxConflict = conflictDegrees[i];
				examPos = i;
			}
		}
		//now choose which timeslot to assign to the node
		foundTimeSlot = false;
		assignATimeSlot(foundTimeSlot, TimeSlots, permutation, examPos, conflictDegrees, g, Exams, adjMatrix);
		if(!foundTimeSlot){
			//If we are here, we have to make a new timeslot as we have tried all the other ones and none is suitable
			TimeSlots.push_back(vector<int>());
			TimeSlots.back().push_back(permutation[examPos]);
			Exams[permutation[examPos]] = TimeSlots.size()-1;
			//Remember to update the conflict degrees array
			for(i=0; i<permutation.size(); i++){
				numConfChecks++;
				if(g[permutation[examPos]][permutation[i]]){
					conflictDegrees[i]++;
				}
			}
		}
		//Finally, we remove the node from the permutation
		permutation.erase(permutation.begin() + examPos);
		conflictDegrees.erase(conflictDegrees.begin() + examPos);
	}
}

int generateInitialK(Graph &g, vector<int> &FeasibleSolution){
	//Produce an solution using a constructive algorithm to get an intial setting for k (number of TimeSlots utilized)
	int i, j;

	//Make the structures needed for the constructive algorithms
	vector< vector<int> > TimeSlots;  //TimeSlots vector; each timeslot contains the list of the exams assigned to it
	vector< vector<int> > adjMatrix(g.n,vector<int>()); //Adiacence Matrix: contains, for each exam, the list of the conficting exam
	vector<int> Exams(g.n, INT_MAX);
	for(i=0; i<g.n; i++){
		for(j=0; j<g.n; j++){
			if(g[i][j] && i!=j){
				adjMatrix[i].push_back(j);         //TO BE OPTIMIZED
			}
		}
	}

	//Now make the solution
	FillTimeSlots(TimeSlots,Exams,g,adjMatrix);
	for(i=0;i<TimeSlots.size();i++) for(j=0;j<TimeSlots[i].size();j++) FeasibleSolution[TimeSlots[i][j]] = i;
	//And return the number of timeslots it has used
	return TimeSlots.size();
}




//-----------BELOW ARE THE FUNCTIONS FOR GENERATING SOLUTIONS WITH A MAXIMUM K TIMESLOTS
inline int assignToTimeSlot(vector< vector<bool> > &availTSlots, vector< vector<int> > &TimeSlots, int k, int v)
{
	int t = 0;
	while(t < k){
		if(availTSlots[v][t]){
			//timeslot t is OK for vertex (exam) v, so we assign it and exit
			TimeSlots[t].push_back(v);
			return t;
		}
		t++;
	}
	//If we are here then there is an error as we thought v had available timeslots, but it turns out it didn't
	exit(1);
}

inline void updateTSOptions(vector< vector<bool> > &availTSlots, vector<int> &numTSOptions, Graph &g, int v, int tslot)
{
	int i;
	//Updates TSOptions vector due to node v being assigned a timeslot
	numTSOptions[v] = ASSIGNED;
	for(i=0; i<g.n; i++){
		if(numTSOptions[i] != ASSIGNED){
			numConfChecks++;
			if(availTSlots[i][tslot] && g[i][v]){
				availTSlots[i][tslot] = false;
				numTSOptions[i]--;
			}
		}
	}
}

inline bool timeSlotsAvailable(vector<int> &TSOptions)
{
	int i;
	for(i=0; i<TSOptions.size(); i++)
		if(TSOptions[i] >= 1)
			return true;
	return false;
}

inline int chooseNextNode(vector<int> &TSOptions)
{
	int i;
	int minOptions = INT_MAX;
	vector<int> a;
	for(i=0; i<TSOptions.size(); i++)
		if(TSOptions[i] != ASSIGNED)
			if(TSOptions[i] >= 1){
				if(TSOptions[i] < minOptions){
					a.clear();
					a.push_back(i);
					minOptions = TSOptions[i];
				}
				else if (TSOptions[i] == minOptions){
					a.push_back(i);
				}
			}

	if(a.empty()){
		cout<<"Error: There were supposed to be choices, but there aren't";
		exit(1);
	}

	int x = rand()%a.size();
	return(a[x]);
}

void makeInitSolution(Graph &g, vector<int> &sol, int k, int verbose)
{
	int i, v, j, t;

	//1) Make a 2D vector containing all timeslots options for each node (initially k for all nodes)
	vector<int> numTSOptions(g.n,k);
	vector< vector<bool> > availTSlots(g.n, vector<bool>(k,true));

	//... and make an empty solution in convienient representation
	vector< vector<int> > TimeSlots(k,vector<int>());

	//2) Now add a random node to the first timeslot and update TSOptions
	v = rand()%g.n;
	t = assignToTimeSlot(availTSlots,TimeSlots,k,v);
	updateTSOptions(availTSlots,numTSOptions,g,v,t);
	//3) For each remaining node with available timeslots options, choose a node with minimal (>=1) options and assign to an early timeslot
	while(timeSlotsAvailable(numTSOptions)){
		//choose node to be assigned to the timeslot
		v = chooseNextNode(numTSOptions);
		//assign to a timeslot
		t = assignToTimeSlot(availTSlots,TimeSlots,k,v);
		updateTSOptions(availTSlots,numTSOptions,g,v,t);
	}

	//When we are here, we either have a full valid solution, or some nodes are still unplaced (marked with 0's in numTSOptions)
	//These are now placed in random timeslots
	for(i=0; i<g.n; i++){
		if(numTSOptions[i] > 0){
			cout<<"Error: node "<<i<<"should have gone somewhere as it appears to have had "<<numTSOptions[i] << "timeslots available"<<endl;
			exit(1);
		}

		if(numTSOptions[i] == 0){
			//put node i into a random timeslot
			TimeSlots[rand()%k].push_back(i);
		}
	}

	//3) Now tranfer to the more convienient representation in the population itself and end
	for(i=0; i<k; i++){
		for(j=0; j<TimeSlots[i].size(); j++){
			sol[TimeSlots[i][j]] = i+1;
		}
	}
}
