#include <iostream>
#include <cstdlib>
#include <chrono>
#include <cuda_runtime.h>

__global__ void multiplyMatrices(int* firstMatrix, int* secondMatrix, int* resultMatrix, int sizeOfMatrix) {
    int elementIndex = threadIdx.x + blockIdx.x * blockDim.x;

    if (elementIndex < sizeOfMatrix) {
        resultMatrix[elementIndex] = firstMatrix[elementIndex] * secondMatrix[elementIndex];
    }
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

    int* deviceMatrixA, * deviceMatrixB, * deviceResultMatrix;
    cudaMalloc((void**)&deviceMatrixA, memorySize);
    cudaMalloc((void**)&deviceMatrixB, memorySize);
    cudaMalloc((void**)&deviceResultMatrix, memorySize);

    cudaMemcpy(deviceMatrixA, hostMatrixA, memorySize, cudaMemcpyHostToDevice);
    cudaMemcpy(deviceMatrixB, hostMatrixB, memorySize, cudaMemcpyHostToDevice);


    auto start = std::chrono::high_resolution_clock::now();

    int threadsPerBlock = 256;
    int blocksPerGrid = (matrixSize + threadsPerBlock - 1) / threadsPerBlock;
    multiplyMatrices << <blocksPerGrid, threadsPerBlock >> > (deviceMatrixA, deviceMatrixB, deviceResultMatrix, matrixSize);

    cudaMemcpy(hostResultMatrix, deviceResultMatrix, memorySize, cudaMemcpyDeviceToHost);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "GPU Time: " << duration.count() << " seconds" << std::endl;

    std::cout << "First 10 elements of the result matrix:" << std::endl;
    for (int elementIndex = 0; elementIndex < 10; elementIndex++) {
        std::cout << hostResultMatrix[elementIndex] << " ";
    }
    std::cout << std::endl;

    cudaFree(deviceMatrixA);
    cudaFree(deviceMatrixB);
    cudaFree(deviceResultMatrix);
    free(hostMatrixA);
    free(hostMatrixB);
    free(hostResultMatrix);

    return 0;
}
