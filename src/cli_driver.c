//
// Created by Rohan on 8/24/2020.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/stego.h"

#define MAX_TABLE 5
#define MAX_KEY 8
#define MAX_DATA 12
#define ENCODE "encode"
#define DECODE "decode"
#define MESSAGE "msg"
#define SOURCE "src"
#define DEST "dest"

void parse_args(char *argv[]);

struct node
{
    char key[MAX_KEY];
    char *value;
    struct node *next;
};

typedef struct node Node;

Node *tb[MAX_TABLE];
char keys[MAX_DATA][MAX_KEY];
int values[MAX_DATA];

void init()
{
    for ( int i = 0; i < MAX_TABLE; ++i )
    {
        Node *cur = tb[i];
        Node *tmp;
        while ( cur != NULL)
        {
            tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        tb[i] = NULL;
    }
}

void my_str_cpy(char *dest, char *src)
{
    while ((*dest++ = *src++) != '\0' )
    {}
}

int my_str_cmp(char *str1, char *str2)
{
    while ( *str1 != '\0' && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

int hash(char *str)
{
    int hash = 401;
    while ( *str != '\0' )
    {
        hash = ((hash << 4) + (int) (*str)) % MAX_TABLE;
        str++;
    }
    return hash % MAX_TABLE;
}

void add(char *key, char *value)
{
    Node *new_node = (Node *) malloc(sizeof(Node));
    my_str_cpy(new_node->key, key);
    new_node->value = value;
    new_node->next = NULL;
    int index = hash(key);
    if ( tb[index] == NULL)
    {
        tb[index] = new_node;
    }
    else
    {
        Node *cur = tb[index];
        while ( cur != NULL)
        {
            if ( my_str_cmp(cur->key, key) == 0 )
            {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }
        new_node->next = tb[index];
        tb[index] = new_node;
    }
}

char *find(char *key)
{
    int index = hash(key);
    Node *cur = tb[index];
    while ( cur != NULL)
    {
        if ( my_str_cmp(cur->key, key) == 0 )
        {
            return cur->value;
        }
        cur = cur->next;
    }

}

int main(int argc, char *argv[])
{
    char *stego_flag = argv[1];
    if ( strcmp(stego_flag, ENCODE) == 0 )
    {
        if ( argc != 8 )
        {
            printf("Expected eight arguments but got %d arguments", argc);
            exit(1);
        }
        parse_args(argv + 2);
        encode_driver(
                find(MESSAGE),
                find(SOURCE),
                find(DEST)
        );
    }
    else if ( strcmp(stego_flag, DECODE) == 0 )
    {
        if ( argc != 3 )
        {
            printf("Expected three arguments but got %d arguments", argc);
            exit(1);
        }
        decode_driver(argv[2]);
    }
    else
    {
        printf("Invalid command: %s", stego_flag);
        exit(1);
    }
}

void parse_args(char *argv[])
{
    init();
    char *c;
    printf("Parsed Arguments \n");
    while ((c = *argv++) != NULL)
    {
        if ( c[0] == '-' && c[1] == '-' )
        {
            add(c + 2, *argv++);
            printf("(%s => %s) \n", c + 2, find(c + 2));
        }
    }
}