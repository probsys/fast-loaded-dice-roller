all: libfldr.a sample.out samplef.out

%.o: %.c
	gcc -c -o $@ -O3 $^

libfldr.a: flip.o fldr.o fldrf.o
	ar rcs $@ $^

%.out: %.c libfldr.a
	gcc -o $@ -O3 $^ -lm

.PHONY: clean
clean:
	rm -rf *.a *.o *.out
