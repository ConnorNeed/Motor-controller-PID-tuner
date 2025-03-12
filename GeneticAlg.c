#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define populationSize 3
#define mutationFactor 0.5
#define mutationProbability 0.1
typedef struct{
    double kp;
    double ki;
    double kd;
    double fitnessValue;
}valueTuple;
typedef struct{
    valueTuple generationValues[populationSize];
}generation;
void printTuple(valueTuple tup){
    printf("Kp=%lf, Ki=%lf, Kd=%lf, Fitness Value=%lf\n", tup.kp, tup.ki, tup.kd, tup.fitnessValue);
}
//Run fitness function on a specific tuple
valueTuple fitnessFunction(valueTuple tup){
    double fitness = (tup.kp * 2) - (tup.ki * 1.5) / (tup.kd * 0.34);
    tup.fitnessValue = fitness;
    valueTuple newTup = {tup.kp, tup.ki, tup.kd, fitness};
    return newTup;
}
//Run fitness function on a whole generation
generation fitnessFunctionGeneration(generation gen){
    for(int i=0;i<populationSize;i++){
        gen.generationValues[i] = fitnessFunction(gen.generationValues[i]);
    }
    return gen;
}
void printGeneration(generation gen){
    for(int i=0;i<populationSize;i++){
        printTuple(gen.generationValues[i]);
    }
}

generation nextGeneration(generation currentGeneration){
    generation newGeneration;
    double bestFitnessValue = INFINITY;
    int bestFitnessIndex = 0;
    int index = 0;
    //Find the minimum fitness value
    for(int i=0; i<populationSize; i++){
        double cur = currentGeneration.generationValues[i].fitnessValue;
        if(cur < bestFitnessValue){
            bestFitnessValue = cur;
            bestFitnessIndex = i;
        }
    }
    //Initialize the values of the array
    for(int i=0;i<populationSize;i++){
        valueTuple currentVal;
        currentVal.kp = -1;
        currentVal.ki = -1;
        currentVal.kd = -1;
        currentVal.fitnessValue = -1;
        newGeneration.generationValues[i] = currentVal;
    }
    //Store our best value in the next generation
    newGeneration.generationValues[0] = currentGeneration.generationValues[bestFitnessIndex];
    printTuple(currentGeneration.generationValues[bestFitnessIndex]);
    newGeneration.generationValues[0].fitnessValue = -1;
    
    //Shuffle kp values
    for(int i=1; i<populationSize;i++){
        int rd_num = (rand() % (populationSize-1)) + 1;
        newGeneration.generationValues[i].kp = currentGeneration.generationValues[rd_num].kp;
    }

    //Shuffle ki values
    for(int i=1; i<populationSize;i++){
        int rd_num = (rand() % (populationSize-1)) + 1;
        newGeneration.generationValues[i].ki = currentGeneration.generationValues[rd_num].ki;
    }
    //Shuffle kd values
    for(int i=1; i<populationSize;i++){
        int rd_num = (rand() % (populationSize-1)) + 1;
        newGeneration.generationValues[i].kd = currentGeneration.generationValues[rd_num].kd;
    }
    printf("Current lowest value:%lf\n", bestFitnessValue);
    return newGeneration; 
};
// Function to generate a random permutation in the range [min, max]
void generatePermutation(int min, int max, int result[]) {
    int size = max - min + 1;
    
    // Step 1: Fill the array with sequential numbers
    for (int i = 0; i < size; i++) {
        result[i] = min + i;
    }

    // Step 2: Shuffle the array using Fisher-Yates algorithm
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Get a random index from [0, i]
        
        // Swap result[i] and result[j]
        int temp = result[i];
        result[i] = result[j];
        result[j] = temp;
    }
}
int main(){
    srand(time(NULL));
    valueTuple a={2,3,4,-1};
    valueTuple b={5,8,6,-1};
    valueTuple c={1,1,9,-1};
    generation gen = {{a,b,c}};
    
    for(int i=0;i<100;i++){
        gen = fitnessFunctionGeneration(gen);
        gen = nextGeneration(gen);
    }
    int min = 1, max = 10;  // Define the range
    int size = max - min + 1;
    int result[size];  // Array to store the permutation
    
    generatePermutation(min, max, result);

    // Print the generated permutation
    printf("Random permutation of numbers from %d to %d:\n", min, max);
    for (int i = 0; i < size; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
    gen = fitnessFunctionGeneration(gen);
    printGeneration(gen);
    //printf("Random Value: %d\n", randomValue);
    return 0;
}
