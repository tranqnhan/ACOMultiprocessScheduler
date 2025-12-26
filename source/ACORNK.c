#include "Program.h"
#include "Graph.h"
#include "Entity.h"
#include "ACORNK.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "assert.h"
#include "string.h"
#include "limits.h"
#include <float.h>

// Global algorithm data structures
MetaHeuristics gMetaHeuristics;
ACORNK gACORNK;

void InitMetaHeuristics() {
    gMetaHeuristics.infPheromone = 1;
    gMetaHeuristics.infTaskLength = 1;
    gMetaHeuristics.infProcessor = 1;
    gMetaHeuristics.decayParameter = 0.2; // Pheromone decay
    gMetaHeuristics.maxIteration = 10;
    gMetaHeuristics.deltaIterationAging = 5; // Number of iteration required until the next aging period
    gMetaHeuristics.numAnts = 4;
    gMetaHeuristics.numBestAnts = 2;

    FILE *fp;
    fp = fopen("../config/metaheuristics.txt", "r");
    if (fp) { 
        char buffer[MAX_LINE];

        while (fgets(buffer, MAX_LINE, fp) != NULL) {
            if (strcmp(buffer, "\n") == 0) break;
            char *token;
            const char delimiter[] = " ";
            
            token = strtok(buffer, delimiter);
            char *property = token;
            token = strtok(NULL, delimiter);
            float value = atof(token);

            if (strcmp(property, "pheromone") == 0) {
                gMetaHeuristics.infPheromone = (int) value;
            } else if (strcmp(property, "taskLength") == 0) {
                gMetaHeuristics.infTaskLength = (int) value;
            } else if (strcmp(property, "processor") == 0) {
                gMetaHeuristics.infProcessor = (int) value;
            } else if (strcmp(property, "decayParameter") == 0) {
                gMetaHeuristics.decayParameter = (float) value;
            } else if (strcmp(property, "maxIteration") == 0) {
                gMetaHeuristics.maxIteration = (int) value;
            } else if (strcmp(property, "deltaIterationAging") == 0) {
                gMetaHeuristics.deltaIterationAging = (int) value;
            } else if (strcmp(property, "numAnts") == 0) {
                gMetaHeuristics.numAnts = (int) value;
            } else if (strcmp(property, "numBestAnts") == 0) {
                gMetaHeuristics.numBestAnts = (int) value;
            }
        }
    
        fclose(fp);
    } else {
        printf("Cannot open ../config/metaheuristics.txt \n Reverting to defaults \n");
    }

    printf("MetaHeuristics: \n");
    printf("pheromone %i\n", gMetaHeuristics.infPheromone);
    printf("taskLength %i\n", gMetaHeuristics.infTaskLength);
    printf("processor %i\n", gMetaHeuristics.infProcessor);
    printf("decayParameter %f\n", gMetaHeuristics.decayParameter);
    printf("maxIteration %i\n", gMetaHeuristics.maxIteration);
    printf("deltaIterationAging %i\n", gMetaHeuristics.deltaIterationAging);
    printf("numAnts %i\n", gMetaHeuristics.numAnts);
    printf("numBestAnts %i\n", gMetaHeuristics.numBestAnts);
}

void InitACORNK() {
    gACORNK.isRunning = 0;
    gACORNK.iterationNum = 0;
    gACORNK.isAging = 0;
    gACORNK.preMatrix = NULL;
    gACORNK.postMatrix = NULL;
    gACORNK.timeUntilNextUpdate = 0;
    gACORNK.bestSolutionIteration = 0;
    gACORNK.bestSolution = INT_MAX;
    gACORNK.maxTimeUntilNextUpdate = UPDATE_TIME;
    gACORNK.ants = NULL;
    gACORNK.graph = NULL;
}


void InitAlgorithm() {
    InitMetaHeuristics();
    InitACORNK();
}

void AllocACORNK() {
    gACORNK.preMatrix = (double *) malloc(sizeof(double) * gNodes.length * gProcessors.length);
    gACORNK.postMatrix = (double *) malloc(sizeof(double) * gNodes.length * gProcessors.length);
    gACORNK.ants = (AntMemory *) malloc(sizeof(AntMemory) * gMetaHeuristics.numAnts);
    gACORNK.bestAssignedProcess = (int *) malloc(sizeof(int) * gNodes.length);
    gACORNK.bestEndTime = (int *) malloc(sizeof(int) * gNodes.length);
    gACORNK.bestStartTime = (int *) malloc(sizeof(int) * gNodes.length);
    gACORNK.graph = AllocGraph();
}

void FreeACORNK() {
    free(gACORNK.preMatrix);
    free(gACORNK.postMatrix);
    free(gACORNK.ants);
    free(gACORNK.bestAssignedProcess);
    free(gACORNK.bestEndTime);
    free(gACORNK.bestStartTime);
    
    FreeGraph(gACORNK.graph);

    gACORNK.preMatrix = NULL;
    gACORNK.postMatrix = NULL;
    gACORNK.ants = NULL;
    gACORNK.graph = NULL;
}

void AllocAnt(AntMemory *ant) {
    ant->readyTasks.list = (int *) malloc(sizeof(int) * gNodes.length);
    ant->readyTasks.numParents = (int *) malloc(sizeof(int) * gNodes.length);

    ant->sequence = (int *) malloc(sizeof(int) * gNodes.length * 2);
    ant->latestProcessorTime = (int *) malloc(sizeof(int) * gProcessors.length);
    ant->maxDependencyResolveTime = (int *) malloc(sizeof(int) * gNodes.length);

    ant->taskEndTime = (int *) malloc(sizeof(int) * gNodes.length);
    ant->taskStartTime = (int *) malloc(sizeof(int) * gNodes.length);
}

void FreeAnt(AntMemory *ant) {
    free(ant->readyTasks.list);
    free(ant->readyTasks.numParents);

    free(ant->sequence); 
    free(ant->latestProcessorTime);
    free(ant->maxDependencyResolveTime);
}

void InitAnt(AntMemory *ant) {
    for (int i = 0; i < gNodes.length; ++i) {
        ant->readyTasks.numParents[i] = gNodes.data[i].numPrevTasks;
    } 

    ant->readyTasks.list[0] = START_NODE;

    ant->readyTasks.capacity = 1;
    ant->readyTasks.first = 0;
    ant->readyTasks.last = 1;

    ant->sequenceSize = 0;
    ant->completionTime = 0;

    for (int i = 0; i < gProcessors.length; ++i) {
        ant->latestProcessorTime[i] = 0;
    } 
    
    for (int i = 0; i < gNodes.length; ++i) {
        ant->maxDependencyResolveTime[i] = 0;
    } 

    for (int i = 0; i < gNodes.length; ++i) {
        ant->taskEndTime[i] = 0;
    }

    for (int i = 0; i < gNodes.length; ++i) {
        ant->taskStartTime[i] = 0;
    }
}



// Return the task at index in readyTasks
int ReadyTasksPop(AntMemory *antMemory, const int index) {
    assert(index < antMemory->readyTasks.last);
    //printf("ready task pop\n");
    const int task = antMemory->readyTasks.list[index];
    antMemory->readyTasks.list[index] = antMemory->readyTasks.list[antMemory->readyTasks.first++];
    
    //printf("ready task first %i\n", antMemory->readyTasks.first);
    return task;
}


// Gets the average computation cost of a task
double GetAverageComputationCost(GraphNode *graph, const int task) {
    assert(task < gNodes.length);

    if (graph[task].averageComputateCost > -1) return graph[task].averageComputateCost;

    if (gProcessors.length == 0) return 0;

    int sumCost = 0;
    for (int i = 0; i < gProcessors.length; ++i) {
        sumCost += gProcessors.data[i].computeCosts[task];
    }

    graph[task].averageComputateCost = (double) sumCost / gProcessors.length;

    return graph[task].averageComputateCost;
}

// Gets the urank of a task
double Urank(GraphNode *graph, int task) {
    assert(task < gNodes.length);
    
    if (graph[task].uRank > -1) return graph[task].uRank;

    double averageComputationCost = GetAverageComputationCost(graph, task); 

    const GraphNode subgraph = graph[task];

    double max = 0;
    for (int i = 0; i < subgraph.numNextTasks; ++i) {
        
        const int nextTask = subgraph.nextTasks[i];
        const int commCost = subgraph.commCosts[i];
        const double subMax = commCost + Urank(graph, nextTask);
    
        if (subMax > max) {
            max = subMax;
        }
    }

    graph[task].uRank = averageComputationCost + max;

    return max;
}


// Precompute average computation cost
// Precompute urank
void PrecomputeValues(GraphNode *graph) {
    // Precompute Average Computation Cost
    for (int i = 0; i < gNodes.length; ++i) {
        GetAverageComputationCost(graph, i);
    }

    // Precompute Urank
    Urank(graph, 0);
}

// Compute the task heuristics
double TaskHeuristic(GraphNode *graph, int task) {
    assert(task < gNodes.length && gNodes.length != 0);
    return 1.0 / Urank(graph, task);
}

// Compute the processor heuristics
double ProcessorHeuristic(GraphNode *graph, const AntMemory *antMemory, const int processor, const int task) {
    assert(processor < gProcessors.length && task < gNodes.length);
    
    // Time that the processor finish its last task
    const int currentProcTime = antMemory->latestProcessorTime[processor]; 
    const int maxDependencyResolveTime = antMemory->maxDependencyResolveTime[task];

    const int estimateStartTime = currentProcTime > maxDependencyResolveTime ? currentProcTime : maxDependencyResolveTime; 

    return 1.0 / (GetAverageComputationCost(graph, task) * estimateStartTime);
}

// Get the pheromone of a pheromone matrix
double GetPheromone(const double *matrix, int processor, int task) {
    assert(processor < gProcessors.length && task < gNodes.length);
    
    return matrix[task * gProcessors.length + processor];
}

// Set the pheromone of a pheromone matrix
void SetPheromone(double *matrix, int processor, int task, double value) {
    assert(processor < gProcessors.length && task < gNodes.length);
    
    matrix[task * gProcessors.length + processor] = value;
}

// Select a task based on the STask rule
int SelectTask(
    GraphNode *graph,
    ACORNK *algodata,
    AntMemory *antMemory,
    const MetaHeuristics *metaHeuristics
) {
    if (antMemory->sequenceSize == 0) {
        // Start Task
        return ReadyTasksPop(antMemory, antMemory->readyTasks.first);
    }

    const int processor = antMemory->sequence[antMemory->sequenceSize - 1];
    const double *preMatrix = algodata->preMatrix;

    const int numReady = antMemory->readyTasks.last - antMemory->readyTasks.first;
    double weightedCost[numReady];
    double sum = 0;
    
    for (int i = 0; i < numReady; ++i) {
        const int task = antMemory->readyTasks.list[i + antMemory->readyTasks.first];
        const double pheromone = GetPheromone(preMatrix, processor, task);
        const double h = TaskHeuristic(graph, task);
        const double cost = pow(pheromone, metaHeuristics->infPheromone) * pow(h, metaHeuristics->infTaskLength);
        sum += weightedCost[i] = cost;
    }

    if (sum <= 0) return -1;

    for (int i = 0; i < numReady; ++i) {
        weightedCost[i] /= sum;
    }

    for (int i = 1; i < numReady; ++i) {
        weightedCost[i] += weightedCost[i - 1];
    }

    const double r = (double)rand() / RAND_MAX;

    int pickReadyTask = numReady - 1; 
    for (int i = 0; i < numReady; ++i) {
        if (r < weightedCost[i]) {
            pickReadyTask = i;
            break;
        }
    }

    return ReadyTasksPop(antMemory, pickReadyTask + antMemory->readyTasks.first);
}

// Select a processor based on processor rules
int SelectProcessor(
    GraphNode *graph,
    ACORNK *algodata,
    AntMemory *antMemory,
    const MetaHeuristics *metaHeuristics
) {
    
    const int task = antMemory->sequence[antMemory->sequenceSize - 1];
    const double *postMatrix = algodata->postMatrix;

    double weightedCost[gProcessors.length];
    double sum = 0;
    
    for (int processor = 0; processor < gProcessors.length; ++processor) {
        const double pheromone = GetPheromone(postMatrix, processor, task);
        const double h = ProcessorHeuristic(graph, antMemory, processor, task);
        const double cost = pheromone * pow(h, metaHeuristics->infProcessor);
        sum += weightedCost[processor] = cost;
    }

    if (sum <= 0) return -1;

    for (int i = 0; i < gProcessors.length; ++i) {
        weightedCost[i] /= sum;
    }

    for (int i = 1; i < gProcessors.length; ++i) {
        weightedCost[i] += weightedCost[i - 1];
    }

    const double r = (double)rand() / RAND_MAX;

    int pickProcessor = gProcessors.length - 1;
    for (int i = 0; i < gProcessors.length; ++i) {
        if (r < weightedCost[i]) {
            pickProcessor = i;
            break;
        }
    }

    return pickProcessor;
}

// Local pheromone update as describe in the paper
void LocalPheromoneUpdate(ACORNK *algodata, const AntMemory *antMemory, const MetaHeuristics *metaHeuristics) {
    const int completionTime = antMemory->completionTime;
    const double decayParameter = metaHeuristics->decayParameter;
    double *preMatrix = algodata->preMatrix;
    double *postMatrix = algodata->postMatrix;

    double deltaAddPheromone = 1.0 / completionTime;

    int task, processor;
    double pheromone;


    for (int i = 0; i < antMemory->sequenceSize; i += 2) {
        task = antMemory->sequence[i];
        processor = antMemory->sequence[i + 1];
        pheromone = (1.0 - decayParameter) 
            * GetPheromone(preMatrix, processor, task) 
            + deltaAddPheromone;
        SetPheromone(preMatrix, processor, task, pheromone);
    }

    for (int i = 1; i < antMemory->sequenceSize - 2; i += 2) {
        processor = antMemory->sequence[i];
        task = antMemory->sequence[i + 1];

        pheromone = (1.0 - decayParameter) 
            * GetPheromone(postMatrix, processor, task) 
            + deltaAddPheromone;
        SetPheromone(postMatrix, processor, task, pheromone);
    }
    
}


// Apply pheromone to the ant with the best solution
void GlobalPheromoneUpdate(ACORNK *algodata, const AntMemory *antMemory, const MetaHeuristics *metaHeuristics) {
    const int completionTime = antMemory->completionTime;
    const double decayParameter = metaHeuristics->decayParameter;
    double *preMatrix = algodata->preMatrix;
    double *postMatrix = algodata->postMatrix;

    double deltaAddPheromone = 1.0 / completionTime;

    int task, processor;
    double pheromone;
    
    for (int i = 0; i < antMemory->sequenceSize; i += 2) {
        task = antMemory->sequence[i];
        processor = antMemory->sequence[i + 1];
        pheromone = (1.0 - decayParameter) 
            * GetPheromone(preMatrix, processor, task) 
            + (decayParameter * deltaAddPheromone);
        SetPheromone(preMatrix, processor, task, pheromone);
    }

    for (int i = 1; i < antMemory->sequenceSize - 2; i += 2) {
        processor = antMemory->sequence[i];
        task = antMemory->sequence[i + 1];
        pheromone = (1.0 - decayParameter) 
            * GetPheromone(postMatrix, processor, task) 
            + (decayParameter * deltaAddPheromone);
        SetPheromone(postMatrix, processor, task, pheromone);
    }
}

void AgePheromone(ACORNK *algodata,  const AntMemory *antMemory) {
    double *preMatrix = algodata->preMatrix;
    double *postMatrix = algodata->postMatrix;

    int task, processor;
    
    for (int i = 0; i < antMemory->sequenceSize; i += 2) {
        task = antMemory->sequence[i];
        processor = antMemory->sequence[i + 1];
        const double currentPheromone = GetPheromone(preMatrix, processor, task);
        const double r = (double)rand() / RAND_MAX;
        
        SetPheromone(preMatrix, processor, task, currentPheromone * r);
    }

    for (int i = 1; i < antMemory->sequenceSize - 2; i += 2) {
        processor = antMemory->sequence[i];
        task = antMemory->sequence[i + 1];
        const double currentPheromone = GetPheromone(postMatrix, processor, task);
        const double r = (double)rand() / RAND_MAX;
        
        SetPheromone(postMatrix, processor, task, currentPheromone * r);
    }
}

// Add a task to ant list
void AntAddTask(AntMemory *antMemory, const int task) {
    antMemory->sequence[antMemory->sequenceSize++] = task;    
}


// Add a processor to ant list
void AntAddProcessor(AntMemory *antMemory, const int processor) {
    antMemory->sequence[antMemory->sequenceSize++] = processor;    
}


void AntSelectionIteration(
    GraphNode *graph,
    ACORNK *algodata,
    AntMemory *antMemory,
    const MetaHeuristics *metaHeuristics
) {
    int task, processor;
    if (algodata->iterationNum == 0 || algodata->isAging) {
        const int r = rand() % (antMemory->readyTasks.last - antMemory->readyTasks.first);
        
        task = ReadyTasksPop(antMemory, r + antMemory->readyTasks.first);
        AntAddTask(antMemory, task);

        processor = rand() % (gProcessors.length);
        AntAddProcessor(antMemory, processor);
    } else {
        task = SelectTask(graph, algodata, antMemory, metaHeuristics);
        AntAddTask(antMemory, task);

        processor = SelectProcessor(graph, algodata, antMemory, metaHeuristics);
        AntAddProcessor(antMemory, processor);
    }

    /* After selecting a processor,
        get the processor computation cost for selected task
        add it to the latest processor time
    */
    const int maxCommCostForSelectTask = antMemory->maxDependencyResolveTime[task];

    if (antMemory->latestProcessorTime[processor] < maxCommCostForSelectTask) {
        antMemory->latestProcessorTime[processor] = maxCommCostForSelectTask;
    }

    const int startTimeForTask = antMemory->latestProcessorTime[processor];
    antMemory->taskStartTime[task] = startTimeForTask;
    gNodes.data[task].startTime = startTimeForTask;

    antMemory->latestProcessorTime[processor] += gProcessors.data[processor].computeCosts[task];

    const int endTimeForTask = antMemory->latestProcessorTime[processor]; 
    antMemory->taskEndTime[task] = endTimeForTask;
    gNodes.data[task].endTime = endTimeForTask;


    for (int i = 0; i < graph[task].numNextTasks; ++i) {
        const int nextTask = graph[task].nextTasks[i];
        const int nextDependencyResolveTime = endTimeForTask + graph[task].commCosts[i];

        antMemory->readyTasks.numParents[nextTask]--;

        
        if (!antMemory->readyTasks.numParents[nextTask]) {
            antMemory->readyTasks.list[antMemory->readyTasks.capacity++] = nextTask;
        } 
        
        if (antMemory->maxDependencyResolveTime[nextTask] < nextDependencyResolveTime) {
            antMemory->maxDependencyResolveTime[nextTask] = nextDependencyResolveTime;
        }

    }


    // Compute complete time
    if (antMemory->completionTime < endTimeForTask) {
        antMemory->completionTime = endTimeForTask;
    }

    gNodes.data[task].processor = processor;
}


void BeginAlgorithm() {
    if (gACORNK.isRunning) return;

    InitACORNK();
    AllocACORNK();

    for (int i = 0; i < gProcessors.length * gNodes.length; ++i) {
        gACORNK.postMatrix[i] = DBL_MIN;
        gACORNK.preMatrix[i] = DBL_MIN;
    }

    gACORNK.isRunning = 1;

    PrecomputeValues(gACORNK.graph);

    for (int i = 0; i < gMetaHeuristics.numAnts; ++i) {
        AllocAnt(&gACORNK.ants[i]);
        InitAnt(&gACORNK.ants[i]);
    }

    printf("Running ACORNK...\n");
}


void EndAlgorithm() {
    gACORNK.isRunning = 0;

    for (int i = 0; i < gMetaHeuristics.numAnts; ++i) {
        FreeAnt(&gACORNK.ants[i]);
    }

    // Write to output file
    FILE *fp;
    fp = fopen("../config/output.txt", "w+");
    if (fp == NULL) {
        printf("Cannot output to file output.txt\n");
        return;
    }

    for (int i = 0; i < gNodes.length; ++i) {
        char out[MAX_LINE];
        snprintf(out, MAX_LINE, "%i %i\n", 
            gNodes.data[i].id,
            gNodes.data[i].processor
        );
        fprintf(fp, "%s", out);
    }
    fclose(fp);

    FreeACORNK();
    printf("ACORNK completed!\n");
}

int CompareScores(const void *a, const void *b) {
    AntMemory *antA = (AntMemory *) a;
    AntMemory *antB = (AntMemory *) b;

    return antA->completionTime - antB->completionTime;
}


void SetSolution() {
    for (int i = 0; i < gNodes.length; ++i) {
        gNodes.data[i].processor = gACORNK.bestAssignedProcess[i];
        gNodes.data[i].endTime = gACORNK.bestEndTime[i];
        gNodes.data[i].startTime = gACORNK.bestStartTime[i];
    }
}

void CopySolution() {
    AntMemory *antMemory = &gACORNK.ants[0];
    for (int i = 0; i < antMemory->sequenceSize; i += 2) {
       gACORNK.bestAssignedProcess[antMemory->sequence[i]] = antMemory->sequence[i + 1];
    }

    for (int i = 0; i < gNodes.length; ++i) {
        gACORNK.bestEndTime[i] = antMemory->taskEndTime[i];
        gACORNK.bestStartTime[i] = antMemory->taskStartTime[i];
    }
}

void UpdateACORNK(float deltaTime) {
    if (!gACORNK.isRunning) return;
    gACORNK.timeUntilNextUpdate -= deltaTime;
    if (gACORNK.timeUntilNextUpdate > 0) return;

    if (gACORNK.iterationNum == gMetaHeuristics.maxIteration) {
        SetSolution();
        EndAlgorithm();
        return;
    }

    // Run iteration
    //printf("first %i\n", antMemory->readyTasks.first);
    
    if (gACORNK.currentAnt < gMetaHeuristics.numAnts) {
        AntMemory *antMemory = &gACORNK.ants[gACORNK.currentAnt];
        
        if (antMemory->readyTasks.last == antMemory->readyTasks.first) {
            antMemory->readyTasks.last = antMemory->readyTasks.capacity;
        }

        AntSelectionIteration(gACORNK.graph, &gACORNK, antMemory, &gMetaHeuristics);

        if (antMemory->readyTasks.first == gNodes.length) {
            gACORNK.currentAnt++;
        }

    } else {
        // Update pheromones for the best ants
        qsort(gACORNK.ants, gMetaHeuristics.numAnts, sizeof(AntMemory), CompareScores);

        for (int i = 0; i < gMetaHeuristics.numBestAnts; ++i) {
            LocalPheromoneUpdate(&gACORNK, &gACORNK.ants[i], &gMetaHeuristics);
        }
        
        GlobalPheromoneUpdate(&gACORNK, &gACORNK.ants[0], &gMetaHeuristics);

        gACORNK.currentAnt = 0;

        if (gACORNK.bestSolution > gACORNK.ants[0].completionTime) {
            gACORNK.bestSolution = gACORNK.ants[0].completionTime;
            CopySolution();
        } else {
            gACORNK.bestSolutionIteration += 1;
        }

        gACORNK.iterationNum += 1;

        if (gACORNK.bestSolutionIteration % gMetaHeuristics.deltaIterationAging) {
            gACORNK.isAging = 1;
            AgePheromone(&gACORNK, &gACORNK.ants[0]);
            gACORNK.bestSolutionIteration = 0;
        } else {
            gACORNK.isAging = 0;
        }

        for (int i = 0; i < gMetaHeuristics.numAnts; ++i) {
            InitAnt(&gACORNK.ants[i]);
        }
    }

    gACORNK.timeUntilNextUpdate = UPDATE_TIME;
}

