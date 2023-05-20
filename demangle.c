#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "cppmangle.h"
const LenId toLenId(const char * const id, BumpAllocator * alloc);
size_t calculate_type_length(const TypeIdentifier * ti);
void setSubstitutions(MethodIdentifierData * d, BumpAllocator * alloc);
static LenId demangle_identifier(const char ** mangled_name, BumpAllocator * alloc){
	if (isdigit(**mangled_name)) {
		const char * save=*mangled_name;
		size_t chars_in_identifier;
		LenId result;
		char * full_id;

		sscanf(*mangled_name,"%zu",&chars_in_identifier);
		const size_t id_len_len=digCount(chars_in_identifier);

		result.id_len=chars_in_identifier;
		result.full_id_len=id_len_len+chars_in_identifier;
		result.full_id=full_id=bump_alloc(alloc,(result.full_id_len+1)*sizeof(char));
		memcpy(full_id, save, result.full_id_len);
		full_id[result.full_id_len]='\0';
		result.id=result.full_id+id_len_len;

		*mangled_name+=result.full_id_len;
		return result;
	} else {
		//TODO
		(*mangled_name)++;
	}
}
inline static void move_to_next_identifier(const char ** str){
	size_t chars_in_identifier;
	sscanf(*str,"%zu",&chars_in_identifier);
	(*str)+=digCount(chars_in_identifier)+chars_in_identifier;
}
inline static size_t count_nests(const char * str){
	size_t result=0;
	while (*str != 'E'){
		if (isdigit(*str)){
			move_to_next_identifier(&str);
		} else str=strchr(str, 'E');
		result++;
	}
	return --result;
}
inline static size_t calculate_auxdata_len(const char * str){
	size_t result=0;
	while(strchr("rVPKROCG", *(str++))) result++;
	return result;
	
}
inline static size_t count_pointer_qualifiers(const char * str){
	size_t result=0;
	while(strchr("rVPKROCG", *str)) result+=(*(str++) == 'P')?1:0;
	return result;

}
static char ** find(const char * potential_subsitution, char ** subs){
	char ** sub_ptr=subs;
	for (; *sub_ptr!=NULL; sub_ptr++){
		if(!strcmp(potential_subsitution,*sub_ptr)) return sub_ptr;
	}
	return sub_ptr;
}
static void demangle_substitutions(const char * str,char *** subs){
	size_t auxdata_count=0;
	size_t sub_length=(*str=='D')?2:1;
	//get to end of qualifiers
	while(strchr("rVPKROCG", *str)) {str++; auxdata_count++;}
	if (isdigit(*str)){
		sscanf(str,"%zu",&sub_length);
		sub_length+=digCount(sub_length);
		char ** sub_ptr=find(str, *subs);
		if (!*sub_ptr){
			*sub_ptr=malloc((sub_length+1)*sizeof(char));
			memcpy(*sub_ptr, str, sub_length);
			(*sub_ptr)[sub_length]='\0';
		}
		
	}
	str--;
	while(auxdata_count>0){
		if (*str!='V'){
			sub_length++;
			char ** sub_ptr=find(str, *subs);
			if (!*sub_ptr){
				*sub_ptr=malloc((sub_length+1)*sizeof(char));
				memcpy(*sub_ptr, str, sub_length);
				(*sub_ptr)[sub_length]='\0';
			}
			str--;
		}
		auxdata_count--;
	}
	
}
static size_t calculate_type_count(const char * str){
	size_t result=0;
	if (*str=='v') return 0;
	while(*str!='\0' && *str!='E'){
		if (isdigit(*str)){
			move_to_next_identifier(&str);
		} else {
			while(strchr("rVPKROCGD", *str)) str++;
			if (*str=='S'){
				while (*str!='_' && !islower(*str)) str++;
				str++;
			} 
			else if(*str=='F' || *str == 'N') {
				size_t fcount=1;
				str++;
				while(fcount>0){
					if (*str == 'E') fcount--;
					if (*str == 'F' || *str == 'N') fcount++;
					
					if (isdigit(*str)) move_to_next_identifier(&str);
					else str++;
				} 
			} else str++;
		}
		result++;
	}
	return result;
}

TypeIdentifier demangle_type(
	const char ** mangled_name_ptr,
	char *** subs,
	BumpAllocator * alloc
){
	TypeIdentifier result={
		.member_auxdata_len=calculate_auxdata_len(*mangled_name_ptr),
		.member_complexity=REAL,
		.member_ref=VALUE

	};
	

	{
	const size_t pointer_qualifier_count=count_pointer_qualifiers(*mangled_name_ptr);
	POINTER_QUALIFIER * pointer_qualifiers
		=bump_alloc(alloc, sizeof(POINTER_QUALIFIER)*
			(pointer_qualifier_count+1));
	memset (pointer_qualifiers, 0, pointer_qualifier_count);
	result.member_pointers = pointer_qualifiers;
	result.member_pointers_end = result.member_pointers+pointer_qualifier_count;
	pointer_qualifiers[pointer_qualifier_count]=END;
	demangle_substitutions(*mangled_name_ptr, subs);
	while(strchr("rVPKROCG", **mangled_name_ptr)){
		result.member_can_have_substitution=true;
		switch (**mangled_name_ptr){
			case 'r':
				*pointer_qualifiers|=RESTRICT;
				break;
			case 'V':
				*pointer_qualifiers|=VOLATILE;
				break;
			case 'P':
				pointer_qualifiers++;
				break;
			case 'K':
				*pointer_qualifiers|=CONSTANT;
				break;
			case 'O':
				result.member_ref=RVALUEREF;
				break;
			case 'R':
				result.member_ref=LVALUEREF;
				break;
			case 'C':
				result.member_complexity=COMPLEX;
				break;
			case 'G':
				result.member_complexity=IMAGINARY;
				break;
		}
		(*mangled_name_ptr)++;
	}
	}
	char * id;
	if (**mangled_name_ptr=='S') {
		(*mangled_name_ptr)++;
		if (islower(**mangled_name_ptr)) goto twocharid;
		size_t subnum=(**mangled_name_ptr=='_')?0:strtoll(*mangled_name_ptr,NULL, 36)+1;
		const char * new_type=(*subs)[subnum];
		while (**mangled_name_ptr != '_') (*mangled_name_ptr)++;
		(*mangled_name_ptr)++;
		return demangle_type(&new_type,subs, alloc);


	} else if (**mangled_name_ptr == 'N'){
		(*mangled_name_ptr)++;
		result.member_aux_nest_count = count_nests(*mangled_name_ptr);
		LenId * nests=bump_alloc(alloc, sizeof(LenId)*result.member_aux_nest_count);
		LenId * nest_buffer_ptr=nests;
		while (nest_buffer_ptr<nests+result.member_aux_nest_count){
			*(nest_buffer_ptr++)=demangle_identifier(
				mangled_name_ptr, 
				alloc
			);
		}
		{
			size_t chars_in_identifier;
			assert(sscanf(*mangled_name_ptr, "%zu", &chars_in_identifier)==1);
			result.methodnt.name_len=chars_in_identifier;
			result.methodnt.name_len_len=digCount(chars_in_identifier);
			char * id = bump_alloc(alloc, chars_in_identifier+1);
			(*mangled_name_ptr)+=result.methodnt.name_len_len;
			memcpy(id, *mangled_name_ptr, chars_in_identifier);
			id[chars_in_identifier]='\0';
			result.methodnt.isidentifier=true;
			result.methodnt.name=id;
			(*mangled_name_ptr)+=result.methodnt.name_len;
		}
		result.member_aux_nests=nests;
		assert(**mangled_name_ptr=='E');
		(*mangled_name_ptr)++;


	} else if (**mangled_name_ptr=='F'){
		result.member_can_have_substitution=true;
		(*mangled_name_ptr)++;
		result.member_ismethodtype=true;
		TypeIdentifier * ti_ptr;
		result.method.member_return_type=ti_ptr=bump_alloc(alloc, sizeof(TypeIdentifier));

		*ti_ptr=demangle_type(mangled_name_ptr,subs, alloc);
		result.method.member_arg_count=calculate_type_count(*mangled_name_ptr);
		result.method.member_argtypes=ti_ptr=bump_alloc(alloc, result.method.member_arg_count*sizeof(TypeIdentifier));
		while(**mangled_name_ptr!='E'){
			*ti_ptr=demangle_type(mangled_name_ptr,subs, alloc);
		}
		(*mangled_name_ptr)++;
	} else {

		result.methodnt.isidentifier=false;
		POINTER_QUALIFIER current=0;
		if (isdigit(**mangled_name_ptr)){
			size_t chars_in_identifier;
			sscanf(*mangled_name_ptr, "%zu", &chars_in_identifier);
			result.methodnt.name_len=chars_in_identifier;
			result.methodnt.name_len_len=digCount(chars_in_identifier);
			id = bump_alloc(alloc, chars_in_identifier+1);
			(*mangled_name_ptr)+=result.methodnt.name_len_len;
			memcpy(id, *mangled_name_ptr, chars_in_identifier);
			id[chars_in_identifier]='\0';
			result.methodnt.isidentifier=true;
			result.methodnt.name=id;
			(*mangled_name_ptr)+=result.methodnt.name_len;
		} else if(**mangled_name_ptr == 'D'){
		twocharid:
			id=bump_alloc(alloc, 3*sizeof(char));
			memcpy(id, *mangled_name_ptr, 2);
			id[2]='\0';
			result.methodnt.name=id;
			result.methodnt.name_len=2;
			result.methodnt.name_len_len=1;
			(*mangled_name_ptr)+=2;
		} else {
			id=bump_alloc(alloc, 2*sizeof(char));
			id[0]=**mangled_name_ptr;
			id[1]='\0';
			result.methodnt.name=id;
			result.methodnt.name_len=1;
			result.methodnt.name_len_len=1;
			(*mangled_name_ptr)++;
		}
	}
	result.type_length=calculate_type_length(&result);
	return result;
}
const MethodIdentifierData demangle(const char * mangled_name, BumpAllocator * alloc){

	MethodIdentifierData result={
		.ismethod=false,
		.is_special_method=false,
		.member_nest_count=0
	};


	if (memcmp(mangled_name, "_Z", 2)){
		result.member_id=toLenId(mangled_name,alloc);
		return result;
	}

	mangled_name+=2;
	char ** subs=malloc(sizeof(char * )*1024);
	for(size_t i=0; i<1024;i++) subs[i]=NULL;
	if (*mangled_name=='N'){
		char ** sub_ptr=subs;
		mangled_name++;
		result.member_nest_count=count_nests(mangled_name);
		LenId * nests=bump_alloc(alloc, sizeof(LenId)*result.member_nest_count);
		LenId * nest_ptr=nests;
		//LenId * nest_buffer_ptr=nest_buffer;
		size_t nest_len_sum=0;
		while (nest_ptr<nests+result.member_nest_count){
			*nest_ptr=demangle_identifier(
				&mangled_name, 
				alloc
			);
			nest_len_sum+=nest_ptr->full_id_len;
			*sub_ptr=malloc((nest_len_sum+1)*sizeof(char));
			char * sub_ptr_ptr=*sub_ptr;
			for (LenId * nest_ptr2=nests;nest_ptr2<=nest_ptr; nest_ptr2++){
				strcpy(sub_ptr_ptr, nest_ptr2->full_id);
				sub_ptr_ptr+=nest_ptr2->full_id_len;
			}
			*sub_ptr_ptr='\0';
			sub_ptr++;
			nest_ptr++;
		}
		result.member_id=demangle_identifier(
			&mangled_name, 
			alloc
		);
		
		//memcpy(nests,nest_buffer,sizeof(LenId)*result.member_nest_count);
		result.member_nests=nests;
		assert(*mangled_name == 'E');
		mangled_name++;
	} else 
		result.member_id=demangle_identifier(&mangled_name, alloc);
	
	if (*mangled_name=='\0'){
	} else {
		result.ismethod=true;
		result.method.member_return_type=NULL;
		result.method.member_arg_count=calculate_type_count(mangled_name);
		TypeIdentifier * ti_ptr;
		result.method.member_argtypes=ti_ptr=bump_alloc(alloc, result.method.member_arg_count*sizeof(TypeIdentifier));

		while(*mangled_name!='\0')
			*(ti_ptr++)=demangle_type(&mangled_name,&subs,alloc);
		

		
	}
	for(size_t i=0; subs[i];i++) free(subs[i]);
	free(subs);
	setSubstitutions(&result, alloc);
	return result;

	
}