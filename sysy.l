%{
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "sysy.tab.h"
struct Node* new_node(const char* name, int num_children, ...);
// 声明一个全局文件指针
FILE *lex_output_file = NULL;

// 定义一个函数，用于从外部（main函数）设置输出文件
void set_lex_output_file(const char* filename) {
    if (filename) {
        lex_output_file = fopen(filename, "w");
        if (!lex_output_file) {
            perror("Could not open lexer output file");
            exit(1);
        }
    }
}

// 定义一个函数，用于关闭文件
void close_lex_output_file() {
    if (lex_output_file) {
        fclose(lex_output_file);
    }
}
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
[ \t]       ;
\n          { yylineno++; }

"/*"        { BEGIN(COMMENT); }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>\n  { yylineno++; }
<COMMENT>.   ;

"const"     { if(lex_output_file) fprintf(lex_output_file, "(CONST, _)\n"); return TOKEN(CONST); }
"int"       { if(lex_output_file) fprintf(lex_output_file, "(INT, _)\n"); return TOKEN(INT); }
"float"     { if(lex_output_file) fprintf(lex_output_file, "(FLOAT, _)\n"); return TOKEN(FLOAT); }
"void"      { if(lex_output_file) fprintf(lex_output_file, "(VOID, _)\n"); return TOKEN(VOID); }
"if"        { if(lex_output_file) fprintf(lex_output_file, "(IF, _)\n"); return TOKEN(IF); }
"else"      { if(lex_output_file) fprintf(lex_output_file, "(ELSE, _)\n"); return TOKEN(ELSE); }
"while"     { if(lex_output_file) fprintf(lex_output_file, "(WHILE, _)\n"); return TOKEN(WHILE); }
"break"     { if(lex_output_file) fprintf(lex_output_file, "(BREAK, _)\n"); return TOKEN(BREAK); }
"continue"  { if(lex_output_file) fprintf(lex_output_file, "(CONTINUE, _)\n"); return TOKEN(CONTINUE); }
"return"    { if(lex_output_file) fprintf(lex_output_file, "(RETURN, _)\n"); return TOKEN(RETURN); }

{INT}  { if(lex_output_file) fprintf(lex_output_file, "(NUM, \"%s\")\n", yytext); return TOKEN(INCONST); }

{FLOAT}    { if(lex_output_file) fprintf(lex_output_file, "(NUM, %s)\n", yytext); return TOKEN(FLOACONST); }

{L}({L}|{D})*       { if(lex_output_file) fprintf(lex_output_file, "(ID, \"%s\")\n", yytext); return TOKEN(IDENT); }

"=="          { if(lex_output_file) fprintf(lex_output_file, "(==, _)\n"); return TOKEN(EQ); }
"!="          { if(lex_output_file) fprintf(lex_output_file, "(!=, _)\n"); return TOKEN(NEQ); }
"<="          { if(lex_output_file) fprintf(lex_output_file, "(<=, _)\n"); return TOKEN(LTE); }
">="          { if(lex_output_file) fprintf(lex_output_file, "(>=, _)\n"); return TOKEN(GTE); }
"&&"          { if(lex_output_file) fprintf(lex_output_file, "(&&, _)\n"); return TOKEN(AND); }
"||"          { if(lex_output_file) fprintf(lex_output_file, "(||, _)\n"); return TOKEN(OR); }
"+"           { if(lex_output_file) fprintf(lex_output_file, "(+, _)\n"); return TOKEN(PLUS); }
"-"           { if(lex_output_file) fprintf(lex_output_file, "(-, _)\n"); return TOKEN(MINUS); }
"*"           { if(lex_output_file) fprintf(lex_output_file, "(*, _)\n"); return TOKEN(MUL); }
"/"           { if(lex_output_file) fprintf(lex_output_file, "(/, _)\n"); return TOKEN(DIV); }
"%"           { if(lex_output_file) fprintf(lex_output_file, "(%%, _)\n"); return TOKEN(MOD); }
"="           { if(lex_output_file) fprintf(lex_output_file, "(=, _)\n"); return TOKEN(ASSIGN); }
"<"           { if(lex_output_file) fprintf(lex_output_file, "(<, _)\n"); return TOKEN(LT); }
">"           { if(lex_output_file) fprintf(lex_output_file, "(>, _)\n"); return TOKEN(GT); }
"!"           { if(lex_output_file) fprintf(lex_output_file, "(!, _)\n"); return TOKEN(NOT); }
"("           { if(lex_output_file) fprintf(lex_output_file, "((, _)\n"); return TOKEN(LPAREN); }
")"           { if(lex_output_file) fprintf(lex_output_file, "(), _)\n"); return TOKEN(RPAREN); }
"{"           { if(lex_output_file) fprintf(lex_output_file, "({, _)\n"); return TOKEN(LBRACE); }
"}"           { if(lex_output_file) fprintf(lex_output_file, "(}, _)\n"); return TOKEN(RBRACE); }
"["           { if(lex_output_file) fprintf(lex_output_file, "([, _)\n"); return TOKEN(LBRACK); }
"]"           { if(lex_output_file) fprintf(lex_output_file, "(], _)\n"); return TOKEN(RBRACK); }
","           { if(lex_output_file) fprintf(lex_output_file, "(,, _)\n"); return TOKEN(COMMA); }
";"           { if(lex_output_file) fprintf(lex_output_file, "(;, _)\n"); return TOKEN(SEMICOLON); }

.           { printf("Error: unexpected character '%s' at line %d\n", yytext, yylineno); }

%%

int yywrap() {
    return 1;
}