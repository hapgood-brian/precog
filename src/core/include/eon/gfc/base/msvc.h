//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

/** \addtogroup engine
  * @{
  *   \addtogroup base
  *   @{
  *     \addtogroup compiler
  *     @{
  */

//================================================+=============================
//Detect platform:{                               |

  #ifdef _MSC_VER

    #ifdef __cplusplus
      extern"C"{
    #endif
      __declspec(dllimport)void __stdcall DebugBreak();
    #ifdef __cplusplus
      }
    #endif
    #define __PRETTY_FUNCTION__ __FUNCTION__
    #include<memory.h>
    #include<stdarg.h>
    #include<stdio.h>
    #ifdef min
    #undef min
    #endif
    #ifdef max
    #undef max
    #endif

    //--------------------------------------------+-----------------------------
    //Disabled warnings:{                         |
      #pragma warning( disable:4251 )
      //unexpected tokens following #
      #pragma warning( disable:4067 )
      //possible loss of data
      #pragma warning(disable:4244)
      //nonstandard extension used : nameless struct/union
      #pragma warning(disable:4201)
      //unreferenced inline function has been removed
      #pragma warning(disable:4514)
      //no function prototype given: converting '()' to '()'
      #pragma warning(disable:4255)
      //'this' : used in base member initializer list
      #pragma warning(disable:4355)
      //Unsafe function or variable(_CRT_SECURE_NO_WARNINGS)
      #pragma warning(disable:4996)
      //C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
      #pragma warning(disable:4290)
      //unknown pragma
      #pragma warning(disable:4068)
      //no suitable definition provided for explicit template instantiation request
      #pragma warning(disable:4661)
    //}:                                          |
    //Intrinsic SSE headers:{                     |

      #include<intrin.h>

    //}:                                          |
    //Platform types:{                            |
      //e_declptrs:{                              |

        #ifdef __cplusplus

          /** \brief Engine ptr type definition macro.
            *
            * The e_declptrs macro is used to define pointer types with the right
            * naming convention.
            *
            * \param x The one letter name of the type.
            * \param y The base type.
            */

        #define e_declptrs( x, y )  \
          typedef const y* c##x##p; \
          typedef y* x##p
          namespace EON{
            e_declptrs( l, long );
            e_declptrs( c, char );
            e_declptrs( i, int  );
            e_declptrs( v, void );
          }
        #endif

      //}:                                        |
      //e_declints:{                              |

        #ifdef __cplusplus

          /** \brief Engine integer type definition macro.
            *
            * The e_declints macro is used to define pointer types with the right
            * naming convention.
            *
            * \param x The name of the type.
            * \param y The base type.
            */

          #define e_declints( bits ) \
            typedef   signed __int##bits s##bits; \
            typedef unsigned __int##bits u##bits
          namespace EON{
            e_declints( 64 );
            e_declints( 32 );
            e_declints( 16 );
            e_declints(  8 );
          }

        #endif

      //}:                                        |
      //__thread:{                                |

        #define __thread __declspec( thread )

      //}:                                        |
    //}:                                          |
    //Compiler:{                                  |
      //Graphics:{                                |

        #define __compiling_directx__ 0
        #define __compiling_vulkan__  1
        #define __compiling_opengl__  0
        #define __compiling_glsl__                                              \
        (!__compiling_directx__)&&(__compiling_vulkan__||__compiling_opengl__)  \

      //}:                                        |
      //Vendor:{                                  |

        #define __compiling_win64__ 1
        #define __compiling_pc__    1

      //}:                                        |
      //Packed:{                                  |

        #ifndef e_packed
        #define e_packed(T,...)                                                 \
          __pragma( pack( push, 1 ))struct T{__VA_ARGS__;}                      \
          __pragma( pack( pop ))
        #endif

      //}:                                        |
    //}:                                          |
    //Export:{                                    |

      #if e_compiling( enable_dll )
        #if e_compiling( engine_dll )
          #define E_PUBLISH __declspec( dllexport )
          #define E_REFLECT __declspec( dllexport )
          #define E_EXPORT  __declspec( dllexport )
          #define E_IMPORT  __declspec( dllimport )
        #else
          #define E_PUBLISH __declspec( dllimport )
          #define E_REFLECT
          #define E_EXPORT
          #define E_IMPORT
        #endif
      #else
        #define E_PUBLISH
        #define E_REFLECT
        #define E_EXPORT
        #define E_IMPORT
      #endif

    //}:                                          |
    //--------------------------------------------+-----------------------------

  #endif

//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
