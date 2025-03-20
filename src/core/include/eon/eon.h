//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

#define MAX_SCRATCH_MEMORY 1048576
#define MAX_SHARING_MEMORY 65536

//------------------------------------------------------------------------------
// Warnings for clangs, gnus and microsofties everywhere.
//------------------------------------------------------------------------------

#ifdef __clang__
  #ifdef __APPLE__
    #pragma clang diagnostic ignored "-Wquoted-include-in-framework-header"
  #endif
  #pragma clang diagnostic ignored "-Wundefined-var-template"
#elif defined __GCC__
  #pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

//================================================+=============================
//C++:{                                           |

  #ifdef __cplusplus
    #if __cplusplus >= 202002L
      #define __compiling_cpp20__ 1
      #include<type_traits>
    #elif __cplusplus >= 201703L
      #define __compiling_cpp17__ 1
    #elif __cplusplus >= 201500L
      #define __compiling_cpp14__ 1
    #elif __cplusplus >= 199711L
      #define __compiling_cpp11__ 1
    #endif
  #endif

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Commonality:{                                   |
  //compiler:{                                    |
    //extlib:{                                    |
      //Boost:{                                   |

        #ifdef __cplusplus
          #include<boost/type_index.hpp>
          #include<boost/utility.hpp>
          #define BOOST_BIND_GLOBAL_PLACEHOLDERS
          #include<boost/bind.hpp>
        #endif

      //}:                                        |
      //C++:{                                     |

        #ifdef __cplusplus
          #include<unordered_map>
          #include<functional>
          #include<algorithm>
          #include<variant>
          #include<memory>
          #include<thread>
          #include<atomic>
          #include<vector>
          #include<array>
          #include<map>
        #endif

      //}:                                        |
      //C:{                                       |

        #include<stdlib.h>
        #include<math.h>

      //}:                                        |
    //}:                                          |
    //common:{                                    |

      // Includes important e_var_* macros.
      #include"gfc/base/common.h"

    //}:                                          |
    //llvm:{                                      |

      // For iOS/macOS and Linux (including ndk)
      #include"gfc/base/llvm.h"

    //}:                                          |
    //msvc:{                                      |

      // Define basic types like u32 and macros like e_compiling( debug )etc.
      #include"gfc/base/msvc.h"

    //}:                                          |
    //gcc:{                                       |

      // Includes NDK support on Android.
      #include"gfc/base/gcc.h"

    //}:                                          |
  //}:                                            |
  //build:{                                       |
    //release:{                                   |

      #if !e_compiling( debug )
        #include"gfc/base/release.h"
      #endif

    //}:                                          |
    //debug:{                                     |

      #include"gfc/base/debug.h"

    //}:                                          |
  //}:                                            |
  //other:{                                       |
    //Memory:{                                    |

      #include"gfc/mem.h"

    //}:                                          |
    //Half:{                                      |

      // Define the half type (copied from OpenEXR).
      #ifdef __cplusplus
        #include"gfc/base/half.h"
      #endif

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Foundation:{                                    |

  #ifdef __cplusplus
    namespace EON{
      enum class CatmullRomAlpha:u32{
        kCentripetal = 500,
        kStandard    = 0,
        kChordal     = 1000,
      };
    }
  #endif

//}:                                              |
//Templates:{                                     |

  #ifdef __cplusplus
    template<typename T> e_forceinline T e_alignof( const T& value, const T& pow2 ){
      return(( value+( pow2-1 )) & ~( pow2-1 ));
    }
  #endif

//}:                                              |
//Prototypes:{                                    |

  #ifdef __cplusplus

    namespace EON{
      namespace gfc{
        struct String;
        using string = String;
      }
    }

    /** \brief Test if main thread is current.
      *
      * This routine will test if the main thread is current. This API exists
      * outside the regular threading class, which it probably shouldn't, and so,
      * is accessible early on in the inclusion chain. That is, headers that are
      * included by eon.h before threads.h.
      *
      * \return Returns true if called from the main thread and false otherwise.
      */

    bool e_isMainThread();

    /** \brief Get package path.
      *
      * This call will return the current engine package path or location where
      * the .swordlight directory [package] is. This works on all platforms with
      * the exact same formatting for the path: always Linux style even on
      * Windows.
      */

    const EON::gfc::string e_packagePath();

    /** \brief Get environment variable.
      *
      * This routine is a cross platform version of getenv(). It is necessary for
      * us because Microsoft prefers the secure getenv_s function and will issue
      * a warning if we use getenv(). I'd prefer to not turn off security CRT
      * warnings myself so I'm implementing both the Unix and Windows ways under
      * the umbrella of this one function.
      *
      * \param pEnv The name of the environment variable to load.
      *
      * \return Returns a string object containing the value of envvar. If the
      * environment variable couldn't be found the string will be empty.
      */

    EON::gfc::string e_getenv( EON::ccp pEnv );

    /** \brief Get eon path.
      *
      * This function will return a standard EON gfc string to the current path
      * to all eon files for a platform. For OSX for example this will be .osx.
      */

    const EON::gfc::string e_eonPath();

  #endif

//}:                                              |
//Namespaces:{                                    |
  //Protos:{                                      |

    #ifdef __cplusplus

      /** \brief Top-most library namespace.
        *
        * This namespace contains all types and sub-namespaces for the engine.
        */

      namespace EON{

        /** \brief Game Foundation Classes
          *
          * This namespace contains all structures, types and unions relating
          * to the game gfc classes (gfc). All constructs of this genus have a
          * direct bearing on every game activity from storage to threading.
          * The gfc namespace can be considered a low level game API.
          */

        namespace gfc{
          template<typename T, u32 N> struct array;
          struct Stream;
          struct Object;
        }
      }

    #endif

  //}:                                            |
  //gfc:{                                         |

    #ifdef __cplusplus

      //------------------------------------------------------------------------
      // Engine utilities.
      //------------------------------------------------------------------------

      /** \brief Get the number of elements in an array.
        *
        * This routine simply returns the number of element slots in an array.
        */

      template<typename T,EON::u32 N> constexpr EON::u32 e_sizeof( const EON::gfc::array<T,N>& array )noexcept{
        return array.size();
      }

      /** \brief Get the number of elements in an array.
        *
        * This routine simply returns the number of element slots in an array.
        */

      template<typename T,std::size_t N> constexpr EON::u32 e_sizeof( T const (&)[ N ])noexcept{
        return EON::u32( N );
      }

      /** \brief Get path from type.
        *
        * This routine will construct a string from the type provided by
        * walking up the inheritance chain and also by examining inside classes
        * such as Animation::Set, for example.
        *
        * \param subattr An additional subdirectory to append to the end of the
        * new path such as e_pathof<Mesh>( "texture" ) which means look in the
        * {package}/src/data/object/stream/mesh/texture directory for textures.
        *
        * \return Returns a path that best represents where we should save data
        * that the compiler should read when building .eon files and fablets.
        */

      E_PUBLISH EON::gfc::string e_pathof( const EON::u64 clsid, EON::ccp subattr=nullptr );

      /** \brief Get the current when time.
        *
        * This routine is simply a pass through to Object::gameTurn and it will
        * return that value when called. The reason for this routine's
        * existence is so the class factory can obtain the "when" without
        * knowing anything about the engine object.
        *
        * \return Returns the current 64-bit game turn.
        */

      E_PUBLISH EON::u64 e_when();

      /** \brief Threading incremental backoff.
        *
        * This routine will exponentially block.
        */

      E_PUBLISH void e_backoff( const double start );

      //------------------------------------------------------------------------
      // Special global timing functions required by bitbucket container.
      //------------------------------------------------------------------------

      #include"gfc/times.h"

      //------------------------------------------------------------------------
      // GFC basis headers.
      //------------------------------------------------------------------------

      #include"gfc/atomic.h"
      #include"gfc/buffer.h"
      #include"gfc/container.h"
      #include"gfc/thread.h"
      #include"gfc/mutex.h"
      #include"gfc/scratch.h"
      #include"gfc/object.h"
      #include"gfc/strings.h"
      #include"gfc/profiler.h"

      //------------------------------------------------------------------------
      // String operators.
      //------------------------------------------------------------------------

      /** \brief Global string concatenation operator.
        *
        * This global operator will let you append a gfc string to a C string
        * and return the result as a gfc string.
        *
        * \param a A null terminated C string to be upcast to a gfc string and
        * prepended to b.
        *
        * \param b A gfc string to append to the upcast C string 'a'.
        * \return Returns a new gfc string; the result of the concatenation.
        */

      e_forceinline EON::gfc::string operator+(
            EON::ccp a
          , const EON::gfc::string& b ){
        return EON::gfc::string( a ) + b;
      }

      //------------------------------------------------------------------------
      // Math libraries (part of gfc just not in namespace for easy typing).
      //------------------------------------------------------------------------

      #include"gfc/maths.h"

      //------------------------------------------------------------------------
      // Engine RTTI.
      //------------------------------------------------------------------------

      /** \name Class identification methods.
        *
        * These methods let you query a type and get its pretty name or it's
        * engine specific type identifier, etc.
        *
        * @{
        */

      /** \brief Get type pretty name.
        *
        * This routine will return the type's pretty name using boost.
        *
        * \tparam T The type to retrieve the pretty name for.
        *
        * \return Returns a ccp to the pretty name.
        */

      template<typename T> EON::gfc::string e_classof(){
        return boost::typeindex::type_id<T>().pretty_name().c_str();
      }

      /** \brief Get type pretty name.
        *
        * This routine will return the type's pretty name using boost.
        *
        * \tparam T The type to retrieve the pretty name for.
        *
        * \return Returns a ccp to the pretty name.
        */

      template<typename T> e_forceinline EON::gfc::string e_classof( const T& ){
        return e_classof<T>();
      }

      /** \brief Get class identifier for given type.
        *
        * A class identifier, or classid/clsid for short, is the basic type in
        * the EON reflection system. I tried using the typeid( T ).hash_code()
        * method but the value was not invariant across builds! Thus I had to
        * roll my own 64-it hash of typeid( T ).name(), which is of course what
        * the classid is. We use C++s RTTI in this way, though it bloats the
        * app, because it makes certain guarantees such as the name being fully
        * decorated. For example the type Object is just "Object" if you use
        * just the Object::classname() mechanism when the actually type name is
        * far longer: "EON::gfc::Object" instead.
        *
        * \tparam T The type to generate a class identifier for.
        *
        * \return Returns the 64-bit hash of the given type's name.
        */

      template<typename T> e_noinline EON::u64 e_classid(){
        static EON::u64 clsid = e_classof<T>().hash();
        return clsid;
      }

      /** \brief Get class identifier from variable type.
        *
        * This function is identical to e_classid<T>() in that it returns the
        * class of type T. The only difference is that the type is derived from
        * a argument you pass into the function so you don't have to specify
        * <T> when you call it.
        *
        * \tparam T The type to generate a class identifier for.
        *
        * \return Returns the 64-bit hash of the given type's name.
        */

      template<typename T> e_noinline EON::u64 e_classid( const T& ){
        static EON::u64 clsid = e_classof<T>().hash();
        return clsid;
      }

      /** \brief Get class identifier from variable type.
        *
        * This function is identical to e_classid<T>() in that it returns the
        * class of type T. The only difference is that the type is derived from
        * a argument you pass into the function so you don't have to specify
        * <T> when you call it. This version of the function takes a const
        * rvalue reference, one of the few cases where a const rvalue& is used.
        *
        * \tparam T The type to generate a class identifier for.
        *
        * \return Returns the 64-bit hash of the given type's name.
        */

      template<typename T> e_noinline EON::u64 e_classid( const T&& ){
        static EON::u64 clsid = e_classof<T>().hash();
        return clsid;
      }

      /** @}
        *
        * \name Plugin interface.
        *
        * This class defines the plugin interface.
        *
        * @{
        */

      namespace EON{
        namespace gfc{
          struct IPlugin{
            virtual int main( const strings& args )=0;
            static int spawn( const strings& args );
            IPlugin( const string& name );
            virtual~IPlugin();
          private:
            e_var_string( Name );
          };
        }
      }

      /** @}
        *
        * \name Singleton interface.
        *
        * These methods, classes and macros let define the dylib/so/dll safe
        * singleton interface. Simply use the e_singleton interface at the end
        * of this block.
        *
        * @{
        */

      /** \brief Singleton function.
        *
        * This routine will allocate a singleton and return an anonymous pointer
        * to it for use by the e_singleton class and macro.
        *
        * \param _sizeof The size of the structure to allocate and store.
        * \param clsid Class identifier used to store pointer forever.
        */

      E_PUBLISH const std::pair<bool,EON::vp> e_singleton( const std::size_t _sizeof, const EON::u64 clsid );

      /** \brief Singleton interface.
        *
        * This interface allows one to easily construct singletons. It has a very
        * important property: it's shared library safe.
        */

      namespace EON{
        namespace gfc{
          template<typename T> struct ISingleton{
            template<typename...A> static T& get( A...args ){
              auto singleton = e_singleton( sizeof( T ), e_classid<T>() );
              if( !singleton.first ){
                new( singleton.second )T( std::forward<A>( args )... );
              }
              return *reinterpret_cast<T*>( singleton.second );
            }
          };
        }
      }

      /** \brief Singleton macro.
        *
        * This macro is used to easily get a singleton with less typing.
        */

      #define e_singleton(T,...)                                                \
        EON::gfc::ISingleton<T>::get(__VA_ARGS__)                               \

      #include"gfc/base/delegate.h"

      /** @}
        *
        * \name Type queries: 'isa'.
        *
        * These methods let you query whether an instance of an object IS a type
        * of a particular class as specified in the template argument T.
        *
        * @{
        */

      /** \brief Test if type "is a" member of another type.
        *
        * This function will test whether the given type is part of the class
        * inheritance chain.
        *
        * \tparam T The type to check for.
        * \param pObject The object derived to class to check on.
        */

      template<typename T, typename O> e_forceinline bool e_isa( const O* pObject ){
        return( pObject?((EON::gfc::Object*)pObject)->isa( e_classid<T>() ):false );
      }

      /** @} */

      //------------------------------------------------------------------------
      // Special templated pathing API reliant on gfc reflection and strings.
      //------------------------------------------------------------------------

      /** \brief Get path from type.
        *
        * This routine will construct a string from the type provided by
        * walking up the inheritance chain and also by examining inside classes
        * such as Animation::Set, for example.
        *
        * \param subattr An additional subdirectory to append to the end of the
        * new path such as e_pathof<Mesh>( "texture" ) which means look in the
        * {package}/src/data/object/stream/mesh/texture directory for textures.
        *
        * \return Returns a path that best represents where we should save data
        * that the compiler should read when building .eon files and fablets.
        */

      template<typename T> e_noinline EON::gfc::string e_pathof( EON::ccp subattr=nullptr ){
        return e_pathof( e_classid<T>(), subattr );
      }

      //------------------------------------------------------------------------
      // Prototypes.
      //------------------------------------------------------------------------

      template<typename T> typename T::handle e_uniqueAsyncStream(
          const EON::gfc::string& streamId
        , const std::function<void( T& )>& lambda = nullptr );
      template<typename T> EON::gfc::string e_save(
          const EON::gfc::string& in_onlyPath
        , typename std::remove_const<T>::type& stream
        , const EON::u32 flags
      );

      //------------------------------------------------------------------------
      // Object consumers.
      //------------------------------------------------------------------------

      #include"gfc/class.h"
      #include"gfc/property.h"
      #include"gfc/autoref.h"
      #include"gfc/resource.h"
      #include"gfc/history.h"
      #include"gfc/fs.h"
      #include"gfc/fablet.h"
      #include"gfc/serialize.h"

      //------------------------------------------------------------------------
      // Attribute naming via object type.
      //------------------------------------------------------------------------

      /** \brief Get the attribute name for the gifven class identifier.
        *
        * This routine will find the underyling Class<> object represented by
        * the class identifier 'clsid' and return a string that best describes
        * the inheritance chain. This is used extensively in the red compiler.
        *
        * \param clsid The class identifier for the type you want to get the
        * attribute name for.
        *
        * \param subattr Thought of as a subdirectory under the main type,
        * think in terms of 'sprite' which is subordinate to 'texture' but
        * which is not an actual type. A pseudo type if you will.
        *
        * \return Returns the attribute name in camel case form.
        */

      E_PUBLISH EON::gfc::string e_attrof( const EON::u64 clsid, EON::ccp subattr=nullptr );

      /** \brief Get the attribute name from the given template argument T.
        *
        * This routine will call the general e_attrof() with the class
        * identifier of type 'T'.
        *
        * \tparam T The type to query the class identifier from by calling into
        * e_classid<>.
        *
        * \param subattr Thought of as a subdirectory under the main type,
        * think in terms of 'sprite' which is subordinate to 'texture' but
        * which is not an actual type. A pseudo type if you will.
        *
        * \return Returns the attribute name in camel case form.
        */

      template<typename T> e_forceinline EON::gfc::string e_attrof( EON::ccp subattr=nullptr ){
        return e_attrof( e_classid<T>(), subattr );
      }

      //------------------------------------------------------------------------
      // The GFC component model.
      //------------------------------------------------------------------------

      #include"gfc/visual.h"
      #include"gfc/input.h"

      //------------------------------------------------------------------------
      // The metric value to string template.
      //------------------------------------------------------------------------

      /** \brief Get SSI (mass) decorated string from a real mass value.
        *
        * This routine will convert the floating point value specified in
        * "value" and convert it to an SSI formatted string with an emphasis on
        * mass suffixes. For example, if we passed in a value of 1000.0 the
        * string returned would be "1kg".
        *
        * \param value A EON::Maths<T> value containing a real value we want to
        * convert to a SSI mass modified string.
        *
        * \return Returns a string with the converted value. If the input mass
        * is zero then no decoration will have taken place.
        */

      template<typename T> e_noinline EON::gfc::string e_ssiof_mass( const T& value ){
        if( value == T::k0 ){
          // Don't decorate zero.
          return "0g";
        }
        using namespace EON;
        using namespace gfc;
        EON::gfc::string s;
        T f = value.abs();
        // Figure out which ssi suffix to add.
        if(( f > T::k0 )&&( f < T::kg )){
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldg", s64( i ));
          }else{
            s.catf( "%.3fg", typename T::cast( f ));
          }
        }else if(( f >= T::kg )&&( f < T::t )){
          f /= T::kg;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldkg", s64( i ));
          }else{
            s.catf( "%.3fkg", typename T::cast( f ));
          }
        }else if(( f >= T::t )&&( f < T::kt )){
          f /= T::t;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldt", s64( i ));
          }else{
            s.catf( "%.3ft", typename T::cast( f ));
          }
        }else if(( f >= T::kt )&&( f < T::mt )){
          f /= T::kt;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldt", s64( i ));
          }else{
            s.catf( "%.3ft", typename T::cast( f ));
          }
        }else if( f >= T::mt ){
          f /= T::mt;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldt", s64( i ));
          }else{
            s.catf( "%.3ft", typename T::cast( f ));
          }
        }
        if( value < T::k0 ){
          return "-" + s;
        }
        return s;
      }

      /** \brief Get SSI decorated string from a real value.
        *
        * This routine will convert the given floating point value to a string
        * and decorate it with SSI symbols. For example, if the value was
        * 1000.f then the returned string would be "1km". If the value was
        * "100.f" then the returned string would be "1m". And of course
        * anything lower than 100.f will be converted to "?cm" except when the
        * value is < 1.f where it will be decoarated as "mm" or millimetres.
        *
        * All conversions from real to SSI decorated string are done in the
        * engine's scale of 1 unit = 1cm.
        *
        * This routine is especially useful when used by the property system
        * due to the fact that users can type in 1mm or 1cm or 1kn in the
        * coordinate frame fields of a L2W QST transform and let it
        * automatically convert to a proper real number when written to the L2W
        * property. This makes it easy to read and very user friendly. Taken a
        * page out of the Modo book.
        *
        * \param value A EON::Maths<T> value containing a real value we want to
        * convert to a SSI decorated string.
        *
        * \return Returns a string with the converted value. If the input real
        * is zero then no decoration will have taken place.
        */

      template<typename T> e_noinline EON::gfc::string e_ssiof( const T& value ){
        if( value == T::k0 ){
          // Don't decorate zero.
          return "0cm";
        }
        using namespace EON;
        using namespace gfc;
        EON::gfc::string s;
        T f = value.abs();
        // Figure out which ssi suffix to add.
        if( f < T::cm ){
          f /= T::mm;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldmm", s64( i ));
          }else{
            s.catf( "%.3fmm", typename T::cast( f ));
          }
        }else if(( f >= T::cm )&&( f < T::m )){
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldcm", s64( i ));
          }else{
            s.catf( "%.3fcm", typename T::cast( f ));
          }
        }else if(( f >= T::m )&&( f < T::dm )){
          f /= T::m;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldm", s64( i ));
          }else{
            s.catf( "%.3fm", typename T::cast( f ));
          }
        }else if(( f >= T::dm )&&( f < T::km )){
          f /= T::dm;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%llddm", s64( i ));
          }else{
            s.catf( "%.3fdm", typename T::cast( f ));
          }
        }else if(( f >= T::km )&&( f < T::Mm )){
          f /= T::km;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldkm", s64( i ));
          }else{
            s.catf( "%.3fkm", typename T::cast( f ));
          }
        }else{
          f /= T::Mm;
          T i;
          if( f.frac( i ) == T::k0 ){
            s.catf( "%lldMm", s64( i ));
          }else{
            s.catf( "%.3fMm", typename T::cast( f ));
          }
        }
        if( value < T::k0 ){
          return "-" + s;
        }
        return s;
      }

      /** \brief Split string function.
        *
        * This routine will split a C string into two parts given a character. It
        * is useful in certain circumstances.
        *
        * \param inout_L Input string and receives result.
        *
        * \param at The character to split at.
        *
        * \param out_R The right hand side of the split point.
        *
        * \return Returns true if the split point was found and false otherwise.
        */

      E_PUBLISH bool e_strsplit( EON::cp& inout_L, const char at, EON::cp& out_R );

      /** \brief The string to value function.
        *
        * This routine will convert a GFC string to a value pointed to by the
        * anonymous pointer pData. The code understands which type to convert to
        * via the class identifier clsid.
        \code
          f32 f;
          const u64 clsid = e_classid<f32>();
          e_strto( "1", e_classid( f, clsid, &f ),
        \endcode
        * \param value The string value you want to store.
        *
        * \param clsid The class identifier.
        *
        * \param pData The memory location you wish to store the value in.
        */

      E_PUBLISH void e_strto( const EON::gfc::string& value, const EON::u64 clsid, EON::vp pData );

      /** \brief The string-of template.
        *
        * This is the antithesis of e_strto. It takes an object and returns a
        * string of the value in obj.
        *
        * \param obj The object you want to turn into a string.
        *
        * \return Returns a string containing the value in obj. If the type is
        * unknown via e_classid<> then "???" is returned.
        */

      template<typename T> e_noinline EON::gfc::string e_strof( const T& obj ){
        using namespace EON;
        const u64 clsid = e_classid<T>();

        //----------------------------------------------------------------------
        // Metric real numbers.
        //----------------------------------------------------------------------

        if(( clsid == e_classid<ssi_mass<EON::f64>>() )||( clsid == e_classid<ssi_mass<EON::f64::Value>>() )){
          return e_ssiof_mass( reinterpret_cast<const EON::f64&>( obj ));
        }
        if(( clsid == e_classid<ssi_mass<EON::f32>>() )||( clsid == e_classid<ssi_mass<EON::f32::Value>>() )){
          return e_ssiof_mass( reinterpret_cast<const EON::f32&>( obj ));
        }
        if(( clsid == e_classid<ssi<EON::f64>>() )||( clsid == e_classid<ssi<EON::f64::Value>>() )){
          return e_ssiof( reinterpret_cast<const EON::f64&>( obj ));
        }
        if(( clsid == e_classid<ssi<EON::f32>>() )||( clsid == e_classid<ssi<EON::f32::Value>>() )){
          return e_ssiof( reinterpret_cast<const EON::f32&>( obj ));
        }

        //----------------------------------------------------------------------
        // Real numbers.
        //----------------------------------------------------------------------

        if(( clsid == e_classid<double>() )||( clsid == e_classid<EON::f64>() )||( clsid == e_classid<EON::f64::Value>() )){
          const EON::f64& f = reinterpret_cast<const double&>( obj );
          EON::f64 i;
          if( f.frac( i ) == 0. ){
            return e_xfs( "%lld", EON::s64( i ));
          }
          return e_xfs( "%.6f", double( f ));
        }
        if(( clsid == e_classid<float>() )||( clsid == e_classid<EON::f32>() )||( clsid == e_classid<EON::f32::Value>() )){
          const EON::f32& f = reinterpret_cast<const EON::f32&>( obj );
          EON::f32 i;
          if( f.frac( i ) == 0.f ){
            return e_xfs( "%d", EON::s32( i ));
          }
          return e_xfs( "%.6f", EON::f32::cast( f ));
        }
        if(( clsid == e_classid<EON::f16>() )||( clsid == e_classid<half>() )||( clsid == e_classid<EON::f16::Value>() )){
          const EON::f16& f = reinterpret_cast<const EON::f16&>( obj );
          return e_xfs( "%.6f", EON::f32::cast( f ));
        }

        //----------------------------------------------------------------------
        // Boolean.
        //----------------------------------------------------------------------

        if( clsid == e_classid<bool>() ){
          const bool b = reinterpret_cast<const bool&>( obj );
          return b ? "True" : "False";
        }

        //----------------------------------------------------------------------
        // Integers.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::u64>() ){
          const EON::u64 u = reinterpret_cast<const EON::u64&>( obj );
          return e_xfs( "%llu", u );
        }
        if( clsid == e_classid<EON::s64>() ){
          const EON::s64 s = reinterpret_cast<const EON::s64&>( obj );
          return e_xfs( "%lld", s );
        }
        if( clsid == e_classid<EON::u32>() ){
          const EON::u32 u = reinterpret_cast<const EON::u32&>( obj );
          return e_xfs( "%u", u );
        }
        if( clsid == e_classid<EON::s32>() ){
          const EON::s32 s = reinterpret_cast<const EON::s32&>( obj );
          return e_xfs( "%d", s );
        }
        if( clsid == e_classid<EON::u16>() ){
          const EON::u32 u = reinterpret_cast<const EON::u16&>( obj );
          return e_xfs( "%u", u );
        }
        if( clsid == e_classid<EON::s16>() ){
          const EON::s32 s = reinterpret_cast<const EON::s16&>( obj );
          return e_xfs( "%d", s );
        }
        if( clsid == e_classid<EON::u8>() ){
          const EON::u32 u = reinterpret_cast<const EON::u8&>( obj );
          return e_xfs( "%u", u );
        }
        if( clsid == e_classid<EON::s8>() ){
          const EON::s32 s = reinterpret_cast<const EON::s8&>( obj );
          return e_xfs( "%d", s );
        }

        //----------------------------------------------------------------------
        // Strings.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::gfc::string>() ){
          const EON::gfc::string& s = reinterpret_cast<const EON::gfc::string&>( obj );
          return s;
        }

        //----------------------------------------------------------------------
        // Transforms.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::qst3>() ){
          const EON::qst3& L2W = reinterpret_cast<const EON::qst3&>( obj );
          EON::gfc::string s;
          s.catf( "%.3f", EON::f32::cast( L2W[ 0 ]));
          for( EON::u32 i=1; i<7; ++i ){
            s.catf( "    %.3f", EON::f32::cast( L2W[ i ]));
          }
          return s;
        }

        if( clsid == e_classid<EON::vec4x4>() ){
          const EON::vec4x4& m = reinterpret_cast<const EON::vec4x4&>( obj );
          EON::gfc::string s;
          s.catf( "%.3f", f32::cast( m[ 0 ]));
          for( u32 i=1; i<16; ++i ){
            s.catf( "    %.3f", f32::cast( m[ i ]));
          }
          return s;
        }

        //----------------------------------------------------------------------
        // Metric vector and points.
        //----------------------------------------------------------------------

        // Standard values in standard metric mass form.
        if( clsid == e_classid<ssi_mass<EON::vec4d>>() ){
          const EON::vec4d& v = reinterpret_cast<const EON::vec4d&>( obj );
          return e_ssiof_mass( f64( v.x ))+"    "+e_ssiof_mass( f64( v.y ))+"    "+e_ssiof_mass( f64( v.z ))+"    "+e_ssiof_mass( f64( v.w ));
        }
        if( clsid == e_classid<ssi_mass<EON::vec4>>() ){
          const EON::vec4& v = reinterpret_cast<const EON::vec4&>( obj );
          return e_ssiof_mass( f32( v.x ))+"    "+e_ssiof_mass( f32( v.y ))+"    "+e_ssiof_mass( f32( v.z ))+"    "+e_ssiof_mass( f32( v.w ));
        }
        if( clsid == e_classid<ssi_mass<EON::vec3d>>() ){
          const EON::vec3d& v = reinterpret_cast<const EON::vec3d&>( obj );
          return e_ssiof_mass( f64( v.x ))+"    "+e_ssiof_mass( f64( v.y ))+"    "+e_ssiof_mass( f64( v.z ));
        }
        if( clsid == e_classid<ssi_mass<EON::vec3>>() ){
          const EON::vec3& v = reinterpret_cast<const EON::vec3&>( obj );
          return e_ssiof_mass( f32( v.x ))+"    "+e_ssiof_mass( f32( v.y ))+"    "+e_ssiof_mass( f32( v.z ));
        }
        if( clsid == e_classid<ssi_mass<EON::vec2d>>() ){
          const EON::vec2d& v = reinterpret_cast<const EON::vec2d&>( obj );
          return e_ssiof_mass( f64( v.x ))+"    "+e_ssiof_mass( f64( v.y ));
        }
        if( clsid == e_classid<ssi_mass<EON::vec2>>() ){
          const EON::vec2& v = reinterpret_cast<const EON::vec2&>( obj );
          return e_ssiof_mass( f32( v.x ))+"    "+e_ssiof_mass( f32( v.y ));
        }
        if( clsid == e_classid<ssi_mass<EON::pt3d>>() ){
          const EON::pt3d& p = reinterpret_cast<const EON::pt3d&>( obj );
          return e_ssiof_mass( f64( p.x ))+"    "+e_ssiof_mass( f64( p.y ))+"    "+e_ssiof_mass( f64( p.z ));
        }
        if( clsid == e_classid<ssi_mass<EON::pt3>>() ){
          const EON::pt3& p = reinterpret_cast<const EON::pt3&>( obj );
          return e_ssiof_mass( f32( p.x ))+"    "+e_ssiof_mass( f32( p.y ))+"    "+e_ssiof_mass( f32( p.z ));
        }
        if( clsid == e_classid<ssi_mass<EON::pt2d>>() ){
          const EON::pt2d& p = reinterpret_cast<const EON::pt2d&>( obj );
          return e_ssiof_mass( f64( p.x ))+"    "+e_ssiof_mass( f64( p.y ));
        }
        if( clsid == e_classid<ssi_mass<EON::pt2>>() ){
          const EON::pt2& p = reinterpret_cast<const EON::pt2&>( obj );
          return e_ssiof_mass( f32( p.x ))+"    "+e_ssiof_mass( f32( p.y ));
        }

        // Standard values in standard metric form.
        if( clsid == e_classid<ssi<EON::vec4d>>() ){
          const EON::vec4d& v = reinterpret_cast<const EON::vec4d&>( obj );
          return e_ssiof( f64( v.x ))+"    "+e_ssiof( f64( v.y ))+"    "+e_ssiof( f64( v.z ))+"    "+e_ssiof( f64( v.w ));
        }
        if( clsid == e_classid<ssi<EON::vec4>>() ){
          const EON::vec4& v = reinterpret_cast<const EON::vec4&>( obj );
          return e_ssiof( f32( v.x ))+"    "+e_ssiof( f32( v.y ))+"    "+e_ssiof( f32( v.z ))+"    "+e_ssiof( f32( v.w ));
        }
        if( clsid == e_classid<ssi<EON::vec3d>>() ){
          const EON::vec3d& v = reinterpret_cast<const EON::vec3d&>( obj );
          return e_ssiof( f64( v.x ))+"    "+e_ssiof( f64( v.y ))+"    "+e_ssiof( f64( v.z ));
        }
        if( clsid == e_classid<ssi<EON::vec3>>() ){
          const EON::vec3& v = reinterpret_cast<const EON::vec3&>( obj );
          return e_ssiof( f32( v.x ))+"    "+e_ssiof( f32( v.y ))+"    "+e_ssiof( f32( v.z ));
        }
        if( clsid == e_classid<ssi<EON::vec2d>>() ){
          const EON::vec2d& v = reinterpret_cast<const EON::vec2d&>( obj );
          return e_ssiof( f64( v.x ))+"    "+e_ssiof( f64( v.y ));
        }
        if( clsid == e_classid<ssi<EON::vec2>>() ){
          const EON::vec2& v = reinterpret_cast<const EON::vec2&>( obj );
          return e_ssiof( f32( v.x ))+"    "+e_ssiof( f32( v.y ));
        }
        if( clsid == e_classid<ssi<EON::pt3d>>() ){
          const EON::pt3d& p = reinterpret_cast<const EON::pt3d&>( obj );
          return e_ssiof( f64( p.x ))+"    "+e_ssiof( f64( p.y ))+"    "+e_ssiof( f64( p.z ));
        }
        if( clsid == e_classid<ssi<EON::pt3>>() ){
          const EON::pt3& p = reinterpret_cast<const EON::pt3&>( obj );
          return e_ssiof( f32( p.x ))+"    "+e_ssiof( f32( p.y ))+"    "+e_ssiof( f32( p.z ));
        }
        if( clsid == e_classid<ssi<EON::pt2d>>() ){
          const EON::pt2d& p = reinterpret_cast<const EON::pt2d&>( obj );
          return e_ssiof( f64( p.x ))+"    "+e_ssiof( f64( p.y ));
        }
        if( clsid == e_classid<ssi<EON::pt2>>() ){
          const EON::pt2& p = reinterpret_cast<const EON::pt2&>( obj );
          return e_ssiof( f32( p.x ))+"    "+e_ssiof( f32( p.y ));
        }

        //----------------------------------------------------------------------
        // Vectors and points.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::vec4i>() ){
          const EON::vec4i& v = reinterpret_cast<const EON::vec4i&>( obj );
          return e_xfs( "%d    %d    %d    %d", s32( v.x ), s32( v.y ), s32( v.z ), s32( v.w ));
        }
        if( clsid == e_classid<EON::vec4>() ){
          const EON::vec4& v = reinterpret_cast<const EON::vec4&>( obj );
          return e_xfs( "%.3f    %.3f    %.3f    %.3f", f32::cast( v.x ), f32::cast( v.y ), f32::cast( v.z ), f32::cast( v.w ));
        }
        if( clsid == e_classid<EON::vec3i>() ){
          const EON::vec3i& v = reinterpret_cast<const EON::vec3i&>( obj );
          return e_xfs( "%d    %d    %d", s32( v.x ), s32( v.y ), s32( v.z ));
        }
        if( clsid == e_classid<EON::vec3>() ){
          const EON::vec3& v = reinterpret_cast<const EON::vec3&>( obj );
          return e_xfs( "%.3f    %.3f    %.3f", f32::cast( v.x ), f32::cast( v.y ), f32::cast( v.z ));
        }
        if( clsid == e_classid<EON::vec2i>() ){
          const EON::vec2i& v = reinterpret_cast<const EON::vec2i&>( obj );
          return e_xfs( "%d    %d", s32( v.x ), s32( v.y ));
        }
        if( clsid == e_classid<EON::vec2>() ){
          const EON::vec2& v = reinterpret_cast<const EON::vec2&>( obj );
          return e_xfs( "%.3f    %.3f", f32::cast( v.x ), f32::cast( v.y ));
        }
        if( clsid == e_classid<EON::pt3>() ){
          const EON::pt3& p = reinterpret_cast<const EON::pt3&>( obj );
          return e_xfs( "%.3f    %.3f    %.3f", f32::cast( p.x ), f32::cast( p.y ), f32::cast( p.z ));
        }
        if( clsid == e_classid<EON::pt2>() ){
          const EON::pt2& p = reinterpret_cast<const EON::pt2&>( obj );
          return e_xfs( "%.3f    %.3f", f32::cast( p.x ), f32::cast( p.y ));
        }

        //----------------------------------------------------------------------
        // Bounding volumes.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::aabb3>() ){
          const EON::aabb3& b = reinterpret_cast<const EON::aabb3&>( obj );
          EON::gfc::string s;
          s.catf( "%.3f", f32::cast( b[ 0 ]));
          for( u32 i=1; i<6; ++i ){
            s.catf( "    %.3f", f32::cast( b[ i ]));
          }
          return s;
        }
        if( clsid == e_classid<EON::aabb2>() ){
          const EON::aabb2& b = reinterpret_cast<const EON::aabb2&>( obj );
          EON::gfc::string s;
          s.catf( "%.3f", f32::cast( b[ 0 ]));
          for( u32 i=1; i<4; ++i ){
            s.catf( "    %.3f", f32::cast( b[ i ]));
          }
          return s;
        }

        //----------------------------------------------------------------------
        // Rotations.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::quat>() ){
          const EON::quat& q = reinterpret_cast<const EON::quat&>( obj );
          f32 x, y, z;
          q.getEuler( x, y, z );
          return e_xfs( "%.3f    %.3f    %.3f", f32::cast( x ), f32::cast( y ), f32::cast( z ));
        }

        //----------------------------------------------------------------------
        // Colors.
        //----------------------------------------------------------------------

        if( clsid == e_classid<EON::rgba>() ){
          const EON::rgba& c = reinterpret_cast<const EON::rgba&>( obj );
          return e_xfs( "%.3f    %.3f    %.3f    %.3f", f32::cast( c.r ), f32::cast( c.g ), f32::cast( c.b ), f32::cast( c.a ));
        }
        return"???";
      }

    #endif

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Engine:{                                        |

  #ifdef __cplusplus

    #include"gfc/base/engine.h"

    e_forceinline void e_setPackagePath( const EON::gfc::string& path ){
      EON::IEngine::setPackagePath( path );
    }

    e_forceinline bool e_mkdir( const EON::gfc::string& directories ){
      return EON::IEngine::mkdir( directories );
    }

    e_forceinline bool e_md( const EON::gfc::string& directories ){
      return EON::IEngine::md( directories );
    }

    e_forceinline bool e_rd( const EON::gfc::strings& directories ){
      return( 0!=EON::IEngine::rm( directories ));
    }

    e_forceinline bool e_rm( const EON::gfc::string& path ){
      return EON::IEngine::rm( path );
    }

    e_forceinline void e_cp( const EON::gfc::string& from, const EON::gfc::string& to ){
     EON::IEngine::copy( from, to );
    }

    e_forceinline EON::gfc::string e_tmpath(){
      return EON::IEngine::tempPath();
    }

    e_forceinline const EON::gfc::string e_packagePath(){
      return EON::IEngine::toPackagePath();
    }

    e_forceinline const EON::gfc::string e_eonPath(){
      return EON::IEngine::toEonPath();
    }

  #endif

//}:                                              |
//================================================+=============================
