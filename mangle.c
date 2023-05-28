#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cppmangle.h"

const char * mangleTypeWithSubstitution(
	const SubstitutionDataStructure * subs,
	const TypeIdentifier * ti,
	char * buf,
	size_t ** argnum_ptr,
	size_t nest_count,
	size_t * argnum_buf
);
void setSubstitutions(MethodIdentifierData * d, BumpAllocator * alloc);
size_t getSubArrLen(const MethodData * d);

#define SPECIAL_METHOD(STR) \
	(LenId){ \
	.full_id = STR, \
	.full_id_len = (sizeof(STR)-1)/sizeof((STR)[0]), \
	.id = STR, \
	.id_len = (sizeof(STR)-1)/sizeof((STR)[0]) \
}
const LenId special_method_identifiers[] ={
	[COMPLETE_CONSTRUCTOR]=SPECIAL_METHOD("C1"),
	[BASE_CONSTRUCTOR]=SPECIAL_METHOD("C2"),
	[COMPLETE_ALLOCATING_CONSTRUCTOR]=SPECIAL_METHOD("C3"),
	[COMPLETE_INHERITING_CONSTRUCTOR]=SPECIAL_METHOD("CI1"),
	[BASE_INHERITING_CONSTRUCTOR]=SPECIAL_METHOD("CI2"),
	[DELETING_DESTRUCTOR]=SPECIAL_METHOD("D0"),
	[COMPLETE_DESTRUCTOR]=SPECIAL_METHOD("D1"),
	[BASE_DESTRUCTOR]=SPECIAL_METHOD("D2"),
	[NEW_OPERATOR]=SPECIAL_METHOD("nw"),
	[NEW_ARRAY_OPERATOR]=SPECIAL_METHOD("na"),
	[DELETE_OPERATOR]=SPECIAL_METHOD("dl"),
	[DELETE_ARRAY_OPERATOR]=SPECIAL_METHOD("da"),
	[CO_AWAIT_OPERATOR]=SPECIAL_METHOD("aw"),
	[UNARY_PLUS_OPERATOR]=SPECIAL_METHOD("ps"),
	[UNARY_MINUS_OPERATOR]=SPECIAL_METHOD("ng"),
	[REFERENCE_OPERATOR]=SPECIAL_METHOD("ad"),
	[DEREFERENCE_OPERATOR]=SPECIAL_METHOD("de"),
	[BITWISE_COMPLEMENT_OPERATOR]=SPECIAL_METHOD("co"),
	[BINARY_PLUS_OPERATOR]=SPECIAL_METHOD("pl"),
	[BINARY_MINUS_OPERATOR]=SPECIAL_METHOD("mi"),
	[TIMES_OPERATOR]=SPECIAL_METHOD("ml"),
	[DIVIDE_OPERATOR]=SPECIAL_METHOD("dv"),
	[REMAINDER_OPERATOR]=SPECIAL_METHOD("rm"),
	[BITWISE_AND_OPERATOR]=SPECIAL_METHOD("an"),
	[BITWISE_OR_OPERATOR]=SPECIAL_METHOD("or"),
	[BITWISE_XOR_OPERATOR]=SPECIAL_METHOD("eo"),
	[ASSIGN_OPERATOR]=SPECIAL_METHOD("aS"),
	[PLUS_ASSIGN_OPERATOR]=SPECIAL_METHOD("pL"),
	[MINUS_ASSIGN_OPERATOR]=SPECIAL_METHOD("mI"),
	[TIMES_ASSIGN_OPERATOR]=SPECIAL_METHOD("mL"),
	[DIVIDE_ASSIGN_OPERATOR]=SPECIAL_METHOD("dV"),
	[REMAINDER_ASSIGN_OPERATOR]=SPECIAL_METHOD("rM"),
	[BITWISE_AND_ASSIGN_OPERATOR]=SPECIAL_METHOD("aN"),
	[BITWISE_OR_ASSIGN_OPERATOR]=SPECIAL_METHOD("oR"),
	[BITWISE_XOR_ASSIGN_OPERATOR]=SPECIAL_METHOD("eO"),
	[LEFT_SHIFT_OPERATOR]=SPECIAL_METHOD("ls"),
	[RIGHT_SHIFT_OPERATOR]=SPECIAL_METHOD("rs"),
	[LEFT_SHIFT_ASSIGN_OPERATOR]=SPECIAL_METHOD("lS"),
	[RIGHT_SHIFT_ASSIGN_OPERATOR]=SPECIAL_METHOD("rS"),
	[EQUALS_EQUALS_OPERATOR]=SPECIAL_METHOD("eq"),
	[NOT_EQUALS_OPERATOR]=SPECIAL_METHOD("ne"),
	[LESS_THAN_OPERATOR]=SPECIAL_METHOD("lt"),
	[GREATER_THAN_OPERATOR]=SPECIAL_METHOD("rt"),
	[LESS_EQUALS_OPERATOR]=SPECIAL_METHOD("le"),
	[GREATER_EQUALS_OPERATOR]=SPECIAL_METHOD("ge"),
	[SPACESHIP_OPERATOR]=SPECIAL_METHOD("ss"),
	[BOOLEAN_NOT_OPERATOR]=SPECIAL_METHOD("nt"),
	[BOOLEAN_AND_OPERATOR]=SPECIAL_METHOD("aa"),
	[BOOLEAN_OR_OPERATOR]=SPECIAL_METHOD("oo"),
	[INCREMENT_OPERATOR]=SPECIAL_METHOD("pp"),
	[DECREMENT_OPERATOR]=SPECIAL_METHOD("mm"),
	[COMMA_OPERATOR]=SPECIAL_METHOD("cm"),
	[ARROW_MEMBER_OPERATOR]=SPECIAL_METHOD("pm"),
	[ARROW_OPERATOR]=SPECIAL_METHOD("pt"),
	[CALL_OPERATOR]=SPECIAL_METHOD("cl"),
	[ARRAY_ACCESS_OPERATOR]=SPECIAL_METHOD("ix")
};
/*Converts a String to a LenId data structure*/
const LenId toLenId(const char * const id, BumpAllocator * alloc){
	LenId result = {.id_len=strlen(id)};
	char len_str_buf[20];
	sprintf(len_str_buf, "%zu", result.id_len);
	result.full_id_len = result.id_len + strlen(len_str_buf);
	char * full_id;
	result.full_id=full_id=bump_alloc(alloc,result.full_id_len+1);
	strcpy(full_id, len_str_buf);
	strcat(full_id, id);
	result.id=result.full_id+result.full_id_len-result.id_len;
	return result;
}

void set_auxdata_length(AuxilliaryTypeData * aux){
	size_t pointer_char_cnt=0;
	for (const POINTER_QUALIFIER * pointer_ptr=aux->member_pointers;
		pointer_ptr<aux->member_pointers_end; pointer_ptr++){
			/*add 1 for every bit set except NO_P should only be added if reset*/
			pointer_char_cnt+=__builtin_popcount(*pointer_ptr ^ NO_P); 
	}

	aux->member_auxdata_len= pointer_char_cnt+__builtin_popcount(aux->member_ref)+(aux->member_complexity?1:0);
}

/*Returns the length of ti WITHOUT SUBSTITUTIONS*/
size_t calculate_type_length(const TypeIdentifier * ti){
	if (ti->member_ismethodtype){
		size_t result = 2+ti->member_auxdata_len; //Functions are wrapped in F and E
		if (ti->method.member_return_type){
			result+=ti->method.member_return_type->type_length; 
		} else {
			result+=1; //for v
		}
		if (ti->method.member_arg_count == 0){
			result+=1; //another v
		} else {
			for (TypeIdentifier * arg_ti = ti->method.member_argtypes;
				arg_ti<ti->method.member_arg_count+ti->method.member_argtypes;
				arg_ti++
			){
				result+=arg_ti->type_length;

			}
		}
		return result;


	} else {
		size_t result = ti->methodnt.name_len+ti->member_auxdata_len;
		if (ti->methodnt.isidentifier){
			result+=ti->methodnt.name_len_len;
		} 
		return result;
	}
}

void setLengths(TypeIdentifier * ti){
	set_auxdata_length(&ti->auxdata);
	ti->type_length=calculate_type_length(ti);
}

static size_t calculate_mangled_length(const IdentifierData * d){
	if (d->ismethod || d->member_nest_count > 0){
		size_t result = 2+d->member_id.full_id_len;
		if (d->member_nest_count>0) {
			result+=2; //Nests are wrapped in N and E
			for (const LenId * ptr=d->member_nests; ptr<d->member_nests+d->member_nest_count; ptr++)
				result+=ptr->full_id_len;
			
		}
		if (d->ismethod){
			if (d->method.member_arg_count>0){
				for (const TypeIdentifier *ptr=d->method.member_argtypes; ptr<d->method.member_argtypes+d->method.member_arg_count; ptr++){
					/* 
					 * Substitutions can make the type one character longer
					 * (Which kind of defeats the point) so I have to compensate 
					 */
					result+=ptr->type_length+1;
				}
				
			} else 
				result+=1; //for v
			
		}
		return result;
	} else {
		return d->member_id.full_id_len;
	}


}







char * mangleAuxData(const AuxilliaryTypeData * aux, char * buf){
	static const char * refchars[]={
		[VALUE]="",
		[LVALUEREF]="R",
		[CONSTANT_REF|LVALUEREF]="RK",
		[VOLATILE_REF|LVALUEREF]="VR",
		[CONSTANT_REF|VOLATILE_REF|LVALUEREF]="VRK",
		[RVALUEREF]="O",
		[CONSTANT_REF|RVALUEREF]="OK",
		[VOLATILE_REF|RVALUEREF]="VO",
		[CONSTANT_REF|VOLATILE_REF|RVALUEREF]="VOK",
	};
	static const char * complexchars[]={
		[REAL]="",
		[COMPLEX] ="C",
		[IMAGINARY]="G"
	};
	char Ps[aux->member_auxdata_len+1];
	char * Ps_ptr=Ps+aux->member_auxdata_len;
	*(Ps_ptr--)='\0';
	for (
		const POINTER_QUALIFIER * pointer_ptr= aux->member_pointers;
		pointer_ptr<aux->member_pointers_end; pointer_ptr++
	){
		if (*pointer_ptr&CONSTANT) *(Ps_ptr--)='K';
		if (!(*pointer_ptr&NO_P))  *(Ps_ptr--)='P';
		if (*pointer_ptr&VOLATILE) *(Ps_ptr--)='V';
		if (*pointer_ptr&RESTRICT) *(Ps_ptr--)='r';
	}
	sprintf(buf, "%s%s%s",refchars[aux->member_ref], Ps_ptr+1, complexchars[aux->member_complexity]);
	return buf;
}

extern const LenId std_namespace_len_id;
/*Mangles a type without substitutions*/
const char * mangleType(const TypeIdentifier * ti, char * buf){
	char buf2[ti->member_auxdata_len+1];
	const char * mangled_aux_data=mangleAuxData(&ti->auxdata, buf2);
	char * bufptr=buf;
	strcpy(bufptr, mangled_aux_data); bufptr+=ti->member_auxdata_len;
	const bool isstd=(ti->member_aux_nest_count==1 && (ti->member_aux_nests[0].full_id==std_namespace_len_id.full_id));
	if(ti->member_aux_nest_count>0){
		if (!isstd) *(bufptr++)='N';
		for (const LenId * ptr=ti->member_aux_nests; ptr<ti->member_aux_nests+ti->member_aux_nest_count; ptr++){
			strcpy(bufptr,ptr->full_id); bufptr+=ptr->full_id_len;
		}
	}
	if (ti->member_ismethodtype){
		*(bufptr++)='F';
		if (ti->method.member_return_type){
			mangleType(ti->method.member_return_type, bufptr); bufptr+=ti->method.member_return_type->type_length;
		} else 
			*(bufptr++)='v';
		if (ti->method.member_arg_count){
			for (TypeIdentifier * arg_ti = ti->method.member_argtypes; arg_ti<ti->method.member_argtypes+ti->method.member_arg_count; arg_ti++){
				mangleType(arg_ti, bufptr); bufptr+=arg_ti->type_length;
			}
		} else 
			*(bufptr++)='v';
		
		if (!isstd) strcpy(bufptr++, "E");
	} else if (ti->methodnt.isidentifier){
		sprintf(bufptr,"%zu", ti->methodnt.name_len); bufptr+=ti->methodnt.name_len_len;
		strcpy(bufptr, ti->methodnt.name); bufptr+=ti->methodnt.name_len;
	} else 
		strcpy(bufptr, ti->methodnt.name);
	if(ti->member_aux_nest_count>0 && !isstd){
		strcpy(bufptr++,"E");
	}
	return buf;
	
}

static void mangleIdentifierAndNests(const IdentifierData * d, char ** result){

	if(d->member_nest_count>0){
		const bool isstd=(d->member_nest_count==1 && (d->member_nests[0].full_id==std_namespace_len_id.full_id));
		if (!isstd) *((*result)++) ='N';
		for (const LenId * ptr=d->member_nests; ptr<d->member_nests+d->member_nest_count; ptr++){
			strcpy(*result,ptr->full_id); (*result)+=ptr->full_id_len;
		}
		if (d->ismethod &&d->is_special_method){
			strcpy (*result, d->member_id.id); *result+=d->member_id.id_len;
		} else {
			strcpy (*result, d->member_id.full_id); *result+=d->member_id.full_id_len;
		}
		if (!isstd)  strcpy ((*result)++, "E"); 
	} else {
		strcpy (*result, d->member_id.full_id);*result+=d->member_id.full_id_len;
	}
}
const size_t RETURN = (size_t)-1;
/*Mangle Identifier*/
const char * mangleIdentifierData(const IdentifierData * d, BumpAllocator * alloc){
	const size_t mangled_id_length = calculate_mangled_length(d);
	char *const result = bump_alloc(alloc,mangled_id_length+1);
	if (d->ismethod){
		char *result_ptr = result;
		strcpy (result_ptr, "_Z"); result_ptr+=2;
		mangleIdentifierAndNests(d,&result_ptr);
		if (d->method.member_argtypes==0){
			strcpy(result_ptr++, "v"); 
		} else {
			for (const TypeIdentifier * ptr=d->method.member_argtypes; ptr<d->method.member_argtypes+d->method.member_arg_count; ptr++){
				if (ptr->member_can_have_substitution){
					size_t argnum_buf[d->method.substitution_ds.max_nests+1];
					*argnum_buf=ptr-d->method.member_argtypes;
					size_t * argnums_ptr=argnum_buf;
					size_t ** argnums_ptr_ptr=&argnums_ptr;
					mangleTypeWithSubstitution(
						&d->method.substitution_ds,
						ptr,
						result_ptr,
						argnums_ptr_ptr,
						d->member_nest_count,
						argnum_buf
					);
					result_ptr+=strlen(result_ptr);
				} else {
					mangleType(ptr, result_ptr);
					result_ptr+=ptr->type_length;
				}
			} 
		}
		return result;
	} else if (d->member_nest_count>0) {

		char *result_ptr = result;
		strcpy(result_ptr, "_Z"); result_ptr+=2;
		mangleIdentifierAndNests(d,&result_ptr);
		return result;
	} else {
		//Don't mangle bare globals
		return d->member_id.id;
	}

}
extern const LenId std_namespace_len_id;
static void setNests(
	size_t * nest_count_ptr,
	const LenId ** lenid_nests,
	const char *const * str_nests,
	BumpAllocator * alloc
){

	if (str_nests && *str_nests){
		*nest_count_ptr = 0;
		for (const char *const * nests_bak=str_nests; *nests_bak; nests_bak++) (*nest_count_ptr)++;
		LenId * nests_ptr;
		*lenid_nests=nests_ptr=bump_alloc(alloc,(*nest_count_ptr)*sizeof(LenId));
		for (
				const char *const * str_nest_ptr =str_nests;
				str_nest_ptr<(*nest_count_ptr)+str_nests;
				str_nest_ptr++
			) {
				if (!strcmp("std", *str_nest_ptr))
					*(nests_ptr++)=std_namespace_len_id;
				else 
					*(nests_ptr++)=toLenId(*str_nest_ptr, alloc);
				
			}
	} else {
		*nest_count_ptr = 0;
		*lenid_nests = NULL;
	}
}

const TypeIdentifier createTypeId_(
	const char * base,
	const char * const * nests,
	const POINTER_QUALIFIER * ptrs,
	const unsigned long flags,
	BumpAllocator * alloc
){
	size_t name_len = strlen(base);
	char * base_cpy=bump_alloc(alloc,name_len+1);
	strcpy(base_cpy, base);
	const POINTER_QUALIFIER * end;
	for (end=ptrs; *end!=END; end++);
	const size_t ptr_count=end-ptrs;
	POINTER_QUALIFIER * new_ptr_qualifiers=bump_alloc(alloc, sizeof(POINTER_QUALIFIER)*(ptr_count+1));
	memcpy(new_ptr_qualifiers,ptrs,ptr_count);
	new_ptr_qualifiers[((__ssize_t)ptr_count)-1]&=~(VOLATILE|RESTRICT);
	TypeIdentifier result ={
		.methodnt.name=base_cpy,
		.member_ref=flags&0b1111,
		.methodnt.name_len=name_len,
		.methodnt.name_len_len=digCount(name_len),
		.methodnt.isidentifier=flags&IDENTIFIER_BITMASK?true:false,
		.member_complexity=(flags&0b1100000)>>5,
		.member_pointers=new_ptr_qualifiers,
		.member_pointers_end=new_ptr_qualifiers+ptr_count
		
	};
	setNests(&result.member_aux_nest_count, &result.member_aux_nests,nests, alloc );
	setLengths(&result);
	result.member_can_have_substitution=(
		result.member_pointers<result.member_pointers_end || 
		result.member_complexity!=REAL ||
		result.member_ref!=VALUE || 
		result.methodnt.isidentifier
	);
	return result;
}

static TypeIdentifier * DeepCopyArgtypes(const TypeIdentifier * ti,size_t arg_count, BumpAllocator * alloc){
	TypeIdentifier * const new_argtypes=bump_alloc(alloc, arg_count * sizeof(TypeIdentifier));
	if (new_argtypes) {
		memcpy(new_argtypes, ti, arg_count * sizeof(TypeIdentifier));
		const TypeIdentifier * ti_ptr=ti;
		TypeIdentifier * new_argtypes_ptr=new_argtypes;
		for (;
			ti_ptr<ti+arg_count;
			ti_ptr++, new_argtypes_ptr++
		){
			if (ti_ptr->member_ismethodtype){
				new_argtypes_ptr->method.member_argtypes = DeepCopyArgtypes(ti_ptr->method.member_argtypes, ti_ptr->method.member_arg_count, alloc);
			}
		}
	}
	return new_argtypes;

}

const TypeIdentifier createFunctionPtrTypeId(
	const TypeIdentifier * returns,
	const size_t arg_count,
	const TypeIdentifier * args,
	const char * const * nests,
	const POINTER_QUALIFIER * ptrs,
	const unsigned long flags,
	BumpAllocator * alloc
){
	const POINTER_QUALIFIER * end;
	for (end=ptrs; *end!=END; end++);
	const size_t ptr_count=end-ptrs;
	POINTER_QUALIFIER * new_ptr_qualifiers=bump_alloc(alloc, sizeof(POINTER_QUALIFIER)*(ptr_count+1));
	memcpy(new_ptr_qualifiers,ptrs,ptr_count);
	TypeIdentifier * return_copy;
	if (returns) {
		return_copy=bump_alloc(alloc, sizeof(TypeIdentifier));
		*return_copy=*returns;
	} else
		return_copy=NULL;
	TypeIdentifier result={
		.member_pointers=new_ptr_qualifiers,
		.member_pointers_end=new_ptr_qualifiers+ptr_count,
		.member_ref=flags&0b11,
		.member_complexity=REAL,
		.member_ismethodtype=true,
		.method.member_argtypes=DeepCopyArgtypes(args, arg_count, alloc),
		.method.member_return_type=return_copy,
		.method.member_arg_count=arg_count,
		.member_can_have_substitution=true
	};
	setNests(&result.member_aux_nest_count, &result.member_aux_nests,nests, alloc );
	setLengths(&result);
	return result;
}
const IdentifierData createGlobalIdentifierData(const char * id, const char * const * nests, BumpAllocator * alloc){
	
	IdentifierData result ={
		.ismethod=false,
		.member_nest_count=0,
		.member_nests=NULL
	};
	result.member_id=toLenId(id, alloc);
	setNests(&result.member_nest_count, &result.member_nests, nests, alloc);

	return result;
}
const MethodIdentifierData createMethodIdentifierData(
	const char * id,
	const char ** nests,
	const size_t argtype_n,
	const TypeIdentifier * args,
	BumpAllocator * alloc
){
	
	MethodIdentifierData result ={
		.ismethod=true,
		.member_nest_count=0,
		.method.member_arg_count=argtype_n
	};
	result.member_id=toLenId(id, alloc);
	setNests(&result.member_nest_count, &result.member_nests, nests, alloc);
	if (argtype_n>0){
		TypeIdentifier * new_argtypes = bump_alloc(alloc,result.method.member_arg_count*sizeof(TypeIdentifier));
		memcpy(new_argtypes, args, argtype_n*sizeof(TypeIdentifier));
		result.method.member_argtypes=new_argtypes;
		setSubstitutions(&result, alloc);
	}
	return result;
}
const MethodIdentifierData createSpecialMethodIdentifierData(SPECIAL_METHOD tag, const char *const * nests, const size_t argtype_n, const TypeIdentifier * args,BumpAllocator * alloc){
	MethodIdentifierData result ={
		.ismethod=true,
		.is_special_method=true,
		.member_nest_count=0,
		.method.member_arg_count=argtype_n
	};
	result.member_id=special_method_identifiers[tag];

	setNests(&result.member_nest_count, &result.member_nests, nests, alloc);
	if (argtype_n>0){
		TypeIdentifier * new_argtypes = bump_alloc(alloc,result.method.member_arg_count*sizeof(TypeIdentifier));
		memcpy(new_argtypes, args, argtype_n*sizeof(TypeIdentifier));
		result.method.member_argtypes=new_argtypes;
		setSubstitutions(&result, alloc);
	}
	return result;
}


