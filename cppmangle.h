#ifndef CPP_MANGLE_H
#define CPP_MANGLE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "allocator.h"
#ifdef __cplusplus
typedef bool CBool;
#else
typedef _Bool CBool;
#endif
/* Easily deal with the {length}{identifier} format that Itanium loves so much*/
typedef struct {
	const char * full_id;
	size_t full_id_len;
	const char * id;
	size_t id_len;
} LenId;
/* What kind of REF the type is */
typedef enum {
	VALUE=0,
	LVALUEREF,
	RVALUEREF
} REF;
/* Is the arithemtic type real or one of the ccomplex types? */
typedef enum {
	REAL,
	COMPLEX,
	IMAGINARY
} COMPLEXITY;

/*Every pointer can have any of 3 qualifiers*/
typedef enum __attribute__((packed)){
	CONSTANT=1,
	VOLATILE=2,
	RESTRICT=4,
	NO_P=0x80, //For Internal use
	END=0xff
} POINTER_QUALIFIER;

/* Big ol enum of the "special" methods which includes all the constructors, destructors and operators
	These need to be handled seperatly because they aren't prefixed with a length.
*/
typedef enum {
	COMPLETE_CONSTRUCTOR,
	BASE_CONSTRUCTOR,
	COMPLETE_ALLOCATING_CONSTRUCTOR,
	COMPLETE_INHERITING_CONSTRUCTOR,
	BASE_INHERITING_CONSTRUCTOR,
	DELETING_DESTRUCTOR,
	COMPLETE_DESTRUCTOR,
	BASE_DESTRUCTOR,
	NEW_OPERATOR,
	NEW_ARRAY_OPERATOR,
	DELETE_OPERATOR,
	DELETE_ARRAY_OPERATOR,
	CO_AWAIT_OPERATOR,
	UNARY_PLUS_OPERATOR,
	UNARY_MINUS_OPERATOR,
	REFERENCE_OPERATOR,
	DEREFERENCE_OPERATOR,
	BITWISE_COMPLEMENT_OPERATOR,
	BINARY_PLUS_OPERATOR,
	BINARY_MINUS_OPERATOR,
	TIMES_OPERATOR,
	DIVIDE_OPERATOR,
	REMAINDER_OPERATOR,
	BITWISE_AND_OPERATOR,
	BITWISE_OR_OPERATOR,
	BITWISE_XOR_OPERATOR,
	ASSIGN_OPERATOR,
	PLUS_ASSIGN_OPERATOR,
	MINUS_ASSIGN_OPERATOR,
	TIMES_ASSIGN_OPERATOR,
	DIVIDE_ASSIGN_OPERATOR,
	REMAINDER_ASSIGN_OPERATOR,
	BITWISE_AND_ASSIGN_OPERATOR,
	BITWISE_OR_ASSIGN_OPERATOR,
	BITWISE_XOR_ASSIGN_OPERATOR,
	LEFT_SHIFT_OPERATOR,
	RIGHT_SHIFT_OPERATOR,
	LEFT_SHIFT_ASSIGN_OPERATOR,
	RIGHT_SHIFT_ASSIGN_OPERATOR,
	EQUALS_EQUALS_OPERATOR,
	NOT_EQUALS_OPERATOR,
	LESS_THAN_OPERATOR,
	GREATER_THAN_OPERATOR,
	LESS_EQUALS_OPERATOR,
	GREATER_EQUALS_OPERATOR,
	SPACESHIP_OPERATOR,
	BOOLEAN_NOT_OPERATOR,
	BOOLEAN_AND_OPERATOR,
	BOOLEAN_OR_OPERATOR,
	INCREMENT_OPERATOR,
	DECREMENT_OPERATOR,
	COMMA_OPERATOR,
	ARROW_STAR_OPERATOR,
	ARROW_OPERATOR,
	CALL_OPERATOR,
	ARRAY_ACCESS_OPERATOR
} SPECIAL_METHOD;
/*defined in mangle.c, LenId structs for all the special methods*/
const extern LenId special_method_identifiers[];

typedef struct Substitution Substitution;
typedef struct TypeIdentifier TypeIdentifier;

/*All the extra data a type can have. also used in TypeIdentifierDiff*/
struct AuxilliaryTypeData_{
	/*all pointer qualifiers, pointer_end is a single cell after the last qualifier*/
	const POINTER_QUALIFIER * _pointers;
	const POINTER_QUALIFIER * _pointers_end; 
	size_t _aux_nest_count; //Types can be in namespaces too. 
	const LenId * _aux_nests; 
	size_t _len; //length of the entire thing in a mangled name. 
	REF _ref ;
	COMPLEXITY _complexity;
	CBool _ismethodtype;
	CBool _can_have_substitution;
};

/*IdentifierData is pretty self explanatory. It's the whole NblablaE bit*/
struct IdentifierData_{
	LenId _id;
	size_t _nest_count;
	const LenId * _nests;
};

typedef union {
	struct AuxilliaryTypeData_ d;
	struct {struct AuxilliaryTypeData_ d;} auxdata;
} AuxilliaryTypeData;



/*
Data of a Method, used in function pointers as well as MethodIdentifierData.
return_type is NULL for MethodIdentifierData because the return type isn't mangled. 
*/
struct MethodData_{
	size_t _arg_count;
	TypeIdentifier * _argtypes;
	const TypeIdentifier * _return_type;
};
typedef union {
	struct MethodData_ md;
	struct {struct MethodData_ md;} method_data;
} MethodData;
/*
Identifies a Type

*/
struct TypeIdentifier {
	AuxilliaryTypeData auxdata;
	union {
		struct {
			/*
			identifiers are classes, structures, unions, anything that isn't a primitive. 
			Doesn't apply to typedefs. 
			*/
			const char * name;
			size_t name_len; 
			size_t name_len_len;
			CBool isidentifier;
		} methodnt; //Do you have a better name?
		struct {
			MethodData method_data;
		} method;
	};
	size_t type_length; //the length of the type when mangled without substitutions.
};
/* Bunch of structures to deal with the recursive nightmare that is Substitutions*/
struct Substitution{
	TypeIdentifier from;
	const char * to;
	size_t to_len;
};

typedef struct ArgSubstitutionTree{
	size_t argnum;
	Substitution substitution;
	size_t argument_sub_num_for_func_ptr;
	struct ArgSubstitutionTree * subs_of_func_ptr;
} ArgSubstitutionTree;

typedef struct {
	size_t nest_sub_num;
	Substitution * nest_subtitutions;
	size_t argument_sub_num;
	ArgSubstitutionTree * argument_substitutions;
	size_t max_nests;
} SubstitutionDataStructure;

/*Data of an identifier to mangle*/
typedef struct {
	CBool ismethod;
	CBool is_special_method;
	union {
		struct IdentifierData_ d;
		struct {
			struct IdentifierData_ d;
		} id_data;
	};
	struct {
		SubstitutionDataStructure substitution_ds;
		MethodData method_data;
	} method;
} IdentifierData;
typedef IdentifierData MethodIdentifierData;

/*
Convenience macros to access the data of the structs regardless
of which one it is. 
*/
#define member_ref auxdata.d._ref
#define member_pointers auxdata.d._pointers
#define member_pointers_end auxdata.d._pointers_end
#define member_auxdata_len auxdata.d._len
#define member_complexity auxdata.d._complexity
#define member_ismethodtype auxdata.d._ismethodtype
#define member_aux_nests auxdata.d._aux_nests
#define member_aux_nest_count auxdata.d._aux_nest_count
#define member_can_have_substitution auxdata.d._can_have_substitution
#define member_arg_count method_data.md._arg_count
#define member_argtypes method_data.md._argtypes
#define member_return_type method_data.md._return_type
#define member_allocator method_data.md._allocator
#define member_id id_data.d._id
#define member_nest_count id_data.d._nest_count
#define member_nests id_data.d._nests

/* mangles an IdentifierData structure */
const char * mangle(const IdentifierData * d, BumpAllocator * alloc);
/*create IdentifierData structure for a global variable*/
const IdentifierData createGlobalIdentifierData(const char * id, const char *const* nests, BumpAllocator * alloc);
/*creates IdentifierData structure for methodData*/
const MethodIdentifierData createMethodIdentifierData(
	const char * id, const char ** nests, const size_t argtype_n,
	const TypeIdentifier * args, BumpAllocator * alloc);
/*Mangles a Type without substitutions*/
const char * mangleType(const TypeIdentifier * ti, char * buf);
/*Creates a Non Function Pointer Type*/
const TypeIdentifier createTypeId(
	const char * base, const char * const* nests, const POINTER_QUALIFIER * ptrs, unsigned long flags, BumpAllocator * alloc);
/*Creates a Function Pointer Type*/
const TypeIdentifier createFunctionPtrTypeId(
	const TypeIdentifier * returns, size_t arg_n, const TypeIdentifier * args, const char *const * nests,
	const POINTER_QUALIFIER * ptrs, unsigned long flags, BumpAllocator * alloc);
/*Creates a special method (operator/constructor/destructor)*/
const MethodIdentifierData createSpecialMethodIdentifierData(SPECIAL_METHOD tag, const char *const* nests, const size_t argtype_n, const TypeIdentifier * args,BumpAllocator * alloc);

/*Bitmasks for flags*/
#define LVALUE_REF_BITMASK 1
#define RVALUE_REF_BITMASK 2
#define IDENTIFIER_BITMASK 4
#define COMPLEX_BITMASK 8
#define IMAGINARY_BITMASK 16


#include "cpptypes.h"



#ifdef __cplusplus
}
#endif

#endif