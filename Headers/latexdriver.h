#ifndef AUTOMATON_HEADERS_LATEXDRIVER_H_
#define AUTOMATON_HEADERS_LATEXDRIVER_H_


int install_pdflatex_evince();
int init_latex_driver(int **table, const int *accept, char **states, const char *symbols, int num_symbols, int num_states);
void draw_graph();
char *dfa_math_components();
#endif //AUTOMATON_HEADERS_LATEXDRIVER_H_