//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if !e_compiling( arm64 )
  #if !e_compiling( web )
    #if e_compiling( win64 )
      #include<intrin.h>
    #else
      #include<x86intrin.h>
    #endif
  #endif
#endif
#include<float.h>
#include<random>
#include<cmath>

using namespace EON;
using namespace gfc;

//================================================+=============================
//Constants:{                                     |

  //http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html#algo3

#ifdef __APPLE__
  #pragma mark - Constants -
#endif

  /* Constants */

  namespace EON{
    template<> const f32 f32::k0       =  0.f;
    template<> const f32 f32::k1       =  1.f;
    template<> const f32 f32::k2       =  2.f;
    template<> const f32 f32::k3       =  3.f;
    template<> const f32 f32::k4       =  4.f;
    template<> const f32 f32::k5       =  5.f;
    template<> const f32 f32::k6       =  6.f;
    template<> const f32 f32::k7       =  7.f;
    template<> const f32 f32::k8       =  8.f;
    template<> const f32 f32::k9       =  9.f;
    template<> const f32 f32::k10      = 10.f;
    template<> const f32 f32::kPI      = 3.141592653589793f;
    template<> const f32 f32::k2PI     = kPI * k2;
    template<> const f32 f32::k4PI     = kPI * k4;
    template<> const f32 f32::cm       = 1.f;
    template<> const f32 f32::mm       = cm / 10.f;
    template<> const f32 f32::m        =     100.f * cm;
    template<> const f32 f32::dm       =      10.f * m;
    template<> const f32 f32::km       =    1000.f * m;
    template<> const f32 f32::Mm       =    1000.f * km;
    template<> const f32 f32::g        =       1.f;
    template<> const f32 f32::kg       =    1000.f * g;
    template<> const f32 f32::t        =    1000.f * kg;
    template<> const f32 f32::kt       =    1000.f * t;
    template<> const f32 f32::mt       =    1000.f * kt;
    template<> const f32 f32::kMin     =-FLT_MAX;
    template<> const f32 f32::kMax     = FLT_MAX;
    template<> const f32 f32::kEpsilon = 0.00001f;
    template<> const f32 f32::kHalf    = 0.5f;
  }

  /* Vector constants */

  namespace EON{
    template<> const vec4 vec4::kForward ( 0.f, 1.f, 0.f );
    template<> const vec4 vec4::kUp      ( 0.f, 0.f, 1.f );
    template<> const vec4 vec4::kZero    ( 0.f, 0.f, 0.f );
    template<> const vec4 vec4::kRight   ( 1.f, 0.f, 0.f );
    template<> const vec3 vec3::kForward ( 0.f, 1.f, 0.f );
    template<> const vec3 vec3::kRight   ( 1.f, 0.f, 0.f );
    template<> const vec3 vec3::kUp      ( 0.f, 0.f, 1.f );
    template<> const vec3 vec3::kZero    ( 0.f, 0.f, 0.f );
    template<> const vec2 vec2::kRight   ( 1.f, 0.f );
    template<> const vec2 vec2::kUp      ( 0.f,-1.f );
    template<> const vec2 vec2::kZero    ( 0.f, 0.f );
    template<> const pt3  pt3 ::kZero    ( 0.f, 0.f, 0.f );
    template<> const pt2  pt2 ::kZero    ( 0.f, 0.f );
    template<> const quat quat::kIdentity( 0.f, 0.f, 0.f, 1.f );
  }

  /* Color constants */

  namespace EON{
    template<> const rgba rgba::kBlack  ( 0.f, 1.f );
    template<> const rgba rgba::kWhite  ( 1.f );
    template<> const rgba rgba::kGrey   ( .5f, 1.f );
    template<> const rgba rgba::kOrange ( 1.f, .5f, 0.f, 1.f );
    template<> const rgba rgba::kYellow ( 1.f, 1.f, 0.f, 1.f );
    template<> const rgba rgba::kLime   ( .5f, 1.f, 0.f, 1.f );
    template<> const rgba rgba::kGreen  ( 0.f, 1.f, 0.f, 1.f );
    template<> const rgba rgba::kCyan   ( 0.f, 1.f, 1.f, 1.f );
    template<> const rgba rgba::kBlue   ( 0.f, 0.f, 1.f, 1.f );
    template<> const rgba rgba::kMagenta( 1.f, 0.f, 1.f, 1.f );
    template<> const rgba rgba::kRed    ( 1.f, 0.f, 0.f, 1.f );
  }

  /* Bounds constants */

  namespace EON{
    template<> const aabb2 aabb2::kFullUV( 0.f, 0.f, 1.f, 1.f );
    template<> const aabb2 aabb2::kInvUV ( 0.f, 1.f, 1.f, 0.f );
    template<> const aabb2 aabb2::kZero  ( 0.f );
    template<> const aabb3 aabb3::kZero  ( 0.f );
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Private:{                                       |
  //invertMatrix:{                                |

    namespace{
      bool invertMatrix( const float m[16], float inv[16] ){
        inv[ 0] = m[ 5] * m[10] * m[15] -
          m[ 5] * m[11] * m[14] -
          m[ 9] * m[ 6] * m[15] +
          m[ 9] * m[ 7] * m[14] +
          m[13] * m[ 6] * m[11] -
          m[13] * m[ 7] * m[10];
        inv[ 4] =-m[ 4] * m[10] * m[15] +
          m[ 4] * m[11] * m[14] +
          m[ 8] * m[ 6] * m[15] -
          m[ 8] * m[ 7] * m[14] -
          m[12] * m[ 6] * m[11] +
          m[12] * m[ 7] * m[10];
        inv[ 8] = m[ 4] * m[ 9] * m[15] -
          m[ 4] * m[11] * m[13] -
          m[ 8] * m[ 5] * m[15] +
          m[ 8] * m[ 7] * m[13] +
          m[12] * m[ 5] * m[11] -
          m[12] * m[ 7] * m[ 9];
        inv[12] = -m[4] * m[ 9] * m[14] +
          m[ 4] * m[10] * m[13] +
          m[ 8] * m[ 5] * m[14] -
          m[ 8] * m[ 6] * m[13] -
          m[12] * m[ 5] * m[10] +
          m[12] * m[ 6] * m[ 9];
        inv[ 1] =-m[ 1] * m[10] * m[15] +
          m[ 1] * m[11] * m[14] +
          m[ 9] * m[ 2] * m[15] -
          m[ 9] * m[ 3] * m[14] -
          m[13] * m[ 2] * m[11] +
          m[13] * m[ 3] * m[10];
        inv[ 5] = m[ 0] * m[10] * m[15] -
          m[ 0] * m[11] * m[14] -
          m[ 8] * m[ 2] * m[15] +
          m[ 8] * m[ 3] * m[14] +
          m[12] * m[ 2] * m[11] -
          m[12] * m[ 3] * m[10];
        inv[ 9] =-m[ 0] * m[9] * m[15] +
          m[ 0] * m[11] * m[13] +
          m[ 8] * m[ 1] * m[15] -
          m[ 8] * m[ 3] * m[13] -
          m[12] * m[ 1] * m[11] +
          m[12] * m[ 3] * m[ 9];
        inv[13] = m[ 0] * m[ 9] * m[14] -
          m[ 0] * m[10] * m[13] -
          m[ 8] * m[ 1] * m[14] +
          m[ 8] * m[ 2] * m[13] +
          m[12] * m[ 1] * m[10] -
          m[12] * m[ 2] * m[ 9];
        inv[ 2] = m[ 1] * m[ 6] * m[15] -
          m[ 1] * m[ 7] * m[14] -
          m[ 5] * m[ 2] * m[15] +
          m[ 5] * m[ 3] * m[14] +
          m[13] * m[ 2] * m[ 7] -
          m[13] * m[ 3] * m[ 6];
        inv[ 6] =-m[ 0] * m[ 6] * m[15] +
          m[ 0] * m[ 7] * m[14] +
          m[ 4] * m[ 2] * m[15] -
          m[ 4] * m[ 3] * m[14] -
          m[12] * m[ 2] * m[ 7] +
          m[12] * m[ 3] * m[ 6];
        inv[10] = m[ 0] * m[5] * m[15] -
          m[ 0] * m[ 7] * m[13] -
          m[ 4] * m[ 1] * m[15] +
          m[ 4] * m[ 3] * m[13] +
          m[12] * m[ 1] * m[ 7] -
          m[12] * m[ 3] * m[ 5];
        inv[14] =-m[ 0] * m[ 5] * m[14] +
          m[ 0] * m[ 6] * m[13] +
          m[ 4] * m[ 1] * m[14] -
          m[ 4] * m[ 2] * m[13] -
          m[12] * m[ 1] * m[ 6] +
          m[12] * m[ 2] * m[ 5];
        inv[ 3] =-m[ 1] * m[ 6] * m[11] +
          m[ 1] * m[ 7] * m[10] +
          m[ 5] * m[ 2] * m[11] -
          m[ 5] * m[ 3] * m[10] -
          m[ 9] * m[ 2] * m[ 7] +
          m[ 9] * m[ 3] * m[ 6];
        inv[ 7] = m[ 0] * m[ 6] * m[11] -
          m[ 0] * m[ 7] * m[10] -
          m[ 4] * m[ 2] * m[11] +
          m[ 4] * m[ 3] * m[10] +
          m[ 8] * m[ 2] * m[ 7] -
          m[ 8] * m[ 3] * m[ 6];
        inv[11] =-m[ 0] * m[ 5] * m[11] +
          m[ 0] * m[ 7] * m[ 9] +
          m[ 4] * m[ 1] * m[11] -
          m[ 4] * m[ 3] * m[ 9] -
          m[ 8] * m[ 1] * m[ 7] +
          m[ 8] * m[ 3] * m[ 5];
        inv[15] = m[ 0] * m[ 5] * m[10] -
          m[ 0] * m[ 6] * m[ 9] -
          m[ 4] * m[ 1] * m[10] +
          m[ 4] * m[ 2] * m[ 9] +
          m[ 8] * m[ 1] * m[ 6] -
          m[ 8] * m[ 2] * m[ 5];
        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        if( !det ){
          return false;
        }
        det = 1.f / det;
        for( u32 i=0; i<16; ++i ){
          inv[i] *= det;
        }
        return true;
      }
    }

  //}:                                            |
  //line2line:{                                   |

    namespace{
      bool line2line( const pt2& v0, const pt2& v1, const pt2& v2, const pt2& v3 ){
        const f32& denom = (( v3.y-v2.y )*( v1.x-v0.x ))-(( v3.x-v2.x )*( v1.y-v0.y ));
        const f32& numer = (( v3.x-v2.x )*( v0.y-v2.y ))-(( v3.y-v2.y )*( v0.x-v2.x ));
        const f32& nume2 = (( v1.x-v0.x )*( v0.y-v2.y ))-(( v1.y-v0.y )*( v0.x-v2.x ));
        if( denom==0.f ){
          if( numer==0.f && nume2==0.f ){
            return false;//coincident
          }
          return false;//parallel
        }
        const f32& ua = numer / denom;
        const f32& ub = nume2 / denom;
        return( ua >= 0.0f && ua <= 1.f && ub >= 0.0f && ub <= 1.f );
      }
    }

  //}:                                            |
  //line2rect:{                                   |

    namespace{
      bool line2rect( const pt2 v[2], const aabb2& r ){
        const pt2 ll( r.min.x, r.max.y );
        const pt2 ur( r.max.x, r.min.y );
        const pt2 ul( r.min.x, r.min.y );
        const pt2 lr( r.max.x, r.max.y );
        for( u32 i=0; i<2; ++i ){
          if( v[i].x > ll.x && v[i].x < ur.x && v[i].y < ll.y && v[i].y > ur.y ){
            return true;
          }
        }
        if( line2line( v[0], v[1], ul, ll )){
          return true;
        }
        if( line2line( v[0], v[1], ll, lr )){
          return true;
        }
        if( line2line( v[0], v[1], ul, ur )){
          return true;
        }
        if( line2line( v[0], v[1], ur, lr )){
          return true;
        }
        return false;
      }
    }

  //}:                                            |
  //imin3:{                                       |

    #if 0
      namespace{
        int imin3( const int x, const int y, const int z ){
          const int T = (x < y ? x : y);
          return( T < z ? T : z );
        }
      }
    #endif

  //}:                                            |
  //imax3:{                                       |

    #if 0
      namespace{
        int imax3( const int x, const int y, const int z ){
          const int T = (x > y ? x : y);
          return( T > z ? T : z );
        }
      }
    #endif

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Basic:{                                         |
  //[forwarding]:{                                |
    //vec3:{                                      |
      //[operators]:{                             |
        //operator=:{                             |

          namespace EON{
            template<> vec3& vec3::operator=( const vec4& V ){
              x = V.x;
              y = V.y;
              z = V.z;
              return *this;
            }
          }

        //}:                                      |
      //}:                                        |
      //Ctor:{                                    |

        /*namespace EON{
          template<> vec3::Vector3( const vec4& V ){
            x = V.x;
            y = V.y;
            z = V.z;
          }
        }*/

      //}:                                        |
    //}:                                          |
  //}:                                            |
  //smoothstep:{                                  |

    template<> f32 f32::smoothstep( const f32& edge2, const f32& x )const{
      const self& t = clamp(( x-value )/( edge2-value ), k0, k1 );
      return t*t*( k3-k2*t );
    }

  //}:                                            |
  //saturate[d]:{                                 |

    template<> f32 f32::saturated()const{
      return max( min( value, k1 ), k0 );
    }

    template<> void f32::saturate(){
      *this = max( min( value, k1 ), k0 );
    }

  //}:                                            |
  //iround:{                                      |

    namespace EON{
      template<> s32 f32::iround()const{
        return s32( value+.5f );
      }
    }

  //}:                                            |
  //rsqrt:{                                       |

#ifdef __APPLE__
  #pragma mark - Basic functions -
#endif

    namespace EON{
      template<> f32 f32::rsqrt()const{
        #ifdef __SSE3__
          float r;
          _mm_store_ss( &r, _mm_rsqrt_ss( _mm_load_ss( &value )));
        #else // quake style for non-SSE platforms.
          u32 i;
          float y, r;
          y = value * 0.5f;
          i = *reinterpret_cast<const u32*>( &value );
          i = 0x5f3759df - ( i >> 1 );
          r = *reinterpret_cast<float*>( &i );
          r = r * ( 1.5f - r * r * y );
        #endif
        return r;
      }
    }

  //}:                                            |
  //sqrt:{                                        |

    namespace EON{
      template<> f32 f32::sqrt()const{
        e_assert( value >= 0.f );
        #ifdef __SSE3__
          float r;
          _mm_store_ss( &r, _mm_sqrt_ss( _mm_load_ss( &value )));
          return r;
        #else
          return::sqrtf( value );
        #endif
      }
    }

  //}:                                            |
  //floor:{                                       |

    namespace EON{
      template<> f32 f32::floor()const{
        return::floorf( value );
      }
    }

  //}:                                            |
  //ceil:{                                        |

    namespace EON{
      template<> f32 f32::ceil()const{
        return::ceilf( value );
      }
    }

  //}:                                            |
  //frac:{                                        |

    namespace EON{
      template<> f32 f32::frac( f32& i )const{
        i = s32( value );
        return ( value - i );
      }
    }

  //}:                                            |
  //exp:{                                         |

    namespace EON{
      template<> f32 f32::exp()const{
        return::expf( value );
      }
    }

  //}:                                            |
  //pow:{                                         |

    namespace EON{
      template<> f32 f32::pow( const f32& y )const{
        return::powf( value, y.value );
      }
    }

  //}:                                            |
  //atan2:{                                       |

    namespace EON{
      template<> f32 f32::atan2( const f32& y )const{
        return::atan2f( value, y.value );
      }
    }

  //}:                                            |
  //atan:{                                        |

    namespace EON{
      template<> f32 f32::atan()const{
        return::atanf( value );
      }
    }

  //}:                                            |
  //tan:{                                         |

    namespace EON{
      template<> f32 f32::tan()const{
        return::tanf( value );
      }
    }

  //}:                                            |
  //asin:{                                        |

    namespace EON{
      template<> f32 f32::asin()const{
        return::asinf( value );
      }
    }

  //}:                                            |
  //sin:{                                         |

    namespace EON{
      template<> f32 f32::sin()const{
        return::sinf( value );
      }
    }

  //}:                                            |
  //acos:{                                        |

    namespace EON{
      template<> f32 f32::acos()const{
        return::acosf( value );
      }
    }

  //}:                                            |
  //cos:{                                         |

    namespace EON{
      template<> f32 f32::cos()const{
        return::cosf( value );
      }
    }

  //}:                                            |
  //mod:{                                         |

    namespace EON{
      template<> f32 f32::mod( const f32& y )const{
        return::fmodf( value, y.value );
      }
    }

  //}:                                            |
  //abs:{                                         |

    namespace EON{
      template<> f32 f32::abs()const{
        return::fabsf( value );
      }
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Validity:{                                      |

#ifdef __APPLE__
  #pragma mark -
  #pragma mark Validity
  #pragma mark -
#endif

  namespace EON{
    template<> bool f32::valid( const f32& t ){
      //x != x is an odd property of NaNs.
      #if _MSC_VER == 1700
        if( !_finite( t.x )){
          return false;
        }
      #else
        //if( std::fpclassify( t.value ) == FP_SUBNORMAL ){
        //  return false;
        //}
        if( std::isinf( t.value )){
          return false;
        }
        if( std::isnan( t.value )){
          return false;
        }
      #endif
      return true;
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Geometry:{                                      |
  //Points:{                                      |
    //[operators]:{                               |
      //operator-:{                               |

#ifdef __APPLE__
  #pragma mark - Points -
#endif

        namespace EON{
          template<> pt3 pt3::operator-( const vec3& v )const{
            pt3 r;
            r.x = x-v.x;
            r.y = y-v.y;
            r.z = z-v.z;
            return r;
          }
        }

      //}:                                        |
      //operator+:{                               |

        namespace EON{
          template<> pt3 pt3::operator+( const vec3& v )const{
            pt3 r;
            r.x = x+v.x;
            r.y = y+v.y;
            r.z = z+v.z;
            return r;
          }
        }

      //}:                                        |
      //operator/:{                               |

        namespace EON{
          template<> pt3 pt3::operator/( const vec3& v )const{
            pt3 r;
            r.x = x/v.x;
            r.y = y/v.y;
            r.z = z/v.z;
            return r;
          }
        }

      //}:                                        |
      //operator*:{                               |

        namespace EON{
          template<> pt3 pt3::operator*( const vec4x4& M )const{
            return pt3(
              M.XX * x + M.YX * y + M.ZX * z + M.WX,
              M.XY * x + M.YY * y + M.ZY * z + M.WY,
              M.XZ * x + M.YZ * y + M.ZZ * z + M.WZ
            );
          }
          template<> pt3 pt3::operator*( const vec3x3& M )const{
            return pt3(
              M.XX * x + M.YX * y + M.ZX * z,
              M.XY * x + M.YY * y + M.ZY * z,
              M.XZ * x + M.YZ * y + M.ZZ * z
            );
          }
          template<> pt3 pt3::operator*( const vec3& v )const{
            pt3 r;
            r.x = x*v.x;
            r.y = y*v.y;
            r.z = z*v.z;
            return r;
          }
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |
      //nearestPointToLine:{                      |

        namespace EON{
          template<> pt3 pt3::nearestPointToLine( const pt3& P, const pt3& C )const{
            const auto& u = e_normalize( vec3( P.x - x, P.y - y, P.z - z ));
            const auto& v = vec3( C-P );
            const auto& w = u.dot( v );
            return pt3( u.x * w, u.y * w, u.z * w );
          }
        }

      //}:                                        |
    //}:                                          |
    //[ctors]:{                                   |

      namespace EON{
        template<> pt3::Point3( const vec4& v ){
          x = v.x;
          y = v.y;
          z = v.z;
        }
      }

      namespace EON{
        template<> pt3::Point3( const vec3& v ){
          x = v.x;
          y = v.y;
          z = v.z;
        }
      }

    //}:                                          |
    //Methods:{                                   |
      //scale?ToClip:{                            |

        namespace EON{
          template<> pt2 pt2::scaledToClip( const f32& scale )const{
            const auto& cx = 640.f;
            const auto& cy = 360.f;
            pt2 p;
            p.x = (x/cx*2.f-1.f)*scale;
            p.y = (y/cy*2.f-1.f)*scale;
            return p;
          }
          template<> void pt2::scaleToClip( const f32& scale ){
            const auto& cx = 640.f;
            const auto& cy = 360.f;
            x = (x/cx*2.f-1.f)*scale;
            y = (y/cy*2.f-1.f)*scale;
          }
        }

      //}:                                        |
      //scale?Back:{                              |

        namespace EON{
          template<> void pt2::scaleBack(){
            const f32& cx = 640.f;
            const f32& cy = 360.f;
            x = (x+1.f)*.5f*cx;
            y = (y+1.f)*.5f*cy;
          }
          template<> pt2 pt2::scaledBack()const{
            pt2 p( *this );
            p.scaleBack();
            return p;
          }
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
  //Ray:{                                         |
    //closestDistance:{                           |

      template<> f32 ray3::closestDistance( const ray3& l2, f32& aT, f32& bT ){
        const auto& l1  = *this;
        const auto& dp  = l2.p - l1.p;
        const auto& v12  = e_dot( l1.d, l1.d );
        const auto& v22  = e_dot( l2.d, l2.d );
        const auto& v1v2 = e_dot( l1.d, l2.d );
        const auto& det  = v1v2 * v1v2 - v12 * v22;
        if( det.abs() > FLT_MIN ){
          const auto& inv_det = 1.f / det;
          const auto& dpv1 = e_dot( dp, l1.d );
          const auto& dpv2 = e_dot( dp, l2.d );
          aT = inv_det * ( v22  * dpv1 - v1v2 * dpv2 );
          bT = inv_det * ( v1v2 * dpv1 - v12  * dpv2 );
          const auto& v =( dp + l2.d * bT - l1.d * aT );
          if( v.valid() ){
            return v.length();
          }
          return 0.f;
        }
        const vec3& a = e_cross( dp, l1.d );
        return f32( e_dot( a, a ) / v12 ).sqrt();
      }

    //}:                                          |
  //}:                                            |
  //Circle:{                                      |
    //toBounds:{                                  |

      namespace EON{
        template<> aabb2 circle::toBounds()const{
          return aabb2(
            v.x - v.z,
            v.y - v.z,
            v.x + v.z,
            v.y + v.z
          );
        }
      }

    //}:                                          |
  //}:                                            |
  //Sphere:{                                      |
    //intersects:{                                |

      namespace EON{
        template<> bool sphere::intersects( const ray3& in_tRay, f32& out_fDistance )const{
          const vec3 Q = this->v.xyz() - in_tRay.p;
          const f32& c = Q.length();
          const f32& v = Q.dot( in_tRay.d );
          const f32& d = toRadius()*toRadius() - (c*c - v*v);
          if( d < 0.f ){
            out_fDistance = -1.f;
            return false;
          }
          out_fDistance = (v - sqrt( d ));
          return true;
        }
      }

    //}:                                          |
    //toBounds:{                                  |

      namespace EON{
        template<> aabb3 sphere::toBounds()const{
          return aabb3(
            v.x - v.w,
            v.y - v.w,
            v.z - v.w,
            v.x + v.w,
            v.y + v.w,
            v.z + v.w
          );
        }
      }

    //}:                                          |
  //}:                                            |
  //Plane:{                                       |

    namespace EON{
      template<> void plane::set( const triangle& in_tTriangle ){
        a() =   in_tTriangle.A.y*( in_tTriangle.B.z-in_tTriangle.C.z ) + in_tTriangle.B.y*( in_tTriangle.C.z-in_tTriangle.A.z ) + in_tTriangle.C.y*( in_tTriangle.A.z-in_tTriangle.B.z );
        b() =   in_tTriangle.A.z*( in_tTriangle.B.x-in_tTriangle.C.x ) + in_tTriangle.B.z*( in_tTriangle.C.x-in_tTriangle.A.x ) + in_tTriangle.C.z*( in_tTriangle.A.x-in_tTriangle.B.x );
        c() =   in_tTriangle.A.x*( in_tTriangle.B.y-in_tTriangle.C.y ) + in_tTriangle.B.x*( in_tTriangle.C.y-in_tTriangle.A.y ) + in_tTriangle.C.x*( in_tTriangle.A.y-in_tTriangle.B.y );
        d() =-( in_tTriangle.A.x*( in_tTriangle.B.y*in_tTriangle.C.z-in_tTriangle.C.y*in_tTriangle.B.z )+in_tTriangle.B.x*( in_tTriangle.C.y*in_tTriangle.A.z-in_tTriangle.A.y*in_tTriangle.C.z )+in_tTriangle.C.x*( in_tTriangle.A.y*in_tTriangle.B.z-in_tTriangle.B.y*in_tTriangle.A.z ));
      }
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Algebra:{                                       |
  //Quaternion:{                                  |
    //sqLength:{                                  |

#ifdef __APPLE__
  #pragma mark - Algebra -
#endif

      namespace EON{
        template<> f32 quat::lengthSquared()const{
          return
            a[0]*a[0] + a[1]*a[1] +
            a[2]*a[2] + a[3]*a[3]
          ;
        }
      }

    //}:                                          |
    //invert:{                                    |

      namespace EON{
        template<> void quat::invert(){
          const f32& norm = lengthSquared();
          if( norm > 0.f ){
            const f32& invNorm = 1.f/norm;
            x =  x * invNorm;
            y = -y * invNorm;
            z = -z * invNorm;
            w = -w * invNorm;
          }else{
            x = y = z = w = 0.f;
          }
        }
      }

    //}:                                          |
    //[operators]:{                               |
      //operator::==:{                            |

        namespace EON{
          template<> bool quat::operator==( const quat& q )const{
            return( 0 == memcmp( a, q.a, sizeof( a )));
          }
        }

      //}:                                        |
      //operator::!=:{                            |

        namespace EON{
          template<> bool quat::operator!=( const quat& q )const{
            return( 0 != memcmp( a, q.a, sizeof( a )));
          }
        }

      //}:                                        |
      //operator::<:{                             |

        namespace EON{
          template<> bool quat::operator<( const quat& q )const{
            return( memcmp( a, q.a, sizeof( a )) < 0 );
          }
        }

      //}:                                        |
      //operator::<=:{                            |

        namespace EON{
          template<> bool quat::operator<=( const quat& q )const{
            return( memcmp( a, q.a, sizeof( a )) <= 0 );
          }
        }

      //}:                                        |
      //operator::<:{                             |

        namespace EON{
          template<> bool quat::operator>( const quat& q )const{
            return( memcmp( a, q.a, sizeof( a )) > 0 );
          }
        }

      //}:                                        |
      //operator::<=:{                            |

        namespace EON{
          template<> bool quat::operator>=( const quat& q )const{
            return( memcmp( a, q.a, 16 ) >= 0 );
          }
        }

      //}:                                        |
      //operator::+?:{                            |

        namespace EON{
          template<> quat& quat::operator+=( const quat& q ){
            x+=q.x;
            y+=q.y;
            z+=q.z;
            w+=q.w;
            return *this;
          }
        }

        namespace EON{
          template<> quat quat::operator+( const quat& q )const{
            return quat(
              x+q.x,
              y+q.y,
              z+q.z,
              w+q.w
            );
          }
        }

      //}:                                        |
      //operator::*?:{                            |

        namespace EON{
          template<> quat quat::operator*( const quat& q )const{
            return quat(
              w*q.x + x*q.w + y*q.z - z*q.y,
              w*q.y + y*q.w + z*q.x - x*q.z,
              w*q.z + z*q.w + x*q.y - y*q.x,
              w*q.w - x*q.x - y*q.y - z*q.z
            );
          }
        }

        namespace EON{
          template<> quat quat::operator*( const f32& s )const{
            return quat(
              x * s,
              y * s,
              z * s,
              w * s
            );
          }
        }

        namespace EON{
          template<> quat& quat::operator*=( const quat& q ){
            const quat r(
              w*q.x + x*q.w + y*q.z - z*q.y,
              w*q.y + y*q.w + z*q.x - x*q.z,
              w*q.z + z*q.w + x*q.y - y*q.x,
              w*q.w - x*q.x - y*q.y - z*q.z );
            *this = r;
            return *this;
          }
        }

        namespace EON{
          template<> quat& quat::operator*=( const f32& s ){
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
          }
        }

      //}:                                        |
      //operator::/?:{                            |

        namespace EON{
          template<> quat& quat::operator/=( const quat& q ){
            quat p( q );
            p.invert();
            (*this)*p;
            return *this;
          }
        }

        namespace EON{
          template<> quat& quat::operator/=( const f32& s ){
            if( s != 0 ){
              x/=s;
              y/=s;
              z/=s;
              w/=s;
            }else{
              x = 0;
              y = 0;
              z = 0;
              w = 0;
            }
            return *this;
          }
        }

        namespace EON{
          template<> quat quat::operator/( const quat& q )const{
            quat p( q );
            p.invert();
            return (*this)*p;
          }
        }

        namespace EON{
          template<> quat quat::operator/( const f32& s )const{
            quat q;
            if( s != 0 ){
              q.x=x/s;
              q.y=y/s;
              q.z=z/s;
              q.w=w/s;
            }else{
              q.x = 0;
              q.y = 0;
              q.z = 0;
              q.w = 0;
            }
            return q;
          }
        }

      //}:                                        |
    //}:                                          |
    //setAxisAngle:{                              |

#ifdef __APPLE__
  #pragma mark -
  #pragma mark Quaternions
  #pragma mark -
#endif

      namespace EON{
        template<> void quat::setAxisAngle( const vec3& axis, const f32& angleDeg ){
          const f32& a = rad( angleDeg );
          const vec3& v = axis*sin( a / 2.f );
          x = v.x;
          y = v.y;
          z = v.z;
          w = cos( a/2.f );
        }
      }

    //}:                                          |
    //asQuaternion:{                              |

      namespace EON{
        template<> quat vec3x3::asQuaternion()const{
          // Use the Graphics Gems code, from
          // ftp://ftp.cis.upenn.edu/pub/graphics/shoemake/quatut.ps.Z
          // *NOT* the "Matrix and Quaternions FAQ", which has errors!

          // the trace is the sum of the diagonal elements; see
          // http://mathworld.wolfram.com/MatrixTrace.html
          f32 t = m[0][0] + m[1][1] + m[2][2];
          f32 x, y, z, w;

          // we protect the division by s by ensuring that s>=1
          if( t >= 0.f ){ // |w| >= .5
            f32 s = sqrt( t + 1.f ); // |s|>=1 ...
            w = 0.5f * s;
            s = 0.5f / s; // so this division isn't bad
            x = (m[2][1] - m[1][2]) * s;
            y = (m[0][2] - m[2][0]) * s;
            z = (m[1][0] - m[0][1]) * s;
          }else if(( m[0][0] > m[1][1] )&&( m[0][0] > m[2][2] )){
            f32 s = sqrt( 1.f + m[0][0] - m[1][1] - m[2][2] ); // |s|>=1
            x = s * 0.5f; // |x| >= .5
            s = 0.5f / s;
            y = (m[1][0] + m[0][1]) * s;
            z = (m[0][2] + m[2][0]) * s;
            w = (m[2][1] - m[1][2]) * s;
          }else if( m[1][1] > m[2][2] ){
            f32 s = sqrt( 1.f + m[1][1] - m[0][0] - m[2][2] ); // |s|>=1
            y = s * 0.5f; // |y| >= .5
            s = 0.5f / s;
            x = (m[1][0] + m[0][1]) * s;
            z = (m[2][1] + m[1][2]) * s;
            w = (m[0][2] - m[2][0]) * s;
          }else{
            f32 s = sqrt( 1.f + m[2][2] - m[0][0] - m[1][1] ); // |s|>=1
            z = s * 0.5f; // |z| >= .5
            s = 0.5f / s;
            x = (m[0][2] + m[2][0]) * s;
            y = (m[2][1] + m[1][2]) * s;
            w = (m[1][0] - m[0][1]) * s;
          }
          quat q;
          q.x = x;
          q.y = y;
          q.z = z;
          q.w = w;
          return q;
        }
      }

    //}:                                          |
    //compare:{                                   |

      namespace EON{
        template<> bool quat::compare( const quat& q, const f32& tolerance )const{
          if( abs( x - q.x) > tolerance ){
            return false;
          }
          if( abs( y - q.y) > tolerance ){
            return false;
          }
          if( abs( z - q.z) > tolerance ){
            return false;
          }
          if( abs( w - q.w) > tolerance ){
            return false;
          }
          return true;
        }
      }

    //}:                                          |
    //rotate:{                                    |

      namespace EON{
        template<> void quat::rotate( const vec3& axis, const f32& angleDeg ){
          *this = quat( axis, angleDeg ) * *this;
        }
      }

      namespace EON{
        template<> void quat::rotate( const vec2& xyMouse ){
          quat q1 = *this, q2, q3;
          q2.setAxisAngle( vec3( 0, 0,-1 ), xyMouse.x );
          q3.setAxisAngle( vec3(-1, 0, 0 ), xyMouse.y );
          q1 = q2 * q1;
          q1 = q1 * q3;
          *this = q1;
        }
      }

    //}:                                          |
    //length:{                                    |

      namespace EON{
        template<> f32 quat::length()const{
          return sqrt(
            a[0]*a[0] + a[1]*a[1] +
            a[2]*a[2] + a[3]*a[3]
          );
        }
      }

    //}:                                          |
    //normalize:{                                 |

      namespace EON{
        template<> quat& quat::normalize(){
          const f32& l = length();
          if( l > 0.00001f ){
            const f32& rcpLength=1.f/l;
            x *= rcpLength;
            y *= rcpLength;
            z *= rcpLength;
            w *= rcpLength;
          }else{
            x = y = z = w = 0.f;
          }
          return *this;
        }
      }

    //}:                                          |
    //setEuler:{                                  |

      namespace EON{
        template<> void quat::setEuler( const vec3& e ){
          const f32 cos_x = ( e.x.rad() * .5f ).cos();
          const f32 cos_y = ( e.y.rad() * .5f ).cos();
          const f32 cos_z = ( e.z.rad() * .5f ).cos();
          const f32 sin_x = ( e.x.rad() * .5f ).sin();
          const f32 sin_y = ( e.y.rad() * .5f ).sin();
          const f32 sin_z = ( e.z.rad() * .5f ).sin();
          x = sin_x*cos_y*cos_z - cos_x*sin_y*sin_z;
          y = cos_x*sin_y*cos_z + sin_x*cos_y*sin_z;
          z = cos_x*cos_y*sin_z - sin_x*sin_y*cos_z;
          w = cos_x*cos_y*cos_z + sin_x*sin_y*sin_z;
          normalize();
        }
      }

    //}:                                          |
    //getEuler:{                                  |

      namespace EON{
        template<> void quat::getEuler( f32& out_fX, f32& out_fY, f32& out_fZ, const bool bHomogeneous )const{
          const f32& sqw = w*w;
          const f32& sqx = x*x;
          const f32& sqy = y*y;
          const f32& sqz = z*z;
          if( bHomogeneous ){
            out_fZ = atan2( 2.f * (x*y + z*w), sqx - sqy - sqz + sqw );
            out_fY = asin( -2.f * (x*z - y*w) );
            out_fX = atan2( 2.f * (y*z + x*w),-sqx - sqy + sqz + sqw );
          }else{
            out_fZ = atan2( 2.f * (z*y + x*w), 1 - 2*( sqx + sqy ));
            out_fY = asin( -2.f * (x*z - y*w));
            out_fX = atan2( 2.f * (x*y + z*w), 1 - 2*( sqy + sqz ));
          }
          out_fX = out_fX.deg();
          out_fY = out_fY.deg();
          out_fZ = out_fZ.deg();
        }
      }

    //}:                                          |
    //randomize:{                                 |

      namespace EON{
        template<> void quat::randomize( const f32& x1, const f32& x2, const f32& x3 ){
          const f32& Z = x1;
          const f32& o = k2PI * x2;
          const f32& r = sqrt( 1.f - z * z );
          const f32& W = kPI * x3;
          const f32& sw = sin( w );
          x = sw * cos(o) * r;
          y = sw * sin(o) * r;
          z = sw * Z;
          w = cos( W );
        }
      }

    //}:                                          |
    //get?x?:{                                    |

      namespace EON{
        template<> vec4x4 quat::get4x4()const{
          vec4x4 R( 0.f );
          const f32& xx = x * x;
          const f32& xy = x * y;
          const f32& xz = x * z;
          const f32& xw = x * w;
          const f32& yy = y * y;
          const f32& yz = y * z;
          const f32& yw = y * w;
          const f32& zz = z * z;
          const f32& zw = z * w;
          R( 0, 0 ) = 1.f - 2.f * (yy + zz);
          R( 1, 0 ) =       2.f * (xy - zw);
          R( 2, 0 ) =       2.f * (xz + yw);
          R( 0, 1 ) =       2.f * (xy + zw);
          R( 1, 1 ) = 1.f - 2.f * (xx + zz);
          R( 2, 1 ) =       2.f * (yz - xw);
          R( 0, 2 ) =       2.f * (xz - yw);
          R( 1, 2 ) =       2.f * (yz + xw);
          R( 2, 2 ) = 1.f - 2.f * (xx + yy);
          R( 3, 3 ) = 1.f;
          return R;
        }
        template<> vec3x4 quat::get3x4()const{
          vec3x4 R( 0.f );
          const f32& xx = x * x;
          const f32& xy = x * y;
          const f32& xz = x * z;
          const f32& xw = x * w;
          const f32& yy = y * y;
          const f32& yz = y * z;
          const f32& yw = y * w;
          const f32& zz = z * z;
          const f32& zw = z * w;
          R( 0, 0 ) = 1.f - 2.f * (yy + zz);
          R( 0, 1 ) =       2.f * (xy - zw);
          R( 0, 2 ) =       2.f * (xz + yw);
          R( 1, 0 ) =       2.f * (xy + zw);
          R( 1, 1 ) = 1.f - 2.f * (xx + zz);
          R( 1, 2 ) =       2.f * (yz - xw);
          R( 2, 0 ) =       2.f * (xz - yw);
          R( 2, 1 ) =       2.f * (yz + xw);
          R( 2, 2 ) = 1.f - 2.f * (xx + yy);
          return R;
        }
      }

      namespace EON{
        template<> vec3x3 quat::get3x3()const{
          vec3x3 R( 0.f );
          const f32& xx = x * x;
          const f32& xy = x * y;
          const f32& xz = x * z;
          const f32& xw = x * w;
          const f32& yy = y * y;
          const f32& yz = y * z;
          const f32& yw = y * w;
          const f32& zz = z * z;
          const f32& zw = z * w;
          R( 0, 0 ) = 1.f - 2.f * (yy + zz);
          R( 1, 0 ) =       2.f * (xy - zw);
          R( 2, 0 ) =       2.f * (xz + yw);
          R( 0, 1 ) =       2.f * (xy + zw);
          R( 1, 1 ) = 1.f - 2.f * (xx + zz);
          R( 2, 1 ) =       2.f * (yz - xw);
          R( 0, 2 ) =       2.f * (xz - yw);
          R( 1, 2 ) =       2.f * (yz + xw);
          R( 2, 2 ) = 1.f - 2.f * (xx + yy);
          return R;
        }
      }

    //}:                                          |
    //dot:{                                       |

      namespace EON{
        template<> f32 quat::dot( const quat& q )const{
          return
            a[0]*q.a[0] + a[1]*q.a[1] +
            a[2]*q.a[2] + a[3]*q.a[3]
          ;
        }
      }

    //}:                                          |
    //lookat:{                                    |

      namespace EON{
        template<> void quat::lookat( const pt3& srcPoint, const pt3& dstPoint ){
          vec3 direction = dstPoint - srcPoint;
          direction.normalize();
          f32 dot = vec3::kForward.dot( direction );
          if( f32( dot+1.f ).abs() < 0.000001f ){
            *this = quat( vec3::kForward, deg( kPI ));
          }else if( f32( dot-1.f ).abs() < 0.000001f ){
            *this = quat();
          }else{
            f32 angle = deg( dot.acos() );
            vec3 axis = vec3::kForward.cross( direction );
            axis.normalize();
            *this = quat( axis, angle );
          }
        }
      }

    //}:                                          |
    //inverse:{                                   |

      namespace EON{
        template<> quat quat::inverse()const{
          quat inv=*this;
          inv.invert();
          return inv;
        }
      }

    //}:                                          |
    //conjugate:{                                 |

      namespace EON{
        template<> quat quat::conjugate()const{
          return quat( a[0],-a[1],-a[2],-a[3] );
        }
      }

    //}:                                          |
    //exp:{                                       |

      namespace EON{
        template<> quat quat::exp()const{
          const f32& a = ((vec3*)this)->length();
          const f32& sina = sin( a );
          const f32& cosa = cos( a );
          quat ret;
          ret.w = cosa;
          if( a > 0 ){
            ret.x = sina * x / a;
            ret.y = sina * y / a;
            ret.z = sina * z / a;
          }else{
            ret.x = 0;
            ret.y = 0;
            ret.z = 0;
          }
          return ret;
        }
      }

    //}:                                          |
    //log:{                                       |

      namespace EON{
        template<> quat quat::log()const{
          const f32& a = acos( w );
          const f32& sina = sin( a );
          quat ret;
          ret.w = 0;
          if( sina > 0 ){
            ret.x = a*x/sina;
            ret.y = a*y/sina;
            ret.z = a*z/sina;
          }
          return ret;
        }
      }

    //}:                                          |
    //lerped:{                                    |

      namespace EON{
        template<> quat quat::lerped( const quat& p, const quat& q, const f32& fTime ){
          quat result = p*(1-fTime) + q*fTime;
          result.normalize();
          return result;
        }
      }

    //}:                                          |
    //lerp:{                                      |

      namespace EON{
        template<> quat& quat::lerp( const quat& p, const quat& q, const f32& fTime ){
          *this = lerped( p, q, fTime );
          return *this;
        }
      }

    //}:                                          |
    //slerpedNoInvert:{                           |

      namespace EON{
        template<> quat quat::slerpedNoInvert( const quat& p, const quat& q, const f32& fTime ){
          const f32& _dot = p.dot( q );
          if(( _dot > -0.95f )&&( _dot < 0.95f )){
            const f32& angle = acos( _dot );
            return( p*sin( angle*( 1.f-fTime ))+q*sin( angle*fTime ))/sin( angle );
          }
          // if the angle is small, use linear interpolation
          return lerped( p, q, fTime );
        }
      }

    //}:                                          |
    //slerp:{                                     |

      namespace EON{
        template<> quat& quat::slerp( const quat& p, const quat& q, const f32& fTime ){
          quat q3;
          f32 _dot = p.dot( q );
          if( _dot < 0 ){
            _dot = -_dot;
            q3 = q;
            q3.negate();
          }else{
            q3 = q;
          }
          if( _dot < 0.95f ){
            const f32& angle = acos( _dot );
            *this=( p*sin( angle*( 1.f-fTime )) + q3*sin( angle*fTime ))/sin( angle );
          }else{// if the angle is small, use linear interpolation
            *this=lerped( p, q3, fTime );
          }
          return *this;
        }
      }

    //}:                                          |
    //squad:{                                     |

      namespace EON{
        template<> quat& quat::squad( const quat& q1, const quat& q2, const quat& a, const quat& b, const f32& fTime ){
          const quat& c = slerpedNoInvert( q1, q2, fTime );
          const quat& d = slerpedNoInvert(  a,  b, fTime );
          *this = slerpedNoInvert( c, d, 2*fTime*( 1.f-fTime ));
          return *this;
        }
      }

    //}:                                          |
    //align:{                                     |

      namespace EON{
        template<> quat& quat::align( const vec3& up, const vec3& norm ){
          vec3 w = up.cross( norm );
          *this = quat( w.x, w.y, w.z, 1.f + up.dot( norm ));
          return *this;
        }
      }

    //}:                                          |
  //}:                                            |
  //Matrices:{                                    |
    //vec4x4:{                                    |
      //[operators]:{                             |
        //operator*=:{                            |

#ifdef __APPLE__
  #pragma mark - Matrices -
#endif

        namespace EON{
          template<> vec4x4& vec4x4::operator*=( const vec4x4& B ){
            vec4x4 A = *this;
            #ifdef __SSE__
              //https://software.intel.com/en-us/articles/using-simd-technologies-on-intel-architecture-to-speed-up-game-code
              const float* a = (float*)A.t;
              const float* b = (float*)B.t;
              float* output =  (float*)t;
              __m128 ma_col_0 = _mm_load_ps( a );
              __m128 ma_col_1 = _mm_load_ps( a + 4 );
              __m128 ma_col_2 = _mm_load_ps( a + 8 );
              __m128 ma_col_3 = _mm_load_ps( a + 12 );
              __m128 mb_col_0 = _mm_load_ps( b );
              __m128 mb_col_1 = _mm_load_ps( b + 4 );
              __m128 mb_col_2 = _mm_load_ps( b + 8 );
              __m128 mb_col_3 = _mm_load_ps( b + 12 );
              // get ready to store the result
              __m128 result0;
              __m128 result1;
              __m128 result2;
              __m128 result3;
              // result = first column of B x first row of A
              result0 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_0 ));
              result1 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_1 ));
              result2 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_2 ));
              result3 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_3 ));
              // result += second column of B x second row of A
              result0 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_0 ), result0 );
              result1 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_1 ), result1 );
              result2 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_2 ), result2 );
              result3 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_3 ), result3 );
              // result += third column of B x third row of A
              result0 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_0 ), result0 );
              result1 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_1 ), result1 );
              result2 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_2 ), result2 );
              result3 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_3 ), result3 );
              // result += last column of B x last row of A
              result0 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_0 ), result0 );
              result1 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_1 ), result1 );
              result2 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_2 ), result2 );
              result3 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_3 ), result3 );
              // store the result to memory
              _mm_store_ps( output,    result0 );
              _mm_store_ps( output+4,  result1 );
              _mm_store_ps( output+8,  result2 );
              _mm_store_ps( output+12, result3 );
            #else
              for( u32 i=0; i<4; ++i ){
                for( u32 j=0; j<4; ++j ){
                  f32 sum = 0.f;
                  for( u32 e=0; e<4; ++e ){
                    sum += A.m[i][e] * B.m[e][j];
                  }
                  m[i][j]=sum;
                }
              }
            #endif
            return *this;
          }
        }

        //}:                                      |
      //}:                                        |
      //transpose:{                               |

        namespace EON{
          template<> void vec4x4::transpose(){
            vec4x4 M=*this;
            XY=M.YX; XZ=M.ZX; XW=M.WX;
            YX=M.XY; YZ=M.ZY; YW=M.WY;
            ZX=M.XZ; ZY=M.YZ; ZW=M.WZ;
            WX=M.XW; WY=M.YW; WZ=M.ZW;
          }
        }

      //}:                                        |
      //setPerspectiveV2C:{                       |

        namespace EON{
          template<> void vec4x4::setPerspectiveV2C( const f32& degFovY, const f32& aspect, const f32& zn, const f32& zf ){
            const f32& f = 1.f / tan( rad( degFovY )*.5f );
            const f32& a = aspect;
            const f32& z = zn-zf;
            // +--         --+   +--        --+
            // | XX XY XZ XW |   | RR RR RR 0 |
            // | YX YY YZ YW | = | RR RR RR 0 |
            // | ZX ZY ZZ ZW | = | RR RR RR 0 |
            // | WX WY WZ WW |   | Tx Ty Tz 1 |
            // +--         --+   +--        --+
            // +--                                --+
            // | f/a  0  0           0              |
            // | 0    f  0           0              |
            // | 0    0  -(zf+zn)/z  (-2.f*zf*zn)/z |
            // | 0    0  -1          0              |
            // +--                                --+
            *this = vec4x4( f/a, 0, 0, 0, 0, f, 0, 0, 0, 0, -(zf+zn)/z, (-2.f*zf*zn)/z, 0, 0, -1, 0 );
            #if e_compiling( metal )
              // +--     --+
              // | 1 0 0 0 |
              // | 0 1 0 0 |
              // | 0 0 h 0 | row major
              // | 0 0 h 1 |
              // +--     --+
              static constexpr float h = .5f;
              static const vec4x4 mprojMetal(
               1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, h, 0,
               0, 0, h, 1 );
              *this = *this * mprojMetal;
            #endif
          }
        }

      //}:                                        |
      //setAxisIdentity:{                         |

        namespace EON{
          template<> void vec4x4::setAxisIdentity(){
            YX = ZX = WX = XY = WY = YY = 0;
            XZ = WZ = XW = YW = ZW = ZZ = 0;
            XX = 1.f;
            YZ = 1.f;
            ZY = 1.f;
            WW = 1.f;
          }
        }

      //}:                                        |
      //setTranslate:{                            |

        namespace EON{
          template<> void vec4x4::setTranslation( const f32& x, const f32& y, const f32& z ){
            XX = 1; XY = 0; XZ = 0; XW = 0;
            YX = 0; YY = 1; YZ = 0; YW = 0;
            ZX = 0; ZY = 0; ZZ = 1; ZW = 0;
            WX = x; WY = y; WZ = z; WW = 1;
          }
        }

      //}:                                        |
      //setScale:{                                |

        namespace EON{
          template<> void vec4x4::setScale( const f32& x, const f32& y, const f32& z ){
            XX = x; YX = 0; ZX = 0; WX = 0;
            XY = 0; YY = y; ZY = 0; WY = 0;
            XZ = 0; YZ = 0; ZZ = z; WZ = 0;
            XW = 0; YW = 0; ZW = 0; WW = 1;
          }
        }

        namespace EON{
          template<> void vec4x4::setScale( const f32& x ){
            XX = x; YX = 0; ZX = 0; WX = 0;
            XY = 0; YY = x; ZY = 0; WY = 0;
            XZ = 0; YZ = 0; ZZ = x; WZ = 0;
            XW = 0; YW = 0; ZW = 0; WW = 1;
          }
        }

      //}:                                        |
      //setRotX:{                                 |

        namespace EON{
          template<> void vec4x4::setRotX( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            // [ 0 4      8 12 ]
            // [ 1 cos -sin 13 ]
            // [ 2 sin cos  14 ]
            // [ 3 7     11 15 ]
            t[10] = t[ 5] = cos( f );
            t[ 6] = sin( f );
            t[ 9] =-t[ 6];
            t[ 0] = t[15] = 1.f;
            t[ 1] = t[ 2] = t[ 3] = t[ 4] =
            t[ 7] = t[ 8] = t[11] = t[12] =
            t[13] = t[14] = 0.f;
          }
        }

      //}:                                        |
      //setRotY:{                                 |

        namespace EON{
          template<> void vec4x4::setRotY( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            // [ cos 4  -sin 12 ]
            // [ 1   5   9   13 ]
            // [ sin 6  cos  14 ]
            // [ 3   7  11   15 ]
            t[ 0] = t[10] = cos( f );
            t[ 2] = sin( f );
            t[ 8] =-t[ 2];
            t[ 5] = t[15] = 1.f;
            t[ 1] = t[ 3] = t[ 4] = t[ 6] =
            t[ 7] = t[ 9] = t[11] = t[12] =
            t[13] = t[14] = 0.f;
          }
        }

      //}:                                        |
      //setRotZ:{                                 |

        namespace EON{
          template<> void vec4x4::setRotZ( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            // [ cos -sin 8 12 ]
            // [ sin cos  9 13 ]
            // [ 2   6   10 14 ]
            // [ 3   7   11 15 ]
            t[ 0] = t[ 5] = cos( f );
            t[ 1] = sin( f );
            t[ 4] =-t[ 1];
            t[10] = t[15] = 1.f;
            t[ 2] = t[ 3] = t[ 6] = t[ 7] =
            t[ 8] = t[ 9] = t[11] = t[12] =
            t[13] = t[14] = 0.f;
          }
        }

      //}:                                        |
      //removeTranslation:{                       |

        namespace EON{
          template<> void vec4x4::removeTranslation(){
            WX = WY = WZ = 0;
          }
        }

      //}:                                        |
      //removeScale:{                             |

        namespace EON{
          template<> void vec4x4::removeScale(){
            Vector3 vx = Vector3( XX, YX, ZX );
            Vector3 vy = Vector3( XY, YY, ZY );
            Vector3 vz = Vector3( XZ, YZ, ZZ );
            Vector3 v;
            v.x = 1.f/vx.length();
            v.y = 1.f/vy.length();
            v.z = 1.f/vz.length();
            XX *= v.x; YX *= v.x; ZX *= v.x;
            XY *= v.y; YY *= v.y; ZY *= v.y;
            XZ *= v.z; YZ *= v.z; ZZ *= v.z;
          }
        }

      //}:                                        |
      //inverse:{                                 |

        namespace EON{
          template<> vec4x4 vec4x4::inverse()const{
            vec4x4 M;
            if( !invertMatrix( (float*)t, (float*)M.t )){
              M.setIdentity();
            }
            return M;
          }
        }

      //}:                                        |
      //invert:{                                  |

        namespace EON{
          template<> void vec4x4::invert(){
            const vec4x4 M = *this;
            if( !invertMatrix( (float*)M.t, (float*)t )){
              setIdentity();
            }
          }
        }

      //}:                                        |
    //}:                                          |
    //vec3x4:{                                    |
      //[converter]:{                             |
        //vec4x4:{                                |

          namespace EON{
            template<> template<> vec4x4 vec3x4::as<vec4x4>()const{
              vec4x4 m;
              for( u32 r=0; r<3; ++r ){
                for( u32 c=0; c<4; ++c ){
                  m( c, r ) = (*this)( r, c );
                }
              }
              return m;
            }
          }

        //}:                                      |
        //quat:{                                  |

          namespace EON{
            template<> template<> quat vec3x4::as<quat>()const{
              e_assert( valid() );
              //http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
              self qw, qx, qy, qz;
              const self tr = m[0][0] + m[1][1] + m[2][2];
              if( tr > k0 ){
                const self& s = sqrt( tr+1.f )*k2; // s=4*qw
                qw = .25 * s;
                qx = (m[1][2] - m[2][1]) / s;
                qy = (m[2][0] - m[0][2]) / s;
                qz = (m[0][1] - m[1][0]) / s;
              }else if(( m[0][0] > m[1][1] ) & ( m[0][0] > m[2][2] )){
                const self& s = sqrt( 1.f + m[0][0] - m[1][1] - m[2][2] ) * k2; // s=4*qx
                qw = (m[1][2] - m[2][1]) / s;
                qx = .25 * s;
                qy = (m[1][0] + m[0][1]) / s;
                qz = (m[2][0] + m[0][2]) / s;
              }else if( m[1][1] > m[2][2] ){
                const self& s = sqrt( 1.f + m[1][1] - m[0][0] - m[2][2] ) * k2; // S=4*qy
                qw = (m[2][0] - m[0][2]) / s;
                qx = (m[1][0] + m[0][1]) / s;
                qy = .25 * s;
                qz = (m[2][1] + m[1][2]) / s;
              }else{
                const self& s = sqrt( 1.f + m[2][2] - m[0][0] - m[1][1] ) * k2; // S=4*qz
                qw = (m[0][1] - m[1][0]) / s;
                qx = (m[2][0] + m[0][2]) / s;
                qy = (m[2][1] + m[1][2]) / s;
                qz = .25 * s;
              }
              return quat( qx, qy, qz, qw );
            }
          }

        //}:                                      |
        //vec3x3:{                                |

          namespace EON{
            template<> template<> vec3x3 vec3x4::as<vec3x3>()const{
              vec3x3 M;
              for( u32 r=0; r<3; ++r ){
                for( u32 c=0; c<3; ++c ){
                  M.m[r][c] = m[c][r];
                }
              }
              return M;
            }
          }

        //}:                                      |
        //pt3:{                                   |

          namespace EON{
            template<> template<> pt3 vec3x4::as<pt3>()const{
              e_assert( valid() );
              return toPosition();
            }
          }

        //}:                                      |
      //}:                                        |
      //[operators]:{                             |
        //operator*=:{                            |

#ifdef __APPLE__
  #pragma mark - Matrices -
#endif

          namespace EON{
            //https://software.intel.com/en-us/articles/using-simd-technologies-on-intel-architecture-to-speed-up-game-code
            template<> vec3x4& vec3x4::operator*=( const vec3x4& B ){
              vec3x4 A = *this;
              #ifdef __SSE__
                const float dummy[4]={ 0, 0, 0, 1 };
                const float* a = (float*)A.t;
                const float* b = (float*)B.t;
                float* output =  (float*)t;
                __m128 ma_col_0 = _mm_load_ps( a );
                __m128 ma_col_1 = _mm_load_ps( a + 4 );
                __m128 ma_col_2 = _mm_load_ps( a + 8 );
                __m128 ma_col_3 = _mm_load_ps( dummy );
                __m128 mb_col_0 = _mm_load_ps( b );
                __m128 mb_col_1 = _mm_load_ps( b + 4 );
                __m128 mb_col_2 = _mm_load_ps( b + 8 );
                __m128 mb_col_3 = _mm_load_ps( dummy );
                // get ready to store the result
                __m128 result0;
                __m128 result1;
                __m128 result2;
                __m128 result3;
                // result = first column of B x first row of A
                result0 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_0 ));
                result1 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_1 ));
                result2 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_2 ));
                result3 = _mm_mul_ps( mb_col_0, _mm_replicate_x_ps( ma_col_3 ));
                // result += second column of B x second row of A
                result0 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_0 ), result0 );
                result1 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_1 ), result1 );
                result2 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_2 ), result2 );
                result3 = _mm_madd_ps( mb_col_1, _mm_replicate_y_ps( ma_col_3 ), result3 );
                // result += third column of B x third row of A
                result0 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_0 ), result0 );
                result1 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_1 ), result1 );
                result2 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_2 ), result2 );
                result3 = _mm_madd_ps( mb_col_2, _mm_replicate_z_ps( ma_col_3 ), result3 );
                // result += last column of B x last row of A
                result0 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_0 ), result0 );
                result1 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_1 ), result1 );
                result2 = _mm_madd_ps( mb_col_3, _mm_replicate_w_ps( ma_col_2 ), result2 );
                // store the result to memory
                _mm_store_ps( output+0, result0 );
                _mm_store_ps( output+4, result1 );
                _mm_store_ps( output+8, result2 );
              #endif
              return *this;
            }
          }

        //}:                                      |
      //}:                                        |
      //as<>:{                                    |

        namespace EON{
          template<> template<> vec3x3 vec4x4::as<vec3x3>()const{
            vec3x3 M;
            for( u32 r=0; r<3; ++r ){
              for( u32 c=0; c<3; ++c ){
                M.m[r][c] = m[c][r];
              }
            }
            return M;
          }
        }

      //}:                                        |
      //removeTranslation:{                       |

        namespace EON{
          template<> void vec3x4::removeTranslation(){
            WX = WY = WZ = 0;
          }
        }

      //}:                                        |
      //setTranslate:{                            |

        namespace EON{
          template<> void vec3x4::setTranslation( const f32& x, const f32& y, const f32& z ){
            XX = 1; YX = 0; ZX = 0; WX = x;
            XY = 0; YY = 1; ZY = 0; WY = y;
            XZ = 0; YZ = 0; ZZ = 1; WZ = z;
          }
        }

      //}:                                        |
      //removeScale:{                             |

        namespace EON{
          template<> void vec3x4::removeScale(){
            Vector3 vx = Vector3( XX, YX, ZX );
            Vector3 vy = Vector3( XY, YY, ZY );
            Vector3 vz = Vector3( XZ, YZ, ZZ );
            Vector3 v;
            v.x = 1.f/vx.length();
            v.y = 1.f/vy.length();
            v.z = 1.f/vz.length();
            XX *= v.x; YX *= v.x; ZX *= v.x;
            XY *= v.y; YY *= v.y; ZY *= v.y;
            XZ *= v.z; YZ *= v.z; ZZ *= v.z;
          }
        }

      //}:                                        |
      //setScale:{                                |

        namespace EON{
          template<> void vec3x4::setScale( const f32& x, const f32& y, const f32& z ){
            XX = x; YX = 0; ZX = 0; WX = 0;
            XY = 0; YY = y; ZY = 0; WY = 0;
            XZ = 0; YZ = 0; ZZ = z; WZ = 0;
          }
        }

        namespace EON{
          template<> void vec3x4::setScale( const f32& x ){
            XX = x; YX = 0; ZX = 0; WX = 0;
            XY = 0; YY = x; ZY = 0; WY = 0;
            XZ = 0; YZ = 0; ZZ = x; WZ = 0;
          }
        }

      //}:                                        |
      //setRotX:{                                 |

        namespace EON{
          template<> void vec3x4::setRotX( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            const f32& c = f.cos();
            const f32& s = f.sin();
            // [ 1  0 0 0 ]
            // [ 0  c s 0 ]
            // [ 0 -s c 0 ]
            XX = 1; YX = 0; ZX = 0; WX = 0.f;
            XY = 0; YY = c; ZY = s; WY = 0.f;
            XZ = 0; YZ =-s; ZZ = c; WZ = 0.f;
          }
        }

      //}:                                        |
      //setRotY:{                                 |

        namespace EON{
          template<> void vec3x4::setRotY( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            const f32& c = f.cos();
            const f32& s = f.sin();
            // [  c 0 s 0 ]
            // [  0 1 0 0 ]
            // [ -s 0 c 0 ]
            XX = c; YX = 0; ZX = s; WX = 0;
            XY = 0; YY = 1; ZY = 0; WY = 0;
            XZ =-s; YZ = 0; ZZ = c; WZ = 0;
          }
        }

      //}:                                        |
      //setRotZ:{                                 |

        namespace EON{
          template<> void vec3x4::setRotZ( const f32& angleInDeg ){
            const f32& f = angleInDeg.rad();
            const f32& c = f.cos();
            const f32& s = f.sin();
            // [  c s 0 0 ]
            // [ -s c 0 0 ]
            // [  0 0 1 0 ]
            XX = c; YX = s; ZX = 0; WX = 0;
            XY =-s; YY = c; ZY = 0; WY = 0;
            XZ = 0; YZ = 0; ZZ = 1; WZ = 0;
          }
        }

      //}:                                        |
      //inverse:{                                 |

        namespace EON{
          template<> vec3x4 vec3x4::inverse()const{
            vec4x4 a0;
            vec4x4 a1;
            #ifdef __SSE__
              a0.v[0] = v[0];
              a0.v[1] = v[1];
              a0.v[2] = v[2];
            #else
              a0.a[0] = a[0];
              a0.a[1] = a[1];
              a0.a[2] = a[2];
            #endif
            if( !invertMatrix( (float*)a0.t, (float*)a1.t )){
              a1.setIdentity();
            }
            vec3x4 a2;
            #ifdef __SSE__
              a2.v[0] = a1.v[0];
              a2.v[1] = a1.v[1];
              a2.v[2] = a1.v[2];
            #else
              a2.a[0] = a1.a[0];
              a2.a[1] = a1.a[1];
              a2.a[2] = a1.a[2];
            #endif
            return a2;
          }
        }

      //}:                                        |
      //invert:{                                  |

        namespace EON{
          template<> void vec3x4::invert(){
            *this = inverse();
          }
        }

      //}:                                        |
    //}:                                          |
    //vec3x3:{                                    |
      //setAxisAngle:{                            |

        namespace EON{
          template<> void vec3x3::setAxisAngle( const vec3& axis, const f32& angle ){
            const f32& cs          = cos( angle );
            const f32& sn          = sin( angle );
            const f32& oneMinusCos = k1-cs;
            const f32& x2          = axis.x*axis.x;
            const f32& y2          = axis.y*axis.y;
            const f32& z2          = axis.z*axis.z;
            const f32& xym         = axis.x*axis.y*oneMinusCos;
            const f32& xzm         = axis.x*axis.z*oneMinusCos;
            const f32& yzm         = axis.y*axis.z*oneMinusCos;
            const f32& xSin        = axis.x*sn;
            const f32& ySin        = axis.y*sn;
            const f32& zSin        = axis.z*sn;
            m[0][0] = x2*oneMinusCos + cs;
            m[0][1] = xym - zSin;
            m[0][2] = xzm + ySin;
            m[1][0] = xym + zSin;
            m[1][1] = y2*oneMinusCos + cs;
            m[1][2] = yzm - xSin;
            m[2][0] = xzm - ySin;
            m[2][1] = yzm + xSin;
            m[2][2] = z2*oneMinusCos + cs;
          }
        }

      //}:                                        |
      //setScale:{                                |

        namespace EON{
          template<> void vec3x3::setScale( const f32& x, const f32& y, const f32& z ){
            XX = x; YX = 0; ZX = 0;
            XY = 0; YY = y; ZY = 0;
            XZ = 0; YZ = 0; ZZ = z;
          }
          template<> void vec3x3::setScale( const f32& s ){
            XX = s; YX = 0; ZX = 0;
            XY = 0; YY = s; ZY = 0;
            XZ = 0; YZ = 0; ZZ = s;
          }
        }

      //}:                                        |
      //setRotX:{                                 |

        namespace EON{
          template<> void vec3x3::setRotX( const f32& angleInDeg ){
            // [ 0   3    6 ]
            // [ 1 cos -sin ]
            // [ 2 sin  cos ]
            const f32& a = rad( angleInDeg );
            t[4] = t[8] = cos( a );
            t[5] = sin( a );
            t[7] =-t[5];
            t[0] = 1;
            t[3] = t[6] = t[1] = t[2] = 0;
          }
        }

      //}:                                        |
      //setRotY:{                                 |

        namespace EON{
          template<> void vec3x3::setRotY( const f32& angleInDeg ){
            // [ cos 3 -sin ]
            // [ 1   4    7 ]
            // [ sin 5  cos ]
            const f32& a = rad( angleInDeg );
            t[0] = t[8] = cos( a );
            t[2] = sin( a );
            t[6] =-t[2];
            t[4] = 1;
            t[1] = t[5] = t[7] = 0;
          }
        }

      //}:                                        |
      //setRotZ:{                                 |

        namespace EON{
          template<> void vec3x3::setRotZ( const f32& angleInDeg ){
            // [ cos -sin 6 ]
            // [ sin  cos 7 ]
            // [ 2    5   8 ]
            const f32& a = rad( angleInDeg );
            t[0] = t[4] = cos( a );
            t[1] = sin( a );
            t[3] =-t[1];
            t[8] = 1;
            t[2] = t[5] = t[6] = t[7] = 0;
          }
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
  //Frustum:{                                     |
    //intersects:{                                |

#ifdef __APPLE__
  #pragma mark - Frustums -
#endif

      namespace EON{
        template<> bool frustum::intersects( const pt3& P )const{
          const Plane* next = m_aPlanes;
          const Plane* end = next + 6;
          while( next < end ){
            const auto& t = next->signedDistance( P );
            if( t < kEpsilon ){
              return false;
            }
            ++next;
          }
          return true;
        }
      }

      namespace EON{
        template<> bool frustum::intersects( const sphere& S )const{
          const Plane* next = m_aPlanes;
          const Plane* end = next+6;
          while( next < end ){
            const f32 d = next->distance( S.toCenter() );
            if( d >= 2.f * S.toRadius() ){
              return false;
            }
            ++next;
          }
          //fully inside.
          return true;
        }
      }

    //}:                                          |
    //set:{                                       |

      namespace EON{
        template<> void frustum::set( const vec4x4& inW2C ){

          vec4x4 W2C = inW2C;

          const auto& xw = W2C[ 3];
          const auto& yw = W2C[ 7];
          const auto& zw = W2C[11];
          const auto& ww = W2C[15];
          const auto& xz = W2C[ 2];
          const auto& yz = W2C[ 6];
          const auto& zz = W2C[10];
          const auto& wz = W2C[14];

          // near
          m_aPlanes[0].x = xw - xz;
          m_aPlanes[0].y = yw - yz;
          m_aPlanes[0].z = zw - zz;
          m_aPlanes[0].w = ww - wz;

          // far
          m_aPlanes[1].x = xw + xz;
          m_aPlanes[1].y = yw + yz;
          m_aPlanes[1].z = zw + zz;
          m_aPlanes[1].w = ww + wz;

          const f32& xx = W2C[ 0];
          const f32& yx = W2C[ 4];
          const f32& zx = W2C[ 8];
          const f32& wx = W2C[12];

          // left
          m_aPlanes[2].x = xw - xx;
          m_aPlanes[2].y = yw - yx;
          m_aPlanes[2].z = zw - zx;
          m_aPlanes[2].w = ww - wx;

          // right
          m_aPlanes[3].x = xw + xx;
          m_aPlanes[3].y = yw + yx;
          m_aPlanes[3].z = zw + zx;
          m_aPlanes[3].w = ww + wx;

          const f32& xy = W2C[ 1];
          const f32& yy = W2C[ 5];
          const f32& zy = W2C[ 9];
          const f32& wy = W2C[13];

          // top
          m_aPlanes[4].x = xw + xy;
          m_aPlanes[4].y = yw + yy;
          m_aPlanes[4].z = zw + zy;
          m_aPlanes[4].w = ww + wy;

          // bottom
          m_aPlanes[5].x = xw - xy;
          m_aPlanes[5].y = yw - yy;
          m_aPlanes[5].z = zw - zy;
          m_aPlanes[5].w = ww - wy;
        }
      }

    //}:                                          |
  //}:                                            |
  //Vectors:{                                     |
    //Vector4:{                                   |
      //[operators]:{                             |

        namespace EON{
          template<> vec4 vec4::operator*( const vec4x4& M )const{
            return vec4(
              M.XX * x + M.YX * y + M.ZX * z + M.WX * w,
              M.XY * x + M.YY * y + M.ZY * z + M.WY * w,
              M.XZ * x + M.YZ * y + M.ZZ * z + M.WZ * w,
              M.XW * x + M.YW * y + M.ZW * z + M.WW * w
            );
          }
          template<> vec4 vec4::operator*( const vec3x3& M )const{
            return vec4(
              M.XX * x + M.YX * y + M.ZX * z,
              M.XY * x + M.YY * y + M.ZY * z,
              M.XZ * x + M.YZ * y + M.ZZ * z,
              f32( 1.f )
            );
          }
        }

      //}:                                        |
    //}:                                          |
    //Vector3:{                                   |
      //[operators]:{                             |

        namespace EON{
          template<> vec3 vec3::operator*( const vec4x4& M )const{
            return vec3(
              M.XX * x + M.YX * y + M.ZX * z,
              M.XY * x + M.YY * y + M.ZY * z,
              M.XZ * x + M.YZ * y + M.ZZ * z
            );
          }
          template<> vec3 vec3::operator*( const vec3x3& M )const{
            return vec3(
              M.XX * x + M.YX * y + M.ZX * z,
              M.XY * x + M.YY * y + M.ZY * z,
              M.XZ * x + M.YZ * y + M.ZZ * z
            );
          }
        }

      //}:                                        |
    //}:                                          |
    //Vector2:{                                   |
      //[operators]:{                             |
        //vec2:{                                  |

          namespace EON{
            template<> vec2  vec2::operator+ ( const vec2& v )const{ return vec2( x+v.x, y+v.y ); }
            template<> vec2  vec2::operator- ( const vec2& v )const{ return vec2( x-v.x, y-v.y ); }
            template<> vec2  vec2::operator/ ( const vec2& v )const{ return vec2( x/v.x, y/v.y ); }
            template<> vec2  vec2::operator* ( const vec2& v )const{ return vec2( x*v.x, y*v.y ); }
            template<> vec2& vec2::operator+=( const vec2& v ){ x+=v.x; y+=v.y; return*this; }
            template<> vec2& vec2::operator-=( const vec2& v ){ x-=v.x; y-=v.y; return*this; }
            template<> vec2& vec2::operator/=( const vec2& v ){ x/=v.x; y/=v.y; return*this; }
            template<> vec2& vec2::operator*=( const vec2& v ){ x*=v.x; y*=v.y; return*this; }
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
  //}:                                            |
  //QST:{                                         |
    //get?x?*:{                                   |

#ifdef __APPLE__
  #pragma mark - QSTs -
#endif

      namespace EON{
        template<> vec4x4 qst3::get4x4()const{
          const vec4x4& R = toRotation().get4x4();
          const vec4x4& T = vec4x4::getT(
              toPosition() );
          const vec4x4& S = vec4x4::getS(
              toScale()
            , toScale()
            , toScale() );
          return( S * R * T );
        }
        template<> vec3x4 qst3::get3x4()const{
          const vec3x4& R = toRotation().get3x4();
          const vec3x4& T = vec3x4::getT(
              toPosition() );
          const vec3x4& S = vec3x4::getS(
              toScale()
            , toScale()
            , toScale() );
          return( T * R * S );
        }
        template<> vec3x3 qst3::get3x3()const{
          const vec3x3 S( m_fScale );
          return S * toRotation().get3x3();
        }
        template<> vec4x4 qst3::get4x4RT()const{
          const vec4x4& T = vec4x4::getT( m_tPosition );
          const vec4x4& R = toRotation().get4x4();
          return R * T;
        }
        template<> vec3x4 qst3::get3x4RT()const{
          const vec3x4& T = vec3x4::getT( m_tPosition );
          const vec3x4& R = toRotation().get3x4();
          return T * R;
        }
        template<> vec4x4 qst3::get4x4ST()const{
          const vec4x4& T = vec4x4::getT( m_tPosition );
          const vec4x4& S = vec4x4( m_fScale );
          return S * T;
        }
        template<> vec3x4 qst3::get3x4ST()const{
          const vec3x4& T = vec3x4::getT( m_tPosition );
          const vec3x4& S = vec3x4( m_fScale );
          return T * S;
        }
        template<> vec4x4 qst3::get4x4T()const{
          return vec4x4::getT( m_tPosition );
        }
        template<> vec3x4 qst3::get3x4T()const{
          return vec3x4::getT( m_tPosition );
        }
        template<> vec4x4 qst3::get4x4R()const{
          return toRotation().get4x4();
        }
        template<> vec3x4 qst3::get3x4R()const{
          return toRotation().get3x4();
        }
        template<> vec3x3 qst3::get3x3R()const{
          return toRotation().get3x3();
        }
        template<> vec4x4 qst3::get4x4S()const{
          return vec4x4( m_fScale );
        }
        template<> vec3x4 qst3::get3x4S()const{
          return vec3x4( m_fScale );
        }
        template<> vec3x3 qst3::get3x3S()const{
          return vec3x3( m_fScale );
        }
      }

    //}:                                          |
    //invert:{                                    |

      namespace EON{
        template<> void qst3::invert(){
          toPosition().negate();
          toRotation().invert();
        }
      }

    //}:                                          |
    //[operators]:{                               |
      //Multiply:{                                |

        namespace EON{
          template<> pt3 qst3::operator*( const pt3& P )const{
            return P * get4x4();
          }
        }

        namespace EON{
          template<> void qst3::operator*=( const qst3& L2W ){
            x() += L2W.m_tPosition.x;
            y() += L2W.m_tPosition.y;
            z() += L2W.m_tPosition.z;
            quat q = L2W.m_qRotation;
            q.normalize();
            m_qRotation *= q;
            m_qRotation.normalize();
            m_fScale *= L2W.m_fScale;
          }
        }

      //}:                                        |
      //Array:{                                   |

        namespace EON{
          template<> f32 qst3::operator[]( const u32 i )const{
            e_assert( i < 7 );
            f32 euler[ 3 ];
            switch( i ){
              case 0:
              case 1:
              case 2:
                return m_tPosition[ i ];
              case 3:
              case 4:
              case 5:
                m_qRotation.getEuler( euler[ 0 ], euler[ 1 ], euler[ 2 ]);
                return euler[i-3];
            }
            return m_fScale;
          }
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Bounds:{                                        |
  //aabb3:{                                       |
    //intersects:{                                |

#ifdef __APPLE__
  #pragma mark -
  #pragma mark Bounding volumes
  #pragma mark -
#endif

      //http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
      namespace EON{
        template<> bool aabb3::intersects( const ray3& inRay, f32& outDst )const{
          vec3 dirfrac;
          const pt3& lb = min;
          const pt3& rt = max;
          // r.dir is unit direction vector of ray
          const vec3 d = inRay.d.normalized();
          dirfrac.x = 1.f / d.x;
          dirfrac.y = 1.f / d.y;
          dirfrac.z = 1.f / d.z;
          // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
          // r.org is origin of ray
          const f32& t1 = (lb.x - inRay.p.x)*dirfrac.x;
          const f32& t2 = (rt.x - inRay.p.x)*dirfrac.x;
          const f32& t3 = (lb.y - inRay.p.y)*dirfrac.y;
          const f32& t4 = (rt.y - inRay.p.y)*dirfrac.y;
          const f32& t5 = (lb.z - inRay.p.z)*dirfrac.z;
          const f32& t6 = (rt.z - inRay.p.z)*dirfrac.z;
          const f32& tmin = f32::max( f32::max( t1.min( t2 ), t3.min( t4 )), t5.min( t6 ));
          const f32& tmax = f32::min( f32::min( t1.max( t2 ), t3.max( t4 )), t5.max( t6 ));
          // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
          if( tmax < 0.f ){
            outDst = tmax;
            return false;
          }
          // if tmin > tmax, ray doesn't intersect AABB
          if( tmin > tmax ){
            outDst = tmax;
            return false;
          }
          outDst = tmin;
          return true;
        }
      }

    //}:                                          |
    //contains:{                                  |

      namespace EON{
        template<> bool aabb3::contains( const pt3& P )const{
          if( P.x < min.x ){
            return false;
          }
          if( P.y < min.y ){
            return false;
          }
          if( P.z < min.z ){
            return false;
          }
          if( P.x > max.x ){
            return false;
          }
          if( P.y > max.y ){
            return false;
          }
          if( P.z > max.z ){
            return false;
          }
          return true;
        }
      }

      namespace EON{
        template<> bool aabb3::contains( const aabb3& B )const{
          if( contains( B.min ) || contains( B.max )){
            return true;
          }
          return false;
        }
      }

      namespace EON{
        template<> bool aabb3::contains( const triangle& t )const{
          u32 n = 0;
          if( contains( t.A )){
            ++n;
          }
          if( contains( t.B )){
            ++n;
          }
          if( contains( t.C )){
            ++n;
          }
          return( n >= 2 );
        }
      }

    //}:                                          |
    //empty:{                                     |

      namespace EON{
        template<> bool aabb3::empty()const{
          e_assert( min.valid() );
          e_assert( max.valid() );
          if( min.x != kMax ){
            return false;
          }
          if( min.y != kMax ){
            return false;
          }
          if( max.x != kMin ){
            return false;
          }
          if( max.y != kMin ){
            return false;
          }
          if( max.z != kMin ){
            return false;
          }
          return true;
        }
      }

    //}:                                          |
    //valid:{                                     |

      namespace EON{
        template<> bool aabb3::valid()const{
          if( !min.valid() ){
            return false;
          }
          if( !max.valid() ){
            return false;
          }
          if( !empty() ){
            if( min.x > max.x ){
              return false;
            }
            if( min.y > max.y ){
              return false;
            }
            if( min.z > max.z ){
              return false;
            }
          }
          return true;
        }
      }

    //}:                                          |
    //clear:{                                     |

      namespace EON{
        template<> void aabb3::reset(){
          max.x = kMin;
          max.y = kMin;
          max.z = kMin;
          min.x = kMax;
          min.y = kMax;
          min.z = kMax;
        }
      }

    //}:                                          |
  //}:                                            |
  //aabb2:{                                       |
    //scaledToClip:{                              |

      namespace EON{
        template<> aabb2 aabb2::scaledToClip( const f32& scale )const{
          aabb2 b = *this;
          b.min.scaleToClip( scale );
          b.max.scaleToClip( scale );
          return b;
        }
      }

    //}:                                          |
    //scaleToClip:{                               |

      namespace EON{
        template<> void aabb2::scaleToClip( const f32& scale ){
          min.scaleToClip( scale );
          max.scaleToClip( scale );
        }
      }

    //}:                                          |
    //scaleBack:{                                 |

      namespace EON{
        template<> void aabb2::scaleBack(){
          const f32& cx = 640.f;
          const f32& cy = 360.f;
          min.x = (min.x+1.f)*.5f*cx;
          min.y = (min.y+1.f)*.5f*cy;
          max.x = (max.x+1.f)*.5f*cx;
          max.y = (max.y+1.f)*.5f*cy;
        }
      }

    //}:                                          |
    //empty:{                                     |

      namespace EON{
        template<> bool aabb2::empty()const{
          e_assert( min.valid() );
          e_assert( max.valid() );
          if( min.x != kMax ){
            return false;
          }
          if( min.y != kMax ){
            return false;
          }
          if( max.x != kMin ){
            return false;
          }
          if( max.y != kMin ){
            return false;
          }
          return true;
        }
      }

    //}:                                          |
    //valid:{                                     |

      namespace EON{
        template<> bool aabb2::valid()const{
          if( !min.valid() ){
            return false;
          }
          if( !max.valid() ){
            return false;
          }
          if( !empty() ){
            if( min.x > max.x ){
              return false;
            }
            if( min.y > max.y ){
              return false;
            }
          }
          return true;
        }
      }

    //}:                                          |
    //contains:{                                  |

      namespace EON{
        template<> bool aabb2::contains( const pt2& P )const{
          return(( P.x >= min.x )&&( P.y >= min.y )&&( P.x <= max.x )&&( P.y <= max.y ));
        }
      }

      namespace EON{
        template<> bool aabb2::contains( const triangle& t )const{
          if( !contains( t.A.xy() )){
            return false;
          }
          if( !contains( t.B.xy() )){
            return false;
          }
          if( !contains( t.C.xy() )){
            return false;
          }
          return true;
        }
      }

      namespace EON{
        template<> bool aabb2::contains( const aabb2& B )const{
          return( contains( B.min ) && contains( B.max ));
        }
      }

    //}:                                          |
    //intersects:{                                |

      namespace EON{
        template<> bool aabb2::intersects( const ray2& r )const{
          const pt2 v[ 2 ]={ r.p, r.p + r.d };
          return line2rect( v, *this );
        }
      }

      namespace EON{
        template<> bool aabb2::intersects( const aabb2& b )const{

          //--------------------------------------------------------------------
          // Get all the corners.
          //--------------------------------------------------------------------

          const pt2 bA = b.min;
          const pt2 bB = pt2( b.max.x, b.min.y );
          const pt2 bC = b.max;
          const pt2 bD = pt2( b.min.x, b.max.y );

          //--------------------------------------------------------------------
          // Do any of b points appear in this?
          //--------------------------------------------------------------------

          if( contains( bA )){
            return true;
          }
          if( contains( bB )){
            return true;
          }
          if( contains( bC )){
            return true;
          }
          if( contains( bD )){
            return true;
          }

          //--------------------------------------------------------------------
          // Get all the edges.
          //--------------------------------------------------------------------

          const ray2 bEdge0( bA, bB-bA );
          const ray2 bEdge1( bB, bC-bB );
          const ray2 bEdge2( bC, bD-bC );
          const ray2 bEdge3( bD, bA-bD );
          const ray2 bDiag0( bA, bC-bA );
          const ray2 bDiag1( bB, bD-bB );

          //--------------------------------------------------------------------
          // Do any lines intersect this?
          //--------------------------------------------------------------------

          if( intersects( bEdge0 )){
            return true;
          }
          if( intersects( bEdge1 )){
            return true;
          }
          if( intersects( bEdge2 )){
            return true;
          }
          if( intersects( bEdge3 )){
            return true;
          }
          if( intersects( bDiag0 )){
            return true;
          }
          if( intersects( bDiag1 )){
            return true;
          }
          return false;
        }
      }

    //}:                                          |
    //clipAgainst:{                               |

      namespace EON{
        template<> bool aabb2::clipAgainst( const aabb2& b ){
          if( b.intersects( *this )){
            if( min.x < b.min.x ){
              min.x = b.min.x;
            }
            if( min.y < b.min.y ){
              min.y = b.min.y;
            }
            if( max.x > b.max.x ){
              max.x = b.max.x;
            }
            if( max.y > b.max.y ){
              max.y = b.max.y;
            }
            return valid();
          }
          return false;
        }
      }

    //}:                                          |
    //circle:{                                    |

      namespace EON{
        template<> circle aabb2::circle()const{
          const self x = (max.x - min.x) * .5f;
          const self y = (max.y - min.y) * .5f;
          return Circle( min.x + x, min.y + y, x.max( y ));
        }
      }

    //}:                                          |
    //clear:{                                     |

      namespace EON{
        template<> void aabb2::reset(){
          max.x = kMin;
          max.y = kMin;
          min.x = kMax;
          min.y = kMax;
        }
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Random:{                                        |
  //e_rand<>:{                                    |

#ifdef __APPLE__
  #pragma mark - Random -
#endif

    namespace{
      std::mt19937 kGen32;
    }

    template<> f32 e_rand<f32>( const f32 start, const f32 end ){
      std::uniform_real_distribution<float> dis(( f32::cast( start )), f32::cast( end ));
      return dis( kGen32 );
    }

    template<> f32 e_rand<f32>(){
      std::uniform_real_distribution<float> dis;
      return dis( kGen32 );
    }

    template<> float e_rand<float>( float start, float end ){
      std::uniform_real_distribution<float> dis( start, end );
      return dis( kGen32 );
    }

    template<> float e_rand<float>(){
      std::uniform_real_distribution<float> dis;
      return dis( kGen32 );
    }

    template<> u32 e_rand<u32>( const u32 start, const u32 end ){
      std::uniform_int_distribution<u32> dis( start, end );
      return dis( kGen32 );
    }

    template<> u32 e_rand<u32>(){
      std::uniform_int_distribution<s32> dis;
      return dis( kGen32 );
    }

    template<> s32 e_rand<s32>( const s32 start, const s32 end ){
      std::uniform_int_distribution<s32> dis( start, end );
      return dis( kGen32 );
    }

    template<> s32 e_rand<s32>(){
      std::uniform_int_distribution<s32> dis;
      return dis( kGen32 );
    }

    template<> u8 e_rand<u8>( const u8 start, const u8 end ){
      std::uniform_int_distribution<int> dis( start, end );
      return dis( kGen32 );
    }

    template<> u8 e_rand<u8>(){
      std::uniform_int_distribution<int> dis;
      return dis( kGen32 );
    }

  //}:                                            |
  //e_randunitvec*:{                              |

    vec2 e_randunitvec2(){
      const f32& t = e_rand2pi();
      const f32& s = t.sin();
      const f32& c = t.cos();
      return vec2( c, s );
    }

    vec3 e_randunitvec3(){
      const f32& theta = e_rand2pi();
      const f32& r = e_randunit().sqrt();
      const f32& z = f32::sqrt( 1.f - r*r )*( e_rand<u32>( 0, 1 ) ? -1.f : 1.f );
      return vec3( r * theta.cos(), r * theta.sin(), z );
    }

  //}:                                            |
  //e_randunit:{                                  |

    f32 e_randunit(){
      return e_rand<f32>( 0.f, 1.f );
    }

  //}:                                            |
  //e_rand2pi:{                                   |

    f32 e_rand2pi(){
      return e_randpi()*2.f;
    }

  //}:                                            |
  //e_randpi:{                                    |

    f32 e_randpi(){
      return e_randunit() * f32::kPI;
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Color:{                                         |
  //Methods:{                                     |
    //multiply:{                                  |

#ifdef __APPLE__
  #pragma mark - Color -
#endif

      namespace EON{
        template<> f32 rgba::multiply( const f32& base, const f32& blend ){
          return( base*blend );
        }
      }

    //}:                                          |
    //lighten:{                                   |

      namespace EON{
        template<> f32 rgba::lighten( const f32& base, const f32& blend ){
          return max( base, blend );
        }
      }

    //}:                                          |
    //darken:{                                    |

      namespace EON{
        template<> f32 rgba::darken( const f32& base, const f32& blend ){
          return min( base, blend );
        }
      }

    //}:                                          |
    //average:{                                   |

      namespace EON{
        template<> f32 rgba::average( const f32& base, const f32& blend ){
          return( base+blend )*kHalf;
        }
      }

    //}:                                          |
    //linearDodge:{                               |

      namespace EON{
        template<> f32 rgba::linearDodge( const f32& base, const f32& blend ){
          return self::saturate( base+blend );
        }
      }

    //}:                                          |
    //linearBurn:{                                |

      namespace EON{
        template<> f32 rgba::linearBurn( const f32& base, const f32& blend ){
          return self::saturate( base-blend );
        }
      }

    //}:                                          |
    //difference:{                                |

      namespace EON{
        template<> f32 rgba::difference( const f32& base, const f32& blend ){
          return abs( base-blend );
        }
      }

    //}:                                          |
    //negation:{                                  |

      namespace EON{
        template<> f32 rgba::negation( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          return( k1 - abs( k1 - base - blend ));
        }
      }

    //}:                                          |
    //screen:{                                    |

      namespace EON{
        template<> f32 rgba::screen( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          return 1.f-(( 1.f - base )*( 1.f - blend ));
        }
      }

    //}:                                          |
    //exclusion:{                                 |

      namespace EON{
        template<> f32 rgba::exclusion( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          return( base+blend-k2*base*blend );
        }
      }

    //}:                                          |
    //overlay:{                                   |

      namespace EON{
        template<> f32 rgba::overlay( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          return( base <kHalf ) ?( 2.f * base * blend ) :( 1.f - 2.f*( 1.f - base )*( 1.f - blend ));
        }
      }

    //}:                                          |
    //hardLight:{                                 |

      namespace EON{
        template<> f32 rgba::hardLight( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          return( blend <kHalf ) ?( k2 * blend * base ) :( k1 - k2 *( k1 - blend ) *( k1 - base ));
        }
      }

    //}:                                          |
    //softLight:{                                 |

      namespace EON{
        template<> f32 rgba::softLight( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          if( blend < kHalf ){
            return( k2 * base * blend + base*base *( k1 - k2 * blend ));
          }
          return( sqrt( base ) * ( k2 * blend - k1 ) + k2 * base * ( k1 - blend ));
        }
      }

    //}:                                          |
    //softDodge:{                                 |

      namespace EON{
        template<> f32 rgba::softDodge( const f32& base, const f32& blend ){
          return(( base + blend ) < 1.f ) ? (( base * .5f ) / ( 1.f - blend )) : ( 1.f - (( .5f *( 1.f - blend )) / base ));
        }
      }

    //}:                                          |
    //colorDodge:{                                |

      namespace EON{
        template<> f32 rgba::colorDodge( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          if( blend != k1 ){
            return min( base/( k1 - blend ), k1 );
          }
          return k1;
        }
      }

    //}:                                          |
    //colorBurn:{                                 |

      namespace EON{
        template<> f32 rgba::colorBurn( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          if( blend > k0 ){
            return max(( k1 -(( k1 - base ) / blend )), k0 );
          }
          return blend;
        }
      }

    //}:                                          |
    //linearLight:{                               |

      namespace EON{
        template<> f32 rgba::linearLight( const f32& base, const f32& blend ){
          return( blend < kHalf ) ? linearBurn( base, k2*blend ) : linearDodge( base, k2*( blend-kHalf ));
        }
      }

    //}:                                          |
    //vividLight:{                                |

      namespace EON{
        template<> f32 rgba::vividLight( const f32& base, const f32& blend ){
          return( blend < kHalf ) ? colorBurn( base,( k2 * blend )) : colorDodge( base,( k2 *( blend -kHalf )));
        }
      }

    //}:                                          |
    //pinLight:{                                  |

      namespace EON{
        template<> f32 rgba::pinLight( const f32& base, const f32& blend ){
          return( blend < kHalf ) ? darken( base,( k2 * blend )) : lighten( base,( k2 *( blend -kHalf )));
        }
      }

    //}:                                          |
    //hardMix:{                                   |

      namespace EON{
        template<> f32 rgba::hardMix( const f32& base, const f32& blend ){
          return( vividLight( base, blend ) < kHalf ) ? k0 : k1;
        }
      }

    //}:                                          |
    //reflection:{                                |

      namespace EON{
        template<> f32 rgba::reflect( const f32& in_fBase, const f32& in_fBlend ){
          const f32& blend = self::saturate( in_fBlend );
          const f32& base  = self::saturate( in_fBase  );
          if( blend != k1 ){
            return min( base*base/( k1 - blend ), k1 );
          }
          return k1;
        }
      }

    //}:                                          |
    //glow:{                                      |

      namespace EON{
        template<> f32 rgba::glow( const f32& base, const f32& blend ){
          if( base != k1 ){
            return min( blend*blend/( k1 - base ), k1 );
          }
          return k1;
        }
      }

    //}:                                          |
    //pheonix:{                                   |

      namespace EON{
        template<> f32 rgba::phoenix( const f32& base, const f32& blend ){
          return min( base, blend ) - max( base, blend ) + k1;
        }
      }

    //}:                                          |
    //setHSV:{                                    |

      namespace EON{
        //https://www.programmingalgorithms.com/algorithm/hsv-to-rgb?lang=C%2B%2B
        //http://www.tech-faq.com/hsv.html
        template<> void rgba::setHSV( const f32& H, const f32& S, const f32& V ){
          f32 h( H );
          f32 s( S );
          f32 v( V );
          if( !s ){
            r = v;
            g = v;
            b = v;
          }else{
            f32 f, p, q, t;
            if( h == 360.f ){
              h = 0;
            }else{
              h = h / 60.f;
            }
            s32 i = s32( truncf( f32::cast( h )));
            f = h - i;
            p = v * ( 1.f - s );
            q = v * ( 1.f - ( s * f ));
            t = v * ( 1.f - ( s * ( 1.f-f )));
            switch( i ){
              case 0:
                r = v;
                g = t;
                b = p;
                break;
              case 1:
                r = q;
                g = v;
                b = p;
                break;
              case 2:
                r = p;
                g = v;
                b = t;
                break;
              case 3:
                r = p;
                g = q;
                b = v;
                break;
              case 4:
                r = t;
                g = p;
                b = v;
                break;
              default:
                r = v;
                g = p;
                b = q;
                break;
            }
          }
        }
      }

    //}:                                          |
    //hsv:{                                       |

      namespace EON{
        //http://www.tech-faq.com/hsv.html
        template<> hsva rgba::hsv()const{
          hsva result;
          f32 max = r;
          if( max < g ) max = g;
          if( max < b ) max = b;
          f32 min = r;
          if( min > g ) min = g;
          if( min > b ) min = b;
          result.h = 0.f;
          if( max == min ){
            result.h = 0.f;
          }else if( max == r ){
            result.h = 60.f * (g - b)/(max - min);
            if( result.h < 0.f ){
              result.h += 360.f;
            }
            if( result.h >= 360.f ){
              result.h -= 360.f;
            }
          }else if( max == g ){
            result.h = 60.f*( b-r )/( max-min )+120.f;
          }else if( max == b ){
            result.h = 60.f*( r-g )/( max-min )+240.f;
          }
          if( !max ){
            result.s = 0;
          }else{
            result.s = 1.f-( min/max );
          }
          result.v = max;
          return result;
        }
      }

    //}:                                          |
  //}:                                            |
  //Ctor:{                                        |

    namespace EON{
      template<> void rgba::set( const u32 inBGRA ){
        const bgra v( inBGRA );
        r = f32( v.r )/255.f;
        g = f32( v.g )/255.f;
        b = f32( v.b )/255.f;
        a = f32( v.a )/255.f;
      }
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Triangle:{                                      |
  //intersects:{                                  |

#ifdef __APPLE__
  #pragma mark - Geometry -
#endif

    namespace EON{
      template<> bool triangle::intersects( const vec4x4& L2W, const ray3& ray, pt3& hit, vec3& norm )const{
        const auto& v0 = A * L2W;
        const auto& v1 = B * L2W;
        const auto& v2 = C * L2W;
        #if 1 //http://geomalgorithms.com/a06-_intersect-2.html (C++fied by BpH)
          const auto& u = vec3( v1 - v0 );
          const auto& v = vec3( v2 - v0 );
          const auto& n = u.cross( v );
          if( !n.length() ){
            return false;
          }
          const auto& dir = ray.d;
          const auto& w0  = vec3( ray.p - v0 );
          const auto&  a  = -n.dot( w0 );
          const auto&  b  = n.dot( dir );
          if( b.abs() < 0.00000001f ){
            return false;
          }
          const auto& r = a / b;
          if( r < 0.f ){
            return false;
          }
          hit = ray.p + r * dir;
          norm = N;

          const auto& uu = u.dot( u );
          const auto& uv = u.dot( v );
          const auto& vv = v.dot( v );
          const auto& w = vec3( hit - v0 );
          const auto& wu = w.dot( u );
          const auto& wv = w.dot( v );
          const auto&  D = uv * uv - uu * vv;

          // get and test parametric coords
          const auto& s = (uv * wv - vv * wu) / D;
          if( s < 0.f || s > 1.f ){
            return false;
          }
          const auto& t = (uv * wu - uu * wv) / D;
          if(( t < 0.f )||(( s + t )> 1.f )){
            return false;
          }
          return true;
        #elif 0 // smaller sequence from tiny-gizmos: faster?
          const auto& e1 = vec3( v1 - v0 );
          const auto& e2 = vec3( v2 - v0 );
          const auto& hh = ray.d.cross( e2 );
          auto a = e1.dot( hh );
          if( !a.abs() ){
            return false;
          }
          const auto  f = 1.f / a;
          const auto& s = vec3( ray.p - v0 );
          const auto  u = f * s.dot( hh );
          if(( u < 0 )||( u > 1 )){
            return false;
          }
          const auto& q = s.cross( e1 );
          const auto& v = f * ray.d.dot( q );
          if(( v < 0 )||( u + v > 1 )){
            return false;
          }
          const auto t = f * e2.dot( q );
          if( t < 0 ){
            return false;
          }
          hit = ray.at( t, true );
          return true;
        #else
          const auto& diff   = vec3( ray.p - v0 );
          const auto& edge1  = vec3( v1 - v0 );
          const auto& edge2  = vec3( v2 - v0 );
          const auto& normal = edge1.cross( edge2 );
          f32 DdN = ray.d.dot( normal );
          f32 sign;
          if( DdN > .001f ){
            sign = 1.f;
          }else if( DdN < -.001f ){
            sign = -1.f;
            DdN = -DdN;
          }else{
            return false;
          }
          const f32& DdQxE2 = sign*ray.d.dot( diff.cross( edge2 ));
          if( DdQxE2 >= 0.f ){
            const f32& DdE1xQ = sign*ray.d.dot( edge1.cross( diff ));
            if( DdE1xQ >= 0.f ){
              if( DdQxE2 + DdE1xQ <= DdN ){
                const f32& QdN = -sign * diff.dot( normal );
                if( QdN >= 0.f ){
                  const f32& inv = 1.f / DdN;
                  const f32& par = QdN * inv;
                  hit.x = ray.p.x + ray.d.x * par;
                  hit.y = ray.p.y + ray.d.y * par;
                  hit.z = ray.p.z + ray.d.z * par;
                  calcNormal();
                  norm.x = N.x;
                  norm.y = N.y;
                  norm.z = N.z;
                  return true;
                }
              }
            }
          }
          return false;
        #endif
      }
      template<> bool triangle::intersects( const qst3& L2W, const ray3& ray, pt3& hit, vec3& norm )const{
        return intersects( L2W.get4x4(), ray, hit, norm );
      }
      template<> bool triangle::intersects( const ray3& ray, pt3& hit, vec3& norm )const{
        return intersects( vec4x4(), ray, hit, norm );
      }
    }

  //}:                                            |
  //toBounds2D:{                                  |

    namespace EON{
      template<> aabb2 triangle::toBounds2D()const{
        aabb2 bounds;
        bounds += pt2( A.x, A.y );
        bounds += pt2( B.x, B.y );
        bounds += pt2( C.x, C.y );
        return bounds;
      }
    }

  //}:                                            |
  //toBounds:{                                |

    namespace EON{
      template<> aabb3 triangle::toBounds()const{
        aabb3 bounds;
        bounds += A;
        bounds += B;
        bounds += C;
        return bounds;
      }
    }

  //}:                                        |
  //draw:{                                        |

    namespace EON{
      template<> bool triangle::draw( IRasterizer& rasterizer, const bool bTestPrimitive )const{
        #if /* DISABLES CODE */ (1)
          const u32 w = rasterizer.toW();
          const u32 h = rasterizer.toH();
          aabb2 b;
          b += pt2( A.x, A.y );
          b += pt2( B.x, B.y );
          b += pt2( C.x, C.y );
          b.snap();
          const vec3 vs1( B.x-A.x, B.y-A.y );
          const vec3 vs2( C.x-A.x, C.y-A.y );

          //--------------------------------------------------------------------
          // Test that triangle doesn't overwrite pixels.
          //--------------------------------------------------------------------

          if( bTestPrimitive ){
            for( s32 y=b.min.y.as<s32>(); y<=b.max.y.as<s32>(); y++ ){
              for( s32 x=b.min.x.as<s32>(); x<=b.max.x.as<s32>(); x++ ){
                if(( x >= 0 )&&( x < s32( w ))&&( y >= 0 )&&( y < s32( h ))){
                  const vec3 q( x-A.x, y-A.y );
                  const f32 s = (q.cross( vs2 ) / vs1.cross( vs2 )).length();
                  const f32 t = (vs1.cross( q ) / vs1.cross( vs2 )).length();
                  if(( s >= 0.f ) && ( t >= 0.f ) && ( s+t <= 1.f )){
                    if( rasterizer.onTest( x, y )){
                      return false;
                    }
                  }
                }
              }
            }
          }

          //--------------------------------------------------------------------
          // Render triangle using rasterizer.
          //--------------------------------------------------------------------

          for( s32 y=b.min.y.as<s32>(); y<=b.max.y.as<s32>(); y++ ){
            for( s32 x=b.min.x.as<s32>(); x<=b.max.x.as<s32>(); x++ ){
              if(( x >= 0 )&&( x < s32( w ))&&( y >= 0 )&&( y < s32( h ))){
                const vec3 q( x-A.x, y-A.y );
                const f32 s = (q.cross( vs2 ) / vs1.cross( vs2 )).length();
                const f32 t = (vs1.cross( q ) / vs1.cross( vs2 )).length();
                if(( s >= 0.f ) && ( t >= 0.f ) && ( s+t <= 1.f )){
                  const f32& u = f32( x )/f32( w );
                  const f32& v = f32( y )/f32( h );
                  rasterizer.onDraw( x, y, vec2( u, v ));
                }
              }
            }
          }
        #else
          // 28.4 fixed-point coordinates
          const int Y1 = int( 16.0f * A.y+.5f );
          const int Y2 = int( 16.0f * B.y+.5f );
          const int Y3 = int( 16.0f * C.y+.5f );

          const int X1 = int( 16.0f * A.x+.5f );
          const int X2 = int( 16.0f * B.x+.5f );
          const int X3 = int( 16.0f * C.x+.5f );

          // Deltas
          const int DX12 = X1 - X2;
          const int DX23 = X2 - X3;
          const int DX31 = X3 - X1;

          const int DY12 = Y1 - Y2;
          const int DY23 = Y2 - Y3;
          const int DY31 = Y3 - Y1;

          // Fixed-point deltas
          const int FDX12 = DX12 << 4;
          const int FDX23 = DX23 << 4;
          const int FDX31 = DX31 << 4;

          const int FDY12 = DY12 << 4;
          const int FDY23 = DY23 << 4;
          const int FDY31 = DY31 << 4;

          // Bounding rectangle
          int minx = (imin3( X1, X2, X3 )+0xF) >> 4;
          int maxx = (imax3( X1, X2, X3 )+0xF) >> 4;
          int miny = (imin3( Y1, Y2, Y3 )+0xF) >> 4;
          int maxy = (imax3( Y1, Y2, Y3 )+0xF) >> 4;

          // Block size, standard 8x8 (must be power of two)
          const int q = 8;

          // Start in corner of 8x8 block
          minx &= ~(q - 1);
          miny &= ~(q - 1);

          // Clipping
          if( minx < 0 ){
            minx = 0;
          }
          if( miny < 0 ){
            miny = 0;
          }

          // Half-edge constants
          int C1 = DY12 * X1 - DX12 * Y1;
          int C2 = DY23 * X2 - DX23 * Y2;
          int C3 = DY31 * X3 - DX31 * Y3;

          // Correct for fill convention
          if( DY12 < 0 || (DY12 == 0 && DX12 > 0 )) C1++;
          if( DY23 < 0 || (DY23 == 0 && DX23 > 0 )) C2++;
          if( DY31 < 0 || (DY31 == 0 && DX31 > 0 )) C3++;

          //--------------------------------------------------------------------
          // Test that triangle doesn't overwrite pixels.
          //--------------------------------------------------------------------

          if( bTestPrimitive ){
            for( int y = miny; y < maxy; y += q ){
              for( int x = minx; x < maxx; x += q ){
                // Corners of block
                int x0 = x << 4;
                int x1 = (x + q - 1) << 4;
                int y0 = y << 4;
                int y1 = (y + q - 1) << 4;

                // Evaluate half-space functions
                bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
                bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
                bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
                bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
                int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

                bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
                bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
                bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
                bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
                int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

                bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
                bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
                bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
                bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
                int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

                // Skip block when outside an edge
                if( !a || !b || !c ){
                  continue;
                }

                // Accept whole block when totally covered
                if( a == 0xF && b == 0xF && c == 0xF ){
                  for( int iy=0; iy<q; ++iy ){
                    const real h = real( rasterizer.toH() );
                    if( y+iy>=h ){
                      break;
                    }
                    for( int ix=x; ix<x+q; ++ix ){
                      const real w = real( rasterizer.toW() );
                      if( ix>=w ){
                        break;
                      }
                      if( ix<0 ){
                        continue;
                      }
                      if( rasterizer.onTest( ix, y+iy )){
                        return false;
                      }
                    }
                  }
                  continue;
                }

                // Partially covered block
                int CY1 = C1 + DX12 * y0 - DY12 * x0;
                int CY2 = C2 + DX23 * y0 - DY23 * x0;
                int CY3 = C3 + DX31 * y0 - DY31 * x0;
                for( int iy=y; iy<y+q; ++iy ){
                  const real h = real( rasterizer.toH() );
                  if( iy>=h ){
                    break;
                  }
                  int CX1 = CY1;
                  int CX2 = CY2;
                  int CX3 = CY3;
                  for( int ix=x; ix<x+q; ++ix ){
                    if( CX1 > 0 && CX2 > 0 && CX3 > 0 ){
                      const real w = real( rasterizer.toW() );
                      if( ix>=w ){
                        break;
                      }
                      if( ix<0 ){
                        continue;
                      }
                      if( rasterizer.onTest( ix, iy )){
                        return false;
                      }
                    }
                    CX1 -= FDY12;
                    CX2 -= FDY23;
                    CX3 -= FDY31;
                  }
                  CY1 += FDX12;
                  CY2 += FDX23;
                  CY3 += FDX31;
                }
              }
            }
          }

          //--------------------------------------------------------------------
          // Render triangle using rasterizer.
          //--------------------------------------------------------------------

          // Loop through blocks
          for( int y = miny; y < maxy; y += q ){
            for( int x = minx; x < maxx; x += q ){
              // Corners of block
              int x0 = x << 4;
              int x1 = (x + q - 1) << 4;
              int y0 = y << 4;
              int y1 = (y + q - 1) << 4;

              // Evaluate half-space functions
              bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
              bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
              bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
              bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
              int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

              bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
              bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
              bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
              bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
              int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

              bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
              bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
              bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
              bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
              int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

              // Skip block when outside an edge
              if( !a || !b || !c ){
                continue;
              }

              // Accept whole block when totally covered
              if( a == 0xF && b == 0xF && c == 0xF ){
                for( int iy=0; iy<q; ++iy ){
                  const real h = real( rasterizer.toH() );
                  const real v = real( y+iy )/h;
                  if( y+iy>=h ){
                    break;
                  }
                  for( int ix=x; ix<x+q; ++ix ){
                    const real w = real( rasterizer.toW() );
                    const real u = real( ix )/w;
                    if( ix>=w ){
                      break;
                    }
                    if( ix<0 ){
                      continue;
                    }
                    rasterizer.onDraw( ix, y+iy, vec2( u, v ));
                  }
                }
                continue;
              }

              // Partially covered block
              int CY1 = C1 + DX12 * y0 - DY12 * x0;
              int CY2 = C2 + DX23 * y0 - DY23 * x0;
              int CY3 = C3 + DX31 * y0 - DY31 * x0;
              for( int iy=y; iy<y+q; ++iy ){
                const real h = real( rasterizer.toH() );
                const real v = real( iy )/h;
                if( iy>=h ){
                  break;
                }
                int CX1 = CY1;
                int CX2 = CY2;
                int CX3 = CY3;
                for( int ix=x; ix<x+q; ++ix ){
                  if( CX1 > 0 && CX2 > 0 && CX3 > 0 ){
                    const real w = real( rasterizer.toW() );
                    const real u = real( ix )/w;
                    if( ix>=w ){
                      break;
                    }
                    if( ix<0 ){
                      continue;
                    }
                    rasterizer.onDraw( ix, iy, vec2( u, v ));
                  }
                  CX1 -= FDY12;
                  CX2 -= FDY23;
                  CX3 -= FDY31;
                }
                CY1 += FDX12;
                CY2 += FDX23;
                CY3 += FDX31;
              }
            }
          }
        #endif
        return true;
      }
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Quadtree:{                                      |
  //Structs:{                                     |
    //Leaf:{                                      |
      //Destruct:{                                |

        namespace EON{
          template<> f32::Quadtree::Leaf::~Leaf(){
            for( u32 i=0; i<4; ++i ){
              delete m_aChildren[ i ];
            }
            m_vPolygons.clear();
          }
        }

      //}:                                        |
      //Methods:{                                 |
        //intersects:{                            |

#ifdef __APPLE__
  #pragma mark - Quadtree: methods -
#endif

          namespace EON{
            template<> f32::Quadtree::Leaf::Result f32::Quadtree::Leaf::intersects( const Quadtree* pQuadtree, const vec4x4& L2W, const Ray3& r )const{

              //----------------------------------------------------------------
              // Top level bounds intersection.
              //----------------------------------------------------------------

              aabb3 b;
              b.min.x = m_bBounds.min.x;
              b.min.y = m_bBounds.min.y;
              b.min.z = 0.f;
              b.max.x = m_bBounds.min.x;
              b.max.y = m_bBounds.min.y;
              b.max.z = 0.f;
              if( !( b * L2W ).intersects( r )){
                return nullptr;
              }

              //----------------------------------------------------------------
              // Search polygons for intersection.
              //----------------------------------------------------------------

              if( !m_vPolygons.empty() ){
                vec3 n;
                pt3  p;
                auto it = m_vPolygons.getIterator();
                while( it ){
                  const auto& polygon = pQuadtree->inPolygons( *it );
                  if( polygon.intersects( L2W, r, p, n )){
                    auto res  = std::make_shared<Intersection>();
                    res->poly = polygon;
                    res->norm = n;
                    res->hit  = p;
                    return res;
                  }
                  ++it;
                }
              }

              //----------------------------------------------------------------
              // Search children.
              //----------------------------------------------------------------

              u32 cell = 0;
              for( u32 y=0; y<2; ++y ){
                for( u32 x=0; x<2; ++x, ++cell ){
                  const auto* pLeaf = m_aChildren[ cell ];
                  if( !pLeaf ){
                    continue;
                  }
                  auto res = pLeaf->intersects( pQuadtree, L2W, r );
                  if( !res ){
                    continue;
                  }
                  return res;
                }
              }
              return nullptr;
            }

            template<> f32::Quadtree::Leaf::Result f32::Quadtree::Leaf::intersects( const Quadtree* pQuadtree, const vec4x4& L2W, const Ray3& r, const u32 page )const{

              //----------------------------------------------------------------
              // Top level bounds intersection.
              //----------------------------------------------------------------

              aabb3 b( 0.f );
              b.min.x = m_bBounds.min.x;
              b.max.x = m_bBounds.max.x;
              b.min.y = m_bBounds.min.y;
              b.max.y = m_bBounds.max.y;
              b = b * L2W;
              if( !b.intersects( r )){
                return nullptr;
              }

              //----------------------------------------------------------------
              // Search polygons for intersection.
              //----------------------------------------------------------------

              vec3 n;
              pt3  p;
              for( u32 i=0; i<4; ++i ){
                if( m_aChildren[ i ]){
                  const auto& result = m_aChildren[ i ]->intersects( pQuadtree, L2W, r, page );
                  if( result ){//Go deep!
                    return result;
                  }
                }
              }
              auto it = m_vPolygons.getIterator();
              while( it ){
                const auto& input = pQuadtree->m_vPolygons[ *it ];
                f32::Quadtree::Polygon poly( input );
                if( poly.intersects( L2W, r/* ray */, p/* pos */, n/* norm */)){
                  auto pResult = std::make_shared<Intersection>();
                  pResult->poly = poly;
                  pResult->norm = n;
                  pResult->hit = p;
                  pResult->uid = page;
                  return pResult;
                }
                ++it;
              }
              return nullptr;
            }
          }

        //}:                                      |
        //addPolygon:{                            |

          namespace EON{
            template<> f32::Quadtree::Leaf::Status f32::Quadtree::Leaf::addPolygon( Quadtree* pQuadtree, const u32 depth, const Polygon& poly ){
              //     min _________________.
              //        /        /       /|
              //       /   0    /   1   / |
              //      /        /       /  |
              //     /--------+-------/   |
              //    /        /       /| 1 |
              //   /   2    /   3   / |  /
              //  /        /       /  | /
              //  |-------+-------| 3 |/
              //  |       |       |   /
              //  |   2   |   3   |  /
              //  |       |       | /
              //  |_______:_______|/ max
              //
              if( depth == 4 ){
                return Status::kTrue;
              }
              const pt3& m = m_bBounds.min;
              const f32& h = m_bBounds.height()/2;
              const f32& w = m_bBounds.width()/2;
              u32 cell = 0;
              for( u32 y=0; y<2; ++y ){
                for( u32 x=0; x<2; ++x, ++cell ){
                  const auto& A = pt2(
                      m.x+float( x )*w,
                      m.y+float( y )*h );
                  const auto& B = pt2(
                      A.x + w,
                      A.y + h );
                  const aabb2 b( A, B );
                  if( !b.contains( poly.A.xy() ) && !b.contains( poly.B.xy() ) && !b.contains( poly.C.xy() )){
                    continue;
                  }
                  Leaf*& rpLeaf = m_aChildren[ cell ];
                  if( !rpLeaf ){
                    rpLeaf = new Leaf( b );
                    rpLeaf->setParent( this );
                  }
                  const auto& status = rpLeaf->addPolygon( pQuadtree, depth+1, poly );
                  switch( status ){
                    case Status::kFound:
                      return status;
                    case Status::kFalse:
                      break;
                    case Status::kTrue:/**/{
                      const auto ixPoly = pQuadtree->toPolygons().size();
                      pQuadtree->toPolygons().push( poly );
                      m_vPolygons.push( ixPoly );
                      return Status::kFound;
                    }
                  }
                }
              }
              if( !depth ){
                return Status::kFalse;
              }
              return Status::kTrue;
            }
          }

        //}:                                      |
        //clear:{                                 |

          namespace EON{
            template<> void f32::Quadtree::Leaf::clear(){
              for( u32 i=0; i<4; ++i ){
                delete m_aChildren[ i ];
              }
              m_vPolygons.clear();
            }
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Octree:{                                        |
  //Structs:{                                     |
    //Leaf:{                                      |
      //Destruct:{                                |

        namespace EON{
          template<> f32::Octree::Leaf::~Leaf(){
            for( u32 i=0; i<8; ++i ){
              delete m_aChildren[ i ];
            }
          }
        }

      //}:                                        |
      //Methods:{                                 |
        //intersects:{                            |

#ifdef __APPLE__
  #pragma mark - Octree: methods -
#endif

          namespace EON{
            template<> f32::Octree::Leaf::Result f32::Octree::Leaf::intersects( const Octree* pOctree, const vec4x4& S, const Ray3& r )const{

              //----------------------------------------------------------------
              // Top level bounds intersection.
              //----------------------------------------------------------------

              if( !( m_bBounds * S ).intersects( r )){
                return nullptr;
              }

              //----------------------------------------------------------------
              // Search polygons for intersection.
              //----------------------------------------------------------------

              if( !m_vPolygons.empty() ){
                vec3 n;
                pt3  p;
                auto it = m_vPolygons.getIterator();
                while( it ){
                  auto& polygon = pOctree->inPolygons( *it );
                  if( polygon.intersects( S, r, p, n )){
                    auto r = std::make_shared<Intersection>();
                    r->poly = static_cast<const triangle&>( polygon );
                    r->norm = n;
                    r->hit = p;
                    return r;
                  }
                  ++it;
                }
              }

              //----------------------------------------------------------------
              // Search children.
              //----------------------------------------------------------------

              u32 cell = 0;
              for( u32 z=0; z<2; ++z ){
                for( u32 y=0; y<2; ++y ){
                  for( u32 x=0; x<2; ++x, ++cell ){
                    const auto* pLeaf = m_aChildren[ cell ];
                    if( !pLeaf ){
                      continue;
                    }
                    auto res = pLeaf->intersects( pOctree, S, r );
                    if( !res ){
                      continue;
                    }
                    return res;
                  }
                }
              }
              return nullptr;
            }
          }

        //}:                                      |
        //addPolygon:{                            |

          namespace EON{
            template<> f32::Octree::Leaf::Status f32::Octree::Leaf::addPolygon( Octree* pOctree, const u32 depth, const Polygon& poly ){
              //     min _________________.
              //        /        /       /|
              //       /   0    /   1   / |
              //      /        /       /  |
              //     /--------+-------/   |
              //    /        /       /| 1 |
              //   /   2    /   3   / |  /
              //  /        /       /  | /
              //  |-------+-------| 3 |/
              //  |       |       |   /
              //  |   2   |   3   |  /
              //  |       |       | /
              //  |_______:_______|/______.
              //        /        /       /|
              //       /   4    /   5   / |
              //      /        /       /  |
              //     /--------+-------/ 5 |
              //    /        /       /|   |
              //   /   6    /   7   / |  /
              //  /        /       /  | /
              //  |-------+-------| 7 |/
              //  |       |       |   /
              //  |   6   |   7   |  /
              //  |       |       | /
              //  |_______:_______|/ max
              //
              if( depth < 16 ){
                const auto& m = m_bBounds.min;
                const auto& w = m_bBounds.width () * .5f;
                const auto& d = m_bBounds.depth () * .5f;
                const auto& h = m_bBounds.height() * .5f;
                #if 0 // Log the half axis lengths.
                  e_msgf(
                      "wdh: %.2f, %.2f, %.2f"
                    , w.as<float>()
                    , d.as<float>()
                    , h.as<float>() );
                #endif
                u32 cell = 0;
                for( u32 z=0; z<2; ++z ){
                  for( u32 y=0; y<2; ++y ){
                    for( u32 x=0; x<2; ++x, ++cell ){
                      const auto& A = pt3(
                          m.x + float( x ) * w,
                          m.y + float( y ) * d,
                          m.z + float( z ) * h );
                      const auto& B = pt3(
                          A.x + w,
                          A.y + d,
                          A.z + h );
                      const aabb3 b( A, B );
                      if( !b.contains( poly )){
                        continue;
                      }
                      Leaf*& l = m_aChildren[ cell ];
                      if( !l ){
                        l = new Leaf( b );
                        l->setParent( this );
                      }
                      const auto& status = l->addPolygon( pOctree, depth+1, poly );
                      if( status == Status::kFound ){
                        return status;
                      }
                    }
                  }
                }
                const auto ixPoly = pOctree->toPolygons().size();
                pOctree->toPolygons().push( poly );
                m_vPolygons.push( ixPoly );
                return Status::kFound;
              }
              return Status::kFalse;
            }
          }

        //}:                                      |
        //clear:{                                 |

          namespace EON{
            template<> void f32::Octree::Leaf::clear(){
              for( u32 i=0; i<8; ++i ){
                delete m_aChildren[ i ];
              }
              m_vPolygons.clear();
            }
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
  //}:                                            |
  //Methods:{                                     |
    //intersects:{                                |

      template<> f32::Octree::Result f32::Octree::intersects( const vec4x4& L2W, const ray3& r )const{
        return m_pRoot->intersects(
            this
          , L2W
          , r
        );
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
