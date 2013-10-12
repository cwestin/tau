
%language "C"
%define api.pure full /* generate a re-entrant parser */
%defines

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

%type <pString> comp_unit
%type <pString> package_spec
%type <pString> qualified_name

%start comp_unit

%%

comp_unit
  : package_spec
    { return $1; }
  ;

package_spec
  : PACKAGE qualified_name SEMICOLON
    { return $2; }
  ;

qualified_name
  : ID
  | ID DOT qualified_name
    { return $3; }
  ;
