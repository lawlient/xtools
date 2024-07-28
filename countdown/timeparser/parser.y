%{
#include "time-parser.h"
#include <stdio.h>

extern int yyerror(const char*);
extern int yylex();

#define YYSTYPE long

TSMode tsmode = 0;
time_t tscmd = 0;

%}

%token RELATIVE
%token ABSOLUTE

%%

program: /* empty */
|   statement { tscmd += $1; }
;
 
statement:
    relative { $$ = $1; tsmode = REL; }
|   ABSOLUTE { $$ = $1; tsmode = ABS; }
;

relative:
    RELATIVE { $$ = $1; }
|   relative RELATIVE { $$ += $2; }
;


%%

int yyerror(const char* msg) {
    printf("%d: %s\n", __LINE__, msg);
    return 0;
}