CC = gcc
CFLAGS = -std=c17 -g -Wall -Wextra
SRC = ./src
TEST = ./test
SOURCES = $(wildcard $(SRC)/*.c)
TEST_SOURCES = $(wildcard $(TEST)/*.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
TEST_OBJECTS = $(patsubst %.c, %.o, $(TEST_SOURCES))
NAME = lang
TNAME = _test
TMAIN = $(TEST)/test.o
CCOBJ = $(CC) -o $@ $^
TOKEN_SOURCES = $(SRC)/token.o
PARSER_SOURCES = $(SRC)/parser.o $(TOKEN_SOURCES)
TYPE_SOURCES = $(SRC)/type.o
AST_SOURCES = $(PARSER_SOURCES) $(TYPE_SOURCES)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CCOBJ)

ast$(TNAME): $(AST_SOURCES) $(TEST)/ast.o $(TMAIN)
	$(CCOBJ)

type$(TNAME): $(TYPE_SOURCES) $(TEST)/type.o $(TMAIN)
	$(CCOBJ)

parser$(TNAME): $(PARSER_SOURCES) $(TEST)/parser.o $(TMAIN)
	$(CCOBJ)

token$(TNAME): $(TOKEN_SOURCES) $(TEST)/token.o $(TMAIN)
	$(CCOBJ)

basic$(TNAME): $(TEST)/basic.o $(TMAIN)
	$(CCOBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(SRC)/*.o
	rm -f $(TEST)/*.o
	rm -f $(NAME)
	rm -f *$(TNAME)
