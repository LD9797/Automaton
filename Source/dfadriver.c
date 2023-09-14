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
    for (int i = 0; inputInt[i] != FINAL_CHARACTER; i++) {
        int symbolIndex = inputInt[i];

        registry[*registryCount].step = i;
        registry[*registryCount].state = k;
        registry[*registryCount].symbol = originalSymbols[symbolIndex];
        (*registryCount)++;

        k = tableMappings[k][symbolIndex];
        if (k == FINAL_CHARACTER || inputInt[i+1] == FINAL_CHARACTER) {
            registry[*registryCount].step = i + 1;
            registry[*registryCount].state = k;
            registry[*registryCount].symbol = ' ';
            (*registryCount)++;
            break;
        }
    }

    if (k == FINAL_CHARACTER) {
        return 0;
    } else {
        return acceptStates[k];
    }
}




char **my_DFA_driver (int **Table,
                      char *simbolos,
                      const char *string, char **Estados) {
    // TODO -> Transiciones invalidas y todo eso.

    int k = 0; // Estado inicial siempre va a ser 0.

    char **transiciones = malloc((strlen(string)) * sizeof(char *));

    for (int i = 0; i < strlen(string) ; ++i) {
        char buffer[1000];
        char simbolo = string[i];
        int from_code = symbolToInt(simbolo, simbolos);

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

int nose_test_DFA(int argc, char *argv[]) {

    int tabla[1][1] = {{0}};

    //int tabla[4][4] = { {3,1,2,0},
     //                   {0,1,2,0},
      //                  {3,1,3,0},
       //                 {3,3,3,3}};

    int **ptr_tabla = malloc(4 * sizeof(int *));
    for(int i = 0; i < 1; ++i) {
        ptr_tabla[i] = tabla[i];
    }

    // int accept[4] = {1, 1, 1, 0};
    int accept[1] = {0};
    // char *estados[] = {"Inicio", "Arriba", "Centro", "Sink"};

    char *estados[] = {"Inicio"};

    const char hilera[] = "AAAA";
    // char simbolos[] = { 'A', 'T', 'C', 'G'};

    char simbolos[] = { 'A'};

    char **transiciones = my_DFA_driver(ptr_tabla, simbolos, hilera, estados);


    //my_DFA_driver(ptr_tabla, simbolos, "CATATATAGCGCT", estados);
    //my_DFA_driver(ptr_tabla, simbolos, "CATATATATAGCTACACA", estados);

    char *ultimo_estado = transiciones[strlen(hilera)];
    char final = ultimo_estado[strlen(ultimo_estado) - 1];
    printf("\nUltimo estado: %c\n", final);
    printf("Acepto 1 O Rechazo 0? %d", accept[final - '0' - 1]);

}
