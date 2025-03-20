//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Vector4:{                                       |

  /** \brief Four tuple homogeneous vector.
    *
    * This class defines the four tuple vector. It is not marked 'final'
    * because the Plane class derives from it. This is an error in my opinion
    * and should be fixed as soon as possible.
    */

  struct alignas( 16 ) E_PUBLISH Vector4{

    //--------------------------------------------+-----------------------------
    //Statics:{                                   |

      static const u32     kTuples = 4;
      static const Vector4 kForward;
      static const Vector4 kRight;
      static const Vector4 kUp;
      static const Vector4 kZero;

    //}:                                          |
    //Structs:{                                   |

      union{
        #ifdef  __SSE__
          __m128i i;
          __m128  v;
        #endif
        struct{
          self x, y, z, w;
        };
        struct{
          self r, g, b, a;
        };
        self m[ 4 ];
      };

    //}:                                          |
    //Operate:{                                   |
      //Indexing:{                                |

        e_noinline self operator[]( const s32 i )const{
          switch( i ){
            case 0:
              return x;
            case 1:
              return y;
            case 2:
              return z;
            case 3:
              [[fallthrough]];
            default:
              return w;
          }
        }

        e_noinline self& operator[]( const s32 i ){
          switch( i ){
            case 0:
              return x;
            case 1:
              return y;
            case 2:
              return z;
            case 3:
              [[fallthrough]];
            default:
              return w;
          }
        }

      //}:                                        |
      //Assignment:{                              |

        e_forceinline Vector4& operator=( const Vector4& V ){
          if( this != &V ){
            if( is_sse() ){
              #ifdef __SSE__
                v = V.v;
                return *this;
              #endif
            }
            x = V.x;
            y = V.y;
            z = V.z;
            w = V.w;
          }
          return *this;
        }

        e_forceinline Vector4& operator=( const Vector3& V ){
          x = V.x;
          y = V.y;
          z = V.z;
          w = 1.f;
          return *this;
        }

        e_forceinline Vector4& operator=( const Point3& P ){
          x = P.x;
          y = P.y;
          z = P.z;
          w = 1.f;
          return *this;
        }

        #ifdef __SSE__
          e_forceinline Vector4& operator=( const __m128 V ){
            static_assert( is_sse(), "Not available!" );
            v = V;
            return *this;
          }
        #endif

      //}:                                        |
      //Equality:{                                |

        e_noinline bool operator!=( const Vector4& V )const{

          //--------------------------------------------------------------------
          // Bail conditions: vectors are the same.
          //--------------------------------------------------------------------

          if( this == &V ){
            return false;
          }

          //--------------------------------------------------------------------
          // The optimized path.
          //--------------------------------------------------------------------

          if( is_sse() ){
            #ifdef __SSE__
              const __m128i c = __m128i(_mm_cmpneq_ps( v, V.v ));
              const u16 r = _mm_movemask_epi8( c );
              if( r == 0xFFFF ){
                // All elements not equal.
                return true;
              }
              if( r != 0 ){
                // Some elements not equal.
                return true;
              }
              return false;
            #endif
          }

          //--------------------------------------------------------------------
          // Slow manual path.
          //--------------------------------------------------------------------

          if( x != V.x ){
            return true;
          }
          if( y != V.y ){
            return true;
          }
          if( z != V.z ){
            return true;
          }
          if( w != V.w ){
            return true;
          }
          return false;
        }

        e_forceinline bool operator==( const Vector4& V )const{

          //--------------------------------------------------------------------
          // Bail conditions: vectors are the same.
          //--------------------------------------------------------------------

          if( this == &V ){
            return true;
          }

          //--------------------------------------------------------------------
          // The optimized path.
          //--------------------------------------------------------------------

          if( is_sse() ){
            #ifdef __SSE__
              const __m128i c = __m128i(_mm_cmpneq_ps( v, V.v ));
              const u16 r = _mm_movemask_epi8( c );
              if( r == 0xFFFF ){
                // All elements not equal.
                return false;
              }
              if( r != 0 ){
                // Some elements not equal.
                return false;
              }
              return true;
            #endif
          }

          //--------------------------------------------------------------------
          // Slow manual path.
          //--------------------------------------------------------------------

          if( x != V.x ){
            return false;
          }
          if( y != V.y ){
            return false;
          }
          if( z != V.z ){
            return false;
          }
          if( w != V.w ){
            return false;
          }
          return true;
        }

      //}:                                        |
      //Bracket:{                                 |

        e_forceinline self operator[]( const u32 i )const{
          return m[ i ];
        }

        e_forceinline self& operator[]( const u32 i ){
          return m[ i ];
        }

      //}:                                        |
      //Unary:{                                   |

        e_forceinline Vector4 operator-()const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32( 0x80000000 ));
              r.v = _mm_xor_ps( v, SIGNMASK );
              return r;
            #endif
          }
          r.x = -x;
          r.y = -y;
          r.z = -z;
          r.w = -w;
          return r;
        }

      //}:                                        |
      //Quotients:{                               |

        e_forceinline Vector4 operator/( const Vector4& V )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_div_ps( v, V.v );
              return r;
            #endif
          }
          r.x = x / V.x;
          r.y = y / V.y;
          r.z = z / V.z;
          r.w = w / V.w;
          return r;
        }

        e_forceinline Vector4& operator/=( const Vector4& V ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_div_ps( v, V.v );
              return *this;
            #endif
          }
          x /= V.x;
          y /= V.y;
          z /= V.z;
          w /= V.w;
          return *this;
        }

        e_forceinline Vector4 operator/( const self& s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_div_ps( v,_mm_set1_ps( float( s )));
              return r;
            #endif
          }
          r.x = x / s;
          r.y = y / s;
          r.z = z / s;
          r.w = w / s;
          return r;
        }

        e_forceinline Vector4& operator/=( const self& s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_div_ps( v,_mm_set1_ps( float( s )));
              return *this;
            #endif
          }
          x /= s;
          y /= s;
          z /= s;
          w /= s;
          return *this;
        }

        e_forceinline Vector4 operator/( const float s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_div_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x / s;
          r.y = y / s;
          r.z = z / s;
          r.w = w / s;
          return r;
        }

        e_forceinline Vector4& operator/=( const float s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_div_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x /= s;
          y /= s;
          z /= s;
          w /= s;
          return *this;
        }

      //}:                                        |
      //Products:{                                |

        Vector4 operator*( const Matrix4& )const;
        Vector4 operator*( const Matrix3& )const;

        e_forceinline Vector4& operator*=( const Vector4& V ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_mul_ps( v, V.v );
              return *this;
            #endif
          }
          x *= V.x;
          y *= V.y;
          z *= V.z;
          w *= V.w;
          return *this;
        }

        e_forceinline Vector4 operator*( const Vector4& V )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_mul_ps( v, V.v );
              return r;
            #endif
          }
          r.x = x * V.x;
          r.y = y * V.y;
          r.z = z * V.z;
          r.w = w * V.w;
          return r;
        }

        e_forceinline Vector4& operator*=( const self& s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_mul_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x *= s;
          y *= s;
          z *= s;
          w *= s;
          return *this;
        }

        e_forceinline Vector4 operator*( const self& s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_mul_ps( v,_mm_set1_ps( float( s )));
              return r;
            #endif
          }
          r.x = x * s;
          r.y = y * s;
          r.z = z * s;
          r.w = w * s;
          return r;
        }

        e_forceinline Vector4& operator*=( const float s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_mul_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x *= s;
          y *= s;
          z *= s;
          w *= s;
          return *this;
        }

        e_forceinline Vector4 operator*( const float s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_mul_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x * s;
          r.y = y * s;
          r.z = z * s;
          r.w = w * s;
          return r;
        }

      //}:                                        |
      //Diffs:{                                   |

        e_forceinline Vector4& operator-=( const Vector4& V ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_sub_ps( v, V.v );
              return *this;
            #endif
          }
          x -= V.x;
          y -= V.y;
          z -= V.z;
          w -= V.w;
          return *this;
        }

        e_forceinline Vector4 operator-( const Vector4& V )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_sub_ps( v, V.v );
              return r;
            #endif
          }
          r.x = x - V.x;
          r.y = y - V.y;
          r.z = z - V.z;
          r.w = w - V.w;
          return r;
        }

        e_forceinline Vector4 operator-( const Point3& p )const{
          Vector4 r;
          r.x = x - p.x;
          r.y = y - p.y;
          r.z = z - p.z;
          return r;
        }

        e_forceinline Vector4& operator-=( const self& s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_sub_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x -= s;
          y -= s;
          z -= s;
          w -= s;
          return *this;
        }

        e_forceinline Vector4 operator-( const self& s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_sub_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x - s;
          r.y = y - s;
          r.z = z - s;
          r.w = w - s;
          return r;
        }

        e_forceinline Vector4& operator-=( const float s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_sub_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x -= s;
          y -= s;
          z -= s;
          w -= s;
          return *this;
        }

        e_forceinline Vector4 operator-( const float s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_sub_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x - s;
          r.y = y - s;
          r.z = z - s;
          r.w = w - s;
          return r;
        }

      //}:                                        |
      //Sums:{                                    |

        e_forceinline Vector4& operator+=( const Vector4& V ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_add_ps( v, V.v );
              return *this;
            #endif
          }
          x += V.x;
          y += V.y;
          z += V.z;
          w += V.w;
          return *this;
        }

        e_forceinline Vector4 operator+( const Vector4& V )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_add_ps( v, V.v );
              return r;
            #endif
          }
          r.x = x + V.x;
          r.y = y + V.y;
          r.z = z + V.z;
          r.w = w + V.w;
          return r;
        }

        e_forceinline Vector4 operator+( const Point3& p )const{
          Vector4 r;
          r.x = x + p.x;
          r.y = y + p.y;
          r.z = z + p.z;
          return r;
        }

        e_forceinline Vector4& operator+=( const self& s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_add_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x += s;
          y += s;
          z += s;
          w += s;
          return *this;
        }

        e_forceinline Vector4 operator+( const self& s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_add_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x + s;
          r.y = y + s;
          r.z = z + s;
          r.w = w + s;
          return r;
        }

        e_forceinline Vector4& operator+=( const float s ){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_add_ps( v,_mm_set1_ps( s ));
              return *this;
            #endif
          }
          x += s;
          y += s;
          z += s;
          w += s;
          return *this;
        }

        e_forceinline Vector4 operator+( const float s )const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_add_ps( v,_mm_set1_ps( s ));
              return r;
            #endif
          }
          r.x = x + s;
          r.y = y + s;
          r.z = z + s;
          r.w = w + s;
          return r;
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |
      //[swizzling]:{                             |
        //xxxx:{                                  |

          e_forceinline Vector4 xxxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x00 );
                return r;
              #endif
            }
            r = Vector4( x, x, x, x );
            return r;
          }

        //}:                                      |
        //xxxy:{                                  |

          e_forceinline Vector4 xxxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x40 );
                return r;
              #endif
            }
            r = Vector4( x, x, x, y );
            return r;
          }

        //}:                                      |
        //xxxz:{                                  |

          e_forceinline Vector4 xxxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x80 );
                return r;
              #endif
            }
            r = Vector4( x, x, x, z );
            return r;
          }

        //}:                                      |
        //xxxw:{                                  |

          e_forceinline Vector4 xxxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC0 );
                return r;
              #endif
            }
            r = Vector4( x, x, x, w );
            return r;
          }

        //}:                                      |
        //xxyx:{                                  |

          e_forceinline Vector4 xxyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x10 );
                return r;
              #endif
            }
            r = Vector4( x, x, y, x );
            return r;
          }

        //}:                                      |
        //xxyy:{                                  |

          e_forceinline Vector4 xxyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x50 );
                return r;
              #endif
            }
            r = Vector4( x, x, y, y );
            return r;
          }

        //}:                                      |
        //xxyz:{                                  |

          e_forceinline Vector4 xxyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x90 );
                return r;
              #endif
            }
            r = Vector4( x, x, y, z );
            return r;
          }

        //}:                                      |
        //xxyw:{                                  |

          e_forceinline Vector4 xxyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD0 );
                return r;
              #endif
            }
            r = Vector4( x, x, y, w );
            return r;
          }

        //}:                                      |
        //xxzx:{                                  |

          e_forceinline Vector4 xxzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x20 );
                return r;
              #endif
            }
            r = Vector4( x, x, z, x );
            return r;
          }

        //}:                                      |
        //xxzy:{                                  |

          e_forceinline Vector4 xxzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x60 );
                return r;
              #endif
            }
            r = Vector4( x, x, z, y );
            return r;
          }

        //}:                                      |
        //xxzz:{                                  |

          e_forceinline Vector4 xxzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA0 );
                return r;
              #endif
            }
            r = Vector4( x, x, z, z );
            return r;
          }

        //}:                                      |
        //xxzw:{                                  |

          e_forceinline Vector4 xxzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE0 );
                return r;
              #endif
            }
            r = Vector4( x, x, z, w );
            return r;
          }

        //}:                                      |
        //xxwx:{                                  |

          e_forceinline Vector4 xxwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x30 );
                return r;
              #endif
            }
            r = Vector4( x, x, w, x );
            return r;
          }

        //}:                                      |
        //xxwy:{                                  |

          e_forceinline Vector4 xxwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x70 );
                return r;
              #endif
            }
            r = Vector4( x, x, w, y );
            return r;
          }

        //}:                                      |
        //xxwz:{                                  |

          e_forceinline Vector4 xxwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB0 );
                return r;
              #endif
            }
            r = Vector4( x, x, w, z );
            return r;
          }

        //}:                                      |
        //xxww:{                                  |

          e_forceinline Vector4 xxww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF0 );
                return r;
              #endif
            }
            r = Vector4( x, x, w, w );
            return r;
          }

        //}:                                      |
        //xyxx:{                                  |

          e_forceinline Vector4 xyxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x04 );
                return r;
              #endif
            }
            r = Vector4( x, y, x, x );
            return r;
          }

        //}:                                      |
        //xyxy:{                                  |

          e_forceinline Vector4 xyxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x44 );
                return r;
              #endif
            }
            r = Vector4( x, y, x, y );
            return r;
          }

        //}:                                      |
        //xyxz:{                                  |

          e_forceinline Vector4 xyxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x84 );
                return r;
              #endif
            }
            r = Vector4( x, y, x, z );
            return r;
          }

        //}:                                      |
        //xyxw:{                                  |

          e_forceinline Vector4 xyxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC4 );
                return r;
              #endif
            }
            r = Vector4( x, y, x, w );
            return r;
          }

        //}:                                      |
        //xyyx:{                                  |

          e_forceinline Vector4 xyyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x14 );
                return r;
              #endif
            }
            r = Vector4( x, y, y, x );
            return r;
          }

        //}:                                      |
        //xyyy:{                                  |

          e_forceinline Vector4 xyyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x54 );
                return r;
              #endif
            }
            r = Vector4( x, y, y, y );
            return r;
          }

        //}:                                      |
        //xyyz:{                                  |

          e_forceinline Vector4 xyyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x94 );
                return r;
              #endif
            }
            r = Vector4( x, y, y, z );
            return r;
          }

        //}:                                      |
        //xyyw:{                                  |

          e_forceinline Vector4 xyyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD4 );
                return r;
              #endif
            }
            r = Vector4( x, y, y, w );
            return r;
          }

        //}:                                      |
        //xyzx:{                                  |

          e_forceinline Vector4 xyzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x24 );
                return r;
              #endif
            }
            r = Vector4( x, y, z, x );
            return r;
          }

        //}:                                      |
        //xyzy:{                                  |

          e_forceinline Vector4 xyzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x64 );
                return r;
              #endif
            }
            r = Vector4( x, y, z, y );
            return r;
          }

        //}:                                      |
        //xyzz:{                                  |

          e_forceinline Vector4 xyzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA4 );
                return r;
              #endif
            }
            r = Vector4( x, y, z, z );
            return r;
          }

        //}:                                      |
        //xyzw:{                                  |

          e_forceinline Vector4 xyzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE4 );
                return r;
              #endif
            }
            r = Vector4( x, y, z, w );
            return r;
          }

        //}:                                      |
        //xywx:{                                  |

          e_forceinline Vector4 xywx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x34 );
                return r;
              #endif
            }
            r = Vector4( x, y, w, x );
            return r;
          }

        //}:                                      |
        //xywy:{                                  |

          e_forceinline Vector4 xywy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x74 );
                return r;
              #endif
            }
            r = Vector4( x, y, w, y );
            return r;
          }

        //}:                                      |
        //xywz:{                                  |

          e_forceinline Vector4 xywz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB4 );
                return r;
              #endif
            }
            r = Vector4( x, y, w, z );
            return r;
          }

        //}:                                      |
        //xyww:{                                  |

          e_forceinline Vector4 xyww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF4 );
                return r;
              #endif
            }
            r = Vector4( x, y, w, w );
            return r;
          }

        //}:                                      |
        //xzxx:{                                  |

          e_forceinline Vector4 xzxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x08 );
                return r;
              #endif
            }
            r = Vector4( x, z, x, x );
            return r;
          }

        //}:                                      |
        //xzxy:{                                  |

          e_forceinline Vector4 xzxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x48 );
                return r;
              #endif
            }
            r = Vector4( x, z, x, y );
            return r;
          }

        //}:                                      |
        //xzxz:{                                  |

          e_forceinline Vector4 xzxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x88 );
                return r;
              #endif
            }
            r = Vector4( x, z, x, z );
            return r;
          }

        //}:                                      |
        //xzxw:{                                  |

          e_forceinline Vector4 xzxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC8 );
                return r;
              #endif
            }
            r = Vector4( x, z, x, w );
            return r;
          }

        //}:                                      |
        //xzyx:{                                  |

          e_forceinline Vector4 xzyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x18 );
                return r;
              #endif
            }
            r = Vector4( x, z, y, x );
            return r;
          }

        //}:                                      |
        //xzyy:{                                  |

          e_forceinline Vector4 xzyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x58 );
                return r;
              #endif
            }
            r = Vector4( x, z, y, y );
            return r;
          }

        //}:                                      |
        //xzyz:{                                  |

          e_forceinline Vector4 xzyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x98 );
                return r;
              #endif
            }
            r = Vector4( x, z, y, z );
            return r;
          }

        //}:                                      |
        //xzyw:{                                  |

          e_forceinline Vector4 xzyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD8 );
                return r;
              #endif
            }
            r = Vector4( x, z, y, w );
            return r;
          }

        //}:                                      |
        //xzzx:{                                  |

          e_forceinline Vector4 xzzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x28 );
                return r;
              #endif
            }
            r = Vector4( x, z, z, x );
            return r;
          }

        //}:                                      |
        //xzzy:{                                  |

          e_forceinline Vector4 xzzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x68 );
                return r;
              #endif
            }
            r = Vector4( x, z, z, y );
            return r;
          }

        //}:                                      |
        //xzzz:{                                  |

          e_forceinline Vector4 xzzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA8 );
                return r;
              #endif
            }
            r = Vector4( x, z, z, z );
            return r;
          }

        //}:                                      |
        //xzzw:{                                  |

          e_forceinline Vector4 xzzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE8 );
                return r;
              #endif
            }
            r = Vector4( x, z, z, w );
            return r;
          }

        //}:                                      |
        //xzwx:{                                  |

          e_forceinline Vector4 xzwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x38 );
                return r;
              #endif
            }
            r = Vector4( x, z, w, x );
            return r;
          }

        //}:                                      |
        //xzwy:{                                  |

          e_forceinline Vector4 xzwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x78 );
                return r;
              #endif
            }
            r = Vector4( x, z, w, y );
            return r;
          }

        //}:                                      |
        //xzwz:{                                  |

          e_forceinline Vector4 xzwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB8 );
                return r;
              #endif
            }
            r = Vector4( x, z, w, z );
            return r;
          }

        //}:                                      |
        //xzww:{                                  |

          e_forceinline Vector4 xzww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF8 );
                return r;
              #endif
            }
            r = Vector4( x, z, w, w );
            return r;
          }

        //}:                                      |
        //xwxx:{                                  |

          e_forceinline Vector4 xwxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0C );
                return r;
              #endif
            }
            r = Vector4( x, w, x, x );
            return r;
          }

        //}:                                      |
        //xwxy:{                                  |

          e_forceinline Vector4 xwxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4C );
                return r;
              #endif
            }
            r = Vector4( x, w, x, y );
            return r;
          }

        //}:                                      |
        //xwxz:{                                  |

          e_forceinline Vector4 xwxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8C );
                return r;
              #endif
            }
            r = Vector4( x, w, x, z );
            return r;
          }

        //}:                                      |
        //xwxw:{                                  |

          e_forceinline Vector4 xwxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCC );
                return r;
              #endif
            }
            r = Vector4( x, w, x, w );
            return r;
          }

        //}:                                      |
        //xwyx:{                                  |

          e_forceinline Vector4 xwyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1C );
                return r;
              #endif
            }
            r = Vector4( x, w, y, x );
            return r;
          }

        //}:                                      |
        //xwyy:{                                  |

          e_forceinline Vector4 xwyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5C );
                return r;
              #endif
            }
            r = Vector4( x, w, y, y );
            return r;
          }

        //}:                                      |
        //xwyz:{                                  |

          e_forceinline Vector4 xwyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9C );
                return r;
              #endif
            }
            r = Vector4( x, w, y, z );
            return r;
          }

        //}:                                      |
        //xwyw:{                                  |

          e_forceinline Vector4 xwyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDC );
                return r;
              #endif
            }
            r = Vector4( x, w, y, w );
            return r;
          }

        //}:                                      |
        //xwzx:{                                  |

          e_forceinline Vector4 xwzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2C );
                return r;
              #endif
            }
            r = Vector4( x, w, z, x );
            return r;
          }

        //}:                                      |
        //xwzy:{                                  |

          e_forceinline Vector4 xwzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6C );
                return r;
              #endif
            }
            r = Vector4( x, w, z, y );
            return r;
          }

        //}:                                      |
        //xwzz:{                                  |

          e_forceinline Vector4 xwzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAC );
                return r;
              #endif
            }
            r = Vector4( x, w, z, z );
            return r;
          }

        //}:                                      |
        //xwzw:{                                  |

          e_forceinline Vector4 xwzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xEC );
                return r;
              #endif
            }
            r = Vector4( x, w, z, w );
            return r;
          }

        //}:                                      |
        //xwwx:{                                  |

          e_forceinline Vector4 xwwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3C );
                return r;
              #endif
            }
            r = Vector4( x, w, w, x );
            return r;
          }

        //}:                                      |
        //xwwy:{                                  |

          e_forceinline Vector4 xwwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7C );
                return r;
              #endif
            }
            r = Vector4( x, w, w, y );
            return r;
          }

        //}:                                      |
        //xwwz:{                                  |

          e_forceinline Vector4 xwwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBC );
                return r;
              #endif
            }
            r = Vector4( x, w, w, z );
            return r;
          }

        //}:                                      |
        //xwww:{                                  |

          e_forceinline Vector4 xwww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFC );
                return r;
              #endif
            }
            r = Vector4( x, w, w, w );
            return r;
          }

        //}:                                      |
        //yxxx:{                                  |

          e_forceinline Vector4 yxxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x01 );
                return r;
              #endif
            }
            r = Vector4( y, x, x, x );
            return r;
          }

        //}:                                      |
        //yxxy:{                                  |

          e_forceinline Vector4 yxxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x41 );
                return r;
              #endif
            }
            r = Vector4( y, x, x, y );
            return r;
          }

        //}:                                      |
        //yxxz:{                                  |

          e_forceinline Vector4 yxxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x81 );
                return r;
              #endif
            }
            r = Vector4( y, x, x, z );
            return r;
          }

        //}:                                      |
        //yxxw:{                                  |

          e_forceinline Vector4 yxxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC1 );
                return r;
              #endif
            }
            r = Vector4( y, x, x, w );
            return r;
          }

        //}:                                      |
        //yxyx:{                                  |

          e_forceinline Vector4 yxyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x11 );
                return r;
              #endif
            }
            r = Vector4( y, x, y, x );
            return r;
          }

        //}:                                      |
        //yxyy:{                                  |

          e_forceinline Vector4 yxyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x51 );
                return r;
              #endif
            }
            r = Vector4( y, x, y, y );
            return r;
          }

        //}:                                      |
        //yxyz:{                                  |

          e_forceinline Vector4 yxyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x91 );
                return r;
              #endif
            }
            r = Vector4( y, x, y, z );
            return r;
          }

        //}:                                      |
        //yxyw:{                                  |

          e_forceinline Vector4 yxyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD1 );
                return r;
              #endif
            }
            r = Vector4( y, x, y, w );
            return r;
          }

        //}:                                      |
        //yxzx:{                                  |

          e_forceinline Vector4 yxzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x21 );
                return r;
              #endif
            }
            r = Vector4( y, x, z, x );
            return r;
          }

        //}:                                      |
        //yxzy:{                                  |

          e_forceinline Vector4 yxzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x61 );
                return r;
              #endif
            }
            r = Vector4( y, x, z, y );
            return r;
          }

        //}:                                      |
        //yxzz:{                                  |

          e_forceinline Vector4 yxzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA1 );
                return r;
              #endif
            }
            r = Vector4( y, x, z, z );
            return r;
          }

        //}:                                      |
        //yxzw:{                                  |

          e_forceinline Vector4 yxzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE1 );
                return r;
              #endif
            }
            r = Vector4( y, x, z, w );
            return r;
          }

        //}:                                      |
        //yxwx:{                                  |

          e_forceinline Vector4 yxwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x31 );
                return r;
              #endif
            }
            r = Vector4( y, x, w, x );
            return r;
          }

        //}:                                      |
        //yxwy:{                                  |

          e_forceinline Vector4 yxwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x71 );
                return r;
              #endif
            }
            r = Vector4( y, x, w, y );
            return r;
          }

        //}:                                      |
        //yxwz:{                                  |

          e_forceinline Vector4 yxwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB1 );
                return r;
              #endif
            }
            r = Vector4( y, x, w, z );
            return r;
          }

        //}:                                      |
        //yxww:{                                  |

          e_forceinline Vector4 yxww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF1 );
                return r;
              #endif
            }
            r = Vector4( y, x, w, w );
            return r;
          }

        //}:                                      |
        //yyxx:{                                  |

          e_forceinline Vector4 yyxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x05 );
                return r;
              #endif
            }
            r = Vector4( y, y, x, x );
            return r;
          }

        //}:                                      |
        //yyxy:{                                  |

          e_forceinline Vector4 yyxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x45 );
                return r;
              #endif
            }
            r = Vector4( y, y, x, y );
            return r;
          }

        //}:                                      |
        //yyxz:{                                  |

          e_forceinline Vector4 yyxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x85 );
                return r;
              #endif
            }
            r = Vector4( y, y, x, z );
            return r;
          }

        //}:                                      |
        //yyxw:{                                  |

          e_forceinline Vector4 yyxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC5 );
                return r;
              #endif
            }
            r = Vector4( y, y, x, w );
            return r;
          }

        //}:                                      |
        //yyyx:{                                  |

          e_forceinline Vector4 yyyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x15 );
                return r;
              #endif
            }
            r = Vector4( y, y, y, x );
            return r;
          }

        //}:                                      |
        //yyyy:{                                  |

          e_forceinline Vector4 yyyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x55 );
                return r;
              #endif
            }
            r = Vector4( y, y, y, y );
            return r;
          }

        //}:                                      |
        //yyyz:{                                  |

          e_forceinline Vector4 yyyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x95 );
                return r;
              #endif
            }
            r = Vector4( y, y, y, z );
            return r;
          }

        //}:                                      |
        //yyyw:{                                  |

          e_forceinline Vector4 yyyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD5 );
                return r;
              #endif
            }
            r = Vector4( y, y, y, w );
            return r;
          }

        //}:                                      |
        //yyzx:{                                  |

          e_forceinline Vector4 yyzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x25 );
                return r;
              #endif
            }
            r = Vector4( y, y, z, x );
            return r;
          }

        //}:                                      |
        //yyzy:{                                  |

          e_forceinline Vector4 yyzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x65 );
                return r;
              #endif
            }
            r = Vector4( y, y, z, y );
            return r;
          }

        //}:                                      |
        //yyzz:{                                  |

          e_forceinline Vector4 yyzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA5 );
                return r;
              #endif
            }
            r = Vector4( y, y, z, z );
            return r;
          }

        //}:                                      |
        //yyzw:{                                  |

          e_forceinline Vector4 yyzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE5 );
                return r;
              #endif
            }
            r = Vector4( y, y, z, w );
            return r;
          }

        //}:                                      |
        //yywx:{                                  |

          e_forceinline Vector4 yywx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x35 );
                return r;
              #endif
            }
            r = Vector4( y, y, w, x );
            return r;
          }

        //}:                                      |
        //yywy:{                                  |

          e_forceinline Vector4 yywy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x75 );
                return r;
              #endif
            }
            r = Vector4( y, y, w, y );
            return r;
          }

        //}:                                      |
        //yywz:{                                  |

          e_forceinline Vector4 yywz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB5 );
                return r;
              #endif
            }
            r = Vector4( y, y, w, z );
            return r;
          }

        //}:                                      |
        //yyww:{                                  |

          e_forceinline Vector4 yyww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF5 );
                return r;
              #endif
            }
            r = Vector4( y, y, w, w );
            return r;
          }

        //}:                                      |
        //yzxx:{                                  |

          e_forceinline Vector4 yzxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x09 );
                return r;
              #endif
            }
            r = Vector4( y, z, x, x );
            return r;
          }

        //}:                                      |
        //yzxy:{                                  |

          e_forceinline Vector4 yzxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x49 );
                return r;
              #endif
            }
            r = Vector4( y, z, x, y );
            return r;
          }

        //}:                                      |
        //yzxz:{                                  |

          e_forceinline Vector4 yzxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x89 );
                return r;
              #endif
            }
            r = Vector4( y, z, x, z );
            return r;
          }

        //}:                                      |
        //yzxw:{                                  |

          e_forceinline Vector4 yzxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC9 );
                return r;
              #endif
            }
            r = Vector4( y, z, x, w );
            return r;
          }

        //}:                                      |
        //yzyx:{                                  |

          e_forceinline Vector4 yzyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x19 );
                return r;
              #endif
            }
            r = Vector4( y, z, y, x );
            return r;
          }

        //}:                                      |
        //yzyy:{                                  |

          e_forceinline Vector4 yzyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x59 );
                return r;
              #endif
            }
            r = Vector4( y, z, y, y );
            return r;
          }

        //}:                                      |
        //yzyz:{                                  |

          e_forceinline Vector4 yzyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x99 );
                return r;
              #endif
            }
            r = Vector4( y, z, y, z );
            return r;
          }

        //}:                                      |
        //yzyw:{                                  |

          e_forceinline Vector4 yzyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD9 );
                return r;
              #endif
            }
            r = Vector4( y, z, y, w );
            return r;
          }

        //}:                                      |
        //yzzx:{                                  |

          e_forceinline Vector4 yzzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x29 );
                return r;
              #endif
            }
            r = Vector4( y, z, z, x );
            return r;
          }

        //}:                                      |
        //yzzy:{                                  |

          e_forceinline Vector4 yzzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x69 );
                return r;
              #endif
            }
            r = Vector4( y, z, z, y );
            return r;
          }

        //}:                                      |
        //yzzz:{                                  |

          e_forceinline Vector4 yzzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA9 );
                return r;
              #endif
            }
            r = Vector4( y, z, z, z );
            return r;
          }

        //}:                                      |
        //yzzw:{                                  |

          e_forceinline Vector4 yzzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE9 );
                return r;
              #endif
            }
            r = Vector4( y, z, z, w );
            return r;
          }

        //}:                                      |
        //yzwx:{                                  |

          e_forceinline Vector4 yzwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x39 );
                return r;
              #endif
            }
            r = Vector4( y, z, w, x );
            return r;
          }

        //}:                                      |
        //yzwy:{                                  |

          e_forceinline Vector4 yzwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x79 );
                return r;
              #endif
            }
            r = Vector4( y, z, w, y );
            return r;
          }

        //}:                                      |
        //yzwz:{                                  |

          e_forceinline Vector4 yzwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB9 );
                return r;
              #endif
            }
            r = Vector4( y, z, w, z );
            return r;
          }

        //}:                                      |
        //yzww:{                                  |

          e_forceinline Vector4 yzww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF9 );
                return r;
              #endif
            }
            r = Vector4( y, z, w, w );
            return r;
          }

        //}:                                      |
        //ywxx:{                                  |

          e_forceinline Vector4 ywxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0D );
                return r;
              #endif
            }
            r = Vector4( y, w, x, x );
            return r;
          }

        //}:                                      |
        //ywxy:{                                  |

          e_forceinline Vector4 ywxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4D );
                return r;
              #endif
            }
            r = Vector4( y, w, x, y );
            return r;
          }

        //}:                                      |
        //ywxz:{                                  |

          e_forceinline Vector4 ywxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8D );
                return r;
              #endif
            }
            r = Vector4( y, w, x, z );
            return r;
          }

        //}:                                      |
        //ywxw:{                                  |

          e_forceinline Vector4 ywxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCD );
                return r;
              #endif
            }
            r = Vector4( y, w, x, w );
            return r;
          }

        //}:                                      |
        //ywyx:{                                  |

          e_forceinline Vector4 ywyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1D );
                return r;
              #endif
            }
            r = Vector4( y, w, y, x );
            return r;
          }

        //}:                                      |
        //ywyy:{                                  |

          e_forceinline Vector4 ywyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5D );
                return r;
              #endif
            }
            r = Vector4( y, w, y, y );
            return r;
          }

        //}:                                      |
        //ywyz:{                                  |

          e_forceinline Vector4 ywyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9D );
                return r;
              #endif
            }
            r = Vector4( y, w, y, z );
            return r;
          }

        //}:                                      |
        //ywyw:{                                  |

          e_forceinline Vector4 ywyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDD );
                return r;
              #endif
            }
            r = Vector4( y, w, y, w );
            return r;
          }

        //}:                                      |
        //ywzx:{                                  |

          e_forceinline Vector4 ywzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2D );
                return r;
              #endif
            }
            r = Vector4( y, w, z, x );
            return r;
          }

        //}:                                      |
        //ywzy:{                                  |

          e_forceinline Vector4 ywzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6D );
                return r;
              #endif
            }
            r = Vector4( y, w, z, y );
            return r;
          }

        //}:                                      |
        //ywzz:{                                  |

          e_forceinline Vector4 ywzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAD );
                return r;
              #endif
            }
            r = Vector4( y, w, z, z );
            return r;
          }

        //}:                                      |
        //ywzw:{                                  |

          e_forceinline Vector4 ywzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xED );
                return r;
              #endif
            }
            r = Vector4( y, w, z, w );
            return r;
          }

        //}:                                      |
        //ywwx:{                                  |

          e_forceinline Vector4 ywwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3D );
                return r;
              #endif
            }
            r = Vector4( y, w, w, x );
            return r;
          }

        //}:                                      |
        //ywwy:{                                  |

          e_forceinline Vector4 ywwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7D );
                return r;
              #endif
            }
            r = Vector4( y, w, w, y );
            return r;
          }

        //}:                                      |
        //ywwz:{                                  |

          e_forceinline Vector4 ywwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBD );
                return r;
              #endif
            }
            r = Vector4( y, w, w, z );
            return r;
          }

        //}:                                      |
        //ywww:{                                  |

          e_forceinline Vector4 ywww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFD );
                return r;
              #endif
            }
            r = Vector4( y, w, w, w );
            return r;
          }

        //}:                                      |
        //zxxx:{                                  |

          e_forceinline Vector4 zxxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x02 );
                return r;
              #endif
            }
            r = Vector4( z, x, x, x );
            return r;
          }

        //}:                                      |
        //zxxy:{                                  |

          e_forceinline Vector4 zxxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x42 );
                return r;
              #endif
            }
            r = Vector4( z, x, x, y );
            return r;
          }

        //}:                                      |
        //zxxz:{                                  |

          e_forceinline Vector4 zxxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x82 );
                return r;
              #endif
            }
            r = Vector4( z, x, x, z );
            return r;
          }

        //}:                                      |
        //zxxw:{                                  |

          e_forceinline Vector4 zxxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC2 );
                return r;
              #endif
            }
            r = Vector4( z, x, x, w );
            return r;
          }

        //}:                                      |
        //zxyx:{                                  |

          e_forceinline Vector4 zxyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x12 );
                return r;
              #endif
            }
            r = Vector4( z, x, y, x );
            return r;
          }

        //}:                                      |
        //zxyy:{                                  |

          e_forceinline Vector4 zxyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x52 );
                return r;
              #endif
            }
            r = Vector4( z, x, y, y );
            return r;
          }

        //}:                                      |
        //zxyz:{                                  |

          e_forceinline Vector4 zxyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x92 );
                return r;
              #endif
            }
            r = Vector4( z, x, y, z );
            return r;
          }

        //}:                                      |
        //zxyw:{                                  |

          e_forceinline Vector4 zxyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD2 );
                return r;
              #endif
            }
            r = Vector4( z, x, y, w );
            return r;
          }

        //}:                                      |
        //zxzx:{                                  |

          e_forceinline Vector4 zxzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x22 );
                return r;
              #endif
            }
            r = Vector4( z, x, z, x );
            return r;
          }

        //}:                                      |
        //zxzy:{                                  |

          e_forceinline Vector4 zxzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x62 );
                return r;
              #endif
            }
            r = Vector4( z, x, z, y );
            return r;
          }

        //}:                                      |
        //zxzz:{                                  |

          e_forceinline Vector4 zxzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA2 );
                return r;
              #endif
            }
            r = Vector4( z, x, z, z );
            return r;
          }

        //}:                                      |
        //zxzw:{                                  |

          e_forceinline Vector4 zxzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE2 );
                return r;
              #endif
            }
            r = Vector4( z, x, z, w );
            return r;
          }

        //}:                                      |
        //zxwx:{                                  |

          e_forceinline Vector4 zxwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x32 );
                return r;
              #endif
            }
            r = Vector4( z, x, w, x );
            return r;
          }

        //}:                                      |
        //zxwy:{                                  |

          e_forceinline Vector4 zxwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x72 );
                return r;
              #endif
            }
            r = Vector4( z, x, w, y );
            return r;
          }

        //}:                                      |
        //zxwz:{                                  |

          e_forceinline Vector4 zxwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB2 );
                return r;
              #endif
            }
            r = Vector4( z, x, w, z );
            return r;
          }

        //}:                                      |
        //zxww:{                                  |

          e_forceinline Vector4 zxww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF2 );
                return r;
              #endif
            }
            r = Vector4( z, x, w, w );
            return r;
          }

        //}:                                      |
        //zyxx:{                                  |

          e_forceinline Vector4 zyxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x06 );
                return r;
              #endif
            }
            r = Vector4( z, y, x, x );
            return r;
          }

        //}:                                      |
        //zyxy:{                                  |

          e_forceinline Vector4 zyxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x46 );
                return r;
              #endif
            }
            r = Vector4( z, y, x, y );
            return r;
          }

        //}:                                      |
        //zyxz:{                                  |

          e_forceinline Vector4 zyxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x86 );
                return r;
              #endif
            }
            r = Vector4( z, y, x, z );
            return r;
          }

        //}:                                      |
        //zyxw:{                                  |

          e_forceinline Vector4 zyxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC6 );
                return r;
              #endif
            }
            r = Vector4( z, y, x, w );
            return r;
          }

        //}:                                      |
        //zyyx:{                                  |

          e_forceinline Vector4 zyyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x16 );
                return r;
              #endif
            }
            r = Vector4( z, y, y, x );
            return r;
          }

        //}:                                      |
        //zyyy:{                                  |

          e_forceinline Vector4 zyyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x56 );
                return r;
              #endif
            }
            r = Vector4( z, y, y, y );
            return r;
          }

        //}:                                      |
        //zyyz:{                                  |

          e_forceinline Vector4 zyyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x96 );
                return r;
              #endif
            }
            r = Vector4( z, y, y, z );
            return r;
          }

        //}:                                      |
        //zyyw:{                                  |

          e_forceinline Vector4 zyyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD6 );
                return r;
              #endif
            }
            r = Vector4( z, y, y, w );
            return r;
          }

        //}:                                      |
        //zyzx:{                                  |

          e_forceinline Vector4 zyzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x26 );
                return r;
              #endif
            }
            r = Vector4( z, y, z, x );
            return r;
          }

        //}:                                      |
        //zyzy:{                                  |

          e_forceinline Vector4 zyzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x66 );
                return r;
              #endif
            }
            r = Vector4( z, y, z, y );
            return r;
          }

        //}:                                      |
        //zyzz:{                                  |

          e_forceinline Vector4 zyzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA6 );
                return r;
              #endif
            }
            r = Vector4( z, y, z, z );
            return r;
          }

        //}:                                      |
        //zyzw:{                                  |

          e_forceinline Vector4 zyzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE6 );
                return r;
              #endif
            }
            r = Vector4( z, y, z, w );
            return r;
          }

        //}:                                      |
        //zywx:{                                  |

          e_forceinline Vector4 zywx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x36 );
                return r;
              #endif
            }
            r = Vector4( z, y, w, x );
            return r;
          }

        //}:                                      |
        //zywy:{                                  |

          e_forceinline Vector4 zywy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x76 );
                return r;
              #endif
            }
            r = Vector4( z, y, w, y );
            return r;
          }

        //}:                                      |
        //zywz:{                                  |

          e_forceinline Vector4 zywz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB6 );
                return r;
              #endif
            }
            r = Vector4( z, y, w, z );
            return r;
          }

        //}:                                      |
        //zyww:{                                  |

          e_forceinline Vector4 zyww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF6 );
                return r;
              #endif
            }
            r = Vector4( z, y, w, w );
            return r;
          }

        //}:                                      |
        //zzxx:{                                  |

          e_forceinline Vector4 zzxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0A );
                return r;
              #endif
            }
            r = Vector4( z, z, x, x );
            return r;
          }

        //}:                                      |
        //zzxy:{                                  |

          e_forceinline Vector4 zzxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4A );
                return r;
              #endif
            }
            r = Vector4( z, z, x, y );
            return r;
          }

        //}:                                      |
        //zzxz:{                                  |

          e_forceinline Vector4 zzxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8A );
                return r;
              #endif
            }
            r = Vector4( z, z, x, z );
            return r;
          }

        //}:                                      |
        //zzxw:{                                  |

          e_forceinline Vector4 zzxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCA );
                return r;
              #endif
            }
            r = Vector4( z, z, x, w );
            return r;
          }

        //}:                                      |
        //zzyx:{                                  |

          e_forceinline Vector4 zzyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1A );
                return r;
              #endif
            }
            r = Vector4( z, z, y, x );
            return r;
          }

        //}:                                      |
        //zzyy:{                                  |

          e_forceinline Vector4 zzyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5A );
                return r;
              #endif
            }
            r = Vector4( z, z, y, y );
            return r;
          }

        //}:                                      |
        //zzyz:{                                  |

          e_forceinline Vector4 zzyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9A );
                return r;
              #endif
            }
            r = Vector4( z, z, y, z );
            return r;
          }

        //}:                                      |
        //zzyw:{                                  |

          e_forceinline Vector4 zzyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDA );
                return r;
              #endif
            }
            r = Vector4( z, z, y, w );
            return r;
          }

        //}:                                      |
        //zzzx:{                                  |

          e_forceinline Vector4 zzzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2A );
                return r;
              #endif
            }
            r = Vector4( z, z, z, x );
            return r;
          }

        //}:                                      |
        //zzzy:{                                  |

          e_forceinline Vector4 zzzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6A );
                return r;
              #endif
            }
            r = Vector4( z, z, z, y );
            return r;
          }

        //}:                                      |
        //zzzz:{                                  |

          e_forceinline Vector4 zzzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAA );
                return r;
              #endif
            }
            r = Vector4( z, z, z, z );
            return r;
          }

        //}:                                      |
        //zzzw:{                                  |

          e_forceinline Vector4 zzzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xEA );
                return r;
              #endif
            }
            r = Vector4( z, z, z, w );
            return r;
          }

        //}:                                      |
        //zzwx:{                                  |

          e_forceinline Vector4 zzwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3A );
                return r;
              #endif
            }
            r = Vector4( z, z, w, x );
            return r;
          }

        //}:                                      |
        //zzwy:{                                  |

          e_forceinline Vector4 zzwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7A );
                return r;
              #endif
            }
            r = Vector4( z, z, w, y );
            return r;
          }

        //}:                                      |
        //zzwz:{                                  |

          e_forceinline Vector4 zzwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBA );
                return r;
              #endif
            }
            r = Vector4( z, z, w, z );
            return r;
          }

        //}:                                      |
        //zzww:{                                  |

          e_forceinline Vector4 zzww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFA );
                return r;
              #endif
            }
            r = Vector4( z, z, w, w );
            return r;
          }

        //}:                                      |
        //zwxx:{                                  |

          e_forceinline Vector4 zwxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0E );
                return r;
              #endif
            }
            r = Vector4( z, w, x, x );
            return r;
          }

        //}:                                      |
        //zwxy:{                                  |

          e_forceinline Vector4 zwxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4E );
                return r;
              #endif
            }
            r = Vector4( z, w, x, y );
            return r;
          }

        //}:                                      |
        //zwxz:{                                  |

          e_forceinline Vector4 zwxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8E );
                return r;
              #endif
            }
            r = Vector4( z, w, x, z );
            return r;
          }

        //}:                                      |
        //zwxw:{                                  |

          e_forceinline Vector4 zwxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCE );
                return r;
              #endif
            }
            r = Vector4( z, w, x, w );
            return r;
          }

        //}:                                      |
        //zwyx:{                                  |

          e_forceinline Vector4 zwyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1E );
                return r;
              #endif
            }
            r = Vector4( z, w, y, x );
            return r;
          }

        //}:                                      |
        //zwyy:{                                  |

          e_forceinline Vector4 zwyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5E );
                return r;
              #endif
            }
            r = Vector4( z, w, y, y );
            return r;
          }

        //}:                                      |
        //zwyz:{                                  |

          e_forceinline Vector4 zwyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9E );
                return r;
              #endif
            }
            r = Vector4( z, w, y, z );
            return r;
          }

        //}:                                      |
        //zwyw:{                                  |

          e_forceinline Vector4 zwyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDE );
                return r;
              #endif
            }
            r = Vector4( z, w, y, w );
            return r;
          }

        //}:                                      |
        //zwzx:{                                  |

          e_forceinline Vector4 zwzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2E );
                return r;
              #endif
            }
            r = Vector4( z, w, z, x );
            return r;
          }

        //}:                                      |
        //zwzy:{                                  |

          e_forceinline Vector4 zwzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6E );
                return r;
              #endif
            }
            r = Vector4( z, w, z, y );
            return r;
          }

        //}:                                      |
        //zwzz:{                                  |

          e_forceinline Vector4 zwzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAE );
                return r;
              #endif
            }
            r = Vector4( z, w, z, z );
            return r;
          }

        //}:                                      |
        //zwzw:{                                  |

          e_forceinline Vector4 zwzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xEE );
                return r;
              #endif
            }
            r = Vector4( z, w, z, w );
            return r;
          }

        //}:                                      |
        //zwwx:{                                  |

          e_forceinline Vector4 zwwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3E );
                return r;
              #endif
            }
            r = Vector4( z, w, w, x );
            return r;
          }

        //}:                                      |
        //zwwy:{                                  |

          e_forceinline Vector4 zwwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7E );
                return r;
              #endif
            }
            r = Vector4( z, w, w, y );
            return r;
          }

        //}:                                      |
        //zwwz:{                                  |

          e_forceinline Vector4 zwwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBE );
                return r;
              #endif
            }
            r = Vector4( z, w, w, z );
            return r;
          }

        //}:                                      |
        //zwww:{                                  |

          e_forceinline Vector4 zwww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFE );
                return r;
              #endif
            }
            r = Vector4( z, w, w, w );
            return r;
          }

        //}:                                      |
        //wxxx:{                                  |

          e_forceinline Vector4 wxxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x03 );
                return r;
              #endif
            }
            r = Vector4( w, x, x, x );
            return r;
          }

        //}:                                      |
        //wxxy:{                                  |

          e_forceinline Vector4 wxxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x43 );
                return r;
              #endif
            }
            r = Vector4( w, x, x, y );
            return r;
          }

        //}:                                      |
        //wxxz:{                                  |

          e_forceinline Vector4 wxxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x83 );
                return r;
              #endif
            }
            r = Vector4( w, x, x, z );
            return r;
          }

        //}:                                      |
        //wxxw:{                                  |

          e_forceinline Vector4 wxxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC3 );
                return r;
              #endif
            }
            r = Vector4( w, x, x, w );
            return r;
          }

        //}:                                      |
        //wxyx:{                                  |

          e_forceinline Vector4 wxyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x13 );
                return r;
              #endif
            }
            r = Vector4( w, x, y, x );
            return r;
          }

        //}:                                      |
        //wxyy:{                                  |

          e_forceinline Vector4 wxyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x53 );
                return r;
              #endif
            }
            r = Vector4( w, x, y, y );
            return r;
          }

        //}:                                      |
        //wxyz:{                                  |

          e_forceinline Vector4 wxyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x93 );
                return r;
              #endif
            }
            r = Vector4( w, x, y, z );
            return r;
          }

        //}:                                      |
        //wxyw:{                                  |

          e_forceinline Vector4 wxyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD3 );
                return r;
              #endif
            }
            r = Vector4( w, x, y, w );
            return r;
          }

        //}:                                      |
        //wxzx:{                                  |

          e_forceinline Vector4 wxzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x23 );
                return r;
              #endif
            }
            r = Vector4( w, x, z, x );
            return r;
          }

        //}:                                      |
        //wxzy:{                                  |

          e_forceinline Vector4 wxzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x63 );
                return r;
              #endif
            }
            r = Vector4( w, x, z, y );
            return r;
          }

        //}:                                      |
        //wxzz:{                                  |

          e_forceinline Vector4 wxzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA3 );
                return r;
              #endif
            }
            r = Vector4( w, x, z, z );
            return r;
          }

        //}:                                      |
        //wxzw:{                                  |

          e_forceinline Vector4 wxzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE3 );
                return r;
              #endif
            }
            r = Vector4( w, x, z, w );
            return r;
          }

        //}:                                      |
        //wxwx:{                                  |

          e_forceinline Vector4 wxwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x33 );
                return r;
              #endif
            }
            r = Vector4( w, x, w, x );
            return r;
          }

        //}:                                      |
        //wxwy:{                                  |

          e_forceinline Vector4 wxwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x73 );
                return r;
              #endif
            }
            r = Vector4( w, x, w, y );
            return r;
          }

        //}:                                      |
        //wxwz:{                                  |

          e_forceinline Vector4 wxwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB3 );
                return r;
              #endif
            }
            r = Vector4( w, x, w, z );
            return r;
          }

        //}:                                      |
        //wxww:{                                  |

          e_forceinline Vector4 wxww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF3 );
                return r;
              #endif
            }
            r = Vector4( w, x, w, w );
            return r;
          }

        //}:                                      |
        //wyxx:{                                  |

          e_forceinline Vector4 wyxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x07 );
                return r;
              #endif
            }
            r = Vector4( w, y, x, x );
            return r;
          }

        //}:                                      |
        //wyxy:{                                  |

          e_forceinline Vector4 wyxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x47 );
                return r;
              #endif
            }
            r = Vector4( w, y, x, y );
            return r;
          }

        //}:                                      |
        //wyxz:{                                  |

          e_forceinline Vector4 wyxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x87 );
                return r;
              #endif
            }
            r = Vector4( w, y, x, z );
            return r;
          }

        //}:                                      |
        //wyxw:{                                  |

          e_forceinline Vector4 wyxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xC7 );
                return r;
              #endif
            }
            r = Vector4( w, y, x, w );
            return r;
          }

        //}:                                      |
        //wyyx:{                                  |

          e_forceinline Vector4 wyyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x17 );
                return r;
              #endif
            }
            r = Vector4( w, y, y, x );
            return r;
          }

        //}:                                      |
        //wyyy:{                                  |

          e_forceinline Vector4 wyyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x57 );
                return r;
              #endif
            }
            r = Vector4( w, y, y, y );
            return r;
          }

        //}:                                      |
        //wyyz:{                                  |

          e_forceinline Vector4 wyyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x97 );
                return r;
              #endif
            }
            r = Vector4( w, y, y, z );
            return r;
          }

        //}:                                      |
        //wyyw:{                                  |

          e_forceinline Vector4 wyyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xD7 );
                return r;
              #endif
            }
            r = Vector4( w, y, y, w );
            return r;
          }

        //}:                                      |
        //wyzx:{                                  |

          e_forceinline Vector4 wyzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x27 );
                return r;
              #endif
            }
            r = Vector4( w, y, z, x );
            return r;
          }

        //}:                                      |
        //wyzy:{                                  |

          e_forceinline Vector4 wyzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x67 );
                return r;
              #endif
            }
            r = Vector4( w, y, z, y );
            return r;
          }

        //}:                                      |
        //wyzz:{                                  |

          e_forceinline Vector4 wyzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xA7 );
                return r;
              #endif
            }
            r = Vector4( w, y, z, z );
            return r;
          }

        //}:                                      |
        //wyzw:{                                  |

          e_forceinline Vector4 wyzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xE7 );
                return r;
              #endif
            }
            r = Vector4( w, y, z, w );
            return r;
          }

        //}:                                      |
        //wywx:{                                  |

          e_forceinline Vector4 wywx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x37 );
                return r;
              #endif
            }
            r = Vector4( w, y, w, x );
            return r;
          }

        //}:                                      |
        //wywy:{                                  |

          e_forceinline Vector4 wywy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x77 );
                return r;
              #endif
            }
            r = Vector4( w, y, w, y );
            return r;
          }

        //}:                                      |
        //wywz:{                                  |

          e_forceinline Vector4 wywz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xB7 );
                return r;
              #endif
            }
            r = Vector4( w, y, w, z );
            return r;
          }

        //}:                                      |
        //wyww:{                                  |

          e_forceinline Vector4 wyww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xF7 );
                return r;
              #endif
            }
            r = Vector4( w, y, w, w );
            return r;
          }

        //}:                                      |
        //wzxx:{                                  |

          e_forceinline Vector4 wzxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0B );
                return r;
              #endif
            }
            r = Vector4( w, z, x, x );
            return r;
          }

        //}:                                      |
        //wzxy:{                                  |

          e_forceinline Vector4 wzxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4B );
                return r;
              #endif
            }
            r = Vector4( w, z, x, y );
            return r;
          }

        //}:                                      |
        //wzxz:{                                  |

          e_forceinline Vector4 wzxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8B );
                return r;
              #endif
            }
            r = Vector4( w, z, x, z );
            return r;
          }

        //}:                                      |
        //wzxw:{                                  |

          e_forceinline Vector4 wzxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCB );
                return r;
              #endif
            }
            r = Vector4( w, z, x, w );
            return r;
          }

        //}:                                      |
        //wzyx:{                                  |

          e_forceinline Vector4 wzyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1B );
                return r;
              #endif
            }
            r = Vector4( w, z, y, x );
            return r;
          }

        //}:                                      |
        //wzyy:{                                  |

          e_forceinline Vector4 wzyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5B );
                return r;
              #endif
            }
            r = Vector4( w, z, y, y );
            return r;
          }

        //}:                                      |
        //wzyz:{                                  |

          e_forceinline Vector4 wzyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9B );
                return r;
              #endif
            }
            r = Vector4( w, z, y, z );
            return r;
          }

        //}:                                      |
        //wzyw:{                                  |

          e_forceinline Vector4 wzyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDB );
                return r;
              #endif
            }
            r = Vector4( w, z, y, w );
            return r;
          }

        //}:                                      |
        //wzzx:{                                  |

          e_forceinline Vector4 wzzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2B );
                return r;
              #endif
            }
            r = Vector4( w, z, z, x );
            return r;
          }

        //}:                                      |
        //wzzy:{                                  |

          e_forceinline Vector4 wzzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6B );
                return r;
              #endif
            }
            r = Vector4( w, z, z, y );
            return r;
          }

        //}:                                      |
        //wzzz:{                                  |

          e_forceinline Vector4 wzzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAB );
                return r;
              #endif
            }
            r = Vector4( w, z, z, z );
            return r;
          }

        //}:                                      |
        //wzzw:{                                  |

          e_forceinline Vector4 wzzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xEB );
                return r;
              #endif
            }
            r = Vector4( w, z, z, w );
            return r;
          }

        //}:                                      |
        //wzwx:{                                  |

          e_forceinline Vector4 wzwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3B );
                return r;
              #endif
            }
            r = Vector4( w, z, w, x );
            return r;
          }

        //}:                                      |
        //wzwy:{                                  |

          e_forceinline Vector4 wzwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7B );
                return r;
              #endif
            }
            r = Vector4( w, z, w, y );
            return r;
          }

        //}:                                      |
        //wzwz:{                                  |

          e_forceinline Vector4 wzwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBB );
                return r;
              #endif
            }
            r = Vector4( w, z, w, z );
            return r;
          }

        //}:                                      |
        //wzww:{                                  |

          e_forceinline Vector4 wzww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFB );
                return r;
              #endif
            }
            r = Vector4( w, z, w, w );
            return r;
          }

        //}:                                      |
        //wwxx:{                                  |

          e_forceinline Vector4 wwxx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x0F );
                return r;
              #endif
            }
            r = Vector4( w, w, x, x );
            return r;
          }

        //}:                                      |
        //wwxy:{                                  |

          e_forceinline Vector4 wwxy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x4F );
                return r;
              #endif
            }
            r = Vector4( w, w, x, y );
            return r;
          }

        //}:                                      |
        //wwxz:{                                  |

          e_forceinline Vector4 wwxz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x8F );
                return r;
              #endif
            }
            r = Vector4( w, w, x, z );
            return r;
          }

        //}:                                      |
        //wwxw:{                                  |

          e_forceinline Vector4 wwxw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xCF );
                return r;
              #endif
            }
            r = Vector4( w, w, x, w );
            return r;
          }

        //}:                                      |
        //wwyx:{                                  |

          e_forceinline Vector4 wwyx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x1F );
                return r;
              #endif
            }
            r = Vector4( w, w, y, x );
            return r;
          }

        //}:                                      |
        //wwyy:{                                  |

          e_forceinline Vector4 wwyy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x5F );
                return r;
              #endif
            }
            r = Vector4( w, w, y, y );
            return r;
          }

        //}:                                      |
        //wwyz:{                                  |

          e_forceinline Vector4 wwyz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x9F );
                return r;
              #endif
            }
            r = Vector4( w, w, y, z );
            return r;
          }

        //}:                                      |
        //wwyw:{                                  |

          e_forceinline Vector4 wwyw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xDF );
                return r;
              #endif
            }
            r = Vector4( w, w, y, w );
            return r;
          }

        //}:                                      |
        //wwzx:{                                  |

          e_forceinline Vector4 wwzx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x2F );
                return r;
              #endif
            }
            r = Vector4( w, w, z, x );
            return r;
          }

        //}:                                      |
        //wwzy:{                                  |

          e_forceinline Vector4 wwzy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x6F );
                return r;
              #endif
            }
            r = Vector4( w, w, z, y );
            return r;
          }

        //}:                                      |
        //wwzz:{                                  |

          e_forceinline Vector4 wwzz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xAF );
                return r;
              #endif
            }
            r = Vector4( w, w, z, z );
            return r;
          }

        //}:                                      |
        //wwzw:{                                  |

          e_forceinline Vector4 wwzw()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xEF );
                return r;
              #endif
            }
            r = Vector4( w, w, z, w );
            return r;
          }

        //}:                                      |
        //wwwx:{                                  |

          e_forceinline Vector4 wwwx()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x3F );
                return r;
              #endif
            }
            r = Vector4( w, w, w, x );
            return r;
          }

        //}:                                      |
        //wwwy:{                                  |

          e_forceinline Vector4 wwwy()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0x7F );
                return r;
              #endif
            }
            r = Vector4( w, w, w, y );
            return r;
          }

        //}:                                      |
        //wwwz:{                                  |

          e_forceinline Vector4 wwwz()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xBF );
                return r;
              #endif
            }
            r = Vector4( w, w, w, z );
            return r;
          }

        //}:                                      |
        //wwww:{                                  |

          e_forceinline Vector4 wwww()const{
            Vector4 r;
            if( is_sse() ){
              #ifdef __SSE__
                r.i = _mm_shuffle_epi32( i, 0xFF );
                return r;
              #endif
            }
            r = Vector4( w, w, w, w );
            return r;
          }

        //}:                                      |
        //xxx:{                                   |

          e_forceinline Vector3 xxx()const{
            return Vector3( x, x, x );
          }

        //}:                                      |
        //xxy:{                                   |

          e_forceinline Vector3 xxy()const{
            return Vector3( x, x, y );
          }

        //}:                                      |
        //xxz:{                                   |

          e_forceinline Vector3 xxz()const{
            return Vector3( x, x, z );
          }

        //}:                                      |
        //xyx:{                                   |

          e_forceinline Vector3 xyx()const{
            return Vector3( x, y, x );
          }

        //}:                                      |
        //xyy:{                                   |

          e_forceinline Vector3 xyy()const{
            return Vector3( x, y, y );
          }

        //}:                                      |
        //xyz:{                                   |

          e_forceinline Vector3 xyz()const{
            return Vector3( x, y, z );
          }

        //}:                                      |
        //xzx:{                                   |

          e_forceinline Vector3 xzx()const{
            return Vector3( x, z, x );
          }

        //}:                                      |
        //xzy:{                                   |

          e_forceinline Vector3 xzy()const{
            return Vector3( x, z, y );
          }

        //}:                                      |
        //xzz:{                                   |

          e_forceinline Vector3 xzz()const{
            return Vector3( x, z, z );
          }

        //}:                                      |
        //yxx:{                                   |

          e_forceinline Vector3 yxx()const{
            return Vector3( y, x, x );
          }

        //}:                                      |
        //yxy:{                                   |

          e_forceinline Vector3 yxy()const{
            return Vector3( y, x, y );
          }

        //}:                                      |
        //yxz:{                                   |

          e_forceinline Vector3 yxz()const{
            return Vector3( y, x, z );
          }

        //}:                                      |
        //yyx:{                                   |

          e_forceinline Vector3 yyx()const{
            return Vector3( y, y, x );
          }

        //}:                                      |
        //yyy:{                                   |

          e_forceinline Vector3 yyy()const{
            return Vector3( y, y, y );
          }

        //}:                                      |
        //yyz:{                                   |

          e_forceinline Vector3 yyz()const{
            return Vector3( y, y, z );
          }

        //}:                                      |
        //yzx:{                                   |

          e_forceinline Vector3 yzx()const{
            return Vector3( y, z, x );
          }

        //}:                                      |
        //yzy:{                                   |

          e_forceinline Vector3 yzy()const{
            return Vector3( y, z, y );
          }

        //}:                                      |
        //yzz:{                                   |

          e_forceinline Vector3 yzz()const{
            return Vector3( y, z, z );
          }

        //}:                                      |
        //zxx:{                                   |

          e_forceinline Vector3 zxx()const{
            return Vector3( z, x, x );
          }

        //}:                                      |
        //zxy:{                                   |

          e_forceinline Vector3 zxy()const{
            return Vector3( z, x, y );
          }

        //}:                                      |
        //zxz:{                                   |

          e_forceinline Vector3 zxz()const{
            return Vector3( z, x, z );
          }

        //}:                                      |
        //zyx:{                                   |

          e_forceinline Vector3 zyx()const{
            return Vector3( z, y, x );
          }

        //}:                                      |
        //zyy:{                                   |

          e_forceinline Vector3 zyy()const{
            return Vector3( z, y, y );
          }

        //}:                                      |
        //zyz:{                                   |

          e_forceinline Vector3 zyz()const{
            return Vector3( z, y, z );
          }

        //}:                                      |
        //zzx:{                                   |

          e_forceinline Vector3 zzx()const{
            return Vector3( z, z, x );
          }

        //}:                                      |
        //zzy:{                                   |

          e_forceinline Vector3 zzy()const{
            return Vector3( z, z, y );
          }

        //}:                                      |
        //zzz:{                                   |

          e_forceinline Vector3 zzz()const{
            return Vector3( z, z, z );
          }

        //}:                                      |
        //xx:{                                    |

          e_forceinline Vector2 xx()const{
            return Vector2( x, x );
          }

        //}:                                      |
        //xy:{                                    |

          e_forceinline Vector2 xy()const{
            return Vector2( x, y );
          }

        //}:                                      |
        //xz:{                                    |

          e_forceinline Vector2 xz()const{
            return Vector2( x, z );
          }

        //}:                                      |
        //xw:{                                    |

          e_forceinline Vector2 xw()const{
            return Vector2( x, w );
          }

        //}:                                      |
        //yx:{                                    |

          e_forceinline Vector2 yx()const{
            return Vector2( y, x );
          }

        //}:                                      |
        //yy:{                                    |

          e_forceinline Vector2 yy()const{
            return Vector2( y, y );
          }

        //}:                                      |
        //yz:{                                    |

          e_forceinline Vector2 yz()const{
            return Vector2( y, z );
          }

        //}:                                      |
        //yw:{                                    |

          e_forceinline Vector2 yw()const{
            return Vector2( y, w );
          }

        //}:                                      |
        //zx:{                                    |

          e_forceinline Vector2 zx()const{
            return Vector2( z, x );
          }

        //}:                                      |
        //zy:{                                    |

          e_forceinline Vector2 zy()const{
            return Vector2( z, y );
          }

        //}:                                      |
        //zz:{                                    |

          e_forceinline Vector2 zz()const{
            return Vector2( z, z );
          }

        //}:                                      |
        //zw:{                                    |

          e_forceinline Vector2 zw()const{
            return Vector2( z, w );
          }

        //}:                                      |
        //wx:{                                    |

          e_forceinline Vector2 wx()const{
            return Vector2( w, x );
          }

        //}:                                      |
        //wy:{                                    |

          e_forceinline Vector2 wy()const{
            return Vector2( w, y );
          }

        //}:                                      |
        //wz:{                                    |

          e_forceinline Vector2 wz()const{
            return Vector2( w, z );
          }

        //}:                                      |
        //ww:{                                    |

          e_forceinline Vector2 ww()const{
            return Vector2( w, w );
          }

        //}:                                      |
      //}:                                        |
      //[swizzling]:{                             |
        //rrrr:{                                  |

          e_forceinline Vector4 rrrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x00 );
                return r;
              #endif
            }
            c = Vector4( r, r, r, r );
            return c;
          }

        //}:                                      |
        //rrrg:{                                  |

          e_forceinline Vector4 rrrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x40 );
                return r;
              #endif
            }
            c = Vector4( r, r, r, g );
            return c;
          }

        //}:                                      |
        //rrrb:{                                  |

          e_forceinline Vector4 rrrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x80 );
                return r;
              #endif
            }
            c = Vector4( r, r, r, b );
            return c;
          }

        //}:                                      |
        //rrra:{                                  |

          e_forceinline Vector4 rrra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC0 );
                return r;
              #endif
            }
            c = Vector4( r, r, r, a );
            return c;
          }

        //}:                                      |
        //rrgr:{                                  |

          e_forceinline Vector4 rrgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x10 );
                return r;
              #endif
            }
            c = Vector4( r, r, g, r );
            return c;
          }

        //}:                                      |
        //rrgg:{                                  |

          e_forceinline Vector4 rrgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x50 );
                return r;
              #endif
            }
            c = Vector4( r, r, g, g );
            return c;
          }

        //}:                                      |
        //rrgb:{                                  |

          e_forceinline Vector4 rrgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x90 );
                return r;
              #endif
            }
            c = Vector4( r, r, g, b );
            return c;
          }

        //}:                                      |
        //rrga:{                                  |

          e_forceinline Vector4 rrga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD0 );
                return r;
              #endif
            }
            c = Vector4( r, r, g, a );
            return c;
          }

        //}:                                      |
        //rrbr:{                                  |

          e_forceinline Vector4 rrbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x20 );
                return r;
              #endif
            }
            c = Vector4( r, r, b, r );
            return c;
          }

        //}:                                      |
        //rrbg:{                                  |

          e_forceinline Vector4 rrbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x60 );
                return r;
              #endif
            }
            c = Vector4( r, r, b, g );
            return c;
          }

        //}:                                      |
        //rrbb:{                                  |

          e_forceinline Vector4 rrbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA0 );
                return r;
              #endif
            }
            c = Vector4( r, r, b, b );
            return c;
          }

        //}:                                      |
        //rrba:{                                  |

          e_forceinline Vector4 rrba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE0 );
                return r;
              #endif
            }
            c = Vector4( r, r, b, a );
            return c;
          }

        //}:                                      |
        //rrar:{                                  |

          e_forceinline Vector4 rrar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x30 );
                return r;
              #endif
            }
            c = Vector4( r, r, a, r );
            return c;
          }

        //}:                                      |
        //rrag:{                                  |

          e_forceinline Vector4 rrag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x70 );
                return r;
              #endif
            }
            c = Vector4( r, r, a, g );
            return c;
          }

        //}:                                      |
        //rrab:{                                  |

          e_forceinline Vector4 rrab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB0 );
                return r;
              #endif
            }
            c = Vector4( r, r, a, b );
            return c;
          }

        //}:                                      |
        //rraa:{                                  |

          e_forceinline Vector4 rraa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF0 );
                return r;
              #endif
            }
            c = Vector4( r, r, a, a );
            return c;
          }

        //}:                                      |
        //rgrr:{                                  |

          e_forceinline Vector4 rgrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x04 );
                return r;
              #endif
            }
            c = Vector4( r, g, r, r );
            return c;
          }

        //}:                                      |
        //rgrg:{                                  |

          e_forceinline Vector4 rgrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x44 );
                return r;
              #endif
            }
            c = Vector4( r, g, r, g );
            return c;
          }

        //}:                                      |
        //rgrb:{                                  |

          e_forceinline Vector4 rgrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x84 );
                return r;
              #endif
            }
            c = Vector4( r, g, r, b );
            return c;
          }

        //}:                                      |
        //rgra:{                                  |

          e_forceinline Vector4 rgra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC4 );
                return r;
              #endif
            }
            c = Vector4( r, g, r, a );
            return c;
          }

        //}:                                      |
        //rggr:{                                  |

          e_forceinline Vector4 rggr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x14 );
                return r;
              #endif
            }
            c = Vector4( r, g, g, r );
            return c;
          }

        //}:                                      |
        //rggg:{                                  |

          e_forceinline Vector4 rggg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x54 );
                return r;
              #endif
            }
            c = Vector4( r, g, g, g );
            return c;
          }

        //}:                                      |
        //rggb:{                                  |

          e_forceinline Vector4 rggb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x94 );
                return r;
              #endif
            }
            c = Vector4( r, g, g, b );
            return c;
          }

        //}:                                      |
        //rgga:{                                  |

          e_forceinline Vector4 rgga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD4 );
                return r;
              #endif
            }
            c = Vector4( r, g, g, a );
            return c;
          }

        //}:                                      |
        //rgbr:{                                  |

          e_forceinline Vector4 rgbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x24 );
                return r;
              #endif
            }
            c = Vector4( r, g, b, r );
            return c;
          }

        //}:                                      |
        //rgbg:{                                  |

          e_forceinline Vector4 rgbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x64 );
                return r;
              #endif
            }
            c = Vector4( r, g, b, g );
            return c;
          }

        //}:                                      |
        //rgbb:{                                  |

          e_forceinline Vector4 rgbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA4 );
                return r;
              #endif
            }
            c = Vector4( r, g, b, b );
            return c;
          }

        //}:                                      |
        //rgba:{                                  |

          e_forceinline Vector4 rgba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE4 );
                return r;
              #endif
            }
            c = Vector4( r, g, b, a );
            return c;
          }

        //}:                                      |
        //rgar:{                                  |

          e_forceinline Vector4 rgar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x34 );
                return r;
              #endif
            }
            c = Vector4( r, g, a, r );
            return c;
          }

        //}:                                      |
        //rgag:{                                  |

          e_forceinline Vector4 rgag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x74 );
                return r;
              #endif
            }
            c = Vector4( r, g, a, g );
            return c;
          }

        //}:                                      |
        //rgab:{                                  |

          e_forceinline Vector4 rgab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB4 );
                return r;
              #endif
            }
            c = Vector4( r, g, a, b );
            return c;
          }

        //}:                                      |
        //rgaa:{                                  |

          e_forceinline Vector4 rgaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF4 );
                return r;
              #endif
            }
            c = Vector4( r, g, a, a );
            return c;
          }

        //}:                                      |
        //rbrr:{                                  |

          e_forceinline Vector4 rbrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x08 );
                return r;
              #endif
            }
            c = Vector4( r, b, r, r );
            return c;
          }

        //}:                                      |
        //rbrg:{                                  |

          e_forceinline Vector4 rbrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x48 );
                return r;
              #endif
            }
            c = Vector4( r, b, r, g );
            return c;
          }

        //}:                                      |
        //rbrb:{                                  |

          e_forceinline Vector4 rbrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x88 );
                return r;
              #endif
            }
            c = Vector4( r, b, r, b );
            return c;
          }

        //}:                                      |
        //rbra:{                                  |

          e_forceinline Vector4 rbra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC8 );
                return r;
              #endif
            }
            c = Vector4( r, b, r, a );
            return c;
          }

        //}:                                      |
        //rbgr:{                                  |

          e_forceinline Vector4 rbgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x18 );
                return r;
              #endif
            }
            c = Vector4( r, b, g, r );
            return c;
          }

        //}:                                      |
        //rbgg:{                                  |

          e_forceinline Vector4 rbgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x58 );
                return r;
              #endif
            }
            c = Vector4( r, b, g, g );
            return c;
          }

        //}:                                      |
        //rbgb:{                                  |

          e_forceinline Vector4 rbgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x98 );
                return r;
              #endif
            }
            c = Vector4( r, b, g, b );
            return c;
          }

        //}:                                      |
        //rbga:{                                  |

          e_forceinline Vector4 rbga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD8 );
                return r;
              #endif
            }
            c = Vector4( r, b, g, a );
            return c;
          }

        //}:                                      |
        //rbbr:{                                  |

          e_forceinline Vector4 rbbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x28 );
                return r;
              #endif
            }
            c = Vector4( r, b, b, r );
            return c;
          }

        //}:                                      |
        //rbbg:{                                  |

          e_forceinline Vector4 rbbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x68 );
                return r;
              #endif
            }
            c = Vector4( r, b, b, g );
            return c;
          }

        //}:                                      |
        //rbbb:{                                  |

          e_forceinline Vector4 rbbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA8 );
                return r;
              #endif
            }
            c = Vector4( r, b, b, b );
            return c;
          }

        //}:                                      |
        //rbba:{                                  |

          e_forceinline Vector4 rbba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE8 );
                return r;
              #endif
            }
            c = Vector4( r, b, b, a );
            return c;
          }

        //}:                                      |
        //rbar:{                                  |

          e_forceinline Vector4 rbar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x38 );
                return r;
              #endif
            }
            c = Vector4( r, b, a, r );
            return c;
          }

        //}:                                      |
        //rbag:{                                  |

          e_forceinline Vector4 rbag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x78 );
                return r;
              #endif
            }
            c = Vector4( r, b, a, g );
            return c;
          }

        //}:                                      |
        //rbab:{                                  |

          e_forceinline Vector4 rbab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB8 );
                return r;
              #endif
            }
            c = Vector4( r, b, a, b );
            return c;
          }

        //}:                                      |
        //rbaa:{                                  |

          e_forceinline Vector4 rbaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF8 );
                return r;
              #endif
            }
            c = Vector4( r, b, a, a );
            return c;
          }

        //}:                                      |
        //rarr:{                                  |

          e_forceinline Vector4 rarr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0C );
                return r;
              #endif
            }
            c = Vector4( r, a, r, r );
            return c;
          }

        //}:                                      |
        //rarg:{                                  |

          e_forceinline Vector4 rarg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4C );
                return r;
              #endif
            }
            c = Vector4( r, a, r, g );
            return c;
          }

        //}:                                      |
        //rarb:{                                  |

          e_forceinline Vector4 rarb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8C );
                return r;
              #endif
            }
            c = Vector4( r, a, r, b );
            return c;
          }

        //}:                                      |
        //rara:{                                  |

          e_forceinline Vector4 rara()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCC );
                return r;
              #endif
            }
            c = Vector4( r, a, r, a );
            return c;
          }

        //}:                                      |
        //ragr:{                                  |

          e_forceinline Vector4 ragr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1C );
                return r;
              #endif
            }
            c = Vector4( r, a, g, r );
            return c;
          }

        //}:                                      |
        //ragg:{                                  |

          e_forceinline Vector4 ragg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5C );
                return r;
              #endif
            }
            c = Vector4( r, a, g, g );
            return c;
          }

        //}:                                      |
        //ragb:{                                  |

          e_forceinline Vector4 ragb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9C );
                return r;
              #endif
            }
            c = Vector4( r, a, g, b );
            return c;
          }

        //}:                                      |
        //raga:{                                  |

          e_forceinline Vector4 raga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDC );
                return r;
              #endif
            }
            c = Vector4( r, a, g, a );
            return c;
          }

        //}:                                      |
        //rabr:{                                  |

          e_forceinline Vector4 rabr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2C );
                return r;
              #endif
            }
            c = Vector4( r, a, b, r );
            return c;
          }

        //}:                                      |
        //rabg:{                                  |

          e_forceinline Vector4 rabg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6C );
                return r;
              #endif
            }
            c = Vector4( r, a, b, g );
            return c;
          }

        //}:                                      |
        //rabb:{                                  |

          e_forceinline Vector4 rabb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAC );
                return r;
              #endif
            }
            c = Vector4( r, a, b, b );
            return c;
          }

        //}:                                      |
        //raba:{                                  |

          e_forceinline Vector4 raba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xEC );
                return r;
              #endif
            }
            c = Vector4( r, a, b, a );
            return c;
          }

        //}:                                      |
        //raar:{                                  |

          e_forceinline Vector4 raar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3C );
                return r;
              #endif
            }
            c = Vector4( r, a, a, r );
            return c;
          }

        //}:                                      |
        //raag:{                                  |

          e_forceinline Vector4 raag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7C );
                return r;
              #endif
            }
            c = Vector4( r, a, a, g );
            return c;
          }

        //}:                                      |
        //raab:{                                  |

          e_forceinline Vector4 raab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBC );
                return r;
              #endif
            }
            c = Vector4( r, a, a, b );
            return c;
          }

        //}:                                      |
        //raaa:{                                  |

          e_forceinline Vector4 raaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFC );
                return r;
              #endif
            }
            c = Vector4( r, a, a, a );
            return c;
          }

        //}:                                      |
        //grrr:{                                  |

          e_forceinline Vector4 grrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x01 );
                return r;
              #endif
            }
            c = Vector4( g, r, r, r );
            return c;
          }

        //}:                                      |
        //grrg:{                                  |

          e_forceinline Vector4 grrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x41 );
                return r;
              #endif
            }
            c = Vector4( g, r, r, g );
            return c;
          }

        //}:                                      |
        //grrb:{                                  |

          e_forceinline Vector4 grrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x81 );
                return r;
              #endif
            }
            c = Vector4( g, r, r, b );
            return c;
          }

        //}:                                      |
        //grra:{                                  |

          e_forceinline Vector4 grra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC1 );
                return r;
              #endif
            }
            c = Vector4( g, r, r, a );
            return c;
          }

        //}:                                      |
        //grgr:{                                  |

          e_forceinline Vector4 grgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x11 );
                return r;
              #endif
            }
            c = Vector4( g, r, g, r );
            return c;
          }

        //}:                                      |
        //grgg:{                                  |

          e_forceinline Vector4 grgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x51 );
                return r;
              #endif
            }
            c = Vector4( g, r, g, g );
            return c;
          }

        //}:                                      |
        //grgb:{                                  |

          e_forceinline Vector4 grgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x91 );
                return r;
              #endif
            }
            c = Vector4( g, r, g, b );
            return c;
          }

        //}:                                      |
        //grga:{                                  |

          e_forceinline Vector4 grga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD1 );
                return r;
              #endif
            }
            c = Vector4( g, r, g, a );
            return c;
          }

        //}:                                      |
        //grbr:{                                  |

          e_forceinline Vector4 grbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x21 );
                return r;
              #endif
            }
            c = Vector4( g, r, b, r );
            return c;
          }

        //}:                                      |
        //grbg:{                                  |

          e_forceinline Vector4 grbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x61 );
                return r;
              #endif
            }
            c = Vector4( g, r, b, g );
            return c;
          }

        //}:                                      |
        //grbb:{                                  |

          e_forceinline Vector4 grbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA1 );
                return r;
              #endif
            }
            c = Vector4( g, r, b, b );
            return c;
          }

        //}:                                      |
        //grba:{                                  |

          e_forceinline Vector4 grba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE1 );
                return r;
              #endif
            }
            c = Vector4( g, r, b, a );
            return c;
          }

        //}:                                      |
        //grar:{                                  |

          e_forceinline Vector4 grar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x31 );
                return r;
              #endif
            }
            c = Vector4( g, r, a, r );
            return c;
          }

        //}:                                      |
        //grag:{                                  |

          e_forceinline Vector4 grag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x71 );
                return r;
              #endif
            }
            c = Vector4( g, r, a, g );
            return c;
          }

        //}:                                      |
        //grab:{                                  |

          e_forceinline Vector4 grab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB1 );
                return r;
              #endif
            }
            c = Vector4( g, r, a, b );
            return c;
          }

        //}:                                      |
        //graa:{                                  |

          e_forceinline Vector4 graa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF1 );
                return r;
              #endif
            }
            c = Vector4( g, r, a, a );
            return c;
          }

        //}:                                      |
        //ggrr:{                                  |

          e_forceinline Vector4 ggrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x05 );
                return r;
              #endif
            }
            c = Vector4( g, g, r, r );
            return c;
          }

        //}:                                      |
        //ggrg:{                                  |

          e_forceinline Vector4 ggrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x45 );
                return r;
              #endif
            }
            c = Vector4( g, g, r, g );
            return c;
          }

        //}:                                      |
        //ggrb:{                                  |

          e_forceinline Vector4 ggrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x85 );
                return r;
              #endif
            }
            c = Vector4( g, g, r, b );
            return c;
          }

        //}:                                      |
        //ggra:{                                  |

          e_forceinline Vector4 ggra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC5 );
                return r;
              #endif
            }
            c = Vector4( g, g, r, a );
            return c;
          }

        //}:                                      |
        //gggr:{                                  |

          e_forceinline Vector4 gggr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x15 );
                return r;
              #endif
            }
            c = Vector4( g, g, g, r );
            return c;
          }

        //}:                                      |
        //gggg:{                                  |

          e_forceinline Vector4 gggg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x55 );
                return r;
              #endif
            }
            c = Vector4( g, g, g, g );
            return c;
          }

        //}:                                      |
        //gggb:{                                  |

          e_forceinline Vector4 gggb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x95 );
                return r;
              #endif
            }
            c = Vector4( g, g, g, b );
            return c;
          }

        //}:                                      |
        //ggga:{                                  |

          e_forceinline Vector4 ggga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD5 );
                return r;
              #endif
            }
            c = Vector4( g, g, g, a );
            return c;
          }

        //}:                                      |
        //ggbr:{                                  |

          e_forceinline Vector4 ggbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x25 );
                return r;
              #endif
            }
            c = Vector4( g, g, b, r );
            return c;
          }

        //}:                                      |
        //ggbg:{                                  |

          e_forceinline Vector4 ggbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x65 );
                return r;
              #endif
            }
            c = Vector4( g, g, b, g );
            return c;
          }

        //}:                                      |
        //ggbb:{                                  |

          e_forceinline Vector4 ggbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA5 );
                return r;
              #endif
            }
            c = Vector4( g, g, b, b );
            return c;
          }

        //}:                                      |
        //ggba:{                                  |

          e_forceinline Vector4 ggba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE5 );
                return r;
              #endif
            }
            c = Vector4( g, g, b, a );
            return c;
          }

        //}:                                      |
        //ggar:{                                  |

          e_forceinline Vector4 ggar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x35 );
                return r;
              #endif
            }
            c = Vector4( g, g, a, r );
            return c;
          }

        //}:                                      |
        //ggag:{                                  |

          e_forceinline Vector4 ggag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x75 );
                return r;
              #endif
            }
            c = Vector4( g, g, a, g );
            return c;
          }

        //}:                                      |
        //ggab:{                                  |

          e_forceinline Vector4 ggab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB5 );
                return r;
              #endif
            }
            c = Vector4( g, g, a, b );
            return c;
          }

        //}:                                      |
        //ggaa:{                                  |

          e_forceinline Vector4 ggaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF5 );
                return r;
              #endif
            }
            c = Vector4( g, g, a, a );
            return c;
          }

        //}:                                      |
        //gbrr:{                                  |

          e_forceinline Vector4 gbrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x09 );
                return r;
              #endif
            }
            c = Vector4( g, b, r, r );
            return c;
          }

        //}:                                      |
        //gbrg:{                                  |

          e_forceinline Vector4 gbrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x49 );
                return r;
              #endif
            }
            c = Vector4( g, b, r, g );
            return c;
          }

        //}:                                      |
        //gbrb:{                                  |

          e_forceinline Vector4 gbrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x89 );
                return r;
              #endif
            }
            c = Vector4( g, b, r, b );
            return c;
          }

        //}:                                      |
        //gbra:{                                  |

          e_forceinline Vector4 gbra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC9 );
                return r;
              #endif
            }
            c = Vector4( g, b, r, a );
            return c;
          }

        //}:                                      |
        //gbgr:{                                  |

          e_forceinline Vector4 gbgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x19 );
                return r;
              #endif
            }
            c = Vector4( g, b, g, r );
            return c;
          }

        //}:                                      |
        //gbgg:{                                  |

          e_forceinline Vector4 gbgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x59 );
                return r;
              #endif
            }
            c = Vector4( g, b, g, g );
            return c;
          }

        //}:                                      |
        //gbgb:{                                  |

          e_forceinline Vector4 gbgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x99 );
                return r;
              #endif
            }
            c = Vector4( g, b, g, b );
            return c;
          }

        //}:                                      |
        //gbga:{                                  |

          e_forceinline Vector4 gbga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD9 );
                return r;
              #endif
            }
            c = Vector4( g, b, g, a );
            return c;
          }

        //}:                                      |
        //gbbr:{                                  |

          e_forceinline Vector4 gbbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x29 );
                return r;
              #endif
            }
            c = Vector4( g, b, b, r );
            return c;
          }

        //}:                                      |
        //gbbg:{                                  |

          e_forceinline Vector4 gbbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x69 );
                return r;
              #endif
            }
            c = Vector4( g, b, b, g );
            return c;
          }

        //}:                                      |
        //gbbb:{                                  |

          e_forceinline Vector4 gbbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA9 );
                return r;
              #endif
            }
            c = Vector4( g, b, b, b );
            return c;
          }

        //}:                                      |
        //gbba:{                                  |

          e_forceinline Vector4 gbba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE9 );
                return r;
              #endif
            }
            c = Vector4( g, b, b, a );
            return c;
          }

        //}:                                      |
        //gbar:{                                  |

          e_forceinline Vector4 gbar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x39 );
                return r;
              #endif
            }
            c = Vector4( g, b, a, r );
            return c;
          }

        //}:                                      |
        //gbag:{                                  |

          e_forceinline Vector4 gbag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x79 );
                return r;
              #endif
            }
            c = Vector4( g, b, a, g );
            return c;
          }

        //}:                                      |
        //gbab:{                                  |

          e_forceinline Vector4 gbab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB9 );
                return r;
              #endif
            }
            c = Vector4( g, b, a, b );
            return c;
          }

        //}:                                      |
        //gbaa:{                                  |

          e_forceinline Vector4 gbaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF9 );
                return r;
              #endif
            }
            c = Vector4( g, b, a, a );
            return c;
          }

        //}:                                      |
        //garr:{                                  |

          e_forceinline Vector4 garr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0D );
                return r;
              #endif
            }
            c = Vector4( g, a, r, r );
            return c;
          }

        //}:                                      |
        //garg:{                                  |

          e_forceinline Vector4 garg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4D );
                return r;
              #endif
            }
            c = Vector4( g, a, r, g );
            return c;
          }

        //}:                                      |
        //garb:{                                  |

          e_forceinline Vector4 garb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8D );
                return r;
              #endif
            }
            c = Vector4( g, a, r, b );
            return c;
          }

        //}:                                      |
        //gara:{                                  |

          e_forceinline Vector4 gara()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCD );
                return r;
              #endif
            }
            c = Vector4( g, a, r, a );
            return c;
          }

        //}:                                      |
        //gagr:{                                  |

          e_forceinline Vector4 gagr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1D );
                return r;
              #endif
            }
            c = Vector4( g, a, g, r );
            return c;
          }

        //}:                                      |
        //gagg:{                                  |

          e_forceinline Vector4 gagg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5D );
                return r;
              #endif
            }
            c = Vector4( g, a, g, g );
            return c;
          }

        //}:                                      |
        //gagb:{                                  |

          e_forceinline Vector4 gagb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9D );
                return r;
              #endif
            }
            c = Vector4( g, a, g, b );
            return c;
          }

        //}:                                      |
        //gaga:{                                  |

          e_forceinline Vector4 gaga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDD );
                return r;
              #endif
            }
            c = Vector4( g, a, g, a );
            return c;
          }

        //}:                                      |
        //gabr:{                                  |

          e_forceinline Vector4 gabr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2D );
                return r;
              #endif
            }
            c = Vector4( g, a, b, r );
            return c;
          }

        //}:                                      |
        //gabg:{                                  |

          e_forceinline Vector4 gabg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6D );
                return r;
              #endif
            }
            c = Vector4( g, a, b, g );
            return c;
          }

        //}:                                      |
        //gabb:{                                  |

          e_forceinline Vector4 gabb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAD );
                return r;
              #endif
            }
            c = Vector4( g, a, b, b );
            return c;
          }

        //}:                                      |
        //gaba:{                                  |

          e_forceinline Vector4 gaba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xED );
                return r;
              #endif
            }
            c = Vector4( g, a, b, a );
            return c;
          }

        //}:                                      |
        //gaar:{                                  |

          e_forceinline Vector4 gaar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3D );
                return r;
              #endif
            }
            c = Vector4( g, a, a, r );
            return c;
          }

        //}:                                      |
        //gaag:{                                  |

          e_forceinline Vector4 gaag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7D );
                return r;
              #endif
            }
            c = Vector4( g, a, a, g );
            return c;
          }

        //}:                                      |
        //gaab:{                                  |

          e_forceinline Vector4 gaab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBD );
                return r;
              #endif
            }
            c = Vector4( g, a, a, b );
            return c;
          }

        //}:                                      |
        //gaaa:{                                  |

          e_forceinline Vector4 gaaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFD );
                return r;
              #endif
            }
            c = Vector4( g, a, a, a );
            return c;
          }

        //}:                                      |
        //brrr:{                                  |

          e_forceinline Vector4 brrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x02 );
                return r;
              #endif
            }
            c = Vector4( b, r, r, r );
            return c;
          }

        //}:                                      |
        //brrg:{                                  |

          e_forceinline Vector4 brrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x42 );
                return r;
              #endif
            }
            c = Vector4( b, r, r, g );
            return c;
          }

        //}:                                      |
        //brrb:{                                  |

          e_forceinline Vector4 brrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x82 );
                return r;
              #endif
            }
            c = Vector4( b, r, r, b );
            return c;
          }

        //}:                                      |
        //brra:{                                  |

          e_forceinline Vector4 brra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC2 );
                return r;
              #endif
            }
            c = Vector4( b, r, r, a );
            return c;
          }

        //}:                                      |
        //brgr:{                                  |

          e_forceinline Vector4 brgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x12 );
                return r;
              #endif
            }
            c = Vector4( b, r, g, r );
            return c;
          }

        //}:                                      |
        //brgg:{                                  |

          e_forceinline Vector4 brgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x52 );
                return r;
              #endif
            }
            c = Vector4( b, r, g, g );
            return c;
          }

        //}:                                      |
        //brgb:{                                  |

          e_forceinline Vector4 brgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x92 );
                return r;
              #endif
            }
            c = Vector4( b, r, g, b );
            return c;
          }

        //}:                                      |
        //brga:{                                  |

          e_forceinline Vector4 brga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD2 );
                return r;
              #endif
            }
            c = Vector4( b, r, g, a );
            return c;
          }

        //}:                                      |
        //brbr:{                                  |

          e_forceinline Vector4 brbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x22 );
                return r;
              #endif
            }
            c = Vector4( b, r, b, r );
            return c;
          }

        //}:                                      |
        //brbg:{                                  |

          e_forceinline Vector4 brbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x62 );
                return r;
              #endif
            }
            c = Vector4( b, r, b, g );
            return c;
          }

        //}:                                      |
        //brbb:{                                  |

          e_forceinline Vector4 brbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA2 );
                return r;
              #endif
            }
            c = Vector4( b, r, b, b );
            return c;
          }

        //}:                                      |
        //brba:{                                  |

          e_forceinline Vector4 brba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE2 );
                return r;
              #endif
            }
            c = Vector4( b, r, b, a );
            return c;
          }

        //}:                                      |
        //brar:{                                  |

          e_forceinline Vector4 brar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x32 );
                return r;
              #endif
            }
            c = Vector4( b, r, a, r );
            return c;
          }

        //}:                                      |
        //brag:{                                  |

          e_forceinline Vector4 brag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x72 );
                return r;
              #endif
            }
            c = Vector4( b, r, a, g );
            return c;
          }

        //}:                                      |
        //brab:{                                  |

          e_forceinline Vector4 brab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB2 );
                return r;
              #endif
            }
            c = Vector4( b, r, a, b );
            return c;
          }

        //}:                                      |
        //braa:{                                  |

          e_forceinline Vector4 braa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF2 );
                return r;
              #endif
            }
            c = Vector4( b, r, a, a );
            return c;
          }

        //}:                                      |
        //bgrr:{                                  |

          e_forceinline Vector4 bgrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x06 );
                return r;
              #endif
            }
            c = Vector4( b, g, r, r );
            return c;
          }

        //}:                                      |
        //bgrg:{                                  |

          e_forceinline Vector4 bgrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x46 );
                return r;
              #endif
            }
            c = Vector4( b, g, r, g );
            return c;
          }

        //}:                                      |
        //bgrb:{                                  |

          e_forceinline Vector4 bgrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x86 );
                return r;
              #endif
            }
            c = Vector4( b, g, r, b );
            return c;
          }

        //}:                                      |
        //bgra:{                                  |

          e_forceinline Vector4 bgra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC6 );
                return r;
              #endif
            }
            c = Vector4( b, g, r, a );
            return c;
          }

        //}:                                      |
        //bggr:{                                  |

          e_forceinline Vector4 bggr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x16 );
                return r;
              #endif
            }
            c = Vector4( b, g, g, r );
            return c;
          }

        //}:                                      |
        //bggg:{                                  |

          e_forceinline Vector4 bggg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x56 );
                return r;
              #endif
            }
            c = Vector4( b, g, g, g );
            return c;
          }

        //}:                                      |
        //bggb:{                                  |

          e_forceinline Vector4 bggb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x96 );
                return r;
              #endif
            }
            c = Vector4( b, g, g, b );
            return c;
          }

        //}:                                      |
        //bgga:{                                  |

          e_forceinline Vector4 bgga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD6 );
                return r;
              #endif
            }
            c = Vector4( b, g, g, a );
            return c;
          }

        //}:                                      |
        //bgbr:{                                  |

          e_forceinline Vector4 bgbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x26 );
                return r;
              #endif
            }
            c = Vector4( b, g, b, r );
            return c;
          }

        //}:                                      |
        //bgbg:{                                  |

          e_forceinline Vector4 bgbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x66 );
                return r;
              #endif
            }
            c = Vector4( b, g, b, g );
            return c;
          }

        //}:                                      |
        //bgbb:{                                  |

          e_forceinline Vector4 bgbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA6 );
                return r;
              #endif
            }
            c = Vector4( b, g, b, b );
            return c;
          }

        //}:                                      |
        //bgba:{                                  |

          e_forceinline Vector4 bgba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE6 );
                return r;
              #endif
            }
            c = Vector4( b, g, b, a );
            return c;
          }

        //}:                                      |
        //bgar:{                                  |

          e_forceinline Vector4 bgar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x36 );
                return r;
              #endif
            }
            c = Vector4( b, g, a, r );
            return c;
          }

        //}:                                      |
        //bgag:{                                  |

          e_forceinline Vector4 bgag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x76 );
                return r;
              #endif
            }
            c = Vector4( b, g, a, g );
            return c;
          }

        //}:                                      |
        //bgab:{                                  |

          e_forceinline Vector4 bgab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB6 );
                return r;
              #endif
            }
            c = Vector4( b, g, a, b );
            return c;
          }

        //}:                                      |
        //bgaa:{                                  |

          e_forceinline Vector4 bgaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF6 );
                return r;
              #endif
            }
            c = Vector4( b, g, a, a );
            return c;
          }

        //}:                                      |
        //bbrr:{                                  |

          e_forceinline Vector4 bbrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0A );
                return r;
              #endif
            }
            c = Vector4( b, b, r, r );
            return c;
          }

        //}:                                      |
        //bbrg:{                                  |

          e_forceinline Vector4 bbrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4A );
                return r;
              #endif
            }
            c = Vector4( b, b, r, g );
            return c;
          }

        //}:                                      |
        //bbrb:{                                  |

          e_forceinline Vector4 bbrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8A );
                return r;
              #endif
            }
            c = Vector4( b, b, r, b );
            return c;
          }

        //}:                                      |
        //bbra:{                                  |

          e_forceinline Vector4 bbra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCA );
                return r;
              #endif
            }
            c = Vector4( b, b, r, a );
            return c;
          }

        //}:                                      |
        //bbgr:{                                  |

          e_forceinline Vector4 bbgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1A );
                return r;
              #endif
            }
            c = Vector4( b, b, g, r );
            return c;
          }

        //}:                                      |
        //bbgg:{                                  |

          e_forceinline Vector4 bbgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5A );
                return r;
              #endif
            }
            c = Vector4( b, b, g, g );
            return c;
          }

        //}:                                      |
        //bbgb:{                                  |

          e_forceinline Vector4 bbgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9A );
                return r;
              #endif
            }
            c = Vector4( b, b, g, b );
            return c;
          }

        //}:                                      |
        //bbga:{                                  |

          e_forceinline Vector4 bbga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDA );
                return r;
              #endif
            }
            c = Vector4( b, b, g, a );
            return c;
          }

        //}:                                      |
        //bbbr:{                                  |

          e_forceinline Vector4 bbbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2A );
                return r;
              #endif
            }
            c = Vector4( b, b, b, r );
            return c;
          }

        //}:                                      |
        //bbbg:{                                  |

          e_forceinline Vector4 bbbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6A );
                return r;
              #endif
            }
            c = Vector4( b, b, b, g );
            return c;
          }

        //}:                                      |
        //bbbb:{                                  |

          e_forceinline Vector4 bbbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAA );
                return r;
              #endif
            }
            c = Vector4( b, b, b, b );
            return c;
          }

        //}:                                      |
        //bbba:{                                  |

          e_forceinline Vector4 bbba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xEA );
                return r;
              #endif
            }
            c = Vector4( b, b, b, a );
            return c;
          }

        //}:                                      |
        //bbar:{                                  |

          e_forceinline Vector4 bbar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3A );
                return r;
              #endif
            }
            c = Vector4( b, b, a, r );
            return c;
          }

        //}:                                      |
        //bbag:{                                  |

          e_forceinline Vector4 bbag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7A );
                return r;
              #endif
            }
            c = Vector4( b, b, a, g );
            return c;
          }

        //}:                                      |
        //bbab:{                                  |

          e_forceinline Vector4 bbab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBA );
                return r;
              #endif
            }
            c = Vector4( b, b, a, b );
            return c;
          }

        //}:                                      |
        //bbaa:{                                  |

          e_forceinline Vector4 bbaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFA );
                return r;
              #endif
            }
            c = Vector4( b, b, a, a );
            return c;
          }

        //}:                                      |
        //barr:{                                  |

          e_forceinline Vector4 barr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0E );
                return r;
              #endif
            }
            c = Vector4( b, a, r, r );
            return c;
          }

        //}:                                      |
        //barg:{                                  |

          e_forceinline Vector4 barg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4E );
                return r;
              #endif
            }
            c = Vector4( b, a, r, g );
            return c;
          }

        //}:                                      |
        //barb:{                                  |

          e_forceinline Vector4 barb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8E );
                return r;
              #endif
            }
            c = Vector4( b, a, r, b );
            return c;
          }

        //}:                                      |
        //bara:{                                  |

          e_forceinline Vector4 bara()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCE );
                return r;
              #endif
            }
            c = Vector4( b, a, r, a );
            return c;
          }

        //}:                                      |
        //bagr:{                                  |

          e_forceinline Vector4 bagr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1E );
                return r;
              #endif
            }
            c = Vector4( b, a, g, r );
            return c;
          }

        //}:                                      |
        //bagg:{                                  |

          e_forceinline Vector4 bagg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5E );
                return r;
              #endif
            }
            c = Vector4( b, a, g, g );
            return c;
          }

        //}:                                      |
        //bagb:{                                  |

          e_forceinline Vector4 bagb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9E );
                return r;
              #endif
            }
            c = Vector4( b, a, g, b );
            return c;
          }

        //}:                                      |
        //baga:{                                  |

          e_forceinline Vector4 baga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDE );
                return r;
              #endif
            }
            c = Vector4( b, a, g, a );
            return c;
          }

        //}:                                      |
        //babr:{                                  |

          e_forceinline Vector4 babr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2E );
                return r;
              #endif
            }
            c = Vector4( b, a, b, r );
            return c;
          }

        //}:                                      |
        //babg:{                                  |

          e_forceinline Vector4 babg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6E );
                return r;
              #endif
            }
            c = Vector4( b, a, b, g );
            return c;
          }

        //}:                                      |
        //babb:{                                  |

          e_forceinline Vector4 babb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAE );
                return r;
              #endif
            }
            c = Vector4( b, a, b, b );
            return c;
          }

        //}:                                      |
        //baba:{                                  |

          e_forceinline Vector4 baba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xEE );
                return r;
              #endif
            }
            c = Vector4( b, a, b, a );
            return c;
          }

        //}:                                      |
        //baar:{                                  |

          e_forceinline Vector4 baar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3E );
                return r;
              #endif
            }
            c = Vector4( b, a, a, r );
            return c;
          }

        //}:                                      |
        //baag:{                                  |

          e_forceinline Vector4 baag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7E );
                return r;
              #endif
            }
            c = Vector4( b, a, a, g );
            return c;
          }

        //}:                                      |
        //baab:{                                  |

          e_forceinline Vector4 baab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBE );
                return r;
              #endif
            }
            c = Vector4( b, a, a, b );
            return c;
          }

        //}:                                      |
        //baaa:{                                  |

          e_forceinline Vector4 baaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFE );
                return r;
              #endif
            }
            c = Vector4( b, a, a, a );
            return c;
          }

        //}:                                      |
        //arrr:{                                  |

          e_forceinline Vector4 arrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x03 );
                return r;
              #endif
            }
            c = Vector4( a, r, r, r );
            return c;
          }

        //}:                                      |
        //arrg:{                                  |

          e_forceinline Vector4 arrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x43 );
                return r;
              #endif
            }
            c = Vector4( a, r, r, g );
            return c;
          }

        //}:                                      |
        //arrb:{                                  |

          e_forceinline Vector4 arrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x83 );
                return r;
              #endif
            }
            c = Vector4( a, r, r, b );
            return c;
          }

        //}:                                      |
        //arra:{                                  |

          e_forceinline Vector4 arra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC3 );
                return r;
              #endif
            }
            c = Vector4( a, r, r, a );
            return c;
          }

        //}:                                      |
        //argr:{                                  |

          e_forceinline Vector4 argr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x13 );
                return r;
              #endif
            }
            c = Vector4( a, r, g, r );
            return c;
          }

        //}:                                      |
        //argg:{                                  |

          e_forceinline Vector4 argg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x53 );
                return r;
              #endif
            }
            c = Vector4( a, r, g, g );
            return c;
          }

        //}:                                      |
        //argb:{                                  |

          e_forceinline Vector4 argb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x93 );
                return r;
              #endif
            }
            c = Vector4( a, r, g, b );
            return c;
          }

        //}:                                      |
        //arga:{                                  |

          e_forceinline Vector4 arga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD3 );
                return r;
              #endif
            }
            c = Vector4( a, r, g, a );
            return c;
          }

        //}:                                      |
        //arbr:{                                  |

          e_forceinline Vector4 arbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x23 );
                return r;
              #endif
            }
            c = Vector4( a, r, b, r );
            return c;
          }

        //}:                                      |
        //arbg:{                                  |

          e_forceinline Vector4 arbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x63 );
                return r;
              #endif
            }
            c = Vector4( a, r, b, g );
            return c;
          }

        //}:                                      |
        //arbb:{                                  |

          e_forceinline Vector4 arbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA3 );
                return r;
              #endif
            }
            c = Vector4( a, r, b, b );
            return c;
          }

        //}:                                      |
        //arba:{                                  |

          e_forceinline Vector4 arba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE3 );
                return r;
              #endif
            }
            c = Vector4( a, r, b, a );
            return c;
          }

        //}:                                      |
        //arar:{                                  |

          e_forceinline Vector4 arar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x33 );
                return r;
              #endif
            }
            c = Vector4( a, r, a, r );
            return c;
          }

        //}:                                      |
        //arag:{                                  |

          e_forceinline Vector4 arag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x73 );
                return r;
              #endif
            }
            c = Vector4( a, r, a, g );
            return c;
          }

        //}:                                      |
        //arab:{                                  |

          e_forceinline Vector4 arab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB3 );
                return r;
              #endif
            }
            c = Vector4( a, r, a, b );
            return c;
          }

        //}:                                      |
        //araa:{                                  |

          e_forceinline Vector4 araa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF3 );
                return r;
              #endif
            }
            c = Vector4( a, r, a, a );
            return c;
          }

        //}:                                      |
        //agrr:{                                  |

          e_forceinline Vector4 agrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x07 );
                return r;
              #endif
            }
            c = Vector4( a, g, r, r );
            return c;
          }

        //}:                                      |
        //agrg:{                                  |

          e_forceinline Vector4 agrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x47 );
                return r;
              #endif
            }
            c = Vector4( a, g, r, g );
            return c;
          }

        //}:                                      |
        //agrb:{                                  |

          e_forceinline Vector4 agrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x87 );
                return r;
              #endif
            }
            c = Vector4( a, g, r, b );
            return c;
          }

        //}:                                      |
        //agra:{                                  |

          e_forceinline Vector4 agra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xC7 );
                return r;
              #endif
            }
            c = Vector4( a, g, r, a );
            return c;
          }

        //}:                                      |
        //aggr:{                                  |

          e_forceinline Vector4 aggr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x17 );
                return r;
              #endif
            }
            c = Vector4( a, g, g, r );
            return c;
          }

        //}:                                      |
        //aggg:{                                  |

          e_forceinline Vector4 aggg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x57 );
                return r;
              #endif
            }
            c = Vector4( a, g, g, g );
            return c;
          }

        //}:                                      |
        //aggb:{                                  |

          e_forceinline Vector4 aggb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x97 );
                return r;
              #endif
            }
            c = Vector4( a, g, g, b );
            return c;
          }

        //}:                                      |
        //agga:{                                  |

          e_forceinline Vector4 agga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xD7 );
                return r;
              #endif
            }
            c = Vector4( a, g, g, a );
            return c;
          }

        //}:                                      |
        //agbr:{                                  |

          e_forceinline Vector4 agbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x27 );
                return r;
              #endif
            }
            c = Vector4( a, g, b, r );
            return c;
          }

        //}:                                      |
        //agbg:{                                  |

          e_forceinline Vector4 agbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x67 );
                return r;
              #endif
            }
            c = Vector4( a, g, b, g );
            return c;
          }

        //}:                                      |
        //agbb:{                                  |

          e_forceinline Vector4 agbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xA7 );
                return r;
              #endif
            }
            c = Vector4( a, g, b, b );
            return c;
          }

        //}:                                      |
        //agba:{                                  |

          e_forceinline Vector4 agba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xE7 );
                return r;
              #endif
            }
            c = Vector4( a, g, b, a );
            return c;
          }

        //}:                                      |
        //agar:{                                  |

          e_forceinline Vector4 agar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x37 );
                return r;
              #endif
            }
            c = Vector4( a, g, a, r );
            return c;
          }

        //}:                                      |
        //agag:{                                  |

          e_forceinline Vector4 agag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x77 );
                return r;
              #endif
            }
            c = Vector4( a, g, a, g );
            return c;
          }

        //}:                                      |
        //agab:{                                  |

          e_forceinline Vector4 agab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xB7 );
                return r;
              #endif
            }
            c = Vector4( a, g, a, b );
            return c;
          }

        //}:                                      |
        //agaa:{                                  |

          e_forceinline Vector4 agaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xF7 );
                return r;
              #endif
            }
            c = Vector4( a, g, a, a );
            return c;
          }

        //}:                                      |
        //abrr:{                                  |

          e_forceinline Vector4 abrr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0B );
                return r;
              #endif
            }
            c = Vector4( a, b, r, r );
            return c;
          }

        //}:                                      |
        //abrg:{                                  |

          e_forceinline Vector4 abrg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4B );
                return r;
              #endif
            }
            c = Vector4( a, b, r, g );
            return c;
          }

        //}:                                      |
        //abrb:{                                  |

          e_forceinline Vector4 abrb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8B );
                return r;
              #endif
            }
            c = Vector4( a, b, r, b );
            return c;
          }

        //}:                                      |
        //abra:{                                  |

          e_forceinline Vector4 abra()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCB );
                return r;
              #endif
            }
            c = Vector4( a, b, r, a );
            return c;
          }

        //}:                                      |
        //abgr:{                                  |

          e_forceinline Vector4 abgr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1B );
                return r;
              #endif
            }
            c = Vector4( a, b, g, r );
            return c;
          }

        //}:                                      |
        //abgg:{                                  |

          e_forceinline Vector4 abgg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5B );
                return r;
              #endif
            }
            c = Vector4( a, b, g, g );
            return c;
          }

        //}:                                      |
        //abgb:{                                  |

          e_forceinline Vector4 abgb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9B );
                return r;
              #endif
            }
            c = Vector4( a, b, g, b );
            return c;
          }

        //}:                                      |
        //abga:{                                  |

          e_forceinline Vector4 abga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDB );
                return r;
              #endif
            }
            c = Vector4( a, b, g, a );
            return c;
          }

        //}:                                      |
        //abbr:{                                  |

          e_forceinline Vector4 abbr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2B );
                return r;
              #endif
            }
            c = Vector4( a, b, b, r );
            return c;
          }

        //}:                                      |
        //abbg:{                                  |

          e_forceinline Vector4 abbg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6B );
                return r;
              #endif
            }
            c = Vector4( a, b, b, g );
            return c;
          }

        //}:                                      |
        //abbb:{                                  |

          e_forceinline Vector4 abbb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAB );
                return r;
              #endif
            }
            c = Vector4( a, b, b, b );
            return c;
          }

        //}:                                      |
        //abba:{                                  |

          e_forceinline Vector4 abba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xEB );
                return r;
              #endif
            }
            c = Vector4( a, b, b, a );
            return c;
          }

        //}:                                      |
        //abar:{                                  |

          e_forceinline Vector4 abar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3B );
                return r;
              #endif
            }
            c = Vector4( a, b, a, r );
            return c;
          }

        //}:                                      |
        //abag:{                                  |

          e_forceinline Vector4 abag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7B );
                return r;
              #endif
            }
            c = Vector4( a, b, a, g );
            return c;
          }

        //}:                                      |
        //abab:{                                  |

          e_forceinline Vector4 abab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBB );
                return r;
              #endif
            }
            c = Vector4( a, b, a, b );
            return c;
          }

        //}:                                      |
        //abaa:{                                  |

          e_forceinline Vector4 abaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFB );
                return r;
              #endif
            }
            c = Vector4( a, b, a, a );
            return c;
          }

        //}:                                      |
        //aarr:{                                  |

          e_forceinline Vector4 aarr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x0F );
                return r;
              #endif
            }
            c = Vector4( a, a, r, r );
            return c;
          }

        //}:                                      |
        //aarg:{                                  |

          e_forceinline Vector4 aarg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x4F );
                return r;
              #endif
            }
            c = Vector4( a, a, r, g );
            return c;
          }

        //}:                                      |
        //aarb:{                                  |

          e_forceinline Vector4 aarb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x8F );
                return r;
              #endif
            }
            c = Vector4( a, a, r, b );
            return c;
          }

        //}:                                      |
        //aara:{                                  |

          e_forceinline Vector4 aara()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xCF );
                return r;
              #endif
            }
            c = Vector4( a, a, r, a );
            return c;
          }

        //}:                                      |
        //aagr:{                                  |

          e_forceinline Vector4 aagr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x1F );
                return r;
              #endif
            }
            c = Vector4( a, a, g, r );
            return c;
          }

        //}:                                      |
        //aagg:{                                  |

          e_forceinline Vector4 aagg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x5F );
                return r;
              #endif
            }
            c = Vector4( a, a, g, g );
            return c;
          }

        //}:                                      |
        //aagb:{                                  |

          e_forceinline Vector4 aagb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x9F );
                return r;
              #endif
            }
            c = Vector4( a, a, g, b );
            return c;
          }

        //}:                                      |
        //aaga:{                                  |

          e_forceinline Vector4 aaga()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xDF );
                return r;
              #endif
            }
            c = Vector4( a, a, g, a );
            return c;
          }

        //}:                                      |
        //aabr:{                                  |

          e_forceinline Vector4 aabr()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x2F );
                return r;
              #endif
            }
            c = Vector4( a, a, b, r );
            return c;
          }

        //}:                                      |
        //aabg:{                                  |

          e_forceinline Vector4 aabg()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x6F );
                return r;
              #endif
            }
            c = Vector4( a, a, b, g );
            return c;
          }

        //}:                                      |
        //aabb:{                                  |

          e_forceinline Vector4 aabb()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xAF );
                return r;
              #endif
            }
            c = Vector4( a, a, b, b );
            return c;
          }

        //}:                                      |
        //aaba:{                                  |

          e_forceinline Vector4 aaba()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xEF );
                return r;
              #endif
            }
            c = Vector4( a, a, b, a );
            return c;
          }

        //}:                                      |
        //aaar:{                                  |

          e_forceinline Vector4 aaar()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x3F );
                return r;
              #endif
            }
            c = Vector4( a, a, a, r );
            return c;
          }

        //}:                                      |
        //aaag:{                                  |

          e_forceinline Vector4 aaag()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0x7F );
                return r;
              #endif
            }
            c = Vector4( a, a, a, g );
            return c;
          }

        //}:                                      |
        //aaab:{                                  |

          e_forceinline Vector4 aaab()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xBF );
                return r;
              #endif
            }
            c = Vector4( a, a, a, b );
            return c;
          }

        //}:                                      |
        //aaaa:{                                  |

          e_forceinline Vector4 aaaa()const{
            Vector4 c;
            if( is_sse() ){
              #ifdef __SSE__
                c.i = _mm_shuffle_epi32( i, 0xFF );
                return r;
              #endif
            }
            c = Vector4( a, a, a, a );
            return c;
          }

        //}:                                      |
        //rrr:{                                   |

          e_forceinline Vector3 rrr()const{
            return Vector3( r, r, r );
          }

        //}:                                      |
        //rrg:{                                   |

          e_forceinline Vector3 rrg()const{
            return Vector3( r, r, g );
          }

        //}:                                      |
        //rrb:{                                   |

          e_forceinline Vector3 rrb()const{
            return Vector3( r, r, b );
          }

        //}:                                      |
        //rgr:{                                   |

          e_forceinline Vector3 rgr()const{
            return Vector3( r, g, r );
          }

        //}:                                      |
        //rgg:{                                   |

          e_forceinline Vector3 rgg()const{
            return Vector3( r, g, g );
          }

        //}:                                      |
        //rgb:{                                   |

          e_forceinline Vector3 rgb()const{
            return Vector3( r, g, b );
          }

        //}:                                      |
        //rbr:{                                   |

          e_forceinline Vector3 rbr()const{
            return Vector3( r, b, r );
          }

        //}:                                      |
        //rbg:{                                   |

          e_forceinline Vector3 rbg()const{
            return Vector3( r, b, g );
          }

        //}:                                      |
        //rbb:{                                   |

          e_forceinline Vector3 rbb()const{
            return Vector3( r, b, b );
          }

        //}:                                      |
        //grr:{                                   |

          e_forceinline Vector3 grr()const{
            return Vector3( g, r, r );
          }

        //}:                                      |
        //grg:{                                   |

          e_forceinline Vector3 grg()const{
            return Vector3( g, r, g );
          }

        //}:                                      |
        //grb:{                                   |

          e_forceinline Vector3 grb()const{
            return Vector3( g, r, b );
          }

        //}:                                      |
        //ggr:{                                   |

          e_forceinline Vector3 ggr()const{
            return Vector3( g, g, r );
          }

        //}:                                      |
        //ggg:{                                   |

          e_forceinline Vector3 ggg()const{
            return Vector3( g, g, g );
          }

        //}:                                      |
        //ggb:{                                   |

          e_forceinline Vector3 ggb()const{
            return Vector3( g, g, b );
          }

        //}:                                      |
        //gbr:{                                   |

          e_forceinline Vector3 gbr()const{
            return Vector3( g, b, r );
          }

        //}:                                      |
        //gbg:{                                   |

          e_forceinline Vector3 gbg()const{
            return Vector3( g, b, g );
          }

        //}:                                      |
        //gbb:{                                   |

          e_forceinline Vector3 gbb()const{
            return Vector3( g, b, b );
          }

        //}:                                      |
        //brr:{                                   |

          e_forceinline Vector3 brr()const{
            return Vector3( b, r, r );
          }

        //}:                                      |
        //brg:{                                   |

          e_forceinline Vector3 brg()const{
            return Vector3( b, r, g );
          }

        //}:                                      |
        //brb:{                                   |

          e_forceinline Vector3 brb()const{
            return Vector3( b, r, b );
          }

        //}:                                      |
        //bgr:{                                   |

          e_forceinline Vector3 bgr()const{
            return Vector3( b, g, r );
          }

        //}:                                      |
        //bgg:{                                   |

          e_forceinline Vector3 bgg()const{
            return Vector3( b, g, g );
          }

        //}:                                      |
        //bgb:{                                   |

          e_forceinline Vector3 bgb()const{
            return Vector3( b, g, b );
          }

        //}:                                      |
        //bbr:{                                   |

          e_forceinline Vector3 bbr()const{
            return Vector3( b, b, r );
          }

        //}:                                      |
        //bbg:{                                   |

          e_forceinline Vector3 bbg()const{
            return Vector3( b, b, g );
          }

        //}:                                      |
        //bbb:{                                   |

          e_forceinline Vector3 bbb()const{
            return Vector3( b, b, b );
          }

        //}:                                      |
        //rr:{                                    |

          e_forceinline Vector2 rr()const{
            return Vector2( r, r );
          }

        //}:                                      |
        //rg:{                                    |

          e_forceinline Vector2 rg()const{
            return Vector2( r, g );
          }

        //}:                                      |
        //rb:{                                    |

          e_forceinline Vector2 rb()const{
            return Vector2( r, b );
          }

        //}:                                      |
        //ra:{                                    |

          e_forceinline Vector2 ra()const{
            return Vector2( r, a );
          }

        //}:                                      |
        //gr:{                                    |

          e_forceinline Vector2 gr()const{
            return Vector2( g, r );
          }

        //}:                                      |
        //gg:{                                    |

          e_forceinline Vector2 gg()const{
            return Vector2( g, g );
          }

        //}:                                      |
        //gb:{                                    |

          e_forceinline Vector2 gb()const{
            return Vector2( g, b );
          }

        //}:                                      |
        //ga:{                                    |

          e_forceinline Vector2 ga()const{
            return Vector2( g, a );
          }

        //}:                                      |
        //br:{                                    |

          e_forceinline Vector2 br()const{
            return Vector2( b, r );
          }

        //}:                                      |
        //bg:{                                    |

          e_forceinline Vector2 bg()const{
            return Vector2( b, g );
          }

        //}:                                      |
        //bb:{                                    |

          e_forceinline Vector2 bb()const{
            return Vector2( b, b );
          }

        //}:                                      |
        //ba:{                                    |

          e_forceinline Vector2 ba()const{
            return Vector2( b, a );
          }

        //}:                                      |
        //ar:{                                    |

          e_forceinline Vector2 ar()const{
            return Vector2( a, r );
          }

        //}:                                      |
        //ag:{                                    |

          e_forceinline Vector2 ag()const{
            return Vector2( a, g );
          }

        //}:                                      |
        //ab:{                                    |

          e_forceinline Vector2 ab()const{
            return Vector2( a, b );
          }

        //}:                                      |
        //aa:{                                    |

          e_forceinline Vector2 aa()const{
            return Vector2( a, a );
          }

        //}:                                      |
      //}:                                        |
      //setIdentity:{                             |

        e_forceinline void setIdentity(){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_set1_ps( 0 );
              w = 1;
              return;
            #endif
          }
          x = y = z = 0;
          w = 1;
        }

      //}:                                        |
      //normalize*:{                              |

        e_forceinline friend Vector4 e_normalize( const Vector4& v ){
          return v.normalized();
        }

        e_forceinline Vector4 normalized()const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE4_1__
              #if 1 // fast
                const __m128 inverse_norm = _mm_rsqrt_ps(_mm_dp_ps( v, v, 0x77 ));
                r.v = _mm_mul_ps( v, inverse_norm );
              #else // accurate but slow.
                const __m128 norm = _mm_sqrt_ps(_mm_dp_ps( v,  v, 0x7F ));
                r.v = _mm_div_ps( v, norm );
              #endif
              return r;
            #endif
          }
          const self& l = length();
          const self& f = (k1 / l);
          r.x = x*f;
          r.y = y*f;
          r.z = z*f;
          r.w = w*f;
          return r;
        }

        e_forceinline void normalize(){
          if( is_sse() ){
            #ifdef __SSE4_1__
              #if 1 // fast
                const __m128 inverse_norm = _mm_rsqrt_ps(_mm_dp_ps( v, v, 0x77 ));
                v = _mm_mul_ps( v, inverse_norm );
              #else // accurate
                const __m128 norm = _mm_sqrt_ps(_mm_dp_ps( v,  v, 0x7F ));
                v = _mm_div_ps( v, norm );
              #endif
                return;
            #endif
          }
          const self& l = length();
          const self& f = (k1 / l);
          x *= f;
          y *= f;
          z *= f;
          w *= f;
        }

      //}:                                        |
      //distance*:{                               |

        e_forceinline friend self e_distance_squared( const Vector4& v1, const Vector4& v2 ){
          return v1.distanceSquared( v2 );
        }

        e_forceinline friend self e_distance( const Vector4& v1, const Vector4& v2 ){
          return v1.distance( v2 );
        }

        e_forceinline self distanceSquared( const Vector4& V )const{
          if( is_sse() ){
            #ifdef __SSE4_1__
              const __m128 d = _mm_sub( V.v, v );
              return _mm_cvtss_f32(_mm_dp_ps( d, d, 0x71 ));
            #endif
          }
          return Vector4( V.x-x, V.y-y, V.z-z, V.w-w ).lengthSquared();
        }

        e_forceinline self distance( const Vector4& V )const{
          if( is_sse() ){
            #ifdef __SSE4_1__
              const __m128 d = _mm_sub( V.v, v );
              return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps( d, d, 0x71 )));
            #endif
          }
          return Vector4( V.x-x, V.y-y, V.z-z, V.w-w ).length();
        }

      //}:                                        |
      //length*:{                                 |

        e_forceinline friend self e_length_squared( const Vector4& v ){
          return v.lengthSquared();
        }

        e_forceinline friend self e_length( const Vector4& v ){
          return v.length();
        }

        e_forceinline self lengthSquared()const{
          if( is_sse() ){
            #if defined __SSE4_1__
              return _mm_cvtss_f32(_mm_dp_ps( v, v, 0x71 ));
            #endif
          }
          return x*x+y*y+z*z+w*w;
        }

        e_forceinline self length()const{
          if( is_sse() ){
            #ifdef __SSE4_1__
              return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps( v, v, 0x71 )));
            #endif
          }
          return sqrt( lengthSquared() );
        }

      //}:                                        |
      //negate*:{                                 |

        e_forceinline friend Vector4 e_negate( const Vector4& v ){
          return v.negated();
        }

        e_forceinline Vector4 negated()const{
          return -*this;
        }

        e_forceinline Vector4& negate(){
          if( is_sse() ){
            #ifdef __SSE4_1__
              static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32( 0x80000000 ));
              v = _mm_xor_ps( v, SIGNMASK );
            #endif
          }else{
            x = -x;
            y = -y;
            z = -z;
            w = -w;
          }
          return *this;
        }

      //}:                                        |
      //isValid:{                                 |

        e_forceinline friend bool e_valid( const Vector4& v ){
          return v.valid();
        }

        e_forceinline bool valid()const{
          return( x.valid() && y.valid() && z.valid() && w.valid() );
        }

      //}:                                        |
      //empty:{                                   |

        e_forceinline friend bool e_empty( const Vector4& v ){
          return v.empty();
        }

        e_forceinline bool empty()const{
          if( is_sse() ){
            #ifdef __SSE__
              const __m128 m = _mm_set1_ps( 0 );
              return bool( v == m );
            #endif
          }
          if( x != 0 ){
            return false;
          }
          if( y != 0 ){
            return false;
          }
          if( z != 0 ){
            return false;
          }
          if( w != 0 ){
            return false;
          }
          return true;
        }

      //}:                                        |
      //lerp*:{                                   |

        e_forceinline friend Vector4 e_lerp( const Vector4& v1, const Vector4& v2, const self& t ){
          return v1.lerped( v2, t );
        }

        e_forceinline Vector4& lerp( const Vector4& V, const self& t ){
          e_assert(( t >= 0 ) && ( t <= 1.f ));
          if( is_sse() ){
            #ifdef __SSE__
              const __m128 m = _mm_mul_ps( V.v,_mm_set1_ps( float( t )));
              const __m128 b = _mm_sub_ps( V.v, v );
              v = _mm_mul_ps( m, b );
              return *this;
            #endif
          }
          x =( V.x+t )*( V.x-x );
          y =( V.y+t )*( V.y-y );
          z =( V.z+t )*( V.z-z );
          w =( V.w+t )*( V.w-w );
          return *this;
        }

        e_forceinline Vector4 lerped( const Vector4& V, const self& t ){
          Vector4 r = *this;
          r.lerp( V, t );
          return r;
        }

      //}:                                        |
      //cross:{                                   |

        e_forceinline friend Vector4 e_cross( const Vector4& v1, const Vector4& v2 ){
          return v1.cross( v2 );
        }

        e_forceinline Vector4 cross( const Vector4& V )const{
          if( is_sse() ){
            #ifdef __SSE__
              Vector4 r;
              r.v = _mm_sub_ps(
                    _mm_mul_ps(_mm_shuffle_ps( v, v,_MM_SHUFFLE( 3, 0, 2, 1 )),_mm_shuffle_ps( V.v, V.v,_MM_SHUFFLE( 3, 1, 0, 2 ))),
                    _mm_mul_ps(_mm_shuffle_ps( v, v,_MM_SHUFFLE( 3, 1, 0, 2 )),_mm_shuffle_ps( V.v, V.v,_MM_SHUFFLE( 3, 0, 2, 1 ))));
              return r;
            #endif
          }
          return Vector4( y*V.z-z*V.y, z*V.x-x*V.z, x*V.y-y*V.x, 1.f );
        }

      //}:                                        |
      //clear:{                                   |

        e_forceinline void clear(){
          if( is_sse() ){
            #ifdef __SSE__
              v = _mm_set1_ps( 0 );
            #endif
          }else{
            x = y = z = w = 0;
          }
        }

      //}:                                        |
      //sum:{                                     |

        e_forceinline friend self e_sum( const Vector4& v ){
          return v.sum();
        }

        e_forceinline self sum()const{
          self fSum = k0;
          for( u32 i=0; i<4; ++i ){
            fSum += m[ i ];
          }
          return fSum;
        }

      //}:                                        |
      //abs:{                                     |

        e_forceinline friend Vector4 e_abs( const Vector4& v ){
          return v.abs();
        }

        e_forceinline Vector4 abs()const{
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32( 0x80000000 ));
              r.v = _mm_andnot_ps( SIGNMASK, v );
            #endif
          }else{
            r.x = -x;
            r.y = -y;
            r.z = -z;
            r.w = -w;
          }
          return r;
        }

      //}:                                        |
      //set:{                                     |

        e_forceinline void set( const self& X, const self& Y, const self& Z, const self& W=1 ){
          x = X;
          y = Y;
          z = Z;
          w = W;
        }

        e_forceinline void set( const Vector2& N, const self& Z, const self& W=1 ){
          x = N.x;
          y = N.y;
          z = Z;
          w = W;
        }

        e_forceinline void set( const Vector3& N, const self& W=k1 ){
          x = N.x;
          y = N.y;
          z = N.z;
          w = W;
        }

      //}:                                        |
      //dot:{                                     |

        e_forceinline friend Vector4 e_dot( const Vector4& v1, const Vector4& v2 ){
          return v1.dot( v2 );
        }

        e_forceinline self dot( const Vector4& V )const{
          if( is_sse() ){
            self t;
            #ifdef __SSE4_1__
              __m128 r = _mm_dp_ps( v, V.v, 0x71 );
              _mm_store_ss( &t, r );
            #elif defined __SSE__
              __m128 r1 = _mm_mul_ps( v, V.v );
              __m128 r2 = _mm_hadd_ps( r1, r1 );
              __m128 r3 = _mm_hadd_ps( r2, r2 );
              _mm_store_ss( &t, r3 );
            #endif
            return t;
          }
          return x*V.x + y*V.y + z*V.z + w*V.w;
        }

      //}:                                        |
      //max:{                                     |

        e_forceinline friend Vector4 e_maxf( const Vector4& v1, const Vector4& v2 ){
          return v1.max( v2 );
        }

        e_forceinline Vector4 max( const Vector4& V ){
          Vector4 r;
          if( is_sse() ){
            #ifdef __SSE__
              r.v = _mm_max_ps( v, V.v );
              return r;
            #endif
          }
          r.x = max( r.x, V.x );
          r.y = max( r.y, V.y );
          r.z = max( r.z, V.z );
          r.w = max( r.w, V.w );
          return r;
        }

      //}:                                        |
      //min:{                                     |

        e_forceinline friend Vector4 e_minf( const Vector4& v1, const Vector4& v2 ){
          return v1.min( v2 );
        }

        e_forceinline Vector4 min( const Vector4& V ){
          Vector4 r;
          if( is_sse() ){
            #ifdef  __SSE__
              r.v = _mm_min_ps( v, V.v );
              return r;
            #endif
          }
          r.x = min( r.x, V.x );
          r.y = min( r.y, V.y );
          r.z = min( r.z, V.z );
          r.w = min( r.w, V.w );
          return r;
        }

      //}:                                        |
    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline_always Vector4( const std::initializer_list<T>& ilist ){
      e_assert( ilist.size() <= 4 );
      u32 i = 0;
      for( auto& v : ilist ){
        m[ i++ ]=v;
      }
    }

    /** \name Component constructors as const expressions.
      *
      * These constructors will build m vector component by component via const
      * expressions so the vector will be built at compile time.
      *
      * @{
      */

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param in_x The in_x of the vector.
      * \param in_y The in_y.
      * \param in_z The in_z.
      * \param in_w The in_w defaulting to 1.
      */

    e_forceinline_always Vector4( const float in_x, const float in_y, const float in_z, const float in_w )
      : x( in_x )
      , y( in_y )
      , z( in_z )
      , w( in_w )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param in_x The in_x of the vector.
      * \param in_y The in_y.
      * \param in_z The in_z.
      */

    e_forceinline_always Vector4( const float in_x, const float in_y, const float in_z )
      : x( in_x )
      , y( in_y )
      , z( in_z )
      , w( 1 )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param in_x The in_x of the vector.
      * \param in_y The in_y.
      */

    e_forceinline_always Vector4( const float in_x, const float in_y )
      : x( in_x )
      , y( in_y )
      , z( 0 )
      , w( 1 )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with m single scalar set in all
      * four components.
      *
      * \param in_x The scalar to set X, Y, Z and W to.
      */

    e_forceinline_always Vector4( const float in_x )
      : x( in_x )
      , y( in_x )
      , z( in_x )
      , w( in_x )
    {}

    /** @}
      *
      * \name Component constructors as templates.
      *
      * These constructors will build m vector component by component. All
      * components must be the same type but they can be any type. Only the
      * floating point path above is guaranteed to be done at compile time.
      *
      * @{
      */

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      * \param Z The Z.
      * \param W The W defaulting to 1.
      */

    e_forceinline_always Vector4( const self& X, const self& Y, const self& Z, const self& W )
      : x( X )
      , y( Y )
      , z( Z )
      , w( W )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      * \param Z The Z.
      */

    e_forceinline_always Vector4( const self& X, const self& Y, const self& Z )
      : x( X )
      , y( Y )
      , z( Z )
      , w( 1 )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector4 is vec4.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      */

    e_forceinline_always Vector4( const self& X, const self& Y )
      : x( X )
      , y( Y )
      , z( 0 )
      , w( 1 )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with m single scalar set in all
      * four components.
      *
      * \param s The scalar to set x, y, z and w to.
      */

    e_forceinline_always Vector4( const self& s )
      : x( s )
      , y( s )
      , z( s )
      , w( s )
    {}

    /** @}
      *
      * \name A mixture of vector and real constructors.
      *
      * These constructors are great for building m vector out of outher
      * vectors and templated reals.
      *
      * @{
      */

    /** \brief Vector from point constructor.
      *
      * This routine will construct m new four tuple vector from m three tuple
      * point. It is m convenience helper to match the assignment operator.
      *
      * \param P A 2D point containing the XY position.
      * \param Z The Z axis in homogeneous space.
      * \param W The W axis in homogeneous space.
      */

    e_forceinline_always Vector4( const Point2& P, const self& Z, const self& W )
      : x( P.x )
      , y( P.y )
      , z( Z )
      , w( W )
    {}

    /** \brief Vector constructor.
      *
      * This constructor builds m homogeneous vector from m 3D vector and m W
      * component.
      *
      * \param V The 3D vector containing XY and Z.
      * \param W The W component.
      */

    e_forceinline_always Vector4( const Vector3& V, const self& W )
      : x( V.x )
      , y( V.y )
      , z( V.z )
      , w( W )
    {}

    /** \brief Vector from point constructor.
      *
      * This routine will construct m new four tuple vector from m three tuple
      * point. It is m convenience helper to match the assignment operator.
      *
      * \param P A 2D point containing the XY position.
      * \param W The W axis in homogeneous space.
      */

    e_forceinline_always Vector4( const Point3& P, const self& W )
      : x( P.x )
      , y( P.y )
      , z( P.z )
      , w( W )
    {}

    /** \brief Vector constructor.
      *
      * This constructor will build m homogeneous vector from two tuple
      * vectors.
      *
      * \param m The 1st vector.
      * \param b The 2nd vector.
      */

    e_forceinline_always Vector4( const Vector2& m, const Vector2& b )
      : x( m.x )
      , y( m.y )
      , z( b.x )
      , w( b.y )
    {}

    /** \brief Vector copy constructor.
      *
      * This constructor builds m new vector from the one provided.
      *
      * \param v The vector to copy into this one.
      */

    e_forceinline_always Vector4( const Vector4& v )
      : x( v.x )
      , y( v.y )
      , z( v.z )
      , w( v.w )
    {}

    /** @} */

    #ifdef __SSE__
      e_forceinline_always Vector4( const __m128i I ){
        static_assert( is_sse(), "Not available!" );
        i = I;
      }
      e_forceinline_always Vector4( const __m128 V ){
        static_assert( is_sse(), "Not available!" );
        v = V;
      }
    #endif

    /** \brief Default vector constructor.
      *
      * The default constructor will initialize the homogenious vector to its
      * identity: [0 0 0 1].
      */

    e_forceinline_always Vector4()
      : x( 0 )
      , y( 0 )
      , z( 0 )
      , w( 1 )
    {}
  };

//}:                                              |
//================================================+=============================