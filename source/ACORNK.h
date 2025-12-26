#pragma once

#include "Graph.h"

#define UPDATE_TIME 0.002

// Stores the meta heuristics
typedef struct {
    int infPheromone;
    int infTaskLength;
    int infProcessor;
    double decayParameter; // Pheromone decay
    int maxIteration;
    int deltaIterationAging; // Number of iteration required until the next aging period
    int numAnts;
    int numBestAnts;
} MetaHeuristics;


// Stores the ant's memory in the previous frame
typedef struct {
    struct ReadyTasks {
        int *list;
        int capacity;
        int first;
        int last;

        int *numParents;
    } readyTasks;

    int *sequence; // startTask -> proc -> task -> proc -> ... -> endTask -> proc
    int sequenceSize;

    // Time that the processor finish its last task
    int *latestProcessorTime;
    
    // Time the task dependencies are resolved
    int *maxDependencyResolveTime;

    int *taskEndTime;
    int *taskStartTime;

    int completionTime;
    
} AntMemory;


// Stores the ACORNK algorithm data in the previous frame
typedef struct {
    char isRunning;
    int iterationNum;
    char isAging;
    double *preMatrix;
    double *postMatrix;
    float timeUntilNextUpdate;
    float maxTimeUntilNextUpdate;
    AntMemory *ants;
    int currentAnt;
    GraphNode *graph;
    int bestSolutionIteration;
    int bestSolution;

    int *bestAssignedProcess;
    int *bestStartTime;
    int *bestEndTime;

} ACORNK;

extern MetaHeuristics gMetaHeuristics;
extern ACORNK gACORNK;

void InitAlgorithm();
void BeginAlgorithm();

void UpdateACORNK(float deltaTime);