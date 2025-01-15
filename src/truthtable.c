#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NAME_LENGTH 17

typedef struct {
    char name[MAX_NAME_LENGTH];
    int value;
} Variable;

typedef struct {
    char type[MAX_NAME_LENGTH];
    int *inputs;
    int *outputs;
    int *selectors;
    int inputCount;
    int outputCount;
    int selectorCount;
} Gate;

// Function to find a variable index
int findVariableIndex(Variable *variables, int varCount, const char *name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to add a variable
int addVariable(Variable *variables, int *varCount, const char *name) {
    strcpy(variables[*varCount].name, name);
    variables[*varCount].value = 0;
    return (*varCount)++;
}

// Function to simulate gates
void simulateGate(Gate *gate, Variable *variables) {
    if (strcmp(gate->type, "NOT") == 0) {
        variables[gate->outputs[0]].value = !variables[gate->inputs[0]].value;
    } else if (strcmp(gate->type, "AND") == 0) {
        variables[gate->outputs[0]].value = variables[gate->inputs[0]].value &&
                                            variables[gate->inputs[1]].value;
    } else if (strcmp(gate->type, "OR") == 0) {
        variables[gate->outputs[0]].value = variables[gate->inputs[0]].value ||
                                            variables[gate->inputs[1]].value;
    } else if (strcmp(gate->type, "XOR") == 0) {
        variables[gate->outputs[0]].value = variables[gate->inputs[0]].value ^
                                            variables[gate->inputs[1]].value;
    } else if (strcmp(gate->type, "NAND") == 0) {
        variables[gate->outputs[0]].value = !(variables[gate->inputs[0]].value &&
                                              variables[gate->inputs[1]].value);
    } else if (strcmp(gate->type, "NOR") == 0) {
        variables[gate->outputs[0]].value = !(variables[gate->inputs[0]].value ||
                                              variables[gate->inputs[1]].value);
    } else if (strcmp(gate->type, "DECODER") == 0) {
        int index = 0;
        for (int i = 0; i < gate->inputCount; i++) {
            index = (index << 1) | variables[gate->inputs[i]].value;
        }
        for (int i = 0; i < gate->outputCount; i++) {
            variables[gate->outputs[i]].value = (i == index) ? 1 : 0;
        }
    } else if (strcmp(gate->type, "MULTIPLEXER") == 0) {
        int index = 0;
        for (int i = 0; i < gate->selectorCount; i++) {
            index = (index << 1) | variables[gate->selectors[i]].value;
        }
        variables[gate->outputs[0]].value = variables[gate->inputs[index]].value;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    Variable variables[1024];
    Gate gates[1024];
    int varCount = 0, gateCount = 0;

    // Add constants 0 and 1
    strcpy(variables[varCount].name, "0");
    variables[varCount++].value = 0;
    strcpy(variables[varCount].name, "1");
    variables[varCount++].value = 1;

    // Read INPUT directive
    int inputCount;
    fscanf(file, " INPUT %d", &inputCount);
    for (int i = 0; i < inputCount; i++) {
        char buffer[MAX_NAME_LENGTH];
        fscanf(file, "%s", buffer);
        addVariable(variables, &varCount, buffer);
    }

    // Read OUTPUT directive
    int outputCount;
    fscanf(file, " OUTPUT %d", &outputCount);
    for (int i = 0; i < outputCount; i++) {
        char buffer[MAX_NAME_LENGTH];
        fscanf(file, "%s", buffer);
        addVariable(variables, &varCount, buffer);
    }

    // Read gates
    while (!feof(file)) {
        char buffer[MAX_NAME_LENGTH];
        if (fscanf(file, "%s", buffer) != 1) break;

        Gate *gate = &gates[gateCount++];
        strcpy(gate->type, buffer);
        gate->inputs = gate->outputs = gate->selectors = NULL;

        if (strcmp(buffer, "NOT") == 0 || strcmp(buffer, "PASS") == 0) {
            gate->inputCount = 1;
            gate->outputCount = 1;
        } else if (strcmp(buffer, "AND") == 0 || strcmp(buffer, "OR") == 0 ||
                   strcmp(buffer, "XOR") == 0 || strcmp(buffer, "NAND") == 0 ||
                   strcmp(buffer, "NOR") == 0) {
            gate->inputCount = 2;
            gate->outputCount = 1;
        } else if (strcmp(buffer, "DECODER") == 0) {
            fscanf(file, "%d", &gate->inputCount);
            gate->outputCount = 1 << gate->inputCount;
        } else if (strcmp(buffer, "MULTIPLEXER") == 0) {
            fscanf(file, "%d", &gate->selectorCount);
            gate->inputCount = 1 << gate->selectorCount;
            gate->outputCount = 1;
        }

        gate->inputs = malloc(gate->inputCount * sizeof(int));
        gate->outputs = malloc(gate->outputCount * sizeof(int));
        gate->selectors = malloc(gate->selectorCount * sizeof(int));

        for (int i = 0; i < gate->inputCount; i++) {
            fscanf(file, "%s", buffer);
            gate->inputs[i] = findVariableIndex(variables, varCount, buffer);
        }
        for (int i = 0; i < gate->selectorCount; i++) {
            fscanf(file, "%s", buffer);
            gate->selectors[i] = findVariableIndex(variables, varCount, buffer);
        }
        for (int i = 0; i < gate->outputCount; i++) {
            fscanf(file, "%s", buffer);
            int index = findVariableIndex(variables, varCount, buffer);
            if (index == -1) {
                index = addVariable(variables, &varCount, buffer);
            }
            gate->outputs[i] = index;
        }
    }
    fclose(file);

    // Simulate and print truth table
    int rowCount = 1 << inputCount;
    for (int row = 0; row < rowCount; row++) {
        for (int i = 0; i < inputCount; i++) {
            variables[2 + i].value = (row >> (inputCount - i - 1)) & 1;
        }

        for (int i = 0; i < gateCount; i++) {
            simulateGate(&gates[i], variables);
        }

        for (int i = 0; i < inputCount; i++) {
            printf("%d ", variables[2 + i].value);
        }
        printf("|");
        for (int i = 0; i < outputCount; i++) {
            printf(" %d", variables[2 + inputCount + i].value);
        }
        printf("\n");
    }

    // Free allocated memory
    for (int i = 0; i < gateCount; i++) {
        free(gates[i].inputs);
        free(gates[i].outputs);
        free(gates[i].selectors);
    }

    return 0;
}



