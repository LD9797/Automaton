#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATES_LEN 4
#define SYMBOLS_LEN 4
#define STRING_LEN 50

#define INITIALIZE 0

struct Expression {
    char *value;
    int length;
};

struct Linked_node {
    int value;
    struct Linked_node *next;
};

struct Linked_list {
    int length;
    struct Linked_node *head;
    struct Linked_node *tail;
};

// List methods

struct Linked_list *initialize_list() {
    struct Linked_list *list = (struct Linked_list *)malloc(sizeof(struct Linked_list));

    list->length = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

struct Linked_node *initialize_node(int value) {
    struct Linked_node *node = (struct Linked_node *)malloc(sizeof(struct Linked_node));

    node->value = value;
    node->next = NULL;

    return node;
}

void insert_element(struct Linked_list *list, int value, int position) {
    struct Linked_node *node = initialize_node(value);

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        if (position < 0 || position >= list->length) {
            list->tail->next = node;
            list->tail = node;
        } else if (position == 0) {
            node->next = list->head;
            list->head = node;
        } else {
            int current_position = 0;
            struct Linked_node *current_node = list->head;

            while(current_node != NULL) {
                if (current_position + 1 == position) {
                    node->next = current_node->next;
                    current_node->next = node;
                    break;
                }

                current_node = current_node->next;
                current_position++;
            }
        }
    }

    list->length++;
}

void remove_element(struct Linked_list *list, int value) {
    struct Linked_node *previous_node = NULL;
    struct Linked_node *current_node = list->head;

    while(current_node != NULL) {
        if (current_node->value == value) {
            if (previous_node == NULL) {
                list->head = current_node->next;
            } else {
                previous_node->next = current_node->next;
            }

            free(current_node);
            return;
        }

        previous_node = current_node;
        current_node = current_node->next;
    }
}

int contains_element(struct Linked_list *list, int value) {
    struct Linked_node *current_node = list->head;

    while(current_node != NULL) {
        if (current_node->value == value) {
            return 1;
        }

        current_node = current_node->next;
    }

    return 0;
}

// Expression methods

void print_table(struct Expression **expression_table, int rows, int columns) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%s\t", (expression_table[i][j].value[0] != '\0') ? expression_table[i][j].value : "-");
        }
        printf("\n");
    }
}

void modify_expression(struct Expression *expression, const char *value, int length, int rule) {
    if (rule == INITIALIZE) {
        if (expression->length == 0) {
            strcpy(expression->value, value);
            expression->length = (int)strlen(expression->value);
        } else {
            char temp[STRING_LEN];
            char *open_paren = strchr(expression->value, '(');
            char *close_paren = strrchr(expression->value, ')');

            if (open_paren != NULL && close_paren != NULL && open_paren < close_paren) {
                *close_paren = '\0';
                snprintf(temp, sizeof(temp), "%s+%s", expression->value + 1, value);
            } else {
                snprintf(temp, sizeof(temp), "(%s+%s)", expression->value, value);
            }

            strcpy(expression->value, temp);
            expression->length = (int)strlen(expression->value);
        }
    } else {

    }
}

struct Expression ** create_expression_table(int **transitions, char **symbols, int states_len, int symbols_len, int verbose) {
    int rows = states_len;
    int columns = states_len + 1;

    struct Expression **expression_table = (struct Expression **)malloc(rows * sizeof(struct Expression *));
    for (int i = 0; i < rows; i++) {
        expression_table[i] = (struct Expression *)malloc(columns * sizeof(struct Expression));
        for (int j = 0; j < columns; j++) {
            expression_table[i][j].length = 0;
            expression_table[i][j].value = (char *)malloc(STRING_LEN * sizeof(char));
            expression_table[i][j].value[0] = '\0';
        }
    }

    modify_expression(&expression_table[0][columns - 1], "ε", 1, INITIALIZE);
    for (int i = 0; i < states_len; i++) {
        for (int j = 0; j < symbols_len; j++) {
            modify_expression(&expression_table[transitions[i][j] - 1][i], symbols[j], 1, INITIALIZE);
        }
    }

    if (verbose == 1) {
        print_table(expression_table, rows, columns);
    }

    return expression_table;
}

void process_expression_table(struct Expression **expression_table, const int *accept, char **symbols, int states_len, int symbols_len, int accept_len, int verbose) {
    struct Linked_list *pending = initialize_list();

    for (int i = 0; i < accept_len; i++) {
        insert_element(pending, accept[i], -1);
    }

    do {
        for (int i = 0; i < pending->length; i++) {
            int expression_checked = accept[i];

            for (int j = 0; j < states_len; j++) {
                struct Expression *current_expression = &expression_table[expression_checked][j];
                if (expression_checked != j && current_expression->length != 0) {
                    if (expression_table[j][j].length == 0) {
                        // TODO: resolve_expression(&expression_table, expression_checked, j);
                    } else {
                        struct Linked_list *excluded = initialize_list();
                        insert_element(excluded, expression_checked, -1);
                        // TODO: int resolve_expression_dependency(&expression_table, j, &excluded);
                        // TODO: if 1 try resolve_expression(&expression_table, expression_checked, j);
                        // TODO: free excluded_list memory
                    }
                }
            }

            // TODO: check if expression is ready for final arden
        }

        // TODO: remove from pending those states that have received a final arden
    } while (pending->length != 0);

    if (verbose == 1) {
        print_table(expression_table, states_len, states_len + 1);
    }
}

int main() {
    int accept_states[] = {1, 1, 1, 0};
    char *symbols[] = {"A", "T", "C", "G"};
    int table[STATES_LEN][STATES_LEN] = {
            {4, 2, 3, 1},
            {1, 2, 3, 1},
            {4, 2, 4, 1},
            {4, 4, 4, 4}
    };

    // Adapt table to readable format
    int *transitions[STATES_LEN];
    for (int i = 0; i < STATES_LEN; i++) {
        transitions[i] = table[i];
    }

    // Convert one-hot-vector and states
    int accept_len = 0;
    int *states = (int *)malloc(STATES_LEN * sizeof(int));
    int *accept = (int *)malloc(STATES_LEN * sizeof(int));
    for (int i = 0; i < STATES_LEN; i++) {
        states[i] = i;
        if (accept_states[i] == 1) {
            accept[accept_len] = accept_len;
            accept_len++;
        }
    }

    struct Expression **expression_table = create_expression_table(transitions, symbols, STATES_LEN, SYMBOLS_LEN, 1);
    process_expression_table(expression_table, accept, symbols, STATES_LEN, SYMBOLS_LEN, accept_len, 1);
    // TODO: free { expression_table, states, accept } memory
    return 0;
}