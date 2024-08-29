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

typedef struct s_token
{
    int type;
    int value;
    struct s_token *next;
    struct s_token *prev;
}               t_token;

int main (int argc, char **argv)
{
    char *input;
    int i = 0;
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
        else
        {
            i = 0;
            while (input[i] && input[i] != '\n')
            {
                t_token *token = (t_token *)malloc(sizeof(t_token));
                if (token == NULL)
                {
                    printf("Error: memory allocation failed\n");
                    return (1);
                }
                if (isspace(input[i]))
                    i++;
                else if (isdigit(input[i]))
                {
                    token->type = INTEGER;
                    token->value = atoi(&input[i]);
                    while (isdigit(input[i])) i++;
                }
                else if (input[i] == '+')
                {
                    token->type = PLUS;
                    token->value = 0;
                    i++;
                }
                else
                {
                    printf("Syntax error\n");
                    free(token);
                    return (1);
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
        }

        int sum = 0;
        t_token *tmp = head;
        while (tmp)
        {
            if (tmp->type == INTEGER)
            {
                printf("%d", tmp->value);
                sum += tmp->value;
            }
            else if (tmp->type == PLUS)
            {
                printf("+");
            }
            tmp = tmp->next;
        }
        printf(" = %d\n", sum);

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