
cppmangle: A library that implements Itanium C++ Name Mangling in Pure C

### reasoning
`extern "C"` kinda sucks. It doesn't work with classes, only goes one way and if the library doesn't have any extern "C" methods you're kinda screwed.

The reason you need `extern "C"` at all is because C++ mangles the names. And despite compilers having a "demangle" builtin they don't have a "mangle" builtin.

Also even entirely within C++ you're stuck with linking at compile time. Runtime linking is done with the dlopen and dlsym functions in Unix or LoadLibrary and GetProcAddress in Windows. dlsym requires an input in the form of a C string. 
So I made this library specifically to mangle C strings so dlsym can FINALLY interface with C++ natively. Itanium Name Mangling is used by gcc, clang and icc on all architectures. 

This is only an MVP right now and is missing a lot of features and is probably quite buggy.  

### implemented features
* methods, both nested and unnested.  
* support for most existing C types and function signatures. Basically feature parity with `extern "C"`. 
* Demangle string into MethodIdentifierData

### future ideas
* some common C++ types in the form of opaque types as a header file. 
* templates, templates require some knowledge of the contents of a C++ header file so they might not be possible. 
* other name mangling schemes (Such as MSVC or Rust)
