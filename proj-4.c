#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int switchFlag;

struct global_var
{
  struct varNode * node;
  struct global_var * next;
};

struct global_var * globals;

struct varNode * var_find(char *);
struct varNode * var_create(char *);

struct varNode * var_find(char * id)
{
  if (globals == NULL)
  {
    return var_create(id);
  }
  else
  {
    struct global_var * var;
    var = globals;
    while (var != NULL)
    {
      if (strcmp(id, var->node->name) == 0)
        return var->node;
      var = var->next;
    }
  }
  return var_create(id);
}

struct varNode * var_create(char * id)
{
  struct varNode * node;
  struct global_var * new_var;
  node = (struct varNode *)malloc(sizeof(struct varNode));
  node->name = (char *)malloc(sizeof(char) * (strlen(id) + 1));
  strcpy(node->name, token);
  node->value = 0;
  new_var = (struct global_var *)malloc(sizeof(struct global_var));
  new_var->node = node;
  new_var->next = NULL;
  if (globals == NULL)
  {
    globals = new_var;
  }
  else
  {
    new_var->next = globals;
    globals = new_var;
  }
  return node;
}

struct statementNode * parse_var_section();
  struct statementNode * parse_id_list();
struct statementNode * parse_body();
  struct statementNode * parse_stmt_list(struct statementNode *);
    struct statementNode * parse_stmt(struct statementNode *);
      struct statementNode * parse_assign_stmt();
      struct statementNode * parse_if_stmt();
      struct statementNode * parse_print_stmt();
      struct statementNode * parse_while_stmt();
      struct statementNode * parse_switch_stmt();
        struct statementNode * parse_case_list(struct varNode *);

void fix_true_branch(struct statementNode *, struct statementNode *);

struct statementNode * parse_program_and_generate_intermediate_representation()
{
  struct statementNode * program;
  // program = (struct statementNode *)malloc(sizeof(struct statementNode *));
  parse_var_section();
  program = parse_body();
  return program;
}

struct statementNode * parse_var_section()
{
  struct statementNode * var_section;
  var_section = parse_id_list();
  ttype = getToken();
  if (ttype != SEMICOLON)
  {
    printf("Error parsing var_section: SEMICOLON expected.\n");
    exit(1);
  }
  return var_section;
}

struct statementNode * parse_id_list()
{
  struct statementNode * id;
  id = (struct statementNode *)malloc(sizeof(struct statementNode));
  ttype = getToken();
  if (ttype != ID)
  {
    printf("Error parsing id_list: ID expected.\n");
    exit(1);
  }
  var_create(token);
  ttype = getToken();
  if (ttype == COMMA)
  {
    id->next = parse_id_list();
  }
  else ungetToken();
  return id;
}

struct statementNode * parse_body()
{
  struct statementNode * body;
  ttype = getToken();
  if (ttype != LBRACE)
  {
    printf("Error parsing body: LBRACE expected.\n");
    exit(1);
  }
  body = parse_stmt_list(NULL);
  ttype = getToken();
  if (ttype != RBRACE)
  {
    printf("Error parsing body: RBRACE expected.\n");
    exit(1);
  }
  return body;
}

struct statementNode * parse_stmt_list(struct statementNode * prev)
{
  struct statementNode * stmt_list;
  stmt_list = parse_stmt(prev);
  ttype = getToken();
  ungetToken();
  if (ttype != RBRACE)
  {
    stmt_list->next = parse_stmt_list(stmt_list);
  }
  else if (stmt_list->stmt_type == IFSTMT)
  {
    if (stmt_list->if_stmt->false_branch == NULL)
    {
      stmt_list->if_stmt->false_branch = (struct statementNode *)malloc(sizeof(struct statementNode));
      stmt_list->if_stmt->false_branch->stmt_type = NOOPSTMT;
    }
  }
  return stmt_list;
}

struct statementNode * parse_stmt(struct statementNode * prev)
{
  struct statementNode * stmt;
  // stmt = (struct statementNode *)malloc(sizeof(struct statementNode *));
  ttype = getToken();
  ungetToken();
  switch (ttype)
  {
    case ID:
      stmt = parse_assign_stmt();
      break;
    case IF:
      stmt = parse_if_stmt();
      break;
    case PRINT:
      stmt = parse_print_stmt();
      break;
    case WHILE:
      stmt = parse_while_stmt();
      break;
    case SWITCH:
      stmt = parse_switch_stmt();
      switchFlag = 1;
      break;
    default:
      printf("Error parsing statement: ID | IF | PRINT | WHILE | SWITCH expected.\n");
      exit(1);
  }
  if (prev != NULL)
  {
    if (switchFlag == 2)
    {
      int hasDefault = 1;
      struct statementNode * caseNode, * caseBody;
      switchFlag = 0;
      caseNode = prev;
      // Add goto to every case body
      while (caseNode != NULL && caseNode->stmt_type == IFSTMT)
      {
        caseBody = caseNode->if_stmt->false_branch;
        fix_true_branch(caseBody, stmt);
        if (caseNode->if_stmt->true_branch == NULL)
        {
          caseNode->if_stmt->true_branch = stmt;
          hasDefault = 0;
          break;
        }
        caseNode = caseNode->if_stmt->true_branch;
      }
      // Add goto to default body
      if (hasDefault)
      {
        caseBody = caseNode;
        while (caseBody->next != NULL) caseBody = caseBody->next;
        caseBody->next = stmt;
      }
    }
    else if (prev->stmt_type == IFSTMT)
    {
      prev->if_stmt->false_branch = stmt;
      fix_true_branch(prev->if_stmt->true_branch, stmt);
    }
  }
  if (switchFlag) switchFlag = 2;
  stmt->next = NULL;
  return stmt;
}

struct statementNode * parse_assign_stmt()
{
  struct statementNode * stmt;
  stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  ttype = getToken();
  if (ttype != ID)
  {
    printf("Error parsing assign_stmt: ID expected.\n");
    exit(1);
  }
  stmt->stmt_type = ASSIGNSTMT;
  stmt->assign_stmt = (struct assignmentStatement *)malloc(sizeof(struct assignmentStatement));
  stmt->assign_stmt->lhs = var_find(token);
  ttype = getToken();
  if (ttype != EQUAL)
  {
    printf("Error parsing assign_stmt: EQUAL expected.\n");
    exit(1);
  }
  ttype = getToken();
  if (ttype == NUM)
  {
    stmt->assign_stmt->op1 = (struct varNode *)malloc(sizeof(struct varNode));
    stmt->assign_stmt->op1->value = atoi(token);
  }
  else if (ttype == ID)
  {
    stmt->assign_stmt->op1 = var_find(token);
  }
  else
  {
    printf("Error parsing assign_stmt: ID | NUM expected.\n");
    exit(1);
  }
  ttype = getToken();
  if (ttype >= PLUS && ttype <= MULT)
  {
    stmt->assign_stmt->op = ttype;
    ttype = getToken();
    if (ttype == NUM)
    {
      stmt->assign_stmt->op2 = (struct varNode *)malloc(sizeof(struct varNode));
      stmt->assign_stmt->op2->value = atoi(token);
    }
    else if (ttype == ID)
    {
      stmt->assign_stmt->op2 = var_find(token);
    }
    else
    {
      printf("Error parsing assign_stmt: ID | NUM expected.\n");
      exit(1);
    }
  }
  else
  {
    ungetToken();
    stmt->assign_stmt->op = 0;
  }
  ttype = getToken();
  if (ttype != SEMICOLON)
  {
    printf("Error parsing assign_stmt: SEMICOLON expected.\n");
    exit(1);
  }
  return stmt;
}

struct statementNode * parse_if_stmt()
{
  struct statementNode * stmt;
  stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  // OP1
  ttype = getToken();
  if (ttype != IF)
  {
    printf("Error parsing if_stmt: IF expected.\n");
    exit(1);
  }
  stmt->stmt_type = IFSTMT;
  stmt->if_stmt = (struct ifStatement *)malloc(sizeof(struct ifStatement));
  ttype = getToken();
  if (ttype == NUM)
  {
    stmt->if_stmt->op1 = (struct varNode *)malloc(sizeof(struct varNode));
    stmt->if_stmt->op1->value = atoi(token);
  }
  else if (ttype == ID)
  {
    stmt->if_stmt->op1 = var_find(token);
  }
  else
  {
    printf("Error parsing if_stmt: ID | NUM expected.\n");
    exit(1);
  }
  // RELOP
  ttype = getToken();
  if (ttype == GREATER || ttype == LESS || ttype == NOTEQUAL)
  {
    stmt->if_stmt->relop = ttype;
  }
  else
  {
    printf("Error parsing if_stmt: GREATER | LESS | NOTEQUAL expected.\n");
    exit(1);
  }
  // OP2
  ttype = getToken();
  if (ttype == NUM)
  {
    stmt->if_stmt->op2 = (struct varNode *)malloc(sizeof(struct varNode));
    stmt->if_stmt->op2->value = atoi(token);
  }
  else if (ttype == ID)
  {
    stmt->if_stmt->op2 = var_find(token);
  }
  else
  {
    printf("Error parsing if_stmt: ID | NUM expected.\n");
    exit(1);
  }
  stmt->if_stmt->true_branch = parse_body();
  return stmt;
}

struct statementNode * parse_print_stmt()
{
  struct statementNode * stmt;
  stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  stmt->stmt_type = PRINTSTMT;
  stmt->print_stmt = (struct printStatement *)malloc(sizeof(struct printStatement));
  ttype = getToken();
  if (ttype != PRINT)
  {
    printf("Error parsing print_stmt: PRINT expected.\n");
    exit(1);
  }
  ttype = getToken();
  if (ttype != ID)
  {
    printf("Error parsing print_stmt: ID expected.\n");
    exit(1);
  }
  stmt->print_stmt->id = var_find(token);
  ttype = getToken();
  if (ttype != SEMICOLON)
  {
    printf("Error parsing print_stmt: SEMICOLON expected.\n");
    exit(1);
  }
  return stmt;
}

struct statementNode * parse_while_stmt()
{
  struct statementNode * stmt;
  stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  // OP1
  ttype = getToken();
  if (ttype != WHILE)
  {
    printf("Error parsing while_stmt: WHILE expected.\n");
    exit(1);
  }
  stmt->stmt_type = IFSTMT;
  stmt->if_stmt = (struct ifStatement *)malloc(sizeof(struct ifStatement));
  ttype = getToken();
  if (ttype == NUM)
  {
    stmt->if_stmt->op1 = (struct varNode *)malloc(sizeof(struct varNode));
    stmt->if_stmt->op1->value = atoi(token);
  }
  else if (ttype == ID)
  {
    stmt->if_stmt->op1 = var_find(token);
  }
  else
  {
    printf("Error parsing while_stmt: ID | NUM expected.\n");
    exit(1);
  }
  // RELOP
  ttype = getToken();
  if (ttype == GREATER || ttype == LESS || ttype == NOTEQUAL)
  {
    stmt->if_stmt->relop = ttype;
  }
  else
  {
    printf("Error parsing while_stmt: GREATER | LESS | NOTEQUAL expected.\n");
    exit(1);
  }
  // OP2
  ttype = getToken();
  if (ttype == NUM)
  {
    stmt->if_stmt->op2 = (struct varNode *)malloc(sizeof(struct varNode));
    stmt->if_stmt->op2->value = atoi(token);
  }
  else if (ttype == ID)
  {
    stmt->if_stmt->op2 = var_find(token);
  }
  else
  {
    printf("Error parsing while_stmt: ID | NUM expected.\n");
    exit(1);
  }
  stmt->if_stmt->true_branch = parse_body();
  struct statementNode * gotoNode;
  gotoNode = (struct statementNode *)malloc(sizeof(struct statementNode));
  gotoNode->stmt_type = GOTOSTMT;
  gotoNode->goto_stmt = (struct gotoStatement *)malloc(sizeof(struct gotoStatement));
  gotoNode->goto_stmt->target = stmt;
  fix_true_branch(stmt->if_stmt->true_branch, gotoNode);
  return stmt;
}

struct statementNode * parse_switch_stmt()
{
  struct statementNode * stmt;
  // stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  // SWITCH
  ttype = getToken();
  if (ttype != SWITCH)
  {
    printf("Error parsing switch_stmt: SWITCH expected.\n");
    exit(1);
  }
  // ID
  ttype = getToken();
  if (ttype != ID)
  {
    printf("Error parsing switch_stmt: ID expected.\n");
    exit(1);
  }
  struct varNode * var;
  var = var_find(token);
  // LBRACE
  ttype = getToken();
  if (ttype != LBRACE)
  {
    printf("Error parsing switch_stmt: LBRACE expected.\n");
    exit(1);
  }
  // CASES
  stmt = parse_case_list(var);
  // RBRACE
  ttype = getToken();
  if (ttype != RBRACE)
  {
    printf("Error parsing switch_stmt: RBRACE expected.\n");
    exit(1);
  }
  return stmt;
}

struct statementNode * parse_case_list(struct varNode * var)
{
  struct statementNode * stmt;
  stmt = (struct statementNode *)malloc(sizeof(struct statementNode));
  stmt->stmt_type = IFSTMT;
  stmt->if_stmt = (struct ifStatement *)malloc(sizeof(struct ifStatement));
  // CASE
  ttype = getToken();
  if (ttype != CASE)
  {
    printf("Error parsing case_list: CASE expected.\n");
    exit(1);
  }
  // NUM
  ttype = getToken();
  if (ttype != NUM)
  {
    printf("Error parsing case_list: NUM expected.\n");
    exit(1);
  }
  stmt->if_stmt->op1 = var;
  stmt->if_stmt->op2 = (struct varNode *)malloc(sizeof(struct varNode));
  stmt->if_stmt->op2->value = atoi(token);
  stmt->if_stmt->relop = NOTEQUAL;
  // COLON
  ttype = getToken();
  if (ttype != COLON)
  {
    printf("Error parsing case_list: COLON expected.\n");
    exit(1);
  }
  // NOTEQUAL + FALSE = TRUE
  stmt->if_stmt->false_branch = parse_body();
  stmt->if_stmt->true_branch = NULL;
  ttype = getToken();
  ungetToken();
  if (ttype == CASE)
  {
    stmt->if_stmt->true_branch = parse_case_list(var);
  }
  else if (ttype == DEFAULT)
  {
    getToken();
    ttype = getToken();
    if (ttype != COLON)
    {
      printf("Error parsing case_list: COLON expected.\n");
      exit(1);
    }
    stmt->if_stmt->true_branch = (struct statementNode *)malloc(sizeof(struct statementNode));
    stmt->if_stmt->true_branch->stmt_type = NOOPSTMT;
    stmt->if_stmt->true_branch->next = parse_body();
  }
  return stmt;
}

void fix_true_branch(struct statementNode * trueNode, struct statementNode * next)
{
  while (trueNode->next != NULL) trueNode = trueNode->next;
  trueNode->next = next;
  if (trueNode->stmt_type == IFSTMT)
  {
    trueNode->if_stmt->false_branch = next;
    fix_true_branch(trueNode->if_stmt->true_branch, next);
  }
}
