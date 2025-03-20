//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//QST3:{                                          |

  /** \brief The QST 3D transform.
    *
    * This class defines the 3D QST transform. QST stands for quaternion, scale
    * and translation. It is a way of packing a 4x4 matrix into just two vector
    * objects and is a lot less demanding on memory especially when it comes to
    * animation data with it's complex set of curves. The full type for a QST3
    * is math<float>::QST3 but everywhere in the engine we refer to it by it's
    * shorthand type "qst3".
    */

  struct alignas(16) E_PUBLISH QST3{

    //--------------------------------------------+-----------------------------
    //Operate:{                                   |

      /** \brief Difference operator.
        *
        * This operator takes the difference between two QSTs and returns it.
        */

      e_noinline QST3 operator-( const QST3& M )const{
        QST3 m = *this;
        m.m_qRotation = ( m.toRotation().inverse() * M.toRotation() ).inverse();//quaternion divide.
        m.m_tPosition.x -= M.m_tPosition.x;
        m.m_tPosition.y -= M.m_tPosition.y;
        m.m_tPosition.z -= M.m_tPosition.z;
        return m;
      }

      /** \brief Array operator.
        *
        * This operator will treat the QST as an array and return the
        * individual channels of the position, quaternion and scale. Please
        * note that this is a const operator because the rotation indices are
        * special cases: we return the euler angles for the QST in degrees not
        * the actual four-tuple quaternion components. The following code is
        * a good example of using this operator, as well as it's primary
        * intention which was bone animation.
        \code
          bool myClass::testQST( const qst3& L2W )const{
            f32 tx = L2W[0];
            f32 ty = L2W[1];
            f32 tz = L2W[2];
            f32 ex = L2W[3];//pitch
            f32 ey = L2W[4];//roll
            f32 ez = L2W[5];//yaw
            f32 sx = L2W[6];//scale
            string sL2W;
            sL2W.catf(
                "%f, %f, %f, %f, %f, %f, %f",
                f32::cast( tx ), f32::cast( ty ), f32::cast( tz ),
                f32::cast( rx ), f32::cast( ry ), f32::cast( rz ),
                f32::cast( sx ));
            const qst3 compareL2W = sL2W.as<qst3>();
            return( L2W==compareL2W );//always true
          }
        \endcode
        *
        * \param i The Nth channel into the QST. 0-2 is the translation lifted
        * straight out of the position member. 3-5 is the rotation in euler
        * angles in degrees. 6 is the scale. In debug builds the operator will
        * fire a debug assertion if i >= 7 but in release builds it will return
        * the scale.
        *
        * \return Returns the value at the given indexed channel.
        */

      self operator[]( const u32 i )const;

      /** \brief Position update operator.
        *
        * This operator will add the given vector to the position portion of
        * the QST transform.
        *
        * \param V A three-tuple vector to add to the position.
        */

      e_forceinline void operator+=( const Vector3& V ){
        x() += V.x;
        y() += V.y;
        z() += V.z;
      }

      /** \brief Concatenate QST transforms.
        *
        * This routine will add the two positions and store, multiply the two
        * quaternions and multiply the scales. Please see the following code
        * for a proof of this multiplicative operator.
        *
        * \param L2W The QST transform to concatenate with this one.
        */

      void operator*=( const QST3& L2W );

      /** \brief Multiply QST by scalar to get a homogeneous matrix.
        *
        * This routine will convert the QST to a 4x4 matrix and multiply it by
        * the scalar provided. The result is returned.
        *
        * \param scalar An amount to scale the resulting homogeneous matrix.
        *
        * \return Returns a scaled matrix of type math<float>::Affine3 or the
        * shorthand f32::Affine3 or the shorthand vec4x4 which is most common
        * in the engine.
        */

      e_forceinline Affine3 operator*( const self& scalar )const{
        return Affine3( scalar ) * get4x4();
      }

      /** \brief Multiply two QST3 transforms.
        *
        * This operator multiplies two QSTs as if they were normal matrices. It
        * adds the two positions together, concatenates the quaternions and
        * multiplies the scales. QSTs are an efficient representation because
        * of the fact that they only store two vectors. The downside is you
        * lose non-uniform scaling. One scalar for all three axis in other
        * words.
        *
        * \param L2W The local-to-world QST format transform.
        */

      e_forceinline QST3 operator*( const QST3& L2W )const{
        QST3 q = *this;
        q *= L2W;
        return q;
      }

      /** \brief Transform a point by the QST.
        *
        * This routine will transform the input three-tuple point by the matrix
        * derived from this QST (calling get4x4) and return it.
        *
        * \param P The point to transform.
        *
        * \return Returns a fully transformed point.
        */

      Point3 operator*( const Point3& P )const;

      /** \brief Equality operator.
        *
        * This operator will check to see if two QSTs are equal. NOT almost
        * equal using an epsilon value, you'll have to implement that yourself
        * if you want it.
        *
        * \param qst The transform to compare again.
        *
        * \return Returns true if this QST and the one provided are equal and
        * false otherwise.
        */

      e_forceinline bool operator==( const QST3& qst )const{
        if( qst.m_tPosition != m_tPosition ){
          return false;
        }
        if( qst.m_qRotation != m_qRotation ){
          return false;
        }
        return( qst.m_fScale==m_fScale );
      }

      /** \brief Inequality operator.
        *
        * This operator will check to see if two QSTs are not equal.
        *
        * \param qst The transform to compare again.
        *
        * \return Returns true if this QST and the one provided are not equal
        * and false otherwise.
        */

      e_forceinline bool operator!=( const QST3& qst )const{
        if( qst.m_tPosition != m_tPosition ){
          return true;
        }
        if( qst.m_qRotation != m_qRotation ){
          return true;
        }
        return( m_fScale!=qst.m_fScale );
      }

    //}:                                          |
    //Methods:{                                   |
      //[get?x?*]:{                               |

        Matrix4 get4x4RT()const;
        Affine3 get3x4RT()const;
        Matrix4 get4x4ST()const;
        Affine3 get3x4ST()const;
        Matrix4 get4x4T ()const;
        Affine3 get3x4T ()const;
        Matrix4 get4x4R ()const;
        Affine3 get3x4R ()const;
        Matrix3 get3x3R ()const;
        Matrix4 get4x4S ()const;
        Affine3 get3x4S ()const;
        Matrix3 get3x3S ()const;
        Matrix4 get4x4  ()const;
        Affine3 get3x4  ()const;
        Matrix3 get3x3  ()const;

      //}:                                        |
      //[rotation]:{                              |

        /** \brief Set rotation by axis and angle.
          *
          * This routine will set the rotation by the axis and angle provided.
          *
          * \param axis The three-tuple vector describing the axis of rotation.
          * \param a The angle.
          */

        e_forceinline void setAxisAngle( const Vector3& axis, const self& a ){
          toRotation().setAxisAngle( axis, a );
          e_assert( valid() );
        }

        /** \brief Reset the rotation part.
          *
          * This routine will remove any rotation fromt the QST by resetting
          * the rotation to a default quaternion.
          */

        e_forceinline void resetRotation(){
          toRotation() = Quaternion();
        }

        /** \brief Apply delta rotation to this QST.
          *
          * This routine will rotate from the current orientation by the axis
          * and angle provided. These are loaded into a quaternion and
          * multiplied with this rotation.
          *
          * \param axis The axis of rotation.
          * \param angleDeg The angle of rotation in that axis in degrees.
          */

        e_forceinline void rotate( const Vector3& axis, const self& angleDeg ){
          toRotation().rotate( axis, angleDeg );
          e_assert( valid() );
        }

        /** \brief Rotate by screen space position.
          *
          * This routine is used for rotating a camera using the screen space
          * coordinates of the mouse. It is only currently used for this
          * operation.
          *
          * \param xyMouse The screen space delta coordinates of the mouse.
          */

        e_forceinline void rotate( const Vector2& xyMouse ){
          toRotation().rotate( xyMouse );
          e_assert( valid() );
        }

        /** \brief Align rotation with normal and up vector.
          *
          * This routine will align the rotation with the cross of the up and
          * normal vectors provided. This is great for cameras, which the QST
          * chiefly services along with rigid bodies.
          *
          * \param up The up vector of the coordinate frame.
          * \param N The normal vector.
          */

        e_forceinline void align( const Vector3& up, const Vector3& N ){
          toRotation().align( up, N );
          e_assert( valid() );
        }

        /** \brief Lookat point.
          *
          * This routine will create a look-at vector from point A to point B
          * and rotate the QST accordingly.
          *
          * \param a The origin of the look-at operation.
          * \param b The target of the look-at operation.
          */

        e_forceinline void lookat( const Point3& a, const Point3& b ){
          toRotation().lookat( a, b );
          e_assert( valid() );
        }

        /** \brief Lookat point.
          *
          * This routine will look at a point in 3D space using the current QST
          * position as the origin.
          *
          * \param p The target point to look at.
          */

        e_forceinline void lookat( const Point3& p ){
          toRotation().lookat( toPosition(), p );
          e_assert( valid() );
        }

      //}:                                        |
      //[position]:{                              |

        /** \brief Set absolute position on ground plane.
          *
          * This routine will set the position of the coordinate frame supplied
          * by this QST. This method will set the position on the ground plane
          * and not above.
          *
          * \param p The new position to set the QST to as a Point2 object..
          */

        e_forceinline void setPosition( const Point2& p ){
          x() = p.x;
          y() = p.y;
          z() = 0.f;
          e_assert( valid() );
        }

        /** \brief Set absolute position in full local or world space.
          *
          * This routine sets the QST's position in all three dimensions.
          *
          * \param X The x coordinate (right).
          * \param Y The y coordinate (forward).
          * \param Z The z coordinate (up).
          */

        e_forceinline void setPosition( const self& X, const self& Y, const self& Z ){
          x() = X;
          y() = Y;
          z() = Z;
          e_assert( valid() );
        }

        /** \brief Set absolute position on ground plane.
          *
          * This routine will set the position relative to the ground plane. It
          * will snap to this plane by setting the Z component to zero.
          *
          * \param X The x coordinate (right).
          * \param Y The y coordinate (forward).
          */

        e_forceinline void setPosition( const self& X, const self& Y ){
          x() = X;
          y() = Y;
          z() = 0.f;
          e_assert( valid() );
        }

        /** \brief Translate the position by a delta vector.
          *
          * This routine will set the position of the QST relative to its
          * current location.
          *
          * \param V The delta position to add to the current position.
          */

        e_forceinline void translate( const Vector3& V ){
          x() += V.x;
          y() += V.y;
          z() += V.z;
          e_assert( valid() );
        }

        /** \brief Translate the position by a delta amount.
          *
          * This routine will set the position of the QST relative to its
          * current position.
          *
          * \param X The delta x coordinate (right).
          * \param Y The delta y coordinate (forward).
          * \param Z The delta z coordinate (up).
          */

        e_forceinline void translate( const self& X, const self& Y, const self& Z ){
          x() += X;
          y() += Y;
          z() += Z;
          e_assert( valid() );
        }

        /** \brief Translate the position by delta amount in XY plane.
          *
          * This routine will apply a delta change in position parallel to the
          * ground plane. You can have any Z location, that won't be touched,
          * but the position will move through the horizontal plane.
          *
          * \param X The delta x coordinate (right).
          * \param Y The delta y coordinate (forward).
          */

        e_forceinline void translate( const self& X, const self& Y ){
          x() += X;
          y() += Y;
          e_assert( valid() );
        }

      //}:                                        |
      //[identity]:{                              |

        /** \brief Check if QST is the identity.
          *
          * This routine compares the position, rotation and scale against the
          * identity transform and itself.
          *
          * \return Returns true if the QST is identity or false.
          */

        e_forceinline bool isIdentity()const{
          e_assert( valid() );
          return( *this == QST3() );
        }

        /** \brief Set QST identity.
          *
          * This routine sets the QST to it's identity. It is loaded with
          * values that will produce an identity matrix if you call getMatrix()
          * on this object.
          */

        e_forceinline void setIdentity(){
          m_qRotation.setIdentity();
          m_tPosition.clear();
          m_fScale = k1;
        }

      //}:                                        |
      //[channels]:{                              |

        e_forceinline self x()const{
          e_assert( valid() );
          return toPosition().x;
        }

        e_forceinline self& x(){
          e_assert( valid() );
          return toPosition().x;
        }

        e_forceinline self y()const{
          e_assert( valid() );
          return toPosition().y;
        }

        e_forceinline self& y(){
          e_assert( valid() );
          return toPosition().y;
        }

        e_forceinline self z()const{
          e_assert( valid() );
          return toPosition().z;
        }

        e_forceinline self& z(){
          e_assert( valid() );
          return toPosition().z;
        }

      //}:                                        |
      //invert:{                                  |

        void invert();

      //}:                                        |
      //valid:{                                   |

        e_forceinline bool valid()const{
          if( !m_tPosition.valid() ){
            return false;
          }
          if( !m_qRotation.valid() ){
            return false;
          }
          if( !m_fScale.valid() ){
            return false;
          }
          return true;
        }

      //}:                                        |
      //tuple:{                                   |

        e_forceinline self tuple( const u32 i )const{
          return ((self*)&m_tPosition)[ i ];
        }

        e_forceinline self& tuple( const u32 i ){
          return ((self*)&m_tPosition)[i];
        }

      //}:                                        |
      //scale:{                                   |

        /** \brief Scale the QST by a scalar.
          *
          * This routine will scale the current uniform scale by a uniform
          * scalar.
          *
          * \param s The amount to scale the QST.
          */

        e_forceinline void scale( const self& s ){
          m_fScale *= s;
        }

        /** \brief Get copy at given scale.
          *
          * This routine will return a QST at the given scale.
          *
          * \param s The new scale to return with.
          *
          * \return Returns a new QST with the given scale.
          */

        e_forceinline QST3 atScale( const self& s )const{
          return QST3( m_qRotation, s, m_tPosition );
        }

      //}:                                        |
    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline QST3( const self& tx, const self& ty, const self& tz, const self& rx, const self& ry, const self& rz, const self& rw, const self& sx )
      : m_tPosition( tx, ty, tz     )
      , m_qRotation( rx, ry, rz, rw )
      , m_fScale   ( sx             )
    {}

    constexpr QST3( const float tx, const float ty, const float tz, const float rx, const float ry, const float rz, const float rw, const float sx )
      : m_tPosition( tx, ty, tz     )
      , m_qRotation( rx, ry, rz, rw )
      , m_fScale   ( sx             )
    {}

    e_forceinline_always QST3( const Quaternion& Q, const self& S, const Point3& t )
      : m_qRotation( Q )
      , m_fScale   ( S )
      , m_tPosition( t )
    {}

    e_noinline_or_debug QST3( const std::initializer_list<T>& ilist ){
      u32 i = 0;
      for( auto& v : ilist ){
        if( i < 3 ){
          m_tPosition[ i ]=v;
        }else if( i < 6 ){
          m_qRotation[ i-3 ]=v;
        }else{
          m_fScale = v;
        }
      }
    }

    QST3() = default;

  private:

    e_var( Quaternion, q, Rotation );
    e_var( Point3,     t, Position );
    e_var( self,       f, Scale    ) = 1.f;
  };

//}:                                              |
//================================================+=============================
