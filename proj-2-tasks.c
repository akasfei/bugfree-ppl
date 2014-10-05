#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj-2.h"

int strGenTestS (GRAMMAR * g, char * ntName)
{
  NONTERM * nt;
  nt = getNonTerm(g, ntName);
  if (nt == NULL)
    return 1;
  return strGenTest(nt);
}

int strGenTest (NONTERM * nt)
{
  int res = 1; // 1 --> yes, 0 --> no
  RULE * r;
  SYMBOL * s;
  // get calculated result
  if (nt->canGenStr >= 0)
    return nt->canGenStr;
  // set this NT to 0 first
  nt->canGenStr = 0;
  // if nt -> epsilon
  if (nt->rules->symbols->type == TYPE_E)
  {
    nt->canGenStr = 1;
    return 1;
  }
  for (r = nt->rules; r != NULL; r = r->next)
  {
    res = 1;
    for (s = r->symbols; s != NULL; s = s->next)
    {
      if (s->type == TYPE_NT)
      {
        // self-refering, cannot generate string with this rule
        if (strcmp(((NONTERM *)s->symbol)->name, nt->name) == 0)
        {
          res = 0;
          break;
        }
        // refers a nt that cannot generate string
        if (strGenTest((NONTERM *)s->symbol) == 0)
        {
          res = 0;
          break;
        }
      }
    }
    if (res == 1)
    {
      nt->canGenStr = 1;
      return 1;
    }
  }
  return res;
}

void calcFIRST (GRAMMAR * g)
{
  NONTERM * nt;
  RULE * r;
  SYMBOL * s;
  int changed = 1;
  int orig;
  // printf("DEBUG: calcFIRST\n");
  while (changed)
  {
    changed = 0;
    for (nt = g->nt; nt != NULL; nt = nt->next)
    {
      // printf("DEBUG: calcFIRST: %s\n", nt->name);
      orig = nt->firstSet;
      for (r = nt->rules; r != NULL; r = r->next)
      {
        // printf("DEBUG: calcFIRST: %s: rule\n", nt->name);
        for (s = r->symbols; s != NULL; s = s->next)
        {
          if (s->type == TYPE_T) // Rule I
          {
            nt->firstSet |= (1 << ((TERM *)s->symbol)->index);
            break;
          }
          if (s->type == TYPE_E) // Rule I
          {
            nt->firstSet |= 1;
            break;
          }
          // Rule III & IV; /2*2 to remove epsilon
          nt->firstSet |= ((NONTERM *)s->symbol)->firstSet / 2 * 2;
          // does not have EPSILON
          if (((NONTERM *)s->symbol)->firstSet % 2 == 0)
            break;
          if (s->next == NULL) // Rule V
            nt->firstSet |= 1;
        }
      }
      if (nt->firstSet != orig)
        changed += 1;
    }
  }
}

void calcFOLLOW (GRAMMAR * g)
{
  NONTERM * nt;
  RULE * r;
  SYMBOL * s, * sfollow;
  int changed = 1;
  long orig, modified;
  // printf("DEBUG: calcFOLLOW\n");
  while (changed)
  {
    // printf("DEBUG: calcFOLLOW: pass\n");
    changed = 0;
    orig = 0;
    modified = 0;
    g->nt->followSet |= 1; // Rule I
    for (nt = g->nt; nt != NULL; nt = nt->next)
      orig += nt->followSet;
    // printf("DEBUG: calcFOLLOW: orig: %x\n", orig);
    for (nt = g->nt; nt != NULL; nt = nt->next)
    {
      // printf("DEBUG: calcFOLLOW: %s: %x\n", nt->name, nt->followSet);
      for (r = nt->rules; r != NULL; r = r->next)
      {
        // printf("DEBUG: calcFOLLOW: %s: %x: rule\n", nt->name, nt->followSet);
        for (s = r->symbols; s != NULL; s = s->next)
        {
          if (s->type == TYPE_NT)
          {
            if (s->next == NULL)
            {
              ((NONTERM *)s->symbol)->followSet |= nt->followSet; // Rule II
              break;
            }
            for (sfollow = s->next; sfollow != NULL; sfollow = sfollow->next)
            {
              if (sfollow->type == TYPE_T)
              {
                // printf("DEBUG: calcFOLLOW: %s: %x: symbol: %s,%d\n", nt->name, nt->followSet, ((TERM *)sfollow->symbol)->name, ((TERM *)sfollow->symbol)->index);
                // Rule IV & V, next symbol is term
                ((NONTERM *)s->symbol)->followSet |= 1 << ((TERM *)sfollow->symbol)->index;
                break;
              }
              else
              {
                // Rule IV & V, next symbol is nonterm
                ((NONTERM *)s->symbol)->followSet |= ((NONTERM *)sfollow->symbol)->firstSet / 2 * 2;
                if (((NONTERM *)sfollow->symbol)->firstSet % 2 == 0)
                  break;
                if (sfollow->next == NULL)
                  ((NONTERM *)s->symbol)->followSet |= nt->followSet; // Rule III
              }
            }
          }
        }
      }
    }
    // printf("DEBUG: calcFOLLOW: modified\n");
    for (nt = g->nt; nt != NULL; nt = nt->next)
      modified += nt->followSet;
    // printf("DEBUG: calcFOLLOW: modified: %x\n", modified);
    if (orig != modified)
      changed = 1;
  }
}

void printStrTest (GRAMMAR * g)
{
  NONTERM * nt;
  for (nt = g->nt; nt != NULL; nt = nt->next)
  {
    // printf("DEBUG: strGenTest(%s)\n", nt->name);
    printf("%s: %s\n", nt->name, strGenTest(nt) ? "YES": "NO");
  }
}

// type 0 -> FIRST; 1 -> FOLLOW
void printSET (GRAMMAR * g, int type)
{
  int set, i, sorted;
  NONTERM * nt;
  RESULT * resHead = NULL, * resThis = NULL, * tmp;
  for (nt = g->nt; nt != NULL; nt = nt->next)
  {
    if (type)
      printf("FOLLOW(%s) = {", nt->name);
    else
      printf("FIRST(%s) = {", nt->name);
    resHead = NULL;
    resThis = NULL;
    if (type)
      set = nt->followSet;
    else
      set = nt->firstSet;
    for (i = 0; i <= __termsCount__; i++)
    {
      if (set % 2)
      {
        if (resHead == NULL || resThis == NULL) {
          resHead = (RESULT *) malloc(sizeof(RESULT));
          resThis = resHead;
          resThis->prev = NULL;
          resThis->next = NULL;
        }
        else
        {
          resThis->next = (RESULT *) malloc(sizeof(RESULT));
          resThis->next->prev = resThis;
          resThis = resThis->next;
          resThis->next = NULL;
        }
        if (type && i == 0)
          resThis->result = __eof__;
        else
          resThis->result = __terms__[i];
        // printf("DEBUG: result: %s\n", resThis->result);
      }
      set = set >> 1;
    }
    // bubble-sort the results
    sorted = 0;
    while (sorted == 0)
    {
      sorted = 1;
      for (resThis = resHead; resThis != NULL && resThis->next != NULL; resThis = resThis->next)
      {
        if (strcmp(resThis->result, resThis->next->result) > 0)
        {
          sorted = 0;
          if (resThis == resHead)
            resHead = resThis->next;
          if (resThis->prev != NULL)
            resThis->prev->next = resThis->next;
          resThis->next->prev = resThis->prev;
          resThis->prev = resThis->next;
          resThis->next = resThis->next->next;
          if (resThis->next != NULL)
            resThis->next->prev = resThis;
          resThis->prev->next = resThis;
        }
      }
    }
    resThis = resHead;
    while (resThis != NULL)
    {
      printf(" %s%s", resThis->result, resThis->next == NULL ? "": ",");
      tmp = resThis->next;
      free(resThis);
      resThis = tmp;
    }
    printf(" }\n");
  }
}
