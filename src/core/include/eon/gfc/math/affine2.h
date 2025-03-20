//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Affine2:{                                       |

  /** \brief affine matrix.
    *
    * This Affine2 class defines a column major transform for use with OpenGL
    * mat3x4 GLSL type.
    *
    * Memory layout
    * +--         --+   +--      --+
    * | XX XY XZ XW |   | R R R Tx |
    * | YX YY YZ YW | = | R R R Ty |
    * | ZX ZY ZZ ZW |   | R R R Tz |
    * +--         --+   +--      --+
    */

  struct alignas(16) E_PUBLISH Affine2 final{

    //--------------------------------------------+-----------------------------
    //Operate:{                                   |
      //Assignment:{                              |

        /** \name Assignment operators.
          *
          * These operators assign a matrix or a type to this matrix.
          *
          * @{
          */

        /** \brief affine matrix assignment operator.
          *
          * This operator quickly assigns one affine matrix to another
          * using the SIMD type if possible.
          *
          * \param M The matrix to assign to this one.
          *
          * \return Returns the dereferenced this pointer.
          */

        e_forceinline Affine2& operator=( const Affine2& M ){
          #ifdef __SSE__
            v[0] = M.v[0];
            v[1] = M.v[1];
            v[2] = M.v[2];
          #else
            memcpy( t, M.t, sizeof( t ));
          #endif
          return *this;
        }

      //}:                                        |
      //Equality:{                                |

        /** @}
          *
          * \name Equality operators.
          *
          * These methods compare two affine matrices for equality and
          * returns the result as a bool.
          *
          * @{
          */

        /** \brief Equality operator.
          *
          * This operator compares this matrix with the input to see if they
          * are equal.
          *
          * \param M The input matrix to compare.
          *
          * \return Returns true if the two matrices are identical or false.
          */

        e_forceinline bool operator==( const Affine2& M )const{
          #if __SSE__
            for( u32 i=0; i<3; ++i ){
              const __m128i c = __m128i(_mm_cmpneq_ps( v[ i ], M.v[ i ]));
              const u16 r = _mm_movemask_epi8( c );
              if( r == 0xFFFF ){
                // all elements not equal.
                return false;
              }
              if( r != 0 ){
                // some elements not equal.
                return false;
              }
            }
            return true;
          #else
            return( 0 == memcmp( this, &M, sizeof( Affine2 )));
          #endif
        }

        /** \brief Inequality operator.
          *
          * This operator compares two matrices and returns true if they are
          * unequal.
          *
          * \param M The input matrix to compare.
          *
          * \return Returns true if the two matrices are NOT equal or false.
          */

        e_forceinline bool operator!=( const Affine2& M )const{
          return( !(( *this )==M ));
        }

      //}:                                        |
      //Multiply:{                                |

        /** @}
          *
          * \name Multiplication operators.
          *
          * These operators multiply the affine matrix by various other
          * types, such as vectors and points.
          *
          * @{
          */

        /** \brief Multiplication operator.
          *
          * This routine will multiply two affine matrices together.
          *
          * \param A The input matrix to multiply.
          *
          * \return Returns the transformed matrix.
          */

        e_forceinline Affine2 operator*( const Affine2& A )const{
          Affine2 M = *this;
          M *= A;
          return M;
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies the matrix by a scalar.
          *
          * \param s The scalar to multiply this matrix by.
          *
          * \return Returns the scaled matrix.
          */

        e_forceinline Affine2 operator*( const self& s )const{
          Affine2 M = *this;
          M *= s;
          return M;
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies this matrix by a 3D point.
          *
          * \param P The point to multiply.
          *
          * \return Returns the transformed point.
          */

        e_forceinline Point2 operator*( const Point2& P )const{
          return Point3(
            XX * P.x + YX * P.y + ZX * P.z + WX,
            XY * P.x + YY * P.y + ZY * P.z + WY
          );
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies this matrix by an AABB.
          *
          * \param B The aabb to transform.
          *
          * \return Returns the transformed AABB.
          */

        e_forceinline AABB2 operator*( const AABB2& B )const{
          e_assert( B.valid() );
          e_assert( valid() );
          AABB3 b;
          b += *this * B.min;
          b += *this * B.max;
          return b;
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies this matrix by the input and updates this.
          *
          * \param M The input matrix to multiply against.
          *
          * \return Returns the this pointer dereferenced.
          */

        Affine2& operator*=( const Affine2& M );

        /** \brief Multiplication operator.
          *
          * This operator multiplies the matrix by a scalar.
          *
          * \param s The scalar to multiply this matrix by.
          *
          * \return Returns the scaled matrix.
          */

        e_forceinline Affine2& operator*=( const self& s ){
          #ifdef __SSE__
            const __m128 x = _mm_set1_ps( f32::cast( s ));
            v[ 0 ]=_mm_mul_ps( v[ 0 ], x );
            v[ 1 ]=_mm_mul_ps( v[ 1 ], x );
          #else
            XX *= s; YX *= s; ZX *= s; WX *= s;
            XY *= s; YY *= s; ZY *= s; WY *= s;
          #endif
          return *this;
        }

      //}:                                        |
      //Array:{                                   |

        /** @}
          *
          * \name Indexing operators.
          *
          * These operators allow you to index the matrix by column, row or by
          * a tuple index.
          *
          * @{
          */

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by row and column.
          *
          * \param r The row to use.
          * \param c The column to use.
          *
          * \return Returns the tuple you've indexed.
          */

        e_forceinline self operator()( const u32 r, const u32 c )const{
          e_assert( r < 3 );
          e_assert( c < 4 );
          return m[ r ][ c ];
        }

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by row and column.
          *
          * \param r The row to use.
          * \param c The column to use.
          *
          * \return Returns the tuple you've indexed.
          */

        e_forceinline self& operator()( const u32 r, const u32 c ){
          e_assert( r < 3 );
          e_assert( c < 4 );
          return m[ r ][ c ];
        }

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by row and column.
          *
          * \param r The row to use.
          *
          * \return Returns the four tuple vector indexed.
          */

        e_forceinline const Vector4& operator()( const u32 r )const{
          e_assert( r < 3 );
          return a[ r ];
        }

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by row and column.
          *
          * \param r The row to use.
          *
          * \return Returns the four tuple vector indexed.
          */

        e_forceinline Vector4& operator()( const u32 r ){
          e_assert( r < 3 );
          return a[ r ];
        }

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by index.
          *
          * \param i The tuple index between 0 and 15.
          *
          * \return Returns the tuple you've indexed.
          */

        e_forceinline self operator[]( const u32 i )const{
          return t[ i ];
        }

        /** \brief Indexing operator.
          *
          * This routine will index the desired tuple by index.
          *
          * \param i The tuple index between 0 and 15.
          *
          * \return Returns the tuple you've indexed.
          */

        e_forceinline self& operator[]( const u32 i ){
          return t[ i ];
        }

      //}:                                        |
      //Diff:{                                    |

        /** @}
          *
          * \name Difference operators.
          *
          * These methods allow you to subtract one matrix from another.
          *
          * @{
          */

        e_forceinline Affine2 operator-( const Affine2& b )const{
          #ifdef __SSE__
            Affine2 a;
            a.v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
            return a;
          #else
            return Affine2(
              XX-b.XX, YX-b.YX, ZX-b.ZX, WX-b.XW,
              XY-b.XY, YY-b.YY, ZY-b.ZY, WY-b.YW );
          #endif
        }

        e_forceinline Affine2& operator-=( const Affine2& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
          #else
            *this = Affine2(
              XX-b.XX, YX-b.YX, ZX-b.ZX, WX-b.WX,
              XY-b.XY, YY-b.YY, ZY-b.ZY, WY-b.WY );
          #endif
          return *this;
        }

      //}:                                        |
      //Sum:{                                     |

        /** @}
          *
          * \name Sum operators.
          *
          * These methods allow you to add one matrix from another.
          *
          * @{
          */

        e_forceinline Affine2 operator+( const Affine2& b )const{
          #ifdef __SSE__
            Affine2 a;
            a.v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
            a.v[ 2 ]=_mm_add_ps( v[ 2 ], b.v[ 2 ]);
            return a;
          #else
            return Affine2(
              XX+b.XX, YX+b.YX, ZX+b.ZX, WX+b.WX,
              XY+b.XY, YY+b.YY, ZY+b.ZY, WY+b.WY );
          #endif
        }

        e_forceinline Affine2& operator+=( const Affine2& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
          #else
            *this = Affine2(
              XX+b.XX, YX+b.YX, ZX+b.ZX, WX+b.WX,
              XY+b.XY, YY+b.YY, ZY+b.ZY, WY+b.WY );
          #endif
          return *this;
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |

      e_forceinline bool valid()const{
        if( !a[ 0 ].valid() ){
          return false;
        }
        if( !a[ 1 ].valid() ){
          return false;
        }
        return true;
      }

      /** @}
        *
        * \name Matrix scale methods.
        *
        * These methods do all the normal scale operations you'd expect from an
        * affine matrix.
        *
        * @{
        */

      /** \brief Get scale matrix.
        *
        * This routine will return a scale matrix from the given vector.
        *
        * \param x The X scale.
        * \param y The Y scale.
        *
        * \return Returns a 4x4 matrix with scale set.
        */

      e_forceinline static Affine2 getS( const self& x, const self& y ){
        Affine2 M;
        M.setScale( x, y );
        e_assert( M.valid() );
        return M;
      }

      /** \brief Get scale matrix.
        *
        * This routine will return a scale matrix from the given vector.
        *
        * \param s The XYZ scale.
        *
        * \return Returns a 4x4 matrix with scale set.
        */

      e_forceinline static Affine2 getS( const Vector3& s ){
        Affine2 M;
        M.setScale( s.x, s.y );
        e_assert( M.valid() );
        return M;
      }

      /** \brief Get scale matrix.
        *
        * This routine will return a scale matrix from the given vector.
        *
        * \param s The XYZ scale.
        *
        * \return Returns a 4x4 matrix with scale set.
        */

      e_forceinline static Affine2 getS( const self& s ){
        e_assert( s.valid() );
        return Affine2( s );
      }

      /** \brief Construct scale matrix.
        *
        * This routine will create a scale matrix from the given components.
        *
        * \param x The X component of the scale.
        * \param y The Y component of the scale.
        */

      void setScale( const self& x, const self& y );

      /** \brief Construct uniform scale matrix.
        *
        * This routine will create a scale matrix from the given components.
        *
        * \param x The scale to load into scale uniformly.
        */

      void setScale( const self& x );

      /** \brief Add delta scale to 4x4 transform.
        *
        * This routine will scale the matrix by xyz.
        */

      e_forceinline void scale( const self& x, const self& y ){
        *this *= getS( x, y );
      }

      /** \brief Add delta scale to 4x4 transform.
        *
        * This routine will scale the matrix by xyz.
        */

      e_forceinline void scale( const self& s ){
        *this *= getS( s, s );
      }

      /** \brief Set uniform scale.
        *
        * This routine will return the uniform scale of the matrix by treating
        * the scale components as a vector and taking the length.
        *
        * \return Returns a real number representing the uniform scale.
        */

      e_forceinline self uniformScale()const{
        e_assert( valid() );
        return Vector2( XX, YY ).length();
      }

      /** \brief Remove scale.
        *
        * This routine will remove the scale from a copy of this transform and
        * return the result.
        *
        * \return Returns a copy of this transform with the scale removed.
        */

      e_forceinline Affine2 noscale()const{
        Affine2 M( *this );
        M.removeScale();
        e_assert( M.valid() );
        return M;
      }

      /** \brief Remove scale from matrix.
        *
        * This routine will remove the scale from this matrix. When converting
        * this 4x4 to a 3x3 transform you should always call removeScale()
        * first.
        */

      void removeScale();

      /** @}
        *
        * \name Matrix rotation methods.
        *
        * These methods do all the normal rotataion operations you'd expect
        * from an affine matrix.
        *
        * @{
        */

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_forceinline static Affine2 getR( const self& xEuler, const self& yEuler ){
        return Quaternion( xEuler, yEuler ).get2x4();
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_forceinline static Affine2 getR( const Vector2& v ){
        return Quaternion( v ).get2x4();
      }

      /** \brief Construct rotation matrix about X axis.
        *
        * This routine will construct a rotation matrix.
        *
        * \param deg The angle in degrees.
        */

      void setRotX( const self& deg );

      /** \brief Construct rotation matrix about Y axis.
        *
        * This routine will construct a rotation matrix.
        *
        * \param deg The angle in degrees.
        */

      void setRotY( const self& deg );

      /** @}
        *
        * \name Matrix translation methods.
        *
        * These methods do all the normal translation operations you'd expect
        * from an affine matrix.
        *
        * @{
        */

      /** \brief Get translation matrix.
        *
        * This routine will return a translation matrix from the given vector.
        *
        * \param x The X translation.
        * \param y The Y translation.
        *
        * \return Returns a 4x4 matrix with translation set.
        */

      e_forceinline static Affine2 getT( const self& x, const self& y ){
        Affine2 M;
        M.setTranslation( x, y );
        return M;
      }

      /** \brief Get translation matrix.
        *
        * This routine will return a translation matrix from the given vector.
        *
        * \param v The XYZ translation.
        *
        * \return Returns a 4x4 matrix with translation set.
        */

      e_forceinline static Affine2 getT( const Vector2& v ){
        Affine2 M;
        M.setTranslation( v.x, v.y, v.z );
        return M;
      }

      /** \brief Get translation matrix.
        *
        * This routine will return a translation matrix from the given vector.
        *
        * \param p The XYZ translation.
        *
        * \return Returns a 4x4 matrix with translation set.
        */

      e_forceinline static Affine2 getT( const Point2& p ){
        Affine2 M;
        M.setTranslation( p.x, p.y );
        return M;
      }

      /** \brief Add delta translation to 4x4 transform.
        *
        * This routine will translate the matrix by xyz.
        */

      e_forceinline void translate( const self& x, const self& y ){
        *this *= getT( x, y );
      }

      /** \brief Construct translation matrix.
        *
        * This routine will load the matrix with just identity and scale.
        *
        * \param x The X component of translation.
        * \param y The Y component of translation.
        */

      void setTranslation( const self& x, const self& y );

      /** \brief Remove translation.
        *
        * This routine will remove the translation from a copy of this
        * transform and return the result.
        *
        * \return Returns a copy of this transform with the translation
        * removed.
        */

      e_forceinline Affine2 noTranslation()const{
        Affine2 M( *this );
        M.removeTranslation();
        e_assert( M.valid() );
        return M;
      }

      /** \brief Remove translation from matrix.
        *
        * This routine will zero out the translation row.
        */

      void removeTranslation();

      /** \brief Get matrix translation.
        *
        * This routine will return the matrix translation as a position.
        */

      e_forceinline Point2 toPosition()const{
        e_assert( valid() );
        return Point2( column( 3 ));
      }

      /** @}
        *
        * \name Matrix column and row methods.
        *
        * These methods do all the normal column and row accessors.
        *
        * @{
        */

      /** \brief Get matrix column.
        *
        * This routine will grab the given column by index.
        *
        * \param i The column index, must be 0 to 3.
        *
        * \return Returns the column indexed by i.
        */

      e_forceinline Vector2 column( const u32 i )const{
        e_assert( valid() );
        e_assert( i < 4, "Column does not exist" );
        return Vector2( a[0][i], a[1][i] );
      }

      /** \brief Get matrix row.
        *
        * This routine will grab the given row by index.
        *
        * \param i The row index, must be 0 to 3.
        *
        * \return Returns the row indexed by i.
        */

      e_forceinline Vector4 row( const u32 i )const{
        e_assert( valid() );
        e_assert( i < 2, "Row does not exist" );
        return a[i];
      }

      /** @}
        *
        * \name Matrix eigen vector accessors.
        *
        * These methods to grab various eigen values.
        *
        * @{
        */

      /** \brief Get matrix forward as vector.
        *
        * This routine will return the forward of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 forward()const{
        e_assert( valid() );
        return row( 0 );
      }

      /** \brief Get matrix right as vector.
        *
        * This routine will return the right of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 right()const{
        e_assert( valid() );
        return row( 1 );
      }

      /** @}
        *
        * \name Special edge case methods.
        *
        * These methods let you flip the YZ. Other strangeness goes here.
        *
        * @{
        */

      /** \brief Set identity matrix.
        *
        * This routine will clear the matrix to its identity.
        */

      e_forceinline void setIdentity(){
        *this = Affine2();
      }

      /** \brief Check if identity.
        *
        * This routine will check to see if this is an identity matrix.
        *
        * \param a The affine transform to check.
        * \return Returns true if the affine transform is identity or false.
        */

      e_forceinline friend bool e_isIdentity( const Affine2& a ){
        return a.isIdentity();
      }

      /** \brief Check if identity.
        *
        * This routine will check to see if this is an identity matrix.
        *
        * \return Returns true if the affine transform is identity or false.
        */

      e_forceinline bool isIdentity()const{
        return( *this == Affine2() );
      }

      /** \brief Set the matrix to zero.
        *
        * This routine will load every component of the matrix with zero.
        */

      e_forceinline void clear(){
        #ifdef __SSE__
          v[0] = _mm_set1_ps( 0 );
          v[1] = v[0];
        #else
          memset( t, 0, sizeof( t ));
        #endif
      }

      /** @}
        *
        * \name Transform.
        *
        * These methods let you transform various types.
        *
        * @{
        */

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend Affine2 e_mul( const Affine2& A, const Affine2& B ){
        return A * B;
      }

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend Point3 e_mul( const Affine2& M, const Point3& P ){
        return M * P;
      }

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend AABB2 e_mul( const Affine2& A, const AABB2& B ){
        e_assert( A.valid() && B.valid() );
        return A * B;
      }

      /** @}
        *
        * \name Conversion methods.
        *
        * These methods have to do with converting the matrix to other types
        * such as a quaternion.
        */

      /** \brief Get quaternion from matrix.
        *
        * This routine will convert the rotation portion of the transform and
        * return it as a quaternion.
        *
        * \param a The affine transform to convert to a quaternion.
        *
        * \return Returns a rotation expressed as a quaternion.
        */

      e_forceinline friend Quaternion e_toQuaternion( const Affine2& a ){
        return a.template as<Quaternion>();
      }

      /** \brief Get point from matrix.
        *
        * This routine will convert the translation portion of the transform
        * and return it as a pt3 type.
        *
        * \param a The affine transform to convert to a quaternion.
        *
        * \return Returns a point in 3D space.
        */

      e_forceinline friend Point2 e_toPoint( const Affine2& a ){
        return a.template as<Point2>();
      }

      /** \brief Get quaternion from matrix.
        *
        * This routine will convert the rotation portion of the transform and
        * return it as a quaternion.
        *
        * \tparam Ty The type to convert this affine transform to. Valid types
        * are currently quat and pt3.
        *
        * \return Returns converted value.
        */

      template<typename Ty> Ty as()const;

      /** @} */

    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline Affine2(
        const self& xx, const self& yx, const self& zx, const self& wx,
        const self& xy, const self& yy, const self& zy, const self& wy ){
      XX = xx; YX = yx; ZX = zx; WX = wx;
      XY = xy; YY = yy; ZY = zy; WY = wy;
      e_assert( valid() );
    }

    constexpr Affine2(
        const float xx, const float yx, const float zx, const float wx,
        const float xy, const float yy, const float zy, const float wy )
      : m00( xx ), m10( yx ), m20( zx ), m30( wx )
      , m01( xy ), m11( yy ), m21( zy ), m31( wy )
    {}

    e_forceinline Affine2( const Affine2& M ){
      #ifdef __SSE__
        v[ 0 ]=M.v[ 0 ];
        v[ 1 ]=M.v[ 1 ];
      #else
        XX=M.XX; YX=M.YX; ZX=M.ZX; WX=M.WX;
        XY=M.XY; YY=M.YY; ZY=M.ZY; WY=M.WY;
      #endif
      e_assert( valid() );
    }

    e_forceinline Affine2( const self& scale ){
      setScale( scale );
      e_assert( valid() );
    }

    constexpr Affine2( const float s )
      : m00( s ), m10( 0 ), m20( 0 ), m30( 0 )
      , m01( 0 ), m11( s ), m21( 0 ), m31( 0 )
    {}

    constexpr Affine2()
      : m00( 1 ), m10( 0 ), m20( 0 ), m30( 0 )
      , m01( 0 ), m11( 1 ), m21( 0 ), m31( 0 )
    {}

    union{
      struct{
        float m00, m10, m20, m30;
        float m01, m11, m21, m31;
      };
      struct{
        self XX, YX, ZX, WX;
        self XY, YY, ZY, WY;
      };
      #ifdef __SSE__
        __m128 v[2];
      #endif
      Vector4 a[2];
      self    m[2][4];
      self    t[8];
    };
  };

//}:                                              |
//================================================+=============================
