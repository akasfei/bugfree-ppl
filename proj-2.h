#ifndef __CSE340_PROJ_2__
#define __CSE340_PROJ_2__

#define TYPE_T 1
#define TYPE_NT 2
#define TYPE_E 3

typedef struct grammar
{
  struct term * t;
  struct nonterm * nt;
}GRAMMAR;

typedef struct term
{
  char * name;
  int index; // should start with 1
  struct term * next;
}TERM;

typedef struct nonterm
{
  char * name;
  struct nonterm * next;
  struct rule * rules;
  int canGenStr; // should be inited as -1
  int firstSet;
  int followSet;
}NONTERM;

typedef struct rule
{
  struct symbol * symbols;
  struct rule * next;
}RULE;

typedef struct symbol
{
  void * symbol;
  int type;
  struct symbol * next;
}SYMBOL;

typedef struct result
{
  char * result;
  struct result * next;
  struct result * prev;
}RESULT;

GRAMMAR * newGrammar (void);
TERM * getTerm(GRAMMAR *, char *);
NONTERM * getNonTerm(GRAMMAR *, char *);
SYMBOL * getSymbol(GRAMMAR *, char *);
void initT (GRAMMAR *);
void initNT (GRAMMAR *);
void initRules (GRAMMAR *);
int strGenTest (NONTERM *);
int strGenTestS (GRAMMAR *, char *);
void calcFIRST (GRAMMAR *);
void calcFOLLOW (GRAMMAR *);
void printStrTest (GRAMMAR *);
void printSET (GRAMMAR *, int);

extern char __eof__[2];
extern char __epsilon__[2];
extern char ** __terms__;
extern int __termsCount__;
// extern int * __sortedTerms__;

#endif
