#ifndef AUTOMATON_DFADRIVER_H
#define AUTOMATON_DFADRIVER_H

#define START_STATE 0
#define MAX_SYMBOLS 52
#define MAX_STATES 100
#define FINAL_CHARACTER (-1)
#define INVALID_CHARACTER (-1)

struct Registry {
    int step;
    int state;
    char symbol;
};

void init_DFA_driver(int **table, int *accept, char** states, char *symbols, int num_simbolos, int num_estados);
int DFA_driver(const char *string, struct Registry registry[], int *registryCount);

char **my_DFA_driver (int **Table,
                      char *simbolos,
                      const char *string, char **Estados);

#endif //AUTOMATON_DFADRIVER_H
