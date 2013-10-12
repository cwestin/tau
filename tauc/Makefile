
# TODO(cwestin) share these with the generated makefile in crtl
CC = cc
DEBUG = -g
INCLUDE = /Users/cwestin/git/tau/crtl/include/
CFLAGS = -Wall -Wno-invalid-offsetof -I$(INCLUDE) $(DEBUG)

# use the most recent versions of tools
FLEX = /usr/local/bin/flex

tauc:	lex.yy.c
	$(CC) $(CFLAGS) -o tauc lex.yy.c

lex.yy.c:	lexer.l
	$(FLEX) lexer.l

clean:
	-@rm tauc 2>/dev/null
	-@rm lex.yy.c 2>/dev/null
	-@rm *.o 2>/dev/null
