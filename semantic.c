/*----------------------------------------------------------------------------
Note: the code in this file is not to be shared with anyone or posted online.
(c) Rida Bazzi, 2014
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj-3.h"

#define TRUE 1
#define FALSE 0

/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */

#define KEYWORDS  7

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

char *reserved[] = {"",
	"VAR", "WHILE", "INT", "REAL", "STRING", "BOOLEAN", "TYPE",
	"+", "-", "/", "*", "=",
	":", ",", ";",
	"[", "]", "(", ")", "{", "}",
	"<>", ">", "<", "<=", ">=", ".",
	"ID", "NUM", "REALNUM",
	"ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
int ttype; // token type
int activeToken = FALSE;
int tokenLength;
int line_no = 1;

void skipSpace()
{
	char c;

	c = getchar();
	line_no += (c == '\n');
	while (!feof(stdin) && isspace(c)) {
		c = getchar();
		line_no += (c == '\n');
	}
	ungetc(c, stdin);
}

int isKeyword(char *s)
{
	int i;

	for (i = 1; i <= KEYWORDS; i++)
		if (strcmp(reserved[i], s) == 0)
			return i;
	return FALSE;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old ttype is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
	activeToken = TRUE;
}

int scan_number()
{
	char c;

	c = getchar();
	if (isdigit(c))
	{
		// First collect leading digits before dot
		// 0 is a nNUM by itself
		if (c == '0')
		{
			token[tokenLength] = c;
			tokenLength++;
			token[tokenLength] = '\0';
		}
		else
		{
			while (isdigit(c))
			{
				token[tokenLength] = c;
				tokenLength++;
				c = getchar();
			}
			ungetc(c, stdin);
			token[tokenLength] = '\0';
		}
		// Check if leading digits are integer part of a REALNUM
		c = getchar();
		if (c == '.')
		{
			c = getchar();
			if (isdigit(c))
			{
				token[tokenLength] = '.';
				tokenLength++;
				while (isdigit(c))
				{
					token[tokenLength] = c;
					tokenLength++;
					c = getchar();
				}
				token[tokenLength] = '\0';
				if (!feof(stdin))
					ungetc(c, stdin);
				return REALNUM;
			}
			else
			{
				ungetc(c, stdin);
				c = '.';
				ungetc(c, stdin);
				return NUM;
			}
		}
		else
		{
			ungetc(c, stdin);
			return NUM;
		}
	}
	else
		return ERROR;
}

int scan_id_or_keyword()
{
	int ttype;
	char c;

	c = getchar();
	if (isalpha(c))
	{
		while (isalnum(c))
		{
			token[tokenLength] = c;
			tokenLength++;
			c = getchar();
		}
		if (!feof(stdin))
			ungetc(c, stdin);
		token[tokenLength] = '\0';
		ttype = isKeyword(token);
		if (ttype == 0)
			ttype = ID;
		return ttype;
	}
	else
		return ERROR;
}

int getToken()
{
	char c;

	if (activeToken)
	{
		activeToken = FALSE;
		return ttype;
	}
	skipSpace();
	tokenLength = 0;
	c = getchar();
	switch (c)
	{
		case '.': return DOT;
		case '+': return PLUS;
		case '-': return MINUS;
		case '/': return DIV;
		case '*': return MULT;
		case '=': return EQUAL;
		case ':': return COLON;
		case ',': return COMMA;
		case ';': return SEMICOLON;
		case '[': return LBRAC;
		case ']': return RBRAC;
		case '(': return LPAREN;
		case ')': return RPAREN;
		case '{': return LBRACE;
		case '}': return RBRACE;
		case '<':
			c = getchar();
			if (c == '=')
				return LTEQ;
			else if (c == '>')
				return NOTEQUAL;
			else
			{
				ungetc(c, stdin);
				return LESS;
			}
		case '>':
			c = getchar();
			if (c == '=')
				return GTEQ;
			else
			{
				ungetc(c, stdin);
				return GREATER;
			}
		default:
			if (isdigit(c))
			{
				ungetc(c, stdin);
				return scan_number();
			}
			else if (isalpha(c))
			{
				ungetc(c, stdin);
				return scan_id_or_keyword();
			}
			else if (c == EOF)
				return EOF;
			else
				return ERROR;
	}
}

/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
	printf("Syntax error while parsing %s line %d\n", msg, line_no);
	exit(1);
}

/* -------------------- PRINTING PARSE TREE -------------------- */
void print_parse_tree(struct programNode* program)
{
	print_decl(program->decl);
	print_body(program->body);
}

void print_decl(struct declNode* dec)
{
	if (dec->type_decl_section != NULL)
	{
		print_type_decl_section(dec->type_decl_section);
	}
	if (dec->var_decl_section != NULL)
	{
		print_var_decl_section(dec->var_decl_section);
	}
}

void print_body(struct bodyNode* body)
{
	printf("{\n");
	print_stmt_list(body->stmt_list);
	printf("}\n");
}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
	printf("VAR\n");
	if (varDeclSection->var_decl_list != NULL)
		print_var_decl_list(varDeclSection->var_decl_list);
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
	print_var_decl(varDeclList->var_decl);
	if (varDeclList->var_decl_list != NULL)
		print_var_decl_list(varDeclList->var_decl_list);
}

void print_var_decl(struct var_declNode* varDecl)
{
	print_id_list(varDecl->id_list);
	printf(": ");
	print_type_name(varDecl->type_name);
	printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
	printf("TYPE\n");
	if (typeDeclSection->type_decl_list != NULL)
		print_type_decl_list(typeDeclSection->type_decl_list);
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
	print_type_decl(typeDeclList->type_decl);
	if (typeDeclList->type_decl_list != NULL)
		print_type_decl_list(typeDeclList->type_decl_list);
}

void print_type_decl(struct type_declNode* typeDecl)
{
	print_id_list(typeDecl->id_list);
	printf(": ");
	print_type_name(typeDecl->type_name);
	printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
	if (typeName->type != ID)
		printf("%s ", reserved[typeName->type]);
	else
		printf("%s ", typeName->id);
}

void print_id_list(struct id_listNode* idList)
{
	printf("%s ", idList->id);
	if (idList->id_list != NULL)
	{
		printf(", ");
		print_id_list(idList->id_list);
	}
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
	print_stmt(stmt_list->stmt);
	if (stmt_list->stmt_list != NULL)
		print_stmt_list(stmt_list->stmt_list);

}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
	printf("%s ", assign_stmt->id);
	printf("= ");
	print_expression_prefix(assign_stmt->expr);
	printf("; \n");
}

void print_stmt(struct stmtNode* stmt)
{
	if (stmt->stmtType == ASSIGN)
		print_assign_stmt(stmt->assign_stmt);
	if (stmt->stmtType == WHILE)
		print_while_stmt(stmt->while_stmt);
}

void print_expression_prefix(struct exprNode* expr)
{
	if (expr->tag == EXPR)
	{
		printf("%s ", reserved[expr->operator]);
		print_expression_prefix(expr->leftOperand);
		print_expression_prefix(expr->rightOperand);
	}
	else if (expr->tag == PRIMARY)
	{
		if (expr->primary->tag == ID)
			printf("%s ", expr->primary->id);
		else if (expr->primary->tag == NUM)
			printf("%d ", expr->primary->ival);
		else if (expr->primary->tag == REALNUM)
			printf("%.4f ", expr->primary->fval);
	}
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
	printf("WHILE ");
	print_condition(while_stmt->condition);
	printf("\n");
	print_body(while_stmt->body);
	printf("\n");
}

void print_condition(struct conditionNode* condition)
{
	if (condition->left_operand->tag == ID)
		printf("%s ", condition->left_operand->id);
	else if (condition->left_operand->tag == NUM)
		printf("%d ", condition->left_operand->ival);
	else if (condition->left_operand->tag == REALNUM)
		printf("%.4f ", condition->left_operand->fval);
	switch (condition->relop)
	{
		case GREATER:
		  printf("> ");
			break;
		case GTEQ:
		  printf(">= ");
			break;
		case EQUAL:
		  printf("== ");
			break;
		case LESS:
		  printf("< ");
			break;
		case LTEQ:
		  printf("<= ");
			break;
		case NOTEQUAL:
		  printf("!= ");
			break;
		case 0:
		  return;
	}
	if (condition->right_operand->tag == ID)
		printf("%s ", condition->right_operand->id);
	else if (condition->right_operand->tag == NUM)
		printf("%d ", condition->right_operand->ival);
	else if (condition->right_operand->tag == REALNUM)
		printf("%.4f ", condition->right_operand->fval);
}

/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */
struct primaryNode* primary()
{
	struct primaryNode* prim;
	prim = ALLOC(struct primaryNode);

	ttype = getToken();
	switch (ttype)
	{
		case ID:
		  prim->tag = ID;
			prim->id = (char *) malloc((strlen(token) + 1) * sizeof(char));
			strcpy(prim->id, token);
			return prim;
		case NUM:
		  prim->tag = NUM;
			prim->ival = atoi(token);
			return prim;
		case REALNUM:
		  prim->tag = REALNUM;
			prim->fval = atof(token);
			return prim;
		default:
		  free(prim);
			syntax_error("primary. ID or NUM or REALNUM expected");
			break;
	}
	return NULL;
}

struct conditionNode* condition()
{
	struct conditionNode* cond;

	ttype = getToken();
	if (ttype == ID || ttype == NUM || ttype == REALNUM)
	{
		ungetToken();
		cond = ALLOC(struct conditionNode);
		cond->left_operand = primary();
		ttype = getToken();
		if (ttype != GREATER && ttype != GTEQ && ttype != LESS && ttype != NOTEQUAL && ttype != LTEQ && ttype != EQUAL) {
			if (cond->left_operand->tag != ID) {
				syntax_error("condition. RELOP expected");
			}
			else
			{
				ungetToken();
				cond->relop = 0;
				return cond;
			}
		}
		else
		{
			cond->relop = ttype;
			cond->right_operand = primary();
			return cond;
		}
	}
	else
		syntax_error("condition. ID or PRIMARY expected");
	return NULL;
}

struct while_stmtNode* while_stmt()
{
	struct while_stmtNode* wstmt;

	ttype = getToken();
	if (ttype == WHILE)
	{
		wstmt = ALLOC(struct while_stmtNode);
		wstmt->condition = condition();
		wstmt->body = body();
		return wstmt;
	}
	else
		syntax_error("while_stmt. WHILE expected");
	return NULL;
}

struct exprNode* factor()
{
	struct exprNode* facto;

	ttype = getToken();
	if (ttype == LPAREN)
	{
		facto = expr();
		ttype = getToken();
		if (ttype == RPAREN)
			return facto;
		else
			syntax_error("factor. RPAREN expected");
	}
	else if (ttype == NUM)
	{
		facto = ALLOC(struct exprNode);
		facto->primary = ALLOC(struct primaryNode);
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = NUM;
		facto->primary->ival = atoi(token);
		return facto;
	}
	else if (ttype == REALNUM)
	{
		facto = ALLOC(struct exprNode);
		facto->primary = ALLOC(struct primaryNode);
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = REALNUM;
		facto->primary->fval = atof(token);
		return facto;
	}
	else if (ttype == ID)
	{
		facto = ALLOC(struct exprNode);
		facto->primary = ALLOC(struct primaryNode);
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = ID;
		facto->primary->id = strdup(token);
		return facto;
	}
	else
		syntax_error("factor. NUM, REALNUM, or ID, expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
	struct exprNode* ter;
	struct exprNode* f;

	ttype = getToken();
	if (ttype == ID || ttype == LPAREN || ttype == NUM || ttype == REALNUM)
	{
		ungetToken();
		f = factor();
		ttype = getToken();
		if (ttype == MULT || ttype == DIV)
		{
			ter = ALLOC(struct exprNode);
			ter->operator = ttype;
			ter->leftOperand = f;
			ter->rightOperand = term();
			ter->tag = EXPR;
			return ter;
		}
		else if (ttype == SEMICOLON || ttype == PLUS ||
				 ttype == MINUS || ttype == RPAREN)
		{
			ungetToken();
			return f;
		}
		else
			syntax_error("term. MULT or DIV expected");
	}
	else
		syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
	struct exprNode* exp;
	struct exprNode* t;

	ttype = getToken();
	if (ttype == ID || ttype == LPAREN || ttype == NUM || ttype == REALNUM)
	{
		ungetToken();
		t = term();
		ttype = getToken();
		if (ttype == PLUS || ttype == MINUS)
		{
			exp = ALLOC(struct exprNode);
			exp->operator = ttype;
			exp->leftOperand = t;
			exp->rightOperand = expr();
			exp->tag = EXPR;
			return exp;
		}
		else if (ttype == SEMICOLON || ttype == MULT ||
				 ttype == DIV || ttype == RPAREN)
		{
			ungetToken();
			return t;
		}
		else
			syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
	}
	else
		syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct assign_stmtNode* assign_stmt()
{
	struct assign_stmtNode* assignStmt;

	ttype = getToken();
	if (ttype == ID)
	{
		assignStmt = ALLOC(struct assign_stmtNode);
		assignStmt->id = strdup(token);
		ttype = getToken();
		if (ttype == EQUAL)
		{
			assignStmt->expr = expr();
			return assignStmt;
		}
		else
			syntax_error("assign_stmt. EQUAL expected");
	}
	else
		syntax_error("assign_stmt. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
	struct stmtNode* stm;

	stm = ALLOC(struct stmtNode);
	ttype = getToken();
	if (ttype == ID) // assign_stmt
	{
		ungetToken();
		stm->assign_stmt = assign_stmt();
		stm->stmtType = ASSIGN;
		ttype = getToken();
		if (ttype == SEMICOLON)
			return stm;
		else
			syntax_error("stmt. SEMICOLON expected");
	}
	else if (ttype == WHILE) // while_stmt
	{
		ungetToken();
		stm->while_stmt = while_stmt();
		stm->stmtType = WHILE;
		return stm;
	}
	else
		syntax_error("stmt. ID or WHILE expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmt_listNode* stmt_list()
{
	struct stmt_listNode* stmtList;

	ttype = getToken();
	if (ttype == ID || ttype == WHILE)
	{
		ungetToken();
		stmtList = ALLOC(struct stmt_listNode);
		stmtList->stmt = stmt();
		ttype = getToken();
		if (ttype == ID || ttype == WHILE)
		{
			ungetToken();
			stmtList->stmt_list = stmt_list();
			return stmtList;
		}
		else // If the next token is not in FOLLOW(stmt_list),
			 // let the caller handle it.
		{
			ungetToken();
			stmtList->stmt_list = NULL;
			return stmtList;
		}
	}
	else
		syntax_error("stmt_list. ID or WHILE expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
	struct bodyNode* bod;

	ttype = getToken();
	if (ttype == LBRACE)
	{
		bod = ALLOC(struct bodyNode);
		bod->stmt_list = stmt_list();
		ttype = getToken();
		if (ttype == RBRACE)
			return bod;
		else
			syntax_error("body. RBRACE expected");
	}
	else
		syntax_error("body. LBRACE expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_nameNode* type_name()
{
	struct type_nameNode* tName;

	tName = ALLOC(struct type_nameNode);
	ttype = getToken();
	if (ttype == ID || ttype == INT || ttype == REAL ||
		ttype == STRING || ttype == BOOLEAN)
	{
		tName->type = ttype;
		if (ttype == ID)
			tName->id = strdup(token);
		return tName;
	}
	else
		syntax_error("type_name. type name expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct id_listNode* id_list()
{
	struct id_listNode* idList;

	idList = ALLOC(struct id_listNode);
	ttype = getToken();
	if (ttype == ID)
	{
		idList->id = strdup(token);
		ttype = getToken();
		if (ttype == COMMA)
		{
			idList->id_list = id_list();
			return idList;
		}
		else if (ttype == COLON)
		{
			ungetToken();
			idList->id_list = NULL;
			return idList;
		}
		else
			syntax_error("id_list. COMMA or COLON expected");
	}
	else
		syntax_error("id_list. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_declNode* type_decl()
{
	struct type_declNode* typeDecl;

	typeDecl = ALLOC(struct type_declNode);
	ttype = getToken();
	if (ttype == ID)
	{
		ungetToken();
		typeDecl->id_list = id_list();
		ttype = getToken();
		if (ttype == COLON)
		{
			typeDecl->type_name = type_name();
			ttype = getToken();
			if (ttype == SEMICOLON)
				return typeDecl;
			else
				syntax_error("type_decl. SEMICOLON expected");
		}
		else
			syntax_error("type_decl. COLON expected");
	}
	else
		syntax_error("type_decl. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_declNode* var_decl()
{
	struct var_declNode* varDecl;

	varDecl = ALLOC(struct var_declNode);
	ttype = getToken();
	if (ttype == ID)
	{
		ungetToken();
		varDecl->id_list = id_list();
		ttype = getToken();
		if (ttype == COLON)
		{
			varDecl->type_name = type_name();
			ttype = getToken();
			if (ttype == SEMICOLON)
				return varDecl;
			else
				syntax_error("var_decl. SEMICOLON expected");
		}
		else
			syntax_error("var_decl. COLON expected");
	}
	else
		syntax_error("var_decl. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_listNode* var_decl_list()
{
	struct var_decl_listNode* varDeclList;

	varDeclList = ALLOC(struct var_decl_listNode);
	ttype = getToken();
	if (ttype == ID)
	{
		ungetToken();
		varDeclList->var_decl = var_decl();
		ttype = getToken();
		if (ttype == ID)
		{
			ungetToken();
			varDeclList->var_decl_list = var_decl_list();
			return varDeclList;
		}
		else
		{
			ungetToken();
			return varDeclList;
		}
	}
	else
		syntax_error("var_decl_list. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_listNode* type_decl_list()
{
	struct type_decl_listNode* typeDeclList;

	typeDeclList = ALLOC(struct type_decl_listNode);
	ttype = getToken();
	if (ttype == ID)
	{
		ungetToken();
		typeDeclList->type_decl = type_decl();
		ttype = getToken();
		if (ttype == ID)
		{
			ungetToken();
			typeDeclList->type_decl_list = type_decl_list();
			return typeDeclList;
		}
		else
		{
			ungetToken();
			return typeDeclList;
		}
	}
	else
		syntax_error("type_decl_list. ID expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_sectionNode* var_decl_section()
{
	struct var_decl_sectionNode *varDeclSection;

	varDeclSection = ALLOC(struct var_decl_sectionNode);
	ttype = getToken();
	if (ttype == VAR)
	{
		// no need to ungetToken()
		varDeclSection->var_decl_list = var_decl_list();
		return varDeclSection;
	}
	else
		syntax_error("var_decl_section. VAR expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_sectionNode* type_decl_section()
{
	struct type_decl_sectionNode *typeDeclSection;

	typeDeclSection = ALLOC(struct type_decl_sectionNode);
	ttype = getToken();
	if (ttype == TYPE)
	{
		typeDeclSection->type_decl_list = type_decl_list();
		return typeDeclSection;
	}
	else
		syntax_error("type_decl_section. TYPE expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct declNode* decl()
{
	struct declNode* dec;

	dec = ALLOC(struct declNode);
	ttype = getToken();
	if (ttype == TYPE)
	{
		ungetToken();
		dec->type_decl_section = type_decl_section();
		ttype = getToken();
		if (ttype == VAR)
		{
			// type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
		}
		else
		{
			ungetToken();
			dec->var_decl_section = NULL;
		}
		return dec;
	}
	else
	{
		dec->type_decl_section = NULL;
		if (ttype == VAR)
		{
			// type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
			return dec;
		}
		else
		{
			if (ttype == LBRACE)
			{
				ungetToken();
				dec->var_decl_section = NULL;
				return dec;
			}
			else
				syntax_error("decl. LBRACE expected");
		}
	}
	return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
	struct programNode* prog;

	prog = ALLOC(struct programNode);
	ttype = getToken();
	if (ttype == TYPE || ttype == VAR || ttype == LBRACE)
	{
		ungetToken();
		prog->decl = decl();
		prog->body = body();
		return prog;
	}
	else
		syntax_error("program. TYPE or VAR or LBRACE expected");
	return NULL; // control never reaches here, this is just for the sake of GCC
}

int main()
{
	struct type * t;
	struct programNode* parseTree;
	parseTree = program();
	// TODO: remove the next line after you complete the parser
	// This is just for debugging purposes
	// print_parse_tree(parseTree);
	// TODO: do type checking & print output according to project specification
	t = ALLOC(struct type);
	t->seq = 0;
	t->name = NULL;
	t->vars = NULL;
	t->next = NULL;
	readDecl(t, parseTree->decl);
	readStmt(t, parseTree->body->stmt_list);
	printAll(t);
	return 0;
}
