#pragma once

// Macro file for defining the API and calling conventions for the FlexEngine library.

#ifdef __FLX_DISABLED // _WIN32

  #pragma warning(disable: 4251) // Disable warning about exporting STL classes in DLLs

  // This specifies that the called function removes its parameters from the stack before returning.
  // This is the calling convention used by the WinAPI functions.
  //#define __FLX_WINAPI __stdcall

  // Remember to define FLX_BUILD_DLL in the project settings for the FlexEngine library.
  #ifdef FLX_BUILD_DLL
    // Mark symbols for export in a DLL on Windows platforms
    #define __FLX_API __declspec(dllexport)
  #else
    #define __FLX_API __declspec(dllimport)
  #endif

#else

  //#define __FLX_WINAPI
  #define __FLX_API

#endif

// Optimize function calling conventions for vector operations and other performance-sensitive code.
// It is designed to optimize the passing of arguments in functions that use vector or
// SIMD (Single Instruction, Multiple Data) operations.
//#define __FLX_VECTORCALL __vectorcall

// The compiler ensures that the starting address of the variable is divisible by X.
// 
// Example usage: __FLX_ALIGNAS(16) float myArray[4];
// Each float is 4 bytes, so the array is 16 bytes.
// 
// Proper alignment can improve memory access performance, particularly when dealing with
// vectorized operations, SIMD (Single Instruction, Multiple Data) instructions, or GPU operations.
// 
// Misaligned memory access can result in performance penalties because the CPU or GPU
// may need to perform additional work to handle unaligned access.
#define __FLX_ALIGNAS(X) alignas(X)
