//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Matrix4:{                                       |

  /** \brief Homogeneous matrix.
    *
    * This Matrix4 class defines a row major transform for use with OpenGL
    * and DirectX. The memory layout is compatible with both.
    *
    * Memory layout
    * +--         --+   +--        --+
    * | XX XY XZ XW |   | RR RR RR 0 |
    * | YX YY YZ YW | = | RR RR RR 0 |
    * | ZX ZY ZZ ZW | = | RR RR RR 0 |
    * | WX WY WZ WW |   | Tx Ty Tz 1 |
    * +--         --+   +--        --+
    */

  struct alignas(16) E_PUBLISH Matrix4 final{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |

      /** \brief Rotation order.
        *
        * This enum class defines the rotation order that a rotation matrix can
        * be constructed from.
        */

      enum class Order:u32{
        kNone,
        XYZ,
        XZY,
        YZX,
        YXZ,
        ZXY,
        ZYX,
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

        /** \brief Homogeneous matrix assignment operator.
          *
          * This operator quickly assigns one homogeneous matrix to another
          * using the SIMD type if possible.
          *
          * \param M The matrix to assign to this one.
          *
          * \return Returns the dereferenced this pointer.
          */

        e_forceinline Matrix4& operator=( const Matrix4& M ){
          #ifdef __SSE__
            v[0] = M.v[0];
            v[1] = M.v[1];
            v[2] = M.v[2];
            v[3] = M.v[3];
          #else
            memcpy( t, M.t, sizeof( t ));
          #endif
          return *this;
        }

      //}:                                        |
      //Equality:{                                |

        /** \name Equality operators.
          *
          * These methods compare two homogeneous matrices for equality and
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

        e_forceinline bool operator==( const Matrix4& M )const{
          #if __SSE__
            for( u32 i=0; i<4; ++i ){
              const __m128i c = __m128i( _mm_cmpneq_ps( v[ i ], M.v[ i ]));
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
            return( 0 == memcmp( this, &M, sizeof( Matrix4 )));
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

        e_forceinline bool operator!=( const Matrix4& M )const{
          #ifdef __SSE__
            for( u32 i=0; i<4; ++i ){
              const __m128 c = _mm_cmpneq_ps( v[ i ], M.v[ i ]);
              const u16 r = _mm_movemask_ps( c );
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
            return( 0 != memcmp( this, &M, sizeof( Matrix4 )));
          #endif
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
      //Multiply:{                                |

        /** @}
          *
          * \name Multiplication operators.
          *
          * These operators multiply the homogeneous matrix by various other
          * types, such as vectors and points.
          *
          * @{
          */

        /** \brief Multiplication operator.
          *
          * This routine will multiply two homogeneous matrices together.
          *
          * \param A The input matrix to multiply.
          *
          * \return Returns the transformed matrix.
          */

        e_forceinline Matrix4 operator*( const Matrix4& A )const{
          Matrix4 M = *this;
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

        e_forceinline Matrix4 operator*( const self& s )const{
          Matrix4 M = *this;
          M *= s;
          return M;
        }

        /** \brief Multiplication operator.
          *
          * This operator multiplies this matrix by the input and updates this.
          *
          * \param M The input matrix to multiply against.
          *
          * \return Returns the this pointer dereferenced.
          */

        Matrix4& operator*=( const Matrix4& M );

        /** \brief Multiplication operator.
          *
          * This operator multiplies the matrix by a scalar.
          *
          * \param s The scalar to multiply this matrix by.
          *
          * \return Returns the scaled matrix.
          */

        e_forceinline Matrix4& operator*=( const self& s ){
          #ifdef __SSE__
            const __m128 x = _mm_set1_ps( f32::cast( s ));
            v[ 0 ]=_mm_mul_ps( v[ 0 ], x );
            v[ 1 ]=_mm_mul_ps( v[ 1 ], x );
            v[ 2 ]=_mm_mul_ps( v[ 2 ], x );
            v[ 3 ]=_mm_mul_ps( v[ 3 ], x );
          #else
            XX *= s; XY *= s; XZ *= s; XW *= s;
            YX *= s; YY *= s; YZ *= s; YW *= s;
            ZX *= s; ZY *= s; ZZ *= s; ZW *= s;
            WX *= s; WY *= s; WZ *= s; WW *= s;
          #endif
          return *this;
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

        e_forceinline Matrix4 operator-( const Matrix4& b )const{
          #ifdef __SSE__
            Matrix4 a;
            a.v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
            a.v[ 2 ]=_mm_sub_ps( v[ 2 ], b.v[ 2 ]);
            a.v[ 3 ]=_mm_sub_ps( v[ 3 ], b.v[ 3 ]);
            return a;
          #else
            return Matrix4(
              XX-b.XX, XY-b.XY, XZ-b.XZ, XW-b.XW,
              YX-b.YX, YY-b.YY, YZ-b.YZ, YW-b.YW,
              ZX-b.ZX, ZY-b.ZY, ZZ-b.ZZ, ZW-b.ZW,
              WX-b.WX, WY-b.WY, WZ-b.WZ, WW-b.WW );
          #endif
        }

        e_forceinline Matrix4& operator-=( const Matrix4& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_sub_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_sub_ps( v[ 1 ], b.v[ 1 ]);
            v[ 2 ]=_mm_sub_ps( v[ 2 ], b.v[ 2 ]);
            v[ 3 ]=_mm_sub_ps( v[ 3 ], b.v[ 3 ]);
          #else
            *this = Matrix4(
              XX-b.XX, XY-b.XY, XZ-b.XZ, XW-b.XW,
              YX-b.YX, YY-b.YY, YZ-b.YZ, YW-b.YW,
              ZX-b.ZX, ZY-b.ZY, ZZ-b.ZZ, ZW-b.ZW,
              WX-b.WX, WY-b.WY, WZ-b.WZ, WW-b.WW );
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

        e_forceinline Matrix4 operator+( const Matrix4& b )const{
          #ifdef __SSE__
            Matrix4 a;
            a.v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            a.v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
            a.v[ 2 ]=_mm_add_ps( v[ 2 ], b.v[ 2 ]);
            a.v[ 3 ]=_mm_add_ps( v[ 3 ], b.v[ 3 ]);
            return a;
          #else
            return Matrix4(
              XX+b.XX, XY+b.XY, XZ+b.XZ, XW+b.XW,
              YX+b.YX, YY+b.YY, YZ+b.YZ, YW+b.YW,
              ZX+b.ZX, ZY+b.ZY, ZZ+b.ZZ, ZW+b.ZW,
              WX+b.WX, WY+b.WY, WZ+b.WZ, WW+b.WW );
          #endif
        }

        e_forceinline Matrix4& operator+=( const Matrix4& b ){
          #ifdef __SSE__
            v[ 0 ]=_mm_add_ps( v[ 0 ], b.v[ 0 ]);
            v[ 1 ]=_mm_add_ps( v[ 1 ], b.v[ 1 ]);
            v[ 2 ]=_mm_add_ps( v[ 2 ], b.v[ 2 ]);
            v[ 3 ]=_mm_add_ps( v[ 3 ], b.v[ 3 ]);
          #else
            *this = Matrix4(
              XX+b.XX, XY+b.XY, XZ+b.XZ, XW+b.XW,
              YX+b.YX, YY+b.YY, YZ+b.YZ, YW+b.YW,
              ZX+b.ZX, ZY+b.ZY, ZZ+b.ZZ, ZW+b.ZW,
              WX+b.WX, WY+b.WY, WZ+b.WZ, WW+b.WW );
          #endif
          return *this;
        }

      //}:                                        |
    //}:                                          |
    //Methods:{                                   |

      /** @}
        *
        * \name Matrix methods.
        *
        * These methods do all the normal operations you'd expect from a
        * homogeneous matrix.
        *
        * @{
        */

      e_noinline bool valid()const{
        if( !XX.valid() ){
          return false;
        }
        if( !XY.valid() ){
          return false;
        }
        if( !XZ.valid() ){
          return false;
        }
        if( !XW.valid() ){
          return false;
        }
        if( !YX.valid() ){
          return false;
        }
        if( !YY.valid() ){
          return false;
        }
        if( !YZ.valid() ){
          return false;
        }
        if( !YW.valid() ){
          return false;
        }
        if( !ZX.valid() ){
          return false;
        }
        if( !ZY.valid() ){
          return false;
        }
        if( !ZZ.valid() ){
          return false;
        }
        if( !ZW.valid() ){
          return false;
        }
        if( !WX.valid() ){
          return false;
        }
        if( !WY.valid() ){
          return false;
        }
        if( !WZ.valid() ){
          return false;
        }
        if( !WW.valid() ){
          return false;
        }
        return true;
      }

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

      e_forceinline static Matrix4 getS( const self& x, const self& y, const self& z ){
        Matrix4 M;
        M.setScale( x, y, z );
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

      e_forceinline static Matrix4 getS( const Vector3& s ){
        Matrix4 M;
        M.setScale( s.x, s.y, s.z );
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

      e_forceinline static Matrix4 getS( const self& s ){
        return Matrix4( s );
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_forceinline static Matrix4 getR( const self& xEuler, const self& yEuler, const self& zEuler ){
        return Quaternion( xEuler, yEuler, zEuler ).get4x4();
      }

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_noinline static Matrix4 getR( const self& angleX, const self& angleY, const self& angleZ, const Order order ){
        Matrix4 R;
        if( order == Order::kNone ){
          R = Quaternion( angleX, angleY, angleZ ).get4x4();
        }else{
          const Affine3& Rx = getRotX( angleX );
          const Affine3& Ry = getRotY( angleY );
          const Affine3& Rz = getRotZ( angleZ );
          switch( order ){
            case Order::XYZ:
              R = Rz * Ry * Rx;
              break;
            case Order::XZY:
              R = Ry * Rz * Rx;
              break;
            case Order::YZX:
              R = Rx * Rz * Ry;
              break;
            case Order::YXZ:
              R = Rz * Rx * Ry;
              break;
            case Order::ZXY:
              R = Ry * Rx * Rz;
              break;
            case Order::ZYX:
              R = Rx * Ry * Rz;
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

      e_noinline static Matrix4 getR( const Vector3& v, const Order order ){
        Matrix4 R;
        if( order == Order::kNone ){
          R = Quaternion( v ).get4x4();
        }else{
          const Matrix4& Rx = getRotX( v.x );
          const Matrix4& Ry = getRotY( v.y );
          const Matrix4& Rz = getRotZ( v.z );
          switch( order ){
            case Order::XYZ:
              R = Rz * Ry * Rx;
              break;
            case Order::XZY:
              R = Ry * Rz * Rx;
              break;
            case Order::YZX:
              R = Rx * Rz * Ry;
              break;
            case Order::YXZ:
              R = Rz * Rx * Ry;
              break;
            case Order::ZXY:
              R = Ry * Rx * Rz;
              break;
            case Order::ZYX:
              R = Rx * Ry * Rz;
              break;
            default:
              break;
          }
        }
        return R;
      }

      /** \brief Get X rotation.
        *
        * This routine will return a rotation around X as an affine 3x4 matrix.
        */

      e_forceinline static Matrix4 getRotX( const self& deg ){
        Matrix4 R;
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

      static Matrix4 getRotY( const self& deg ){
        Matrix4 R;
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

      static Matrix4 getRotZ( const self& deg ){
        Matrix4 R;
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

      /** \brief Get rotation matrix.
        *
        * This routine will return a rotation matrix from the given vector.
        */

      e_forceinline static Matrix4 getR( const Vector3& v ){
        return Quaternion( v ).get4x4();
      }

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

      e_forceinline static Matrix4 getT( const self& x, const self& y, const self& z ){
        Matrix4 M;
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

      e_forceinline static Matrix4 getT( const Vector3& v ){
        Matrix4 M;
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

      e_forceinline static Matrix4 getT( const Point3& p ){
        Matrix4 M;
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

      /** \brief Remove translation from matrix.
        *
        * This routine will zero out the translation row.
        */

      void removeTranslation();

      /** \brief Remove translation.
        *
        * This routine will remove the translation from a copy of this
        * transform and return the result.
        *
        * \return Returns a copy of this transform with the translation
        * removed.
        */

      e_forceinline Matrix4 noTranslation()const{
        Matrix4 M( *this );
        M.removeTranslation();
        return M;
      }

      /** \brief Remove scale from matrix.
        *
        * This routine will remove the scale from this matrix. When converting
        * this 4x4 to a 3x3 transform you should always call removeScale()
        * first.
        */

      void removeScale();

      /** \brief Remove scale.
        *
        * This routine will remove the scale from a copy of this transform and
        * return the result.
        *
        * \return Returns a copy of this transform with the scale removed.
        */

      e_forceinline Matrix4 noscale()const{
        Matrix4 M( *this );
        M.removeScale();
        return M;
      }

      /** \brief Transpose matrix.
        *
        * This routine will transpose the matrix.
        */

      void transpose();

      /** \brief Transpose matrix.
        *
        * This routine will clone the matrix, transpose it then return the
        * result.
        *
        * \return Returns the transposition of this matrix without destroying
        * it.
        */

      e_forceinline Matrix4 transposed()const{
        Matrix4 M( *this );
        M.transpose();
        return M;
      }

      /** \brief Invert matrix.
        *
        * This routine will invert the matrix.
        */

      void invert();

      /** \brief Return inverted matrix.
        *
        * This routine will clone and invert the matrix then return it.
        *
        * \return Returns an inverted matrix without destroying this.
        */

      Matrix4 inverse()const;

      /** \brief Get matrix column.
        *
        * This routine will grab the given column by index.
        *
        * \param i The column index, must be 0 to 3.
        *
        * \return Returns the column indexed by i.
        */

      e_forceinline Vector4 column( const u32 i )const{
        return Vector4( a[0][i], a[1][i], a[2][i], a[3][i] );
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
        return a[i];
      }

      /** \brief Get matrix forward as vector.
        *
        * This routine will return the forward of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 forward()const{
        return column( 0 ).xyz();
      }

      /** \brief Get matrix right as vector.
        *
        * This routine will return the right of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 right()const{
        return column( 1 ).xyz();
      }

      /** \brief Get matrix up as vector.
        *
        * This routine will return the up of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 up()const{
        return column( 2 ).xyz();
      }

      /** \brief Get matrix translation.
        *
        * This routine will return the matrix translation as a position.
        */

      e_forceinline Point3 toPosition()const{
        return Point3( a[ 3 ]);
      }

      /** \brief Get YZ flip transform.
        *
        * This routine returns a matrix that will flip the Y and Z axis. This
        * is used primarily by the CPU skinning code.
        *
        * \return Returns a homogeneous transform that will flip Y and Z axis
        * when multiplied through another transform.
        */

      e_forceinline static Matrix4 flipYZ(){
        Matrix4 flipYZ;
        flipYZ.setZero();
        flipYZ[ 0] = 1.f;
        flipYZ[ 6] = 1.f;
        flipYZ[ 9] = 1.f;
        flipYZ[15] = 1.f;
        return flipYZ;
      }

        /** \brief Rotation matrix assignment operator.
          *
          * This operator quickly assigns one rotation matrix to a homgeneous
          * one using the SIMD type if possible.
          *
          * \param M The matrix to assign to this one.
          *
          * \return Returns the dereferenced this pointer.
          */

        e_forceinline Matrix4& set3x3( const Matrix3& M ){
          for( u32 r=0; r<3; ++r ){
            for( u32 c=0; c<3; ++c ){
              m[ r ][ c ] = M( r, c );
            }
          }
          return *this;
        }

      /** \brief Multiply two matrices.
        *
        * This routine will multiply two matrices together.
        */

      e_forceinline void mul( const Matrix4& M ){
        *this *= M;
      }

      /** @}
        *
        * \name Matrix setters.
        *
        * These routines will load the matrix with everything from a rotation
        * transform to a projection one.
        *
        * @{
        */

      /** \brief Construct orthographic matrix.
        *
        * This routine will load the matrix with an orthgraphic V2C transform.
        *
        * \param left The left edge of the orthographic bounds.
        * \param top The top edge of the bounds.
        * \param right The right edge of the bounds.
        * \param bottom The bottom edge of the bounds.
        * \param zNear The near plane.
        * \param zFar The far plane.
        */

      e_noinline void setOrthographicV2C( const self& left, const self& top, const self& right, const self& bottom, const self& zNear, const self& zFar ){
        Matrix4 M;
        M( 0, 0 ) =  k2 / (right - left);
        M( 1, 1 ) =  k2 / (top - bottom);
        M( 2, 2 ) = -k2 / (zFar - zNear);
        M( 0, 3 ) = -(right + left) / (right - left);
        M( 1, 3 ) = -(top + bottom) / (top - bottom);
        M( 2, 3 ) = -(zFar + zNear) / (zFar - zNear);
        *this = M;
      }

      /** \brief Construct orthographic matrix.
        *
        * This routine will load the matrix with an orthgraphic V2C transform.
        *
        * \param w The width of the input bounds.
        * \param h The height of the input bounds.
        */

      e_noinline void setOrthographicV2C( const self& w, const self& h, const self& nearZ, const self& farZ ){
        setZero();
        XX = -(2 / w);
        YY = -(2 / h);
        ZZ = 1 / (nearZ - farZ);
        WZ = nearZ / (nearZ - farZ);
        WW = 1;
      }

      /** \brief Set as perspective V2C matrix.
        *
        * This routine will establish this homogenious matrix as a perspective
        * correct view to world transform.
        */

      void setPerspectiveV2C( const self& fov, const self& aspect, const self& nearZ, const self& farZ );

      /** \brief Construct translation matrix.
        *
        * This routine will load the matrix with just identity and scale.
        *
        * \param x The X component of translation.
        * \param y The Y component of translation.
        * \param z The Z component of translation.
        */

      void setTranslation( const self& x, const self& y, const self& z );

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

      /** \brief Set the matrix to zero.
        *
        * This routine will load every component of the matrix with zero.
        */

      e_forceinline void setZero(){
        #ifdef __SSE__
          if( is_sse() ){
            v[0] = _mm_set1_ps( 0 );
            v[1] = v[0];
            v[2] = v[0];
            v[3] = v[0];
            return;
          }
        #endif
        memset( t, 0, sizeof( t ));
      }

      /** \brief Set identity matrix.
        *
        * This routine will clear the matrix to its identity.
        */

      e_forceinline void setIdentity(){
        YX = ZX = WX = XY = ZY = WY = 0;
        XZ = YZ = WZ = XW = YW = ZW = 0;
        XX = YY = ZZ = WW = 1;
      }

      /** \brief Create an axis identity matrix.
        *
        * This identity matrix will swap the Z and Y axis so that we get the
        * engine's native +X right, +Y forward and +Z up.
        */

      void setAxisIdentity();

      /** \brief Cast 4x4 matrix to lower type.
        *
        * This routine will convert the 4x4 transform to a 3x3.
        */

      template<typename Ty> Ty as()const;

      /** @}
        *
        * \name Other methods.
        *
        * These methods have to do with lookat and uniform scale etc.
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

      e_noinline static Matrix4 lookat( const Point3& at, const Point3& eye ){
        const Vector3 yaxis = Vector3( at - eye ).normalized();
        const Vector3 xaxis = Vector3::kUp.cross( yaxis ).normalized();
        const Vector3 zaxis = yaxis.cross( xaxis );
        return Matrix4(
            xaxis.x,          yaxis.x,          zaxis.x,        0.f,
            xaxis.y,          yaxis.y,          zaxis.y,        0.f,
            xaxis.z,          yaxis.z,          zaxis.z,        0.f,
          - xaxis.dot( eye ), - yaxis.dot( eye ), - zaxis.dot( eye ), 1.f );
      }

      /** \brief Set uniform scale.
        *
        * This routine will return the uniform scale of the matrix by treating
        * the scale components as a vector and taking the length.
        *
        * \return Returns a real number representing the uniform scale.
        */

      e_forceinline self uniformScale()const{
        return Vector4( XX, YY, ZZ, WW ).length();
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
        * \return Returns a rotation expressed as a quaternion.
        */

      e_noinline Quaternion asQuaternion()const{
        //http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
        self qw, qx, qy, qz;
        const self tr = m[0][0] + m[1][1] + m[2][2];
        if( tr > k0 ){
          const self& s = sqrt( tr+1.f )*k2; // s=4*qw
          qw = .25 * s;
          qx = (m[2][1] - m[1][2]) / s;
          qy = (m[0][2] - m[2][0]) / s;
          qz = (m[1][0] - m[0][1]) / s;
        }else if(( m[0][0] > m[1][1] ) & ( m[0][0] > m[2][2] )){
          const self& s = sqrt( 1.f + m[0][0] - m[1][1] - m[2][2] ) * k2; // s=4*qx
          qw = (m[2][1] - m[1][2]) / s;
          qx = .25 * s;
          qy = (m[0][1] + m[1][0]) / s;
          qz = (m[0][2] + m[2][0]) / s;
        }else if( m[1][1] > m[2][2] ){
          const self& s = sqrt( 1.f + m[1][1] - m[0][0] - m[2][2] ) * k2; // S=4*qy
          qw = (m[0][2] - m[2][0]) / s;
          qx = (m[0][1] + m[1][0]) / s;
          qy = .25 * s;
          qz = (m[1][2] + m[2][1]) / s;
        }else{
          const self& s = sqrt( 1.f + m[2][2] - m[0][0] - m[1][1] ) * k2; // S=4*qz
          qw = (m[1][0] - m[0][1]) / s;
          qx = (m[0][2] + m[2][0]) / s;
          qy = (m[1][2] + m[2][1]) / s;
          qz = .25 * s;
        }
        return Quaternion( qx, qy, qz, qw );
      }

      /** @} */

    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline_always Matrix4(
        const self& xx, const self& xy, const self& xz, const self& xw,
        const self& yx, const self& yy, const self& yz, const self& yw,
        const self& zx, const self& zy, const self& zz, const self& zw,
        const self& wx, const self& wy, const self& wz, const self& ww ){
      XX = xx; XY = xy; XZ = xz; XW = xw;
      YX = yx; YY = yy; YZ = yz; YW = yw;
      ZX = zx; ZY = zy; ZZ = zz; ZW = zw;
      WX = wx; WY = wy; WZ = wz; WW = ww;
    }

    e_forceinline_always Matrix4(
        const float xx, const float xy, const float xz, const float xw,
        const float yx, const float yy, const float yz, const float yw,
        const float zx, const float zy, const float zz, const float zw,
        const float wx, const float wy, const float wz, const float ww )
      : m00( xx ), m01( xy ), m02( xz ), m03( xw )
      , m10( yx ), m11( yy ), m12( yz ), m13( yw )
      , m20( zx ), m21( zy ), m22( zz ), m23( zw )
      , m30( wx ), m31( wy ), m32( wz ), m33( ww )
    {}

    e_forceinline_always Matrix4( const Matrix4& M ){
      #ifdef __SSE__
        v[ 0 ]=M.v[ 0 ];
        v[ 1 ]=M.v[ 1 ];
        v[ 2 ]=M.v[ 2 ];
        v[ 3 ]=M.v[ 3 ];
      #else
        XX=M.XX; XY=M.XY; XZ=M.XZ; XW=M.XW;
        YX=M.YX; YY=M.YY; YZ=M.YZ; YW=M.YW;
        ZX=M.ZX; ZY=M.ZY; ZZ=M.ZZ; ZW=M.ZW;
        WX=M.WX; WY=M.WY; WZ=M.WZ; WW=M.WW;
      #endif
    }

    e_forceinline_always Matrix4( const self& scale ){
      setScale( scale );
    }

    e_forceinline_always Matrix4( const Affine3& affine ){
      for( u32 r=0; r<3; ++r ){
        for( u32 c=0; c<4; ++c ){
          m[r][c] = affine( r, c );
        }
      }
      m30 = m31 = m32 = 0;
      m33 = 1;
      transpose();
    }

    e_forceinline_always Matrix4( const float s )
      : m00( s ), m01( 0 ), m02( 0 ), m03( 0 )
      , m10( 0 ), m11( s ), m12( 0 ), m13( 0 )
      , m20( 0 ), m21( 0 ), m22( s ), m23( 0 )
      , m30( 0 ), m31( 0 ), m32( 0 ), m33( 1 )
    {}

    e_forceinline_always Matrix4()
      : m00( 1 ), m01( 0 ), m02( 0 ), m03( 0 )
      , m10( 0 ), m11( 1 ), m12( 0 ), m13( 0 )
      , m20( 0 ), m21( 0 ), m22( 1 ), m23( 0 )
      , m30( 0 ), m31( 0 ), m32( 0 ), m33( 1 )
    {}

    union{
      struct{
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
      };
      struct{
        self XX, XY, XZ, XW;
        self YX, YY, YZ, YW;
        self ZX, ZY, ZZ, ZW;
        self WX, WY, WZ, WW;
      };
      #ifdef __SSE__
        __m128 v[4];
      #endif
      Vector4 a[4];
      RGBa    c[4];
      self    m[4][4];
      self    t[16];
    };
  };

//}:                                              |
//================================================+=============================