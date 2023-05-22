#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <check.h>
#include "cppmangle.h"
#include "cpptypes.h"




START_TEST(dont_mangle_unnested_global){
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createGlobalIdentifierData("myglobal", (const char * []){
			"Nester"
			,NULL
		}, &allocator);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_ZN6Nester8myglobalE");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST
START_TEST(nested_global){
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createGlobalIdentifierData("myglobal", NULL, &allocator);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "myglobal");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST
START_TEST(void_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData("void_func", NULL,0, NULL, &allocator);
	mangled_name=mangle(&d, &allocator);
	ck_assert_str_eq(mangled_name, "_Z9void_funcv");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(int_int_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
	"add", NULL,2, (const TypeIdentifier []){
		int_identifier,
		int_identifier
	},
	&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z3addii");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(nest_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"nested", (const char * []){
			"Nester"
			,NULL
		}
		,
		0,
		NULL,
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_ZN6Nester6nestedEv");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);

} END_TEST

START_TEST(named_argument_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"named", NULL,
		1,
		(TypeIdentifier[]){
			createTypeId(
				"MyName",
				NULL,
				(POINTER_QUALIFIER[]){END},
				IDENTIFIER_BITMASK,
				&allocator
				)
		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z5named6MyName");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);

} END_TEST

START_TEST(pointer_argument_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr", NULL,
		1,
		(TypeIdentifier[]){
			int_ptr_identifier
		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z3ptrPi");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);

} END_TEST


START_TEST(fully_compressed_pointer_argument_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr", NULL,
		2,
		(TypeIdentifier[]){
			int_ptr_identifier,
			int_ptr_identifier

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z3ptrPiS_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST


START_TEST(partially_compressed_pointer_argument_function) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr", NULL,
		3,
		(TypeIdentifier[]){
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,0,END},
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z3ptrPiPS_PS0_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(all_subs_in_first_argument) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"all_in_first", NULL,
		3,
		(TypeIdentifier[]){
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,END},
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z12all_in_firstPPPiS_S0_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(compressed_pointer_argument_function_with_nest) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr", (const char * []){
			"Nester"
			,NULL
		},
		3,
		(TypeIdentifier[]){
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,END},
				0,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,0,0,END},
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_ZN6Nester3ptrEPiPS0_PS1_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(complex_number_argument) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ccomplex",
		NULL,
		2,
		(TypeIdentifier[]){
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){0,END},
				COMPLEX_BITMASK,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){END},
				COMPLEX_BITMASK,
				&allocator
			)
			

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z8ccomplexPCiS_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST
START_TEST(constant_pointer_argument) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"cpointer",
		NULL,
		1,
		(TypeIdentifier[]){
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT,END},
				0,
				&allocator
			)
			

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_str_eq(mangled_name, "_Z8cpointerPKi");
} END_TEST

START_TEST(ellipsis_argument) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"printf",
		NULL,
		2,
		
		(TypeIdentifier[]){
			char_ptr_identifier,
			ellipsis_identifier
			

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z6printfPcz");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(void_function_pointer_argument) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"voidfunc",
		NULL,
		1,
		
		(TypeIdentifier[]){
			createFunctionPtrTypeId(
				NULL,
				0,
				NULL,
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			)
			

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z8voidfuncPFvvE");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(function_pointer_argument_no_subs) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"fptr",
		NULL,
		1,
		
		(TypeIdentifier[]){
			createFunctionPtrTypeId(
				&int_identifier,
				2,
				(TypeIdentifier[]){
					int_identifier,
					int_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			)
			

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z4fptrPFiiiE");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(pointer_qualifier_order) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr_order",
		NULL,
		1,
		
		(TypeIdentifier[]){
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
				0,
				&allocator
			),

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z9ptr_orderPrVPKi");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST


START_TEST(pointer_qualifier_substitution) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"ptr_order_subs",
		NULL,
		4,
		
		(TypeIdentifier[]){
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|VOLATILE,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z14ptr_order_subsPrVPKiPiPVS0_S2_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
	d=createMethodIdentifierData(
		"ptr_mangle",
		NULL,
		5,
		
		(TypeIdentifier[]){
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){0,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){RESTRICT,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|VOLATILE,0,END},
				0,
				&allocator
			),
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT,0,END},
				0,
				&allocator
			),

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z10ptr_manglePrVPKiPPiPrS3_PVS0_PS0_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(ignore_restrict_and_volatile_on_last) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"funcsub",
		NULL,
		1,
		
		(TypeIdentifier[]){
			createTypeId(
				int_string,
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,RESTRICT|VOLATILE,END},
				0,
				&allocator
			),

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPrVPKi");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(substitution_in_function_pointer) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"funcsub",
		NULL,
		2,
		
		(TypeIdentifier[]){
			createFunctionPtrTypeId(
				&int_identifier,
				2,
				(TypeIdentifier[]){
					int_ptr_identifier,
					int_ptr_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			),
			int_ptr_identifier

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFiPiS_ES_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(substitution_into_function_pointer) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"funcsub",
		NULL,
		2,
		
		(TypeIdentifier[]){
			int_ptr_identifier,
			createFunctionPtrTypeId(
				&int_identifier,
				2,
				(TypeIdentifier[]){
					int_ptr_identifier,
					int_ptr_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPiPFiS_S_E");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(substitution_of_function_pointer) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"funcsub",
		NULL,
		2,
		
		(TypeIdentifier[]){
			createFunctionPtrTypeId(
				&int_identifier,
				2,
				(TypeIdentifier[]){
					int_ptr_identifier,
					int_ptr_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			),
			createFunctionPtrTypeId(
				&int_identifier,
				2,
				(TypeIdentifier[]){
					int_ptr_identifier,
					int_ptr_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFiPiS_ES1_");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(special_method_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createSpecialMethodIdentifierData(
		COMPLETE_CONSTRUCTOR,
		(const char * []){
			"MyClass",
			NULL
		},
		2,
		(const TypeIdentifier []){
			int_identifier,
			int_identifier
		},
		&allocator

	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_ZN7MyClassC1Eii");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(substitution_in_return_type) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createMethodIdentifierData(
		"funcsub",
		NULL,
		1,
		
		(TypeIdentifier[]){
			createFunctionPtrTypeId(
				&int_ptr_identifier,
				2,
				(TypeIdentifier[]){
					int_ptr_identifier,
					int_ptr_identifier
				},
				NULL,
				(POINTER_QUALIFIER[]){0,END}, 
				0,
				&allocator
			)

		},
		&allocator
	);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFPiS_S_E");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST


START_TEST(std_edge_case) {
BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	uint32_t * thing;
	d=createGlobalIdentifierData("cout", (const char * []){
			"std"
			,NULL
		}, &allocator);
	mangled_name=mangle(&d,&allocator);
	ck_assert_str_eq(mangled_name, "_ZSt4cout");
	thing = bump_alloc(&allocator, 4);
	*thing = 0xcccccccc;
	ck_assert_mem_eq((allocator.buffer+allocator.index), "\xdd\xdd\xdd\xdd",4);
} END_TEST

START_TEST(demangle_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name="_Z3addiRPc2idFicE";
	d=demangle(mangled_name,&allocator);
	
	ck_assert_str_eq(d.member_id.id, "add");
	ck_assert_str_eq(d.method.member_argtypes[0].methodnt.name, "i");
	ck_assert_ptr_eq(d.method.member_argtypes[0].member_pointers,
		d.method.member_argtypes[0].member_pointers_end);
	ck_assert_str_eq(d.method.member_argtypes[1].methodnt.name, "c");
	ck_assert_int_eq(d.method.member_argtypes[1].member_ref, LVALUEREF);
	ck_assert_ptr_eq(d.method.member_argtypes[1].member_pointers+1,
		d.method.member_argtypes[1].member_pointers_end);
	

	ck_assert_str_eq(d.method.member_argtypes[2].methodnt.name, "id");
	ck_assert_str_eq(d.method.member_argtypes[3].method.member_argtypes->methodnt.name, "c");
	ck_assert_str_eq(d.method.member_argtypes[3].method.member_return_type->methodnt.name, "i");
	ck_assert_str_eq(mangle(&d,&allocator), mangled_name);
} END_TEST
START_TEST(demangle_nest_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name="_ZN7MyClass4funcEN11MyNamespace6MyTypeE";
	d=demangle(mangled_name,&allocator);
	ck_assert_str_eq(d.member_nests->id,"MyClass");
	ck_assert_int_eq(d.member_nest_count,1);
	ck_assert_str_eq(d.member_id.id,"func");
	ck_assert_str_eq(d.method.member_argtypes->methodnt.name,"MyType");
	ck_assert_str_eq(d.method.member_argtypes->member_aux_nests->id,"MyNamespace");

	ck_assert_int_eq(d.method.member_argtypes->member_aux_nest_count,1);
	ck_assert_str_eq(mangle(&d,&allocator), mangled_name);
} END_TEST

START_TEST(demangle_substitution_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name="_Z4funcPiS_";
	d=demangle(mangled_name,&allocator);
	ck_assert_str_eq(mangle(&d,&allocator), mangled_name);
} END_TEST

START_TEST(const_lvalue_ref_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name;
	d=createMethodIdentifierData(
		"const_a",
		NULL,
		2,
		
		(TypeIdentifier[]){
			
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){END},
				CONST_LVALUE_REF_BITMASK,
				&allocator
			),
			createTypeId(
				"i",
				NULL,
				(POINTER_QUALIFIER[]){CONSTANT,END},
				0,
				&allocator
			)

		},

		
		&allocator
	);

	mangled_name=mangle(&d, &allocator);
	ck_assert_str_eq(mangled_name, "_Z7const_aRKiPS_");
} END_TEST
int main(void){
	const TTest * all_mangle_tests[]={
		dont_mangle_unnested_global,
		nested_global,
		void_function,
		int_int_function,
		nest_function,
		named_argument_function,
		pointer_argument_function,
		fully_compressed_pointer_argument_function,
		partially_compressed_pointer_argument_function,
		compressed_pointer_argument_function_with_nest,
		complex_number_argument,
		constant_pointer_argument,
		ellipsis_argument,
		function_pointer_argument_no_subs,
		void_function_pointer_argument,
		all_subs_in_first_argument,
		pointer_qualifier_order,
		ignore_restrict_and_volatile_on_last,
		pointer_qualifier_substitution,
		substitution_in_function_pointer,
		substitution_of_function_pointer,
		substitution_into_function_pointer,
		special_method_test,
		substitution_in_return_type,
		std_edge_case,
		const_lvalue_ref_test,
		NULL
	};
	Suite *s;
	TCase * idfc;
	s=suite_create("MySuite");
	idfc=tcase_create("Mangling");
    for (const TTest ** test_ptr=all_mangle_tests; *test_ptr; test_ptr++){
		tcase_add_test(idfc, *test_ptr);
	}
	suite_add_tcase(s,idfc);

	idfc=tcase_create("Demangling");
	const TTest * all_demangle_tests[]={
		demangle_test,
		demangle_nest_test,
		demangle_substitution_test,
		NULL
	};
	for (const TTest ** test_ptr=all_demangle_tests; *test_ptr; test_ptr++){
		tcase_add_test(idfc, *test_ptr);
	}
	suite_add_tcase(s,idfc);
	//int number_failed;
    SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	return 0;
}