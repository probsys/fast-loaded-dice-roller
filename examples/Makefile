all: example.out examplef.out

%.out: %.c
	gcc -o $@ -O3 \
		-I ../build/include \
		-L ../build/lib \
		$^ -lfldr -lm

.PHONY: clean
clean:
	rm -rf *.out
