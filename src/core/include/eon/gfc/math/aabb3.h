//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//AABB3:{                                         |

  /** \brief The 3D axis-aligned bounding box.
    *
    * The three dimensional axis-aligned bounding box is used by all entity
    * objects in the engine to represent its bounds. Like its 2D counterpart
    * the AABB3 is initialized to the opposite extremes so we can easily just
    * add points to it to construct any bounds from a point cloud. To get a
    * bounding box with zeros in it use the built in kZero constant.
    */

  struct E_PUBLISH AABB3 final{

    Point3 min;
    Point3 max;

    //--------------------------------------------+-----------------------------
    //Statics:{                                   |

      static const AABB3 kZero;

    //}:                                          |
    //Operate:{                                   |

      /** \name Bounding box operators.
        *
        * These routines will perform various routines on the axis-aligned
        * bounding box from comparing equality to multiplying by a scalar.
        *
        * @{
        */

      e_forceinline AABB3& operator=( const AABB3& b ){
        min = b.min;
        max = b.max;
        e_assert( valid() );
        return *this;
      }

      e_noinline AABB3& operator*=( const Matrix4& M ){
        e_assert( M.valid() );
        const auto& v0 = Point3( min.x, min.y, min.z ) * M;
        const auto& v1 = Point3( min.x, max.y, min.z ) * M;
        const auto& v2 = Point3( max.x, min.y, min.z ) * M;
        const auto& v3 = Point3( max.x, max.x, min.z ) * M;
        const auto& v4 = Point3( min.x, min.y, max.z ) * M;
        const auto& v5 = Point3( min.x, max.y, max.z ) * M;
        const auto& v6 = Point3( max.x, min.y, max.z ) * M;
        const auto& v7 = Point3( max.x, max.x, max.z ) * M;
        reset();
        *this += v0;
        *this += v1;
        *this += v2;
        *this += v3;
        *this += v4;
        *this += v5;
        *this += v6;
        *this += v7;
        return*this;
      }

      e_forceinline AABB3 operator*( const Matrix4& M )const{
        e_assert( M.valid() );
        e_assert( valid() );
        AABB3 b( *this );
        b *= M;
        return b;
      }

      /** \brief Inequality operator.
        *
        * This operator compares the two axis-aligned bounding boxes to see if
        * they are unequal.
        *
        * \param tBounds The bounding box to compare.
        *
        * \return Returns true if the two bounding boxes (self vs. b) are
        * different and false otherwise.
        */

      e_forceinline bool operator!=( const AABB3& tBounds )const{
        e_assert( valid() );
        if( min == tBounds.min ){
          return false;
        }
        if( max == tBounds.max ){
          return false;
        }
        return true;
      }

      /** \brief Equality operator.
        *
        * This operator compares the two axis-aligned bounding boxes to see if
        * they are equal.
        *
        * \param tBounds The bounding box to compare.
        *
        * \return Returns false if the two bounding boxes (self vs. b) are
        * different and true otherwise.
        */

      e_forceinline bool operator==( const AABB3& tBounds )const{
        e_assert( valid() );
        if( min != tBounds.min ){
          return false;
        }
        if( max != tBounds.max ){
          return false;
        }
        return true;
      }

      /** \brief Scalar operator.
        *
        * This routine will scale the bounding box by the given amount. This is
        * not a geometric scale so the min point will not drift in the opposite
        * direction to the max point.
        *
        * \param scalar The real value to scale the bounding box by.
        */

      e_forceinline AABB3& operator*=( const float scalar ){
        e_assert( self( scalar ).valid() );
        e_assert( valid() );
        min *= scalar;
        max *= scalar;
        return *this;
      }

      /** \brief Scalar operator.
        *
        * This routine will scale the bounding box by the given amount. This is
        * not a geometric scale so the min point will not drift in the opposite
        * direction to the max point.
        *
        * \param scalar The real value to scale the bounding box by.
        */

      e_forceinline AABB3& operator*=( const self& scalar ){
        e_assert( scalar.valid() );
        e_assert( valid() );
        min *= scalar;
        max *= scalar;
        return *this;
      }

      /** \brief Additive operator.
        *
        * This routine will add a point to the current bounding box. This will
        * grow the bounds as necessary. It's the method used for building boxes
        * from geometry.
        *
        * \param v The point to add to the bounding box which will cause both
        * min and max to expand.
        *
        * \return Returns the dereferenced this pointer so additional bounding
        * box operations can be performed in a line.
        */

      e_forceinline AABB3& operator+=( const Vector3& v ){
        if( min.x > v.x ) min.x = v.x;
        if( min.y > v.y ) min.y = v.y;
        if( min.z > v.z ) min.z = v.z;
        if( max.x < v.x ) max.x = v.x;
        if( max.y < v.y ) max.y = v.y;
        if( max.z < v.z ) max.z = v.z;
        return *this;
      }

      /** \brief Additive operator.
        *
        * This routine will add a point to the current bounding box. This will
        * grow the bounds as necessary. It's the method used for building boxes
        * from geometry.
        *
        * \param p The point to add to the bounding box which will cause both
        * min and max to expand.
        *
        * \return Returns the dereferenced this pointer so additional bounding
        * box operations can be performed in a line.
        */

      e_forceinline AABB3& operator+=( const Point3& p ){
        if( min.x > p.x ) min.x = p.x;
        if( min.y > p.y ) min.y = p.y;
        if( min.z > p.z ) min.z = p.z;
        if( max.x < p.x ) max.x = p.x;
        if( max.y < p.y ) max.y = p.y;
        if( max.z < p.z ) max.z = p.z;
        return *this;
      }

      /** \brief Additive operator.
        *
        * This operator will add a bounding box to this bounding box. Basically
        * the input bounds min and max points are added to *this which will
        * cause this bounding box to expand appropriately. For this to work the
        * bounds had to have been default constructed to its numerical
        * extremes.
        *
        * \param tBounds The bounding box to add to *this by point.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3& operator+=( const AABB3& tBounds ){
        *this += tBounds.min;
        *this += tBounds.max;
        return *this;
      }

      /** @}
        *
        * \name Indexing operators.
        *
        * These operators are used for indexing into the bounds like an array.
        *
        * @{
        */

      e_forceinline const self& operator[]( const u32 i )const{
        return reinterpret_cast<const self*>( this )[ i ];
      }

      e_forceinline self& operator[]( const u32 i ){
        return reinterpret_cast<self*>( this )[ i ];
      }

    //}:                                          |
    //Methods:{                                   |

      /** @}
        *
        * \name Validation methods.
        *
        * These methods are used to validate the bounding box for numerical
        * error and other problems.
        *
        * @{
        */

      /** \brief Check if the bounding box is empty.
        *
        * This routine will check to see if the current bounding box had had
        * no points added to it.
        *
        * \param b The bounding box to validate.
        *
        * \return Returns true if no points were added or false otherwise.
        */

      e_forceinline friend bool e_empty( const AABB3& b ){
        return b.empty();
      }

      /** \brief Check if the bounding box is empty.
        *
        * This routine will check to see if the current bounding box had had
        * no points added to it.
        *
        * \return Returns true if no points were added or false otherwise.
        */

      bool empty()const;

      /** \brief Check if the bounding box is valid.
        *
        * This routine will see if the bounds are at their inverse max extents
        * and if they are return false. True otherwise.
        *
        * \param b The bounding box to validate.
        *
        * \return Returns true if the bounds were valid or false otherwise.
        */

      e_forceinline friend bool e_valid( const AABB3& b ){
        return b.valid();
      }

      /** \brief Check if the bounding box is valid.
        *
        * This routine will see if the bounds are at their inverse max extents
        * and if they are return false. True otherwise.
        *
        * \return Returns true if the bounds were valid or false otherwise.
        */

      bool valid()const;

      /** @}
        *
        * \name Half conversion.
        *
        * These methods will convert the bounding box to its half axis.
        *
        * @{
        */

      /** \brief Get the half axis of the bounding box.
        *
        * This routine will return the half axis of the volume provided.
        *
        * \param b The bounding box to get the half axis from.
        *
        * \return Returns a vector that's half the width, height and depth of
        * the bounding box.
        */

      e_forceinline friend Vector3 e_half_axis( const AABB3& b ){
        return b.half();
      }

      /** \brief Get the half axis of the bounding box.
        *
        * \return Returns a vector that's half the width, height and depth of
        * the bounding box.
        */

      e_forceinline Vector3 half()const{
        e_assert( valid() );
        return Vector3( width()*kHalf, depth()*kHalf, height()*kHalf );
      }

      /** @}
        *
        * \name Conversion to other types.
        *
        * These methods will convert the bounding box to other types such as
        * spheres, vectors and cones.
        *
        * @{
        */

      /** \brief Get the capsule enclosing this bounding box.
        *
        * This routine calculates a new capsule from the Z axis.
        *
        * \param b The bounding box to convert.
        *
        * \return Returns a capsule object that fully contains this volume.
        */

      e_forceinline friend Capsule e_capsuleOf( const AABB3& b ){
        return b.toCapsule();
      }

      /** \brief Get the capsule enclosing this bounding box.
        *
        * This routine calculates a new capsule from the Z axis.
        *
        * \return Returns a capsule object that fully contains this volume.
        */

      e_forceinline Capsule toCapsule()const{
        e_assert( valid() );
        const auto& origin = toOrigin();
        const auto& begins = Point3( origin.x, origin.y, min.z );
        const auto& ending = Point3( origin.x, origin.y, max.z );
        return Capsule( begins, ending, toRadius() );
      }

      /** \brief Get the sphere enclosing this bounding box.
        *
        * This routine calculates a new sphere from the maximum axis.
        *
        * \param b The bounding box to convert.
        *
        * \return Returns a Sphere object that fully contains this volume.
        */

      e_forceinline friend Sphere e_sphereOf( const AABB3& b ){
        return b.toSphere();
      }

      /** \brief Get the sphere enclosing this bounding box.
        *
        * This routine calculates a new sphere from the maximum axis.
        *
        * \return Returns a Sphere object that fully contains this volume.
        */

      e_forceinline Sphere toSphere()const{
        e_assert( valid() );
        return Sphere( toOrigin(), toRadius() );
      }

      /** \brief Get the bounding box origin.
        *
        * This routine will return the bounding box's origin or the center of
        * the volume.
        *
        * \return Returns the center of the volume.
        */

      e_forceinline Point3 toOrigin()const{
        e_assert( valid() );
        return( min + max )*kHalf;
      }

      /** \brief Get radius of bounding volume.
        *
        * This routine will return the best (average) radius of the bounds.
        *
        * \return Returns the average radius of this bounding volume.
        */

      e_forceinline f32 toRadius()const{
        return half().ave();
      }

      /** @}
        *
        * \name Offsetting methods.
        *
        * These methods will offset the bounding box by a vector offset.
        *
        * @{
        */

      /** \brief Offset by 3D vector.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline friend AABB3 e_offsetBy( const AABB3& b, const Vector3& vOffset ){
        return b.offsetBy( vOffset );
      }

      /** \brief Offset by 3D vector.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3 offsetBy( const Vector3& vOffset )const{
        e_assert( valid() && vOffset.valid() );
        return (*this) + vOffset;
      }

      /** \brief Offset by 2D vector parallel to floor plane.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3 offsetBy( const Vector2& vOffset )const{
        e_assert( valid() && vOffset.valid() );
        return (*this) + vOffset;
      }

      /** \brief Offset by 2D vector parallel to floor plane.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline friend AABB3 e_offsetBy( const AABB3& b, const Vector2& vOffset ){
        return b.offsetBy( vOffset );
      }

      /** \brief Offset by 3D vector.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3& offsetBy( const Vector3& vOffset ){
        e_assert( valid() && vOffset.valid() );
        min += vOffset;
        max += vOffset;
        return *this;
      }

      /** \brief Offset by 2D vector parallel to floor plane.
        *
        * This routine will offset the axis-aligned bounding box by the vector
        * given. This is simply by adding the point to both min and max points.
        *
        * \param vOffset The point to offset this
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3& offsetBy( const Vector2& vOffset ){
        e_assert( valid() && vOffset.valid() );
        min.x += vOffset.x;
        min.y += vOffset.y;
        max.x += vOffset.x;
        max.y += vOffset.y;
        return *this;
      }

      /** @}
        *
        * \name Scaling methods.
        *
        * These methods will scale the bounding box in various ways.
        *
        * @{
        */

      /** \brief Geometric scale.
        *
        * This routine will scale the bounding box geometrically or in other
        * words from the center of the volume. This is great for scaling 3D
        * objects from their bounding volume's origin rather than both min and
        * max points the same.
        *
        * \param scalar The amount to scale the min and max points in their
        * respective directions.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB3& scaleFromOrigin( const self& scalar ){
        e_assert( valid() && scalar.valid() );
        min += ( min - toOrigin() ).normalized() * scalar;
        max += ( max + toOrigin() ).normalized() * scalar;
        return *this;
      }

      /** @}
        *
        * \name Axis metrics.
        *
        * These methods will return the width, depth or height of the bounds.
        *
        * @{
        */

      /** \brief Get width of bounding box.
        *
        * This routine calculates the width of the bounding box in units. What
        * those units are is up to the caller.
        */

      e_forceinline self width()const{
        e_assert( valid() );
        return( max.x - min.x );
      }

      /** \brief Get height of bounding box.
        *
        * This routine calculates the height of the bounding box in units. What
        * those units are is up to the caller.
        */

      e_forceinline self depth()const{
        e_assert( valid() );
        return( max.y - min.y );
      }

      /** \brief Get depth of bounding box.
        *
        * This routine calculates the depth of the bounding box in units. What
        * those units are is up to the caller.
        */

       e_forceinline self height()const{
        e_assert( valid() );
        return( max.z - min.z );
      }

      /** @}
        *
        * \name Intersection test methods.
        *
        * These methods will return true for a variety of intersection tests.
        *
        * @{
        */

      /** \brief Ray intersection test.
        *
        * This routine will test the AABB against the ray.
        *
        * \param b The bounding volume to test for intersection with the ray.
        * \param r The input ray to test against.
        * \param dist The output distance to the box.
        *
        * \return True if collision and false otherwise.
        */

      e_forceinline friend bool e_intersects( const AABB3& b, const Ray3& r, self& dist ){
        return b.intersects( r, dist );
      }

      /** \brief Ray intersection test.
        *
        * This routine will test the AABB against the ray.
        *
        * \param r The input ray to test again.
        * \param dist The distance to the box.
        * \return True if collision and false otherwise.
        */

      bool intersects( const Ray3& r, self& dist )const;

      /** \brief Ray intersection test.
        *
        * This routine will test the AABB against the ray.
        *
        * \param b The bounding volume to test for intersection with the ray.
        * \param r The input ray to test against.
        *
        * \return True if collision and false otherwise.
        */

      e_forceinline friend bool e_intersects( const AABB3& b, const Ray3& r ){
        return b.intersects( r );
      }

      /** \brief Ray intersection test.
        *
        * This routine will test the AABB against the ray.
        *
        * \param r The input ray to test again.
        * \return True if collision and false otherwise.
        */

      e_forceinline bool intersects( const Ray3& r )const{
        self d; return intersects( r, d );
      }

      /** \brief Check if primitive is contained by bounding box.
        *
        * This routine will return true if the primitive is contained by the
        * bounding box and false otherwise.
        */

      e_forceinline friend bool e_contains( const AABB3& b, const Triangle& t ){
        return b.contains( t );
      }

      /** \brief Check if primitive is contained by bounding box.
        *
        * This routine will return true if the primitive is contained by the
        * bounding box and false otherwise.
        */

      bool contains( const Triangle& )const;

      /** \brief Check if point is contained by bounding box.
        *
        * This routine will return true if the point is contained by the
        * bounding box and false otherwise.
        */

      e_forceinline friend bool e_contains( const AABB3& b, const Point3& p ){
        return b.contains( p );
      }

      /** \brief Check if point is contained by bounding box.
        *
        * This routine will return true if the point is contained by the
        * bounding box and false otherwise.
        */

      bool contains( const Point3& )const;

      /** \brief Check if aabb is contained by bounding box.
        *
        * This routine will return true if the aabb is contained by the
        * bounding box and false otherwise.
        */

      e_forceinline friend bool e_contains( const AABB3& a, const AABB3& b ){
        return a.contains( b );
      }

      /** \brief Check if aabb is contained by bounding box.
        *
        * This routine will return true if the aabb is contained by the
        * bounding box and false otherwise.
        */

      bool contains( const AABB3& )const;

      /** @}
        *
        * \name Clearing and resetting the bounds.
        *
        * These methods will clear the bounds in a variety of ways. Clear will
        * reset the bounds to it's opposite numerical extremes while zero set
        * the min and max points to the world origin.
        *
        * @{
        */

      /** \brief Reset the bounding box.
        *
        * This method will clear the bounding box or return it to defaults
        * ready to start adding points again.
        */

      e_forceinline friend void e_reset( AABB3& b ){
        b.reset();
      }

      /** \brief Reset the bounding box.
        *
        * This method will clear the bounding box or return it to defaults
        * ready to start adding points again.
        */

      void reset();

      /** \brief Set bounds to zero.
        *
        * This routine will clear out the bounding box to zero. If you have no
        * intention of adding points then calls this method after construction.
        */

      e_forceinline friend void e_clear( AABB3& b ){
        b.clear();
      }

      /** \brief Set bounds to zero.
        *
        * This routine will clear out the bounding box to zero. If you have no
        * intention of adding points then calls this method after construction.
        */

      e_forceinline void clear(){
        min.clear();
        max.clear();
      }

      /** @} */

    //}:                                          |
    //--------------------------------------------+-----------------------------

    /** \brief 3D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      *
      * \param fMinX The minimum X coordinate.
      * \param fMinY The minimum Y coordinate.
      * \param fMinZ The minimum Z coordinate.
      * \param fMaxX The maximum X coordinate.
      * \param fMaxY The maximum Y coordinate.
      * \param fMaxZ The maximum Z coordinate.
      */

    e_forceinline_always AABB3( const self& fMinX, const self& fMinY, const self& fMinZ, const self& fMaxX, const self& fMaxY, const self& fMaxZ )
      : min( fMinX, fMinY, fMinZ )
      , max( fMaxX, fMaxY, fMaxZ )
    {}

    /** \brief 3D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      *
      * \param fMinX The minimum X coordinate.
      * \param fMinY The minimum Y coordinate.
      * \param fMinZ The minimum Z coordinate.
      * \param fMaxX The maximum X coordinate.
      * \param fMaxY The maximum Y coordinate.
      * \param fMaxZ The maximum Z coordinate.
      */

    e_forceinline_always AABB3( const float fMinX, const float fMinY, const float fMinZ, const float fMaxX, const float fMaxY, const float fMaxZ )
      : min( fMinX, fMinY, fMinZ )
      , max( fMaxX, fMaxY, fMaxZ )
    {}

    /** \brief 3D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      *
      * \param in_min The minimum point in local or world space.
      * \param in_max The maximum point in local or world space.
      */

    e_forceinline_always AABB3( const Point3& in_min, const Point3& in_max )
      : min( in_min )
      , max( in_max )
    {}

    /** \brief 3D axis-aligned bounding box constructor.
      *
      * This constructor will build an AABB from a single scalar.
      *
      * \param scalar The scalar to apply to all points.
      */

    e_forceinline_always AABB3( const self& scalar )
      : min( -scalar )
      , max(  scalar )
    {}

    /** \brief 3D axis-aligned bounding box constructor.
      *
      * This constructor will build an AABB from a single scalar.
      *
      * \param scalar The scalar to apply to all points.
      */

    e_forceinline_always AABB3( const float scalar )
      : min( -scalar )
      , max(  scalar )
    {}

    /** \brief Construct bounding box from sphere.
      *
      * This routine will inititalize the bounding volume from a sphere.
      *
      * \param s The sphere to initialize the bounds with.
      */

    e_forceinline_always AABB3( const Sphere& s )
      : min( s.toCenter().x - s.toRadius(), s.toCenter().y - s.toRadius(), s.toCenter().z - s.toRadius() )
      , max( s.toCenter().x + s.toRadius(), s.toCenter().y + s.toRadius(), s.toCenter().z + s.toRadius() )
    {}

    /** \brief Default AABB constructor.
      *
      * Initializes the bounding box to the opposite extremes so you can easily
      * say "add point to bounds".
      *
      \code
        AABB3 aabb;
        for( int i=0, n=vPoints.size(); i<n; ++i ){
          const pt3& p = vPoints[i];
          aabb += p;
        }
      \endcode
      */

    e_forceinline_always AABB3()
      : min( kMax, kMax, kMax )
      , max( kMin, kMin, kMin )
    {}
  };

//}:                                              |
//================================================+=============================
