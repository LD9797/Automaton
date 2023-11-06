#include <stdlib.h>
#include "../Headers/latexdriver.h"
#include "../Headers/dfadriver.h"
#include <string.h>
#include <stdio.h>
#include "../Headers/string_generator.h"

static int accept_states[MAX_STATES];
static char original_symbols[MAX_SYMBOLS];
static char **states_names[MAX_STATES];
static int tables_mappings[MAX_STATES][MAX_SYMBOLS];
static int global_num_states;
static int global_num_symbols;
static char *regex_automaton;

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


int init_latex_driver(int **table, const int *accept, char **states, const char *symbols, int num_symbols, int num_states, char *regex) {
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
  regex_automaton = regex;

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


char* read_template(const char* template_file) {
  FILE* file = fopen(template_file, "r");

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* template = (char*)malloc(length + 1);

  size_t read_len = fread(template, 1, length, file);
  template[length] = '\0';

  fclose(file);
  return template;
}


int save_and_compile_dot(char *automaton_graph){
  FILE *file = fopen("fsm.dot", "w");
  fputs(automaton_graph, file);
  fclose(file);
  char *command = "dot -Tpdf -o fsm.pdf fsm.dot";
  int ret = system(command);
  return ret;
}


void remove_last_comma(char* str) {
  int len = strlen(str);
  for (int i = len - 1; i >= 0; i--) {
    if (str[i] == ',') {
      memmove(&str[i], &str[i + 1], len - i - 1);
      break;
    }
  }
}


char* dfa_math_components() {
  // Initial state q0
  char* q0 = malloc(strlen(states_names[0][0]) + 30);
  sprintf(q0, "%s%s", states_names[0][0], "\n\n");

  // Acceptance states -> F
  char* acceptance_states = malloc(1000);
  acceptance_states[0] = '\0';
  strcat(acceptance_states, "\\{");

  // All states -> Q
  char* states_list = malloc(1000);
  states_list[0] = '\0';
  strcat(states_list, "\\{");

  int accept_states_found = 0;

  for (int i = 0; i < global_num_states; i++) {
    if (accept_states[i] == 1) {
      accept_states_found = 1;
      strcat(acceptance_states, states_names[i][0]);
      strcat(acceptance_states, ", ");
    }
    strcat(states_list, states_names[i][0]);
    if(i != global_num_states - 1)
      strcat(states_list, ", ");
    else
      strcat(states_list, ",");
  }

  if(accept_states_found) {
    int len = strlen(acceptance_states);
    if (len > 0) {
      acceptance_states[len - 1] = '\0';
    }
  }

  strcat(acceptance_states, "\\}\n");

  strcat(states_list, "\\}\n");
  remove_last_comma(acceptance_states);
  remove_last_comma(states_list);

  // Alphabet SIGMA
  char* list_of_symbols = malloc(1000);
  list_of_symbols[0] = '\0';
  strcat(list_of_symbols, "\\{");

  for(int j = 0; j < global_num_symbols; j++) {
    char num_str[3] = {original_symbols[j], '\0'};
    if (j != 0) {
      strcat(list_of_symbols, ", ");
    }
    strcat(list_of_symbols, num_str);
  }

  strcat(list_of_symbols, "\\}\n\n");

  // LaTeX Table Header
  char* header = malloc(strlen("\\begin{tabular}{|c|}\n\\hline\n") + (global_num_symbols * 2) + 20);
  header[0] = '\0';
  strcat(header, "\\begin{tabular}{|c");
  for(int i = 0; i < global_num_symbols; i++) {
    strcat(header, "|c");
  }
  strcat(header, "|}\n\\hline\n");

  // Symbol Row
  char* symbol_row = malloc(global_num_symbols * 3);
  symbol_row[0] = '\0';
  strcat(symbol_row, "&");
  for(int i = 0; i < global_num_symbols; i++) {
    char num_str[3] = {original_symbols[i], '\0'};
    strcat(symbol_row, num_str);
    if (i == global_num_symbols - 1) {
      strcat(symbol_row, " \\\\ \\hline\n");
    } else {
      strcat(symbol_row, " & ");
    }
  }

  // LaTeX Table for Transitions
  char** latex_table = malloc(global_num_states * sizeof(char *));
  for (int i = 0; i < global_num_states; i++) {
    latex_table[i] = malloc(10000);
    sprintf(latex_table[i], "%s & ", states_names[i][0]);
    for (int j = 0; j < global_num_symbols; j++) {
      if (tables_mappings[i][j] == -1) {
        strcat(latex_table[i], "-");
      } else {
        strcat(latex_table[i], states_names[tables_mappings[i][j]][0]);
      }
      if (j == global_num_symbols - 1) {
        strcat(latex_table[i], " \\\\ \\hline\n");
      } else {
        strcat(latex_table[i], " & ");
      }
    }
  }

  // Calculate total length and allocate memory for the final string
  size_t total_length = strlen(q0) + strlen(acceptance_states) + strlen(states_list) +
      strlen(list_of_symbols) + strlen(header) + strlen(symbol_row) + 300;
  for (int i = 0; i < global_num_states; i++) {
    total_length += strlen(latex_table[i]);
  }

  char* dfa_components = malloc(total_length);
  sprintf(dfa_components, "\\noindent\n\nq0 = %sQ = %s$\\Sigma$ = %sF = %s\n $\\delta$ = \n%s%s",
          q0, states_list, list_of_symbols, acceptance_states, header, symbol_row);

  for(int i = 0; i < global_num_states; i++) {
    strcat(dfa_components, latex_table[i]);
    free(latex_table[i]);
  }
  strcat(dfa_components, "\\end{tabular}\n");

  // Free allocated memory
  free(q0);
  free(acceptance_states);
  free(states_list);
  free(list_of_symbols);
  free(header);
  free(symbol_row);
  free(latex_table);

  return dfa_components;
}


char* string_array_to_latex(char** strings_array){
  char* itemize_string = "\\begin{itemize}\n"
                         "%s"
                         "%s"
                         "%s"
                         "%s"
                         "%s"
                         "\\end{itemize}";

  size_t total_size = strlen(itemize_string);

  char* format = "\\item %s\n";
  char** formatted_strings = malloc(ARRAY_SIZE* sizeof(char *));
  for(int i = 0; i < ARRAY_SIZE; i++){
    size_t size = strlen(format) + strlen(strings_array[i]);
    total_size += size;
    char* formatted_string = malloc(size * sizeof(char *));
    sprintf(formatted_string, format, strings_array[i]);
    formatted_strings[i] = formatted_string;
  }

  char* string_array_latex = malloc(total_size * sizeof (char *));
  sprintf(string_array_latex, itemize_string,
          formatted_strings[0],
          formatted_strings[1],
          formatted_strings[2],
          formatted_strings[3],
          formatted_strings[4]);

  for(int i = 0; i < ARRAY_SIZE; i++){
    free(formatted_strings[i]);
  }
  free(formatted_strings);

  return string_array_latex;
}


char* generate_valid_strings() {
  char** valid_strings = valid_strings_array(regex_automaton);
  return string_array_to_latex(valid_strings);
}


char* generate_invalid_strings(){
  char** invalid_strings = invalid_strings_array(regex_automaton, original_symbols);
  return string_array_to_latex(invalid_strings);
}


int generate_display_latex_doc(char *automaton_graph){
  char *template = read_template("template.tex");

  char* components_subtitle = " \\subsection*{Components:}";

  char* components = dfa_math_components();

  // Formatting DFA graph for Latex.
  char* dfa_subtitle = " \\subsection*{DFA Graph:}";
  char* replaced = str_replace(automaton_graph, "digraph", "\\digraph[width=0.8\\columnwidth]");
  char* automaton_replaced = str_replace(replaced, "fsm", "{fsm}");

  char* sample_accepted_subtitle = " \\subsection*{Sample accepted strings:}";

  char* sample_accepted = generate_valid_strings();

  char* sample_rejected_subtitle = " \\subsection*{Sample rejected strings:}";

  char* sample_rejected = generate_invalid_strings();

  char* regex_subtitle = " \\subsection*{Associated Regular Expression:}";

  char *end = "\\end{document}";

  size_t size_document = strlen(components_subtitle) + strlen(dfa_subtitle) + strlen(automaton_replaced) +
      strlen(sample_accepted_subtitle) + strlen(sample_rejected_subtitle) + strlen(regex_subtitle) + strlen(end) +
          strlen(components) + strlen(template) + strlen(sample_accepted) + strlen(sample_rejected) + strlen(regex_automaton) + 2;

  char* document = malloc(size_document * sizeof (char*));

  sprintf(document, "%s %s %s %s %s %s %s %s %s %s $%s$ %s", template, components_subtitle, components,
          dfa_subtitle,
          automaton_replaced,
          sample_accepted_subtitle,
          sample_accepted,
          sample_rejected_subtitle,
          sample_rejected,
          regex_subtitle,
          regex_automaton,
          end );
//
//
  FILE *file = fopen("main.tex", "w");
  fputs(document, file);
  fclose(file);
//
  free(template);
  free(components);
  free(replaced);
  free(automaton_replaced);
  free(sample_accepted);
  free(sample_rejected);
  free(document);
//
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
  char *header = "digraph fsm { rankdir=LR; ";

  // Accept States
  size_t size_accept = 0;
  int accept_empty = 0;
  for(int i = 0; i < global_num_states; i++){
    if (accept_states[i] == 1) {
      accept_empty = 1;
      size_accept += strlen(*states_names[i]);
    }
  }

  char *list_accept_states = malloc(size_accept + MAX_STATES * sizeof(char*));
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
  char** transitions = malloc(((global_num_states * global_num_symbols) + 1) * sizeof(char*));
  int num_transitions = 1;

  size_t size_initial_transition = strlen("secret_node [style=invis, shape=point]; secret_node->%s [style=bold];") + strlen(*states_names[0]);
  char *initial_transition = malloc(size_initial_transition);
  sprintf(initial_transition, "secret_node [style=invis, shape=point]; secret_node->%s [style=bold];", *states_names[0]);

  transitions[0] = initial_transition;

  for (int i = 0; i < global_num_states; i++) {
    for (int j = 0; j < global_num_symbols; j++) {
     int table_destination = tables_mappings[i][j];
      if (table_destination == -1) continue;
      size_t len_transition = strlen("%s -> %s [label=\"%c\"]; ") + strlen(*states_names[i]) + strlen(*states_names[table_destination]) + 1;
      char *transition = malloc(len_transition);
      sprintf(transition, "%s -> %s [label=\"%c\"]; ", *states_names[i], *states_names[table_destination], original_symbols[j]);
      transitions[num_transitions] = transition;
      num_transitions++;
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
//
  save_and_compile_dot(automaton_graph);
  generate_display_latex_doc(automaton_graph);

  // Clean up
  free(list_accept_states);
  free(accept_states_nodes);
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

