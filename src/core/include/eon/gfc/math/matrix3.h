//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Matrix3:{                                       |

  /** \brief 3x3 rotation matrix.
    *
    * This structure defines the unaligned 3x3 rotation matrix.
    */

  struct E_PUBLISH Matrix3 final{

    //--------------------------------------------+-----------------------------
    //Operate:{                                   |

      /** \name Indexing operators.
        *
        * These operators index into the matrix in various ways in both its
        * const and non-const forms.
        *
        * @{
        */

      /** \brief Parenthesis operator.
        *
        * This operator returns a const reference to the tuple at the given row
        * and column.
        *
        * \param r The row of the tuple to return.
        * \param c The column of the tuple to return.
        *
        * \return Returns a real number representing the indxed tuple.
        */

      e_forceinline const self& operator()( const s32 r, const s32 c )const{
        return m[r][c];
      }

      /** \brief Parenthesis operator.
        *
        * This operator returns a reference to the tuple at the given row and
        * column.
        *
        * \param r The row of the tuple to return.
        * \param c The column of the tuple to return.
        *
        * \return Returns a real number representing the indxed tuple.
        */

      e_forceinline self& operator()( const s32 r, const s32 c ){
        return m[r][c];
      }

      /** \brief Bracket operator.
        *
        * This operator will return the tuple indexed by a single integer. This
        * will treat the matrix like a flat array of nine tuples.
        *
        * \param i The integer between 0 and 8 that gives the index into the
        * matrix as if it were an array.
        *
        * \return Returns a reference to the tuple indexed.
        */

      e_forceinline const self& operator[]( const s32 i )const{
        return t[i];
      }

      /** \brief Bracket operator.
        *
        * This operator will return the tuple indexed by a single integer. This
        * will treat the matrix like a flat array of nine tuples.
        *
        * \param i The integer between 0 and 8 that gives the index into the
        * matrix as if it were an array.
        *
        * \return Returns a reference to the tuple indexed.
        */

      e_forceinline self& operator[]( const s32 i ){
        return t[i];
      }

      /** @}
        *
        * \name Assignment operators.
        *
        * These operators assign the incoming matrix to this one.
        *
        * @{
        */

      e_forceinline Matrix3& operator=( const Matrix4& M ){
        for( u32 r=0; r<3; ++r ){
          for( u32 c=0; c<3; ++c ){
            m[r][c] = M.m[r][c];
          }
        }
        return *this;
      }

      e_forceinline Matrix3& operator=( const Matrix3& M ){
        for( u32 r=0; r<3; ++r ){
          for( u32 c=0; c<3; ++c ){
            m[r][c] = M.m[r][c];
          }
        }
        return *this;
      }

      /** @}
        *
        * \name Multiplication operators.
        *
        * These operators multiply the matrix by an incoming point or vector.
        *
        * @{
        */

      /** \brief Matrix multiplication operator.
        *
        * This operator will multiply two 3x3 matrices together.
        *
        * \param M The matrix to multiply against.
        *
        * \return Returns the transformed matrix.
        */

      e_forceinline Matrix3 operator*( const Matrix3& M )const{
        Matrix3 result = *this;
        result *= M;
        return result;
      }

      /** \brief Matrix multiplication operator.
        *
        * This operator will multiply two 3x3 matrices together and assign the
        * result to this object.
        *
        * \param B The matrix to multiply against.
        *
        * \return Returns *this.
        */

      e_noinline Matrix3& operator*=( const Matrix3& B ){
        Matrix3 A = *this;
        for( u32 i=0; i<3; ++i ){
          for( u32 j=0; j<3; ++j ){
            self sum = 0;
            for( u32 e=0; e<3; ++e ){
              sum += A.m[i][e] * B.m[e][j];
            }
            m[i][j] = sum;
          }
        }
        return *this;
      }

      /** @} */

    //}:                                          |
    //Methods:{                                   |

      /** \name Multiplication methods.
        *
        * These methods perform multiplication of this matrix with another
        * type.
        *
        * @{
        */

      /** \brief Check if matrix is valid.
        *
        * This routin will check the matrix to make sure it's numerically
        * valid.
        */

      e_forceinline friend bool e_valid( const Matrix3& m ){
        return m.valid();
      }

      /** \brief Check if matrix is valid.
        *
        * This routin will check the matrix to make sure it's numerically
        * valid.
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
        if( !YX.valid() ){
          return false;
        }
        if( !YY.valid() ){
          return false;
        }
        if( !YZ.valid() ){
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
        return true;
      }

      /** \brief Transform vector method.
        *
        * This routine will multiply the incoming vector by the transform this
        * and return the new vector.
        *
        * \param M The input matrix.
        * \param V The input vector to transform.
        *
        * \return Returns a new transformed vector.
        */

      e_forceinline friend Vector3 e_mul( const Matrix3& M, const Vector3& V ){
        return M.mul( V );
      }

      /** \brief Transform vector method.
        *
        * This routine will multiply the incoming vector by the transform this
        * and return the new vector.
        *
        * \param V The input vector to transform.
        *
        * \return Returns a new transformed vector.
        */

      e_forceinline Vector3 mul( const Vector3& V )const{
        return (*this) * V;
      }

      /** \brief Transform point method.
        *
        * This routine will multiply the incoming point by the transform this
        * and return the new point.
        *
        * \param M The input matrix.
        * \param P The input point to transform.
        *
        * \return Returns a new transformed point.
        */

      e_forceinline friend Point3 e_mul( const Matrix3& M, const Point3& P ){
        return M.mul( P );
      }

      /** \brief Transform point method.
        *
        * This routine will multiply the incoming point by the transform this
        * and return the new point.
        *
        * \param V The input point to transform.
        *
        * \return Returns a new transformed point.
        */

      e_forceinline Point3 mul( const Point3& V )const{
        return (*this) * V;
      }

      /** @}
        *
        * \name Set rotation methods.
        *
        * These methods set the matrix rotation completely in various ways.
        *
        * @{
        */

      /** \brief Get matrix column.
        *
        * This routine will grab the given column by index.
        *
        * \param M The input matrix.
        * \param i The column index, must be 0 to 3.
        *
        * \return Returns the column indexed by i.
        */

      e_forceinline friend Vector3 e_column( const Matrix3& M, const u32 i ){
        return M.column( i );
      }

      /** \brief Get matrix column.
        *
        * This routine will grab the given column by index.
        *
        * \param i The column index, must be 0 to 3.
        *
        * \return Returns the column indexed by i.
        */

      e_forceinline Vector3 column( const u32 i )const{
        e_assert( i < 3, "Column does not exist" );
        return Vector3( a[0][i], a[1][i], a[2][i] );
      }

      /** \brief Get matrix row.
        *
        * This routine will grab the given row by index.
        *
        * \param M The input matrix.
        * \param i The row index, must be 0 to 3.
        *
        * \return Returns the row indexed by i.
        */

      e_forceinline friend Vector3 e_row( const Matrix3& M, const u32 i ){
        return M.row( i );
      }

      /** \brief Get matrix row.
        *
        * This routine will grab the given row by index.
        *
        * \param i The row index, must be 0 to 3.
        *
        * \return Returns the row indexed by i.
        */

      e_forceinline Vector3 row( const u32 i )const{
        e_assert( i < 3, "Column does not exist" );
        return a[i];
      }

      /** \brief Get matrix forward as vector.
        *
        * This routine will return the forward of the matrix as a four-tuple
        * vector.
        *
        * \param M The input matrix.
        */

      e_forceinline friend Vector3 e_forward_axis( const Matrix3& M ){
        return M.forward();
      }

      /** \brief Get matrix forward as vector.
        *
        * This routine will return the forward of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 forward()const{
        return column( 1 ).xyz();
      }

      /** \brief Get matrix right as vector.
        *
        * This routine will return the right of the matrix as a four-tuple
        * vector.
        *
        * \param M The input matrix.
        */

      e_forceinline friend Vector3 e_right_axis( const Matrix3& M ){
        return M.right();
      }

      /** \brief Get matrix right as vector.
        *
        * This routine will return the right of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 right()const{
        return column( 0 ).xyz();
      }

      /** \brief Get matrix up as vector.
        *
        * This routine will return the up of the matrix as a four-tuple
        * vector.
        *
        * \param M The input matrix.
        */

      e_forceinline friend Vector3 e_up_axis( const Matrix3& M ){
        return M.up();
      }

      /** \brief Get matrix up as vector.
        *
        * This routine will return the up of the matrix as a four-tuple
        * vector.
        */

      e_forceinline Vector3 up()const{
        return column( 2 ).xyz();
      }

      /** \brief Set rotation from axis and angle.
        *
        * This routine will establish a rotation from an axis and angle in
        * degrees.
        *
        * \param axis The axis to apply rotation on.
        * \param angleInDeg The angle in degrees.
        */

      void setAxisAngle( const Vector3& axis, const self& angleInDeg );

      /** \brief Set the rotation scale.
        *
        * This routine will set the scale of the rotation by axis.
        *
        * \param x The X axis scale.
        * \param y The Y axis scale.
        * \param z The Z axis scale.
        */

      void setScale( const self& x, const self& y, const self& z );

      /** \brief Set the rotation scale.
        *
        * This routine will set the scale of the rotation by axis.
        *
        * \param x The X axis scale.
        */

      void setScale( const self& x );

      /** \brief Set the matrix to rotation.
        *
        * This routine will establish a new rotation matrix around the X axis.
        *
        * \param angleInDeg The angle of rotation in degrees.
        */

      void setRotX( const self& angleInDeg );

      /** \brief Set the matrix to rotation.
        *
        * This routine will establish a new rotation matrix around the Y axis.
        *
        * \param angleInDeg The angle of rotation in degrees.
        */

      void setRotY( const self& angleInDeg );

      /** \brief Set the matrix to rotation.
        *
        * This routine will establish a new rotation matrix around the Z axis.
        *
        * \param angleInDeg The angle of rotation in degrees.
        */

      void setRotZ( const self& angleInDeg );

      /** \brief Set identity matrix.
        *
        * This routine will set the rotation matrix to its identity.
        */

      e_forceinline void setIdentity(){
        YX=ZX=XY=ZY=XZ=YZ=0;
        XX=YY=ZZ=1;
      }

      /** \brief Set the matrix to zero.
        *
        * This routine will set the matrix to all zeros.
        */

      e_forceinline void clear(){
        XX=YX=ZX=0;
        XY=YY=ZY=0;
        XZ=YZ=ZZ=0;
      }

      /** \breif Freidn function to invert matrix.
        *
        * This routine will invert the matrix given.
        *
        * \param M The matrix to invert.
        *
        * \return Returns a 3x3 matrix that's the inverse of this.
        */

      e_forceinline friend Matrix3 e_inverse( const Matrix3& M ){
        return M.inverse();
      }

      /** \brief Get inverse matrix.
        *
        * This routine will invert the matrix and return it.
        *
        * \return Returns a 3x3 matrix that's the inverse of this.
        */

      e_noinline Matrix3 inverse()const{
        const auto m = *this;
        const auto det =
          m( 0, 0 ) * ( m( 1, 1 ) * m( 2, 2 ) - m( 2, 1 ) * m( 1, 2 )) -
          m( 0, 1 ) * ( m( 1, 0 ) * m( 2, 2 ) - m( 1, 2 ) * m( 2, 0 )) +
          m( 0, 2 ) * ( m( 1, 0 ) * m( 2, 1 ) - m( 1, 1 ) * m( 2, 0 ));
        const auto invdet = 1./det;
        Matrix3 M;
        M( 0, 0 ) = ( m( 1, 1 )*m( 2, 2 ) - m( 2, 1 )*m( 1, 2 )) * invdet;
        M( 0, 1 ) = ( m( 0, 2 )*m( 2, 1 ) - m( 0, 1 )*m( 2, 2 )) * invdet;
        M( 0, 2 ) = ( m( 0, 1 )*m( 1, 2 ) - m( 0, 2 )*m( 1, 1 )) * invdet;
        M( 1, 0 ) = ( m( 1, 2 )*m( 2, 0 ) - m( 1, 0 )*m( 2, 2 )) * invdet;
        M( 1, 1 ) = ( m( 0, 0 )*m( 2, 2 ) - m( 0, 2 )*m( 2, 0 )) * invdet;
        M( 1, 2 ) = ( m( 1, 0 )*m( 0, 2 ) - m( 0, 0 )*m( 1, 2 )) * invdet;
        M( 2, 0 ) = ( m( 1, 0 )*m( 2, 1 ) - m( 2, 0 )*m( 1, 1 )) * invdet;
        M( 2, 1 ) = ( m( 2, 0 )*m( 0, 1 ) - m( 0, 0 )*m( 2, 1 )) * invdet;
        M( 2, 2 ) = ( m( 0, 0 )*m( 1, 1 ) - m( 1, 0 )*m( 0, 1 )) * invdet;
        return M;
      }

    /** \brief Get inverse matrix.
      *
      * This routine will invert the matrix and return it.
      *
      * \return Returns a 3x3 matrix that's the inverse of this.
      */

      e_forceinline Matrix3& invert(){
        *this = inverse();
        return *this;
      }

      /** @}
        *
        * \name Converstion methods.
        *
        * These methods convert the rotation matrix to other types, such as a
        * quaternion.
        */

      /** \brief Convert to quaternion.
        *
        * This routine will return the rotation matrix converted to a
        * quaternion.
        *
        * \return Returns a quaternion representing this matrix's rotation.
        */

      e_forceinline friend Quaternion e_quaternion( const Matrix3& M ){
        return M.asQuaternion();
      }

      /** \brief Convert to quaternion.
        *
        * This routine will return the rotation matrix converted to a
        * quaternion.
        *
        * \return Returns a quaternion representing this matrix's rotation.
        */

      Quaternion asQuaternion()const;

      /** @} */

    //}:                                          |
    //--------------------------------------------+-----------------------------

    /** \brief Matrix constructor.
      *
      * This constructor sets the matrix from three basis vectors.
      *
      * \param vx The right vector.
      * \param vy The forward vector.
      * \param vz The up vector.
      */

    e_forceinline_always Matrix3( const Vector3& vx, const Vector3& vy, const Vector3& vz )
      : XX( vx.x ), YX( vy.x ), ZX( vz.x )
      , XY( vx.y ), YY( vy.y ), ZY( vz.y )
      , XZ( vx.z ), YZ( vy.z ), ZZ( vz.z )
    {}

    /** \brief Copy constructor.
      *
      * This constructor will copy the content of another rotation matrix.
      *
      * \param M The matrix to copy.
      */

    e_forceinline_always Matrix3( const Matrix3& M )
      : XX( M.XX ), XY( M.XY ), XZ( M.ZX )
      , YX( M.YX ), YY( M.YY ), YZ( M.ZY )
      , ZX( M.ZX ), ZY( M.ZY ), ZZ( M.ZZ )
    {}

    /** \brief Matrix constructor.
      *
      * This constructor will build a rotation matrix from its components in
      * row major format.
      */

    e_forceinline_always Matrix3(
      const self& xx, const self& yx, const self& zx,
      const self& xy, const self& yy, const self& zy,
      const self& xz, const self& yz, const self& zz )
      : XX( xx ), YX( yx ), ZX( zx )
      , XY( xy ), YY( yy ), ZY( zy )
      , XZ( xz ), YZ( yz ), ZZ( zz )
    {}

    /** \brief Matrix constructor.
      *
      * This constructor will build a rotation matrix from its components in
      * row major format.
      */

    e_forceinline_always Matrix3(
        const float xx, const float yx, const float zx,
        const float xy, const float yy, const float zy,
        const float xz, const float yz, const float zz )
      : m00( xx ), m10( yx ), m20( zx )
      , m01( xy ), m11( yy ), m21( zy )
      , m02( xz ), m12( yz ), m22( zz )
    {}

    e_forceinline_always Matrix3( const float s )
      : m00( s ), m01( 0 ), m02( 0 )
      , m10( 0 ), m11( s ), m12( 0 )
      , m20( 0 ), m21( 0 ), m22( s )
    {}

    e_forceinline_always Matrix3( const self& scale ){
      setScale( scale );
    }

    /** \brief Default constructor.
      *
      * The default constructor will construct the matrix and set it
      * immediately to its identity.
      */

    e_forceinline_always Matrix3()
      : m00( 1 ), m01( 0 ), m02( 0 )
      , m10( 0 ), m11( 1 ), m12( 0 )
      , m20( 0 ), m21( 0 ), m22( 1 )
    {}

    union{
      struct{
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
      };
      struct{
        self XX, XY, XZ;
        self YX, YY, YZ;
        self ZX, ZY, ZZ;
      };
      Vector3 a[3];
      self    m[3][3];
      self    t[9];
    };
  };

//}:                                              |
//================================================+=============================
