%{
#include <stdio.h>
#include <stdlib.h>
#include "pollen.h"

int regs[26];
int base;
char *config_name;
char *config_string_value;

void yyerror(const char *s);
int yylex();

%}

%start list

%union { int a; char *identifier; char *strval; }


%token<a> DIGIT IDENTIFIER STRING EQUAL NEWLINE


%type<a> statement number

%%                   /* beginning of rules section */

list:                       /*empty */
         |
        list statement NEWLINE
         |
        list error NEWLINE
         {
           yyerrok;
         }
         ;
statement: identifier EQUAL string
             {
               int rc;
               rc = set_config_option_string(config_name, config_string_value);
               free(config_name);
               free(config_string_value);
               if (rc)
                 exit(1);
             }
             |
             identifier EQUAL number
             {
               int rc;
               rc = set_config_option_int(config_name, $3);
               free(config_name);
               if (rc)
                 exit(1);
             }
             ;
identifier: IDENTIFIER
          {
            config_name = yylval.identifier;
          }
          ;
string: STRING
          {
            config_string_value = yylval.strval;
          }

number:  DIGIT
         {
           $$ = $1;
           base = ($1==0) ? 8 : 10;
         }       |
         number DIGIT
         {
           $$ = base * $1 + $2;
         }
         ;

%%
int read_config_file()
{
	return(yyparse());
}

void yyerror(s)
const char *s;
{
	fprintf(stderr, "%s\n",s);
}

int yywrap()
{
	return(1);
}
