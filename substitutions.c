#include "allocator.h"
#include "cppmangle.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
extern const size_t RETURN;

void set_auxdata_length(AuxilliaryTypeData * aux);
char * mangleAuxData(const AuxilliaryTypeData * aux, char * buf);
void setLengths(TypeIdentifier * ti);




#define MAX_SUBSTITUTION_SIZE 16
typedef struct {
	char * format_str;
	bool isvalid : 1;
	bool iseq : 1;
	AuxilliaryTypeData auxdata;
} TypeIdentifierDiff;

static TypeIdentifierDiff subtract(
	const TypeIdentifier * ti1,
	const TypeIdentifier * ti2,
	char * buf,
	POINTER_QUALIFIER * pointer_qualifier_copy_buf
){
	/*
	PCi
	Ci
	*/
	const static TypeIdentifierDiff invalid = {
		.isvalid=false
	};
	if (ti1->member_ismethodtype){
		{
		const TypeIdentifier * return_type = ti1->method.member_return_type;
		if (!ti2->member_ismethodtype) return invalid;
		if (
			ti1->method.member_return_type!=NULL &&
			ti2->method.member_return_type!=NULL) {

			char buf2[return_type->type_length+1];
			POINTER_QUALIFIER pqbuf2[return_type->member_pointers_end-return_type->member_pointers+1];
			if (
				ti1->method.member_arg_count != ti2->method.member_arg_count ||
				!subtract(ti1->method.member_return_type,
					ti2->method.member_return_type,buf2,pqbuf2).iseq 
				) 
					return invalid;
		} else {
			if (ti1->method.member_return_type==ti2->method.member_return_type)
				return invalid;
			
		}
		}


		for (size_t i=0; i<ti1->method.member_arg_count; i++){
			TypeIdentifier * ti1_arg=&ti1->method.member_argtypes[i];
			TypeIdentifier * ti2_arg=&ti2->method.member_argtypes[i];
			char buf2[ti1_arg->type_length+1];
			POINTER_QUALIFIER pqbuf2[ti1_arg->member_pointers_end-ti1_arg->member_pointers+1];
			if (!subtract(ti1_arg, ti2_arg,buf2,pqbuf2).iseq) return invalid;
		}
		/*
		return (TypeIdentifierDiff){
			.iseq=true,
			.isvalid=true,
			.member_pointers=pointer_qualifier_copy_buf,
			.member_pointers_end=pointer_qualifier_copy_buf,
			.member_ref=VALUE,
			.format_str="%s",
			.member_complexity=REAL
		};
		*/
	} else{
		if (
				ti2->member_ismethodtype || 
				ti1->methodnt.isidentifier != ti2->methodnt.isidentifier ||
				strcmp(ti1->methodnt.name, ti2->methodnt.name)
			) return invalid;
	}
	TypeIdentifierDiff result;

	if (ti1->member_complexity == REAL && ti2->member_complexity != REAL) return invalid;
	if (ti1->member_ref == VALUE && ti2->member_ref != VALUE) return invalid;
	const POINTER_QUALIFIER * pointer_ptr1 = ti1->member_pointers;
	const POINTER_QUALIFIER * pointer_ptr2 = ti2->member_pointers;

	//POINTER_QUALIFIER pointer_qualifier_copy[pointer_size];
	bool is_ti2_a_pointer;
	if ((is_ti2_a_pointer=ti2->member_pointers<ti2->member_pointers_end)){
		for (;pointer_ptr2<ti2->member_pointers_end-1; pointer_ptr1++, pointer_ptr2++){
			//PKPi PPi
			if (
				(pointer_ptr1>=ti1->member_pointers_end) ||
				(*pointer_ptr1)!=(*pointer_ptr2) 
			) return invalid;
			//*(pointer_qualifier_copy_end_ptr++)=(*pointer_ptr1)-(*pointer_ptr2);
		}
		if (
			((*pointer_ptr1&CONSTANT) && !(*pointer_ptr2&CONSTANT)) ||		/*PKi Pi*/
			(!(*pointer_ptr1&CONSTANT) && (*pointer_ptr2&CONSTANT)) || 		/*Pi (P)Ki*/
			((*pointer_ptr1 & NO_P)   && !(*pointer_ptr2 & NO_P)) ||		/*Ki PKi*/
			(!(*pointer_ptr1 & VOLATILE) && (*pointer_ptr2 & VOLATILE)) ||	/*Pi VPi*/
			(!(*pointer_ptr1 & RESTRICT) && (*pointer_ptr2 & RESTRICT))		/*Pi rPi*/

		) return invalid;
	}
	if (ti1->member_pointers_end>pointer_ptr1){
		const size_t pointer_count=ti1->member_pointers_end-pointer_ptr1;
		POINTER_QUALIFIER * pointer_qualifier_copy_end_ptr=pointer_qualifier_copy_buf+pointer_count;
		if (is_ti2_a_pointer) {
			memcpy(pointer_qualifier_copy_buf+1,pointer_ptr1+1,pointer_count-1);
			*pointer_qualifier_copy_buf=(*pointer_ptr1)-(*pointer_ptr2);

			if ((*pointer_ptr1 & NO_P) == (*pointer_ptr2 & NO_P))
				*pointer_qualifier_copy_buf|=NO_P;
			else if (!(*pointer_ptr1 & NO_P) && (*pointer_ptr2 & NO_P))
				*pointer_qualifier_copy_buf-=NO_P; // reverse the overflow that just happened. 
		} else
			memcpy(pointer_qualifier_copy_buf,pointer_ptr1,pointer_count);
		
		if(*pointer_qualifier_copy_buf == NO_P){
			pointer_qualifier_copy_buf++;
		}
		result=(TypeIdentifierDiff) {
			.isvalid=true,
			.member_pointers=pointer_qualifier_copy_buf,
			.member_pointers_end=pointer_qualifier_copy_end_ptr,
			.member_ref=ti1->member_ref-ti2->member_ref,
			.format_str=buf,
			.member_complexity=ti1->member_complexity-ti2->member_complexity
		};
	
	
	} else {
		result = (TypeIdentifierDiff) {
			.isvalid=true,
			.member_pointers=pointer_qualifier_copy_buf,
			.member_pointers_end=pointer_qualifier_copy_buf,
			.member_ref=ti1->member_ref-ti2->member_ref,
			.format_str=buf,
			.member_complexity=ti1->member_complexity-ti2->member_complexity
		};
	}
	
	set_auxdata_length(&result.auxdata);
	{
	char buf2[result.member_auxdata_len+1];
	const char * fmt = mangleAuxData(&result.auxdata, buf2);
	result.iseq=(*fmt == '\0');
	
	sprintf(result.format_str, "%s%%s", fmt);
	}

	return result;


}

static void findSubstitutionArgRec(
	const TypeIdentifier * from,
	const size_t * argnum,
	const ArgSubstitutionTree * arg_subs,
	//const SubstitutionDataStructure * subs,
	const Substitution ** found,
	TypeIdentifierDiff * found_diff,
	size_t arg_sub_count,
	char * buf
){
	
	const ArgSubstitutionTree * arg_sub_ptr=arg_subs;
	while (
		arg_sub_ptr < arg_subs+arg_sub_count &&
		*argnum!=RETURN &&
		(
			arg_sub_ptr->argnum==RETURN || 
			*argnum>=arg_sub_ptr->argnum
		)
	){
		if (arg_sub_ptr->subs_of_func_ptr)
			findSubstitutionArgRec(
				from,
				argnum+1,
				arg_sub_ptr->subs_of_func_ptr,
				//subs,
				found,
				found_diff,
				arg_sub_ptr->argument_sub_num_for_func_ptr,
				buf
			);
		
		if (arg_sub_ptr->argnum!=RETURN && *argnum<=arg_sub_ptr->argnum) return;
		POINTER_QUALIFIER pq_buf[from->member_pointers_end-from->member_pointers];
		char buf2[from->type_length+1];
		TypeIdentifierDiff diff=subtract(from, &arg_sub_ptr->substitution.from,buf2,pq_buf);
		if (
			diff.isvalid &&
			(!*found ||
				arg_sub_ptr->substitution.from.type_length
				> (*found)->from.type_length)
			){
			*found=&arg_sub_ptr->substitution;
			strcpy(buf, diff.format_str);
			diff.format_str=buf;
			*found_diff=diff;
			if (diff.iseq) break; // optimization
		}
		arg_sub_ptr++;
	}


}

static const char * findSubstitution(
	const SubstitutionDataStructure * subs,
	const TypeIdentifier * from,
	const size_t * argnum,
	const Substitution ** found,
	const size_t nest_count,
	char * buf
){
	assert(found);
	if (!subs) return NULL;
	TypeIdentifierDiff found_diff;
	if (subs->nest_subtitutions){
		for (Substitution * nest_sub = subs->nest_subtitutions;
		nest_sub < subs->nest_subtitutions + nest_count; nest_sub++){

			POINTER_QUALIFIER pq_buf[from->member_pointers_end-from->member_pointers];
			char buf2[from->type_length+1];
			TypeIdentifierDiff diff=subtract(from, &nest_sub->from,buf2,pq_buf);
			if (
				diff.isvalid &&
					(!*found ||
						nest_sub->from.type_length > (*found)->from.type_length)
			){
				*found=nest_sub;
				strcpy(buf, diff.format_str);
				diff.format_str=buf;
				found_diff=diff;
				if (diff.iseq) break;
			}
		}
	}
	if (subs->argument_substitutions) {
		findSubstitutionArgRec(
			from,
			argnum,
			subs->argument_substitutions,
			//subs->argument_substitutions,
			found,
			&found_diff,
			subs->argument_sub_num,
			buf
		);
	}
	return found_diff.format_str;

}
const char * mangleTypeWithSubstitution(
	const SubstitutionDataStructure * subs,
	const TypeIdentifier * ti,
	char * buf,
	size_t ** argnum_ptr,
	size_t nest_count,
	size_t * argnum_buf
){

	const Substitution * found=NULL;
	char buf2[ti->type_length+5];
	const char * format_str = findSubstitution(
		subs,
		ti,
		argnum_buf,
		&found,
		nest_count,
		buf2
	);
	if (found){
		sprintf(buf, format_str, found->to);
	} else if(ti->member_ismethodtype){
		(*argnum_ptr)++;
		char * bufptr=buf;
		{
		char buf2[ti->member_auxdata_len+1];
		const char * mangled_aux_data=mangleAuxData(&ti->auxdata, buf2);
		strcpy(bufptr, mangled_aux_data); bufptr+=ti->member_auxdata_len;
		}
		*(bufptr++)='F';
		if (ti->method.member_return_type){
			**argnum_ptr=RETURN;
			
			const char * s=mangleTypeWithSubstitution(
				subs,
				ti->method.member_return_type,
				bufptr,
				argnum_ptr,
				nest_count,
				argnum_buf
			);

			bufptr+=strlen(s);
			//bufptr+=ti->method.member_return_type->type_length;
		} else 
			*(bufptr++)='v';
		if (ti->method.member_arg_count){
			for (TypeIdentifier * arg_ti = ti->method.member_argtypes;
				arg_ti<ti->method.member_argtypes+ti->method.member_arg_count;
				arg_ti++
			){

				**argnum_ptr=arg_ti-ti->method.member_argtypes;
				const char * s=mangleTypeWithSubstitution(
					subs,
					arg_ti,
					bufptr,
					argnum_ptr,
					nest_count,
					argnum_buf
				);
				bufptr+=strlen(s);
			}
		} else 
			*(bufptr++)='v';
		
		strcpy(bufptr, "E");
	} else {
		mangleType(ti, buf);
	}
	return buf;
}
static bool does_substitution_exist_rec(
	const TypeIdentifier * needle, const ArgSubstitutionTree * tree,size_t argument_sub_num, char * buf){
	if (tree){
		for (const ArgSubstitutionTree * treeptr=tree; treeptr<tree+argument_sub_num; treeptr++){
			if (
				treeptr->subs_of_func_ptr &&
				does_substitution_exist_rec(
					needle,
					treeptr->subs_of_func_ptr,
					treeptr->argument_sub_num_for_func_ptr,
					buf)
			) return true;

			POINTER_QUALIFIER pq_buf[needle->member_pointers_end-needle->member_pointers];
			TypeIdentifierDiff diff=subtract(needle, &treeptr->substitution.from, buf,pq_buf);
			if (diff.isvalid && diff.iseq) return true;
			
		}
	} 
	return false;

}
static bool does_substitution_exist(const TypeIdentifier * needle, const SubstitutionDataStructure * ds){
	char buf[needle->type_length+5];
	if (ds->nest_subtitutions){
		for (Substitution * sub = ds->nest_subtitutions; sub<ds->nest_subtitutions+ds->nest_sub_num;sub++){
			POINTER_QUALIFIER pq_buf[needle->member_pointers_end-needle->member_pointers];
			TypeIdentifierDiff diff=subtract(needle, &sub->from, buf,pq_buf);
			if (diff.isvalid && diff.iseq) return true;
			
		}

	}

	return does_substitution_exist_rec(
		needle,
		ds->argument_substitutions,
		ds->argument_sub_num,
		buf);
	

}
static size_t getSubArrLen(const MethodData * d){
	
	size_t result = 0;
	const TypeIdentifier * return_type;
	if ((return_type=d->member_return_type)){
		result+=return_type->member_auxdata_len;
		if (return_type->member_ismethodtype){
			result+=getSubArrLen(&return_type->method.method_data)+1;
		} else {
			result+=return_type->methodnt.isidentifier?1:0;
		}
	}	
	for (const TypeIdentifier * ti=d->member_argtypes; ti<d->member_argtypes+d->member_arg_count; ti++) {
		result+= ti->member_auxdata_len;
		if (ti->member_ismethodtype){
			result+=getSubArrLen(&ti->method.method_data)+1;
		} else {
			result+=ti->methodnt.isidentifier?1:0;
		}
	}
	return result;
}

static const char * base36(size_t n, char * buf){
	if (n==0){
		return "0";
	} else {
		char * ptr = buf+13;
		*(ptr--)='\0';
		while(n>0){
			unsigned int dig=n%36;
			if (dig>10){
				*(ptr--)=dig-10+'A';
			} else {
				*(ptr--)=dig+'0';
			}
			n/=36;
		}
		return ptr+1;
	}
}
static char * generateSubstitution(size_t n, char * buf){
	if (n==0){
		strcpy(buf, "S_");
	} else {
		--n;
		char buf2[14];
		sprintf(buf,"S%s_", base36(n,buf2));
	}
	return buf;
}
static inline void updatePtr(
	SubstitutionDataStructure * substitution_ds,
	const size_t argnum,
	TypeIdentifier * ticpy,
	ArgSubstitutionTree ** substitution_tree_ptr,
	size_t * subnum_ptr, 
	BumpAllocator * alloc,
	size_t * argument_sub_num_ptr,
	const bool allocate_and_copy_pointer_qualifiers
){
	setLengths(ticpy);
	if (!does_substitution_exist(ticpy, substitution_ds)){
		char * new_to=bump_alloc(alloc, MAX_SUBSTITUTION_SIZE); 
		(*substitution_tree_ptr)->substitution.from=*ticpy;
		if (allocate_and_copy_pointer_qualifiers){
			const size_t pointer_count=ticpy->member_pointers_end-ticpy->member_pointers;
			const POINTER_QUALIFIER * source = ticpy->member_pointers;
			POINTER_QUALIFIER * destination=bump_alloc(alloc, (pointer_count+1)*sizeof(POINTER_QUALIFIER));
			memcpy(destination, source, pointer_count);
			(*substitution_tree_ptr)->substitution.from.member_pointers=destination;
			(*substitution_tree_ptr)->substitution.from.member_pointers_end=destination+pointer_count;

		}
		(*substitution_tree_ptr)->substitution.to=generateSubstitution(*subnum_ptr, new_to); 
		(*substitution_tree_ptr)->substitution.to_len=strlen(new_to); 
		(*substitution_tree_ptr)->argnum=argnum;
		(*subnum_ptr)++; 
		(*substitution_tree_ptr)++;
		(*substitution_tree_ptr)->argument_sub_num_for_func_ptr=0;
		(*substitution_tree_ptr)->subs_of_func_ptr=NULL;
		(*argument_sub_num_ptr)++;
	}
}

static void setArgSubstitutionsRec(
	SubstitutionDataStructure * substitution_ds,
	const TypeIdentifier * ti,
	ArgSubstitutionTree ** substitution_tree_ptr,
	BumpAllocator * alloc,
	size_t * subnum_ptr,
	size_t argnum,
	size_t * argument_sub_num_ptr,
	size_t * max_nests,
	size_t cur_nests
){

	if (cur_nests>*max_nests) *max_nests=cur_nests;
	TypeIdentifier ticpy = {
		.methodnt.name=ti->methodnt.name,
		.methodnt.name_len=ti->methodnt.name_len,
		.methodnt.name_len_len=ti->methodnt.name_len_len,
		.methodnt.isidentifier=ti->methodnt.isidentifier,
		.member_pointers_end=ti->member_pointers,
		.member_pointers=ti->member_pointers,
		.member_ismethodtype=false
	};
	static const bool DO_NOT_COPY_PTR_QUALIFIERS=false;
	static const bool COPY_PTR_QUALIFIERS=true;
	(*substitution_tree_ptr)->subs_of_func_ptr=NULL;
	(*substitution_tree_ptr)->argument_sub_num_for_func_ptr=0;
	if (ti->member_ismethodtype){
		const size_t arrlen = getSubArrLen(&ti->method.method_data);
		if (arrlen){
			(*substitution_tree_ptr)->subs_of_func_ptr=bump_alloc(alloc, arrlen*sizeof (ArgSubstitutionTree));
			//(*substitution_tree_ptr)->argument_sub_num=0;
			ArgSubstitutionTree * argtree2=(*substitution_tree_ptr)->subs_of_func_ptr;
			(*argument_sub_num_ptr)++; //temporarily increment it so the program knows there's a partial function argument here
			if (ti->method.member_return_type != NULL)
				setArgSubstitutionsRec(
					substitution_ds,
					ti->method.member_return_type,
					&argtree2,
					alloc,
					subnum_ptr,
					RETURN,
					&(*substitution_tree_ptr)->argument_sub_num_for_func_ptr,
					max_nests,
					cur_nests+1
				);
			for (const TypeIdentifier * argptr = ti->method.member_argtypes;
				argptr < ti->method.member_argtypes + ti->method.member_arg_count;
				argptr++
			){

				setArgSubstitutionsRec(
					substitution_ds,
					argptr,
					&argtree2,
					alloc,
					subnum_ptr,
					argptr-ti->method.member_argtypes,
					&(*substitution_tree_ptr)->argument_sub_num_for_func_ptr,
					max_nests,
					cur_nests+1
				);

			}
			(*argument_sub_num_ptr)--; //ok done with the partial function
		}
		ticpy.member_ismethodtype=true;
		ticpy.method.member_return_type=ti->method.member_return_type;
		updatePtr(
			substitution_ds,
			argnum,
			&ticpy,
			substitution_tree_ptr,
			subnum_ptr,
			alloc,
			argument_sub_num_ptr,
			DO_NOT_COPY_PTR_QUALIFIERS
		);
		//ticpy.member_ismethodtype=false;
	}
	else if (ti->methodnt.isidentifier){
		ticpy.methodnt.isidentifier=true;
		updatePtr(
			substitution_ds,
			argnum,
			&ticpy,
			substitution_tree_ptr,
			subnum_ptr,
			alloc,
			argument_sub_num_ptr,
			DO_NOT_COPY_PTR_QUALIFIERS
		);
	}
	if (ti->member_complexity){
		ticpy.member_complexity = ti->member_complexity;
		updatePtr(
			substitution_ds,
			argnum,
			&ticpy,
			substitution_tree_ptr,
			subnum_ptr,
			alloc,
			argument_sub_num_ptr,
			DO_NOT_COPY_PTR_QUALIFIERS
		);
	}
	{
	const size_t pointer_count=ti->member_pointers_end-ti->member_pointers;
	POINTER_QUALIFIER pointers_copy[pointer_count+1];
	POINTER_QUALIFIER * pointers_copy_end = pointers_copy+pointer_count;
	memcpy(pointers_copy,ti->member_pointers, pointer_count);
	ticpy.member_pointers_end=ticpy.member_pointers=pointers_copy;
	for (
		POINTER_QUALIFIER * pointer_ptr = pointers_copy;
		pointer_ptr<pointers_copy_end;
		pointer_ptr++
	){
		POINTER_QUALIFIER qualifier_save = *pointer_ptr;
		ticpy.member_pointers_end=pointer_ptr+1;
		
		if (qualifier_save & CONSTANT){
			*pointer_ptr=NO_P | CONSTANT;
			updatePtr(
				substitution_ds,
				argnum,
				&ticpy,
				substitution_tree_ptr,
				subnum_ptr,
				alloc,
				argument_sub_num_ptr,
				COPY_PTR_QUALIFIERS
			);
		}
		if (qualifier_save & RESTRICT){
			*pointer_ptr=qualifier_save&CONSTANT;
			updatePtr(
					substitution_ds,
					argnum,
					&ticpy,
					substitution_tree_ptr,
					subnum_ptr,
					alloc,
					argument_sub_num_ptr,
					COPY_PTR_QUALIFIERS
				);
		}
		if ((qualifier_save & RESTRICT) || !(qualifier_save & VOLATILE)){
			*pointer_ptr=qualifier_save;
			updatePtr(
					substitution_ds,
					argnum,
					&ticpy,
					substitution_tree_ptr,
					subnum_ptr,
					alloc,
					argument_sub_num_ptr,
					COPY_PTR_QUALIFIERS
				);
		}
	}
	ticpy.member_pointers=ti->member_pointers;
	ticpy.member_pointers_end=ti->member_pointers_end;

	}
	if (ti->member_ref){
		ticpy.member_ref=ti->member_ref;
		updatePtr(
			substitution_ds,
			argnum,
			&ticpy,
			substitution_tree_ptr,
			subnum_ptr,
			alloc,
			argument_sub_num_ptr,
			DO_NOT_COPY_PTR_QUALIFIERS
		);
	}
}

void setSubstitutions(MethodIdentifierData * d, BumpAllocator * alloc){
	size_t subnum=0;
	d->method.substitution_ds.nest_sub_num=d->member_nest_count;
	if (d->member_nest_count != 0){
		d->method.substitution_ds.nest_subtitutions=bump_alloc(alloc, d->member_nest_count * sizeof (Substitution));
		const LenId * nest_ptr=d->member_nests;
		for (Substitution * substitution_ptr=d->method.substitution_ds.nest_subtitutions;
			substitution_ptr<d->method.substitution_ds.nest_subtitutions+d->member_nest_count;
			substitution_ptr++
		){
			
			TypeIdentifier ticpy = {
				.member_aux_nest_count=0,
				.member_aux_nests=0,
				.methodnt.name=nest_ptr->full_id

			};
			char * new_to=bump_alloc(alloc, MAX_SUBSTITUTION_SIZE);
			substitution_ptr->from=ticpy;
			substitution_ptr->to=generateSubstitution(subnum, new_to); 
			subnum++;
		}
	} else {
		d->method.substitution_ds.nest_subtitutions=NULL;
	}
	const size_t arrlen = getSubArrLen(&d->method.method_data);
	if (arrlen != 0){
		d->method.substitution_ds.argument_substitutions=bump_alloc(alloc, arrlen * sizeof (ArgSubstitutionTree));
		d->method.substitution_ds.argument_sub_num=0;
		
		if (d->method.member_arg_count!=0){
			ArgSubstitutionTree * argtree=d->method.substitution_ds.argument_substitutions;
			for (
				const TypeIdentifier * ti=d->method.member_argtypes;
				ti<d->method.member_argtypes+d->method.member_arg_count;
				ti++
			){
				setArgSubstitutionsRec(
					&d->method.substitution_ds,
					ti,
					&argtree,
					alloc,
					&subnum,
					ti-d->method.member_argtypes,
					&d->method.substitution_ds.argument_sub_num,
					&d->method.substitution_ds.max_nests,
					0
				);
			}
		}
	} else {
		d->method.substitution_ds.argument_substitutions=NULL;
		d->method.substitution_ds.argument_sub_num=0;
	}
}