
# TODO(cwestin) share these with the generated makefile in crtl
CC = cc
DEBUG = -g
INCLUDE = /Users/cwestin/git/tau/crtl/include/
CFLAGS = -Wall -Wno-invalid-offsetof -I$(INCLUDE) $(DEBUG)

tauc:	lex.yy.c
	$(CC) $(CFLAGS) -o tauc lex.yy.c

lex.yy.c:	lexer.l
	flex lexer.l
