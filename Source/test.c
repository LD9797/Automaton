#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Expressions {
    struct TreeNode** expressions;
    int* states;
    int length;
};

struct InnerStatesResult {
    int* states;
    int length;
};

struct TreeNode {
    char* value;
    struct TreeNode** children;
    int numChildren;
};

struct TreeNode* createNode(char* value) {
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    newNode->value = strdup(value);
    newNode->children = NULL;
    newNode->numChildren = 0;
    return newNode;
}

void intToString(int num, char* str, int maxLen) {
    snprintf(str, maxLen, "%d", num);
}

int isNumber(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

void findNumbers(struct TreeNode* node, struct InnerStatesResult* result) {
    if (node == NULL) {
        return;
    }

    if (isNumber(node->value)) {
        // Si el valor del nodo es un número, agrégalo a la lista de estados
        result->states[result->length++] = atoi(node->value);
    }

    for (int i = 0; i < node->numChildren; i++) {
        findNumbers(node->children[i], result);
    }
}

void addChild(struct TreeNode* parent, struct TreeNode* child) {
    parent->numChildren++;
    parent->children = (struct TreeNode**)realloc(parent->children, parent->numChildren * sizeof(struct TreeNode*));
    parent->children[parent->numChildren - 1] = child;
}

struct TreeNode* factorizeSubtree(struct TreeNode* node) {
    if (node == NULL || node->numChildren == 0) {
        return node;
    }

    for (int i = 0; i < node->numChildren; i++) {
        node->children[i] = factorizeSubtree(node->children[i]);
    }

    // Process required

    return node;
}

void factorize(struct TreeNode* node) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < node->numChildren; i++) {
        node->children[i] = factorizeSubtree(node->children[i]);
    }
}

void printTree(struct TreeNode* node) {
    if (node == NULL) {
        return;
    }

    printf("%s", node->value);

    if (node->numChildren > 0) {
        printf("(");
        for (int i = 0; i < node->numChildren; i++) {
            printTree(node->children[i]);
            if (i < node->numChildren - 1) {
                printf(", ");
            }
        }
        printf(")");
    }
}

struct InnerStatesResult InnerStates(struct TreeNode* node) {
    struct InnerStatesResult result;
    result.states = (int*)malloc(100 * sizeof(int));
    result.length = 0;

    findNumbers(node, &result);

    return result;
}

struct Expressions CalcEquations(int* estados, int tamEstados, int* aceptacion, int tamAceptacion, char** simbolos, int tamSimbolos, int** transiciones) {
    int* estadosExpresiones = (int*)malloc(tamEstados * sizeof(int));
    struct TreeNode** expresionesPorEstado = (struct TreeNode**)malloc(tamEstados * sizeof(struct TreeNode*));
    int* procRequeridos = (int*)malloc(tamEstados * sizeof(int));
    struct TreeNode** totalExpresiones = (struct TreeNode**)malloc(tamEstados * sizeof(struct TreeNode*));

    // Inicializa estadosExpresiones y procRequeridos según los estados de aceptación
    int contExpresionesPorEstado = 0;
    int contProcRequeridos = tamAceptacion;

    for (int i = 0; i < tamEstados; i++) {
        estadosExpresiones[i] = 0;
        procRequeridos[i] = 0;
    }

    for (int i = 0; i < tamAceptacion; i++) {
        estadosExpresiones[aceptacion[i] - 1] = 1; // Marcamos los estados de aceptación
        expresionesPorEstado[i] = createNode(""); // Inicializamos los árboles en estados de aceptación
    }

    // Inicializa totalExpresiones con nodos vacíos
    for (int i = 0; i < tamEstados; i++) {
        totalExpresiones[i] = createNode("");
    }

    for (int i = 0; i < tamEstados; i++) {
        for (int j = 0; j < tamSimbolos; j++) {
            int posArbol = transiciones[i][j] - 1;

            char representNum[10];
            intToString(estados[i], representNum, sizeof(representNum));
            addChild(totalExpresiones[posArbol], createNode(representNum));
            addChild(totalExpresiones[posArbol], createNode("*"));
            addChild(totalExpresiones[posArbol], createNode(simbolos[j]));
        }
    }

    // Procesa estados requeridos
    while (contProcRequeridos != 0) {
        int* tempRequeridos = (int*)malloc(tamEstados * sizeof(int));
        int tempCount = 0;

        for (int i = 0; i < tamEstados; i++) {
            tempRequeridos[i] = procRequeridos[i];
        }

        contProcRequeridos = 0;

        for (int i = 0; i < tamEstados; i++) {
            if (tempRequeridos[i] && !estadosExpresiones[i]) {
                estadosExpresiones[i] = 1;
                expresionesPorEstado[contExpresionesPorEstado] = totalExpresiones[i];
                contExpresionesPorEstado++;

                struct InnerStatesResult estadosEncontrados = InnerStates(totalExpresiones[i]);

                for (int j = 0; j < estadosEncontrados.length; j++) {
                    if (!procRequeridos[estadosEncontrados.states[j] - 1]) {
                        procRequeridos[estadosEncontrados.states[j] - 1] = 1;
                        contProcRequeridos++;
                    }
                }
            }
        }

        free(tempRequeridos);
    }

    free(procRequeridos);

    // Realiza un bucle para imprimir las expresiones
    for (int i = 0; i < tamEstados; i++) {
        if (estadosExpresiones[i]) {
            printf("Expresión para Estado %d: ", estados[i]);
            printTree(expresionesPorEstado[i]);
            printf("\n");
        }
    }

    struct Expressions result;
    result.expressions = expresionesPorEstado;
    result.states = estadosExpresiones;
    result.length = contExpresionesPorEstado;

    return result;
}

int main() {
    int estados[] = {1, 2, 3, 4};
    int aceptacion[] = {1, 2, 3};
    char* simbolos[] = {"A", "T", "C", "G"};
    int transiciones[4][4] = {{4, 2, 3, 1}, {1, 2, 3, 1}, {4, 2, 4, 1}, {4, 4, 4, 4}};

    struct Expressions result = CalcEquations(estados, 4, aceptacion, 3, simbolos, 4, transiciones);

    for (int i = 0; i < result.length; i++) {
        free(result.expressions[i]);
    }

    free(result.expressions);
    free(result.states);

    return 0;
}
