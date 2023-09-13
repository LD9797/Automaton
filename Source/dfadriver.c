#include <string.h>
#include "../Headers/dfadriver.h"
#include <stdlib.h>
#include <gtk/gtk.h>

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

int code(char c);
char **my_DFA_driver (int **Table, int *Accept, int(*code)(char c), const char *string, int state, char **Estados);

int code(char c)
{
    if (c == 'A') return 0;
    if (c == 'T') return 1;
    if (c == 'C') return 2;
    if (c == 'G') return 3;
    return 4;
}

char **my_DFA_driver (int **Table, int *Accept, int(*code)(char c), const char *string, int state, char **Estados) {
    // TODO -> Transiciones invalidas y todo eso.

    int k;
    k = state;

    char **transiciones = malloc(strlen(string) + 1  * sizeof(char *));

    for (int i = 0; i < strlen(string) ; ++i) {
        char buffer[50];
        char simbolo = string[i];
        int from_code = code(simbolo);

        snprintf(buffer, sizeof(buffer), "(%s) %d |%c| -> ", Estados[k], k, simbolo);
        transiciones[i] = strdup(buffer);
        printf("%s", transiciones[i]);

        k = Table[k][from_code];
        if(i == strlen(string) - 1) {
            snprintf(buffer, sizeof(buffer), "(%s) %d", Estados[k], k);
            transiciones[i + 1] = strdup(buffer);
            printf("%s", transiciones[i + 1]);
        }
    }

    return transiciones;
}

int something(int argc, char *argv[]) {

    int tabla[4][4] = { {3,1,2,0},
                        {0,1,2,0},
                        {3,1,3,0},
                        {3,3,3,3}};

    int **ptr_tabla = malloc(4 * sizeof(int *));
    for(int i = 0; i < 4; ++i) {
        ptr_tabla[i] = tabla[i];
    }

    int accept[4] = {1, 1, 1, 0};
    char *estados[] = {"Inicio", "Arriba", "Centro", "Sink"};
    char hilera[] = "CAT";

    char **transiciones = my_DFA_driver(ptr_tabla, accept, code, hilera, 0, estados);


    char *ultimo_estado = transiciones[strlen(hilera)];
    char final = ultimo_estado[strlen(ultimo_estado) - 1];
    printf("\nUltimo estado: %c\n", final);
    printf("Acepto 1 O Rechazo 0? %d", accept[final - '0']);
}
