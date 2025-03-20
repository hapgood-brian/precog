//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Point2:{                                        |

  struct E_PUBLISH Point2{

    //--------------------------------------------+-----------------------------
    //Statics:{                                   |

      static const u32    kTuples = 2;
      static const Point2 kZero;

    //}:                                          |
    //Operate:{                                   |
      //[parenthesis]:{                           |

        e_forceinline self operator()( const u32 i )const{
          e_assert( i < e_sizeof( a ));
          return a[ i ];
        }

        e_forceinline self& operator()( const u32 i ){
          e_assert( i < e_sizeof( a ));
          return a[ i ];
        }

      //}:                                        |
      //[assignment]:{                            |

        e_forceinline Point2& operator=( const self& scalar ){
          x = y = scalar;
          return *this;
        }

        e_forceinline Point2& operator=( const Point2& P ){
          x = P.x;
          y = P.y;
          return *this;
        }

      //}:                                        |
      //[equality]:{                              |

        e_forceinline bool operator==( const Point2& P )const{
          if( x != P.x ){
            return false;
          }
          if( y != P.y ){
            return false;
          }
          return true;
        }

        e_forceinline bool operator!=( const Point2& P )const{
          if( x != P.x ){
            return true;
          }
          if( y != P.y ){
            return true;
          }
          return false;
        }

      //}:                                        |
      //[bracket]:{                               |

        e_forceinline self operator[]( const u32 i )const{
          e_assert( i < e_sizeof( a ));
          return a[ i ];
        }

        e_forceinline self& operator[]( const u32 i ){
          e_assert( i < e_sizeof( a ));
          return a[ i ];
        }

      //}:                                        |
      //[quotient]:{                              |

        e_forceinline Point2 operator/( const Vector2& V )const{
          Point2 ret( *this );
          ret /= V;
          return ret;
        }

        e_forceinline Point2 operator/( const Point2& P )const{
          Point2 ret( *this );
          ret /= P;
          return ret;
        }

        e_forceinline Point2& operator/=( const Vector2& V ){
          x /= V.x;
          y /= V.y;
          return *this;
        }

        e_forceinline Point2& operator/=( const Point2& P ){
          x /= P.x;
          y /= P.y;
          return *this;
        }

        e_forceinline Point2 operator/( const self& scalar )const{
          Point2 ret( *this );
          ret /= scalar;
          return ret;
        }

        e_forceinline Point2& operator/=( const self& scalar ){
          x /= scalar;
          y /= scalar;
          return *this;
        }

        e_forceinline Point2 operator/( const float scalar )const{
          Point2 ret( *this );
          ret /= scalar;
          return ret;
        }

        e_forceinline Point2& operator/=( const float scalar ){
          x /= scalar;
          y /= scalar;
          return *this;
        }

      //}:                                        |
      //[product]:{                               |

        e_forceinline Point2 operator*( const Vector2& V )const{
          return Point2( x*V.x, y*V.y );
        }

        e_forceinline Point2 operator*( const Point2& P )const{
          return Point2( x*P.x, y*P.y );
        }

        e_forceinline Point2& operator*=( const Vector2& V ){
          x *= V.x;
          y *= V.y;
          return *this;
        }

        e_forceinline Point2& operator*=( const Point2& P ){
          x *= P.x;
          y *= P.y;
          return *this;
        }

        e_forceinline Point2& operator*=( const self& scalar ){
          x *= scalar;
          y *= scalar;
          return *this;
        }

        e_forceinline Point2 operator*( const self& scalar )const{
          return Point2( x*scalar, y*scalar );
        }

        e_forceinline Point2& operator*=( const float scalar ){
          x *= scalar;
          y *= scalar;
          return *this;
        }

        e_forceinline Point2 operator*( const float scalar )const{
          return Point2( x*scalar, y*scalar );
        }

      //}:                                        |
      //[diff]:{                                  |

        e_forceinline Point2 operator-( const Vector2& V )const{
          return Point2( x-V.x, y-V.y );
        }

        e_forceinline Point2 operator-( const Point2& P )const{
          return Point2( x-P.x, y-P.y );
        }

        e_forceinline Point2& operator-=( const Vector2& V ){
          x -= V.x;
          y -= V.y;
          return *this;
        }

        e_forceinline Point2& operator-=( const Point2& P ){
          x -= P.x;
          y -= P.y;
          return *this;
        }

        e_forceinline Point2 operator-( const self& scalar )const{
          return Point2( x-scalar, y-scalar );
        }

        e_forceinline Point2& operator-=( const self& scalar ){
          x -= scalar;
          y -= scalar;
          return *this;
        }

        e_forceinline Point2 operator-( const float scalar )const{
          return Point2( x-scalar, y-scalar );
        }

        e_forceinline Point2& operator-=( const float scalar ){
          x -= scalar;
          y -= scalar;
          return *this;
        }

      //}:                                        |
      //[sum]:{                                   |

        e_forceinline Point2 operator+( const Vector2& V )const{
          return Point2( x+V.x, y+V.y );
        }

        e_forceinline Point2 operator+( const Point2& P )const{
          return Point2( x+P.x, y+P.y );
        }

        e_forceinline Point2& operator+=( const Vector2& V ){
          x += V.x;
          y += V.y;
          return *this;
        }

        e_forceinline Point2& operator+=( const Point2& P ){
          x += P.x;
          y += P.y;
          return *this;
        }

        e_forceinline Point2 operator+( const self& scalar )const{
          return Point2( x+scalar, y+scalar );
        }

        e_forceinline Point2& operator+=( const self& scalar ){
          x += scalar;
          y += scalar;
          return *this;
        }

        e_forceinline Point2 operator+( const float scalar )const{
          return Point2( x+scalar, y+scalar );
        }

        e_forceinline Point2& operator+=( const float scalar ){
          x += scalar;
          y += scalar;
          return *this;
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |

      e_forceinline bool valid()const{
        return( x.valid() && y.valid() );
      }

      e_forceinline bool almost( const Point2& P, const self& epsilon=kEpsilon )const{
        return( distance( P ) <= epsilon );
      }

      e_forceinline Point2 snapped( const self& size )const{
        Point2 p( *this );
        p.snap( size );
        return p;
      }

      e_forceinline void set( const self& X, const self& Y ){
        x = X;
        y = Y;
        e_assert( valid() );
      }

      e_forceinline Point2& snap( const self& size ){
        x = floor( x/size )*size;
        y = floor( y/size )*size;
        e_assert( valid() );
        return *this;
      }

      e_forceinline self distanceSquared( const Point2& P )const{
        e_assert( valid() && P.valid() );
        const self& dx = P.x - x;
        const self& dy = P.y - y;
        return dx*dx + dy*dy;
      }

      e_forceinline self distance( const Point2& P )const{
        const self& d = sqrt( distanceSquared( P ));
        e_assert( d.valid() );
        return d;
      }

      Point2 scaledToClip( const self& scale )const;
      Point2 scaledBack()const;

      e_forceinline self lengthSquared()const{
        const self& lsq=( x*x + y*y );
        e_assert( lsq.valid() );
        return lsq;
      }

      void scaleToClip( const self& scale );
      void scaleBack();

      e_forceinline self length()const{
        const self& l = sqrt( lengthSquared() );
        e_assert( l.valid() );
        return l;
      }

      e_forceinline void clear(){
        x = 0;
        y = 0;
      }

      e_forceinline const Point2& clamp( const self& a, const self& b )const{
        Point2 xy( *this );
        xy.x.clamp( a, b );
        xy.y.clamp( a, b );
        return xy;
      }

      e_forceinline const Point2& saturate()const{
        return clamp( 0.f, 1.f );
      }

    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline_always Point2( const self& X, const self& Y )
      : x( X )
      , y( Y )
    {}

    e_forceinline_always Point2( const float X, const float Y )
      : x( X )
      , y( Y )
    {}

    e_forceinline_always Point2( const Point2& p )
      : x( p.x )
      , y( p.y )
    {}

    e_forceinline_always Point2( const self& X )
      : x( X )
      , y( X )
    {}

    e_forceinline_always Point2( const float X )
      : x( X )
      , y( X )
    {}

    e_forceinline_always Point2()
      : x( 0 )
      , y( 0 )
    {}

    union{
      struct{
        self x, y;
      };
      self a[ 2 ];
    };
  };

//}:                                              |
//================================================+=============================
