#include <string.h>
#include "../Headers/dfadriver.h"
#include <stdlib.h>
#include <gtk/gtk.h>

/// PROBLEM MODEL

int acceptStates[MAX_STATES];
char originalSymbols[MAX_SYMBOLS];
int tableMappings[MAX_STATES][MAX_SYMBOLS];

/// HELPER FUNCTIONS

int symbolToInt(char symbol, const char* symbols) {
    for (int i = 0; i < strlen(symbols); i++) {
        if (symbols[i] == symbol) {
            return i;
        }
    }

    return INVALID_CHARACTER;
}

/// DFA DRIVER

void init_DFA_driver(int **table, int *accept, char **states, char *symbols, int num_simbolos, int num_estados) {
    for(int i = 0; i < num_simbolos; i++) {
        originalSymbols[i] = symbols[i];
    }

    for(int i = 0; i < num_estados; i++) {
        acceptStates[i] = accept[i];
    }

    for(int i = 0; i < num_estados; i++) {
        for (int j = 0; j < num_simbolos; j++) {
            tableMappings[i][j] = table[i][j];
        }
    }
}

int DFA_driver(const char *hilera, struct Registry registry[], int *registryCount) {
    int k = START_STATE;
    int inputInt[strlen(hilera) + 1];

    for (int i = 0; hilera[i]; i++) {
        char currentSymbol = hilera[i];
        inputInt[i] = symbolToInt(currentSymbol, originalSymbols);

        if (inputInt[i] == INVALID_CHARACTER) {
            registry[*registryCount].step = 0;
            registry[*registryCount].state = START_STATE;
            registry[*registryCount].symbol = currentSymbol;
            (*registryCount)++;

            return INVALID_CHARACTER;
        }
    }

    inputInt[strlen(hilera)] = FINAL_CHARACTER;

    registry[*registryCount].step = 0;
    registry[*registryCount].state = k;
    registry[*registryCount].symbol = ' ';
    (*registryCount)++;

    for (int i = 0; inputInt[i] != FINAL_CHARACTER; i++) {
        int symbolIndex = inputInt[i];
        k = tableMappings[k][symbolIndex];

        registry[*registryCount].step = i + 1;
        registry[*registryCount].state = k;
        registry[*registryCount].symbol = originalSymbols[symbolIndex];
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
