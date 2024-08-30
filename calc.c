#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <ctype.h>

# define INTEGER    1
# define PLUS       2
# define SUBS       3
# define PRODUCT    4
# define DIVIDE     5

typedef struct s_token
{
    int type;
    int value;
    struct s_token *next;
    struct s_token *prev;
}               t_token;

t_token *lexer(char *input)
{
    int i = 0;
    t_token *head = NULL;
    t_token *current = NULL;

    while (input[i] && input[i] != '\n')
    {
        t_token *token = (t_token *)malloc(sizeof(t_token));
        if (token == NULL)
        {
            printf("Error: memory allocation failed\n");
            return NULL;
        }
        if (isspace(input[i]))
        {
            i++;
            free(token);
            continue;
        }
        else if (isdigit(input[i]))
        {
            token->type = INTEGER;
            token->value = atoi(&input[i]);
            while (isdigit(input[i])) i++;
        }
        else if(input[i] == '-')
        {
            token->type = SUBS;
            token->value = 0;
            i++;
        }
        else if (input[i] == '+')
        {
            token->type = PLUS;
            token->value = 0;
            i++;
        }
        else if (input[i] == '*')
        {
            token->type = PRODUCT;
            token->value = 0;
            i++;
        }
        else if (input[i] == '/')
        {
            token->type = DIVIDE;
            token->value = 0;
            i++;
        }
        else
        {
            printf("Syntax error\n");
            free(token);
            return NULL;
        }

        token->next = NULL;
        token->prev = current;

        if (current != NULL)
        {
            current->next = token;
        }
        else
        {
            head = token;
        }

        current = token;
    }

    // Validate the token sequence
    t_token *tmp = head;
    int expect_number = 1; // Expect a number first
    while (tmp)
    {
        if (expect_number && tmp->type != INTEGER)
        {
            printf("Syntax error: expected a number\n");
            return NULL;
        }
        if (!expect_number && tmp->type == INTEGER)
        {
            printf("Syntax error: expected an operator\n");
            return NULL;
        }
        expect_number = !expect_number;
        tmp = tmp->next;
    }
    if (expect_number == 1)
    {
        printf("Syntax error: expression cannot end with an operator\n");
        return NULL;
    }

    return head;
}

int main (int argc, char **argv)
{
    char *input;
    t_token *head = NULL;
    t_token *current = NULL;

    while(1)
    {
        input = readline("calc> ");

        if (strcmp(input, "exit") == 0)
        {
            printf("Bye !\n");
            return (0);
        }
        if (input == NULL)
        {
            printf("Error: unable to read input\n");
            return (1);
        }

        head = lexer(input);
        if (head == NULL)
        {
            continue;
        }

        int result = 0;
        t_token *tmp = head;
        int last_operator = PLUS; // Assume the first number is positive

        while (tmp)
        {
            if (tmp->type == INTEGER)
            {
                if (last_operator == PLUS)
                {
                    result += tmp->value;
                }
                else if (last_operator == SUBS)
                {
                    result -= tmp->value;
                }
                else if (last_operator == PRODUCT)
                {
                    result *= tmp->value;
                }
                else if (last_operator == DIVIDE)
                {
                    if (tmp->value == 0)
                    {
                        printf("Error: division by zero\n");
                        return (1);
                    }
                    result /= tmp->value;
                }
                printf("%d", tmp->value);
            }
            else if (tmp->type == PLUS)
            {
                last_operator = PLUS;
                printf("+");
            }
            else if (tmp->type == SUBS)
            {
                last_operator = SUBS;
                printf("-");
            }
            else if (tmp->type == PRODUCT)
            {
                last_operator = PRODUCT;
                printf("*");
            }
            else if (tmp->type == DIVIDE)
            {
                last_operator = DIVIDE;
                printf("/");
            }
            tmp = tmp->next;
        }
        printf(" = %d\n", result);

        // Free the linked list
        while (head)
        {
            t_token *tmp = head;
            head = head->next;
            free(tmp);
        }
        current = NULL;
    }
    return (0);
}