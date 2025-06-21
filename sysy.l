%{
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "sysy.tab.h"
struct Node* new_node(const char* name, int num_children, ...);

// 定义一个安全的宏，用于创建 Token 节点并返回
// 1. 使用当前匹配的文本(yytext)创建一个新的叶子节点
// 2. 将返回的 Node* 指针存入 yylval 的 .node_ptr 字段
// 3. 返回 Token 的 ID (t)
#define TOKEN(t) (yylval.node_ptr = new_node(yytext, 0), t)
%}

D               [0-9]
L               [a-zA-Z_]
H               [a-fA-F0-9]
E               [Ee][+-]?{D}+

INT   0[xX]{H}+|0{D}+|{D}+
FLOAT ({D}*\.{D}+|{D}+\.)({E})?|{D}+{E}

%option yylineno
%x COMMENT

%%

"/*"                    { BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n             { yylineno++; }
<COMMENT>.              ;
<COMMENT><<EOF>>        { printf("Error: line %d: unclosed comment\n", yylineno); return 0; }

\/\/[^\n]* ;

[ \t\r]+                ;
\n                      { yylineno++; }


"const"                 { return TOKEN(CONST); }
"int"                   { return TOKEN(INT); }
"float"                 { return TOKEN(FLOAT); }
"void"                  { return TOKEN(VOID); }
"if"                    { return TOKEN(IF); }
"else"                  { return TOKEN(ELSE); }
"while"                 { return TOKEN(WHILE); }
"break"                 { return TOKEN(BREAK); }
"continue"              { return TOKEN(CONTINUE); }
"return"                { return TOKEN(RETURN); }

{INT}         { return TOKEN(INCONST); }
{FLOAT}       { return TOKEN(FLOACONST); }

{L}({L}|{D})* { return TOKEN(IDENT); }

"=="                    { return TOKEN(EQ); }
"!="                    { return TOKEN(NEQ); }
"<="                    { return TOKEN(LTE); }
">="                    { return TOKEN(GTE); }
"&&"                    { return TOKEN(AND); }
"||"                    { return TOKEN(OR); }
"+"                     { return TOKEN(PLUS); }
"-"                     { return TOKEN(MINUS); }
"*"                     { return TOKEN(MUL); }
"/"                     { return TOKEN(DIV); }
"%"                     { return TOKEN(MOD); }
"="                     { return TOKEN(ASSIGN); }
"<"                     { return TOKEN(LT); }
">"                     { return TOKEN(GT); }
"!"                     { return TOKEN(NOT); }
"("                     { return TOKEN(LPAREN); }
")"                     { return TOKEN(RPAREN); }
"{"                     { return TOKEN(LBRACE); }
"}"                     { return TOKEN(RBRACE); }
"["                     { return TOKEN(LBRACK); }
"]"                     { return TOKEN(RBRACK); }
","                     { return TOKEN(COMMA); }
";"                     { return TOKEN(SEMICOLON); }

.                       { printf("Error: unexpected character '%s' at line %d\n", yytext, yylineno); }

%%

int yywrap() {
    return 1;
}