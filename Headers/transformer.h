#ifndef AUTOMATON_HEADERS_TRANSFORMER_H_
#define AUTOMATON_HEADERS_TRANSFORMER_H_

char* dfa_to_regex(const int *accept_states, char* symbols, int** table, int num_symbols, int num_states);

#endif //AUTOMATON_HEADERS_TRANSFORMER_H_
