.PHONY:all
all: c python

.PHONY:c
c: src/c
	$(MAKE) -C src/c
	mkdir -p build/bin
	cp src/c/fldr.out build/bin/fldr

.PHONY:python
python: setup.py
	python setup.py build

.PHONY:clean
clean:
	rm -rf build
	$(MAKE) -C src/c clean
