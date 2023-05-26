#include "cppmangle.h"
#include <dlfcn.h>
#include <stdio.h>

int main(void){
	
	void * cpplib;
	BumpAllocator allocator={0};
	IdentifierData cout_id;
	const char * cout_str;
	std_ostream_64 REF cout;
	MethodIdentifierData cout_func_id;
	const char * mangle_id;
	void (*cout_func)(std_ostream_64 REF, int);
	cpplib=dlopen("libstdc++.so",RTLD_NOW);
	cout_id = createGlobalIdentifierData(
			"cout",
			(const char * []){
				"std",
				NULL
			},
			&allocator
		);
	cout_str=mangle(&cout_id, &allocator);
	printf("%s\n", cout_str);
	cout=dlsym(cpplib,cout_str);
	cout_func_id=createSpecialMethodIdentifierData(
			LEFT_SHIFT_OPERATOR, 
			(const char * []){
				"std",
				NULL
			},
			2,(const TypeIdentifier [2]){
				std_ostream_lvalue_ref_identifier,
				int_identifier
			},
			&allocator);
	mangle_id=mangle(&cout_func_id, &allocator);
	printf("%s\n", mangle_id);
	cout_func=dlsym(cpplib, mangle_id);
	cout_func(cout, 42);
	
	return 0;
	
	
}