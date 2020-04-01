all: fldr.out fldrf.out

fldr.out: flip.c fldr.c sample.c
	gcc -o fldr.out $^

fldrf.out: flip.c fldrf.c samplef.c
	gcc -o fldrf.out $^ -lm

example.out: flip.c fldr.c example.c
	gcc -o example.out $^

examplef.out: flip.c fldrf.c examplef.c
	gcc -o examplef.out $^ -lm

.PHONY: clean
clean:
	rm -rf *.out
