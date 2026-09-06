#include <cstdio>
#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);


//
// workerThreadStart --
//
// Thread entrypoint.
// DOUBLE SPLIT THREAD WORKLOAD
// void workerThreadStart(WorkerArgs * const args) {

//     int rowsPerThread =  args->height / args->numThreads + static_cast<int>(args->height % args->numThreads != 0);
    
//     printf("Thread %d start working\n", args->threadId);
//     double startTime = CycleTimer::currentSeconds();
    
//     for (int partIndex = 0; partIndex < args->numThreads; ++partIndex){
//         int rowsPerPart = rowsPerThread / args->numThreads + static_cast<int>(rowsPerThread % args->numThreads != 0);

//         int threadStartRow = rowsPerThread * partIndex + rowsPerPart * args->threadId;
        
//         int threadNumRows = std::min(rowsPerPart, static_cast<int>(args->height - rowsPerPart));
//         mandelbrotSerial(args->x0,args->y0,args->x1,args->y1,args->width,args->height,threadStartRow, threadNumRows,args->maxIterations, args->output);
//     }

//     double endTime = CycleTimer::currentSeconds();
//     printf("Work done by thread %d by %.3f ms\n", args->threadId, (endTime - startTime) * 1000);
// }

// SPLIT BY SINGLE ROWS
void workerThreadStart(WorkerArgs * const args) {

    printf("Thread %d start working\n", args->threadId);
    double startTime = CycleTimer::currentSeconds();
    for (int row = args->threadId; row < args->height; row += args->numThreads){
        mandelbrotSerial(args->x0,args->y0,args->x1,args->y1,args->width,args->height,row, 1,args->maxIterations, args->output);
    }


    double endTime = CycleTimer::currentSeconds();
    printf("Work done by thread %d by %.3f ms\n", args->threadId, (endTime - startTime) * 1000);
}

// BASIC THREAD WORKLOAD SPLIT
// void workerThreadStart(WorkerArgs * const args) {

//     int rowsPerThread =  args->height / args->numThreads + static_cast<int>(args->height % args->numThreads != 0);

//     int threadStartRow = rowsPerThread * args->threadId;
//     int threadNumRows = std::min(rowsPerThread, static_cast<int>(args->height - threadStartRow));

//     printf("Thread %d start working\n", args->threadId);
//     mandelbrotSerial(args->x0,args->y0,args->x1,args->y1,args->width,args->height,threadStartRow, threadNumRows,args->maxIterations, args->output);
//     printf("Work done by thread %d\n", args->threadId);
// }


//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 64;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {

        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
      
        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

