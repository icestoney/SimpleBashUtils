.PHONY: all rebuild clang clean s21_grep s21_grep.o test

CC = gcc
CCFLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -std=c11

all: s21_grep.o s21_grep

rebuild: clean all

s21_grep.o: s21_grep.c s21_grep.h
	$(CC) $(CCFLAGS) s21_grep.c -c

s21_grep: s21_grep.o
	$(CC) $(CCFLAGS) s21_grep.o -o s21_grep

test: s21_grep
	cp ./tests/grep_test.sh ./
	bash grep_test.sh
	rm -rf grep_test.sh

clang:
	clang-format -style=Google -n s21_grep.c s21_grep.h

clean:
	rm -rf *.o s21_grep *.log grep_test.sh