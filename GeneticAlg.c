#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define populationSize 3
#define mutationFactor 0.2
#define mutationProbability 0.05
//These define the ranges that we want the randomly generated first generation to be in
//We can adjust later to be more exponentially small when moving from kp to ki to kd like Connor suggested
#define kpMin 1
#define kpMax 10
#define kiMin 1
#define kiMax 10
#define kdMin 1
#define kdMax 10
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
// Function to generate a random permutation in the range [min, max]
void generatePermutation(int min, int max, int result[]) {
    int size = max - min + 1;
    for (int i = 0; i < size; i++) {
        result[i] = min + i;
    }
    //Shuffles the array using Fisher-Yates algorithm
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Get a random index from [0, i]
        // Swap result[i] and result[j]
        int temp = result[i];
        result[i] = result[j];
        result[j] = temp;
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
    //printTuple(currentGeneration.generationValues[bestFitnessIndex]);
    newGeneration.generationValues[0].fitnessValue = -1;
    
    //Shuffles by creating a random permutation of 1 to populationSize - 1 for each of the attributes
    int kpIndices[populationSize];  // Array to store the permutation for kp
    generatePermutation(0, populationSize-1, kpIndices);
    int kiIndices[populationSize];  // Array to store the permutation for ki
    generatePermutation(0, populationSize-1, kiIndices);
    int kdIndices[populationSize];  // Array to store the permutation for kd
    generatePermutation(0, populationSize-1, kdIndices);

    //Note on the shuffling: Originally I had the shuffling so that it only swapped between the bottom n-1 tuples
    //but including the possibility for the tuples to take copy values from the best preforming increased the development a lot

    for(int i=1; i<populationSize;i++){
        //Shuffles the kp values
        newGeneration.generationValues[i].kp = currentGeneration.generationValues[kpIndices[i-1]].kp;
        //Shuffle ki values
        newGeneration.generationValues[i].ki = currentGeneration.generationValues[kiIndices[i-1]].ki;
        //Shuffle kd values
        newGeneration.generationValues[i].kd = currentGeneration.generationValues[kdIndices[i-1]].kd;
    }

    //Mutation of the values, this is controlled by the mutation factor which says how much the value will change
    //The mutation happens with a set probability for each individual value
    for(int i=1;i<populationSize;i++){
        int randNum = 100;
        //Determine if kp should mutate
        randNum = rand() % (100 + 1);
        if(randNum <= (mutationProbability*100)){
            //Determine if increase or decrease
            randNum = rand() % (2);
            if(randNum){ //Increase
                newGeneration.generationValues[i].kp = (1-0.2)*newGeneration.generationValues[i].kp; 
            }else{ //Decrease
                newGeneration.generationValues[i].kp = (1+0.2)*newGeneration.generationValues[i].kp;
            }
            printf("Mutating with val %d\n",randNum);
        }
        //Determine if ki should mutate
        randNum = rand() % (100 + 1);
        if(randNum <= (mutationProbability*100)){
            randNum = rand() % (2);
            if(randNum){ //Increase
                newGeneration.generationValues[i].ki = (1-0.2)*newGeneration.generationValues[i].ki; 
            }else{ //Decrease
                newGeneration.generationValues[i].ki = (1+0.2)*newGeneration.generationValues[i].ki;
            }
            //Determine if increase or decrease
            printf("Mutating with val %d\n",randNum);
        }
        //Determine if kd should mutate
        randNum = rand() % (100 + 1);
        if(randNum <= (mutationProbability*100)){
            //Determine if increase or decrease
            randNum = rand() % (2);
            if(randNum){ //Increase
                newGeneration.generationValues[i].kd = (1-0.2)*newGeneration.generationValues[i].kd;
            }else{ //Decrease
                newGeneration.generationValues[i].kd = (1+0.2)*newGeneration.generationValues[i].kd;
            }
            printf("Mutating with val %d\n",randNum);
        }
    }
    printf("Current lowest value:%lf\n", bestFitnessValue);
    return newGeneration; 
};

int main(){
    srand(time(NULL));
    valueTuple a={2,3,4,-1};
    valueTuple b={5,8,6,-1};
    valueTuple c={1,1,9,-1};
    generation gen = {{a,b,c}};
    
    for(int i=0;i<10;i++){
        gen = fitnessFunctionGeneration(gen);
        gen = nextGeneration(gen);
    }
    gen = fitnessFunctionGeneration(gen);
    printGeneration(gen);
    //printf("Random Value: %d\n", randomValue);
    return 0;
}
