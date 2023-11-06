#ifndef AUTOMATON_HEADERS_STRING_GENERATOR_H_
#define AUTOMATON_HEADERS_STRING_GENERATOR_H_

#define ARRAY_SIZE 5

char** valid_strings_array(char *regex);
char** invalid_strings_array(char *regex, char *symbols);

#endif //AUTOMATON_HEADERS_STRING_GENERATOR_H_
