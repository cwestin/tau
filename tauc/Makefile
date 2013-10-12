
# TODO(cwestin) share these with the generated makefile in crtl
CC = cc
DEBUG = -g
INCLUDE = ../crtl/include/
CFLAGS = -Wall -Wno-invalid-offsetof -I$(INCLUDE) $(DEBUG)

# use the most recent versions of tools
FLEX = /usr/local/bin/flex
BISON = /usr/local/bin/bison

tauc:	lex.yy.c
	$(CC) $(CFLAGS) -o tauc lex.yy.c

grammar.tab.h grammar.tab.c:	grammar.y
	$(BISON) grammar.y

lex.yy.c:	grammar.tab.h lexemes.l
	$(FLEX) lexemes.l

clean:
	-@rm tauc 2>/dev/null || true
	-@rm *.o 2>/dev/null || true
	-@rm grammar.tab.c 2>/dev/null || true
	-@rm lex.yy.c 2>/dev/null || true
