#include <string.h>
#include "../Headers/dfadriver.h"

/// PROBLEM MODEL

int accept_states[MAX_STATES];
char original_symbols[MAX_SYMBOLS];
int tables_mappings[MAX_STATES][MAX_SYMBOLS];

/// HELPER FUNCTIONS

int symbol_to_int(char symbol, const char *symbols) {
  for (int i = 0; i < strlen(symbols); i++) {
    if (symbols[i] == symbol) {
      return i;
    }
  }

  if (symbol == ' '){
    return EPSILON;
  }

  return INVALID_CHARACTER;
}

/// DFA DRIVER

void init_dfa_driver(int **table, const int *accept, char **states, const char *symbols, int num_symbols, int num_states) {
  for (int i = 0; i < num_symbols; i++) {
    original_symbols[i] = symbols[i];
  }

  for (int i = 0; i < num_states; i++) {
    accept_states[i] = accept[i];
  }

  for (int i = 0; i < num_states; i++) {
    for (int j = 0; j < num_symbols; j++) {
      tables_mappings[i][j] = table[i][j];
    }
  }
}

int dfa_driver(const char *in_string, struct Registry registry[], int *registryCount) {
  int k = START_STATE;
  int inputInt[strlen(in_string) + 1];

  for (int i = 0; in_string[i]; i++) {
    char currentSymbol = in_string[i];
    inputInt[i] = symbol_to_int(currentSymbol, original_symbols);

    if (inputInt[i] == INVALID_CHARACTER) {
      registry[*registryCount].step = 0;
      registry[*registryCount].state = START_STATE;
      registry[*registryCount].symbol = currentSymbol;
      (*registryCount)++;

      return INVALID_CHARACTER;
    }
  }

  inputInt[strlen(in_string)] = FINAL_CHARACTER;

  registry[*registryCount].step = 0;
  registry[*registryCount].state = k;
  registry[*registryCount].symbol = ' ';
  (*registryCount)++;

  for (int i = 0; inputInt[i] != FINAL_CHARACTER; i++) {
    int symbolIndex = inputInt[i];

    if (symbolIndex == EPSILON){
      continue;
    }

    k = tables_mappings[k][symbolIndex];
    registry[*registryCount].step = i + 1;
    registry[*registryCount].state = k;
    registry[*registryCount].symbol = original_symbols[symbolIndex];
    (*registryCount)++;

    if (k == FINAL_CHARACTER) {
      break;
    }
  }

  if (k == FINAL_CHARACTER) {
    return 0;
  } else {
    return accept_states[k];
  }
}
