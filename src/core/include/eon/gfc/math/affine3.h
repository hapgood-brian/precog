//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Affine3:{                                       |

  /** \brief affine matrix.
    *
    * This Affine3 class defines a column major transform for use with OpenGL
    * mat3x4 GLSL type.
    *
    * Memory layout
    * +--         --+   +--      --+
    * | XX XY XZ XW |   | R R R Tx |
    * | YX YY YZ YW | = | R R R Ty |
    * | ZX ZY ZZ ZW |   | R R R Tz |
    * +--         --+   +--      --+
    */

  struct alignas(16) E_PUBLISH Affine3 final{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |

      /** \brief Rotation order.
        *
        * This enum class defines the rotation order that a rotation matrix can
        * be constructed from.
        */

      enum class Order:u32{
        kNone, //!< No rotation order.
        XYZ,   //!< Regular XYZ order.
        XZY,   //!< Strange XZY order.
        YZX,   //!< Strange YZX order.
        YXZ,   //!< Strange YXZ order.
        ZXY,   //!< Strange ZXY order.
        ZYX,   //!< Strange ZYX order.
      };

    //}:                                          |
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

        e_forceinline Affine3& operator=( const Affine3& M ){
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

        e_forceinline bool operator==( const Affine3& M )const{
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
            return( 0 == memcmp( this, &M, sizeof( Affine3 )));
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

        e_forceinline bool operator!=( const Affine3& M )const{
          #if __SSE__
            for( u32 i=0; i<3; ++i ){
              const __m128i c = __m128i(_mm_cmpneq_ps( v[ i ], M.v[ i ]));
              const u16 r = _mm_movemask_epi8( c );
              if( r == 0xFFFF ){
                // all elements not equal.
                return true;
              }
              if( r != 0 ){
                // some elements not equal.
                return true;
              }
            }
            return false;
          #else
            return( 0 != memcmp( this, &M, sizeof( Affine3 )));
          #endif
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

        e_forceinline Affine3 operator*( const Affine3& A )const{
          Affine3 M = *this;
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

        e_forceinline Affine3 operator*( const self& s )const{
          Affine3 M = *this;
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

        e_forceinline Point3 operator*( const Point3& P )const{
          return Point3(
            XX * P.x + YX * P.y + ZX * P.z + WX,
            XY * P.x + YY * P.y + ZY * P.z + WY,
            XZ * P.x + YZ * P.y + ZZ * P.z + WZ
          );
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies this matrix by a 3D point.
          *
          * \param P The point to multiply.
          *
          * \return Returns the transformed point.
          */

        e_forceinline Vector3 operator*( const Vector3& P )const{
          return Vector3(
            XX * P.x + YX * P.y + ZX * P.z + WX,
            XY * P.x + YY * P.y + ZY * P.z + WY,
            XZ * P.x + YZ * P.y + ZZ * P.z + WZ
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

        e_noinline AABB3 operator*( const AABB3& B )const{
          const auto& v0 = Point3( B.min.x, B.min.y, B.min.z );
          const auto& v1 = Point3( B.min.x, B.max.y, B.min.z );
          const auto& v2 = Point3( B.max.x, B.min.y, B.min.z );
          const auto& v3 = Point3( B.max.x, B.max.x, B.min.z );
          const auto& v4 = Point3( B.min.x, B.min.y, B.max.z );
          const auto& v5 = Point3( B.min.x, B.max.y, B.max.z );
          const auto& v6 = Point3( B.max.x, B.min.y, B.max.z );
          const auto& v7 = Point3( B.max.x, B.max.x, B.max.z );
          AABB3 b;
          b += *this * v0;
          b += *this * v1;
          b += *this * v2;
          b += *this * v3;
          b += *this * v4;
          b += *this * v5;
          b += *this * v6;
          b += *this * v7;
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

        Affine3& operator*=( const Affine3& M );

        /** \brief Multiplication operator.
          *
          * This operator multiplies the matrix by a scalar.
          *
          * \param s The scalar to multiply this matrix by.
          *
          * \return Returns the scaled matrix.
          */

        e_forceinline Affine3& operator*=( const self& s ){
          #ifdef __SSE__
            const __m128 x = _mm_set1_ps( f32::cast( s ));
            v[ 0 ]=_mm_mul_ps( v[ 0 ], x );
            v[ 1 ]=_mm_mul_ps( v[ 1 ], x );
            v[ 2 ]=_mm_mul_ps( v[ 2 ], x );
          #else
            XX *= s; YX *= s; ZX *= s; WX *= s;
            XY *= s; YY *= s; ZY *= s; WY *= s;
            XZ *= s; YZ *= s; ZZ *= s; WZ *= s;
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

        e_forceinline Affine3 operator-( const Affine3& b )const{
          #ifdef __SSE__
            Affine3 a;
            a.v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
            a.v[ 2 ]=_mm_sub_ps( v[ 2 ], b.v[ 2 ]);
            return a;
          #else
            return Affine3(
              XX-b.XX, YX-b.YX, ZX-b.ZX, WX-b.XW,
              XY-b.XY, YY-b.YY, ZY-b.ZY, WY-b.YW,
              XZ-b.XZ, YZ-b.YZ, ZZ-b.ZZ, WZ-b.ZW );
          #endif
        }

        e_forceinline Affine3& operator-=( const Affine3& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
            v[ 2 ]=_mm_sub_ps( v[ 2 ], b.v[ 2 ]);
          #else
            *this = Affine3(
              XX-b.XX, YX-b.YX, ZX-b.ZX, WX-b.WX,
              XY-b.XY, YY-b.YY, ZY-b.ZY, WY-b.WY,
              XZ-b.XZ, YZ-b.YZ, ZZ-b.ZZ, WZ-b.WZ );
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

        e_forceinline Affine3 operator+( const Affine3& b )const{
          #ifdef __SSE__
            Affine3 a;
            a.v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
            a.v[ 2 ]=_mm_add_ps( v[ 2 ], b.v[ 2 ]);
            return a;
          #else
            return Affine3(
              XX+b.XX, YX+b.YX, ZX+b.ZX, WX+b.WX,
              XY+b.XY, YY+b.YY, ZY+b.ZY, WY+b.WY,
              XZ+b.XZ, YZ+b.YZ, ZZ+b.ZZ, WZ+b.WZ );
          #endif
        }

        e_forceinline Affine3& operator+=( const Affine3& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
            v[ 2 ]=_mm_add_ps( v[ 2 ], b.v[ 2 ]);
          #else
            *this = Affine3(
              XX+b.XX, YX+b.YX, ZX+b.ZX, WX+b.WX,
              XY+b.XY, YY+b.YY, ZY+b.ZY, WY+b.WY,
              XZ+b.XZ, YZ+b.YZ, ZZ+b.ZZ, WZ+b.WZ );
          #endif
          return *this;
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |

      e_forceinline bool valid()const{
        return(
          a[ 0 ].valid()
        &&a[ 1 ].valid()
        &&a[ 2 ].valid() );
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
        * \param z The Z scale.
        *
        * \return Returns a 4x4 matrix with scale set.
        */

      e_forceinline static Affine3 getS( const self& x, const self& y, const self& z ){
        Affine3 M;
        M.setScale( x, y, z );
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

      e_forceinline static Affine3 getS( const Vector3& s ){
        Affine3 M;
        M.setScale( s.x, s.y, s.z );
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

      e_forceinline static Affine3 getS( const self& s ){
        e_assert( s.valid() );
        return Affine3( s );
      }

      /** \brief Construct scale matrix.
        *
        * This routine will create a scale matrix from the given components.
        *
        * \param x The X component of the scale.
        * \param y The Y component of the scale.
        * \param z The Z component of the scale.
        */

      void setScale( const self& x, const self& y, const self& z );

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

      e_forceinline void scale( const self& x, const self& y, const self& z ){
        *this *= getS( x, y, z );
      }

      /** \brief Add delta scale to 4x4 transform.
        *
        * This routine will scale the matrix by xyz.
        */

      e_forceinline void scale( const self& s ){
        *this *= getS( s, s, s );
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
        return Vector3( XX, YY, ZZ ).length();
      }

      /** \brief Remove scale.
        *
        * This routine will remove the scale from a copy of this transform and
        * return the result.
        *
        * \return Returns a copy of this transform with the scale removed.
        */

      e_forceinline Affine3 noscale()const{
        Affine3 M( *this );
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

      e_forceinline static Affine3 getR( const self& xEuler, const self& yEuler, const self& zEuler ){
        return Quaternion( xEuler, yEuler, zEuler ).get4x4();
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_noinline static Affine3 getR( const self& angleX, const self& angleY, const self& angleZ, const Order order ){
        Affine3 R;
        if( order == Order::kNone ){
          R = Quaternion( angleX, angleY, angleZ ).get3x4();
        }else{
          const Affine3& Rx = getRotX( angleX );
          const Affine3& Ry = getRotY( angleY );
          const Affine3& Rz = getRotZ( angleZ );
          switch( order ){
            case Order::XYZ:
              R = Rx * Ry * Rz;
              break;
            case Order::XZY:
              R = Rx * Rz * Ry;
              break;
            case Order::YZX:
              R = Ry * Rz * Rx;
              break;
            case Order::YXZ:
              R = Ry * Rx * Rz;
              break;
            case Order::ZXY:
              R = Rz * Rx * Ry;
              break;
            case Order::ZYX:
              R = Rz * Ry * Rx;
              break;
            default:
              break;
          }
        }
        return R;
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_noinline static Affine3 getR( const Vector3& v, const Order order ){
        Affine3 R;
        if( order == Order::kNone ){
          R = Quaternion( v ).get3x4();
        }else{
          const Affine3& Rx = getRotX( v.x );
          const Affine3& Ry = getRotY( v.y );
          const Affine3& Rz = getRotZ( v.z );
          switch( order ){
            case Order::XYZ:
              R = Rx * Ry * Rz;
              break;
            case Order::XZY:
              R = Rx * Rz * Ry;
              break;
            case Order::YZX:
              R = Ry * Rz * Rx;
              break;
            case Order::YXZ:
              R = Ry * Rx * Rz;
              break;
            case Order::ZXY:
              R = Rz * Rx * Ry;
              break;
            case Order::ZYX:
              R = Rz * Ry * Rx;
              break;
            default:
              break;
          }
        }
        return R;
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_forceinline static Affine3 getR( const Vector3& v ){
        return Quaternion( v ).get3x4();
      }

      /** \brief Get X rotation.
        *
        * This routine will return a rotation around X as an affine 3x4 matrix.
        */

      e_forceinline static Affine3 getRotX( const self& deg ){
        Affine3 R;
        R.setRotX( deg );
        return R;
      }

      /** \brief Construct rotation matrix about X axis.
        *
        * This routine will construct a rotation matrix.
        *
        * \param deg The angle in degrees.
        */

      void setRotX( const self& deg );

      /** \brief Get Y rotation.
        *
        * This routine will return a rotation around Y as an affine 3x4 matrix.
        */

      e_forceinline static Affine3 getRotY( const self& deg ){
        Affine3 R;
        R.setRotY( deg );
        return R;
      }

      /** \brief Construct rotation matrix about Y axis.
        *
        * This routine will construct a rotation matrix.
        *
        * \param deg The angle in degrees.
        */

      void setRotY( const self& deg );

      /** \brief Get Z rotation.
        *
        * This routine will return a rotation around Z as an affine 3x4 matrix.
        */

      e_forceinline static Affine3 getRotZ( const self& deg ){
        Affine3 R;
        R.setRotZ( deg );
        return R;
      }

      /** \brief Construct rotation matrix about Z axis.
        *
        * This routine will construct a rotation matrix.
        *
        * \param deg The angle in degrees.
        */

      void setRotZ( const self& deg );

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
        * \param z The Z translation.
        *
        * \return Returns a 4x4 matrix with translation set.
        */

      e_forceinline static Affine3 getT( const self& x, const self& y, const self& z ){
        Affine3 M;
        M.setTranslation( x, y, z );
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

      e_forceinline static Affine3 getT( const Vector3& v ){
        Affine3 M;
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

      e_forceinline static Affine3 getT( const Point3& p ){
        Affine3 M;
        M.setTranslation( p.x, p.y, p.z );
        return M;
      }

      /** \brief Add delta translation to 4x4 transform.
        *
        * This routine will translate the matrix by xyz.
        */

      e_forceinline void translate( const self& x, const self& y, const self& z ){
        *this *= getT( x, y, z );
      }

      /** \brief Construct translation matrix.
        *
        * This routine will load the matrix with just identity and scale.
        *
        * \param x The X component of translation.
        * \param y The Y component of translation.
        * \param z The Z component of translation.
        */

      void setTranslation( const self& x, const self& y, const self& z );

      /** \brief Remove translation.
        *
        * This routine will remove the translation from a copy of this
        * transform and return the result.
        *
        * \return Returns a copy of this transform with the translation
        * removed.
        */

      e_forceinline Affine3 noTranslation()const{
        Affine3 M( *this );
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

      e_forceinline Point3 toPosition()const{
        e_assert( valid() );
        return Point3( column( 3 ));
      }

      /** @}
        *
        * \name Matrix inverse methods.
        *
        * These methods do all the normal translation operations you'd expect
        * from an affine matrix.
        *
        * @{
        */

      /** \brief Return inverted matrix.
        *
        * This routine will clone and invert the matrix then return it.
        *
        * \return Returns an inverted matrix without destroying this.
        */

      Affine3 inverse()const;

      /** \brief Invert matrix.
        *
        * This routine will invert the matrix.
        */

      void invert();

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

      e_forceinline Vector3 column( const u32 i )const{
        e_assert( valid() );
        e_assert( i < 4, "Column does not exist" );
        return Vector3( a[0][i], a[1][i], a[2][i] );
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
        e_assert( i < 4, "Row does not exist" );
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

      /** \brief Get matrix up as vector.
        *
        * This routine will return the up of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 up()const{
        e_assert( valid() );
        return row( 2 );
      }

      /** @}
        *
        * \name Special edge case methods.
        *
        * These methods let you flip the YZ. Other strangeness goes here.
        *
        * @{
        */

      /** \brief Create lookat matrix.
        *
        * This routine will construct a look-at transform.
        *
        * \param at The position to look at.
        * \param eye The position of your virtual camera or eye point.
        *
        * \return Returns a new matrix representing the look-at vector.
        */

      e_noinline friend Affine3 e_lookat3x4( const Point3& at, const Point3& eye ){
        const Vector3& yaxis = Vector3( at - eye ).normalized();
        const Vector3& xaxis = Vector3::kUp.cross( yaxis ).normalized();
        const Vector3& zaxis = yaxis.cross( xaxis );
        return Affine3(
            xaxis.x, yaxis.y, zaxis.z, -xaxis.dot( eye ),
            xaxis.x, yaxis.y, zaxis.z, -yaxis.dot( eye ),
            xaxis.x, yaxis.y, zaxis.z, -zaxis.dot( eye )
        );
      }

      /** \brief Set identity matrix.
        *
        * This routine will clear the matrix to its identity.
        */

      e_forceinline void setIdentity(){
        *this = Affine3();
      }

      /** \brief Check if identity.
        *
        * This routine will check to see if this is an identity matrix.
        *
        * \param a The affine transform to check.
        * \return Returns true if the affine transform is identity or false.
        */

      e_forceinline friend bool e_isIdentity( const Affine3& a ){
        return a.isIdentity();
      }

      /** \brief Check if identity.
        *
        * This routine will check to see if this is an identity matrix.
        *
        * \return Returns true if the affine transform is identity or false.
        */

      e_forceinline bool isIdentity()const{
        return( *this == Affine3() );
      }

      /** \brief Set the matrix to zero.
        *
        * This routine will load every component of the matrix with zero.
        */

      e_forceinline void clear(){
        #ifdef __SSE__
          if( is_sse() ){
            v[0] = _mm_set1_ps( 0 );
            v[1] = v[0];
            v[2] = v[0];
            return;
          }
        #endif
        memset( t, 0, sizeof( t ));
      }

      /** \brief Get YZ flip transform.
        *
        * This routine returns a matrix that will flip the Y and Z axis. This
        * is used primarily by the CPU skinning code.
        *
        * \return Returns an affine transform that will flip Y and Z axis
        * when multiplied through another transform.
        */

      e_forceinline static Affine3 flipYZ(){
        Affine3 flipYZ;
        flipYZ.setZero();
        flipYZ[ 0] = 1.f;
        flipYZ[ 6] = 1.f;
        flipYZ[ 9] = 1.f;
        return flipYZ;
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

      e_forceinline friend Affine3 e_mul( const Affine3& A, const Affine3& B ){
        return A * B;
      }

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend Vector3 e_mul( const Affine3& M, const Vector3& V ){
        return M * V;
      }

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend Point3 e_mul( const Affine3& M, const Point3& P ){
        return M * P;
      }

      /** \brief Multiply two matrices together.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline friend AABB3 e_mul( const Affine3& A, const AABB3& B ){
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

      e_forceinline friend Quaternion e_toQuaternion( const Affine3& a ){
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

      e_forceinline friend Point3 e_toPoint( const Affine3& a ){
        return a.template as<Point3>();
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

    /** \brief By element constuctor.
      *
      * This constructor will build an affine transform from all 12 elements.
      */

    e_forceinline Affine3(
        const self& xx, const self& yx, const self& zx, const self& wx,
        const self& xy, const self& yy, const self& zy, const self& wy,
        const self& xz, const self& yz, const self& zz, const self& wz ){
      XX = xx; YX = yx; ZX = zx; WX = wx;
      XY = xy; YY = yy; ZY = zy; WY = wy;
      XZ = xz; YZ = yz; ZZ = zz; WZ = wz;
      e_assert( valid() );
    }

    /** \brief By element constuctor.
      *
      * This constructor will build an affine transform from all 12 elements.
      */

    constexpr Affine3(
        const float xx, const float yx, const float zx, const float wx,
        const float xy, const float yy, const float zy, const float wy,
        const float xz, const float yz, const float zz, const float wz )
      : m00( xx ), m10( yx ), m20( zx ), m30( wx )
      , m01( xy ), m11( yy ), m21( zy ), m31( wy )
      , m02( xz ), m12( yz ), m22( zz ), m32( wz )
    {}

    e_forceinline Affine3( const Matrix4& M ){
      // We load transposed because layout is transposed.
      XX=M.XX; YX=M.YX; ZX=M.ZX; WX=M.WX;
      XY=M.XY; YY=M.YY; ZY=M.ZY; WY=M.WY;
      XZ=M.XZ; YZ=M.YZ; ZZ=M.ZZ; WZ=M.WZ;
      e_assert( valid() );
    }

    e_forceinline Affine3( const Affine3& M ){
      #ifdef __SSE__
        v[ 0 ]=M.v[ 0 ];
        v[ 1 ]=M.v[ 1 ];
        v[ 2 ]=M.v[ 2 ];
      #else
        XX=M.XX; YX=M.YX; ZX=M.ZX; WX=M.WX;
        XY=M.XY; YY=M.YY; ZY=M.ZY; WY=M.WY;
        XZ=M.XZ; YZ=M.YZ; ZZ=M.ZZ; WZ=M.WZ;
      #endif
      e_assert( valid() );
    }

    e_forceinline Affine3( const self& scale ){
      setScale( scale );
      e_assert( valid() );
    }

    constexpr Affine3( const float s )
      : m00( s ), m10( 0 ), m20( 0 ), m30( 0 )
      , m01( 0 ), m11( s ), m21( 0 ), m31( 0 )
      , m02( 0 ), m12( 0 ), m22( s ), m32( 0 )
    {}

    constexpr Affine3()
      : m00( 1 ), m10( 0 ), m20( 0 ), m30( 0 )
      , m01( 0 ), m11( 1 ), m21( 0 ), m31( 0 )
      , m02( 0 ), m12( 0 ), m22( 1 ), m32( 0 )
    {}

    union{
      struct{
        float m00, m10, m20, m30;
        float m01, m11, m21, m31;
        float m02, m12, m22, m32;
      };
      struct{
        self XX, YX, ZX, WX;
        self XY, YY, ZY, WY;
        self XZ, YZ, ZZ, WZ;
      };
      #ifdef __SSE__
        __m128 v[3];
      #endif
      Vector4 a[3];
      self    m[3][4];
      self    t[12];
    };
  };

//}:                                              |
//================================================+=============================
