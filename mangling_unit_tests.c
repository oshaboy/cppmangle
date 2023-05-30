#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <check.h>
#include "cppmangle.h"
#include "cpptypes.h"




START_TEST(nested_global){
	const char * mangled_name=mangle("myglobal",1,NOT_METHOD, "Nester");
	ck_assert_str_eq(mangled_name, "_ZN6Nester8myglobalE");
} END_TEST
START_TEST(dont_mangle_unnested_global){
	const char * mangled_name=mangle("myglobal",0,NOT_METHOD);
	ck_assert_str_eq(mangled_name, "myglobal");
} END_TEST
START_TEST(void_function) {
	const char * mangled_name=mangle("void_func",0,0);
	ck_assert_str_eq(mangled_name, "_Z9void_funcv");
} END_TEST

START_TEST(int_int_function) {
	const char * mangled_name=mangle("add", 0,2,
		int_identifier,
		int_identifier
	);
	ck_assert_str_eq(mangled_name, "_Z3addii");
} END_TEST

START_TEST(nest_function) {
	const char * mangled_name=mangle(
		"nested",
		1,
		0,
		"Nester"
	);
	ck_assert_str_eq(mangled_name, "_ZN6Nester6nestedEv");

} END_TEST

START_TEST(named_argument_function) {
	const char * mangled_name=mangle(
		"named", 0,
		1,
		createTypeIdentifier(
			"MyName",
			(POINTER_QUALIFIER[]){END},
			IDENTIFIER_BITMASK,
			0
		)
	);
	ck_assert_str_eq(mangled_name, "_Z5named6MyName");

} END_TEST

START_TEST(pointer_argument_function) {
	const char * mangled_name=mangle(
		"ptr", 0,
		1,
		int_ptr_identifier
	);
	ck_assert_str_eq(mangled_name, "_Z3ptrPi");

} END_TEST


START_TEST(fully_compressed_pointer_argument_function) {
	const char * mangled_name=mangle(
		"ptr", 0,
		2,
		int_ptr_identifier,
		int_ptr_identifier
	);
	ck_assert_str_eq(mangled_name, "_Z3ptrPiS_");
} END_TEST


START_TEST(partially_compressed_pointer_argument_function) {
	const char * mangled_name=mangle(
		"ptr", 0,
		3,
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,END},
			0,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,END},
			0,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,0,END},
			0,
			0
		)
	);
	ck_assert_str_eq(mangled_name, "_Z3ptrPiPS_PS0_");
} END_TEST

START_TEST(all_subs_in_first_argument) {
	const char * mangled_name=mangle(
		"all_in_first", 0,
		3,
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,0,END},
			0,0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,END},
			0,0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,END},
			0,0
		)

	);
	ck_assert_str_eq(mangled_name, "_Z12all_in_firstPPPiS_S0_");
} END_TEST

START_TEST(compressed_pointer_argument_function_with_nest) {
	const char * mangled_name=mangle(
		"ptr", 1, 3,
		"Nester",
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,END},
			0,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,END},
			0,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,0,0,END},
			0,
			0
		)

	);
	ck_assert_str_eq(mangled_name, "_ZN6Nester3ptrEPiPS0_PS1_");
} END_TEST

START_TEST(complex_number_argument) {
	const char * mangled_name=mangle(
		"ccomplex",
		0,
		2,
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){0,END},
			COMPLEX_BITMASK,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){END},
			COMPLEX_BITMASK,
			0
		)
			
	);
	ck_assert_str_eq(mangled_name, "_Z8ccomplexPCiS_");
} END_TEST
START_TEST(constant_pointer_argument) {
	const char * mangled_name=mangle(
		"cpointer",
		0,
		1,
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){CONSTANT,END},
			0,
			0
		)
			

	);
	ck_assert_str_eq(mangled_name, "_Z8cpointerPKi");
} END_TEST

START_TEST(ellipsis_argument) {
	const char * mangled_name=mangle(
		"printf",
		0,
		2,
		char_ptr_identifier,
		ellipsis_identifier
	);
	ck_assert_str_eq(mangled_name, "_Z6printfPcz");
} END_TEST

START_TEST(void_function_pointer_argument) {
	const char * mangled_name=mangle(
		"voidfunc",
		0,
		1,
		
		createFunctionPtrTypeIdentifier(
			NULL,
			(POINTER_QUALIFIER[]){0,END}, 
			0,0,0
		)
			

	);
	ck_assert_str_eq(mangled_name, "_Z8voidfuncPFvvE");
} END_TEST

START_TEST(function_pointer_argument_no_subs) {
	const char * mangled_name=mangle(
		"fptr",
		0,
		1,
		
			createFunctionPtrTypeIdentifier(
				&int_identifier,
				(POINTER_QUALIFIER[]){0,END}, 
				0,0,2,
					int_identifier,
					int_identifier
			)
			

	);
	ck_assert_str_eq(mangled_name, "_Z4fptrPFiiiE");
} END_TEST

START_TEST(pointer_qualifier_order) {
	const char * mangled_name=mangle(
		"ptr_order",
		0,
		1,
	
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
			0,
			0
		)

		
	);
	ck_assert_str_eq(mangled_name, "_Z9ptr_orderPrVPKi");
} END_TEST


START_TEST(pointer_qualifier_substitution) {
	const char * mangled_name=mangle(
		"ptr_order_subs",
		0,
		4,
		
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
			0,
			0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){0,END},
			0,
			0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|VOLATILE,0,END},
			0,
			0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
			0,
			0
		)

	);
	ck_assert_str_eq(mangled_name, "_Z14ptr_order_subsPrVPKiPiPVS0_S2_");
	mangled_name=mangle(
		"ptr_mangle",
		0,
		5,
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,0,END},
			0,0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){0,0,END},
			0,0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){RESTRICT,0,END},
			0,0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|VOLATILE,0,END},
			0,0
		),
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT,0,END},
			0,
			0
		)
	);
	ck_assert_str_eq(mangled_name, "_Z10ptr_manglePrVPKiPPiPrS3_PVS0_PS0_");
} END_TEST

START_TEST(ignore_restrict_and_volatile_on_last) {
	const char * mangled_name=mangle(
		"funcsub",
		0,
		1,
		
		createTypeIdentifier(
			int_string,
			(POINTER_QUALIFIER[]){CONSTANT|RESTRICT|VOLATILE,RESTRICT|VOLATILE,END},
			0,
			0
		)

		
	);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPrVPKi");
} END_TEST

START_TEST(substitution_in_function_pointer) {
	const char * mangled_name=mangle(
		"funcsub",
		0,
		2,
		
		createFunctionPtrTypeIdentifier(
			&int_identifier,
			(POINTER_QUALIFIER[]){0,END}, 
			0,0,2,
				int_ptr_identifier,
				int_ptr_identifier
		),
		int_ptr_identifier

		
	);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFiPiS_ES_");
} END_TEST

START_TEST(substitution_into_function_pointer) {
	const char * mangled_name=mangle(
		"funcsub",
		0,
		2,
		

		int_ptr_identifier,
		createFunctionPtrTypeIdentifier(
			&int_identifier,
			(POINTER_QUALIFIER[]){0,END}, 
			0,0,2,
				int_ptr_identifier,
				int_ptr_identifier
		)

	);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPiPFiS_S_E");
} END_TEST

START_TEST(substitution_of_function_pointer) {
	const char * mangled_name=mangle(
		"funcsub",
		0,
		2,
		createFunctionPtrTypeIdentifier(
			&int_identifier,
			(POINTER_QUALIFIER[]){0,END}, 
			0,0,2,
				int_ptr_identifier,
				int_ptr_identifier
		),
		createFunctionPtrTypeIdentifier(
			&int_identifier,
			(POINTER_QUALIFIER[]){0,END},
			0,0,2,
				int_ptr_identifier,
				int_ptr_identifier
		)

		
	);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFiPiS_ES1_");
} END_TEST

START_TEST(special_method_test) {
	const char * mangled_name;
	mangled_name=mangleSpecialMethod(
		COMPLETE_CONSTRUCTOR,
		1,2,
		"MyClass",
		int_identifier,
		int_identifier

	);
	ck_assert_str_eq(mangled_name, "_ZN7MyClassC1Eii");
} END_TEST

START_TEST(substitution_in_return_type) {
	const char * mangled_name=mangle(
		"funcsub",
		0,
		1,
		
		createFunctionPtrTypeIdentifier(
			&int_ptr_identifier,
			(POINTER_QUALIFIER[]){0,END}, 
			0,0,2,
				int_ptr_identifier,
				int_ptr_identifier
		)

	);
	ck_assert_str_eq(mangled_name, "_Z7funcsubPFPiS_S_E");
} END_TEST


START_TEST(std_edge_case) {
	const char * mangled_name=mangle("cout", 1,NOT_METHOD, "std");
	ck_assert_str_eq(mangled_name, "_ZSt4cout");
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
	ck_assert_str_eq(mangleIdentifierData(&d,&allocator), mangled_name);
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
	ck_assert_str_eq(mangleIdentifierData(&d,&allocator), mangled_name);
} END_TEST

START_TEST(demangle_substitution_test) {
	BumpAllocator allocator={0};
	IdentifierData d;
	const char * mangled_name="_Z4funcPiS_";
	d=demangle(mangled_name,&allocator);
	ck_assert_str_eq(mangleIdentifierData(&d,&allocator), mangled_name);
} END_TEST

START_TEST(const_lvalue_ref_test) {
	const char * mangled_name=mangle(
		"const_a",
		0,
		2,
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){END},
			CONSTANT_REF_BITMASK|LVALUE_REF_BITMASK,
			0
		),
		createTypeIdentifier(
			"i",
			(POINTER_QUALIFIER[]){CONSTANT,END},
			0,
			0
		)


	);
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