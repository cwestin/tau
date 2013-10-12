
%language "C"
%define api.pure full /* generate a re-entrant parser */

%union {
    int dummy;
    const char *pString;
}

%token           AS
%token           DOT
%token <pString> ID
%token           IMPORT
%token           PACKAGE
%token           SEMICOLON

%type <foo> comp_unit

%start comp_unit

%%

comp_unit
  : package_spec
  ;

package_spec
  : PACKAGE qualified_name SEMICOLON
  ;

qualified_name
  : ID
  | ID DOT qualified_name
  ;
