//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

//================================================+=============================
//Triangle:{                                      |

  struct E_PUBLISH Triangle{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |

      /** \brief Rasterization interface.
        *
        * This interface is used by the Triangle::draw method to rasterize. The
        * user implements an IRasterizer::set method which gets called for each
        * texel in the surface. The eon compiler's light mapper uses this for
        * computing lumels at each light map U and V for example.
        */

      struct E_PUBLISH IRasterizer{

        virtual~IRasterizer() = default;

        /** \brief Draw texel into surface.
          *
          * This routine is responsible for setting the color of a texel during
          * rendering of a triangle. For the light mapper, for example, it draws
          * rays into a scene and computes lighting contribution, indirect
          * lighting and ambient occlusion then adds result to the texel given.
          *
          * \param x X coordinate of pixel to draw.
          * \param y Y coordinate of pixel to draw.
          * \param uv The coordinate of the texel in UV space.
          */

        virtual void onDraw( const s32 x, const s32 y, const Vector2& uv )const=0;

        /** \brief Test texel at coordinate.
          *
          * This routine will return true if the pixel is touched and false
          * otherwise.
          */

        virtual bool onTest( const s32 x, const s32 y )const{
          return false;
        }

        /** \brief Get surface height.
          *
          * This routine will return the height of the surface in pixels.
          *
          * \return The surface height.
          */

        virtual u32 toH()const=0;

        /** \brief Get surface width.
          *
          * This routine will return the width of the surface in pixels.
          *
          * \return The surface width.
          */

        virtual u32 toW()const=0;
      };

    //}:                                          |
    //Operate:{                                   |

      /** \brief Assignment operator.
        *
        * This operator assigns one triangle to another.
        *
        * \param t The triangle to assign.
        *
        * \return Returns the *this pointer.
        */

      Triangle& operator=( const Triangle& t ){
        A = t.A;
        B = t.B;
        C = t.C;
        return *this;
      }

      /** \brief Bracket operator.
        *
        * This operator returns one of three points.
        *
        * \param i The index of range zero through two.
        *
        * \return Returns one of the points of the triangle.
        */

      const Point3& operator[]( const u32 i )const{
        return( &A )[ i ];
      }

      /** \brief Bracket operator.
        *
        * This operator returns one of three points.
        *
        * \param i The index of range zero through two.
        *
        * \return Returns one of the points of the triangle.
        */

      Point3& operator[]( const u32 i ){
        return( &A )[ i ];
      }

    //}:                                          |
    //Methods:{                                   |

      /** \brief Rasterize the triangle.
        *
        * This routine will rasterize the triangle. For this to work properly
        * it is assumed that the Z component of A, B and C are zero (they'll
        * just be ignored) thereby intrinsically flattening the polygon. A
        * callback function can be supplied that is called for every pixel
        * drawn. This can be NULL. Triangle::draw's main use is for the light
        * mapper in the eon compiler.
        *
        * \param rasterizer User implemented rasterizer object.
        * \param bTestPrimitive Does not draw primitive if any pixels beneath
        * it have a non-zero alpha value.
        * \return Returns true if texels were written and false otherwise. Each
        * texel's alpha is tested to be zero before writing.
        */

      bool draw( IRasterizer& rasterizer, const bool bTestPrimitive )const;

      /** \brief Test intersection of ray.
        *
        * This routine will test for a collision between this triangle and the
        * given ray. The result is returned in 'hit' and 'norm'.
        *
        * \param L2W The coordinate frame that the ray is working in.
        * \param ray The ray to test triangle against for intersection.
        * \param hit Receives the point of the intersection.
        * \param norm Receives the normal of this triangle; same as member N.
        *
        * \return Returns true if intersection found and false otherwise.
        */

      bool intersects( const QST3& L2W, const Ray3& ray, Point3& hit, Vector3& norm )const;

      /** \brief Test intersection of ray.
        *
        * This routine will test for a collision between this triangle and the
        * given ray. The result is returned in 'hit' and 'norm'.
        *
        * \param L2W The coordinate frame that the ray is working in.
        * \param ray The ray to test triangle against for intersection.
        * \param hit Receives the point of the intersection.
        * \param norm Receives the normal of this triangle; same as member N.
        *
        * \return Returns true if intersection found and false otherwise.
        */

      bool intersects( const Matrix4& L2W, const Ray3& ray, Point3& hit, Vector3& norm )const;

      /** \brief Test intersection of ray.
        *
        * This routine will test for a collision between this triangle and the
        * given ray. The result is returned in 'hit' and 'norm'.
        *
        * \param ray The ray to test triangle against for intersection.
        * \param hit Receives the point of the intersection.
        * \param norm Receives the normal of this triangle; same as member N.
        *
        * \return Returns true if intersection found and false otherwise.
        */

      bool intersects( const Ray3& ray, Point3& hit, Vector3& norm )const;

      /** \brief Get bounds of triangle.
        *
        * This routine will return the bounds of this triangle in 2 dimensions.
        *
        * \return Returns a 2D axis-aligned bounding box of the triangle.
        */

      AABB2 toBounds2D()const;

      /** \brief Get bounds of triangle.
        *
        * This routine will return the bounds of this triangle in 3 dimensions.
        *
        * \return Returns a 3D axis-aligned bounding box of the triangle.
        */

      AABB3 toBounds()const;

      /** \brief Get plane of the triangle.
        *
        * This routine calculates the plane of the triangle and returns it.
        *
        * \return Returns the plane of the triangle based on the normal.
        */

      e_forceinline Plane getPlane()const{
        return Plane( *this );
      }

      /** \brief Calculate normal.
        *
        * This routine will calculate the normal from the three positions A, B
        * and C.
        *
        * \return Returns the result (N reference).
        */

      e_noinline Vector3 calcNormal()const{
        const Vector3& BA = B-A;
        const Vector3& CA = C-A;
        Vector3 N = BA.cross( CA );
        N.normalize();
        return N;
      }

      /** \brief Calculate normal and store.
        *
        * This routine will calculate the normal from the three positions A, B
        * and C. It sets the N component with the result.
        *
        * \return Returns the result (N reference).
        */

      e_noinline Vector3& calcNormal(){
        const Vector3& e1 = B-A;
        const Vector3& e2 = C-A;
        N = e1.cross( e2 );
        N.normalize();
        return N;
      }

      /** \brief Get the triangle centroid.
        *
        * This routine will calculate the centroid and return it.
        */

      e_forceinline Point3& centroid()const{
        return( A + B + C )/T( 3 );
      }

    //}:                                          |
    //--------------------------------------------+-----------------------------

    Triangle( const Point3& a, const Point3& b, const Point3& c )
      : A( a )
      , B( b )
      , C( c )
    {}

    explicit Triangle( const Triangle& t ){
      A = t.A;
      B = t.B;
      C = t.C;
    }

    Triangle() = default;

    Point3 A, B, C;
    Vector3 N;
    Vector2 uv[ 3 ];
    RGBa color;
  };

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Collision:{                                     |

  struct E_PUBLISH Collision final{
    Triangle poly;
    Vector3  norm;
    Point3   hit;
    u32      uid = 0;
  };

//}:                                              |
//================================================+=============================
