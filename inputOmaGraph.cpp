#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
#include "string.h"
#include "inputOmaGraph.h"
#include "Graph.h"
#include "string.h"


using namespace std;

void Problem::loadFile(char * instance, Graph &g) {
    // VERSIONE C   MEDIA TEMPO:0.4440 SECONDI

    FILE *slo_fd;

    FILE *exm_fd;
    ifstream inputFile;                 // will open each file one at a time
    char line[255]="";


    // first: read instance.exm to get total number of exams
    exm_fd=fopen((string(instance) + ".exm").c_str(), "r");
    if(exm_fd!=NULL)
        cout<<instance<<".exm opened\n";
    else
    {
        cerr<<"Error opening "<<instance<<".exm file\n";
        exit(0);
    }
    //count exam number reading .exm file
    nExams=0;
    while(fscanf(exm_fd, "%*d %*d")!=EOF)
        nExams++;

    cout<<"\tnumber of exams: "<<nExams<<"\n\n";
    fclose(exm_fd);

    // allocate memory for the graph and its weight matrix
    conflicts = new int* [nExams];
    for (int i = 0; i<nExams; i++) {
        conflicts[i] = new int [nExams];
        for (int j = 0; j<nExams; j++) {
            conflicts[i][j]=0;
        }
    }

    /* //VERSIONE C++   MEDIA TEMPO: 0.4502 SECONDI
    ifstream inputFile; // will open each file one at a time
    char line[255]="";

    // first: read instance.exm to get total number of exams
    inputFile.open( (string(instance) + ".exm").c_str(), ifstream::in );
    if(inputFile.good())
        cout<<instance<<".exm opened! - "<<inputFile.good()<<endl;
    else{
        cout<<instance<<".exm not found, exiting..."<<endl;
        exit(0);
    }
    nExams = 0;
    while (!inputFile.eof()) {
        inputFile.getline(line, 255);
        if (strlen(line)>3) {
            nExams++;
        }
    }
    inputFile.close();
    cout<<"number of exams: "<<nExams<<endl;

    // allocate memory for the graph and its weight matrix
    conflicts = new int* [nExams];
    for (int i = 0; i<nExams; i++) {
        conflicts[i] = new int [nExams];
        for (int j = 0; j<nExams; j++) {
            conflicts[i][j]=0;
        }
    }*/


    g.resize(nExams);

    //reading .slo file
    slo_fd=fopen((string(instance) + ".slo").c_str(), "r");
    if(slo_fd!=NULL)
        cout<<instance<<".slo opened\n";
    else
    {
        cout<<"Error opening "<<instance<<".slo file\n";
        exit(0);
    }

    fscanf(slo_fd, "%d", &nSlots);
    fclose(slo_fd);
    cout<<"\t"<<nSlots<<" slots available\n\n";


    //VERSIONE C++
    // second: read instance.stu to find out, for each student, to which exams they are enrolled
    inputFile.open( (string(instance) + ".stu").c_str(), ifstream::in );
    if(inputFile.good())
        cout<<instance<<".stu opened\n";
    else
    {
        cerr<<instance<<".stu not found, exiting...\n";
        exit(0);
    }
    int student, newStudent;
    int newExam;
    inputFile.getline(line, 255,'s');
    inputFile >> student;
    list<int> exams;
    while (!inputFile.eof()){
        inputFile >> newExam;
        exams.push_back(newExam-1);
        inputFile.getline(line, 255,'s');
        inputFile >> newStudent;

        if (student != newStudent) {
            //cout<<"student "<<student<<"is enrolled to: "<<endl;
            for ( list<int>::iterator i = exams.begin(); i!=exams.end(); i++) {
                //cout<<*i+1<<" ";
                for ( list<int>::iterator j = exams.begin(); j!=exams.end(); j++) {
                    if (*i>*j) {
                        if (g[*i][*j]==0) {
                            g[*i][*j]=1;
                            g[*j][*i]=1;
                            g.nbEdges++;
                        }
                        conflicts[*i][*j]++;
                        conflicts[*j][*i]++;
                    }
                }
            }
            //cout<<endl<<endl;
            student = newStudent;
            exams.clear();
        }
    }
    inputFile.close();
    //cout<<"student "<<student<<" is enrolled to: "<<endl;
    for ( list<int>::iterator i = exams.begin(); i!=exams.end(); i++) {
        //cout<<*i+1<<" ";
        for ( list<int>::iterator j = exams.begin(); j!=exams.end(); j++) {
            if (*i>*j) {
                if (g[*i][*j]==0) {
                    g[*i][*j]=1;
                    g[*j][*i]=1;
                    g.nbEdges++;
                }
                conflicts[*i][*j]++;
                conflicts[*j][*i]++;
            }
        }
    }
    student = newStudent;
    exams.clear();

    //cout<<"done filling up conflict graph and its weight matrix\n\t";
    cout<<"\t"<<g.nbEdges<<" conflicting pairs found\n";
    for (int i = 0; i<nExams; i++) {
        for (int j = 0; j<nExams; j++) {
            //cout<<"("<<i<<" "<<j<<" "<<g[i][j]<<") ";
        }
    }
    cout<<endl;




    ofstream outputFile( (string(instance) + ".txt").c_str() );
    outputFile<<"p edge "<<g.n<<" "<<g.nbEdges<<endl;
    for (int i = 0; i<nExams-1; i++){
        for (int j = i+1; j<nExams; j++) {
            //cout<<i<<" "<<j<<" "<<g[i][j]<<endl;
            if (g[i][j]>0){
                outputFile<<"e "<<i<<" "<<j<<endl;
            }
        }
    }
    outputFile<<"c Finish!"<<endl;
    outputFile.close();
}
