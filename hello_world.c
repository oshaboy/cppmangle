#include "cppmangle.h"
#include <dlfcn.h>
struct ostream{
	char _d[272];
};
int main(void){
	void * cpplib=dlopen("clang-cpp.so",RTLD_NOW);
	BumpAllocator allocator={0};
	const IdentifierData cout_id = createGlobalIdentifierData(
			"cout",
			(const char * []){
				std_namespace_string,
				NULL
			},
			&allocator
		);
	struct ostream REF cout=dlsym(cpplib,mangle(&cout_id, &allocator));
	MethodIdentifierData cout_func_id=createSpecialMethodIdentifierData(
			LEFT_SHIFT_OPERATOR, 
			(const char * []){
				std_namespace_string,
				NULL
			},
			2,(const TypeIdentifier [2]){
				std_ostream_lvalue_ref_identifier,
				createTypeId(
					char_string,
					NULL,
					(POINTER_QUALIFIER []){CONSTANT, END},
					0,
					&allocator
				)
			},
			&allocator);

	void (*cout_func)(struct ostream REF, const char *)=dlsym(cpplib, mangle(&cout_func_id, &allocator));
	cout_func(cout, "Hello, World\n");
	return 0;
	
	
}