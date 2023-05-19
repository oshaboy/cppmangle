## BumpAllocator 
Before you do anything with name mangling you have to create a BumpAllocator. I am using a BumpAllocator because my structures are filled with little dynamically allocated bits that are hard to keep track of. You can easily create one using 
```
BumpAllocator allocator={0};
```
From there you can allocate using `bump_alloc(BumpAllocator *, size_t)` and free all the memory associated with it using `bump_free(BumpAllocator *)`

## Functions
### CreateTypeId
To create a type you use `createTypeId` it takes 5 arguments. 
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

### createGlobalIdentifierData

### createFunctionPtrTypeId

