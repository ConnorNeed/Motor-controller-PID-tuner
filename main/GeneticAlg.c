#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "pid.h"

#define populationSize 10
#define mutationFactor 0.2
#define mutationProbability 0.05
#define number_of_generations 2
//These define the ranges that we want the randomly generated first generation to be in
//We can adjust later to be more exponentially small when moving from kp to ki to kd like Connor suggested
#define kpMin 1
#define kpMax 10
#define kiMin 1
#define kiMax 10
#define kdMin 1
#define kdMax 10
#define target1 100
#define target2 180
#define target3 240
//Time between new target rpm values
#define delayTime 20


//***STRUCTS***
typedef struct{
    double kp;
    double ki;
    double kd;
    double fitnessValue;
}valueTuple;

typedef struct{
    valueTuple generationValues[populationSize];
}generation;

//***UTILITY FUNCTIONS***
void printTuple(valueTuple tup){
    printf("Kp=%lf, Ki=%lf, Kd=%lf, Fitness Value=%lf\n", tup.kp, tup.ki, tup.kd, tup.fitnessValue);
}
void printGeneration(generation gen){
    for(int i=0;i<populationSize;i++){
        printTuple(gen.generationValues[i]);
    }
}
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

//***GENETIC ALGORITHM FUNCTIONS***

//Run fitness function on a specific tuple, we will change this once we can run our hardware
valueTuple fitnessFunction(valueTuple tup){
    //send the tuple values and start timer
    pid_input_t new_values = {.Kd=tup.kd,.Ki=tup.ki,.Kp=tup.kp};
    set_k_values(new_values);
    //send the first target speed
    set_target_speed(target1);
    vTaskDelay(pdMS_TO_TICKS(delayTime * 1000));
    //20 secs later update speed
    set_target_speed(target2);
    vTaskDelay(pdMS_TO_TICKS(delayTime * 1000));
    //40 secs
    set_target_speed(target2);
    vTaskDelay(pdMS_TO_TICKS(delayTime * 1000));
    //60 secs end timer
    //Get error
    double fitness = get_total_error();
    //reset
    reset_pid();
    tup.fitnessValue = fitness;
    valueTuple newTup = {tup.kp, tup.ki, tup.kd, fitness};
    return newTup;
}
//Runs fitness function on a whole generation
generation fitnessFunctionGeneration(generation gen){
    for(int i=0;i<populationSize;i++){
        gen.generationValues[i] = fitnessFunction(gen.generationValues[i]);
    }
    return gen;
}

//Preform the selection, crossover and mutation steps of the genetic algorithm
generation nextGeneration(generation currentGeneration){
    generation newGeneration;
    //SELECTION STARTS HERE
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
    
    //CROSSOVER STARTS HERE
    //Shuffles by creating a random permutation of 1 to populationSize - 1 for each of the attributes
    int kpIndices[populationSize-1];  // Array to store the permutation for kp
    generatePermutation(1, populationSize-1, kpIndices);
    int kiIndices[populationSize];  // Array to store the permutation for ki
    generatePermutation(1, populationSize-1, kiIndices);
    int kdIndices[populationSize];  // Array to store the permutation for kd
    generatePermutation(1, populationSize-1, kdIndices);

    for(int i=1; i<populationSize;i++){
        //Shuffles the kp values
        newGeneration.generationValues[i].kp = currentGeneration.generationValues[kpIndices[i-1]].kp;
        //Shuffle ki values
        newGeneration.generationValues[i].ki = currentGeneration.generationValues[kiIndices[i-1]].ki;
        //Shuffle kd values
        newGeneration.generationValues[i].kd = currentGeneration.generationValues[kdIndices[i-1]].kd;
    }

    //MUTATION STARTS HERE
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
        }
        //Determine if ki should mutate
        randNum = rand() % (100 + 1);
        if(randNum <= (mutationProbability*100)){
            //Determine if increase or decrease
            randNum = rand() % (2);
            if(randNum){ //Increase
                newGeneration.generationValues[i].ki = (1-0.2)*newGeneration.generationValues[i].ki; 
            }else{ //Decrease
                newGeneration.generationValues[i].ki = (1+0.2)*newGeneration.generationValues[i].ki;
            }
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
        }
    }
    printf("Current lowest value:%lf\n", bestFitnessValue);
    return newGeneration; 
};

//Creates our first generation for values
generation initalGeneration(){
    generation initalGen;
    //Initialize the values of the array
    for(int i=0;i<populationSize;i++){
        valueTuple currentVal;
        currentVal.kp = -1;
        currentVal.ki = -1;
        currentVal.kd = -1;
        currentVal.fitnessValue = -1;
        initalGen.generationValues[i] = currentVal;
    }
    //Create the values randomly within the ranges set
    //NEED TO CHANGE: when we set actual max and min we need to convert the rand ints to sufficiently sized doubles
    for(int i=0; i<populationSize;i++){
        double kp = rand() % (kpMax) + kpMin;
        initalGen.generationValues[i].kp = kp;
        double ki = rand() % (kiMax) + kiMin;
        initalGen.generationValues[i].ki = ki;
        double kd = rand() % (kdMax) + kdMin;
        initalGen.generationValues[i].kd = kd;
    }
    return initalGen;

}

int main(){
    srand(time(NULL)); //For seeding random num gen
    generation gen = initalGeneration();
    //printGeneration(gen);
    for(int i=0;i<number_of_generations;i++){
        gen = fitnessFunctionGeneration(gen);
        gen = nextGeneration(gen);
    }
    gen = fitnessFunctionGeneration(gen);
    //printGeneration(gen);
    return 0;
}
