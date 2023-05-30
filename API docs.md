# Simplified Interface 

## mangle 

* const char * id - Name of Identifier
* size_t nest_n - number of "nests" (that is classes and namespaces)
* size_t args_n - number of function arguments or NOT_METHOD for non methods. 
* char * ... - names of all the nests. 
* TypeIdentifier ... - types of all the arguments. 

## CreateTypeIdentifier
This is necessary for types that aren't already defined in `cpptypes.h`
* const char * base - The "base name" of the type. For classes this is the name of the class. For primitive types use one of the strings defined in `cpptypes.h`

* const POINTER_QUALIFIER * ptrs - A list of pointer qualifiers for each pointer, which can be CONST, VOLATILE, RESTRICT or any orred combination of them. Note that classically qualifiers apply to the pointer directly to the left of them. But for this library all the "consts" are shifted one to the left (You can think of it as const meaning "the contents can't be modified" as opposed to "the pointer can't be modified"). so `const int * __restrict volatile` would be an integer with a single pointer qualifier of the form `CONSTANT|VOLATILE|RESTRICT`. If your type isn't a pointer this can be NULL. The array is terminated with the END constant. 

* unsigned long flags - Flags for the rest of the auxilliary type data. The options are. 
  * LVALUE_REF_BITMASK
  * RVALUE_REF_BITMASK
  * IDENTIFIER_BITMASK - marks that the type isn't primitive. 
  * COMPLEX_BITMASK - Note: Only for C style complex numbers, C++ style complex numbers are just templates
  * IMAGINARY_BITMASK

* size_t nest_n - number of nests the type is defined in. 
* const char * nest - names of all the nests. 

# Internal Interface 

## BumpAllocator 
Before you do anything with name mangling you have to create a BumpAllocator. I am using a BumpAllocator because my structures are filled with little dynamically allocated bits that are hard to keep track of. You can easily create one using 
```
BumpAllocator allocator={0};
```
From there you can allocate using `bump_alloc(BumpAllocator *, size_t)` and free all the memory associated with it using `bump_free(BumpAllocator *)`

## Functions
### CreateTypeId_
To create a type you use `createTypeId_` it takes 5 arguments. 
* const char * base - The "base name" of the type. For classes this is the name of the class. For primitive types use one of the strings defined in `cpptypes.h`
* const char * const* nests - If you defined the type inside a namespace or class you have to create an array. listing all the namespaces and classes from the outermost to the innermost. The array is terminated with a NULL. If you defined the type in the global scope you can put NULL here. 
* const POINTER_QUALIFIER * ptrs - A list of pointer qualifiers for each pointer, which can be CONST, VOLATILE, RESTRICT or any orred combination of them. Note that classically qualifiers apply to the pointer directly to the left of them. But for this library all the "consts" are shifted one to the left (You can think of it as const meaning "the contents can't be modified" as opposed to "the pointer can't be modified"). so `const int * __restrict volatile` would be an integer with a single pointer qualifier of the form `CONSTANT|VOLATILE|RESTRICT`. If your type isn't a pointer this can be NULL. The array is terminated with the END constant. 
* unsigned long flags - Flags for the rest of the auxilliary type data. The options are. 
  * LVALUE_REF_BITMASK
  * RVALUE_REF_BITMASK
  * IDENTIFIER_BITMASK - marks that the type isn't primitive. 
  * COMPLEX_BITMASK - Note: Only for C style complex numbers, C++ style complex numbers are just templates
  * IMAGINARY_BITMASK
* BumpAllocator * alloc 

### createMethodIdentifierData
Once you have a type you need a method identifier. 
* const char * id - name of function
* const char ** nests - array of namespaces and classes the function is defined in from outermost to innermost, Terminated by a NULL.
* size_t arg_n - number of arguments the function takes.
* const TypeIdentifier * args - types of arguments. 
* BumpAllocator * alloc
### createGlobalIdentifierData
Creates a global variable. The type of a global variable isn't mangled so it's not inputted. 
* const char * id - name of the variable
* const char *const * nests - namespace/class in which the variable is defined. For classes this only applies to Static variables. 
* BumpAllocator * alloc
### createFunctionPtrTypeId_
C Function pointer type. 
* const TypeIdentifier * return - singleton pointer for return type. 
* const char ** nests - array of namespaces and classes the function is defined in from outermost to innermost, Terminated by a NULL.
* size_t arg_n - number of arguments the function takes.
* const TypeIdentifier * args - types of arguments. 
* BumpAllocator * alloc
### createSpecialMethodIdentifierData
Creates a special method. That is a constructor, a destructor or an operator.
* SPECIAL_METHOD tag - enum for special method. 
* const char ** nests - array of namespaces and classes the function is defined in from outermost to innermost, Terminated by a NULL.
* size_t arg_n - number of arguments the function takes.
* const TypeIdentifier * args - types of arguments. 
* BumpAllocator * alloc