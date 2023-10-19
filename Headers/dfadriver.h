#ifndef AUTOMATON_DFADRIVER_H
#define AUTOMATON_DFADRIVER_H

#define START_STATE 0
#define MAX_SYMBOLS 52
#define MAX_STATES 100
#define FINAL_CHARACTER (-1)
#define INVALID_CHARACTER (-1)
#define EPSILON (-2)

struct Registry {
    int step;
    int state;
    char symbol;
};

void init_dfa_driver(int **table, const int *accept, char** states, const char *symbols, int num_symbols, int num_states);
int dfa_driver(const char *in_string, struct Registry registry[], int *registryCount);

#endif //AUTOMATON_DFADRIVER_H
