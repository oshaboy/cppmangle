CC:=clang
CPP:=clang++
FLASG := -Wall  -O3 -fPIC -Wno-non-c-typedef-for-linkage
OBJECTS := mangle.o allocator.o substitutions.o cpptypes.o demangle.o

all: test libmangle.so 
debug: FLASG := -Wall \
	-Wno-non-c-typedef-for-linkage -g  \
	-fPIC -Wno-unused-command-line-argument \
	-fsanitize=undefined \
	-fno-sanitize-recover=undefined
debug: all 
test: run_tests
mangle.o: mangle.c *.h 
	$(CC) $(FLASG) -S mangle.c
	$(CC) $(FLASG) -c mangle.s
substitutions.o: substitutions.c *.h
	$(CC) $(FLASG) -S substitutions.c
	$(CC) $(FLASG) -c substitutions.s

cpptypes.o: cpptypes.c *.h
	$(CC) $(FLASG) -S cpptypes.c
	$(CC) $(FLASG) -c cpptypes.s
demangle.o: demangle.c *.h 
	$(CC) $(FLASG) -S demangle.c
	$(CC) $(FLASG) -c demangle.s
mangling_unit_tests.o: mangling_unit_tests.c *.h 
	$(CC) $(FLASG) -c mangling_unit_tests.c
allocator.o: allocator.c allocator.h 
	$(CC) $(FLASG) -c allocator.c

mangling_unit_tests: libmangle.so mangling_unit_tests.o 
	$(CC) $(FLASG) -o mangling_unit_tests mangling_unit_tests.o ./libmangle.so -lcheck
run_tests: mangling_unit_tests 
	./mangling_unit_tests

libmangle.so: $(OBJECTS)
	$(CPP) $(FLASG) -shared -o libmangle.so $(OBJECTS)

clean: 
	rm -f *.o *.so *.s 
	rm -f mangling_unit_tests mangle_test
