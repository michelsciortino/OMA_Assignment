#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <time.h>
#include <iomanip>

#include "inputOmaGraph.h"
#include "Graph.h"
#include "makesolution.h"
#include "tabu.h"
#include "xOver.h"
#include "diversity.h"
#include "timetablingOptimizer.h"

#define ON 1
#define OFF 0

#define POP_SIZE 10
#define MAX_ITERATION 16
#define VERBOSE ON
#define RAND_SEED ON
//#define CONSTR_ALG ON
#define MIN_TARGET_COLS 2
//#define X_OVER_TYPE 1

using namespace std;

unsigned long long numConfChecks;

inline bool solIsOptimal(vector<int> &sol, Graph &g, int k);
inline void replace(vector<vector<int> > &population, vector<int> &parents, vector<int> &osp, vector<int> &popCosts, Graph &g, int oCost);

int main(int argc, char ** argv)
{
    Problem data;
    Graph g;
    int i, k;
    int popSize = POP_SIZE,
        maxIterations = MAX_ITERATION,
        verbose = VERBOSE,
        randomSeed = RAND_SEED,
        //constructiveAlg = CONSTR_ALG,
        targetCols = MIN_TARGET_COLS;
        //xOverType = X_OVER_TYPE;
    bool solFound = false,
         doKempeMutation = false,
         measuringDiversity = false;
    //unsigned long long maxChecks = 100000000;
    unsigned long long maxChecks = INT_MAX;
    vector<int> parents;
    ofstream timeStream, confStream;        //OUTPUT FILES
    numConfChecks = 0;   //This variable keeps count of the number of times information
                                            //about the instance is looked up

    clock_t clockStart = clock();

    //Checking command line input
    if(argc!=4)
    {
        cout<<"Command line error"<<endl;
        exit(0);
    }

    // Initialize data structures
    cout<<"Hybrid Evolutionary Algorithm using <"<<argv[1]<<">\n\n";
    data.loadFile(argv[1], g);
    targetCols = data.nSlots;

    //set tabucol limit
    maxIterations = maxIterations*g.n;
    if(targetCols < MIN_TARGET_COLS || targetCols > g.n) targetCols = MIN_TARGET_COLS;

    //Now set up some output files
    timeStream.open("teffort.txt");
    confStream.open("ceffort.txt");
    if (timeStream.fail() || confStream.fail())
    {
        cout << "ERROR OPENING output FILE";
        exit(1);
    }

    //Do a check to see if we have the empty graph. If so, end immediately.
    if(g.nbEdges <= 0)
    {
        confStream<<"1\t0\n0\tX\t0\n";
        timeStream<<"1\t0\n0\tX\t0\n";
        if(verbose)
            cout<<"Graph has no edges. Optimal solution is obviously using one colour. Exiting."<<endl;
        confStream.close();
        timeStream.close();
        exit(1);
    }


    //Make the adjacency list structure
	int **neighbors = new int*[g.n];
	g.makeAdjList(neighbors); //lista adiacenze (prima col numero esami conflict, successive codici esami in conf)
                              //OTTIMIZZARE per creare matrice adiacenze da usare in generateinitialK

    //Produce some output
	if(verbose) cout<<" COLS     CPU-TIME     CHECKS"<<endl;

	//Seed and start timer
	//clock_t clockStart = clock();
	srand(randomSeed);
	numConfChecks = 0;

	//Data structures used for population and offspring
	vector< vector<int> > population(POP_SIZE, vector<int>(g.n));
	vector<int> popCosts(popSize);
	vector<int> osp(g.n), bestColouring(g.n);


	//Generate the initial value for k using greedy or dsatur algorithm
	k = generateInitialK(g, bestColouring);
	cout<<"Min k found: "<<k<<endl;
	//..and write the results to the output file
	int duration = int(((double)(clock()-clockStart)/CLOCKS_PER_SEC)*1000);
	if(verbose>=1) cout<<setw(5)<<k<<setw(11)<<duration<<"ms\t"<<numConfChecks<<" (via constructive)"<<endl;
	confStream<<k<<"\t"<<numConfChecks<<"\n";
	timeStream<<k<<"\t"<<duration<<"\n";
	if(k<=targetCols){
		if(verbose>=1) cout<<"\nSolution with  <="<<targetCols<<" colours has been found. Ending..."<<endl;
		confStream<<"1\t"<<"X"<<"\n";
		timeStream<<"1\t"<<"X"<<"\n";
	}
	else{
        cout<<"Unfeasible problem"<<endl;
        exit(0);
	}


	//MAIN ALGORITHM
	//k--;
	while(numConfChecks < maxChecks && k <= targetCols){
		solFound = false;

		//First build the population
		for(i=0; i<popSize; i++){
			//Build a solution using modified DSatur algorithm
			makeInitSolution(g, population[i], k, verbose);
			//Check to see whether this solution is alrerady optimal or if the cutoff point has been reached. If so, we end
			if(solIsOptimal(population[i],g,k)){
				solFound = true;
				for(int j=0; j<g.n; j++)osp[j]=population[i][j];
				break;
			}
			if(numConfChecks >= maxChecks){
				for(int j=0; j<g.n; j++)osp[j]=population[i][j];
				break;
			}
			//Improve each solution via tabu search and record their costs
			popCosts[i] = tabu(g, population[i], k, maxIterations, 1, neighbors);
			//Check to see whether this solution is now optimal or if the cuttoff point is reached. If so, we end
			if(verbose>=1)cout<<"          -> Individual "<<setw(4)<<i<<" constructed. Cost = "<<popCosts[i]<<endl;
			if(popCosts[i] == 0){
				solFound = true;
				for(int j=0; j<g.n; j++)osp[j]=population[i][j];
				//break;
			}
			if(numConfChecks >= maxChecks){
				for(int j=0; j<g.n; j++)osp[j]=population[i][j];
				break;
			}
		}

		//Now evolve the population
		int rIts = 0, oCost = 1, best = INT_MAX;
		//while(numConfChecks < maxChecks && !solFound){
		while(!solFound){

			//Choose parents and perform crossover to produce a new offspring
			doCrossover(osp,parents,g,k,population);

			//Improve the offspring via tabu search and record its cost
			oCost = tabu(g, osp, k, maxIterations, 0, neighbors);

			//Write osp over weaker parent and update popCosts
			replace(population,parents,osp,popCosts,g,oCost);

			if(verbose>=2){
				cout<<"          -> Offspring "<<setw(5)<<rIts<<" constructed. Cost = "<<oCost;
				if(measuringDiversity) cout<<"\tDiversity = "<<measureDiversity(population,k);
				cout<<endl;
			}

			rIts++;

			if(oCost < best) best = oCost;
			if(oCost == 0) solFound = true;
		}

		//Algorithm has finished at this k
		duration = int(((double)(clock()-clockStart)/CLOCKS_PER_SEC)*1000);
		if(solFound){
			if(verbose>=1) cout<<setw(5)<<k<<setw(11)<<duration<<"ms\t"<<numConfChecks<<endl;
			confStream<<k<<"\t"<<numConfChecks<<"\n";
			timeStream<<k<<"\t"<<duration<<"\n";
			//Copy the current solution as the best solution
			for(int i=0; i<g.n; i++) bestColouring[i] = osp[i]-1;
			if(k==targetCols){
				if(verbose>=1) cout<<"\nSolution with  <="<<targetCols<<" colours has been found. Ending..."<<endl;
				confStream<<"1\t"<<"X"<<"\n";
				timeStream<<"1\t"<<"X"<<"\n";
				break;
			}
		}
		else {
			if(verbose>=1) cout<<"\nRun limit exceeded. No solution using "<<k<<" colours was achieved (Checks = "<<numConfChecks<<", "<<duration<<"ms)"<<endl;
			confStream<<k<<"\tX\t"<<numConfChecks<<"\n";
			timeStream<<k<<"\tX\t"<<duration<<"\n";
		}

		k++;
	} // while(numConfChecks < maxChecks && k+1 > targetCols)
    cout<<"Used timeslots: "<<k<<endl;
	ofstream solStrm;
	solStrm.open("solution.sol");
	//solStrm<<g.n<<"\n";
	for(int i=0;i<g.n;i++)solStrm<<(i+1)<<" "<<bestColouring[i]+1<<"\n";
	solStrm.close();

    // try to get an optimal solution
    /* OPTIMIZATION
    Optimizer opt(data, bestColouring);
	opt.run();
*/
    printf("\n\nend\n\n\n\n");
    return(0);
}

inline
bool solIsOptimal(vector<int> &sol, Graph &g, int k)
{
	int i, j;
	for(i=0; i<g.n; i++){
		if(sol[i] < 1 || sol[i] > k){
			cout<<"Error: Colour of node "<<i<<" out of bounds "<<endl;
			exit(1);
		}
	}

	for(i=0; i<(g.n)-1; i++){
		for(j=i+1; j<g.n; j++){
			if(sol[i] == sol[j] && g[i][j])
				return(false);
		}
	}
	//If we are here then we have established a solution with k or less colours
	return(true);
}

inline
void replace(vector<vector<int> > &population, vector<int> &parents, vector<int> &osp, vector<int> &popCosts, Graph &g, int oCost)
{
	//Go through the parents and ID the worst one
	int toDie, i, max=INT_MIN;
	for(i=0;i<parents.size();i++){
		if(popCosts[parents[i]] > max){
			max = popCosts[parents[i]];
			toDie = parents[i];
		}
	}
	//Copy osp over the parent selected toDie
	population[toDie] = osp;
	popCosts[toDie] = oCost;
}
