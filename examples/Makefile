all: example.out examplef.out

%.out: %.c
	gcc $< -I ../build/include -L ../build/lib -lfldr -lm -O3 -o $@

.PHONY: clean
clean:
	rm -rf *.out
