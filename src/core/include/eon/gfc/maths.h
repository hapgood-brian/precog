//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

/** \addtogroup engine
  * @{
  *   \addtogroup gfc
  *   @{
  *     \addtogroup math
  *     @{
  */

//================================================+=============================
//Equate:{                                        |

  /* Balanced for size and performance */

  #define HEX_X_SCALE 10
  #define HEX_Y_SCALE 20
  #define BIOME_COUNT 15
  #define BIOME_SCALE 100
  #define BIOME_ROWS  16

//}:                                              |
//Macros:{                                        |

  #define e_pow2(x) ((x)*(x))
  #define e_pow3(x) ((x)*(x)*(x))
  #define e_math_alias( x, y )                                                  \
    using x##d = f64::y;                                                        \
    using x    = f32::y;                                                        \
    using x##h = f16::y;                                                        \
    using x##l = i64::y;                                                        \
    using x##i = i32::y;                                                        \
    using x##s = i16::y;                                                        \
    using x##b = i8 ::y;                                                        \
    using x##v = gfc::vector<x>                                                 \

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Alias f64/32/16:{                               |

  namespace EON{
    template<typename T> struct Maths;
    using f64 = Maths<double>;
    using f32 = Maths<float>;
    using f16 = Maths<half>;
    using i64 = Maths<s64>;
    using i32 = Maths<s32>;
    using i16 = Maths<s16>;
    using i8  = Maths<s8>;
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Undo ms min/max:{                               |

  #ifdef min
    #undef min
  #endif

  #ifdef max
    #undef max
  #endif

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Forwarding:{                                    |

  template<typename T> e_forceinline EON::Maths<T> operator*( const float x, const EON::Maths<T>& y );
  template<typename T> e_forceinline EON::Maths<T> operator/( const float x, const EON::Maths<T>& y );
  template<typename T> e_forceinline EON::Maths<T> operator+( const float x, const EON::Maths<T>& y );
  template<typename T> e_forceinline EON::Maths<T> operator-( const float x, const EON::Maths<T>& y );

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Optimizations:{                                 |

  #include<math.h>

  #if !e_compiling( web ) && e_compiling( x64 )
    #ifdef __SSE4_1__
      #include<smmintrin.h>
    #elif e_compiling( linux )
      #include<x86intrin.h>
    #else
      #include<xmmintrin.h>
    #endif
    #define _mm_replicate_x_ps(v) \
      _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0, 0, 0, 0))
    #define _mm_replicate_y_ps(v) \
      _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1, 1, 1, 1))
    #define _mm_replicate_z_ps(v) \
      _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2, 2, 2, 2))
    #define _mm_replicate_w_ps(v) \
      _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3, 3, 3, 3))
    #define SHUFFLE_PARAM(x, y, z, w) \
      ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))
    #define _mm_madd_ps(a, b, c) \
      _mm_add_ps(_mm_mul_ps((a), (b)), (c))
  #elif e_compiling( arm )
    #include<arm_neon.h>
  #endif

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Maths class:{                                   |

  namespace EON{

    /** Maths template.
      *
      * The template below is designed to encapsulate the whole math library
      * under one umbrella class. There are only two types supported: double and
      * float; these types are typecast to f64 and f32 respectively.
      */

    template<typename T> struct E_PUBLISH Maths{

      using self = Maths;
      using cast = T;

      //------------------------------------------+-----------------------------
      //Structs:{                                 |

        enum class Type:u32{
          kVoid,
          kFloat,
          kHalf,
          kShort,
          kInt,
          kVec4x4,
          kVec3x3,
          kVec3x4,
          kVec4,
          kVec3,
          kVec2,
          kVec4h,
          kVec3h,
          kVec2h,
          kVec4i,
          kVec3i,
          kVec2i,
          kVec4s,
          kVec3s,
          kVec2s,
          kVec4b,
          kVec3b,
          kVec2b,
          kMRT,
          kLight
        };

        /** \brief A special value.
          *
          * This property type defines a value with a default value, minimum
          * and maximum extents.
          */

        struct E_PUBLISH Value{

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            Value& operator=( const float value ){
              m_fValue = e_clamp( value, m_fMinimum, m_fMaximum );
              return *this;
            }

            Value& operator=( const f32& value ){
              m_fValue = value.clamped( m_fMinimum, m_fMaximum );
              return *this;
            }

            operator f32()const{
              return m_fValue;
            }

          //}:                                    |
          //Methods:{                             |

            e_forceinline bool isDefault()const{
              return( m_fValue == m_fDefault );
            }

            e_forceinline void reset(){
              m_fValue = m_fDefault;
            }

            e_forceinline f32 toValue()const{
              return m_fValue;
            }

            e_noinline void setValue( const self& fValue ){
              if( fValue < m_fMinimum ){
                m_fValue = m_fMinimum;
              }else if( fValue > m_fMaximum ){
                m_fValue = m_fMaximum;
              }else{
                m_fValue = fValue;
              }
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          Value( ccp name, const self& fMinimum, const self& fMaximum, const self& fDefault )
            : m_fMinimum( fMinimum )
            , m_fMaximum( fMaximum )
            , m_fDefault( fDefault )
            , m_fValue(   fDefault )
            , m_pName( name )
          {}

          Value( ccp name )
            : m_pName( name )
          {}

          Value() = default;
        ~ Value() = default;

        private:

          self m_fValue = 0.f;

          e_var( self, f,        Minimum ) = 0.f;
          e_var( self, f,        Maximum ) = 1.f;
          e_var( self, f,        Default ) = 0.f;
          e_var_const_ptr( char, Name    ) = "";
        };

        /** \brief Vector of values.
          *
          * This class wraps a vector of values, which is returned by various
          * macros (especially e_property_array) and can be converted to a
          * vector of strings for use by the property system.
          */

        struct E_PUBLISH Values{

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_noinline operator gfc::vector<ccp>()const{
              gfc::vector<ccp> result;
              m_vData.foreach(
                [&]( const Value& value ){
                  result += value.toName();
                }
              );
              return result;
            }

            e_noinline Values& operator+=( ccp s ){
              m_vData.push( s );
              return *this;
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          e_noinline Values( const std::initializer_list<Value>& l )
            : m_vData( l )
          {}

          e_noinline Values( const std::initializer_list<ccp>& l ){
            for( auto& i : l ){
              m_vData.push( i );
            }
          }

          Values() = default;
        ~ Values() = default;

        private:

          e_var_vector( ccp, Data );
        };

      //}:                                        |
      //Include:{                                 |
        //Prototypes:{                            |

          struct Quaternion;
          struct Triangle;
          struct Affine3;
          struct Matrix4;
          struct Vector4;
          struct Vector3;
          struct Vector2;
          struct Point3;
          struct Point2;
          struct Octree;
          struct AABB3;
          struct AABB2;
          struct QST3;
          struct QST2;

        //}:                                      |
        //Constants:{                             |

          /** \name Constants.
            *
            * All the static constants for a real type.
            *
            * @{
            */

          #include"math/constants.h"

        //}:                                      |
        //Methods:{                               |

          /** @}
            *
            * \name Basic methods.
            *
            * @{
            */

          #include"math/basic.h"

        //}:                                      |
        //Structs:{                               |

          /** @}
            *
            * \name Vectors and matrices.
            *
            * These structures are used to define all the different vectors and
            * matrices in the engine. Being under the Math<> umbrella you can
            * access them via the longhand Math<T>::Vector4, for example, or
            * the shorthand vec4.
            *
            * @{
            */

          #include"math/vector2.h"
          #include"math/matrix3.h"
          #include"math/vector3.h"
          #include"math/color.h"
          #include"math/affine3.h"
          #include"math/affine2.h"
          #include"math/matrix4.h"
          #include"math/vector4.h"
          #include"math/qst3.h"
          #include"math/qst2.h"

          /** @}
            *
            * \name Quaternions.
            *
            * These structures define the quaternion and all it's methods.
            *
            * @{
            */

          #include"math/quaternion.h"

          /** @}
            *
            * \name Points.
            *
            * These structures define the point class in two or more
            * dimensions. In the engine vectors should never be used as points
            * and vice versa. Always use points for points and vectors for
            * directions with magnitude.
            *
            * @{
            */

          #include"math/point2.h"
          #include"math/point3.h"

          /** @}
            *
            * \name Rays.
            *
            * These structures define the ray classes in two or more
            * dimensions.
            *
            * @{
            */

          #include"math/ray3.h"
          #include"math/ray2.h"

          /** @}
            *
            * \name Geometric primitives.
            *
            * These structures define the different geometric primitives
            * supported by the engine.
            *
            * @{
            */

          #include"math/plane.h"
          #include"math/triangle.h"
          #include"math/capsule.h"
          #include"math/sphere.h"
          #include"math/frustum.h"
          #include"math/splines.h"
          #include"math/cone.h"

          /** @}
            *
            * \name Axis aligned bounding boxes.
            *
            * These structures define the various axis aligned bounding boxes
            * used throughout the engine for 2D and 3D bounds. This includes
            * widgets and entities.
            *
            * @{
            */

          #include"math/aabb2.h"
          #include"math/aabb3.h"

        //}:                                      |
        //Trees:{                                 |

          #include"math/quadtree.h"
          #include"math/octree.h"

        //}:                                      |
        //Lerp:{                                  |

          /** @}
            *
            * \name Basic interpolator class.
            *
            * This section contains classes for interpolation.
            *
            * @{
            */

          #include"math/interpolator.h"

        //}:                                      |
      //}:                                        |
      //Operate:{                                 |

        /** @}
          *
          * \name Math operators.
          *
          * These operators allow you to add and subtract things from a math
          * type Math<T> blah.
          *
          * @{
          */

        #include"math/operators.h"

      //}:                                        |
      //Methods:{                                 |

        /** @}
          *
          * \name Other methods.
          *
          * These methods are used for validating the float and doing other
          * things such as negating the real number.
          *
          * @{
          */

        /** \brief Negate the float.
          *
          * This routine will negate the floating point number (double or
          * float) this math object represents.
          */

        e_forceinline T negated()const{
          return -value;
        }

        /** \brief Negate the float.
          *
          * This routine will negate the floating point number (double or
          * float) this math object represents.
          */

        e_forceinline void negate(){
          value = -value;
        }

        /** \brief Check number.
          *
          * This routine will return true if the value 't' is valid or false.
          */

        e_forceinline bool valid()const{
          return valid( value );
        }

        /** \brief Check number.
          *
          * This routine will return true if the value 't' is valid or false.
          *
          * \param value The value to check if it's valid.
          */

        static bool valid( const self& value );

        /** \brief Cast to type.
          *
          * This routine will cast the maths object to a particular type.
          */

        template<typename R> R as()const{
          return static_cast<const R>( value );
        }

        /** @} */

      //}:                                        |
      //------------------------------------------+-----------------------------

      template<typename R> e_forceinline_always Maths( const R& y )
        : value( T( y ))
      {}

      e_forceinline_always constexpr Maths( const float y )
        : value( y )
      {}

      e_forceinline_always Maths( const Maths& m )
        : value( m.value )
      {}

      constexpr e_forceinline_always Maths()
        : value( 0 )
      {}

    private:

      T value;
    };
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Power:{                                         |

  namespace EON{

    #define __pow2Next0( x ) (( x )|(( x )>>0x01 ))
    #define __pow2Next1( x ) (__pow2Next0( x )|(__pow2Next0( x )>>0x02 ))
    #define __pow2Next2( x ) (__pow2Next1( x )|(__pow2Next1( x )>>0x04 ))
    #define __pow2Next3( x ) (__pow2Next2( x )|(__pow2Next2( x )>>0x08 ))
    #define __pow2Next4( x ) (__pow2Next3( x )|(__pow2Next3( x )>>0x10 ))

    /** \brief Power class.
      *
      * This class exists for the purpose of dealing with powers.
      */

    struct E_PUBLISH Power{
      template<typename T>e_forceinline static T squared( const T x ){
        return x*x;
      }
      template<typename T>e_forceinline static T cubed( const T x ){
        return x*x*x;
      }
      e_forceinline static u64 to2( const u64 x ){
        return __pow2Next4( u64( x ))+1ULL;
      }
      e_forceinline static u32 to2( const u32 x ){
        return __pow2Next4( u32( x ))+1;
      }
      template<typename T> e_forceinline static T to2bsf( const T x ){
        return 0;
      }
      e_forceinline static bool is2( u32 x ){
        return x && !(x & (x - 1));
      }
    };

    template<> u64 e_forceinline Power::to2bsf<u64>( const u64 x ){
      #if e_compiling( win64 )
        u64 ix;
        _BitScanForward64( (unsigned long*)&ix, Power::to2( x ));
        return ix;
      #else
        return __builtin_ffsl( long( Power::to2( x )))-1;
      #endif
    }
    template<> u32 e_forceinline Power::to2bsf<u32>( const u32 x ){
      #if e_compiling( win64 )
        u64 ix;
        _BitScanForward( (unsigned long*)&ix, Power::to2( x ));
        return ix;
      #else
        return __builtin_ffs( Power::to2( x ))-1;
      #endif
    }

    #undef __pow2Next4
    #undef __pow2Next3
    #undef __pow2Next2
    #undef __pow2Next1
    #undef __pow2Next0
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Type:{                                          |
  //Bounding:{                                    |

    namespace EON{
      e_math_alias( aabb3, AABB3 );
      e_math_alias( aabb2, AABB2 );
    }

  //}:                                            |
  //Triangle:{                                    |

    namespace EON{
      e_math_alias( triangle, Triangle );
    }

  //}:                                            |
  //Matrices:{                                    |

    namespace EON{
      e_math_alias( vec4x4, Matrix4 );
      e_math_alias( vec3x4, Affine3 );
      e_math_alias( vec3x3, Matrix3 );
      e_math_alias( qst3,   QST3    );
      e_math_alias( qst2,   QST2    );
      #if e_compiling( web )
        using transform = vec4x4;
      #else
        using transform = vec3x4;
      #endif
      using transforms = gfc::vector<transform>;
    }

  //}:                                            |
  //Capsule:{                                     |

    namespace EON{
      e_math_alias( capsule, Capsule );
    }

  //}:                                            |
  //Frustum:{                                     |

    namespace EON{
      e_math_alias( frustum, Frustum );
    }

  //}:                                            |
  //Vectors:{                                     |

    namespace EON{
      e_math_alias( quat, Quaternion );
      e_math_alias( vec4, Vector4    );
      e_math_alias( vec3, Vector3    );
      e_math_alias( vec2, Vector2    );
    }

  //}:                                            |
  //Points:{                                      |

    namespace EON{
      e_math_alias( pt3, Point3 );
      e_math_alias( pt2, Point2 );
    }

  //}:                                            |
  //Circle:{                                      |

    namespace EON{
      e_math_alias( circle, Circle );
    }

  //}:                                            |
  //Sphere:{                                      |

    namespace EON{
      e_math_alias( sphere, Sphere );
    }

  //}:                                            |
  //Color:{                                       |

    namespace EON{
      e_math_alias( rgb10a2, RGB10a2 );
      e_math_alias( rgbi,    RGBi    );
      e_math_alias( rgba,    RGBa    );
      e_math_alias( bgra,    BGRa    );
      e_math_alias( hsva,    HSVa    );
    }

  //}:                                            |
  //Plane:{                                       |

    namespace EON{
      e_math_alias( plane, Plane );
    }

  //}:                                            |
  //Cone:{                                        |

    namespace EON{
      e_math_alias( cone, Cone );
    }

  //}:                                            |
  //Ray:{                                         |

    namespace EON{
      e_math_alias( ray3, Ray3 );
      e_math_alias( ray2, Ray2 );
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Metric:{                                        |

  /** \brief Metric mass wrapper.
    *
    * This templatized class creates a metric version of a math weight. It's
    * used especially by the property system to display numbers in metric
    * form: g, kg.
    */

  template<typename T> struct E_PUBLISH ssi_mass final:T{
    ssi_mass( const T& t )
      : T( t )
    {}
    ssi_mass( const double t )
      : T( t )
    {}
    ssi_mass( const float t )
      : T( t )
    {}
    ssi_mass( const EON::s32 i )
      : T( i )
    {}
    ssi_mass() = default;
  };

  /** \brief Metric wrapper.
    *
    * This templatized class creates a metric version of a math type. It's
    * used especially by the property system to display numbers in metric
    * form: cm, m, and km.
    */

  template<typename T> struct E_PUBLISH ssi final:T{
    ssi( const T& t )
      : T( t )
    {}
    ssi( const double t )
      : T( t )
    {}
    ssi( const float t )
      : T( t )
    {}
    ssi( const EON::s32 i )
      : T( i )
    {}
    ssi() = default;
  };

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Random:{                                        |

  /** \name Random numbers.
    *
    * This set of functionality deals with random numbers in the engine.
    *
    * @{
    */

  template< typename T> T e_rand( const T rangeStart, const T rangeEnd );
  template< typename T> T e_rand();

  E_PUBLISH EON::vec2d e_randunitvec2d();
  E_PUBLISH EON::vec3d e_randunitvec3d();

  E_PUBLISH EON::f64 e_randunitd();
  E_PUBLISH EON::f64 e_randpid();
  E_PUBLISH EON::f64 e_rand2pid();

  E_PUBLISH EON::vec2 e_randunitvec2();
  E_PUBLISH EON::vec3 e_randunitvec3();

  E_PUBLISH EON::f32 e_randunit();
  E_PUBLISH EON::f32 e_randpi();
  E_PUBLISH EON::f32 e_rand2pi();

  E_PUBLISH EON::vec2h e_randunitvec2h();
  E_PUBLISH EON::vec3h e_randunitvec3h();

  E_PUBLISH EON::f16 e_randunith();
  E_PUBLISH EON::f16 e_randpih();
  E_PUBLISH EON::f16 e_rand2pih();

  /** @} */

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Inline:{                                        |
  //Arithmetic:{                                  |

    template<typename Y> e_forceinline EON::Maths<Y> operator*( const float x, const EON::Maths<Y>& y ){ return x * Y( y ); }
    template<typename Y> e_forceinline EON::Maths<Y> operator/( const float x, const EON::Maths<Y>& y ){ return x / Y( y ); }
    template<typename Y> e_forceinline EON::Maths<Y> operator+( const float x, const EON::Maths<Y>& y ){ return x + Y( y ); }
    template<typename Y> e_forceinline EON::Maths<Y> operator-( const float x, const EON::Maths<Y>& y ){ return x - Y( y ); }

  //}:                                            |
  //Compare:{                                     |

    template<typename Y> e_forceinline bool operator<=( const float x, const EON::Maths<Y>& y ){ return( x <=EON::f32::cast( y )); }
    template<typename Y> e_forceinline bool operator< ( const float x, const EON::Maths<Y>& y ){ return( x < EON::f32::cast( y )); }
    template<typename Y> e_forceinline bool operator>=( const float x, const EON::Maths<Y>& y ){ return( x >=EON::f32::cast( y )); }
    template<typename Y> e_forceinline bool operator> ( const float x, const EON::Maths<Y>& y ){ return( x > EON::f32::cast( y )); }
    template<typename Y> e_forceinline bool operator==( const float x, const EON::Maths<Y>& y ){ return( x ==EON::f32::cast( y )); }
    template<typename Y> e_forceinline bool operator!=( const float x, const EON::Maths<Y>& y ){ return( x !=EON::f32::cast( y )); }

  //}:                                            |
  //Maths:{                                       |

    template<> template<> inline bool EON::Maths<double>::as<bool>()const{
      return( 0 != value );
    }

    template<> template<> inline bool EON::Maths<float>::as<bool>()const{
      return( 0 != value );
    }

    template<> template<> inline bool EON::Maths<half>::as<bool>()const{
      return( 0 != value );
    }

  //}:                                            |
//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
