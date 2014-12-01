src = *.c
hdr = *.h
dep = $(hdr) $(src)
bin = a.out

all: proj4

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

proj3:
	gcc -Wall semantic.c proj-3.c -o a.out

test3:
	gcc -Wall semantic.c proj-3.c -o proj-3.o
	for f in $(wildcard ./tests-3/*.txt); do \
	  echo "Testing `basename $$f`"; \
		./proj-3.o < $$f | grep -v DEBUG > ./tests-3/`basename $$f`.o; \
		diff -Bw ./tests-3/`basename $$f`.o ./tests-3/`basename $$f`.expected; \
	done

print3:
	gcc -Wall semantic.c proj-3.c -o proj-3.o
	for f in $(wildcard ./tests-3/*.txt); do \
		echo "Testing `basename $$f`"; \
		./proj-3.o < $$f; \
	done

proj4:
	gcc -Wall compiler.c proj-4.c -o a.out

test4:
	gcc -Wall compiler.c proj-4.c -o proj-4.o
	for f in $(wildcard ./tests-4/*.txt); do \
	  echo "Testing `basename $$f`"; \
		./proj-4.o < $$f | grep -v DEBUG > ./tests-4/`basename $$f`.o; \
		diff -Bw ./tests-4/`basename $$f`.o ./tests-4/`basename $$f`.expected; \
	done

clean:
	rm $(bin);
