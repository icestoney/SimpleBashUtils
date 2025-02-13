.PHONY: all rebuild clean clang s21_cat s21_cat.o test

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -std=c11

all: s21_cat.o s21_cat

rebuild: clean all

s21_cat.o: s21_cat.c s21_cat.h
	$(CC) $(CFLAGS) s21_cat.c -c

s21_cat: s21_cat.o
	$(CC) $(CFLAGS) s21_cat.o -o s21_cat

cp_heavy_test:
	cp tests/heavy_test/* ./

cp_low_tests:
	cp -r tests/low_tests/* ./

test: s21_cat cp_heavy_test cp_low_tests
	bash cat_test.sh 3 +x > heavy_test.log
	rm -rf cat_geberate.c cat_test.sh
	bash test_leaks.sh +x > low_test1.log
	bash tests.sh +x > low_test2.log
	rm -rf tests.sh test_leaks.sh *.o s21_cat test_files

clang:
	clang-format -style=Google -n s21_cat.c s21_cat.h

clean:
	rm -rf *.o s21_cat *.log tests.sh test_leaks.sh test_files *.o s21_cat cat_geberate.c cat_test.sh