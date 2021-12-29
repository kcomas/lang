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

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CCOBJ)

token$(TNAME): $(SRC)/token.o $(TEST)/token.o $(TMAIN)
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
