#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

const char *__TOKENS__[] = {
  "IF",
  "WHILE",
  "DO",
  "THEN",
  "PRINT",
  "PLUS",
  "MINUS",
  "DIV",
  "MULT",
  "EQUAL",
  "COLON",
  "COMMA",
  "SEMICOLON",
  "LBRAC",
  "RBRAC",
  "LPAREN",
  "RPAREN",
  "NOTEQUAL",
  "GREATER",
  "LESS",
  "LTEQ",
  "GTEQ",
  "DOT",
  "NUM",
  "ID",
  "ERROR"
};

typedef struct node
{
  int line;
  int type;
  char *token;
  struct node *prev;
}NODE;

NODE* new_node(int line, int type, char* token)
{
  NODE *n = (NODE *) malloc(sizeof(NODE));
  if (n == NULL)
  {
    printf("MALLOC FAIL\n");
  }
  n->line = line;
  n->type = type;
  n->token = (char *) malloc(strlen(token) * sizeof(char));
  if (n->token == NULL)
  {
    printf("MALLOC FAIL\n");
  }
  strcpy(n->token, token);
  return n;
}

void print_list(NODE * head)
{
  NODE *n = head;
  while (n != NULL)
  {
    if (
      n->type == NUM ||
      strcmp(n->token, "cse340") == 0 ||
      strcmp(n->token, "programming") == 0 ||
      strcmp(n->token, "language") == 0)
      printf("%d %s %s\n", n->line, __TOKENS__[n->type - 1], n->token);
    n = n->prev;
  }
}

void delete_list(NODE *head)
{
  NODE *n = head, *r; // r means recycle bin
  while (n != NULL)
  {
    free(n->token);
    r = n;
    n = n->prev;
    free(r);
  }
}

int main (int argc, char *argv[])
{
  int fin = 0; // Finished?
  NODE *head = NULL, *node = NULL, *n_node;
  while (!fin)
  {
    getToken();
    if (head)
    {
      n_node = new_node(line, ttype, token);
      n_node -> prev = node;
      node = n_node;
      head = node;
    }
    else
      node = head = new_node(line, ttype, token);
    if (ttype == EOF || ttype == ERROR)
      fin = 1;
  }
  print_list(head);
  delete_list(head);
  return 0;
}
