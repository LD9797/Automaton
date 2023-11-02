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
  int result_graphviz = system("whereis graphviz > /dev/null 2>&1");
  if (result_pdf != 0 || result_evince != 0 || result_graphviz != 0){
    // Installing pdflatex and evince if not installed (requires root).
    // User must prompt the password in order to install.
    system("gnome-terminal --wait -- bash -c 'echo \"Enter your password to install pdflatex:\" && "
           "sudo apt-get install -y texlive-latex-base texlive-latex-recommended texlive-latex-extra "
           "&& sudo apt-get install -y evince "
           "&& sudo apt-get install -y graphviz;"
           " exec bash'");
    // Checking if the installation was successful
    result_pdf = system("which pdflatex > /dev/null 2>&1");
    result_evince = system("which evince > /dev/null 2>&1");
    result_graphviz = system("whereis graphviz > /dev/null 2>&1");
    if (result_pdf != 0 || result_evince != 0 || result_graphviz != 0){
      // Unsuccessfully installed.
      return 1;
    }
  }
  // Tools already installed.
  return 0;
}

int init_latex_driver(int **table, const int *accept, char **states, const char *symbols, int num_symbols, int num_states) {
  if(install_pdflatex_evince() == 1){
    return 1;
  }

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

char* str_replace(const char* original, const char* old_substring, const char* new_substring) {
  int occurrences = 0;
  const char* temp = original;
  while ((temp = strstr(temp, old_substring))) {
    occurrences++;
    temp += strlen(old_substring);
  }

  size_t new_length = strlen(original) + occurrences * (strlen(new_substring) - strlen(old_substring));
  char* result = (char*)malloc(new_length + 1);

  if (result) {
    temp = original;
    char* result_ptr = result;
    while (*temp) {
      const char* old_substring_ptr = strstr(temp, old_substring);
      if (old_substring_ptr) {
        size_t prefix_length = old_substring_ptr - temp;
        strncpy(result_ptr, temp, prefix_length);
        result_ptr += prefix_length;
        strcpy(result_ptr, new_substring);
        result_ptr += strlen(new_substring);
        temp = old_substring_ptr + strlen(old_substring);
      } else {
        strcpy(result_ptr, temp);
        break;
      }
    }
  }

  return result;
}

int save_and_compile_dot(char *automaton_graph){
  FILE *file = fopen("fsm.dot", "w");
  fputs(automaton_graph, file);
  fclose(file);
  char *command = "dot -Tpdf -o fsm.pdf fsm.dot";
  int ret = system(command);
  return ret;
}

int generate_display_latex_doc(char *automaton_graph){
  char *header = "\\documentclass{article}"
                 "\\usepackage[pdf]{graphviz}"
                 "\\begin{document}";
  char *end = "\\end{document}";

  char* replaced = str_replace(automaton_graph, "digraph", "\\digraph[width=0.8\\columnwidth]");
  char* automaton_replaced = str_replace(replaced, "fsm", "{fsm}");

  size_t size_document = strlen(header) + strlen(automaton_replaced) + strlen(end);
  char* document = malloc(size_document * sizeof (char*));
  sprintf(document, "%s %s %s", header, automaton_replaced, end);

  FILE *file = fopen("main.tex", "w");
  fputs(document, file);
  fclose(file);

  free(replaced);
  free(automaton_replaced);
  free(document);

  char *command = "pdflatex --shell-escape main.tex";
  int ret = system(command);
  if (ret == 0){
    char *display = "evince --presentation main.pdf &";
    system(display);
  }
  return ret;
}

void draw_graph() {
  // Header
  char *header = malloc(200);
  strcpy(header, "digraph fsm { rankdir=LR; ");

  // Accept States
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

  size_t final_size = strlen("node [shape = doublecircle];") + strlen("node [shape = circle];") + strlen(list_accept_states) + 1;
  char *accept_states_nodes = malloc(final_size * sizeof (char*));

  if (accept_empty == 1)
    sprintf(accept_states_nodes, "%s %s %s", "node [shape = doublecircle];", list_accept_states, "node [shape = circle];");
  else
    sprintf(accept_states_nodes, "%s %s", "node [shape = doublecircle];", "node [shape = circle];");


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

  save_and_compile_dot(automaton_graph);
  generate_display_latex_doc(automaton_graph);

  dfa_math_components(accept_states, original_symbols);

  // Clean up
  free(header);
  free(list_accept_states);
  free(accept_states_nodes);
  free(transitions);
  free(automaton_graph);
}

void dfa_math_components(int *Accept, const char * symbols) {
    //estado inicial
    char *q0 = *states_names[0];
    printf("q0 %s\n", q0);

    //estados de aceptación
    int n = sizeof(Accept);
    char acceptance_states[10000]; //string con los estados de aceptacion
    strcat(acceptance_states, "\\{");

    char states_list[10000]; //string con los estados de aceptacion
    strcat(states_list, "\\{");
    for (int i = 0; i < global_num_states; i++) {
        if (accept_states[i] == 1) {
            strcat(acceptance_states, *states_names[i]);
            strcat(acceptance_states, ", ");
        }
        //estados
        strcat(states_list, *states_names[i]);
        strcat(states_list, ", ");

    }
    strcat(acceptance_states, "\\}");
    strcat(states_list, "\\}");
    printf("Aceptacion: %s\n", acceptance_states);
    printf("Estados: %s\n", states_list);

    //Alfabeto
    char list_of_symbols[10000]; //string con los estados de aceptacion
    strcat(list_of_symbols, "\\{");

    for(int j = 0; j < global_num_symbols; j++) {
        char num_str[20];
        sprintf(num_str, "%c", original_symbols[j]);
        if (j != 0) {
            strcat(list_of_symbols, ", "); // Agregamos una coma y un espacio
        }
        strcat(list_of_symbols, num_str);
    }

    strcat(list_of_symbols, "\\}");
    printf("Alfabeto: %s\n", list_of_symbols);

    //Tabla
    printf("\\begin{tabular}{|c|c|c|}\n");
    printf("\\hline\n");

    for (int i = 0; i < global_num_states; i++) {
        for (int j = 0; j < global_num_symbols; j++) {
            printf("%d", tables_mappings[i][j]);

            if (j < 2) {
                printf(" & ");  // Separador entre columnas
            } else {
                printf(" \\\\ \\hline\n");  // Fin de la fila y separador horizontal
            }
        }
    }

    printf("\\end{tabular}\n");

}


// sudo apt update
// sudo apt-get install texlive-latex-recommended texlive-latex-extra -y
// sudo apt install graphviz
// graphviz.sty en el mismo directory que el .tex
// crear aut.dot
// dot -Tpdf -o aut.pdf aut.dot
// pdflatex --shell-escape main.tex
// https://jvgomez.github.io/pages/manually-install-latex-packages-in-ubuntu.html

