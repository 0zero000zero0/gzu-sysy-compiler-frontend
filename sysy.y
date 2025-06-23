%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ir.h"
#include "node.h"

// 函数声明
void write_AST(Node *node, FILE* file, int level);
extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char *yytext;
void yyerror(const char *s);
Node* ast_root = NULL;
%}

%union {
    char* str_val;
    struct Node* node_ptr;
}

%token <node_ptr> IDENT INCONST FLOACONST
%token <node_ptr> CONST INT FLOAT VOID
%token <node_ptr> IF ELSE WHILE BREAK CONTINUE RETURN
%token <node_ptr> PLUS MINUS MUL DIV MOD ASSIGN
%token <node_ptr> EQ NEQ LT GT LTE GTE
%token <node_ptr> AND OR NOT
%token <node_ptr> LPAREN RPAREN LBRACE RBRACE LBRACK RBRACK
%token <node_ptr> COMMA SEMICOLON

%type <node_ptr> CompUnit Decl ConstDecl VarDecl ConstDef_List ConstDef VarDef_List VarDef
%type <node_ptr> ConstSubscript_List ConstInitVal InitVal FuncDef FuncFParamsOpt FuncFParams OptConstInitVal_List ConstInitVal_List InitVal_List OptInitVal_List
%type <node_ptr> FuncFParam Block BlockItem_List BlockItem Stmt ExpOpt Exp Cond LVal
%type <node_ptr> Number UnaryOp FuncRParamsOpt FuncRParams ConstExp
%type <node_ptr> LOrExp LAndExp EqExp RelExp AddExp MulExp UnaryExp PrimaryExp BType

/* --- Operator Precedence and Associativity --- */
%nonassoc IFX
%nonassoc ELSE
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left LT GT LTE GTE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT

%start CompUnit

%%
/* ================================================================== */
/* 最终版 SysY 2022 文法                           */
/* ================================================================== */

CompUnit:
    /* empty */      { $$ = new_node("CompUnit", 0); ast_root = $$; }
    | Decl CompUnit      { $$ = new_node("CompUnit", 2, $1, $2); ast_root = $$; }
    | FuncDef CompUnit { $$ = new_node("CompUnit", 2, $1, $2); ast_root = $$; }
    ;

Decl: ConstDecl { $$ = $1; } | VarDecl { $$ = $1; };
BType:
    INT   { $$ = $1; }
    | FLOAT { $$ = $1; }
    ;

ConstDecl: CONST BType ConstDef_List SEMICOLON { $$ = new_node("ConstDecl", 2, $2, $3); };
ConstDef_List:
    ConstDef { $$ = new_node("ConstDef_List", 1, $1); }
    | ConstDef COMMA ConstDef_List { $$ = new_node("ConstDef_List", 2, $1, $3); }
    ;
ConstDef: IDENT ConstSubscript_List ASSIGN ConstInitVal { $$ = new_node("ConstDef", 3, $1, $2, $4); };

VarDecl: BType VarDef_List SEMICOLON { $$ = new_node("VarDecl", 2, $1, $2); };
VarDef_List:
    VarDef { $$ = new_node("VarDef_List", 1, $1); }
    | VarDef COMMA VarDef_List { $$ = new_node("VarDef_List", 2, $1, $3); }
    ;
VarDef:
    IDENT ConstSubscript_List { $$ = new_node("VarDef", 2, $1, $2); }
    | IDENT ConstSubscript_List ASSIGN InitVal { $$ = new_node("VarDef_Init", 3, $1, $2, $4); }
    ;

ConstSubscript_List:
    /* empty */ { $$ = new_node("ArrayDimList", 0); }
    | LBRACK ConstExp RBRACK ConstSubscript_List { $$ = new_node("ArrayDimList", 2, $2, $4); }
    ;

ConstInitVal:
    ConstExp { $$ = $1; }
    | LBRACE OptConstInitVal_List RBRACE { $$ = new_node("InitVal_Aggregate", 1, $2); }
    ;
OptConstInitVal_List:
    /* empty */ { $$ = new_node("ConstInitVal_List", 0); }
    | ConstInitVal_List { $$ = $1; }
    ;
ConstInitVal_List:
    ConstInitVal { $$ = new_node("ConstInitVal_List", 1, $1); }
    | ConstInitVal COMMA ConstInitVal_List { $$ = new_node("ConstInitVal_List", 2, $1, $3); }
    ;

InitVal:
    Exp { $$ = $1; }
    | LBRACE OptInitVal_List RBRACE { $$ = new_node("InitVal_Aggregate", 1, $2); }
    ;
OptInitVal_List:
    /* empty */ { $$ = new_node("InitVal_List", 0); }
    | InitVal_List { $$ = $1; }
    ;
InitVal_List:
    InitVal { $$ = new_node("InitVal_List", 1, $1); }
    | InitVal COMMA InitVal_List { $$ = new_node("InitVal_List", 2, $1, $3); }
    ;

FuncDef: BType IDENT LPAREN FuncFParamsOpt RPAREN Block { $$ = new_node("FuncDef", 4, $1, $2, $4, $6); };

FuncFParamsOpt:
    /* empty */   { $$ = new_node("FuncFParamsOpt", 0); }
    | FuncFParams { $$ = new_node("FuncFParamsOpt", 1, $1); }
    ;
FuncFParams:
    FuncFParam { $$ = new_node("FuncFParams", 1, $1); }
    | FuncFParam COMMA FuncFParams { $$ = new_node("FuncFParams", 2, $1, $3); }
    ;
FuncFParam:
    BType IDENT { $$ = new_node("FuncFParam", 2, $1, $2); }
    | BType IDENT LBRACK RBRACK ConstSubscript_List { $$ = new_node("FuncFParam_Array", 3, $1, $2, $5); }
    ;

Block: LBRACE BlockItem_List RBRACE { $$ = new_node("Block", 1, $2); };
BlockItem_List:
    /* empty */      { $$ = new_node("BlockItemList", 0); }
    | BlockItem BlockItem_List { $$ = new_node("BlockItemList", 2, $1, $2); }
    ;
BlockItem: Decl { $$ = $1; } | Stmt { $$ = $1; };

Stmt:
    LVal ASSIGN Exp SEMICOLON { $$ = new_node("AssignStmt", 2, $1, $3); }
    | ExpOpt SEMICOLON         { $$ = new_node("ExpStmt", 1, $1); }
    | Block                    { $$ = $1; }
    | IF LPAREN Cond RPAREN Stmt %prec IFX { $$ = new_node("IfStmt", 2, $3, $5); }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt { $$ = new_node("IfElseStmt", 3, $3, $5, $7); }
    | WHILE LPAREN Cond RPAREN Stmt { $$ = new_node("WhileStmt", 2, $3, $5); }
    | BREAK SEMICOLON          { $$ = new_node("BreakStmt", 0); }
    | CONTINUE SEMICOLON       { $$ = new_node("ContinueStmt", 0); }
    | RETURN ExpOpt SEMICOLON  { $$ = new_node("ReturnStmt", 1, $2); }
    ;

ExpOpt:
    /* empty */ { $$ = new_node("ExpOpt", 0); }
    | Exp       { $$ = new_node("ExpOpt", 1, $1); }
    ;
Cond: LOrExp { $$ = $1; };
ConstExp: AddExp { $$ = $1; };

Exp: AddExp { $$ = $1; };

LOrExp: LAndExp { $$ = $1; } | LOrExp OR LAndExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

LAndExp: EqExp { $$ = $1; } | LAndExp AND EqExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

EqExp: RelExp { $$ = $1; } | EqExp EQ RelExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | EqExp NEQ RelExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

RelExp: AddExp { $$ = $1; } | RelExp LT AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp GT AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp LTE AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | RelExp GTE AddExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

AddExp: MulExp { $$ = $1; } | AddExp PLUS MulExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | AddExp MINUS MulExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

MulExp: UnaryExp { $$ = $1; } | MulExp MUL UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | MulExp DIV UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); } | MulExp MOD UnaryExp { $$ = new_node("BinaryOp", 3, $2, $1, $3); };

UnaryExp:
    PrimaryExp { $$ = $1; }
    | IDENT LPAREN FuncRParamsOpt RPAREN { $$ = new_node("FuncCall", 2, $1, $3); }
    | UnaryOp UnaryExp { $$ = new_node("UnaryOp", 2, $1, $2); }
    ;
PrimaryExp:
    LPAREN Exp RPAREN { $$ = $2; }
    | LVal { $$ = $1; }
    | Number { $$ = $1; }
    ;
Number: INCONST { $$ = $1; } | FLOACONST { $$ = $1; };
UnaryOp: PLUS {$$ = $1;} | MINUS {$$ = $1;} | NOT {$$ = $1;};

LVal:
    IDENT { $$ = new_node("LVal", 1, $1); } // Base case for LVal
    | LVal LBRACK Exp RBRACK { $$ = new_node("ArrayAccess", 2, $1, $3); }
    ;

FuncRParamsOpt:
    /* empty */   { $$ = new_node("FuncRParamsOpt", 0); }
    | FuncRParams { $$ = new_node("FuncRParamsOpt", 1, $1); }
    ;
FuncRParams:
    Exp { $$ = new_node("FuncRParams", 1, $1); }
    | Exp COMMA FuncRParams { $$ = new_node("FuncRParams", 2, $1, $3); }
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        printf("Usage: %s <input-file> [ir-output-file] [AST-output-file]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror(argv[1]);
        return 1;
    }
    yyin = file;

    const char* ir_output_filename = argv[2] ;
    FILE *ast_output_file = fopen(argv[3], "w");
    if (yyparse() == 0) {
        write_AST(ast_root, ast_output_file, 0);
        fflush(ast_output_file);
        generate_llvm_ir(ast_root, ir_output_filename);
    } else {
        printf("Parse failed.\n");
    }
    fclose(ast_output_file);
    fclose(file);
    return 0;
}