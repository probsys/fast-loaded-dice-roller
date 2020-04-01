all: libfldr.a sample.out samplef.out

%.o: %.c
	gcc -c $< -O3

libfldr.a: flip.o fldr.o fldrf.o
	ar rcs $@ $^

%.out: %.c libfldr.a
	gcc $< -L. -lfldr -lm -O3 -o $@

.PHONY: clean
clean:
	rm -rf *.a *.o *.out
