%{

#include <stdio.h>
#include <string.h>
#include "y.tab.h"
int c;
%}
%%
" \t"     ;
[0-9]     {
            c = yytext[0];
            yylval.a = c - '0';
            return DIGIT;
          }
[A-Za-z_][A-Za-z_0-9]*   {
                           yylval.identifier = strdup(yytext);
                           return IDENTIFIER;
                         }
\"(\\.|[^"\\])*\" {
                    yylval.strval = strdup(yytext);
                    return STRING;
                  }
=                 {
                    return EQUAL;
                  }
\n                {
                    return NEWLINE;
                  }
%%
