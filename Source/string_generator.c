#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Headers/string_generator.h"

// References:
// https://www.scirp.org/pdf/IIM20120400007_41988083.pdf
// https://stackoverflow.com/questions/26948724/create-syntax-tree-from-given-regular-expressions-for-re-to-dfa
// https://www.scaler.com/topics/binary-tree-in-c/
// https://stackoverflow.com/questions/3213827/how-to-iterate-over-a-string-in-c
// https://stackoverflow.com/questions/20080789/given-a-regular-expression-how-would-i-generate-all-strings-that-match-it
// https://en.cppreference.com/w/c/experimental/dynamic/strdup

// TODO: Think about epsilon

typedef enum { CHAR, STAR, CONCAT, ALT } NodeType;

typedef struct Node {
  // https://www.scaler.com/topics/binary-tree-in-c/
  NodeType type;
  char data;
  struct Node *left;
  struct Node *right;
} Node;

Node* create_node(NodeType type, char data);

Node* create_node(NodeType type, char data) {
  // https://www.scaler.com/topics/binary-tree-in-c/
  struct Node* newNode = malloc(sizeof(struct Node));
  newNode->type = type;
  newNode->data = data;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

/*
 * 1. Parse Or
 * 2. Parse Concatenation
 * 3. Parse Star
 * 4. Parse Char / Other
 */

Node* parse_or(char **regex);
Node* parse_concatenation(char **regex);
Node* parse_kleene_star(char **regex);
Node* parse_char_other(char **regex);
Node* parse_regex(char **regex);

Node* parse_char_other(char **regex) {
  if (*regex[0] == '(') {
    (*regex)++;
    // Parse separately
    Node* node = parse_regex(regex);
    (*regex)++;
    return node;
  } else {
    // Node as leaf
    Node* node = create_node(CHAR, **regex);
    (*regex)++;
    return node;
  }
}

Node* parse_kleene_star(char **regex) {
  Node* node = parse_char_other(regex);
  while (*regex[0] == '*') {
    // Node placed as root
    Node* starNode = create_node(STAR, '*');
    starNode->left = node;
    node = starNode;
    (*regex)++;
  }
  return node;
}

Node* parse_concatenation(char **regex) {
  Node* node = parse_kleene_star(regex);
  while (*regex[0] != '\0' && *regex[0] != '|' && *regex[0] != ')') {
    // Node placed as root
    Node* concatNode = create_node(CONCAT, '.');
    concatNode->left = node;
    concatNode->right = parse_kleene_star(regex);
    node = concatNode;
  }
  return node;
}

Node* parse_or(char **regex) {
  Node* node = parse_concatenation(regex);
  while (*regex[0] == '|') {
    // Node placed as root
    Node* altNode = create_node(ALT, '|');
    altNode->left = node;
    (*regex)++;
    altNode->right = parse_concatenation(regex);
    node = altNode;
  }
  return node;
}

Node* parse_regex(char **regex) {
  return parse_or(regex);
}

void in_order_print_tree(Node* root, int level) {
  if (!root) return;

  in_order_print_tree(root->left, level + 1);

  // Print blank spaces
  for (int i = 0; i < level; i++) printf("    ");
  switch (root->type) {
    case CHAR: printf("%c\n", root->data); break;
    case STAR: printf("*\n"); break;
    case CONCAT: printf(".\n"); break;
    case ALT: printf("|\n"); break;
  }

  in_order_print_tree(root->right, level + 1);
}

char* generate_sample_string(Node* root, int star_loop_count, int choose_right) {
  if (!root) return NULL;

  char *leftStr = generate_sample_string(root->left, star_loop_count, choose_right);
  char *rightStr = generate_sample_string(root->right, star_loop_count, choose_right);
  char *resultStr;

  switch (root->type) {
    case CHAR:
      resultStr = malloc(1 * sizeof(char));
      resultStr[0] = root->data;
      resultStr[1] = '\0';
      break;
    case ALT:
      if (choose_right) {
        resultStr = rightStr;
        free(leftStr);
      } else {
        resultStr = leftStr;
        free(rightStr);
      }
      break;
    case CONCAT:
      resultStr = malloc((strlen(leftStr) + strlen(rightStr) + 1) * sizeof(char));
      sprintf(resultStr, "%s%s", leftStr, rightStr);
      free(leftStr);
      free(rightStr);
      break;
    case STAR:
      resultStr = malloc((strlen(leftStr) * star_loop_count + 1) * sizeof(char));
      resultStr[0] = '\0';
      for (int i = 0; i < star_loop_count; ++i) {
        strcat(resultStr, leftStr);
      }
      free(leftStr);
      break;
  }
  return resultStr;
}

int stringInArray(char* string, char** array, int index) {
  for (size_t i = 0; i < index; i++) {
    if (array[i] != NULL && strcmp(string, array[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

char** valid_strings_array(char *regex){
  char* regex_copy = regex;
  Node* parse_tree = parse_regex(&regex_copy);
  char** array = malloc(ARRAY_SIZE * sizeof(char *));
    // Check for Kleene star in regex
    if (strchr(regex, '*') != NULL){
      for(int i = 0; i < ARRAY_SIZE; i++) {
          char* sample_string = generate_sample_string(parse_tree, i + 1, 1);
          if (i > 0 && stringInArray(sample_string, array, i)){
            sample_string = generate_sample_string(parse_tree, i + 1, 0);
          }
          array[i] = strdup(sample_string);
      }
    } else {
      for(int i = 0; i < ARRAY_SIZE; i++) {
        char* sample_string = generate_sample_string(parse_tree, 0, 0);
        if (i > 0 && stringInArray(sample_string, array, i)){
          sample_string = generate_sample_string(parse_tree, 0, 1);
        }
        array[i] = strdup(sample_string);
      }
    }
    free(parse_tree);
  return array;
}


int is_special_char(char c) {
  const char *special_chars = ".*+?|^$()[]{}";
  return strchr(special_chars, c) != NULL;
}

void replaceChar(char *str, char find, char replace) {
  if (str) {
    while (*str) {
      if (*str == find) {
        *str = replace;
      }
      str++;
    }
  }
}


void replaceChars(char *str, const char *list) {
  size_t list_len = strlen(list);
  char *p = str;
  char new_char;
  int rand_index;


  srand((unsigned int)time(NULL));


  while (*p != '\0') {
    if (strchr(list, *p) != NULL) {
      do {
        rand_index = rand() % list_len;
        new_char = list[rand_index];
      } while (new_char == *p);
      *p = new_char;
    }
    p++;
  }
}

void scramble_regex(char **regex_ptr){
  char *regex = *regex_ptr;
  int length = strlen(regex);
  if (length == 1){
    return;
  }
  char *result_regex = malloc(length * sizeof(char));
  int *indices = malloc(length * sizeof(int));

  int count = 0;
  for (int i = 0; i < length; i++) {
    if (!is_special_char(regex[i])) {
      indices[count++] = i;
    } else {
      result_regex[i] = regex[i];
    }
  }

  for (int i = 0; i < count; i++) {
    int j = rand() % count;
    while (regex[indices[j]] == regex[indices[i]] && regex[indices[j]] != ' ') {
      j = rand() % count;
    }
    result_regex[indices[i]] = regex[indices[j]];
  }

  free(indices);
  *regex_ptr = result_regex;
}

char* replaceUTF8Char(const char* str, const char* find, char replace) {
  size_t str_len = strlen(str);
  size_t find_len = strlen(find);

  char* new_str = malloc(str_len + 1);
  if (!new_str) {
    perror("malloc failed");
    return NULL;
  }

  const char* current = str;
  char* new_current = new_str;

  while (*current) {
    if (strncmp(current, find, find_len) == 0) {
      *new_current++ = replace;
      current += find_len;
    } else {
      *new_current++ = *current++;
    }
  }

  *new_current = '\0';

  return new_str;
}

char** invalid_strings_array(char *regex, char* symbols){
  if(strlen(regex) == 0){
    return valid_strings_array(regex);
  }
  char *copy_regex = regex;

  const char find[] = "ε"; // The UTF-8 encoded 'ε' character
  char* modifiedString = replaceUTF8Char(copy_regex, find, '>');
  replaceChar(modifiedString, '>', symbols[0]);

  // scramble_regex(&modifiedString);
  replaceChars(modifiedString, symbols);
  if(strlen(regex) != 1)
    modifiedString[strlen(regex)] = '\0';
  return valid_strings_array(modifiedString);
}

void test_parse_print_sample() {
  char *regex = "((b(b|ab)*aa|a))*";
  Node* parseTree = parse_regex(&regex);
  in_order_print_tree(parseTree, 0);
  char *sampleString = generate_sample_string(parseTree, 1, 0);
  printf("String: %s\n", sampleString);
  sampleString = generate_sample_string(parseTree, 8, 1);
  printf("String: %s\n", sampleString);
  free(sampleString);
}

/*
void test_scramble(){
  char *regex = "((b(b|ab)*aa|a))*";
  char *copy_regex = regex;
  scramble_regex(&copy_regex);
  printf("String: %s\n", copy_regex);
  Node* parseTree = parse_regex(&copy_regex);
  in_order_print_tree(parseTree, 0);
  char *sampleString = generate_sample_string(parseTree, 1, 0);
  printf("String: %s\n", sampleString);
}*/

void test_invalid(){
  char *regex = "((b(b|ab)*aa|a))*";
  //char** invalid_strings = invalid_strings_array(regex, ['a']);
  //for(int i = 0; i < ARRAY_SIZE; i++) {
   // printf("String: %s\n", invalid_strings[i]);
 // }
}

void test_valid(){
  char *regex = "(((ε|(T|CT)*(A|G))|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)|(((T|CT)*|((ε|(T|CT)*(A|G))|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*))|(CG|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*)*)(CG|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)) | (((T|CT)*|((ε|(T|CT)*(A|G))|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*))|(CG|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*)*) | (((ε|(T|CT)*(A|G))|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)C|(((T|CT)*|((ε|(T|CT)*(A|G))|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*))|(CG|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)((T|CT)|(T|CT)*)*)(C|(CG|((G|((T|CT)|(T|CT)*)(A|G))|CG)*)C))";
  char symbols[4] = {'A', 'C', 'T', 'G'};
  char** valid_strings = valid_strings_array(regex);
  char** invalid_strings = invalid_strings_array(regex, symbols);
  for(int i = 0; i < ARRAY_SIZE; i++) {
    printf("String: %s\n", invalid_strings[i]);
  }
}

