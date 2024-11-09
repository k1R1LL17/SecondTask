#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <Windows.h>

struct ThreadData {
    int* firstMatrix;
    int* secondMatrix;
    int* resultMatrix;
    int startIndex;
    int endIndex;
};

DWORD WINAPI multiplyMatrixPart(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    for (int elementIndex = data->startIndex; elementIndex < data->endIndex; ++elementIndex) {
        data->resultMatrix[elementIndex] = data->firstMatrix[elementIndex] * data->secondMatrix[elementIndex];
    }
    return 0;
}

int main() {
    int matrixSize = 4096;
    int memorySize = matrixSize * sizeof(int);


    int* hostMatrixA = (int*)malloc(memorySize);
    int* hostMatrixB = (int*)malloc(memorySize);
    int* hostResultMatrix = (int*)malloc(memorySize);

    srand(time(0));

    for (int elementIndex = 0; elementIndex < matrixSize; elementIndex++) {
        hostMatrixA[elementIndex] = rand() % 100 + 1;
        hostMatrixB[elementIndex] = rand() % 100 + 1;
    }

    int numberOfThreads = 16;
    int chunkSize = matrixSize / numberOfThreads;
    HANDLE* threadHandles = new HANDLE[numberOfThreads];
    ThreadData* threadDataArray = new ThreadData[numberOfThreads];

    auto start = std::chrono::high_resolution_clock::now();

    for (int threadIndex = 0; threadIndex < numberOfThreads; threadIndex++) {
        threadDataArray[threadIndex].firstMatrix = hostMatrixA;
        threadDataArray[threadIndex].secondMatrix = hostMatrixB;
        threadDataArray[threadIndex].resultMatrix = hostResultMatrix;
        threadDataArray[threadIndex].startIndex = threadIndex * chunkSize;
        threadDataArray[threadIndex].endIndex = (threadIndex == numberOfThreads - 1) ? matrixSize : (threadIndex + 1) * chunkSize;

        threadHandles[threadIndex] = CreateThread(
            NULL,
            0,
            multiplyMatrixPart,
            &threadDataArray[threadIndex],
            0,
            NULL
        );
    }

    WaitForMultipleObjects(numberOfThreads, threadHandles, TRUE, INFINITE);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "CPU with Windows API Time: " << duration.count() << " seconds" << std::endl;

    std::cout << "First 10 elements of the result matrix:" << std::endl;
    for (int elementIndex = 0; elementIndex < 10; elementIndex++) {
        std::cout << hostResultMatrix[elementIndex] << " ";
    }
    std::cout << std::endl;

    for (int threadIndex = 0; threadIndex < numberOfThreads; threadIndex++) {
        CloseHandle(threadHandles[threadIndex]);
    }
    delete[] threadHandles;
    delete[] threadDataArray;
    free(hostMatrixA);
    free(hostMatrixB);
    free(hostResultMatrix);

    return 0;
}
