.PHONY: all bbob ecf clean clean_bbob clean_ecf


all: bbob ecf

bbob:
	cd bbob; make

ecf:
	make -f Makefile_ECF


clean: clean_bbob clean_ecf

clean_bbob:
	cd bbob; make clean

clean_ecf:
	make -f Makefile_ECF clean


