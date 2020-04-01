.PHONY:all
all: c python

.PHONY:c
c: src/c
	$(MAKE) -C src/c
	mkdir -p build/bin
	cp src/c/sample.out build/bin/fldr
	cp src/c/samplef.out build/bin/fldrf
	mkdir -p build/lib
	cp src/c/libfldr.a build/lib
	mkdir -p build/include
	cp src/c/*.h build/include
	$(MAKE) -C src/c clean

.PHONY:python
python: setup.py
	python setup.py build

.PHONY:clean
clean:
	rm -rf build
	$(MAKE) -C src/c clean
