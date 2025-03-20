//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Frustum:{                                       |

	/** \brief Frustum class.
		*
		* The frustum is generally used for culling objects in a scene. There is
		* one attached to the sg camera.
		*/

	struct E_PUBLISH Frustum final{

    //--------------------------------------------+-----------------------------
    //Methods:{                                   |

      /** \brief Setup the frustum.
        *
        * This routine will setup the viewing frustum.
        *
        * \param W2C Matrix to build the frustum from.
        */

      void set( const Matrix4& W2C );

      /** \brief Is point visible.
        *
        * This routine will check to see if the given point is on the inside of
        * all planes in the frustum.
        *
        * \param P The point to test.
        *
        * \return Returns if the point is inside the frustum.
        */

      bool intersects( const Point3& P )const;

      /** \brief Is sphere visible.
        *
        * This routine will check to see if the given sphere is on the inside
        * of all planes in the frustum.
        *
        * \param S The sphere to test.
        *
        * \return Returns if the frustum intersects the given sphere.
        */

      bool intersects( const Sphere& S )const;

      /** \brief Is bounds visible.
        *
        * This routine will check to see if the given bounds is on the inside
        * of all planes in the frustum.
        *
        * \param B The bounding box to test.
        */

      e_noinline bool intersects( const AABB3& B )const{
        Point3 a;
        Point3 b;
        for( u32 i=0; i<6; ++i ){
          const auto& next = m_aPlanes[ i ];
          if( next.Vector4::x >= 0.f ){
            a.x = B.min.x;
            b.x = B.max.x;
          }else{
            b.x = B.min.x;
            a.x = B.max.x;
          }
          if( next.Vector4::y >= 0.f ){
            a.y = B.min.y;
            b.y = B.max.y;
          }else{
            b.y = B.min.y;
            a.y = B.max.y;
          }
          if( next.Vector4::z >=0.f ){
            a.z = B.min.z;
            b.z = B.max.z;
          }else{
            b.z = B.min.z;
            a.z = B.max.z;
          }
          int l1;{
            const auto& d = next.signedDistance( a );
            if( d > kEpsilon ){
              l1 = 1;
            }else if( d < -kEpsilon ){
              l1 = -1;
            }else{
              l1 = 0;
            }
          }
          int l2;{
            const auto& d = next.signedDistance( b );
            if( d > kEpsilon ){
              l2 = 1;
            }else if( d < -kEpsilon ){
              l2 = -1;
            }else{
              l2 = 0;
            }
          }
          if(( l1 == l2 )&&( l1 < 0 )){
            return false;
          }
        }
        return true;
      }

      /** \brief Set a plane.
        *
        * This routine will set the given plane.
        *
        * \param i The plane index (0-5).
        * \param p The plane.
        */

      e_forceinline void setPlane( const s32 i, const Plane& p ){
        e_assert(  i < e_sizeof( m_aPlanes ));
        m_aPlanes[ i ] = p;
      }

      /** \brief Get a plane.
        *
        * This routine will return the plane at the given index.
        *
        * \param i The index to the desired plane.
        *
        * \return Returns a reference to the requested plane.
        */

      e_forceinline Plane& toPlane( const s32 i ){
        e_assert( i < e_sizeof( m_aPlanes ));
        return m_aPlanes[ i ];
      }

    //}:                                          |
    //--------------------------------------------+-----------------------------

    Frustum() = default;

	private:

		Plane m_aPlanes[6];
	};

//}:                                              |
//================================================+=============================
