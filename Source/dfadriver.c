#include <string.h>
#include "../Headers/dfadriver.h"

/// PROBLEM MODEL

int acceptStates[MAX_STATES];
char originalSymbols[MAX_SYMBOLS];
int tableMappings[MAX_STATES][MAX_SYMBOLS];

/// HELPER FUNCTIONS

size_t numStates(char** states) {
    return sizeof(states) / sizeof(states[0]);
}

int symbolToInt(char symbol, const char* symbols) {
    for (int i = 0; i < strlen(symbols); i++) {
        if (symbols[i] == symbol) {
            return i;
        }
    }

    return INVALID_CHARACTER;
}

/// DFA DRIVER

void init_DFA_driver(int **table, int *accept, char** states, char *symbols) {
    size_t totalStates = numStates(states);
    size_t totalSymbols = strlen(symbols);

    for(int i = 0; i < totalSymbols; i++) {
        originalSymbols[i] = symbols[i];
    }

    for(int i = 0; i < totalStates; i++) {
        acceptStates[i] = accept[i];
    }

    for(int i = 0; i < totalStates; i++) {
        for (int j = 0; j < totalSymbols; j++) {
            tableMappings[i][j] = table[i][j] - 1;
        }
    }
}

int DFA_driver(char *string, struct Registry registry[], int *registryCount) {
    int k = START_STATE;
    int inputInt[strlen(string) + 1];

    for (int i = 0; string[i]; i++) {
        char currentSymbol = string[i];
        inputInt[i] = symbolToInt(currentSymbol, originalSymbols);

        if (inputInt[i] == INVALID_CHARACTER) {
            registry[*registryCount].step = 0;
            registry[*registryCount].state = START_STATE;
            registry[*registryCount].symbol = currentSymbol;
            (*registryCount)++;

            return INVALID_CHARACTER;
        }
    }

    inputInt[strlen(string)] = FINAL_CHARACTER;
    for (int i = 0; inputInt[i] != FINAL_CHARACTER; i++) {
        int symbolIndex = inputInt[i];
        k = tableMappings[k][symbolIndex];

        registry[*registryCount].step = i;
        registry[*registryCount].state = k;
        registry[*registryCount].symbol = originalSymbols[i];
        (*registryCount)++;

        if (k == FINAL_CHARACTER) {
            break;
        }
    }

    if (k == FINAL_CHARACTER) {
        return 0;
    } else {
        return acceptStates[k];
    }
}