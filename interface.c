#include "cppmangle.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
const char * mangle(
	const char * id,
	size_t nests_n,
	size_t argtype_n,
	...
){
	BumpAllocator allocator ={0};
	char ** nests=bump_alloc(&allocator, sizeof(char *)*nests_n+1);

	TypeIdentifier * argtypes;
	va_list args;
	va_start(args, argtype_n);
	for (size_t i=0; i<nests_n; i++)
		nests[i]=va_arg(args, char *);
	nests[nests_n]=NULL;
	IdentifierData d;
	if (argtype_n!=NOT_METHOD){
		argtypes=
			bump_alloc(&allocator, sizeof(TypeIdentifier)*argtype_n);
		for (size_t i=0; i<argtype_n; i++)
			argtypes[i]=va_arg(args, TypeIdentifier);
		d=createMethodIdentifierData(
			id,
			(const char **)nests,
			argtype_n,
			argtypes,
			&allocator
		);
	} else {
		d=createGlobalIdentifierData(
			id,
			(const char *const *)nests,
			&allocator
		);
	}
	const char * id_data=mangleIdentifierData(&d, &allocator);
	//Pass ownership to caller
	char * result=malloc(strlen(id_data)*sizeof(char));
	strcpy(result, id_data);
	bump_free(&allocator);
	return result;
}
static void decoupleFromAllocator(TypeIdentifier * ti){
	{
	const size_t num_of_ptrs=(ti->member_pointers_end-ti->member_pointers);
	POINTER_QUALIFIER * new_pointers=malloc(num_of_ptrs*sizeof(POINTER_QUALIFIER));
	memcpy(new_pointers, ti->member_pointers,num_of_ptrs*sizeof(POINTER_QUALIFIER));
	ti->member_pointers=new_pointers;
	ti->member_pointers_end=new_pointers+num_of_ptrs;
	}
	{
	LenId * new_nests;
	if (ti->member_aux_nest_count > 0 ){
		new_nests=malloc(ti->member_aux_nest_count*sizeof(LenId));
		memcpy(new_nests, ti->member_aux_nests, ti->member_aux_nest_count*sizeof(LenId));
		ti->member_aux_nests=new_nests;
	}
	}
	if (ti->member_ismethodtype){
		if (ti->method.member_return_type){
			TypeIdentifier * new_return_type=malloc(sizeof(TypeIdentifier));
			*new_return_type=*ti->method.member_return_type;
			decoupleFromAllocator(new_return_type);
			ti->method.member_return_type=new_return_type;
		}
		if (ti->method.member_arg_count>0) {
			TypeIdentifier * const new_argtypes=
				malloc(ti->method.member_arg_count * sizeof(TypeIdentifier));
		
			memcpy(new_argtypes, ti->method.member_argtypes, ti->method.member_arg_count * sizeof(TypeIdentifier));
			
			for (TypeIdentifier * ti_ptr=new_argtypes;
				ti_ptr<new_argtypes+ti->method.member_arg_count;
				ti_ptr++
			){
				decoupleFromAllocator(ti_ptr);
				
			}
			ti->method.member_argtypes=new_argtypes;
		}
	} else {
		char * new_name=malloc(ti->methodnt.name_len+1);
		strcpy(new_name,ti->methodnt.name);
		ti->methodnt.name=new_name;
	}

}
const TypeIdentifier createTypeIdentifier(
	const char * base,
	const POINTER_QUALIFIER * ptrs,
	unsigned long flags,
	size_t nests_n,
	...
){
	BumpAllocator allocator ={0};
	char ** nests=bump_alloc(&allocator, sizeof(char *)*(nests_n+1));
	va_list args;
	va_start(args, nests_n);
	for (size_t i=0; i<nests_n; i++)
		nests[i]=va_arg(args, char *);
	nests[nests_n]=NULL;
	TypeIdentifier result= createTypeId_(
		base,
		(const char *const *)nests,
		ptrs,
		flags,
		&allocator
	);
	decoupleFromAllocator(&result);
	bump_free(&allocator);
	return result;
	
	
}
const TypeIdentifier createFunctionPtrTypeIdentifier(
	const TypeIdentifier * returns,
	const POINTER_QUALIFIER * ptrs,
	unsigned long flags,
	size_t nests_n, size_t arg_n,
	...
){
	BumpAllocator allocator ={0};
	char ** nests=bump_alloc(&allocator, sizeof(char *)*(nests_n+1));
	va_list args;
	va_start(args, arg_n);
	for (size_t i=0; i<nests_n; i++)
		nests[i]=va_arg(args, char *);
	nests[nests_n]=NULL;
	TypeIdentifier * argtypes=bump_alloc(&allocator, sizeof(TypeIdentifier)*arg_n);
	for (size_t i=0; i<arg_n; i++){
		argtypes[i]=va_arg(args, TypeIdentifier);
	}
	TypeIdentifier result= createFunctionPtrTypeId_(
		returns,
		arg_n,
		argtypes,
		(const char *const *)nests,
		ptrs,
		flags,
		&allocator
	);
	decoupleFromAllocator(&result);
	bump_free(&allocator);
	return result;
}

const char * mangleSpecialMethod(
	SPECIAL_METHOD tag,
	size_t nests_n,
	const size_t argtype_n,
	...
){
	assert(argtype_n!=NOT_METHOD);

	BumpAllocator allocator ={0};
	char ** nests=bump_alloc(&allocator, sizeof(char *)*nests_n+1);

	TypeIdentifier * argtypes;
	va_list args;
	va_start(args, argtype_n);
	for (size_t i=0; i<nests_n; i++)
		nests[i]=va_arg(args, char *);
	nests[nests_n]=NULL;
	IdentifierData d;
	argtypes=
		bump_alloc(&allocator, sizeof(TypeIdentifier)*argtype_n);
	for (size_t i=0; i<argtype_n; i++)
		argtypes[i]=va_arg(args, TypeIdentifier);
	d=createSpecialMethodIdentifierData(
		tag,
		(const char *const *)nests,
		argtype_n,
		argtypes,
		&allocator
	);
	const char * id_data=mangleIdentifierData(&d, &allocator);
	//Pass ownership to caller
	char * result=malloc(strlen(id_data)*sizeof(char));
	strcpy(result, id_data);
	bump_free(&allocator);
	return result;
}
void destroyTypeId(TypeIdentifier * ti){
	free((void *)ti->member_pointers);
	free((void *)ti->member_aux_nests);
	if (ti->member_ismethodtype){
		destroyTypeId((void *)ti->method.member_return_type);
		for (TypeIdentifier * ti_ptr=ti->method.member_argtypes;
			ti_ptr<ti->method.member_argtypes+ti->method.member_arg_count;
			ti_ptr++
		)
			free(ti_ptr);
	} else {
		free((void *)ti->methodnt.name);
	}
}