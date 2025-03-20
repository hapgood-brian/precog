//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//DualQuaternion:{                                |

  /** \brief Simplistic implementation of dual quateernions.
    *
    * This class represents a dual quaternion.
    */

  struct E_PUBLISH DualQuaternion final{

    //--------------------------------------------+-----------------------------
    //Operate:{                                   |

      /** \brief Product operator.
        *
        * This operator will multiply two dual quaternions togethher.
        *
        * \param dq The dual quaternion to multiply against.
        *
        * \return Returns a new dual quaternion containing the product.
        */

      DualQuaternion operator*( const DualQuaternion& dq )const{
        return DualQuaternion( qn * dq.qn, qn * dq.qd + qd * dq.qn );
      }

      /** \brief Scalar product operator.
        *
        * This operator will multiply the dual quaternion by a scalar.
        *
        * \param f The scalar to multiply withh.
        *
        * \return Returns a new scaled dual quaternion.
        */

      DualQuaternion operator*( const self& f )const{
        return DualQuaternion( qn*f, qd*f );
      }

    //}:                                          |
    //Methods:{                                   |

      /** \brief Normalize the dual quaternion.
        *
        * This routine will normalize te dual quaternion.
        */

      e_noinline void normalize(){
        f32 rn = qn.norm();
        if( rn > 0.f ){
            rn = 1.f / rn;
          const Quaternion& realN = qn * rn;
          const Quaternion& dualN = qd * rn;
          qn = realN;
          qd = dualN - realN * realN.Dot( dualN );
        }
      }

      /** \brief Get homogeneous matrix.
        *
        * This routine will return a homogeneous matrix represneting this dual
        * quaternion.
        *
        * \return Returns 4x4 matrix.
        */

      Matrix4 get4x4() const;

      /** \brief Get affine matrix.
        *
        * This routine will return an affine matrix represneting this dual
        * quaternion.
        *
        * \return Returns 3x4 matrix.
        */

      Affine3 get3x4() const;

      /** \brief Get dual quaternion as 2x4 matrix.
        *
        * This routine will return the dual quaternion as a GPU friendly 2x4
        * affine matrix.
        *
        * \return Returns an vec2x4 matrix.
        */

      Affine2 as2x4() const;

    //}:                                          |
    //--------------------------------------------+-----------------------------

		explicit DualQuaternion( const Quaternion& r, const Quaternion& d )
			: qn( r )
			, qd( d )
		{}

		explicit DualQuaternion( const Quaternion& q, const Vector4& t )
        : qn( q ){
			qd.x =  .5f*( t.x * q.w + t.y * q.z - t.z * q.y );
			qd.y =  .5f*(-t.x * q.z + t.y * q.w + t.z * q.x );
			qd.z =  .5f*( t.x * q.y - t.y * q.x + t.z * q.w );
			qd.w = -.5f*( t.x * q.x + t.y * q.y + t.z * q.z );
		}

		explicit DualQuaternion( const Quaternion& q, const Vector3& t )
        : qn( q ){
			qd.x =  .5f*( t.x * q.w + t.y * q.z - t.z * q.y );
			qd.y =  .5f*(-t.x * q.z + t.y * q.w + t.z * q.x );
			qd.z =  .5f*( t.x * q.y - t.y * q.x + t.z * q.w );
			qd.w = -.5f*( t.x * q.x + t.y * q.y + t.z * q.z );
		}

		explicit DualQuaternion( const Quaternion& q, const Point3& t )
        : qn( q ){
			qd.x =  .5f*( t.x * q.w + t.y * q.z - t.z * q.y );
			qd.y =  .5f*(-t.x * q.z + t.y * q.w + t.z * q.x );
			qd.z =  .5f*( t.x * q.y - t.y * q.x + t.z * q.w );
			qd.w = -.5f*( t.x * q.x + t.y * q.y + t.z * q.z );
		}

		DualQuaternion( const Matrix4& L2W );

		DualQuaternion()
			: qn( 0, 0, 0, 1 )
			, qd( 0, 0, 0, 0 )
		{}

    Quaternion qn;
    Quaternion qd;
  };

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Quaternion:{                                    |

  /** \brief Quaternion class.
    *
    * This class defines the quaternion class. For iOS platforms it uses the
    * GLKMath library and NEON.
    */

  struct E_PUBLISH Quaternion final{

    static const Quaternion kIdentity;

    //============================================+=============================
    //Operate:{                                   |

      e_forceinline self operator[]( const s32 i )const{
        return a[ i ];
      }

      e_forceinline self& operator[]( const s32 i ){
        return a[ i ];
      }

      /** \brief Assignment operator.
        *
        * This inlined operator assigns one quaternion to another. It simply
        * assigns all four tuples.
        */

      e_forceinline Quaternion& operator=( const Quaternion& q ){
        e_assert( q.valid() );
        if( this != &q ){
          x = q.x;
          y = q.y;
          z = q.z;
          w = q.w;
        }
        return *this;
      }

      /** \brief Equality operator.
        *
        * This routine will compare two quaternions using an epsilon to fudge the
        * result. Perfect equality is not actually necessary in real world apps.
        * The epsilon is very small (0.0001f).
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if the quaternions are equal or almost equal and
        * false otherwise.
        */

      bool operator==( const Quaternion& q )const;

      /** \brief Inequality operator.
        *
        * This operator is the antithesis of the aforementioned equality
        * operator.
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if the quaternions are equal or almost equal and
        * false otherwise.
        */

      bool operator!=( const Quaternion& q )const;

      /** \brief Less than operator.
        *
        * Compare two quaternions and return if this is less than q from memcmp.
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if this quaternions is less than from memcmp else
        * false is returned.
        */

      bool operator<( const Quaternion& q )const;

      /** \brief Less than or equal to operator.
        *
        * Compare two quaternions and return if this is less than or equal to
        * q from a fast memcmp.
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if this quaternions is less than from memcmp else
        * false is returned.
        */

      bool operator<=( const Quaternion& q )const;

      /** \brief Greater than operator.
        *
        * Compare two quaternions and return if this is Greater than q from
        * memcmp.
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if this quaternions is less than from memcmp else
        * false is returned.
        */

      bool operator>( const Quaternion& q )const;

      /** \brief Greater than or equal to operator.
        *
        * Compare two quaternions and return if this is greater than or equal to
        * q from a fast memcmp.
        *
        * \param q The quaternion to compare.
        *
        * \return Returns true if this quaternions is less than from memcmp else
        * false is returned.
        */

      bool operator>=( const Quaternion& q )const;

      /** \brief Quotient operator.
        *
        * \param q The quaternion to rotate by.
        * \return A new quaternion.
        */

      Quaternion operator/( const Quaternion& q )const;

      /** \brief Quotient self operator.
        *
        * \param q The quaternion to rotate by.
        * \return Returns *this.
        */

      Quaternion& operator/=( const Quaternion& q );

      /** \brief Product operator.
        *
        * This routine will multiply two quaternions together (this and q) or in
        * other words it concatenates the two rotations.
        *
        * \param q The quaternion to rotate by.
        * \return A new quaternion that's the concatenation of this and q
        * rotations.
        */

      Quaternion operator*( const Quaternion& q )const;

      /** \brief Product self operator.
        *
        * This routine will rotate this quaternion by the rotation given in q.
        *
        * \param q The quaternion to rotate by.
        *
        * \return Returns *this.
        */

      Quaternion& operator*=( const Quaternion& q );

      /** \brief Sum operator.
        *
        * This routine will add two quaternions together (this and q).
        *
        * \param q The quaternion to add.
        *
        * \return A new quaternion.
        */

      Quaternion operator+( const Quaternion& q )const;

      /** \brief Sum self operator.
        *
        * This routine will add this quaternion to the rotation given by q.
        *
        * \param q The rotation to add (this is not the same as concatenation of
        * rotations).
        *
        * \return Returns *this.
        */

      Quaternion& operator+=( const Quaternion& q );

      /** \brief Scalar operator.
        *
        * This routine will scale the quaternion by the given scalar s.
        *
        * \param s The scalar to scale the rotation by.
        *
        * \return Returns a new quaternion.
        */

      Quaternion operator*( const self& s )const;

      /** \brief Scalar operator.
        *
        * This routine will scale the quaternion by the given scalar s.
        *
        * \param s The scalar to scale the rotation by.
        *
        * \return Returns *this.
        */

      Quaternion& operator*=( const self& s );

      /** \brief Scalar operator.
        *
        * This routine will scale the quaternion by the given scalar s.
        *
        * \param s The scalar to scale the rotation by.
        *
        * \return Returns a new quaternion.
        */

      Quaternion operator/( const self& s )const;

      /** \brief Scalar operator.
        *
        * This routine will scale the quaternion by the given scalar s.
        *
        * \param s The scalar to scale the rotation by.
        *
        * \return Returns *this.
        */

      Quaternion& operator/=( const self& s );

    //}:                                          |
    //Structs:{                                   |

      /** \brief Quaternion interpolation class.
        *
        * This class keeps track of time and computes the rotation between two
        * quaternions using slerp.
        */

      struct Interpolator{

        //----------------------------------------+-----------------------------
        //Methods:{                               |

          e_forceinline void begin( self N, const Quaternion& s, const Quaternion& e ){
            m_fTimeStart = m_fTime;
            m_fTimeEnd   = m_fTime + N;
            m_tStart     = s;
            m_tEnd       = e;
          }

          e_forceinline self time()const{
            return saturate(( m_fTime - m_fTimeStart )/( m_fTimeEnd - m_fTimeStart ));
          }

          e_forceinline Quaternion getValue()const{
            return m_tStart.slerped( m_tEnd, time() );
          }

          e_forceinline bool isFinished()const{
            return( time()==1 );
          }

          e_forceinline void tick( self dt ){
            m_fTime += dt;
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        Interpolator()
          : m_fTimeStart( 0 )
          , m_fTimeEnd( 0 )
          , m_fTime( 0 )
        {}

      private:

        self m_fTimeStart;
        self m_fTimeEnd;
        self m_fTime;

        e_var( Quaternion, t, Start );
        e_var( Quaternion, t, End );
      };

      union{
        struct{
          self x, y, z, w;
        };
        self a[ 4 ];
      };

    //}:                                          |
    //Methods:{                                   |

      /** \name Setters.
        *
        * These routines set the quaternion rotation in a variety of ways.
        *
        * @{
        */

      /** \brief Set axis angle.
        *
        * This routine will load this quaternion via an axis and angle.
        *
        * \param axis The axis to rotate around.
        * \param angleDeg The angle in degrees.
        */

      void setAxisAngle( const Vector3& axis, const self& angleDeg );

      /** \brief Set quaternion by euler angles.
        *
        * This routine will load this quaternion with the euler angles
        * provided; angles expected in vector form.
        */

      e_forceinline void setEuler( const self& pitch, const self& roll, const self& yaw ){
        setEuler( Vector3( pitch, roll, yaw ));
        e_assert( valid() );
      }

      /** \brief Set quaternion by euler angles.
        *
        * This routine will load this quaternion with the euler angles
        * provided; angles expected in vector form.
        */

      void setEuler( const Vector3& );

      /** \brief Set identity quaternion.
        *
        * This method will reinitialize the quaternion to 0 0 0 1.
        */

      e_forceinline void setIdentity(){
        x = k0;
        y = k0;
        z = k0;
        w = k1;
      }

      /** \brief Set zero quaternion.
        *
        * This method will reinitialize the quaternion to 0 0 0 0.
        */

      e_forceinline void setZero(){
        x = k0;
        y = k0;
        z = k0;
        w = k0;
      }

      /** @}
        *
        * \name Getters.
        *
        * These routines grab different aspects of the rotation, such as its
        * Euler angles, and returns them to the caller.
        *
        * @{
        */

      /** \brief Get euler angles.
        *
        * This routine will return the quaternion in terms of its euler
        * angles.
        *
        * \param x Pitch (attitude).
        * \param y Roll (bank).
        * \param z Yaw (heading).
        * \param homogeneous x, y and z are in homogeneous coordinates.
        */

      void getEuler( self& x, self& y, self& z, bool homogeneous=true )const;

      /** \brief Get euler angles.
        *
        * This routine will return the quaternion in terms of its euler
        * angles.
        *
        * \param angles Pitch (x), roll (y) and yaw (z).
        * \param homogeneous x, y and z are in homogeneous coordinates.
        */

      e_forceinline void getEuler( self angles[3], const bool homogeneous=true )const{
        getEuler( angles[0], angles[1], angles[2], homogeneous );
      }

      /** \brief Get 4x4 matrix.
        *
        * This routine will return a homogeneous rotation matrix.
        */

      Matrix4 get4x4()const;

      /** \brief Get 4x3 affine matrix.
        *
        * This routine will return a homogeneous rotation matrix.
        */

      Affine3 get3x4()const;

      /** \brief Get 3x3 matrix.
        *
        * This routine will return a rotation matrix.
        */

      Matrix3 get3x3()const;

      /** @}
        *
        * \name All the other methods.
        *
        * Such as getting the conjugate or rotation matrix from the quaternion.
        *
        * @{
        */

      /** \brief Compare quaternions.
        *
        * This routine compares this and q quaternions for quality using the
        * tolerance provided.
        *
        * \param q Quaternion to compare against.
        * \param tolerance Fudge factor.
        * \return Returns true if the quaternions are close enough to equal
        * and false otherwise.
        */

      bool compare( const Quaternion& q, const self& tolerance )const;

      /** \brief Randomize the quaternion.
        *
        * This routine will impart a random rotation to the quaternion.
        */

      void randomize( const self& x1, const self& x2, const self& x3 );

      /** \brief Rotate quaternion.
        *
        * This routine will rotate the quaternion by the given axis and angle
        * in degrees.
        *
        * \param axis The axis of rotation: +X is right, +Y is forward and
        * +Z is up in engine units.
        * \param angleDeg The angle in degrees.
        */

      void rotate( const Vector3& axis, const self& angleDeg );

      /** \brief Rotation by mouse.
        *
        * This routine will rotate the quaternion based on a 2D delta mouse
        * position. It is useful for free cameras.
        *
        * \param mouse 2D delta position of the mouse.
        */

      void rotate( const Vector2& mouse );

      /** \brief Look at rotation.
        *
        * This routine will create a rotation that "looks" down the ray from
        * the "from" point to the "to" point.
        *
        * \param from The origin of the ray.
        * \param to The end of the ray.
        */

      void lookat( const Point3& from, const Point3& to );

      /** \brief Align rotation to normal.
        *
        * This routine will create a rotation that aligns to the given up
        * vector and normal. It is especially useful when used with the
        * terrain system for example.
        *
        * \param up Up vector.
        * \param N The normal.
        *
        * \return Returns *this.
        */

      Quaternion& align( const Vector3& up, const Vector3& N );

      /** \brief Dot product.
        *
        * This routine returns the quaternion dot product.
        *
        * \param q The second rotation to dot against.
        *
        * \return The quaternion dot product.
        */

      self dot( const Quaternion& q )const;

      /** \brief Get quaternion norm.
        *
        * This routine will return the quaternion norm.
        *
        * \return Returns the quaternion norm.
        */

      e_forceinline self norm()const{
        return dot( *this ).sqrt();
      }

      /** \brief Normalize rotation.
        *
        * This routine will normalize the rotation in place.
        *
        * \return Returns *this.
        */

      Quaternion& normalize();

      /** \brief Normalized rotation.
        *
        * This routine will return the normalized rotation.
        *
        * \return Returns normalized rotation.
        */

      e_forceinline Quaternion normalized()const{
        e_assert( valid(), "Invalid rotation!" );
        Quaternion q( *this );
        q.normalize();
        return q;
      }

      /** \brief Get conjugate.
        *
        * This routine calculates and returns the conjugate of this rotation.
        *
        * \return Returns the conjugate rotation.
        */

      Quaternion conjugate()const;

      /** \brief Get the inverse rotation.
        *
        * This routine returns the inverse rotation of this without changing
        * anything.
        *
        * \return A copy of the inverse rotation.
        */

      Quaternion inverse()const;

      /** \brief Invert rotation in place.
        *
        * This routine will invert this rotation.
        */

      void invert();

      /** \brief Get exponent rotation.
        *
        * This routine will return the exp of this rotation.
        *
        * \return A new exp rotation.
        */

      Quaternion exp()const;

      /** \brief Get log rotation.
        *
        * This routine will return the log of this rotation.
        *
        * \return A new log rotation.
        */

      Quaternion log()const;

      /** \brief Negate the rotation.
        *
        * This is similar to invert but quicker and dirtier.
        */

      e_forceinline void negate(){
        x = -x;
        y = -y;
        z = -z;
        w = -w;
      }

      /** \brief Spherical interpolated function.
        *
        * This routine will spherically interpolate this rotation between the
        * given quaternions by t. It returns a copy of the result without
        * changing this.
        *
        * \param Q End rotation (t=1).
        * \param t The parametric time step.
        *
        * \return Returns new rotation.
        */

      e_forceinline Quaternion slerped( const Quaternion& Q, const self& t )const{
        e_assert( valid() && Q.valid() && t.valid() );
        Quaternion q;
        q.slerp( *this, Q, t );
        return q;
      }

      /** \brief Spherical interpolation function.
        *
        * This routine will spherically interpolate rotations.
        *
        * \param Q End rotation (t=1).
        * \param t The parametric time step.
        *
        * \return Returns *this.
       */

      e_forceinline Quaternion& slerp( const Quaternion& Q, const self& t ){
        e_assert( valid() && Q.valid() && t.valid() );
        *this = slerped( Q, t );
        return *this;
      }

      /** \brief Spherical interpolation function.
        *
        * This routine will spherically interpolate this rotation between the
        * given quaternions by t.
        *
        * \param q1 Start rotation (t=0).
        * \param q2 End rotation (t=1).
        * \param t The parametric time step.
        *
        * \return Returns *this.
        */

      Quaternion& slerp(
        const Quaternion& q1,
        const Quaternion& q2,
        const self& t );

      /** \brief Slerped no invert.
        *
        * This routine is the same as slerp() but performs no invert.
        */

      static Quaternion slerpedNoInvert( const Quaternion& q1, const Quaternion& q2, const self& t );

      /** \brief Linear interpolated function.
        *
        * This routine will linearly interpolate between two rotations.
        *
        * \param q1 Start rotation (t=0).
        * \param q2 End rotation (t=1).
        * \param t The parametric time step.
        * \return Returns new interpolated rotation.
        */

      static Quaternion lerped(
        const Quaternion& q1,
        const Quaternion& q2,
        const self& t );

      /** \brief Linear interpolated function.
        *
        * This routine will linearly interpolate between two rotations.
        *
        * \param q1 Start rotation (t=0).
        * \param q2 End rotation (t=1).
        * \param t The parametric time step.
        * \return Returns *this.
        */

      Quaternion& lerp(
        const Quaternion& q1,
        const Quaternion& q2,
        const self& t );

      /** \brief Spherical cubic interpolation.
        *
        * This routine will spherically cubically interpolate.
        *
        * \param q1 Rotation 1.
        * \param q2 Rotation 2.
        * \param q3 Rotation 3.
        * \param q4 Rotation 4.
        * \param t The parametric time step.
        * \return Returns new rotation.
        */

      Quaternion squaded(
          const Quaternion& q1,
          const Quaternion& q2,
          const Quaternion& q3,
          const Quaternion& q4,
          const self& t ){
        e_assert( valid() );
        e_assert( q1.valid() );
        e_assert( q2.valid() );
        e_assert( q3.valid() );
        e_assert( q4.valid() );
        Quaternion q;
        q.squad( q1, q2, q3, q4, t );
        return q;
      }

      /** \brief Spherical cubic interpolation.
        *
        * This routine will spherically cubically interpolate.
        *
        * \param q1 Rotation 1.
        * \param q2 Rotation 2.
        * \param q3 Rotation 3.
        * \param q4 Rotation 4.
        * \param t The parametric time step.
        * \return Returns *this.
        */

      Quaternion& squad(
        const Quaternion& q1,
        const Quaternion& q2,
        const Quaternion& q3,
        const Quaternion& q4,
        const self& t );

      /** \brief Spherical cubic interpolation.
        *
        * This routine will spherically cubically interpolate.
        *
        * \param q2 Rotation 2.
        * \param q3 Rotation 3.
        * \param q4 Rotation 4.
        * \param t The parametric time step.
        * \return Returns *this.
        */

      e_forceinline Quaternion& squad(
          const Quaternion& q2,
          const Quaternion& q3,
          const Quaternion& q4,
          const self& t ){
        e_assert( valid() );
        e_assert( q2.valid() );
        e_assert( q3.valid() );
        e_assert( q4.valid() );
        squad( *this, q2, q3, q4, t );
        return *this;
      }

      /** \brief Get square length.
        *
        * This routine will return the square length of the rotation.
        *
        * \return The square length.
        */

      self lengthSquared()const;

      /** \brief Get length.
        *
        * This routine will return the length of the rotation.
        *
        * \return The length.
        */

      self length()const;

      /** \brief Is valid rotation?
        *
        * This routine will return true if the rotation is valid and false
        * otherwise.
        */

      e_forceinline bool valid()const{
        if( !x.valid()||( x * x < k0 )){
          return false;
        }
        if( !y.valid()||( y * y < k0 )){
          return false;
        }
        if( !z.valid()||( z * z < k0 )){
          return false;
        }
        if( !w.valid()||( w * w < k0 )){
          return false;
        }
        return true;
      }

      /** \brief Is unit rotation?
        *
        * This routine will return true if the quaternion is a unit rotation
        * and false otherwise.
        */

      e_forceinline bool isUnit(){
        e_assert( valid(), "Invalid rotation!" );
        self f = lengthSquared();
        f = abs( f ) - k1;
        if( f < .001 * .001 ){
          return true;
        }
        return false;
      }

      e_forceinline self tuple( const s32 i )const{
        e_assert( valid(), "Invalid rotation!" );
        e_assert( i < e_sizeof( a ));
        return a[ i ];
      }

      e_forceinline self& tuple( const s32 i ){
        e_assert( valid(), "Invalid rotation!" );
        e_assert( i < e_sizeof( a ));
        return a[ i ];
      }

      /** @} */

    //}:                                          |
    //============================================+=============================

    /** \brief Quaternion constructor.
      *
      * This constructor builds a quaternion from it's component parts: xyzw.
      */

    e_forceinline_always Quaternion( const self& X, const self& Y, const self& Z, const self& W )
        : x( X )
        , y( Y )
        , z( Z )
        , w( W ){
      e_assert( valid() );
    }

    /** \brief Quaternion constructor.
      *
      * This constructor builds a quaternion from it's component parts: xyzw.
      */

    e_forceinline_always Quaternion( const float X, const float Y, const float Z, const float W )
      : x( X )
      , y( Y )
      , z( Z )
      , w( W )
    {}

    /** \brief Construct quaternion from euler angles.
      *
      * This routine will initialize the quaternion from the euler angles
      * provided.
      *
      * \param pitch Angle of rotation around the X axis.
      * \param roll  Angle of rotation around the Y axis.
      * \param yaw   Angle of rotation around the Z axis.
      */

    e_forceinline_always Quaternion( const self& pitch, const self& roll, const self& yaw ){
      setEuler( pitch, roll, yaw );
      e_assert( valid() );
    }

    /** \brief Construct quaternion from initializer list.
      *
      * This constructor will build the four tuples from an initializer list.
      *
      * \param ilist The initializer list with zero to four elements.
      */

    e_forceinline_always Quaternion( const std::initializer_list<T>& ilist )
        : x( 0 )
        , y( 0 )
        , z( 0 )
        , w( 1 ){
      e_assert( ilist.size() <= 4 );
      e_assert( ilist.size() >= 0 );
      u32 i = 0;
      for( auto& v : ilist ){
        a[ i++ ]=v;
      }
    }

    /** \brief Axis angle constructor.
      *
      * This constructor will initialize our rotation with an axis and angle.
      *
      * \param axis The axis of rotation.
      * \param angleDeg The angle in degrees.
      */

    e_forceinline_always Quaternion( const Vector3& axis, const self& angleDeg ){
      setAxisAngle( axis, angleDeg );
      e_assert( valid() );
    }

    /** \brief Quaternion copy constructor.
      *
      * This constructor builds a quaternion by copying the values from another
      * quaternion.
      */

    e_forceinline_always Quaternion( const Quaternion& q )
        : x( q.x )
        , y( q.y )
        , z( q.z )
        , w( q.w ){
      e_assert( valid() );
    }

    /** \brief Construct quaternion from euler angles.
      *
      * This routine will initialize the quaternion from the euler angles
      * provided.
      */

    e_forceinline_always Quaternion( const Vector3& euler ){
      setEuler( euler );
      e_assert( valid() );
    }

    /** \brief Construct quaternion from 3x3 matrix.
      *
      * This routine will initialize the quaternion from the 3x3 matrix given.
      */

    e_noinline_or_debug Quaternion( const Matrix3& M ){
      w = sqrt( 1.f + M.XX + M.YY + M.ZZ) * .5f;
      const self& w4 = 1.f/(4.f * w);
      x = (M.ZY - M.YZ) * w4;
      y = (M.XZ - M.ZX) * w4;
      z = (M.YX - M.XY) * w4;
      e_assert( valid() );
    }

    /** \brief Quaternion default constructor.
      *
      * This constructor initializes the quaternion to the identity 0 0 0 1.
      */

    e_forceinline_always Quaternion()
      : x( 0 )
      , y( 0 )
      , z( 0 )
      , w( 1 )
    {}
  };

//}:                                              |
//================================================+=============================
