#include "syntax.h"

struct type
{
  int seq;
  char * name;
  struct var* vars;
  struct type* next;
};

struct var
{
  char * name;
  struct var* next;
};

struct type * findType(struct type *, char *);
struct type * findVarType(struct type *, char *);
struct type * addType(struct type *, int, char *);
struct var * addVar(struct type *, char *);
struct var * addVarI(struct type *, char *);
struct type * findExprType(struct type *, struct exprNode *);
void setType(struct type *, int, int);

void readDecl(struct type *, struct declNode *);
void readVarDecl(struct type *, struct var_decl_listNode *);
void readTypeDecl(struct type *, struct type_decl_listNode *);
void readStmt(struct type *, struct stmt_listNode *);

void printAll(struct type *);
void printType(struct type *);
