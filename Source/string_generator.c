#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Node* parse_char_other(char **regex);
Node* parse_concatenation(char **regex);
Node* parse_or(char **regex);
Node* parse_kleene_star(char **regex);
Node* parse_regex(char **regex);

Node* parse_char_other(char **regex) {
  if (*regex[0] == '(') {
    (*regex)++;
    // Parse separately
    Node* node = parse_regex(regex);
    //
    (*regex)++;
    return node;
  } else {
    Node* node = create_node(CHAR, **regex);
    (*regex)++;
    return node;
  }
}

Node* parse_kleene_star(char **regex) {
  Node* node = parse_char_other(regex);
  while (*regex[0] == '*') {
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
    Node* concatNode = create_node(CONCAT, ' ');
    concatNode->left = node;
    concatNode->right = parse_kleene_star(regex);
    node = concatNode;
  }
  return node;
}

Node* parse_or(char **regex) {
  Node* node = parse_concatenation(regex);
  while (*regex[0] == '|') {
    Node* altNode = create_node(ALT, '|');
    // Altnode is placed as root.
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

char* generate_sample_string(Node* root, int starLoopCount, int chooseRight) {
  if (!root) return NULL;

  char *leftStr = generate_sample_string(root->left, starLoopCount, chooseRight);
  char *rightStr = generate_sample_string(root->right, starLoopCount, chooseRight);
  char *resultStr;

  switch (root->type) {
    case CHAR:
      resultStr = malloc(1 * sizeof(char));
      resultStr[0] = root->data;
      resultStr[1] = '\0';
      break;
    case ALT:
      if (chooseRight) {
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
      resultStr = malloc((strlen(leftStr) * starLoopCount + 1) * sizeof(char));
      resultStr[0] = '\0';
      for (int i = 0; i < starLoopCount; ++i) {
        strcat(resultStr, leftStr);
      }
      free(leftStr);
      break;
  }
  return resultStr;
}



int main() {
  char *regex = "(ab|(ac)*d)";
  Node* parseTree = parse_regex(&regex);

  printf("Parse Tree:\n");
  in_order_print_tree(parseTree, 0);

  char *sampleString = generate_sample_string(parseTree, 8, 1);
  printf("String: %s\n", sampleString);
  free(sampleString);

  return 0;
}