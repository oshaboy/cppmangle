#ifndef CPP_TYPES_H
#define CPP_TYPES_H
#include "cppmangle.h"

#define TYPE_IDENTIFIER_DECL(TYPE) \
	const extern char * TYPE ## _string; \
	const extern TypeIdentifier TYPE ## _identifier; \
	const extern TypeIdentifier TYPE ## _ptr_identifier; \
	const extern TypeIdentifier TYPE ## _const_ptr_identifier; \
	const extern TypeIdentifier TYPE ## _lvalue_ref_identifier; \
	const extern TypeIdentifier TYPE ## _const_lvalue_ref_identifier; \
	const extern TypeIdentifier TYPE ## _rvalue_ref_identifier;

#define REF *
#define REF_REF *

TYPE_IDENTIFIER_DECL(bool)
TYPE_IDENTIFIER_DECL(char)
TYPE_IDENTIFIER_DECL(signed_char)
TYPE_IDENTIFIER_DECL(unsigned_char)
TYPE_IDENTIFIER_DECL(short)
TYPE_IDENTIFIER_DECL(unsigned_short)
TYPE_IDENTIFIER_DECL(int)
TYPE_IDENTIFIER_DECL(unsigned_int)
TYPE_IDENTIFIER_DECL(long)
TYPE_IDENTIFIER_DECL(unsigned_long)
TYPE_IDENTIFIER_DECL(long_long)
TYPE_IDENTIFIER_DECL(unsigned_long_long)
TYPE_IDENTIFIER_DECL(int128)
TYPE_IDENTIFIER_DECL(unsigned_int128)
TYPE_IDENTIFIER_DECL(float)
TYPE_IDENTIFIER_DECL(double)
TYPE_IDENTIFIER_DECL(long_double)
TYPE_IDENTIFIER_DECL(float16)
TYPE_IDENTIFIER_DECL(float128)
TYPE_IDENTIFIER_DECL(bfloat16)
TYPE_IDENTIFIER_DECL(decimal32)
TYPE_IDENTIFIER_DECL(decimal64)
TYPE_IDENTIFIER_DECL(decimal128)
TYPE_IDENTIFIER_DECL(void_ptr)
TYPE_IDENTIFIER_DECL(std_nullptr_t)
TYPE_IDENTIFIER_DECL(wchar_t)
TYPE_IDENTIFIER_DECL(char8_t)
TYPE_IDENTIFIER_DECL(char16_t)
TYPE_IDENTIFIER_DECL(char32_t)
extern const char * ellipsis_string;
extern const TypeIdentifier ellipsis_identifier;
TYPE_IDENTIFIER_DECL(std_allocator)
TYPE_IDENTIFIER_DECL(std_basicstring)
TYPE_IDENTIFIER_DECL(std_iostream)
TYPE_IDENTIFIER_DECL(std_string)
TYPE_IDENTIFIER_DECL(std_istream)
TYPE_IDENTIFIER_DECL(std_ostream)

#define OPAQUE_TYPE_DECL(_name, _size) typedef struct {char _[_size];} _name;
OPAQUE_TYPE_DECL(std_string_64, 32)
OPAQUE_TYPE_DECL(compat_std_string_64, 8)
OPAQUE_TYPE_DECL(std_istream_64, 280)
OPAQUE_TYPE_DECL(std_ostream_64, 272)
OPAQUE_TYPE_DECL(std_iostream_64, 288)
OPAQUE_TYPE_DECL(std_vector_64, 24)

OPAQUE_TYPE_DECL(std_string_32, 24)
OPAQUE_TYPE_DECL(compat_std_string_32, 4)
OPAQUE_TYPE_DECL(std_istream_32, 144)
OPAQUE_TYPE_DECL(std_ostream_32, 140)
OPAQUE_TYPE_DECL(std_iostream_32, 148)
OPAQUE_TYPE_DECL(std_vector_32, 12)
OPAQUE_TYPE_DECL(std_allocator, 1)
extern std_allocator default_allocator;
#if 0
std_string_64 _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_(const char *, const std_allocator REF);
#define CtoCPPString _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2IS3_EEPKcRKS3_
#endif 
#endif