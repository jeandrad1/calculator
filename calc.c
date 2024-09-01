#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <ctype.h>

#define INTEGER    1
#define PLUS       2
#define SUBS       3
#define PRODUCT    4
#define DIVIDE     5
#define LPAREN     6
#define RPAREN     7

typedef struct s_token {
    int type;
    int value;
    struct s_token *next;
    struct s_token *prev;
} t_token;

typedef struct s_ast {
    int type;
    int value;
    struct s_ast *left;
    struct s_ast *right;
} t_ast;

t_token *lexer(char *input) {
    int i = 0;
    t_token *head = NULL;
    t_token *current = NULL;

    while (input[i] && input[i] != '\n') {
        t_token *token = (t_token *)malloc(sizeof(t_token));
        if (token == NULL) {
            printf("Error: memory allocation failed\n");
            return NULL;
        }
        if (isspace(input[i])) {
            i++;
            free(token);
            continue;
        } else if (isdigit(input[i])) {
            token->type = INTEGER;
            token->value = atoi(&input[i]);
            while (isdigit(input[i])) i++;
        } else if(input[i] == '-') {
            token->type = SUBS;
            token->value = 0;
            i++;
        } else if (input[i] == '+') {
            token->type = PLUS;
            token->value = 0;
            i++;
        } else if (input[i] == '*') {
            token->type = PRODUCT;
            token->value = 0;
            i++;
        } else if (input[i] == '/') {
            token->type = DIVIDE;
            token->value = 0;
            i++;
        } else if (input[i] == '(') {
            token->type = LPAREN;
            token->value = 0;
            i++;
        } else if (input[i] == ')') {
            token->type = RPAREN;
            token->value = 0;
            i++;
        } else {
            printf("Syntax error\n");
            free(token);
            return NULL;
        }

        token->next = NULL;
        token->prev = current;

        if (current != NULL) {
            current->next = token;
        } else {
            head = token;
        }

        current = token;
    }

    return head;
}

t_ast *create_ast_node(int type, int value, t_ast *left, t_ast *right) {
    t_ast *node = (t_ast *)malloc(sizeof(t_ast));
    if (node == NULL) {
        printf("Error: memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

t_ast *parse_expression(t_token **current);

t_ast *parse_factor(t_token **current) {
    t_ast *node = NULL;
    if ((*current)->type == INTEGER) {
        node = create_ast_node(INTEGER, (*current)->value, NULL, NULL);
        *current = (*current)->next;
    } else if ((*current)->type == LPAREN) {
        *current = (*current)->next;
        node = parse_expression(current);
        if ((*current)->type != RPAREN) {
            printf("Syntax error: expected ')'\n");
            exit(1);
        }
        *current = (*current)->next;
    } else {
        printf("Syntax error: expected a number or '('\n");
        exit(1);
    }
    return node;
}

t_ast *parse_term(t_token **current) {
    t_ast *node = parse_factor(current);
    while (*current && ((*current)->type == PRODUCT || (*current)->type == DIVIDE)) {
        if ((*current)->type == PRODUCT) {
            *current = (*current)->next;
            node = create_ast_node(PRODUCT, 0, node, parse_factor(current));
        } else if ((*current)->type == DIVIDE) {
            *current = (*current)->next;
            node = create_ast_node(DIVIDE, 0, node, parse_factor(current));
        }
    }
    return node;
}

t_ast *parse_expression(t_token **current) {
    t_ast *node = parse_term(current);
    while (*current && ((*current)->type == PLUS || (*current)->type == SUBS)) {
        if ((*current)->type == PLUS) {
            *current = (*current)->next;
            node = create_ast_node(PLUS, 0, node, parse_term(current));
        } else if ((*current)->type == SUBS) {
            *current = (*current)->next;
            node = create_ast_node(SUBS, 0, node, parse_term(current));
        }
    }
    return node;
}

void print_ast(t_ast *node, int level) {
    if (node == NULL) return;
    print_ast(node->left, level + 1); // Print left subtree first
    for (int i = 0; i < level; i++) printf("    "); // Indentation for current level
    if (node->type == INTEGER) {
        printf("%d\n", node->value); // Print integer value
    } else {
        printf("%c\n", node->type == PLUS ? '+' : node->type == SUBS ? '-' : node->type == PRODUCT ? '*' : '/'); // Print operator
    }
    print_ast(node->right, level + 1); // Print right subtree
}

int evaluate_ast(t_ast *node) {
    if (node->type == INTEGER) {
        return node->value;
    } else if (node->type == PLUS) {
        return evaluate_ast(node->left) + evaluate_ast(node->right);
    } else if (node->type == SUBS) {
        return evaluate_ast(node->left) - evaluate_ast(node->right);
    } else if (node->type == PRODUCT) {
        return evaluate_ast(node->left) * evaluate_ast(node->right);
    } else if (node->type == DIVIDE) {
        int right = evaluate_ast(node->right);
        if (right == 0) {
            printf("Error: division by zero\n");
            exit(1);
        }
        return evaluate_ast(node->left) / right;
    }
    return 0;
}

void free_ast(t_ast *node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

int main(int argc, char **argv) {
    char *input;
    t_token *head = NULL;
    t_token *current = NULL;

    while (1) {
        input = readline("calc> ");

        if (strcmp(input, "exit") == 0) {
            printf("Bye !\n");
            return (0);
        }
        if (input == NULL) {
            printf("Error: unable to read input\n");
            return (1);
        }

        head = lexer(input);
        if (head == NULL) {
            continue;
        }

        current = head;
        t_ast *ast = parse_expression(&current);
        if (current != NULL) {
            printf("Syntax error: unexpected token\n");
            continue;
        }

        printf("AST:\n");
        print_ast(ast, 0);

        int result = evaluate_ast(ast);
        printf(" = %d\n", result);

        // Free the linked list
        while (head) {
            t_token *tmp = head;
            head = head->next;
            free(tmp);
        }
        current = NULL;

        // Free the AST
        free_ast(ast);
    }
    return (0);
}