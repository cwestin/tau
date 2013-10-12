/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

%option warn
%option reentrant
%option stack
%option noyywrap
%option bison-bridge

%{
#include "grammar.tab.h"
%}

/* scanner states */
%x LINECOMMENT
%x COMMENT

/* definitions */
DIGIT [0-9]
ID [_a-zA-Z][_a-zA-Z0-9]*

%%

"//" { yy_push_state(LINECOMMENT, yyscanner); }
[:space:]+

<LINECOMMENT>\n { yy_pop_state(yyscanner); }
<LINECOMMENT>[^\n]+

"as" { return AS; }
"\." { return DOT; }
"import" { return IMPORT; }
"package" { return PACKAGE; }
";" { return SEMICOLON; }

%%

int main(int argc, char *argv[])
{
    yyscan_t scanner;
    YYSTYPE *pyylval = NULL;

    yylex_init(&scanner);
    yylex(pyylval, scanner);
    yylex_destroy(scanner);

    return 0;
}
