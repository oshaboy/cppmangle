#include "cppmangle.h"
#include "cpptypes.h"
#include <stddef.h>

static const POINTER_QUALIFIER qualifierless_pointers[4]={0,0,0,END};
#define qualifierless_pointers_end (qualifierless_pointers+3)
#define SIZEOF(STR) (sizeof(STR)-1)/sizeof((STR)[0])
#define TYPE_IDENTIFIER(TYPE, STR) \
const char * TYPE ## _string = STR; \
const TypeIdentifier TYPE ## _identifier = { \
	.methodnt.name=STR, \
	.methodnt.name_len=SIZEOF(STR), \
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, \
	.member_pointers=qualifierless_pointers_end, \
	.member_pointers_end=qualifierless_pointers_end, \
	.member_auxdata_len=0, \
	.member_ref=VALUE, \
	.type_length=SIZEOF(STR), \
	.member_can_have_substitution=0 \
}; \
const TypeIdentifier TYPE ## _ptr_identifier = { \
	.methodnt.name=STR, \
	.methodnt.name_len=SIZEOF(STR), \
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, \
	.member_pointers=qualifierless_pointers_end-1, \
	.member_pointers_end=qualifierless_pointers_end, \
	.member_auxdata_len=1, \
	.member_ref=VALUE, \
	.type_length=SIZEOF(STR)+1, \
	.member_can_have_substitution=1 \
}; \
const TypeIdentifier TYPE ## _lvalue_ref_identifier = { \
	.methodnt.name=STR, \
	.methodnt.name_len=SIZEOF(STR), \
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, \
	.member_pointers=qualifierless_pointers_end, \
	.member_pointers_end=qualifierless_pointers_end, \
	.member_auxdata_len=1, \
	.member_ref=LVALUEREF, \
	.type_length=SIZEOF(STR)+1, \
	.member_can_have_substitution=1 \
};
TYPE_IDENTIFIER(bool, "b")
TYPE_IDENTIFIER(char, "c")
TYPE_IDENTIFIER(signed_char, "a")
TYPE_IDENTIFIER(unsigned_char, "h")
TYPE_IDENTIFIER(short, "s")
TYPE_IDENTIFIER(unsigned_short, "t")
TYPE_IDENTIFIER(int	, "i")
TYPE_IDENTIFIER(unsigned_int, "j")
TYPE_IDENTIFIER(long, "l")
TYPE_IDENTIFIER(unsigned_long, "m")
TYPE_IDENTIFIER(long_long, "x")
TYPE_IDENTIFIER(unsigned_long_long, "y")
TYPE_IDENTIFIER(int128, "n")
TYPE_IDENTIFIER(unsigned_int128, "o")
TYPE_IDENTIFIER(float, "f")
TYPE_IDENTIFIER(double, "d")
TYPE_IDENTIFIER(long_double, "e")
TYPE_IDENTIFIER(float16, "Dh")
TYPE_IDENTIFIER(float128, "g")
TYPE_IDENTIFIER(bfloat16, "DF16b")
TYPE_IDENTIFIER(decimal32, "Df")
TYPE_IDENTIFIER(decimal64, "Dd")
TYPE_IDENTIFIER(decimal128, "De")
const TypeIdentifier void_ptr_identifier = { 
	.methodnt.name="v", 
	.methodnt.name_len=1, 
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, 
	.member_pointers=qualifierless_pointers_end-1, 
	.member_pointers_end=qualifierless_pointers_end, 
	.member_auxdata_len=1, 
	.member_ref=VALUE, 
	.type_length=2, 
	.member_can_have_substitution=0 
};
const TypeIdentifier void_ptr_ptr_identifier = { 
	.methodnt.name="v", 
	.methodnt.name_len=1, 
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, 
	.member_pointers=qualifierless_pointers_end-2, 
	.member_pointers_end=qualifierless_pointers_end, 
	.member_auxdata_len=2, 
	.member_ref=VALUE, 
	.type_length=3, 
	.member_can_have_substitution=1 
};
const TypeIdentifier void_ptr_lvalue_ref_identifier = { 
	.methodnt.name="v", 
	.methodnt.name_len=1, 
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ \
	.methodnt.isidentifier=0, 
	.member_pointers=qualifierless_pointers_end-1, 
	.member_pointers_end=qualifierless_pointers_end, 
	.member_auxdata_len=0, 
	.member_ref=LVALUEREF, 
	.type_length=3, 
	.member_can_have_substitution=1 
};
TYPE_IDENTIFIER(std_nullptr_t, "Dn")
TYPE_IDENTIFIER(wchar_t, "w")
TYPE_IDENTIFIER(char8_t, "Du")
TYPE_IDENTIFIER(char16_t, "Ds")
TYPE_IDENTIFIER(char32_t, "Di")
const char * ellipsis_string = "z";
const TypeIdentifier ellipsis_identifier = { 
	.methodnt.name="z", 
	.methodnt.name_len=1, 
	.methodnt.name_len_len=0, /*If it's primitive it doesn't matter anyway */ 
	.methodnt.isidentifier=0, 
	.member_ref=VALUE, 
	.member_pointers=qualifierless_pointers_end, 
	.member_pointers_end=qualifierless_pointers_end, 
	.member_auxdata_len=0, 
	.type_length=1,
	.member_can_have_substitution=0 
};

const LenId std_namespace_len_id = {
	.full_id="St",
	.full_id_len=2,
	.id="St",
	.id_len=2
};
TYPE_IDENTIFIER(std_allocator, "Sa")
TYPE_IDENTIFIER(std_basicstring, "Sb")
TYPE_IDENTIFIER(std_iostream, "Sd")
TYPE_IDENTIFIER(std_string, "Ss")
TYPE_IDENTIFIER(std_istream, "Si")
TYPE_IDENTIFIER(std_ostream, "So")