#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "proj-3.h"

int _typeCount_ = 0;
char _int_[] = "INT";
char _real_[] = "REAL";
char _string_[] = "STRING";
char _boolean_[] = "BOOLEAN";

enum TokenTypes
{
  VAR = 1, WHILE, INT, REAL, STRING, BOOLEAN, TYPE,
  PLUS, MINUS, DIV, MULT, EQUAL,
  COLON, COMMA, SEMICOLON,
  LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
  NOTEQUAL, GREATER, LESS, LTEQ, GTEQ, DOT,
  ID, NUM, REALNUM,
  ERROR
};

struct type * findType(struct type * t, char * name)
{
  printf("DEBUG: findType %s\n", name);
  while (t != NULL)
  {
    if (t->name != NULL && strcmp(t->name, name) == 0)
      return t;
    t = t->next;
  }
  return NULL;
}

struct type * findVarType(struct type * t, char * name)
{
  struct var * v;
  while (t != NULL)
  {
    v = t->vars;
    while (v != NULL)
    {
      if (strcmp(v->name, name) == 0)
        return t;
      v = v->next;
    }
    t = t->next;
  }
  return NULL;
}

struct type * addType(struct type * t, int seq, char * name)
{
  printf("DEBUG: addType %s\n", name);
  struct type * newType;
  if (findType(t, name) != NULL)
  {
    printf("ERROR CODE 0\n");
    exit(0);
  }
  if (seq == 0)
    seq = 10 + _typeCount_;
  _typeCount_ += 1;
  if (t->seq < 10)
  {
    t->seq = seq;
    t->name = name;
    return t;
  }
  else
  {
    while (t->next != NULL)
      t = t->next;
    newType = ALLOC(struct type);
    newType->seq = seq;
    newType->name = name;
    newType->vars = NULL;
    newType->next = NULL;
    t->next = newType;
    return newType;
  }
  return NULL;
}

struct var * addVar(struct type * t, char * name)
{
  printf("DEBUG: addVar %s\n", name);
  struct var * list, * v;
  if (findVarType(t, name) != NULL)
  {
    printf("ERROR CODE 2\n");
    exit(0);
  }
  if (t->vars != NULL)
  {
    list = t->vars;
    while (list->next != NULL)
      list = list->next;
    v = ALLOC(struct var);
    v->name = name;
    v->next = NULL;
    list->next = v;
    return v;
  }
  else
  {
    v = ALLOC(struct var);
    v->name = name;
    v->next = NULL;
    t->vars = v;
    return v;
  }
  return NULL;
}

struct var * addVarI(struct type * t, char * name)
{
  return addVar(addType(t, 0, name), name);
}

struct type * findExprType(struct type * t, struct exprNode * expr)
{
  struct type *left, *right;
  if (expr->tag == PRIMARY)
  {
    switch (expr->primary->tag)
    {
      case NUM:
        return findType(t, "INT");
      case REALNUM:
        return findType(t, "REAL");
      case ID:
        return findVarType(t, expr->primary->id);
    }
  }
  else
  {
    left = findExprType(t, expr->leftOperand);
    right = findExprType(t, expr->rightOperand);
    if (left == NULL && right == NULL)
    {
      left = addType(t, 0, expr->leftOperand->primary->id);
      addVar(left, expr->leftOperand->primary->id);
      if (strcmp(expr->leftOperand->primary->id, expr->rightOperand->primary->id))
        addVar(left, expr->rightOperand->primary->id);
      return left;
    }
    else if (left == NULL && right != NULL)
    {
      addVar(right, expr->leftOperand->primary->id);
      return right;
    }
    else if (left != NULL && right == NULL)
    {
      addVar(left, expr->rightOperand->primary->id);
      return left;
    }
    else if (left->seq == right->seq)
    {
      return left;
    }
    else
    {
      if (left->seq <= 13 && right->seq <= 13)
      {
        printf("ERROR CODE 3\n");
        exit(0);
      }
      else if (left->seq <= 13)
        setType(t, right->seq, left->seq);
      else
        setType(t, left->seq, right->seq);
      return left;
    }
  }
  return NULL;
}

void setType(struct type * t, int before, int after)
{
  while (t != NULL)
  {
    if (t->seq == before)
      t->seq = after;
    t = t->next;
  }
}

void readDecl(struct type * t, struct declNode * decl)
{
  printf("DEBUG: readDecl.\n");
  addType(t, 0, _int_);
  addType(t, 0, _real_);
  addType(t, 0, _string_);
  addType(t, 0, _boolean_);
  printf("DEBUG: Built-in types added.\n");
  if (decl->type_decl_section != NULL)
    readTypeDecl(t, decl->type_decl_section->type_decl_list);
  if (decl->var_decl_section != NULL)
    readVarDecl(t, decl->var_decl_section->var_decl_list);
  printf("DEBUG: readDecl done.\n");
}

void readVarDecl(struct type * t, struct var_decl_listNode * list)
{
  printf("DEBUG: readVarDecl.\n");
  char * name;
  struct type * thisT;
  struct id_listNode* id_list;
  switch (list->var_decl->type_name->type)
  {
    case INT:
      name = _int_;
      break;
    case REAL:
      name = _real_;
      break;
    case STRING:
      name = _string_;
      break;
    case BOOLEAN:
      name = _boolean_;
      break;
    case ID:
      name = list->var_decl->type_name->id;
      break;
  }
  thisT = findType(t, name);
  if (thisT == NULL)
    thisT = addType(t, 0, name);
  id_list = list->var_decl->id_list;
  while (id_list != NULL)
  {
    if (findType(t, id_list->id) != NULL)
    {
      printf("ERROR CODE 1\n");
      exit(0);
    }
    addVar(thisT, id_list->id);
    id_list = id_list->id_list;
  }
  if (list->var_decl_list != NULL)
    readVarDecl(t, list->var_decl_list);
  printf("DEBUG: readVarDecl done.\n");
}

void readTypeDecl(struct type * t, struct type_decl_listNode * list)
{
  printf("DEBUG: readTypeDecl.\n");
  char * name;
  struct type * thisT;
  struct id_listNode* id_list;
  switch (list->type_decl->type_name->type)
  {
    case INT:
      name = _int_;
      break;
    case REAL:
      name = _real_;
      break;
    case STRING:
      name = _string_;
      break;
    case BOOLEAN:
      name = _boolean_;
      break;
    case ID:
      name = list->type_decl->type_name->id;
      break;
  }
  thisT = findType(t, name);
  if (thisT == NULL)
    thisT = addType(t, 0, name);
  id_list = list->type_decl->id_list;
  while (id_list != NULL)
  {
    addType(t, thisT->seq, id_list->id);
    id_list = id_list->id_list;
  }
  if (list->type_decl_list != NULL)
    readTypeDecl(t, list->type_decl_list);
  printf("DEBUG: readTypeDecl done.\n");
}

void readStmt(struct type * t, struct stmt_listNode * list)
{
  struct type * left, * right;
  struct exprNode * expr;
  if (list->stmt->stmtType == ASSIGN)
  {
    printf("DEBUG: readStmt ASSIGN\n");
    expr = list->stmt->assign_stmt->expr;
    right = findExprType(t, expr);
    left = findVarType(t, list->stmt->assign_stmt->id);
    if (left == NULL && right == NULL)
    {
      left = addType(t, 0, list->stmt->assign_stmt->id);
      addVar(left, list->stmt->assign_stmt->id);
      if (strcmp(list->stmt->assign_stmt->id, expr->primary->id))
        addVar(left, expr->primary->id);
    }
    else if (left == NULL && right != NULL)
    {
      addVar(right, list->stmt->assign_stmt->id);
    }
    else if (left != NULL && right == NULL)
    {
      addVar(left, expr->primary->id);
    }
    else if (left->seq != right->seq)
    {
      if (left->seq <= 13 && right->seq <= 13)
      {
        printf("ERROR CODE 3\n");
        exit(0);
      }
      else if (left->seq <= 13)
        setType(t, right->seq, left->seq);
      else
        setType(t, left->seq, right->seq);
    }
  }
  else if (list->stmt->stmtType == WHILE)
  {
    printf("DEBUG: readStmt WHILE\n");
    if (list->stmt->while_stmt->condition->relop == 0)
    {
      left = findVarType(t, list->stmt->while_stmt->condition->left_operand->id);
      right = findType(t, "BOOLEAN");
    }
    else
    {
      switch (list->stmt->while_stmt->condition->left_operand->tag)
      {
        case NUM:
          left = findType(t, "INT");
          break;
        case REALNUM:
          left = findType(t, "REAL");
          break;
        case ID:
          left = findVarType(t, list->stmt->while_stmt->condition->left_operand->id);
          break;
      }
      switch (list->stmt->while_stmt->condition->right_operand->tag)
      {
        case NUM:
          right = findType(t, "INT");
          break;
        case REALNUM:
          right = findType(t, "REAL");
          break;
        case ID:
          right = findVarType(t, list->stmt->while_stmt->condition->right_operand->id);
          break;
      }
    }
    if (left == NULL && right == NULL)
    {
      left = addType(t, 0, list->stmt->while_stmt->condition->left_operand->id);
      addVar(left, list->stmt->while_stmt->condition->left_operand->id);
      if (strcmp(list->stmt->while_stmt->condition->left_operand->id, list->stmt->while_stmt->condition->right_operand->id))
        addVar(left, list->stmt->while_stmt->condition->right_operand->id);
    }
    else if (left == NULL && right != NULL)
    {
      addVar(right, list->stmt->while_stmt->condition->left_operand->id);
    }
    else if (left != NULL && right == NULL)
    {
      addVar(left, list->stmt->while_stmt->condition->right_operand->id);
    }
    else if (left->seq != right->seq)
    {
      if (left->seq <= 13 && right->seq <= 13)
      {
        printf("ERROR CODE 3\n");
        exit(0);
      }
      else if (left->seq <= 13)
        setType(t, right->seq, left->seq);
      else
        setType(t, left->seq, right->seq);
    }
    if (list->stmt->while_stmt->body->stmt_list != NULL)
      readStmt(t, list->stmt->while_stmt->body->stmt_list);
  }
  if (list->stmt_list != NULL)
    readStmt(t, list->stmt_list);
}

void printAll(struct type * t)
{
  int i, j;
  char res[255] = "", buf[63] = "", * tname;
  struct type * h;
  struct var * v;
  h = t;
  for (i = 0; i < _typeCount_; i++)
  {
    t = h;
    j = -1;
    // loop 1 for types
    while (t != NULL)
    {
      if (t->seq == i + 10 && j < 0)
      {
        j = 0;
        sprintf(res, "%s : ", t->name);
        tname = t->name;
      }
      if (t->seq == i + 10 && j >= 0 && t->vars != NULL)
      {
        if (strcmp(t->name, tname))
        {
          sprintf(buf, "%s ", t->name);
          strcat(res, buf);
          j++;
        }
      }
      t = t->next;
    }
    t = h;
    // loop 2 for certain implict types
    while (t != NULL)
    {
      if (t->seq == i + 10 && j >= 0 && t->vars == NULL)
      {
        if (strcmp(t->name, tname))
        {
          sprintf(buf, "%s ", t->name);
          strcat(res, buf);
          j++;
        }
      }
      t = t->next;
    }
    t = h;
    // loop 3 for variables
    while (t != NULL)
    {
      if (t->seq == i + 10 && t->vars != NULL)
      {
        v = t->vars;
        while (v != NULL)
        {
          if (strcmp(t->name, v->name))
          {
            sprintf(buf, "%s ", v->name);
            strcat(res, buf);
            j++;
          }
          v = v->next;
        }
      }
      t = t->next;
    }
    if (j > 0)
      printf("%s#\n", res);
  }
  t = h;
  while (t != NULL)
  {
    printf("DEBUG: %s, %d @%p\n", t->name, t->seq, t->vars);
    t = t->next;
  }
}

void printType(struct type * t)
{
  // int seq;
  struct var * v;
  v = t->vars;
  while (v != NULL)
  {
    if (strcmp(t->name, v->name))
      printf("%s ", v->name);
    v = v->next;
  }
}
