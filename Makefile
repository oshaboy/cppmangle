CC:=clang
CPP:=clang++
FLASG := -Wall  -O3 -fPIC

all: test libmangle.so mangled_stuff.s
debug: FLASG := -Wall -Wno-unused-variable -g -fPIC
debug: mangle_test all 
test: mangle_cpp.so run_tests
mangle.o: mangle.c *.h 
	$(CC) $(FLASG) -S mangle.c
	$(CC) $(FLASG) -c mangle.s
substitutions.o: substitutions.c *.h
	$(CC) $(FLASG) -S substitutions.c
	$(CC) $(FLASG) -c substitutions.s

cpptypes.o: cpptypes.c *.h
	$(CC) $(FLASG) -S cpptypes.c
	$(CC) $(FLASG) -c cpptypes.c
mangling_unit_tests.o: mangling_unit_tests.c *.h 
	$(CC) $(FLASG) -c mangling_unit_tests.c
allocator.o: allocator.c allocator.h 
	$(CC) $(FLASG) -c allocator.c

mangling_unit_tests: libmangle.so mangling_unit_tests.o 
	$(CC) $(FLASG) -o mangling_unit_tests mangling_unit_tests.o ./libmangle.so -lcheck
run_tests: mangling_unit_tests 
	./mangling_unit_tests
mangle_cpp.so: mangle.cpp 
	$(CPP) $(FLASG) -shared -o mangle_cpp.so mangle.cpp
mangled_stuff.s: mangle.cpp
	$(CPP) -o mangled_stuff.s -O -S mangle.cpp
libmangle.so: mangle.o allocator.o substitutions.o cpptypes.o
	$(CPP) $(FLASG) -shared -o libmangle.so mangle.o allocator.o substitutions.o cpptypes.o
mangle_test.o: mangle_test.c
	$(CC) $(FLASG) -S mangle_test.c
	$(CC) $(FLASG) -c mangle_test.s
mangle_test: mangle_test.o libmangle.so
	$(CC) $(FLASG) -o mangle_test mangle_test.o  ./libmangle.so
clean: 
	rm -f *.o *.so *.s 
	rm -f mangling_unit_tests mangle_test
