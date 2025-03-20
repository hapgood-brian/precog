//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//AABB2:{                                         |

  /** \brief The 2D axis-aligned bounding box.
    *
    * This class defines the two dimension axis-aligned bounding box. The
    * entire UI system is built on the back of this object. All 2D bounds are
    * expressed that way. Like it's three dimensional cousin the AABB2 defaults
    * to the opposite numerical extremes for min and max. Then we can add 2D
    * points to it to construct a bounds. If you want to initialize your
    * bounding box to zero use the built in constant kZero.
    *
    * Do not mark this class "final" because it's inherited by the texture
    * Region class.
    */

  struct E_PUBLISH AABB2{

    Point2 min;
    Point2 max;

    //--------------------------------------------+-----------------------------
    //Statics:{                                   |

      static const AABB2 kFullUV; //!< Full UV bounding box.
      static const AABB2 kInvUV;  //!< Inverted UV bounding box.
      static const AABB2 kZero;   //!< Zero bounding box.

    //}:                                          |
    //Operate:{                                   |

      /** \name Axis-aligned bounding box operators.
        *
        * These routines perform various operations on the AABB.
        *
        * @{
        */

      /** \brief Assignment operator.
        *
        * This operator will assign an AABB to this one.
        *
        * \param b The incoming bounds to assign.
        */

      e_forceinline AABB2& operator=( const AABB2& b ){
        e_assert( b.valid() );
        min = b.min;
        max = b.max;
        return *this;
      }

      /** \brief Inequality operator.
        *
        * This operator tests the equality of two bounding boxes.
        *
        * \param b The bounding box to compare.
        *
        * \return Returns true if the two bounds are unequal or false.
        */

      e_forceinline bool operator!=( const AABB2& b )const{
        e_assert( valid() );
        if( min != b.min ){
          return true;
        }
        if( max != b.max ){
          return true;
        }
        return false;
      }

      /** \brief Equality operator.
        *
        * This operator tests the equality of two bounding boxes.
        *
        * \param b The bounding box to compare.
        *
        * \return Returns true if the two bounds are equal or false.
        */

      e_forceinline bool operator==( const AABB2& b )const{
        e_assert( valid() );
        if( min == b.min ){
          return true;
        }
        if( max == b.max ){
          return true;
        }
        return false;
      }

      /** \brief Additive operator.
        *
        * This operator just adds the minimum point to the given point. The
        * behavior is very different from the += operator.
        *
        * \param v A point to add to min and max to be returned to caller.
        *
        * \return Returns a new bounding box.
        */

      e_forceinline AABB2 operator+( const Vector2& v )const{
        e_assert( valid() && v.valid() );
        AABB2 a;
        a.min = min + v;
        a.max = max + v;
        return a;
      }

      /** \brief Additive operator.
        *
        * This operator just adds the minimum point to the given point. The
        * behavior is very different from the += operator.
        *
        * \param p A point to add to min and max to be returned to caller.
        *
        * \return Returns a new bounding box.
        */

      e_forceinline AABB2 operator+( const Point2& p )const{
        e_assert( valid() && p.valid() );
        AABB2 a;
        a.min = min + p;
        a.max = max + p;
        return a;
      }

      /** \brief Additive operator.
        *
        * This operator adds the bounding box to this one and returns the
        * result without destroying this.
        *
        * \param b The bounding box to add to this one.
        *
        * \return Returns the expanded bounding box with the new bounds added
        * in.
        */

      e_forceinline AABB2 operator+( const AABB2& b )const{
        e_assert( valid() && b.valid() );
        AABB2 a = *this;
        a += b.min;
        a += b.max;
        return a;
      }

      /** \brief Additive operator.
        *
        * This routine will add a point to the current bounding box. This will
        * grow the bounds as necessary. It's the method used for building boxes
        * from 2D geometry.
        *
        * \param v The point to add and grow the AABB.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB2& operator+=( const Vector2& v ){
        e_assert( valid() && v.valid() );
        if( min.x > v.x ) min.x = v.x;
        if( min.y > v.y ) min.y = v.y;
        if( max.x < v.x ) max.x = v.x;
        if( max.y < v.y ) max.y = v.y;
        return *this;
      }

      /** \brief Additive operator.
        *
        * This routine will add a point to the current bounding box. This will
        * grow the bounds as necessary. It's the method used for building boxes
        * from 2D geometry.
        *
        * \param p The point to add and grow the AABB.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB2& operator+=( const Point2& p ){
        e_assert( valid() && p.valid() );
        if( min.x > p.x ) min.x = p.x;
        if( min.y > p.y ) min.y = p.y;
        if( max.x < p.x ) max.x = p.x;
        if( max.y < p.y ) max.y = p.y;
        return *this;
      }

      /** \brief Additive operator.
        *
        * This operator will add a bounding box to this bounding box.
        *
        * \param b The bounding box with points to add to this.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB2& operator+=( const AABB2& b ){
        e_assert( valid() && b.valid() );
        *this += b.min;
        *this += b.max;
        return *this;
      }

      /** \brief Multiplicative operator.
        *
        * This operator will multiply a scale with this bounding box.
        *
        * \param scalar The amount to scale this bounding box.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB2& operator*=( const self& scalar ){
        min.x *= scalar;
        min.y *= scalar;
        max.x *= scalar;
        max.y *= scalar;
        return *this;
      }

      /** \brief Multiplicative operator.
        *
        * This operator will multiply a scale with this bounding box.
        *
        * \param scalar The amount to scale this bounding box.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline AABB2& operator*=( const float scalar ){
        min.x *= scalar;
        min.y *= scalar;
        max.x *= scalar;
        max.y *= scalar;
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
        * \name Dimensions.
        *
        * These methods will set and query the dimensions of the axis-aligned
        * bounding box (2D).
        *
        * @{
        */

      /** \brief Get width of bounding box.
        *
        * This routine calculates the width of the bounding box in units. What
        * those units are is up to the caller.
        *
        * \return Returns the width of the bounding box as a real number.
        */

      e_forceinline self width()const{
        e_assert( valid() );
        return max.x-min.x;
      }

      /** \brief Get height of bounding box.
        *
        * This routine calculates the height of the bounding box in units. What
        * those units are is up to the caller.
        *
        * \return Returns the height of the bounding box as a real number.
        */

      e_forceinline self height()const{
        e_assert( valid() );
        return max.y-min.y;
      }

      /** \brief Set the height of the bounding box.
        *
        * This routine just set the height of the bounds.
        *
        * \param h The new height of the bounding box.
        */

      e_forceinline void setHeight( const self& h ){
        max.y = min.y + h;
        e_assert( valid() );
      }

      /** \brief Set the width of the bounding box.
        *
        * This routine just set the height of the bounds.
        *
        * \param w The new width of the bounding box.
        */

      e_forceinline void setWidth( const self& w ){
        max.x = min.x + w;
        e_assert( valid() );
      }

      /** @}
        *
        * \name Scaling methods.
        *
        * These methods have everything to do with scaling the axis-aligned
        * bounding box.
        *
        * @{
        */

      /** \brief Convert bounding box to clip space.
        *
        * This routine will convert the bounding box to clip space using the
        * scale provided.
        *
        * \param scalar The scale to be applied when transferring the bounds to
        * projected clip space.
        *
        * \return Returns the bounding box in scaled clip space.
        */

      AABB2 scaledToClip( const self& scalar )const;

      /** \brief Convert bounding box to clip space.
        *
        * This routine will convert the bounding box to clip space using the
        * scale provided.
        *
        * \param scalar The scale to be applied when transferring the bounds to
        * projected clip space.
        */

      void scaleToClip( const self& scalar );

      /** \brief Convert bounding box from clip space back into screen.
        *
        * This routine will take a bounding box that's had scaleToClip called
        * on it and return it to screen space.
        */

      void scaleBack();

      /** @}
        *
        * \name Translation.
        *
        * These methods allow you to translate the bounding box in 2D space.
        *
        * @{
        */

      /** \brief Translate bounds.
        *
        * This routine will move the bounds in 2D space by a delta amount.
        *
        * \param delta A two-tuple vector to translate by.
        */

      e_forceinline void translate( const Vector2& delta ){
        min += delta;
        max += delta;
      }

      /** \brief Set position.
        *
        * This routine will set the location of the bounding box by its minimum
        * corner.
        *
        * \param p The new position of the bounding box.
        */

      e_forceinline AABB2& setPosition( const Point2& p ){
        const Vector2 v = Vector2( width(), height() );
        min = p;
        max = p + v;
        e_assert( valid() );
        return *this;
      }

      /** \brief Set origin.
        *
        * This routine will set the location of the bounding box by its origin.
        *
        * \param p The new origin of the bounding box.
        */

      e_forceinline AABB2& setOrigin( const Point2& p ){
        const Vector2& h = Vector2( max - min ) * kHalf;
        min = p - h;
        max = p + h;
        e_assert( valid() );
        return *this;
      }

      /** @}
        *
        * \name Snapping.
        *
        * These methods have to do with snapping the axis-aligned bounding box
        * to the floor of each component.
        *
        * @{
        */

      /** \brief Snap.
        *
        * This routine will snap the bounding box to whole integer values.
        */

      e_forceinline AABB2 snapped()const{
        e_assert( valid() );
        AABB2 b( *this );
        b.snap();
        return b;
      }

      /** \brief Snap.
        *
        * This routine will snap the bounding box to whole integer values.
        */

      e_forceinline void snap(){
        min.x = floor( round( min.x ));
        min.y = floor( round( min.y ));
        max.x = floor( round( max.x ));
        max.y = floor( round( max.y ));
        e_assert( valid() );
      }

      /** @}
        *
        * \name Clearing axis-aligned bounding box.
        *
        * These methods have to do with clearing the 2D axis-aligned box to
        * zero.
        *
        * @{
        */

      /** \brief Set unit bounds.
        *
        * This routine will reinitialize the bounding box to unit bounds. This
        * is very useful for setting up a clip space.
        */

      e_forceinline void unit(){
        min.x = min.y = -1.f;
        max.x = max.y = +1.f;
        e_assert( valid() );
      }

      /** \brief Set bounds to zero.
        *
        * This routine will reinitialize the bounding box to zero.
        */

      e_forceinline void clear(){
        min.clear();
        max.clear();
      }

      /** \brief Clear the bounding box.
        *
        * This method will clear the bounding box or return it to defaults
        * ready to start adding points again.
        */

      void reset();

      /** @}
        *
        * \name Intersection tests.
        *
        * These methods perform various intersection and containment tests on
        * the 2D axis-aligned bounding box.
        *
        * @{
        */

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

      bool contains( const Point2& )const;

      /** \brief Check if aabb is contained by bounding box.
        *
        * This routine will return true if the aabb is contained by the
        * bounding box and false otherwise.
        */

      bool contains( const AABB2& )const;

      /** \brief Check if bounding box intersects this bounds.
        *
        * This routine checks to see if the two bounding boxes overlap.
        *
        * \param b Bounds to test against.
        * \return Returns true if the two bounding boxes overlap and false
        * otherwise.
        */

      bool intersects( const AABB2& b )const;

      /** \brief Check if ray intersects this plane.
        *
        * Routine checks to see if the given ray intersects this bounding box.
        */

      bool intersects( const Ray2& )const;

      /** @}
        *
        * \name Expansion and reduction.
        *
        * These methods expand or shrink the 2D axis-aligned bounding box.
        *
        * @{
        */

      /** \brief Shrink bounding box.
        *
        * This routine will shrink the bounding box by the given amount.
        *
        * \param amount The amount to shrink the box by.
        */

      e_noinline void shrink( const self& amount ){
        min.x += amount;
        min.y += amount;
        max.x -= amount;
        max.y -= amount;
        if( max.x < min.x ){
          max.x = min.x+1.f;
        }
        if( max.y < min.y ){
          max.y = min.y+1.f;
        }
        e_assert( valid() );
      }

      /** \brief Grow bounding box.
        *
        * This routine will grow the bounding box by the given amount.
        *
        * \param amount The amount to grow the box by.
        */

      e_forceinline void grow( const self& amount ){
        min.x -= amount;
        min.y -= amount;
        max.x += amount;
        max.y += amount;
        e_assert( valid() );
      }

      /** @}
        *
        * \name Geometric methods.
        *
        * These methods allow one to perform various geometric methods and
        * clipping against the 2D axis-aligned bounding box.
        *
        * @{
        */

      /** \brief Return center.
        *
        * This routine will return the center of the bounding box.
        */

      e_forceinline Point2 center()const{
        e_assert( valid() );
        return Point2( min.x+width()*kHalf, min.y+height()*kHalf );
      }

      /** \brief Return circle.
        *
        * This routine will return the circle that contains the bounding box.
        */

      Circle circle()const;

      /** \brief Clip bounding box against another.
        *
        * This routine will clip this bounding box against the one provided. It
        * is used primarily for clipping scissor rects in the UI system.
        *
        * \param b The bounding box to clip against this one.
        *
        * \return Returns true if the two bounding boxes at least intersect and
        * false otherwise.
        */

      bool clipAgainst( const AABB2& b );

      /** \brief Is valid.
        *
        * This routine will return true if the bounding box is valid.
        */

      bool valid()const;

      /** \brief Is empty.
        *
        * This routine will return true if no points have been added to the
        * bounding box (it is at reset extents) or false otherwise.
        */

      bool empty()const;

      /** @} */

    //}:                                          |
    //--------------------------------------------+-----------------------------

    /** \brief 2D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      */

    e_forceinline_always AABB2( const self& xMin, const self& yMin, const self& xMax, const self& yMax )
      : min( xMin, yMin )
      , max( xMax, yMax )
    {}

    /** \brief 2D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      */

    e_forceinline_always AABB2( const float xMin, const float yMin, const float xMax, const float yMax )
      : min( xMin, yMin )
      , max( xMax, yMax )
    {}

    /** \brief 2D axis-aligned bounding box constructor.
      *
      * This is the constructor for an axis aligned bounding box. It takes
      * two points for the min and max extents.
      */

    e_forceinline_always AABB2( const Point2& in_min, const Point2& in_max )
      : min( in_min )
      , max( in_max )
    {}

    /** \brief Initialize 2D axis-aligned bounding box with single value.
      *
      * This constructor will initialize all members of the bounds to the given
      * value.
      *
      * \param X The value to apply to min and max. Usually zero.
      */

    e_forceinline_always AABB2( const self& X )
      : min( X )
      , max( X )
    {}

    /** \brief Initialize 2D axis-aligned bounding box with single value.
      *
      * This constructor will initialize all members of the bounds to the given
      * value.
      *
      * \param X The value to apply to min and max. Usually zero.
      */

    e_forceinline_always AABB2( const float X )
      : min( X )
      , max( X )
    {}

    /** \brief Default 2D AABB constructor.
      *
      * Initializes the bounding box to the opposite extremes so you can easily
      * say "add point to bounds".
      \code
      AABB2 aabb;
      for( int i=0, n=vPoints.size(); i<n; ++i ){
        const pt2& p = vPoints[i];
        aabb += p;
      }
      \endcode
      */

    e_forceinline_always AABB2()
      : min( kMax, kMax )
      , max( kMin, kMin )
    {}
  };

//}:                                              |
//================================================+=============================
