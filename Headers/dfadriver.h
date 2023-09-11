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

#endif //AUTOMATON_DFADRIVER_H
