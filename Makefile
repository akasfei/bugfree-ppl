src = *.c
hdr = *.h
dep = $(hdr) $(src)
bin = a.out

all: proj2

proj1:
	gcc -Wall proj-1.c lexer.c -o proj-1.o

proj2:
	gcc -Wall proj-2*.c -o a.out

test2:
	gcc -Wall proj-2*.c -o proj-2.o
	./proj-2.o 1 < tests/test01.txt
	./proj-2.o 1 < tests/test02.txt
	./proj-2.o 1 < tests/test03.txt
	./proj-2.o 1 < tests/test04.txt
	./proj-2.o 1 < tests/test05.txt
	./proj-2.o 2 < tests/test01.txt
	./proj-2.o 2 < tests/test02.txt
	./proj-2.o 2 < tests/test03.txt
	./proj-2.o 2 < tests/test04.txt
	./proj-2.o 2 < tests/test05.txt
	./proj-2.o 3 < tests/test01.txt
	./proj-2.o 3 < tests/test02.txt
	./proj-2.o 3 < tests/test03.txt
	./proj-2.o 3 < tests/test04.txt
	./proj-2.o 3 < tests/test05.txt

clean:
	rm $(bin);
