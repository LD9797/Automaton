#include <stdlib.h>
#include "../Headers/latexdriver.h"
#include "../Headers/dfadriver.h"
#include <string.h>
#include <stdio.h>

static int accept_states[MAX_STATES];
static char original_symbols[MAX_SYMBOLS];
static char **states_names[MAX_STATES];
static int tables_mappings[MAX_STATES][MAX_SYMBOLS];
static int global_num_states;
static int global_num_symbols;

int install_pdflatex_evince(){
  // Checking if tools are installed.
  int result_pdf = system("which pdflatex > /dev/null 2>&1");
  int result_evince =  system("which evince > /dev/null 2>&1");
  if (result_pdf != 0 || result_evince != 0) {
    // Installing pdflatex and evince if not installed (requires root).
    // User must prompt the password in order to install.
    system("gnome-terminal --wait -- bash -c 'echo \"Enter your password to install pdflatex:\" && "
           "sudo apt-get update && sudo apt-get install -y texlive-latex-base && sudo apt-get install -y evince; exec bash'");
    // Checking if the installation was successful
    result_pdf = system("which pdflatex > /dev/null 2>&1");
    result_evince = system("which evince > /dev/null 2>&1");
    if (result_pdf != 0 || result_evince != 0){
      // Unsuccessfully installed.
      return 1;
    }
  }
  // Tools already installed.
  return 0;
}

int init_latex_driver(int **table, const int *accept, char **states, const char *symbols, int num_symbols, int num_states) {
  //if(install_pdflatex_evince() == 1){
//    return 1;
//  }

  for (int i = 0; i < num_symbols; i++) {
    original_symbols[i] = symbols[i];
  }

  for (int i = 0; i < num_states; i++) {
    accept_states[i] = accept[i];
    states_names[i] = &states[i];
  }

  for (int i = 0; i < num_states; i++) {
    for (int j = 0; j < num_symbols; j++) {
      tables_mappings[i][j] = table[i][j];
    }
  }

  global_num_states = num_states;
  global_num_symbols = num_symbols;

  return 0;
}

void draw_graph() {
  // Header
  char *header = malloc(200);
  strcpy(header, "digraph fsm { rankdir=LR; ");

  // Accept States
  char *accept_states_nodes = malloc(200);
  strcpy(accept_states_nodes, "node [shape = doublecircle];");

  size_t size_accept = 0;
  int accept_empty = 0;
  for(int i = 0; i < global_num_states; i++){
    if (accept_states[i] == 1) {
      accept_empty = 1;
      size_accept += strlen(*states_names[i]);
    }
  }

  char *list_accept_states = malloc((size_accept) + MAX_STATES * sizeof(char*));
  list_accept_states[0] = '\0';

  for (int i = 0; i < global_num_states; i++) {
    if (accept_states[i] == 1) {
      strcat(list_accept_states, *states_names[i]);
      strcat(list_accept_states, " ");
    }
  }
  strcat(list_accept_states, ";");


  size_t new_size = strlen(accept_states_nodes) + strlen("node [shape = circle];") + strlen(list_accept_states) + 1;
  accept_states_nodes = realloc(accept_states_nodes, new_size);

  if (accept_empty == 1)
    strcat(accept_states_nodes, list_accept_states);
  strcat(accept_states_nodes, " node [shape = circle];");

  // Transitions
  char **transitions = malloc((global_num_states * global_num_symbols) * sizeof(char*));
  int num_transitions = 1;
  transitions[0] = malloc(200);
  sprintf(transitions[0], "secret_node [style=invis, shape=point]; secret_node->%s [style=bold];", *states_names[0]);

  for (int i = 0; i < global_num_states; i++) {
    for (int j = 0; j < global_num_symbols; j++) {
      int table_destination = tables_mappings[i][j];
      if (table_destination == -1) continue;

      char *transition = malloc(300);
      sprintf(transition, "%s -> %s [label=\"%c\"]; ", *states_names[i], *states_names[table_destination], original_symbols[j]);
      transitions[num_transitions++] = transition;
    }
  }

  // Put together
  size_t total_length = strlen(header) + strlen(accept_states_nodes) + 1;
  for (int i = 0; i < num_transitions; i++) {
    total_length += strlen(transitions[i]);
  }

  char *automaton_graph = malloc(total_length * sizeof(char*));
  sprintf(automaton_graph, "%s%s", header, accept_states_nodes);
  for (int i = 0; i < num_transitions; i++) {
    strcat(automaton_graph, transitions[i]);
    free(transitions[i]);
  }

  strcat(automaton_graph, "}");

  printf("String: %s\n", automaton_graph);

  // Clean up
  free(header);
  free(accept_states_nodes);
  free(list_accept_states);
  free(transitions);
  free(automaton_graph);
}




// sudo apt update
// sudo apt-get install texlive-latex-recommended texlive-latex-extra -y
// sudo apt install graphviz
// graphviz.sty en el mismo directory que el .tex
// crear aut.dot
// dot -Tpdf -o aut.pdf aut.dot
// pdflatex --shell-escape main.tex
// https://jvgomez.github.io/pages/manually-install-latex-packages-in-ubuntu.html

