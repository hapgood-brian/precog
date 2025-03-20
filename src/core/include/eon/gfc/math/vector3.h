//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

#include"point3.h"

//================================================+=============================
//Vector3:{                                       |

  /** \brief Three tuple vector.
    *
    * This class is used throughout the engine to represent vectors. It's never
    * used to express points, that's the job of Point3. The shorthand for a
    * f32::Vector3 is vec3.
    */

  struct E_PUBLISH Vector3{

    //--------------------------------------------+-----------------------------
    //Statics:{                                   |

      static const u32     kTuples = 3;
      static const Vector3 kForward;
      static const Vector3 kRight;
      static const Vector3 kUp;
      static const Vector3 kZero;

    //}:                                          |
    //Structs:{                                   |

      union{
        struct{
          self x, y, z;
        };
        struct{
          self u, v, w;
        };
        self m[ 3 ];
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
              [[fallthrough]];
            default:
              return z;
          }
        }

        e_noinline self& operator[]( const s32 i ){
          switch( i ){
            case 0:
              return x;
            case 1:
              return y;
            case 2:
              [[fallthrough]];
            default:
              return z;
          }
        }

      //}:                                        |
      //Assignment:{                              |

        e_forceinline Vector3& operator=( const Vector3& V ){
          if( this != &V ){
            x = V.x;
            y = V.y;
            z = V.z;
          }
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3& operator=( const Point3& P ){
          x = P.x;
          y = P.y;
          z = P.z;
          e_assert( valid() );
          return *this;
        }

        Vector3& operator=( const Vector4& V );

      //}:                                        |
      //Equality:{                                |

        e_forceinline bool operator!=( const Vector3& V )const{
          e_assert( valid() );
          if( this != &V ){
            if( x != V.x ){
              return true;
            }
            if( y != V.y ){
              return true;
            }
            if( z != V.z ){
              return true;
            }
          }
          return false;
        }

        e_forceinline bool operator==( const Vector3& V )const{
          e_assert( valid() );
          if( this != &V ){
            if( x != V.x ){
              return false;
            }
            if( y != V.y ){
              return false;
            }
            if( z != V.z ){
              return false;
            }
          }
          return true;
        }

      //}:                                        |
      //Bracket:{                                 |

        e_forceinline self operator[]( const u32 i )const{
          e_assert( i < e_sizeof( m ));
          e_assert( valid() );
          return m[ i ];
        }

        e_forceinline self& operator[]( const u32 i ){
          e_assert( i < e_sizeof( m ));
          e_assert( valid() );
          return m[ i ];
        }

      //}:                                        |
      //Negate:{                                  |

        e_forceinline Vector3 operator-()const{
          e_assert( valid() );
          Vector3 r;
          r.x = -x;
          r.y = -y;
          r.z = -z;
          return r;
        }

      //}:                                        |
      //Quotients:{                               |

        e_forceinline Vector3 operator/( const Vector3& v )const{
          Vector3 r;
          r.x = x / v.x;
          r.y = y / v.y;
          r.z = z / v.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator/=( const Vector3& V ){
          x /= V.x;
          y /= V.y;
          z /= V.z;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3& operator/=( const self& s ){
          x /= s;
          y /= s;
          z /= s;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator/( const self& s )const{
          Vector3 r;
          r.x = x / s;
          r.y = y / s;
          r.z = z / s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator/=( const float s ){
          x /= s;
          y /= s;
          z /= s;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator/( const float s )const{
          Vector3 r;
          r.x = x / s;
          r.y = y / s;
          r.z = z / s;
          e_assert( valid() );
          return r;
        }

      //}:                                        |
      //Products:{                                |

        e_forceinline Vector3 operator*( const Vector3& V )const{
          Vector3 r;
          r.x = x * V.x;
          r.y = y * V.y;
          r.z = z * V.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator*=( const Vector3& V ){
          x *= V.x;
          y *= V.y;
          z *= V.z;
          e_assert( valid() );
          return *this;
        }

        Vector3 operator*( const Matrix4& )const;
        Vector3 operator*( const Matrix3& )const;

        e_forceinline Vector3 operator*( const self& s )const{
          Vector3 r;
          r.x = x * s;
          r.y = y * s;
          r.z = z * s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator*=( const self& s ){
          x *= s;
          y *= s;
          z *= s;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator*( const float s )const{
          Vector3 r;
          r.x = x * s;
          r.y = y * s;
          r.z = z * s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator*=( const float s ){
          x *= s;
          y *= s;
          z *= s;
          e_assert( valid() );
          return *this;
        }

      //}:                                        |
      //Diffs:{                                   |

        e_forceinline Vector3 operator-( const Vector3& v )const{
          Vector3 r;
          r.x = x - v.x;
          r.y = y - v.y;
          r.z = z - v.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3 operator-( const Point3& p )const{
          Vector3 r;
          r.x = x - p.x;
          r.y = y - p.y;
          r.z = z - p.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator-=( const Vector3& v ){
          x -= v.x;
          y -= v.y;
          z -= v.z;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3& operator-=( const Point3& p ){
          x -= p.x;
          y -= p.y;
          z -= p.z;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator-( const self& s )const{
          Vector3 r;
          r.x = x - s;
          r.y = y - s;
          r.z = z - s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator-=( const self& s ){
          x -= s;
          y -= s;
          z -= s;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator-( const float s )const{
          Vector3 r;
          r.x = x - s;
          r.y = y - s;
          r.z = z - s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator-=( const float s ){
          x -= s;
          y -= s;
          z -= s;
          e_assert( valid() );
          return *this;
        }

      //}:                                        |
      //Sums:{                                    |

        e_forceinline Vector3 operator+( const Vector3& V )const{
          Vector3 r;
          r.x = x + V.x;
          r.y = y + V.y;
          r.z = z + V.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3 operator+( const Point3& p )const{
          Vector3 r;
          r.x = x + p.x;
          r.y = y + p.y;
          r.z = z + p.z;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator+=( const Vector3& V ){
          x += V.x;
          y += V.y;
          z += V.z;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3& operator+=( const Point3& P ){
          x += P.x;
          y += P.y;
          z += P.z;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator+( const self& s )const{
          Vector3 r;
          r.x = x + s;
          r.y = y + s;
          r.z = z + s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator+=( const self& s ){
          x += s;
          y += s;
          z += s;
          e_assert( valid() );
          return *this;
        }

        e_forceinline Vector3 operator+( const float s )const{
          Vector3 r;
          r.x = x + s;
          r.y = y + s;
          r.z = z + s;
          e_assert( valid() );
          return r;
        }

        e_forceinline Vector3& operator+=( const float s ){
          x += s;
          y += s;
          z += s;
          e_assert( valid() );
          return *this;
        }

      //}:                                        |
    //}:                                          |
    //Structs:{                                   |

      /** \brief A structure for interpolating between two vectors.
        *
        * This routine will interpolate between two vectors over time. It uses
        * the standard interface that all member interpolators use.
        */

      struct Interpolator{

        //----------------------------------------+-----------------------------
        //Methods:{                               |

          e_forceinline void begin( self N, const Vector3& s, const Vector3& e ){
            m_fTimeStart = m_fTime;
            m_fTimeEnd   = m_fTime + N;
            m_tStart     = s;
            m_tEnd       = e;
          }

          e_forceinline self time()const{
            return saturate(( m_fTime - m_fTimeStart )/( m_fTimeEnd - m_fTimeStart ));
          }

          e_forceinline Vector3 getValue()const{
            return Vector3(
                self::lerp( m_tStart.x, m_tEnd.x, time() ),
                self::lerp( m_tStart.y, m_tEnd.y, time() ),
                self::lerp( m_tStart.z, m_tEnd.z, time() ));
          }

          e_forceinline bool isFinished()const{
            return( time()==1 );
          }

          e_forceinline void tick( self dt ){
            m_fTime += dt;
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        Interpolator() = default;

      private:

        self m_fTimeStart = 0;
        self m_fTimeEnd   = 0;
        self m_fTime      = 0;

        e_var( Vector3, t, Start );
        e_var( Vector3, t, End );
      };

    //}:                                          |
    //Methods:{                                   |
      //[swizzling]:{                             |
        //xxx:{                                   |

          e_forceinline Vector3 xxx() const{
            e_assert( valid() );
            return Vector3( x, x, x );
          }

        //}:                                      |
        //xxy:{                                   |

          e_forceinline Vector3 xxy() const{
            e_assert( valid() );
            return Vector3( x, x, y );
          }

        //}:                                      |
        //xxz:{                                   |

          e_forceinline Vector3 xxz() const{
            e_assert( valid() );
            return Vector3( x, x, z );
          }

        //}:                                      |
        //xyx:{                                   |

          e_forceinline Vector3 xyx() const{
            e_assert( valid() );
            return Vector3( x, y, x );
          }

        //}:                                      |
        //xyy:{                                   |

          e_forceinline Vector3 xyy() const{
            e_assert( valid() );
            return Vector3( x, y, y );
          }

        //}:                                      |
        //xyz:{                                   |

          e_forceinline Vector3 xyz() const{
            e_assert( valid() );
            return Vector3( x, y, z );
          }

        //}:                                      |
        //xzx:{                                   |

          e_forceinline Vector3 xzx() const{
            e_assert( valid() );
            return Vector3( x, z, x );
          }

        //}:                                      |
        //xzy:{                                   |

          e_forceinline Vector3 xzy() const{
            e_assert( valid() );
            return Vector3( x, z, y );
          }

        //}:                                      |
        //xzz:{                                   |

          e_forceinline Vector3 xzz() const{
            e_assert( valid() );
            return Vector3( x, z, z );
          }

        //}:                                      |
        //yxx:{                                   |

          e_forceinline Vector3 yxx() const{
            e_assert( valid() );
            return Vector3( y, x, x );
          }

        //}:                                      |
        //yxy:{                                   |

          e_forceinline Vector3 yxy() const{
            e_assert( valid() );
            return Vector3( y, x, y );
          }

        //}:                                      |
        //yxz:{                                   |

          e_forceinline Vector3 yxz() const{
            e_assert( valid() );
            return Vector3( y, x, z );
          }

        //}:                                      |
        //yyx:{                                   |

          e_forceinline Vector3 yyx() const{
            e_assert( valid() );
            return Vector3( y, y, x );
          }

        //}:                                      |
        //yyy:{                                   |

          e_forceinline Vector3 yyy() const{
            e_assert( valid() );
            return Vector3( y, y, y );
          }

        //}:                                      |
        //yyz:{                                   |

          e_forceinline Vector3 yyz() const{
            e_assert( valid() );
            return Vector3( y, y, z );
          }

        //}:                                      |
        //yzx:{                                   |

          e_forceinline Vector3 yzx() const{
            e_assert( valid() );
            return Vector3( y, z, x );
          }

        //}:                                      |
        //yzy:{                                   |

          e_forceinline Vector3 yzy() const{
            e_assert( valid() );
            return Vector3( y, z, y );
          }

        //}:                                      |
        //yzz:{                                   |

          e_forceinline Vector3 yzz() const{
            e_assert( valid() );
            return Vector3( y, z, z );
          }

        //}:                                      |
        //zxx:{                                   |

          e_forceinline Vector3 zxx() const{
            e_assert( valid() );
            return Vector3( z, x, x );
          }

        //}:                                      |
        //zxy:{                                   |

          e_forceinline Vector3 zxy() const{
            e_assert( valid() );
            return Vector3( z, x, y );
          }

        //}:                                      |
        //zxz:{                                   |

          e_forceinline Vector3 zxz() const{
            e_assert( valid() );
            return Vector3( z, x, z );
          }

        //}:                                      |
        //zyx:{                                   |

          e_forceinline Vector3 zyx() const{
            e_assert( valid() );
            return Vector3( z, y, x );
          }

        //}:                                      |
        //zyy:{                                   |

          e_forceinline Vector3 zyy() const{
            e_assert( valid() );
            return Vector3( z, y, y );
          }

        //}:                                      |
        //zyz:{                                   |

          e_forceinline Vector3 zyz() const{
            e_assert( valid() );
            return Vector3( z, y, z );
          }

        //}:                                      |
        //zzx:{                                   |

          e_forceinline Vector3 zzx() const{
            e_assert( valid() );
            return Vector3( z, z, x );
          }

        //}:                                      |
        //zzy:{                                   |

          e_forceinline Vector3 zzy() const{
            e_assert( valid() );
            return Vector3( z, z, y );
          }

        //}:                                      |
        //zzz:{                                   |

          e_forceinline Vector3 zzz() const{
            e_assert( valid() );
            return Vector3( z, z, z );
          }

        //}:                                      |
        //xx:{                                    |

          e_forceinline Vector2 xx()const{
            e_assert( valid() );
            return Vector2( x, x );
          }

        //}:                                      |
        //xy:{                                    |

          e_forceinline Vector2 xy()const{
            e_assert( valid() );
            return Vector2( x, y );
          }

        //}:                                      |
        //xz:{                                    |

          e_forceinline Vector2 xz()const{
            e_assert( valid() );
            return Vector2( x, z );
          }

        //}:                                      |
        //yx:{                                    |

          e_forceinline Vector2 yx()const{
            e_assert( valid() );
            return Vector2( y, x );
          }

        //}:                                      |
        //yy:{                                    |

          e_forceinline Vector2 yy()const{
            e_assert( valid() );
            return Vector2( y, y );
          }

        //}:                                      |
        //yz:{                                    |

          e_forceinline Vector2 yz()const{
            e_assert( valid() );
            return Vector2( y, z );
          }

        //}:                                      |
        //zx:{                                    |

          e_forceinline Vector2 zx()const{
            e_assert( valid() );
            return Vector2( z, x );
          }

        //}:                                      |
        //zy:{                                    |

          e_forceinline Vector2 zy()const{
            e_assert( valid() );
            return Vector2( z, y );
          }

        //}:                                      |
        //zz:{                                    |

          e_forceinline Vector2 zz()const{
            e_assert( valid() );
            return Vector2( z, z );
          }

        //}:                                      |
      //}:                                        |
      //orthonormalize:{                          |

        e_forceinline static void orthonormalize( Vector3* v, const u32 n ){
          for( u32 i=0; i<n; ++i ){
            Vector3 acc;
            for( u32 j=0; j<i; ++j ){
              acc += v[ i ].projectToNorm( v[ j ]);
            }
            v[ i ] -= acc.normalized();
          }
        }

        e_forceinline Vector3 projectToNorm( const Vector3& d )const{
          e_assert( valid() && d.valid() );
          return d * dot( d );
        }

        e_forceinline friend void e_orthonormalize( Vector3& m, Vector3& b, Vector3& c ){
          e_assert( m.valid() && b.valid() && c.valid() );
          Vector3 v[3]={ m,b,c };
          orthonormalize( v, 3 );
        }

        e_forceinline friend void e_orthonormalize( Vector3& m, Vector3& b ){
          e_assert( m.valid() && b.valid() );
          Vector3 v[ 2 ]={ m,b };
          orthonormalize( v, 2 );
        }

      //}:                                        |
      //normalize*:{                              |

        e_forceinline friend Vector3 e_normalize( const Vector3& v ){
          return v.normalized();
        }

        e_forceinline Vector3 normalized()const{
          e_assert( valid() );
          Vector3 v = *this;
          v.normalize();
          return v;
        }

        e_forceinline Vector3& normalize(){
          e_assert( valid() );
          const self& l = length();
          if( !l ){
            clear();
          }else{
            const self& d = k1/l;
            x *= d;
            y *= d;
            z *= d;
          }
          return *this;
        }

      //}:                                        |
      //length*:{                                 |

        e_forceinline self lengthSquared( const Vector3& v ){
          return v.lengthSquared();
        }

        e_forceinline self lengthSquared()const{
          self lsq = x*x + y*y + z*z;
          e_assert( lsq.valid() );
          return lsq;
        }

        e_forceinline self length( const Vector3& v ){
          return v.length();
        }

        e_noinline self length()const{
          const self& lsq = lengthSquared();
          if( !lsq ){
            return k0;
          }
          if( lsq < k0 ){
            return k0;
          }
          return lsq.sqrt();
        }

      //}:                                        |
      //abs:{                                     |

        e_forceinline Vector3 abs()const{
          return Vector3( x.abs(), y.abs(), z.abs() );
        }

      //}:                                        |
      //isZero:{                                  |

        e_forceinline friend bool e_empty( const Vector3& v ){
          return v.empty();
        }

        e_forceinline bool empty()const{
          if( x != k0 ){
            return false;
          }
          if( y != k0 ){
            return false;
          }
          if( z != k0 ){
            return false;
          }
          return true;
        }

      //}:                                        |
      //negate:{                                  |

        e_forceinline friend Vector3 e_negate( const Vector3& v ){
          return Vector3( v ).negate();
        }

        e_forceinline Vector3& negate(){
          x = -x;
          y = -y;
          z = -z;
          e_assert( valid() );
          return *this;
        }

      //}:                                        |
      //valid:{                                   |

        e_forceinline bool valid()const{
          return( x.valid() && y.valid() && z.valid() );
        }

      //}:                                        |
      //cross:{                                   |

        e_forceinline friend Vector3 e_cross( const Vector3& v1, const Vector3& v2 ){
          return v1.cross( v2 );
        }

        e_forceinline Vector3 cross( const Vector3& V )const{
          e_assert( valid() && V.valid() );
          Vector3 X(
            y*V.z - z*V.y,
            z*V.x - x*V.z,
            x*V.y - y*V.x );
          return X;
        }

      //}:                                        |
      //angle:{                                   |

        e_forceinline friend self e_angle( const Vector3& v1, const Vector3& v2 ){
          return v1.angle( v2 );
        }

        e_forceinline self angle( const Vector3& V )const{
          e_assert( valid() && V.valid() );
          return atan2( V.y-y, V.x-x );
        }

      //}:                                        |
      //clear:{                                   |

        e_forceinline void clear(){
          e_assert( valid() );
          x = y = z = k0;
        }

      //}:                                        |
      //lerp:{                                    |

        e_forceinline friend Vector3 e_lerp( const Vector3& v1, const Vector3& v2, const self& t ){
          return Vector3( v1 ).lerp( v2, t );
        }

        e_forceinline Vector3& lerp( const Vector3& v, const self& t ){
          e_assert( valid() && v.valid() && t.valid() );
          e_assert( is_saturated( t ));
          x = x.lerp( v.x, t );
          y = y.lerp( v.y, t );
          z = z.lerp( v.z, t );
          return *this;
        }

      //}:                                        |
      //set:{                                     |

        e_forceinline void set( const self& X, const self& Y, const self& Z ){
          x = X;
          y = Y;
          z = Z;
          e_assert( valid() );
        }

      //}:                                        |
      //dot:{                                     |

        e_forceinline friend self e_dot( const Vector3& v1, const Vector3& v2 ){
          return v1.dot( v2 );
        }

        e_forceinline self dot( const Vector3& V )const{
          e_assert( valid() && V.valid() );
          return( x*V.x + y*V.y + z*V.z );
        }

      //}:                                        |
      //ave:{                                     |

        e_forceinline friend self e_ave( const Vector3& v1, const Vector3& v2 ){
          return v1.ave( v2 );
        }

        e_forceinline self ave()const{
          e_assert( valid() );
          return( x+y+z )/3.;
        }

      //}:                                        |
      //max:{                                     |

        e_forceinline friend self e_maxf( const Vector3& v ){
          return v.max();
        }

        e_forceinline self max()const{
          e_assert( valid() );
          return max3( x, y, z );
        }

      //}:                                        |
      //min:{                                     |

        e_forceinline friend self e_minf( const Vector3& v ){
          return v.min();
        }

        e_forceinline self min()const{
          e_assert( valid() );
          return min3( x, y, z );
        }

      //}:                                        |
    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline_always Vector3( const std::initializer_list<T>& ilist ){
      e_assert( ilist.size() <= 3 );
      u32 i = 0;
      for( auto& v : ilist ){
        m[ i++ ]=v;
      }
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector3 is vec3.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      * \param Z The Z.
      */

    e_forceinline_always Vector3( const self& X, const self& Y, const self& Z )
        : x( X )
        , y( Y )
        , z( Z ){
      e_assert( valid() );
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector3 is vec3.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      */

    e_forceinline_always Vector3( const self& X, const self& Y )
        : x( X )
        , y( Y )
        , z( 0 ){
      e_assert( valid() );
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector3 is vec3.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      * \param Z The Z.
      */

    e_forceinline_always Vector3( const float X, const float Y, const float Z )
      : x( X )
      , y( Y )
      , z( Z )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given args. A shortcut
      * to f32::Vector3 is vec3.
      *
      * \param X The X of the vector.
      * \param Y The Y.
      */

    e_forceinline_always Vector3( const float X, const float Y )
      : x( X )
      , y( Y )
      , z( 0 )
    {}

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given Vector2 and z
      * scalar.
      *
      * \param V The X and Y axis as a 2D vector.
      * \param Z The Z axis.
      */

    e_forceinline_always Vector3( const Vector2& V, const self& Z )
        : x( V.x )
        , y( V.y )
        , z( Z ){
      e_assert( valid() );
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given Vector2 and z
      * scalar.
      *
      * \param V The X and Y axis as a 2D vector.
      */

    e_forceinline_always Vector3( const Vector2& V )
        : x( V.x )
        , y( V.y )
        , z( 0 ){
      e_assert( valid() );
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given scalar.
      *
      * \param s The XYZ of the vector.
      */

    e_forceinline_always explicit Vector3( const self& s )
        : x( s )
        , y( s )
        , z( s ){
      e_assert( valid() );
    }

    /** \brief Vector constructor.
      *
      * This constructor initializes the vector with the given scalar.
      *
      * \param s The XYZ of the vector.
      */

    e_forceinline_always explicit Vector3( const float s )
      : x( s )
      , y( s )
      , z( s )
    {}

    /** \brief Point constructor.
      *
      * This routine will construct a vector from a point.
      * \todo This constructor really breaks the rules--delete it.
      */

    e_forceinline_always Vector3( const Point3& P )
        : x( P.x )
        , y( P.y )
        , z( P.z ){
      e_assert( valid() );
    }

    /** \brief Vector copy constructor.
      *
      * This routine will copy the given vector into this one.
      *
      * \param v The vector to copy into this.
      */

    e_forceinline_always Vector3( const Vector3& v )
        : x( v.x )
        , y( v.y )
        , z( v.z ){
      e_assert( valid() );
    }

    /** \brief Four tuple constructor.
      *
      * This constructor will build a vector3 from a four tuple vector.
      */

    e_forceinline_always Vector3( const Vector4& v )
        : x( v.x )
        , y( v.y )
        , z( v.z ){
      e_assert( valid() );
    }

    /** \brief Default constructor.
      *
      * This constructor initializes the vector to zero.
      */

    e_forceinline_always Vector3()
      : x( 0 )
      , y( 0 )
      , z( 0 )
    {}
  };

//}:                                              |
//================================================+=============================
