FLAGS=-Wall -Werror -Wextra
all: s21_cat

s21_cat: cat.o
	gcc $(FLAGS) s21_cat.c -o s21_cat

cat_test: test.sh
	bash test.sh

cat.o: s21_cat.c
	gcc $(FLAGS) -c s21_cat.c

clean:
	rm -rf *.o
	rm -rf s21_cat
	rm -rf test