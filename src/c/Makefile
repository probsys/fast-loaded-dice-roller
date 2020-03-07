all: fldr.out example.out

fldr.out: flip.c fldr.c sample.c
	gcc -o fldr.out $^

example.out: flip.c fldr.c example.c
	gcc -o example.out $^

.PHONY: clean
clean:
	rm -rf *.out
