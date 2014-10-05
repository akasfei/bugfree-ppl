#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj-2.h"

char __eof__[2] = "$";
char __epsilon__[2] = "#";
char ** __terms__;
int __termsCount__;

TERM * getTerm (GRAMMAR * g, char * name)
{
  TERM * thisT = g->t;
  while (thisT != NULL)
  {
    if (strcmp(thisT->name, name) == 0)
      return thisT;
    thisT = thisT->next;
  }
  return NULL;
}

NONTERM * getNonTerm (GRAMMAR * g, char * name)
{
  NONTERM * thisNT = g->nt;
  while (thisNT != NULL)
  {
    if (strcmp(thisNT->name, name) == 0)
      return thisNT;
    thisNT = thisNT->next;
  }
  return NULL;
}

SYMBOL * getSymbol (GRAMMAR * g, char * name)
{
  SYMBOL * res;
  res = (SYMBOL *) malloc(sizeof(SYMBOL));
  res->symbol = getTerm(g, name);
  res->next = NULL;
  if (res->symbol != NULL)
  {
    res->type = TYPE_T;
    return res;
  }
  res->symbol = getNonTerm(g, name);
  if (res->symbol != NULL)
  {
    res->type = TYPE_NT;
    return res;
  }
  return NULL;
}

void initT (GRAMMAR * g)
{
  char buffer[255];
  char thisChar;
  int i = 0, count = 1;
  TERM * thisT = NULL;
  thisChar = getchar();
  while (thisChar != '#')
  {
    if (
      (i > 0 && thisChar >= '0' && thisChar <= '9') ||
      (thisChar >= 'A' && thisChar <= 'Z') ||
      (thisChar >= 'a' && thisChar <= 'z')
    )
    {
      buffer[i] = thisChar;
      i++;
    }
    else if (i > 0)
    {
      buffer[i] = '\0';
      if (thisT == NULL)
      {
        thisT = (TERM *) malloc(sizeof (TERM));
        g->t = thisT;
      }
      else
      {
        thisT->next = (TERM *) malloc(sizeof (TERM));
        thisT = thisT->next;
      }
      thisT->name = (char *) malloc((i + 1) * sizeof (char));
      strcpy(thisT->name, buffer);
      thisT->index = count;
      thisT->next = NULL;
      count++;
      i = 0;
    }
    thisChar = getchar();
  }
  // Build an array of terms
  TERM * t = NULL;
  __termsCount__ = count;
  __terms__ = (char **) malloc((__termsCount__ + 1) * sizeof(char *));
  __terms__[0] = __epsilon__;
  // printf("DEBUG: Added T count: %d\nTerms: ", __termsCount__);
  for (t = g->t, i = 1; t != NULL; t = t->next, i++)
  {
    __terms__[i] = t->name;
  }
  /*
  __sortedTerms__ = (int *) malloc((__termsCount__ + 1) * sizeof(int));
  for (i = 0; i <= __termsCount__; i++)
  {
    for (j = 0; j <= __termsCount__; j++)
  }
  */
}

void initNT (GRAMMAR * g)
{
  char buffer[255];
  char thisChar;
  int i = 0;
  NONTERM * thisNT = NULL;
  thisChar = getchar();
  while (thisChar != '#')
  {
    if (
      (i > 0 && thisChar >= '0' && thisChar <= '9') ||
      (thisChar >= 'A' && thisChar <= 'Z') ||
      (thisChar >= 'a' && thisChar <= 'z')
    )
    {
      buffer[i] = thisChar;
      i++;
    }
    else if (i > 0)
    {
      buffer[i] = '\0';
      if (thisNT == NULL)
      {
        thisNT = (NONTERM *) malloc(sizeof (NONTERM));
        g->nt = thisNT;
      }
      else
      {
        thisNT->next = (NONTERM *) malloc(sizeof (NONTERM));
        thisNT = thisNT->next;
      }
      thisNT->name = (char *) malloc((i + 1) * sizeof (char));
      strcpy(thisNT->name, buffer);
      thisNT->rules = NULL;
      thisNT->canGenStr = -1;
      thisNT->firstSet = 0;
      thisNT->followSet = 0;
      thisNT->next = NULL;
      i = 0;
      // printf("DEBUG: Added NT: %s\n", thisNT->name);
    }
    thisChar = getchar();
  }
}

void initRules (GRAMMAR * g)
{
  char buffer[255];
  char thisChar;
  int i = 0;
  NONTERM * thisNT = NULL;
  RULE * thisRule = NULL;
  while (1)
  {
    thisChar = getchar();
    // Get rule left side
    // printf("DEBUG: Rule LHS\n");
    while (thisChar != '-')
    {
      if (
        (i > 0 && thisChar >= '0' && thisChar <= '9') ||
        (thisChar >= 'A' && thisChar <= 'Z') ||
        (thisChar >= 'a' && thisChar <= 'z')
      )
      {
        buffer[i] = thisChar;
        i++;
      }
      thisChar = getchar();
      if (thisChar == '#')
      {
        thisChar = getchar();
        if (thisChar == '#')
          return;
        else
          ungetc(thisChar, stdin);
      }
    }
    if (getchar() != '>')
      return;
    buffer[i] = '\0';
    thisNT = getNonTerm(g, buffer);
    // printf("DEBUG: Rules RHS %s\n", buffer);
    if (thisNT == NULL)
      return;
    i = 0;
    thisChar = getchar();
    // Get rule right side
    SYMBOL * symbol = NULL;
    thisRule = (RULE *) malloc(sizeof(RULE));
    thisRule->symbols = NULL;
    thisRule->next = NULL;
    while (thisChar != '#')
    {
      if (
        (i > 0 && thisChar >= '0' && thisChar <= '9') ||
        (thisChar >= 'A' && thisChar <= 'Z') ||
        (thisChar >= 'a' && thisChar <= 'z')
      )
      {
        buffer[i] = thisChar;
        i++;
      }
      else if (i > 0)
      {
        buffer[i] = '\0';
        if (symbol == NULL)
        {
          symbol = getSymbol(g, buffer);
          thisRule->symbols = symbol;
        }
        else
        {
          symbol->next = getSymbol(g, buffer);
          symbol = symbol->next;
        }
        if (symbol == NULL)
          return;
        i = 0;
      }
      thisChar = getchar();
    }
    // No symbols specified - Add epsilon
    if (thisRule->symbols == NULL)
    {
      // printf("DEBUG: No symbols specified: %s\n", buffer);
      thisRule->symbols = (SYMBOL *) malloc(sizeof(SYMBOL));
      thisRule->symbols->symbol = NULL;
      thisRule->symbols->type = TYPE_E;
      thisRule->symbols->next = NULL;
    }
    // Add rule to NT
    if (thisNT->rules == NULL)
      thisNT->rules = thisRule;
    else
    {
      RULE * tmp;
      tmp = thisNT->rules;
      while (tmp->next != NULL)
        tmp = tmp->next;
      tmp->next = thisRule;
      // printf("DEBUG: Rule appended to: %s\n", thisNT->name);
    }
  }
}

GRAMMAR * newGrammar ()
{
  GRAMMAR * g = (GRAMMAR *)malloc(sizeof(GRAMMAR));
  g->t = NULL;
  g->nt = NULL;
  // printf("DEBUG: initT\n");
  initT(g);
  // printf("DEBUG: initNT\n");
  initNT(g);
  // printf("DEBUG: initRules\n");
  initRules(g);
  // printf("DEBUG: Grammar Inited\n");
  return g;
}
