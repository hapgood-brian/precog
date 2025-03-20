//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

/** \addtogroup engine
  * @{
  *   \addtogroup gfc
  *   @{
  *     \addtogroup AutoRef
  *     @{
  */

//================================================+=============================
//AutoRef:{                                       |

  namespace EON{

    namespace gfc{

      /** \brief Auto-reference counting class.
        *
        * This class is used as a handle throughout the engine, most notably in
        * places where memory is tight and we frequently need big clouds of
        * objects evaporating when a level ends. The biggest user of the
        * AutoRef is the renderer. All textures, meshes and terrains (for
        * example) define AutoRef's handles through their e_reflect()
        * statements.
        *
        * \tparam T The type of the auto-referencing object.
        */

      template<typename T> struct E_PUBLISH AutoRef final{

        //----------------------------------------+-----------------------------
        //Aliases:{                               |

          using Factory = EON::gfc::Class::Factory;
          using Class   = EON::gfc::Class;
          using self    = T;

        //}:                                      |
        //Operate:{                               |

          /** \brief Inequality operator.
            *
            * This operator checks if two autorefs of the same type are
            * different at the UUID level.
            *
            * \param aref The autoref to compare with this one.
            *
            * \return Returns true if the two autorefs have the same UUID and
            * are therefore equal.
            */

          e_forceinline bool operator!=( const AutoRef& aref )const{
            return( UUID != aref.UUID );
          }

          /** \brief Equality operator.
            *
            * This operator checks if two autorefs of the same type are the same
            * at the UUID level.
            *
            * \param aref The autoref to compare with this one.
            *
            * \return Returns true if the two autorefs have the same UUID and
            * are therefore equal.
            */

          e_forceinline bool operator==( const AutoRef& aref )const{
            return( UUID == aref.UUID );
          }

          /** \brief Conversion operator to pointer.
            *
            * This operator will convert the autoreferencing object to a pointer
            * of the same type <T>.
            *
            * \return A pointer of type T.
            */

          e_forceinline explicit operator const T*()const{
            return pcast();
          }

          /** \brief Conversion operator to pointer.
            *
            * This operator will convert the autoreferencing object to a pointer
            * of the same type <T>.
            *
            * \return A pointer of type T.
            */

          e_forceinline explicit operator T*(){
            return pcast();
          }

          /** \brief Dereference operator.
            *
            * This operator is the same as calling cast() on the autoreferencing
            * object. It's handy shorthand for the following.
            \code
              #include<eon/eon.h>

              using namespace EON;
              using namespace sg;

              static const Entity& foo( const Entity::handle hEntity ){
                return *hEntity;
              }
            \endcode
            *
            * \return Returns a dereferenced value of type T.
            */

          e_forceinline const T& operator*()const{
            return cast();
          }

          /** \brief Dereference operator.
            *
            * This operator is the same as calling cast() on the autoreferencing
            * object. It's handy shorthand for the following.
            \code
              #include<eon/eon.h>

              using namespace EON;
              using namespace sg;

              Entity& foo( Entity::handle hEntity ){
                return *hEntity;
              }
            \endcode
            *
            * \return Returns a dereferenced value of type T.
            */

          e_forceinline T& operator*(){
            return cast();
          }

          /** \brief Equality operator.
            *
            * This routine will compare this handle with the UUID of another
            * AutoRef.
            *
            * \param uuid Identifier.
            * \return Returns true if the two handles are the same and false
            * otherwise.
            */

          e_forceinline bool operator==( const s64 uuid )const{
            return( UUID == uuid );
          }

          /** \brief Equality operator.
            *
            * This operator will check to see if this AutoRef contains a
            * reference to the Object class provided.
            */

          e_forceinline bool operator==( const Object* self ){
            return( UUID == self->UUID );
          }

          /** \brief Inequality operator.
            *
            * This routine will compare this handle with the handle of another
            * AutoRef object.
            *
            * \param uuid Comparator.
            *
            * \return Returns true if the two handles are not the same and
            * false otherwise.
            */

          e_forceinline bool operator!=( const s64 uuid )const{
            return( UUID != uuid );
          }

          /** \brief Not operator.
            *
            * \return Returns true if the handle is zero and false otherwise.
            */

          e_forceinline bool operator!()const{
            return( !UUID || !Factory::valid( UUID ));
          }

          /** \brief UUID operator.
            *
            * \return This routine will return true if the handle is non-zero
            * and false otherwise.
            */

          e_forceinline operator s64()const{
            if( !Factory::valid( UUID )||!UUID ){
              return 0;
            }
            return UUID;
          }

          /** \brief Pointer operator.
            *
            * This routine calls cast() internally and returns a pointer to it.
            *
            * \return Returns a constant pointer to the cast handle.
            */

          e_forceinline_always const T* operator->()const{
            const T* pT = Class::pcast<T>( UUID );
            e_assert( pT );
            e_assert( pT->UUID == UUID );
            return pT;
          }

          /** \brief Pointer operator.
            *
            * This routine calls cast() internally and returns a pointer to it.
            *
            * \return Returns a pointer to the cast handle.
            */

          e_forceinline_always T* operator->(){
            T* pT = Class::pcast<T>( UUID );
            e_assert( pT );
            e_assert( pT->UUID == UUID );
            return pT;
          }

          /** \brief Assignment operator.
            *
            * This routine will assign an AutoRef of type T to this one. Before
            * the assignment the reference count will be decremented then
            * incremented again after the assignment.
            *
            * \param a The AutoRef reference to assign.
            *
            * \return Returns *this.
            */

          e_forceinline AutoRef& operator=( const AutoRef& a ){
            if( a.UUID ){//gain ownership and keep alive if same.
              Class::addref( a.UUID );
            }
            if( UUID ){//lose ownership and die if same.
              Class::subref( UUID );
            }
            UUID = a.UUID;
            #if e_compiling( debug )
              __me = UUID ? Class::pcast<T>( UUID ) : nullptr;
            #endif
            return *this;
          }

          /** \brief Assignment operator.
            *
            * This routine will assign an integer to this AutoRef. Before the
            * assignment the reference count will be decremented then
            * incremented again after the assignment.
            \code
              Texture::handle hTexture = e_stream<Texture>( "myCompiledTexture", nullptr );
              Texture::handle hTextur2 = hTexture->UUID;//This is the same as...
              Texture::handle hTextur3 = hTexture;      //...this line.
            \endcode
            * \param uuid The integer handle to assign.
            * \return Returns *this.
            */

          e_forceinline AutoRef& operator=( const s64 uuid ){
            if( uuid ){//gain ownership and keep alive if same.
              Class::addref( uuid );
            }
            if( UUID ){//lose ownership and die if same.
              Class::subref( UUID );
            }
            UUID = uuid;
            #if e_compiling( debug )
              __me = UUID ? Class::pcast<T>( UUID ) : nullptr;
            #endif
            return *this;
          }

        //}:                                      |
        //Methods:{                               |

          /** \name Validation.
            *
            * These methods validate the auto ref handle.
            *
            * @{
            */

          /** \brief Test whether handle is valid.
            *
            * This routine is used in a multi-threaded application to test
            * whether the AutoRef handle is valid or not. If it is true if
            * returned otherwise false.
            */

          e_forceinline bool valid()const{
            return Factory::valid( UUID );
          }

          /** \brief Auto ref is empty test.
            *
            * This routine will test if the handle is zero and return true if
            * it is and false otherwise.
            */

          e_forceinline bool empty()const{
            if( !UUID ){
              return true;
            }
            return false;
          }

          /** @}
            *
            * \name Reflection system support.
            *
            * These methods support the reflection system and allow the
            * querying of type data such as the class id.
            *
            * @{
            */

          /** \brief Get classid of the type.
            *
            * This routine will return the class id of the type.
            *
            * \return A 32-bit unsigned integer containing the classid of the
            * type.
            */

          e_forceinline static u64 classid(){
            return e_classid<AutoRef>();
          }

          /** \brief Probe the classid of the type referenced by UUID.
            *
            * This routine will return the class id of the type.
            *
            * \return A 64-bit unsigned integer containing the classid of the
            * type.
            */

          e_forceinline u64 probeid()const{
            if( UUID ){
              return Class::cast<Object>( UUID ).probeid();
            }
            return 0;
          }

          /** \brief Auto ref converter function.
            *
            * This routine will convert one type of AutoRef to another. This is
            * good for casting down to a ancestor class but only if you're
            * really sure of the type ahead of time because no compile-time or
            * runtime checks are performed. It's quick and dirty, with emphasis
            * on the dirty.
            */

          template<typename A> e_forceinline typename A::handle as()const{
            if( Factory::valid( UUID )){
              if( isa<A>() ){
                return static_cast<typename A::handle>( UUID );
              }
              e_unreachable( "No conversion possible!" );
            }
            return 0;
          }

          /** \brief Auto ref type checker.
            *
            * This routine will check the type of the AutoRef against the
            * template arguments.
            *
            * \return Returns true if the auto ref is actually of type "T" and
            * false otherwise. It queries the object itself to figure out using
            * the virtual isa method defined in e_reflect for all objects.
            */

          template<typename A> e_forceinline bool isa()const{
            if( Factory::valid( UUID )){
              return e_isa<A>( Class::pcast<T>( UUID ));
            }
            return false;
          }

          /** \brief Strip const from auto ref.
            *
            * This routine will strip the const from a handle. It is the same
            * as saying:
            \code
              Entity::handle const_cast( const Entity::handle& hEntity ){
                return hEntity.as<Entity>();
              }
            \endcode
            */

          e_forceinline AutoRef<T> noconst()const{
            return as<T>();
          }

          /** \brief Type caster.
            *
            * This routine will cast the AutoRef handle to it's type.
            *
            * \return Returns the AutoRef as a T reference.
            */

          e_forceinline const T& cast()const{
            const T& t = Class::cast<T>( UUID );
            #if e_compiling( debug )
              __me = &t;
            #endif
            return t;
          }

          /** \brief Type caster.
            *
            * This routine will cast the AutoRef handle to it's type.
            *
            * \return Returns the AutoRef as a T reference.
            */

          e_forceinline T& cast(){
            return Class::cast<T>( UUID );
          }

          /** \brief Type caster.
            *
            * This routine will cast the AutoRef handle to it's pointer.
            *
            * \return Returns the AutoRef as a T pointer.
            */

          e_forceinline const T* pcast()const{
            if( UUID ){
              return Class::pcast<T>( UUID );
            }
            e_unreachable( "Empty UUID!" );
          }

          /** \brief Type caster.
            *
            * This routine will cast the AutoRef handle to it's pointer.
            *
            * \return Returns the AutoRef as a T pointer.
            */

          e_forceinline T* pcast(){
            if( UUID ){
              T* pT = Class::pcast<T>( UUID );
              #if e_compiling( debug )
                __me = pT;
              #endif
              return pT;
            }
            return nullptr;
          }

          /** @} */

        //}:                                      |
        //----------------------------------------+-----------------------------

        e_forceinline AutoRef( const AutoRef& a )
            : UUID( a.UUID ){
          if( UUID ){
            #if e_compiling( debug )
              __me = pcast();
            #endif
            Class::addref( UUID );
          }
        }

        e_forceinline AutoRef( AutoRef&& r )
            : UUID( r.UUID ){
          #if e_compiling( debug )
              __me = r.__me;
            r.__me = nullptr;
          #endif
          r.UUID = 0;
        }

        e_forceinline AutoRef( const s64 uuid )
            : UUID( uuid ){
          if( UUID ){
            #if e_compiling( debug )
              __me = pcast();
            #endif
            Class::addref( UUID );
          }
        }

        AutoRef() = default;

        e_forceinline~AutoRef(){//relinquish ownership.
          if( UUID ){
            Class::subref( UUID );
          }
        }

      private:

        #if e_compiling( debug )
          mutable const T* __me = nullptr;
        #endif

        mutable s64 UUID = 0;
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//WeakRef:{                                       |

  namespace EON{

    namespace gfc{

      template<typename T> struct E_PUBLISH WeakRef final{

        //----------------------------------------+-----------------------------
        //Aliases:{                               |

          using Factory = EON::gfc::Class::Factory;
          using Class   = EON::gfc::Class;
          using self    = T;

        //}:                                      |
        //Operate:{                               |

          const T* operator->()const{
            return( UUID ? Class::pcast<T>( UUID ) : nullptr );
          }

          T* operator->(){
            return( UUID ? Class::pcast<T>( UUID ) : nullptr );
          }

          const T& operator*()const{
            if( UUID ){
              return Class::cast<T>( UUID );
            }
            e_unreachable( "Null weak pointer!" );
          }

          T& operator*(){
            return Class::cast<T>( UUID );
          }

          bool operator==( const WeakRef& weakRef )const{
            return( UUID == weakRef.UUID );
          }

          bool operator!=( const WeakRef& weakRef )const{
            return( UUID != weakRef.UUID );
          }

          operator bool()const{
            return valid();
          }

        //}:                                      |
        //Methods:{                               |

          /** \brief Check validity of weak handle.
            *
            * This routine checks if the weak reference is valid.
            *
            * \return Returns true if ref is valid and false otherwise.
            */

          e_forceinline bool valid()const{
            return Factory::valid( UUID );
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        WeakRef( const AutoRef<T>& autoRef )
            : UUID( autoRef ? autoRef->UUID : 0 ){
          Class::addweak( UUID );
        }

        WeakRef( const T* pT )
            : UUID( pT ? pT->UUID : 0 ){
          Class::addweak( UUID );
        }

        WeakRef() = default;

        ~WeakRef(){
          Class::subweak( UUID );
        }

      private:

        s64 UUID = 0;
      };
    }
  }

//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
