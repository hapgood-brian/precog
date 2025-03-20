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
  *     \addtogroup reflection
  *     @{
  */

//================================================+=============================
//Property:{                                      |

  namespace EON{

    namespace gfc{

      /** \brief Property object.
        *
        * This templatized class is deesigned for one purpose: to describe a
        * member variable declared with one of the e_property macros. It holds
        * a pointer to the outer object (the class) and to the data itself (the
        * member var).  It also defines two function pointers: one to a static
        * enum defined by the e_property_enum macro or zero if another property
        * macro is used, and one to a member function pointer that's called
        * whenever setValue() is called on the property or whenever the value
        * is changed in the property window of the Swordlight editor. For
        * example,
        \code
          e_property( u32, u, Value, &myClass::onValueChanged );
        \endcode
        * Would call onValueChanged whenever you edited Value in the property
        * window and hit return (not escape) or if you called the generated
        * setter method setValue(). Value, incidentally can be the moniker of
        * any variable you want to name. Do a global search for e_property to
        * see examples of how the engine uses properties.
        *
        * Properties are also an important part of the scripting engine.
        * Whenever you change a member variable via script the same onChanged
        * callback will be fired.
        *
        * I initially wrote Property to take lambdas but due to problems with
        * the std::function construct there were crashes when passing Property
        * by value or adding them to vectors or maps. The std::function
        * destructor was called on the temporary Property we create when
        * passing back from toPropertyValue (where value in your member
        * variable name as passed into e_property*) and the std::function in
        * the copy-constructed Property object couldn't survive a second call
        * to the std::function destructor.  This bug was found using Clang 6.0
        * in Xcode 6.x.
        */

      struct E_PUBLISH Property final{

        #ifdef __APPLE__
          #pragma mark - Property types -
        #endif

        //----------------------------------------+-----------------------------
        //Aliases:{                               |

          using OnChanged = void( Object::* )();
          using OnDefault = cvp(          * )();
          using OnEnums   = string(       * )( const s64 );
          using OnTick    = void( Object::* )( Property* );

        //}:                                      |
        //Operate:{                               |

          #ifdef __APPLE__
            #pragma mark - Operators -
          #endif

          /** \brief Assignment operator.
            *
            * This operator does a dangerous memcpy of the property so be careful
            * how you use it. Do not add virtual functions and do not inherit from
            * it. In every way possible keep it simple.
            *
            * \param property The property to assign.
            *
            * \return Returns *this.
            */

          e_noinline Property& operator=( const Property& property ){
            if( this != &property ){
              e_guardw( m_tLock );
              m_onDefault  = property.m_onDefault;
              m_onChanged  = property.m_onChanged;
              m_onEnums    = property.m_onEnums;
              m_onTick     = property.m_onTick;
              m_mChildren  = property.m_mChildren;
              m_pParent    = property.m_pParent;
              m_pOuter     = property.m_pOuter;
              m_sName      = property.m_sName;
              m_pData      = property.m_pData;
              m_uType      = property.m_uType;
              m_tFlags.all = property.m_tFlags.all;
            }
            return *this;
          }

          /** \brief Move operator.
            *
            * This operator moves an rvalue property into this one.
            *
            * \param property The property to move.
            *
            * \return Returns *this.
            */

          e_noinline Property& operator=( Property&& property ){
            if( this != &property ){
              e_guardw( m_tLock );
              m_onDefault           = property.m_onDefault;
              m_onChanged           = property.m_onChanged;
              m_onEnums             = property.m_onEnums;
              m_onTick              = property.m_onTick;
              m_mChildren           = std::move( property.m_mChildren );
              m_pParent             = std::move( property.m_pParent );
              m_pOuter              = property.m_pOuter;
              m_sName               = property.m_sName;
              m_pData               = property.m_pData;
              m_uType               = property.m_uType;
              m_tFlags.all          = property.m_tFlags.all;
              property.m_onDefault  = nullptr;
              property.m_onChanged  = nullptr;
              property.m_onEnums    = nullptr;
              property.m_onTick     = nullptr;
              property.m_pOuter     = nullptr;
              property.m_sName      = nullptr;
              property.m_pData      = nullptr;
              property.m_uType      = 0;
              property.m_tFlags.all = 0;
            }
            return *this;
          }

        //}:                                      |
        //Methods:{                               |

          #ifdef __APPLE__
            #pragma mark - Methods -
          #endif

          /** \name Property accessors.
            *
            * These methods allow you to typecast the property to a type or set
            * the value that the property references.
            *
            * @{
            */

          /** \brief Is data empty?
            *
            * This routine returns true if the data member is null and false
            * otherwise.
            */

          e_forceinline_always bool empty()const{
            return !m_pData;
          }

          /** \brief Get value.
            *
            * This routine will return the current value of the property.
            */

          template<typename T> e_forceinline_always void queryAs( const std::function<void( const T& )>& lambda )const{
            e_guardr( m_tLock );
            if( m_pOuter && Class::Factory::valid( m_pOuter->UUID )){
              if( m_pData ){
                lambda( *reinterpret_cast<const T*>( m_pData ));
              }
            }
          }

          /** \brief Get value.
            *
            * This routine will return the current value of the property.
            */

          template<typename T> e_forceinline_always void alterAs( const std::function<void( T& )>& lambda ){
            e_guardw( m_tLock );
            if( m_pOuter && Class::Factory::valid( m_pOuter->UUID )){
              if( m_pData ){
                lambda( *(T*)m_pData );
              }
            }
          }

          /** \brief Set value.
            *
            * This routine will set the value referenced by this property. If the
            * callback onChanged has been set then it will execute it too.
            */

          template<typename T> e_noinline void setValue( const T& value ){
            e_guardw( m_tLock );
            if( !m_pOuter || !Class::Factory::valid( m_pOuter->UUID )){
              return;
            }
            const T* defaultValue = toDefault<T>();
            alterAs<T>(
              [&]( T& comparison ){
                bool bChanged=( value != comparison );
                bool bErased = false;
                if(  bChanged ){
                  if( defaultValue && ( value == *defaultValue )){
                    m_tFlags->bKeyframe = 0;
                    m_tFlags->bModified = 0;
                    bChanged = false;
                    bErased  = true;
                  }else if( !m_tFlags->bKeyframe ){
                    m_tFlags->bKeyframe = 1;
                    m_tFlags->bModified = 1;
                  }
                }
                if( bChanged || bErased ){
                  const u64 playHead = Object::gameTurn;
                  if( bErased ){
                    m_vPlayHeads.erase( playHead );
                  }else if( !m_vPlayHeads.find( playHead )){
                    m_vPlayHeads.push( playHead );
                  }
                  comparison = value;
                  sendChange();
                }
              }
            );
          }

          /** \brief Set value.
            *
            * This routine will set the value referenced by this property. If the
            * callback onChanged has been set then it will execute it too.
            */

          template<typename T,u32 N> e_noinline void setValue( const array<T,N>& value ){
            e_guardw( m_tLock );
            value.foreach(
              [&]( const T& t ){
                setValue( t );
              }
            );
          }

          /** \brief Set value.
            *
            * This routine will set the value referenced by this property. If the
            * callback onChanged has been set then it will execute it too.
            */

          template<typename T> e_noinline void setValue( const vector<T>& value ){
            e_guardw( m_tLock );
            value.foreach(
              [&]( const T& t ){
                setValue( t );
              }
            );
          }

          /** \brief Is ignored test.
            *
            * This routine will test whether this property should be ignored while
            * recording not scanning state (not undo/redo buffer).
            */

          e_forceinline bool isIgnored()const{
            return( 1 == m_tFlags->bIgnore );
          }

          /** \brief Is caption text.
            *
            * This routine will test whether this property is a caption or not. It
            * does this by just checking the m_pData member variable. If it's zero
            * then the property is a caption.
            *
            * \return Returns true if this is a caption property or false.
            */

          e_forceinline bool isGroupCaption()const{
            return m_tFlags->bGroup;
          }

          /** \brief Is caption text.
            *
            * This routine will test whether this property is a caption or not. It
            * does this by just checking the m_pData member variable. If it's zero
            * then the property is a caption.
            *
            * \return Returns true if this is a caption property or false.
            */

          e_forceinline bool isCaption()const{
            return m_tFlags->bCaption;
          }

          /** \brief Test if property is read only.
            *
            * This routine returns true if the property was marked read only and
            * false otherwise.
            *
            * \return Returns a boolean.
            */

          e_forceinline bool isReadOnly()const{
            return( 1 == m_tFlags->bReadOnly );
          }

          /** \brief Test if property is an enum.
            *
            * This routine will return true if the property wraps an enum type or
            * false otherwise.
            *
            * \return Returns a boolean describing this state.
            */

          e_forceinline bool isEnum()const{
            return( 1 == m_tFlags->bIsEnum );
          }

          /** \brief Check if property is a container.
            *
            * This routine will return true if the property is a container of some
            * kind including vectors, deques and arrays.
            */

          e_forceinline bool isContainer()const{
            return( isVector() || isArray() );
          }

          /** \brief Set if property is a bit bucket.
            *
            * This routine will return true if the property wraps a bit bucket type
            * or false otherwise.
            *
            * \return Returns a boolean describing this state.
            */

          e_forceinline bool isBitBucket()const{
            return( 1 == m_tFlags->bIsBucket );
          }

          /** \brief Set if property is a vector.
            *
            * This routine will return true if the property wraps a vector type or
            * false otherwise.
            *
            * \return Returns a boolean describing this state.
            */

          e_forceinline bool isVector()const{
            return( 1 == m_tFlags->bIsVector );
          }

          /** \brief Set if property is an array.
            *
            * This routine will return true if the property wraps an array type or
            * false otherwise.
            *
            * \return Returns a boolean describing this state.
            */

          e_forceinline bool isArray()const{
            return( 1 == m_tFlags->bIsArray );
          }

          /** \brief Set if property is a handle.
            *
            * This routine will return true if the property wraps a handle type or
            * false otherwise.
            *
            * \return Returns a boolean describing this state.
            */

          e_forceinline bool isHandle()const{
            return( 1 == m_tFlags->bIsHandle );
          }

          /** @}
            *
            * \name Callback methods.
            *
            * These methods tell the outer object that something has changed,
            * either the entire object or an element of that object. In the case
            * of vectors you can also get callbacks for elements being added or
            * removed (erased).
            *
            * @{
            */

          /** \brief New value applied.
            *
            * This routine will call the onChanged callback.
            */

          void sendChange()const;

          /** \brief Grab default value.
            *
            * This routine will return if the property is in it's default state.
            *
            * \return Returns true if the data this property has its default value
            * and false if it doesn't.
            */

          template<typename T> e_forceinline const T* toDefault()const{
            e_guardr( m_tLock );
            if( m_onDefault && m_pOuter && Class::Factory::valid( m_pOuter->UUID )){
              return reinterpret_cast<const T*>( m_onDefault() );
            }
            return nullptr;
          }

          /** \brief Tick the propery if possible.
            *
            * This routine will tick the property via it's onTick callback. This
            * can be implemented on any property object.
            */

          void tick();

          /** \brief Hide the property.
            *
            * This routine will hide the property and all it's descendants.
            */

          e_forceinline void hide(){
            e_guardw( m_tLock );
            m_tFlags->bHidden = 1;
          }

          /** \brief Show the property.
            *
            * This routine will show the property.
            */

          e_forceinline void show(){
            e_guardw( m_tLock );
            m_tFlags->bHidden = 0;
          }

          /** \brief Test if visible.
            *
            * This routine will return true if the component is visible or false
            * otherwise.
            */

          e_forceinline bool isVisible()const{
            e_guardr( m_tLock );
            return( 0 == m_tFlags->bHidden );
          }

          /** @}
            *
            * Thread synchronized data access.
            *
            * These routines let you access and clone the data pointed to by this
            * property.
            *
            * @{
            */

          e_noinline void query( const std::function<void( cvp, const u32 )>& lambda )const{
            e_guardr( m_tLock );
            lambda( m_pData, toSize() );
          }

          e_noinline void alter( const std::function<void( vp, const u32 )>& lambda ){
            e_guardw( m_tLock );
            lambda( vp( m_pData ), toSize() );
          }

          e_noinline void clear(){
            e_guardw( m_tLock );
            m_onDefault  = nullptr;
            m_onChanged  = nullptr;
            m_onEnums    = nullptr;
            m_onTick     = nullptr;
            m_pOuter     = nullptr;
            m_pData      = nullptr;
            m_pParent    = nullptr;
            m_uType      = 0;
            m_tFlags.all = 0;
            m_mChildren.clear();
            m_sName.clear();
          }

          bool clone();

          /** @}
            *
            * \name Size and element accessors.
            *
            * These routines let you set and get the size and element index.
            *
            * @{
            */

          e_forceinline void setSize( const u32 size ){
            m_tFlags->uSize = size;
          }

          e_forceinline u32 toSize()const{
            return u32( m_tFlags->uSize & 0xFFFFFFFF );
          }

          e_forceinline void setElement( const u32 element ){
            m_tFlags->uElem = element;
          }

          e_forceinline u32 toElement()const{
            return u32( m_tFlags->uElem );
          }

          /** @} */

        //}:                                      |
        //----------------------------------------+-----------------------------

        /** \brief Property constructor.
          *
          * Properties are constructed with a name, data+size and type. This can be
          * used with the e_classid<T>() or Class<T>::isa( uType ) methods to
          * determine equality. Properties are considered to be bucket types.
          *
          \code
            struct A{
              e_property_bool( B );
              A(){
                if( Class<int>::classid()==toPropertyB().toType() ){
                }else{
                }
                if( e_classid<int>()==toPropertyB().toType() ){
                }else{
                }
                if( Class<bool>::is( toPropertyB().toType() )){
                }else{
                }
              }
            };
          \endcode
          *
          * \param pOuter The outer object pointer.
          * \param sName The name of the property. This should be the same as the
          * name provided for the variable without Hungarian decoration.
          * \param pData The data.
          * \param uSize The size of the data.
          * \param uType The type of the data.
          */

        e_noinline Property( const Object* pOuter, const string& sName, cvp pData, const u32 uSize, const u64 uType )
            : m_sName(  sName  )
            , m_pOuter( pOuter )
            , m_uType(  uType  )
            , m_pData(  pData  ){
          setSize( uSize );
        }

        /** \brief Copy constructor.
          *
          * This constructor will duplicate everything in the input property.
          *
          * \param lvalue The property to duplicate (copy).
          */

        e_noinline Property( const Property& lvalue )
            : m_onDefault(                   lvalue.m_onDefault )
            , m_onChanged(                   lvalue.m_onChanged )
            , m_onEnums(                     lvalue.m_onEnums   )
            , m_onTick(                      lvalue.m_onTick    )
            , m_sName(                       lvalue.m_sName     )
            , m_mChildren(                   lvalue.m_mChildren )
            , m_pParent(                     lvalue.m_pParent   )
            , m_pOuter(                      lvalue.m_pOuter    )
            , m_uType(                       lvalue.m_uType     )
            , m_pData(                       lvalue.m_pData     ){
          m_tFlags.all =                     lvalue.m_tFlags.all;
        }

        /** \brief Move constructor.
          *
          * This constructor will move all the members of the rvalue into this
          * object, especially the child vector which may have dozens if not
          * hundreds of sub-properties each with children of their own. So this
          * path is crucially important.
          *
          * \param rvalue The rvalue reference we'll move from.
          */

        e_noinline Property( Property&& rvalue )
            : m_onDefault(              rvalue.m_onDefault )
            , m_onChanged(              rvalue.m_onChanged )
            , m_onEnums(                rvalue.m_onEnums   )
            , m_onTick(                 rvalue.m_onTick    )
            , m_sName(                  rvalue.m_sName     )
            , m_mChildren( std::move(   rvalue.m_mChildren ))
            , m_pParent(   std::move(   rvalue.m_pParent   ))
            , m_pOuter(                 rvalue.m_pOuter    )
            , m_uType(                  rvalue.m_uType     )
            , m_pData(                  rvalue.m_pData     ){
          m_tFlags.all        =         rvalue.m_tFlags.all;
          rvalue.m_onDefault  = nullptr;
          rvalue.m_onChanged  = nullptr;
          rvalue.m_onEnums    = nullptr;
          rvalue.m_onTick     = nullptr;
          rvalue.m_pOuter     = nullptr;
          rvalue.m_sName      = nullptr;
          rvalue.m_pData      = nullptr;
          rvalue.m_uType      = 0;
          rvalue.m_tFlags.all = 0;
        }

        /** \brief Heading property.
          *
          * This property constructor will build a heading in the property window
          * of your editor or app. It should be treated as such. Check for just a
          * name without a type.
          *
          * \param sName The name of the property. This should be the same as the
          * name provided for the variable without Hungarian decoration.
          */

        explicit Property( const string& sName )
          : m_sName( sName )
        {}

        /** \brief Default constructor.
          *
          * Initialize the property to zero values; should be called rarely. Best
          * used if you want to express a line feed in the editor.
          */

        Property() = delete;

        /** \brief Destructor.
          *
          * This will make sure cloned properties are nuked appropriately.
          */

        ~Property(){
          if( m_tFlags->bCloned ){
            e_free( m_pData );
          }
        }

      private:

        #ifdef __APPLE__
          #pragma mark - Proprty data  -
        #endif

        /* Callback vars */

        e_var( OnDefault, on, Default ) = nullptr;
        e_var( OnChanged, on, Changed ) = nullptr;
        e_var( OnEnums,   on, Enums   ) = nullptr;
        e_var( OnTick,    on, Tick    ) = nullptr;

        /* Engine vars */

        e_var( u64, u, PropertyKey ) = 0;
        e_var_string(                       Description );           //!< Descriptive name of this property.
        e_var_string(                       Tooltip     );           //!< Descriptive tooltip text.
        e_var_string(                       Name        );           //!< Name of the property.
        e_var_string_map( Object::prop_ptr, Children    );           //!< Vector of dependents (causes indent and folds in property editor).
        e_var_const_ptr( Property,          Parent      ) = nullptr; //!< Weak pointer back to the outer property (the parent).
        e_var_const_ptr( Object,            Outer       ) = nullptr; //!< Outer class; usually the this pointer.
        e_var_vector(    u64,               PlayHeads   );           //!< Time of recording.
        e_var(           u64,    u,         Type        ) = 0;       //!< Member classid.
        e_var_bits(                         Flags
          , bCollapsed:1 //!< Display in + collapsed state in the property window.
          , bKeyframe:1  //!< The property has been changed on current keyframe.
          , bModified:1  //!< The property has been changed someplace in time.
          , bReadOnly:1  //!< The property is read-only and cannot be changed.
          , bIsHandle:1  //!< The property is an autoref handle.
          , bIsBucket:1  //!< The property is a bit bucket.
          , bIsVector:1  //!< The property is a vector of objects.
          , bIsArray:1   //!< The property is an array of objects.
          , bIsEnum:1    //!< The property is an enum.
          , bHidden:1    //!< The property is hidden (runtime only).
          , bTicked:1    //!< The property has been ticked.
          , bIgnore:1    //!< Ignore property when recording but not scanning.
          , bCloned:1    //!< Data no longer references an object must delete.
          , bDirty:1     //!< The property has changed.
          , uElem:16     //!< The current element size.
          , uSize:24     //!< The data size in bytes.
          , bParametric:1//!< The property value must be parametric.
          , bPositive:1  //!< The property value cannot go negative.
          , bNoScroll:1  //!< The property has no scroll widget.
          , bUnit:1      //!< The property value must be unit (-1 to 1).
          , bLerp:1      //!< The property will be interpolated.
          , bCaption:1
          , bGroup:1
          , bPad1:1
          , bPad2:1
        );
        atomic::ShareLockRecursive m_tLock;
        cvp m_pData = nullptr;
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Macros:{                                        |
  //e_property_*:{                                |
    //e_property_block:{                          |

#ifdef __APPLE__
  #pragma mark - Property macros -
#endif

      /** \brief Property heading.
        *
        * This macro will define a heading in the property window under which
        * an unsorted map in unoriginal order will be added.
        *
        * \param X The alpha numeric name of the heading. Forms a C++ label.
        */

#define e_property_block( ID,... )                                              \
  private:                                                                      \
    e_noinline_or_debug::EON::gfc::Object::prop_ptr ___guardian##ID(){          \
      const ccp pName = #ID;                                                    \
      const u64 uName = e_hashstr64_const( pName );                             \
      auto prop = std::make_shared<Property>( pName );                          \
      prop->toFlags()->bCaption = 1;                                            \
      if(___group_property ){                                                   \
         ___group_property->toChildren().set( uName, prop );                    \
        prop->setParent(___group_property.get() );                              \
      }else{                                                                    \
        ___root_property->toChildren().set( uName, prop );                      \
        prop->setParent(___root_property.get() );                               \
      }                                                                         \
      return(___head_property = prop );                                         \
    }                                                                           \
    ::EON::gfc::Object::prop_ptr ___##ID=___guardian##ID();                     \
    __VA_ARGS__                                                                 \

#define e_property_group( ID,... )                                              \
  private:                                                                      \
    e_noinline_or_debug::EON::gfc::Object::prop_ptr ___guardian##ID(){          \
      const ccp pName = #ID;                                                    \
      const u64 uName = e_hashstr64_const( pName );                             \
      auto prop = std::make_shared<Property>( pName );                          \
      prop->toFlags()->bGroup = 1;                                              \
      ___group_property = prop;                                                 \
      ___root_property->toChildren().set( uName                                 \
    , ___group_property );                                                      \
      ___group_property->setParent(___root_property.get() );return              \
      ___group_property;                                                        \
    }                                                                           \
    ::EON::gfc::Object::prop_ptr ___##ID=___guardian##ID();                     \
    __VA_ARGS__                                                                 \

    //}:                                          |
    //e_property_handle_vector:{                  |

      /** \brief Declare vector of handles property.
        *
        * This macro will declare a vector filled with handles. See e_property
        * for more information on property macros in general. Note that the
        * type generated by this macro is thread safe.
        *
        * \param T The type of object to be put in the vector.
        * \param X The name of the vector as a member variable.
        * \param READONLY If true then the property will not be editable in Lua
        * or Swordlight's property window.
        * \param F A member function pointer to a callback method inside this
        * class. It must take an object of type T.
        * \param TIP The tooltip to display when the user hovers over this
        * property in the Swordlight3D editor.
        */

#define e_property_handle_vector( T, X, READONLY, F, TIP )                      \
  public:                                                                       \
    using X=::EON::gfc::vector<::EON::gfc::AutoRef<T>>;                         \
  private:                                                                      \
    e_noinline_or_debug string ___getHandleVectorElementString##X(              \
                      const::EON::gfc::Property* pProperty )const{              \
      if( m_v##X.empty() ){                                                     \
        return"...";                                                            \
      }                                                                         \
      const auto& hHandle = m_v##X[ pProperty->toElement() ];                   \
      if( hHandle ){                                                            \
        return hHandle.as<Object>()->classof();                                 \
      }                                                                         \
      return"nil";                                                              \
    }                                                                           \
    e_noinline_or_debug string ___getHandleVectorString##X(                     \
        const::EON::gfc::Property* )const{                                      \
      return e_xfs( "(%u)", m_v##X.size() );                                    \
    }                                                                           \
    e_noinline_or_debug void ___tickHandleVectorProperty##X( Property* P ){     \
      if( P->toChildren().size() != m_v##X.size() ){                            \
        P->setDescription(___getHandleVectorString##X( P ));                    \
        const auto children = std::move( P->toChildren() );                     \
        for( u32 n=m_v##X.size(), i=0; i<n; ++i ){                              \
          m_v##X.alter( i,                                                      \
            [&](::EON::gfc::AutoRef<T>& hElement ){                             \
              prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this \
                  , e_xfs( "[%03u]", i )                                        \
                  , &hElement                                                   \
                  , u32( sizeof( hElement ))                                    \
                  , e_classid<T>() );                                           \
              pProperty->toFlags()->bReadOnly  = READONLY;                      \
              pProperty->toFlags()->bCollapsed = true;                          \
              pProperty->toFlags()->bIsHandle  = true;                          \
              pProperty->setElement( i );                                       \
              pProperty->setDescription(                                        \
                  ___getHandleVectorElementString##X( pProperty.get() ));       \
              if( !(READONLY) ){                                                \
                pProperty->setChanged(                                          \
                  ::EON::gfc::Property::OnChanged( &self::F )                   \
                );                                                              \
              }                                                                 \
              P->toChildren().set( e_hashstr64( pProperty->toName() )           \
                  , pProperty );                                                \
              pProperty->setParent( P );                                        \
              if( hElement ){                                                   \
                prop_map out_mProperties;                                       \
                hElement.as<Object>()->getProperties( out_mProperties );        \
                out_mProperties.foreach(                                        \
                  [&]( const prop_ptr& pNext ){                                 \
                    pProperty->toChildren().set(                                \
                        pNext->toName().hash()                                  \
                      , pNext                                                   \
                    );                                                          \
                  }                                                             \
                );                                                              \
              }                                                                 \
            }                                                                   \
          );                                                                    \
        }                                                                       \
      }                                                                         \
    }                                                                           \
    e_noinline_or_debug const X ___populate##X(){                               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_v##X                                                             \
          , u32( sizeof( m_v##X ))                                              \
          , e_classid<T>() );                                                   \
      pProperty->toFlags()->bReadOnly  = READONLY;                              \
      pProperty->toFlags()->bCollapsed = true;                                  \
      pProperty->toFlags()->bIsVector  = true;                                  \
      pProperty->toFlags()->bIsHandle  = true;                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickHandleVectorProperty##X ));                         \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return X();                                                               \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always bool sort##X( const std::function<bool(                \
          const::EON::gfc::AutoRef<T>&,                                         \
          const::EON::gfc::AutoRef<T>& )>& lambda ){                            \
      return m_v##X.sort( lambda );                                             \
    }                                                                           \
    e_noinline_or_debug bool set##X##Property( const X& v ){                    \
      if( &m_v##X != &v ){                                                      \
        toProperty( #X )->setValue( v );                                        \
        return true;                                                            \
      }                                                                         \
      return false;                                                             \
    }                                                                           \
    e_forceinline void set##X( const X& v ){                                    \
      m_v##X = v;                                                               \
    }                                                                           \
    template<typename E> e_forceinline_always const::EON::gfc::AutoRef<T> in##X(\
        const E e )const{                                                       \
      return m_v##X[ e ];                                                       \
    }                                                                           \
    template<typename E> e_noinline_or_debug bool setIn##X( const E e,          \
        const::EON::gfc::AutoRef<T>& hObject ){                                 \
      bool bResult = false;                                                     \
      m_v##X.alter( u32( e ),                                                   \
        [&](::EON::gfc::AutoRef<T>& hOutput ){                                  \
          hOutput = hObject;                                                    \
          bResult = true;                                                       \
        }                                                                       \
      );                                                                        \
      return false;                                                             \
    }                                                                           \
    e_forceinline_always const X& to##X()const{                                 \
      return m_v##X;                                                            \
    }                                                                           \
    e_forceinline_always X& to##X(){                                            \
      return m_v##X;                                                            \
    }                                                                           \
    e_forceinline_always void clear##X(){                                       \
      m_v##X.clear();                                                           \
    }                                                                           \
  private:                                                                      \
    X m_v##X = ___populate##X()                                                 \

#define e_property_handle_vector1( T, READONLY, F, TIP )                        \
   e_property_handle_vector( T, T##s, READONLY, F, TIP )                        \

    //}:                                          |
    //e_property_handle_array:{                   |

      /** \brief Declare array of handles.
        *
        * This macro will declare an array filled with handles. See e_property
        * for more information on property macros in general.
        \code
          using namespace sg;

          struct myClass:Stream{

            e_reflect( myClass, Stream );

            void onTextureChanged(){
            }

          private:

            e_property_handle_array( Texture, Textures, 10, false, onTextureChanged );
          };
        \endcode
        *
        * \param T The type of object to be put in the vector.
        * \param X The name of the vector as a member variable.
        * \param N The number of elements in the handle array.
        * \param READONLY If true then the property will not be editable in Lua
        * or Swordlight's property window.
        * \param F A member function name to a callback method inside this
        * class. Can be IGNORE_CHANGES.
        * \param ... The names of all the elements in the array.
        */

#define e_property_handle_array( T, X, N, READONLY, F, L, TIP )                 \
  public:                                                                       \
    using X=::EON::gfc::array<::EON::gfc::AutoRef<T>,u32( N )>;                 \
  private:                                                                      \
    e_noinline_or_debug::EON::gfc::array<string,u32( N )>___strings##X()const{  \
      const std::function<strings()> onPopulator##X = L;                        \
      ::EON::gfc::array<string,u32( N )>result;                                 \
      if( onPopulator##X ){                                                     \
        u32 i = 0;                                                              \
        onPopulator##X().foreachs(                                              \
          [&]( const string& value ){                                           \
            if( i < u32( N )){                                                  \
              result[ i++ ] = value;                                            \
              return true;                                                      \
            }                                                                   \
            return false;                                                       \
          }                                                                     \
        );                                                                      \
      }                                                                         \
      return result;                                                            \
    }                                                                           \
    e_noinline_or_debug string ___getHandleArrayElementString##X(               \
         const::EON::gfc::Property* pProperty )const{                           \
      const auto& hObject = m_a##X[ pProperty->toElement() ];                   \
      if( !hObject ){                                                           \
        return"nil";                                                            \
      }                                                                         \
      return #T;                                                                \
    }                                                                           \
    e_noinline_or_debug string ___getHandleArrayString##X(                      \
        const::EON::gfc::Property* )const{                                      \
      return e_xfs( "[%03u]", u32( N ));                                        \
    }                                                                           \
    e_noinline_or_debug void ___tickHandleArrayProperty##X( Property* P ){      \
      if( P->toChildren().size() != m_a##X.size() ){                            \
        const auto children = std::move( P->toChildren() );                     \
        P->setDescription(___getHandleArrayString##X( P ));                     \
        const auto childLabels = ___strings##X();                               \
        for( u32 n=m_a##X.size(), i=0; i<n; ++i ){                              \
          m_a##X.alter( i, [&](::EON::gfc::AutoRef<T>& hElement ){              \
            prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this   \
                , childLabels[ i ]                                              \
                , &hElement                                                     \
                , u32( sizeof( hElement ))                                      \
                , e_classid<T>() );                                             \
            pProperty->toFlags()->bReadOnly  = READONLY;                        \
            pProperty->toFlags()->bCollapsed = true;                            \
            pProperty->toFlags()->bIsHandle  = true;                            \
            pProperty->setElement( i );                                         \
            pProperty->setDescription(                                          \
                ___getHandleArrayElementString##X( pProperty.get() ));          \
            if( !(READONLY) ){                                                  \
              pProperty->setChanged(                                            \
                  ::EON::gfc::Property::OnChanged( &self::F ));                 \
            }                                                                   \
            P->toChildren().set( pProperty->toName().hash(), pProperty );       \
            pProperty->setParent( P );                                          \
            if( hElement ){                                                     \
              prop_map out_mProperties;                                         \
              hElement.as<Object>()->getProperties( out_mProperties );          \
              out_mProperties.foreach(                                          \
                [&]( const prop_ptr& pNext ){                                   \
                  pProperty->toChildren().set( pNext->toName().hash()           \
                   , pNext                                                      \
                  );                                                            \
                }                                                               \
              );                                                                \
            }                                                                   \
          });                                                                   \
        }                                                                       \
      }                                                                         \
    }                                                                           \
    e_noinline_or_debug const X ___populate##X(){                               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_a##X                                                             \
          , u32( sizeof( m_a##X ))                                              \
          , e_classid<T>() );                                                   \
      pProperty->toFlags()->bReadOnly  = READONLY;                              \
      pProperty->toFlags()->bCollapsed = true;                                  \
      pProperty->toFlags()->bIsHandle  = true;                                  \
      pProperty->toFlags()->bIsArray   = true;                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(::EON::gfc::Property::OnTick(                          \
        &self::___tickHandleArrayProperty##X ));                                \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return X{};                                                               \
    }                                                                           \
  public:                                                                       \
    template<typename E> e_forceinline_always void set##X##Property(            \
          const X& a##X ){                                                      \
      toProperty( #X )->setValue( a##X );                                       \
    }                                                                           \
    template<typename E> e_forceinline_always void set##X(                      \
          const X& a##X ){                                                      \
      m_a##X = a##X;                                                            \
    }                                                                           \
    template<typename E> e_forceinline_always bool setIn##X( const E e,         \
        const::EON::gfc::AutoRef<T>& h ){                                       \
      auto& value = m_a##X[ e ];                                                \
      if(( &value != &h ) && ( value != h )){                                   \
        value = h;                                                              \
        return true;                                                            \
      }                                                                         \
      return false;                                                             \
    }                                                                           \
    template<typename E> e_forceinline_always const::EON::gfc::AutoRef<T>&      \
        in##X( const E e )const{                                                \
      return m_a##X[ u32( e )];                                                 \
    }                                                                           \
    template<typename E> e_forceinline_always::EON::gfc::AutoRef<T>& in##X(     \
        const E e ){                                                            \
      return m_a##X[ e ];                                                       \
    }                                                                           \
    e_forceinline_always const string& in##X##Strings( const s64 i ){           \
      return ___strings##X()[ i ];                                              \
    }                                                                           \
    e_forceinline_always const X& to##X()const{                                 \
      return m_a##X;                                                            \
    }                                                                           \
    e_forceinline_always void clear##X(){                                       \
      m_a##X = X{};                                                             \
    }                                                                           \
  private:                                                                      \
    X m_a##X = ___populate##X()                                                 \

#define e_property_handle_array1( T, N, READONLY, F, L, TIP )                   \
  e_property_handle_array(  T, T##s, N, READONLY, F, L, TIP )                   \

    //}:                                          |
    //e_property_handle:{                         |

      /** \brief Property handles.
        *
        * The e_property_handle macro is identical to th e_var_handle macros in
        * all cases except one: it adds an extra method called toProperty*. Let
        * it always return a property object that describes the member
        * variable.
        *
        * NB: It is absolutely critical to remember that e_property macros of
        * all varieties cannot be used outside of an Object derived class with
        * a valid e_reflect() statement in it. The property macros all reply
        * heavily on the self type generated by e_reflect().
        *
        * <h4>Usage</h4>
        \code
          using namespace gfc;

          struct TexturePair:Object{

            e_reflect( TexturePair, Object );

          protected:

            virtual void onChangedTexture( cvp pVariable ){
              //
              //  We know that m_hTexture changed and that the new value is
              //  pointed to by pVariable.
              //
              #ifdef e_compiling( debug )
                e_assert( pVariable == &m_hTexture );
                Object* pObject = reinterpret_cast<Object*>( pVariable );
                e_assert( Class::Factory::valid( pObject->UUID ));
                e_assert( pObject->isa( Texture::classid ));
              #endif
            }

          private:

            e_property_handle( Texture, 1st, false, onChangedTexture );
            e_property_handle( Texture, Aux, false, IGNORE_CHANGES );
          };

          int main(){
            TexturePair tex;
            //DANGEROUS: Blind setting of value, assumes that template is right.
            //Property::setValue<> calls onChangedTexture.
            tex.toProperty1st().setValue<Texture::handle>( e_uniqueAsyncStream<Texture>( "name" );
            //Much safer method of calling onChangedTexture.
            tex.set1st( e_uniqueAsyncStream<Texture>( "name" ));
            //This DOESN'T call callback.
            tex.to1st() = 0;
            return 0;
          }
        \endcode
        * The above is an example of how to query one property object. There is
        * another way, an important way of querying a set of properties and
        * that is on any handle (AutoRef<*>). Let's look at the ri::Texture
        * case.
        \code
          using namespace ri;

          int main(){
            Object::prop_map out_mProperties;
            Texture::handle hTexture = e_uniqueAsyncStream<Texture>( "bob" );
            if( hTexture ){
              out_mProperties.push( hTexture.m_tWindows.toActivityWindow() );
              return 0;
            }
            return 1;
          }
        \endcode
        * \param T The handle class; passed directly to AutoRef<T>.
        * \param X The name of the new variable.
        * \param READONLY Set to true to disable editing of the field in
        * the editor. If READONLY is ever true then the F macro parameter is
        * strictly ignored. There will be no callback made when set##X() is
        * called. Lua scripts will be forbidden from changing the value of the
        * property ever. It can only be changed in C++ code.
        * \param F Should be zero, nullptr, or the address of a member function
        * in the class using the &ClassName::MethodName notation or anything
        * that std::bind will work with it will be assigned to a std::function.
        */

#define e_property_handle( T, X, READONLY, F, TIP )                             \
  private:                                                                      \
    /* Get string to describe handle member */                                  \
    e_noinline_or_debug::EON::gfc::string ___getHandleString##X(                \
        const::EON::gfc::Property* )const{                                      \
      if( m_h##X ){                                                             \
        return m_h##X.as<Object>()->classof();                                  \
      }                                                                         \
      return"nil";                                                              \
    }                                                                           \
    /* When called we'll set parents of all children */                         \
    e_noinline_or_debug void ___tickHandleProperty##X( Property* P ){           \
      P->toChildren().clear();                                                  \
      P->setDescription(___getHandleString##X( P ));                            \
      if( m_h##X ){                                                             \
        prop_map out_mProperties;                                               \
        m_h##X.as<Object>()->getProperties( out_mProperties );                  \
        out_mProperties.foreach(                                                \
          [&]( prop_ptr& pNext ){                                               \
            P->toChildren().set( pNext->toName().hash(), pNext );               \
            pNext->setParent( P );                                              \
          }                                                                     \
        );                                                                      \
      }                                                                         \
    }                                                                           \
    /* Hook into adding member variable to property list and creating a new */  \
    /* Property smart pointer to it */                                          \
    e_noinline_or_debug const s64 ___populate##X(){                             \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_h##X                                                             \
          , u32( sizeof( m_h##X ))                                              \
          , e_classid<T>() );                                                   \
      pProperty->toFlags()->bReadOnly  = READONLY;                              \
      pProperty->toFlags()->bCollapsed = true;                                  \
      pProperty->toFlags()->bIsHandle  = true;                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickHandleProperty##X ));                               \
      if( !(READONLY) )                                                         \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return 0;                                                                 \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always void set##X##Property(                                 \
          const::EON::gfc::AutoRef<T>& h##X ){                                  \
      toProperty( #X )->setValue( h##X );                                       \
    }                                                                           \
    e_forceinline_always void set##X(                                           \
          const::EON::gfc::AutoRef<T>& h##X ){                                  \
      m_h##X = h##X;                                                            \
    }                                                                           \
    e_forceinline_always bool is##X( const::EON::gfc::AutoRef<T>& handle )const{\
      return( m_h##X == handle );                                               \
    }                                                                           \
    e_forceinline_always const::EON::gfc::AutoRef<T>& to##X()const{             \
      return m_h##X;                                                            \
    }                                                                           \
    e_forceinline_always::EON::gfc::AutoRef<T>& to##X(){                        \
      return m_h##X;                                                            \
    }                                                                           \
  private:                                                                      \
    ::EON::gfc::AutoRef<T> m_h##X = ___populate##X()                            \

      /** \brief Property handle.
        *
        * The e_property_handle1 macro is identical to th e_property_handle
        * macro except you can omit the name: the name will be the same as the
        * type.
        \code
          struct MyClass:Object{

            e_reflect( MyClass, Object );

          private:

            e_property_handle1( Texture, false, nullptr );
          };
        \endcode
        * \param T The handle class; passed directly to AutoRef<T>.
        * \param READONLY Set to true to disable editing of the field in
        * the editor. If READONLY is ever true then the F macro parameter is
        * strictly ignored. There will be no callback made when set##X() is
        * called. Lua scripts will be forbidden from changing the value of the
        * property ever. It can only be changed in C++ code.
        * \param F Something std::bind can attach to.
        */

#define e_property_handle1( T, READONLY, F, TIP )                               \
  e_property_handle(     T, T, READONLY, F, TIP )                               \

    //}:                                          |
    //e_property_bit_bucket:{                     |

      /** \brief Declare bit bucket property.
        *
        * This macro will declare a bitbucket. See e_property for more
        * information on property macros in general. Note that the type
        * generated by this macro is thread safe.
        *
        * \param X The name of the bitbucket as a member variable.
        * \param N The number of bits in the bucket.
        * \param READONLY If true then the property will not be editable in Lua
        * or Swordlight's property window.
        * \param F A member function pointer to a callback method inside this
        * class. It must take an object of type T.
        * \param TIP The tooltip to display when the user hovers over this
        * property in the Swordlight3D editor.
        */

#define e_property_bit_bucket( X, N, READONLY, F, TIP )                         \
  public:                                                                       \
    using X=::EON::gfc::bitbucket<N>;                                           \
  private:                                                                      \
    /* Populate member variable and create corresponding Property object ptr */ \
    e_noinline_or_debug const X ___populate##X(){                               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_bit##X                                                           \
          , u32( sizeof( m_bit##X ))                                            \
          , e_classid<::EON::gfc::bitbucket<0>>() );                            \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->toFlags()->bIsBucket = true;                                   \
      pProperty->setTooltip( TIP );                                             \
      if( !( READONLY )){                                                       \
        pProperty->setChanged(::EON::gfc::Property::OnChanged( &self::F ));     \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return X();                                                               \
    }                                                                           \
  public:                                                                       \
    e_noinline_or_debug bool set##X##Property( const X& v ){                    \
      if( &m_bit##X != &v ){                                                    \
        toProperty( #X )->setValue( v );                                        \
        return true;                                                            \
      }                                                                         \
      return false;                                                             \
    }                                                                           \
    e_noinline_or_debug void set##X( const X& v ){                              \
      m_bit##X = v;                                                             \
    }                                                                           \
    template<typename E> e_forceinline_always const bool is##X( const E e )     \
      const{ return m_bit##X[ e ];                                              \
    }                                                                           \
    template<typename E> e_forceinline_always void setIs##X( const E e,         \
        const bool value ){                                                     \
      if( value ){                                                              \
        m_bit##X.set( e_underlying( e ));                                       \
      }else{                                                                    \
        m_bit##X.reset( e_underlying( e ));                                     \
      }                                                                         \
    }                                                                           \
    e_forceinline_always const X& to##X()const{                                 \
      return m_bit##X;                                                          \
    }                                                                           \
    e_forceinline_always X& to##X(){                                            \
      return m_bit##X;                                                          \
    }                                                                           \
    e_forceinline_always void clear##X(){                                       \
      m_bit##X.clear();                                                         \
    }                                                                           \
  private:                                                                      \
    X m_bit##X = ___populate##X()                                               \

#define e_property_bit_bucket1(      N, READONLY, F, TIP )                      \
   e_property_bit_bucket( Bucket##N, N, READONLY, F, TIP )                      \

    //}:                                          |
    //e_property_string_vector:{                  |

      /** \brief Declare vector of strings property.
        *
        * This macro will declare a vector filled with strings. See e_property
        * for more information on property macros in general. Note that the
        * type generated by this macro is thread safe.
        *
        * \param T The type of object to be put in the vector.
        * \param X The name of the vector as a member variable.
        * \param READONLY If true then the property will not be editable in Lua
        * or Swordlight's property window.
        * \param F A member function pointer to a callback method inside this
        * class. It must take an object of type T.
        * \param TIP The tooltip to display when the user hovers over this
        * property in the Swordlight3D editor.
        */

#define e_property_string_vector( X, READONLY, F, TIP )                         \
  public:                                                                       \
    using X=::EON::gfc::vector<string>;                                         \
  private:                                                                      \
    /* Get the value of the string vector element */                            \
    e_noinline_or_debug string ___getStringVectorElementString##X(              \
        const::EON::gfc::Property* pProperty )const{                            \
      if( m_v##X.empty() ){                                                     \
        return"...";                                                            \
      }                                                                         \
      const auto& text = m_v##X[ pProperty->toElement() ];                      \
      if( text.empty() ){                                                       \
        return"";                                                               \
      }                                                                         \
      return text;                                                              \
    }                                                                           \
    /* Get string vector element string */                                      \
    e_noinline_or_debug string ___getStringVectorString##X(                     \
        const::EON::gfc::Property* )const{                                      \
      return e_xfs( "(%u)", m_v##X.size() );                                    \
    }                                                                           \
    /* Add sub strings in the vector */                                         \
    e_noinline_or_debug void ___tickStringVectorProperty##X( Property* P ){     \
      if( P->toChildren().size() != m_v##X.size() ){                            \
        const auto children = std::move( P->toChildren() );                     \
        P->setDescription(___getStringVectorString##X( P ));                    \
        for( u32 n=m_v##X.size(), i=0; i<n; ++i ){                              \
          m_v##X.alter( i, [&]( string& text ){                                 \
            prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this   \
                , e_xfs( "[%03u]", i )                                          \
                , &text                                                         \
                , u32( sizeof( text ))                                          \
                , e_classid<::EON::gfc::string>() );                            \
            pProperty->toFlags()->bReadOnly  = READONLY;                        \
            pProperty->toFlags()->bCollapsed = true;                            \
            pProperty->setElement( i );                                         \
            pProperty->setDescription(                                          \
                ___getStringVectorElementString##X( pProperty.get() ));         \
            if( !(READONLY) ){                                                  \
              pProperty->setChanged(                                            \
                  ::EON::gfc::Property::OnChanged( &self::F ));                 \
            }                                                                   \
            P->toChildren().set( pProperty->toName().hash(), pProperty );       \
            pProperty->setParent( P );                                          \
          });                                                                   \
        }                                                                       \
      }                                                                         \
    }                                                                           \
    /* Populate member variable and add property pointer */                     \
    e_noinline_or_debug const X ___populate##X(){                               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_v##X                                                             \
          , u32( sizeof( m_v##X ))                                              \
          , e_classid<::EON::gfc::string>() );                                  \
      pProperty->toFlags()->bReadOnly  = READONLY;                              \
      pProperty->toFlags()->bCollapsed = true;                                  \
      pProperty->toFlags()->bIsVector  = true;                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickStringVectorProperty##X ));                         \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return X();                                                               \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always bool sort##X( const std::function<bool(                \
          const string&, const string& )>& lambda ){                            \
      return m_v##X.sort( lambda );                                             \
    }                                                                           \
    e_noinline_or_debug bool set##X##Property( const X& v ){                    \
      if( &m_v##X != &v ){                                                      \
        toProperty( #X )->setValue( v );                                        \
        return true;                                                            \
      }                                                                         \
      return false;                                                             \
    }                                                                           \
    e_forceinline_always void set##X( const X& v ){                             \
      m_v##X = v;                                                               \
    }                                                                           \
    template<typename E> e_forceinline_always const string in##X(               \
        const E e )const{                                                       \
      return m_v##X[ e ];                                                       \
    }                                                                           \
    template<typename E> e_noinline_or_debug bool setIn##X( const E e,          \
        const string& text ){                                                   \
      bool bResult = false;                                                     \
      m_v##X.alter( u32( e ), [&]( string& output ){                            \
        bResult = true;                                                         \
        output = text;                                                          \
      });                                                                       \
      return bResult;                                                           \
    }                                                                           \
    e_forceinline_always const X& to##X()const{                                 \
      return m_v##X;                                                            \
    }                                                                           \
    e_forceinline_always X& to##X(){                                            \
      return m_v##X;                                                            \
    }                                                                           \
    e_forceinline_always void clear##X(){                                       \
      m_v##X.clear();                                                           \
    }                                                                           \
  private:                                                                      \
    X m_v##X = ___populate##X()                                                 \

#define e_property_handle_vector1( T, READONLY, F, TIP )                        \
   e_property_handle_vector( T, T##s, READONLY, F, TIP )                        \

    //}:                                          |
    //e_property_string:{                         |

      /** \brief Declare string property.
        *
        * Like e_property_handle the macro e_property_string produces a
        * function that returns a Property object. Just call obj.toPropertyX
        * where X is the name of the string. Other than this the macro is
        * identical to e_var_string. This macro will generate a number of
        * methods for accessing the string and a member variable named m_s##X.
        * The defX() method is provided for convenience and is especially
        * useful in XML serializers to detect if a member has changed from it's
        * original value.
        *
        * Please note that there is a deliberate method to bypass the onChanged
        * callback with a property. Just call toX() = value; and you will
        * not be notified. It is always best to use setX( value ); but there
        * are rare occasions where you don't want the callback executed. So for
        * those cases we have the backdoor toX() which returns a reference to
        * the member variable just like the e_var macros do.
        *
        * Please also remember that if READONLY is set to true invoking this
        * macro F might as well be 0 because the callback will not be called.
        \code
          e_property_string( Name, "Unnamed", true, nullptr );
        \endcode
        *
        * \param X The name of the string.
        * \param VALUE The default value of the string as reported by the
        * generated def* (where * is the value of the X parameter or the name
        * of the generated member variable m_s##X.
        * \param READONLY The property will appear as
        * \param F Should be zero, nullptr, or the address of a member function
        * in the class using the &ClassName::MethodName notation. The method
        * will be called whenever set##X() is called or the property is changed
        * in Swordlight's property window or via Lua script.
        */

#define e_property_string( X, VALUE, READONLY, F, TIP )                         \
  private:                                                                      \
    e_noinline_or_debug string ___getStringString##X(                           \
        const::EON::gfc::Property* )const{                                      \
      return m_s##X;                                                            \
    }                                                                           \
    e_noinline_or_debug static cvp ___default##X(){                             \
      static string kDefault##X = VALUE;                                        \
      return      & kDefault##X;                                                \
    }                                                                           \
    e_noinline_or_debug void ___tickStringProperty##X( Property* P ){           \
      P->setDescription( m_s##X );                                              \
    }                                                                           \
    e_noinline_or_debug const::EON::gfc::string ___populate##X(){               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_s##X                                                             \
          , u32( sizeof( m_s##X ))                                              \
          , e_classid( m_s##X ));                                               \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setDefault( &___default##X );                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickStringProperty##X ));                               \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return VALUE;                                                             \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always static ccp default##X(){                               \
      return VALUE;                                                             \
    }                                                                           \
    e_forceinline_always char in##X( const u32 i )const{                        \
      return m_s##X[i];                                                         \
    }                                                                           \
    e_forceinline_always bool is##X( const string& str )const{                  \
      return( m_s##X == str );                                                  \
    }                                                                           \
    e_forceinline_always const::EON::gfc::string& to##X()const{                 \
      return m_s##X;                                                            \
    }                                                                           \
    e_forceinline_always::EON::gfc::string& to##X(){                            \
      return m_s##X;                                                            \
    }                                                                           \
    e_forceinline_always void set##X##Property( const::EON::gfc::string& s ){   \
      toProperty( #X )->setValue( s );                                          \
    }                                                                           \
    e_forceinline_always void set##X( const::EON::gfc::string& s ){             \
      m_s##X = s;                                                               \
    }                                                                           \
    e_forceinline_always void set##X##Property( ccp s ){                        \
      toProperty( #X )->setValue( string( s ));                                 \
    }                                                                           \
    e_forceinline_always void set##X( ccp s ){                                  \
      m_s##X = s;                                                               \
    }                                                                           \
  private:                                                                      \
    ::EON::gfc::string m_s##X = ___populate##X()                                \

    //}:                                          |
    //e_property_array:{                          |

#define e_property_array( T, X, N, READONLY, F, L, TIP )                        \
  public:                                                                       \
    using X=::EON::gfc::array<T,u32( N )>;                                      \
  private:                                                                      \
    e_noinline_or_debug::EON::gfc::string::pairs<u32( N )>___strings##X()const{ \
      const auto& onPopulator##X = L;                                           \
      return onPopulator##X();                                                  \
    }                                                                           \
    e_noinline_or_debug::EON::gfc::string ___getArrayElementString##X(          \
        const::EON::gfc::Property* pProperty )const{                            \
      return ___strings##X()[ pProperty->toElement() ].first;                   \
    }                                                                           \
    e_noinline_or_debug::EON::gfc::string ___getArrayString##X( const u32 n )   \
          const{                                                                \
      return e_xfs( "[%03u]", n );                                              \
    }                                                                           \
    e_noinline_or_debug void ___tickrrayProperty##X( Property* P ){             \
      P->setDescription( e_strof( m_a##X[ P->toElement() ]));                   \
    }                                                                           \
    e_noinline_or_debug const X ___populate##X( const u32 numChildren ){        \
      prop_ptr pProperty = toProperty( #X );                                    \
      if( !pProperty ){                                                         \
        pProperty = std::make_shared<::EON::gfc::Property>( this                \
          , #X                                                                  \
          , &m_a##X                                                             \
          , u32( sizeof( m_a##X ))                                              \
          , e_classid<T>() );                                                   \
        ___head_property->toChildren().set( e_hashstr64_const( #X )             \
             , pProperty );                                                     \
               pProperty->setParent(___head_property.get() );                   \
      }                                                                         \
      const auto& childLabels = ___strings##X();                                \
      const u32 n = e_min( numChildren, childLabels.size() );                   \
      pProperty->setDescription(___getArrayString##X( n ));                     \
      pProperty->toFlags()->bReadOnly  = READONLY;                              \
      pProperty->toFlags()->bCollapsed = true;                                  \
      pProperty->toFlags()->bIsArray   = true;                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->toChildren().clear();                                          \
      for( u32 i=0; i<n; ++i ){                                                 \
        const auto& childLabel = childLabels[ i ];                              \
        prop_ptr pChildProperty = std::make_shared<::EON::gfc::Property>( this  \
            , childLabel.first                                                  \
            , &m_a##X[ i ]                                                      \
            , u32( sizeof( m_a##X[ i ]))                                        \
            , e_classid<T>() );                                                 \
        pChildProperty->toFlags()->bReadOnly = READONLY;                        \
        pChildProperty->setDescription( childLabel.second );                    \
        pChildProperty->setElement( i );                                        \
        pChildProperty->setTick(                                                \
            ::EON::gfc::Property::OnTick(                                       \
                &self::___tickrrayProperty##X ));                               \
        if( !(READONLY) ){                                                      \
          pProperty->setChanged(                                                \
              ::EON::gfc::Property::OnChanged(                                  \
                  &self::F ));                                                  \
        }                                                                       \
        pProperty->toChildren().set( pChildProperty->toName().hash()            \
          , pChildProperty                                                      \
        );                                                                      \
      }                                                                         \
      return X{};                                                               \
    }                                                                           \
  public:                                                                       \
    template<typename E> e_forceinline_always void set##X##Property(            \
          const X& a##X ){                                                      \
      toProperty( #X )->setValue( a##X );                                       \
    }                                                                           \
    template<typename E> e_forceinline_always void set##X(                      \
          const X& a##X ){                                                      \
      m_a##X = a##X;                                                            \
    }                                                                           \
    template<typename E> e_forceinline_always bool setIn##X( const E e,         \
        const T& t ){                                                           \
      T& value = m_a##X[ e ];                                                   \
      if( &value != &t ){                                                       \
        value = t;                                                              \
        return true;                                                            \
      }                                                                         \
      return false;                                                             \
    }                                                                           \
    template<typename E> e_forceinline_always const T& in##X( const E e )const{ \
      return m_a##X[ u32( e )];                                                 \
    }                                                                           \
    template<typename E> e_forceinline_always T& in##X( const E e ){            \
      return m_a##X[ e ];                                                       \
    }                                                                           \
    e_forceinline_always string in##X##Strings( const s64 i )const{             \
      return ___strings##X()[ i ].first;                                        \
    }                                                                           \
    e_forceinline_always void clear##X(){                                       \
      m_a##X = X{};                                                             \
    }                                                                           \
    e_forceinline_always void populate##X( const u32 numChildren ){             \
      ___populate##X( numChildren );                                            \
    }                                                                           \
  private:                                                                      \
    mutable X m_a##X = ___populate##X( u32( N ))                                \

#define e_property_array1( T, N, READONLY, F, L, TIP )                          \
  e_property_array(  T, T##s, N, READONLY, F, L, TIP )                          \

    //}:                                          |
    //e_property_color:{                          |

#define e_property_color( X, VALUE, READONLY, F, TIP )                          \
  private:                                                                      \
    e_noinline_or_debug string ___getColorString##X(                            \
        const::EON::gfc::Property* )const{                                      \
      return e_strof( m_c##X );                                                 \
    }                                                                           \
    e_noinline_or_debug static cvp ___default##X(){                             \
      static rgba kDefault##X = VALUE;                                          \
      return    & kDefault##X;                                                  \
    }                                                                           \
    e_noinline_or_debug void ___tickColorProperty##X( Property* P ){            \
      P->setDescription(___getColorString##X( P ));                             \
    }                                                                           \
    e_noinline_or_debug const rgba ___populate##X(){                            \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_c##X                                                             \
          , u32( sizeof( m_c##X ))                                              \
          , e_classid( m_c##X ));                                               \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setDefault( &___default##X );                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickColorProperty##X ));                                \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return VALUE;                                                             \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always static const rgba default##X(){                        \
      return VALUE;                                                             \
    }                                                                           \
    e_forceinline_always void set##X##Property( const rgba& c ){                \
      toProperty( #X )->setValue( c );                                          \
    }                                                                           \
    e_forceinline_always void set##X( const rgba& c ){                          \
      m_c##X = c;                                                               \
    }                                                                           \
    e_forceinline_always bool is##X( const rgba& col )const{                    \
      return( m_c##X == col );                                                  \
    }                                                                           \
    e_forceinline_always const rgba& to##X()const{                              \
      return m_c##X;                                                            \
    }                                                                           \
    e_forceinline_always rgba& to##X(){                                         \
      return m_c##X;                                                            \
    }                                                                           \
  private:                                                                      \
    rgba m_c##X = ___populate##X()                                              \

    //}:                                          |
    //e_property_bool:{                           |

#define e_property_bool( X, VALUE, READONLY, F, TIP )                           \
  private:                                                                      \
    e_noinline_or_debug string ___getBoolString##X(                             \
        const::EON::gfc::Property* )const{                                      \
      return m_b##X ? "Yes" : "No";                                             \
    }                                                                           \
    e_noinline_or_debug static cvp ___default##X(){                             \
      static bool kDefault##X = VALUE;                                          \
      return    & kDefault##X;                                                  \
    }                                                                           \
    e_noinline_or_debug void ___tickBoolProperty##X( Property* P ){             \
      P->setDescription(___getBoolString##X( P ));                              \
    }                                                                           \
    e_noinline_or_debug bool ___populate##X(){                                  \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X"?"                                                               \
          , &m_b##X                                                             \
          , u32( sizeof( m_b##X ))                                              \
          , e_classid( m_b##X ));                                               \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setDefault( &___default##X );                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickBoolProperty##X ));                                 \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X"?" )            \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return VALUE;                                                             \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always static const bool default##X(){                        \
      return VALUE;                                                             \
    }                                                                           \
    e_forceinline_always bool to##X()const{                                     \
      return m_b##X;                                                            \
    }                                                                           \
    e_forceinline_always bool& to##X(){                                         \
      return m_b##X;                                                            \
    }                                                                           \
    e_forceinline_always void set##X##Property( const bool b ){                 \
      toProperty( #X"?" )->setValue( b );                                       \
    }                                                                           \
    e_forceinline_always void set##X( const bool b ){                           \
      m_b##X = b;                                                               \
    }                                                                           \
    e_forceinline_always void disable##X##Property(){                           \
      set##X##Property( false );                                                \
    }                                                                           \
    e_forceinline_always void enable##X##Property(){                            \
      set##X##Property( true );                                                 \
    }                                                                           \
    e_forceinline_always void disable##X(){                                     \
      set##X( false );                                                          \
    }                                                                           \
    e_forceinline_always void enable##X(){                                      \
      set##X( true );                                                           \
    }                                                                           \
    e_forceinline_always bool is##X()const{                                     \
      return m_b##X;                                                            \
    }                                                                           \
    e_forceinline_always bool is##X( const bool val )const{                     \
      return( m_b##X == val );                                                  \
    }                                                                           \
  private:                                                                      \
    bool m_b##X = ___populate##X()                                              \

    //}:                                          |
    //e_property_enum:{                           |

      /** \brief Create enumerated type property.
        *
        * This macro will define and code generate a special enum property in a
        * class or structure. All the setters, getters and query method you may
        * ever need are produced including toProperty*(), default*(), is*() and
        * to*().
        \code
          // Given the property enum:
          struct MyClass:Object{
            e_reflect( MyClass, Object );
            enum class MyEnum:u32{
              kRed,
              kYellow,
              kBlue
            };
            e_property_enum( MyEnum
                , MyEnumVar
                , MyEnum::kRed
                , true
                , IGNORE_CHANGES
                , "Red"
                , "Yellow"
                , "Blue" );
          };
        \endcode
        * The exact same thing could be rewritten a little more succinctly.
        \code
          // Given the property enum:
          struct MyClass:Object{
            e_reflect( MyClass, Object );
            enum class MyEnum:u32{
              kRed,
              kYellow,
              kBlue
            };
            void MyEnumChanged(){
              // Called when the enum property is modified wither by the set
              // method or via the property system.
            }
            e_property_enum1( MyEnum
                , MyEnum::kRed
                , true
                , MyEnumChanged
                , "Red"
                , "Yellow"
                , "Blue" );
          };
        \endcode
        *
        * \param E The type of the enum or enum class.
        * \param X The name of the member variable prefixed with m_e.
        * \param VALUE The default value, accessible by the default*() method.
        * \param F The on changed method defined in the same class.
        * \param __VAR_ARGS__ A zero terminated array of strings matching the
        * entires in the enumerated type for the Swordlight property window to
        * consume.
        */

#define e_property_enum( E, X, VALUE, READONLY, F, TIP, ... )                   \
  public:                                                                       \
    e_noinline_or_debug static const::EON::gfc::strings&___strings##X(){        \
      static::EON::gfc::strings s_vStrings{__VA_ARGS__,0,0};                    \
      return s_vStrings;                                                        \
    }                                                                           \
  private:                                                                      \
    e_noinline_or_debug string ___getEnumString##X(                             \
        const::EON::gfc::Property* )const{                                      \
      return ___strings##X()[ s64( m_e##X )];                                   \
    }                                                                           \
    e_noinline_or_debug static cvp ___default##X(){                             \
      static E kDefault##X = E::VALUE;                                          \
      return & kDefault##X;                                                     \
    }                                                                           \
    e_noinline_or_debug void ___tickEnumProperty##X(::EON::gfc::Property* P ){  \
      P->setDescription(___getEnumString##X( P ));                              \
    }                                                                           \
    e_noinline_or_debug E ___populate##X(){                                     \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_e##X                                                             \
          , u32( sizeof( m_e##X ))                                              \
          , e_classid( m_e##X ));                                               \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->toFlags()->bIsEnum = true;                                     \
      pProperty->setEnums( in##X##Strings );                                    \
      pProperty->setDefault( &___default##X );                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickEnumProperty##X ));                                 \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return E::VALUE;                                                          \
    }                                                                           \
  public:                                                                       \
    template<typename I> e_forceinline_always static string                     \
           in##X##Strings( const I i ){                                         \
      return ___strings##X()[ u32( i )];                                        \
    }                                                                           \
    e_forceinline_always static E default##X(){                                 \
      return E::VALUE;                                                          \
    }                                                                           \
    e_forceinline_always void set##X##Property( const E e##X ){                 \
      toProperty( #X )->setValue( e##X );                                       \
    }                                                                           \
    e_forceinline_always void set##X( const E e##X ){                           \
      m_e##X = e##X;                                                            \
    }                                                                           \
    e_forceinline_always bool is##X( const E e##X ){                            \
      return( m_e##X == e##X );                                                 \
    }                                                                           \
    e_forceinline_always E to##X()const{                                        \
      return m_e##X;                                                            \
    }                                                                           \
    e_forceinline_always E& to##X(){                                            \
      return m_e##X;                                                            \
    }                                                                           \
    e_forceinline void populate##X(){                                           \
      ___populate##X();                                                         \
    }                                                                           \
  private:                                                                      \
    E m_e##X = ___populate##X()                                                 \

      /** \brief Create enumerated type property.
        *
        * This macro is identical to the one named e_property_enum except for
        * one thing: you do do not have to specify a name. The name will be
        * taken from the type which is often the case with one off enums.
        *
        * \param E The type and name of the enumerated type.
        * \param VALUE The default value of the member variable.
        * \param READONLY True if the property system can modify this member.
        * \param F The callback method defined within the same class that gets
        * invoked whenever the property system changes the member variable or
        * the user calls set##E) on it. READONLY and F are mutually exclusive.
        */

#define e_property_enum1( E, VALUE, READONLY, F, TIP, ... )                     \
  e_property_enum(     E, E, VALUE, READONLY, F, TIP, __VA_ARGS__ )             \

    //}:                                          |
    //e_property:{                                |

      /** \brief Define a member property.
        *
        * Properties are special in the editor. They not only provide a host of
        * accessor methods just like their e_var counterparts but also provide
        * a method to get the property object generated for the type: called
        * toPropertyPropname(). Another very important thing to remember about
        * properties compared to member variables defined with e_var*() is that
        * properties will not return a reference to the member variable with
        * the non-const accessor toPropname(). You must use setPropname()
        * so we can check if the onChanged callback in the property is non-zero
        * and call it if it is when set is called and the new value changes the
        * property. Any e_var declared member will never do this and will
        * return a reference to the type on the non-const toVarname() kind.
        *
        * The reason for properties is simply to enable a very easy way of
        * making member variables show up in the properties window in the
        * Swordlight IDE without a lot of work on the programmer's part. See
        * property.cpp for the code that returns properties for Swordlight to
        * use.
        *
        * Please remember that like e_var* e_property* will declare all member
        * variables as protected and all methods as public. The scope after the
        * e_property* line will be changed to public as if you had said public:
        *
        * NB: It is also absolutely critical to remember that e_property macros
        * of all varieties cannot be used outside of an Object derived class
        * with a valid e_reflect() statement in it. The property macros all
        * reply heavily on the self type generated by e_reflect().  in your
        * header source.
        *
        * \param T The type of the member variable to declare.
        * \param P The prefix to the member variable. It will be named in
        * Hungarian notion as m_prefixVarname. Engine coding standards state
        * that a structure should be "t", a pointer "p", a string "s", a
        * reference "r", a vector "v", a map "m", and an array "a". For
        * pointers, vectors, maps, strings and arrays there exists a special
        * e_var_* (e_var_string or e_var_map for example) that adds the right
        * prefix for you and adds additional methods on a case by case basis. A
        * member variable declared with e_var_vector(), for example, will add
        * methods for finding elements in the vector and clearing it.
        * \param X The name of the variable. The name should be in mixed case,
        * so it starts with a capital letter.
        * \param VALUE The default value of the property. This is extremely
        * important for the XML serializer which won't write out an attribute
        * or element unless the value of the property is different from what
        * was set in the constructor. The constructor should always use the
        * generated methods def##Varname() to initialize the property.
        * \param VALUE The default value of the property.
        * \param READONLY If true the property will not be editable.
        * \param F The name of a member function callback that gets called when
        * the property is changed or IGNORE_CHANGES.
        */

#define e_property( T, P, X, VALUE, READONLY, F, TIP )                          \
  private:                                                                      \
    e_noinline_or_debug string ___getPropertyString##X(                         \
          const::EON::gfc::Property* )const{                                    \
      return e_strof<T>( m_##P##X );                                            \
    }                                                                           \
    e_noinline_or_debug static cvp ___default##X(){                             \
      static T kDefault##X = VALUE;                                             \
      return & kDefault##X;                                                     \
    }                                                                           \
    e_noinline_or_debug void ___tickProperty##X( Property* P ){                 \
      P->setDescription(___getPropertyString##X( P ));                          \
    }                                                                           \
    e_noinline_or_debug const T ___populate##X(){                               \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #X                                                                  \
          , &m_##P##X                                                           \
          , u32( sizeof( m_##P##X ))                                            \
          , e_classid( m_##P##X ));                                             \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setDefault( &___default##X );                                  \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::___tickProperty##X ));                                     \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #X )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return VALUE;                                                             \
    }                                                                           \
  public:                                                                       \
    e_forceinline_always static const T default##X(){                           \
      return VALUE;                                                             \
    }                                                                           \
    e_forceinline_always void set##X##Property( const T& t##X ){                \
      toProperty( #X )->setValue( t##X );                                       \
    }                                                                           \
    e_forceinline_always void set##X( const T& t##X ){                          \
      m_##P##X = t##X;                                                          \
    }                                                                           \
    e_forceinline_always bool is##X( const T& ref )const{                       \
      return( m_##P##X == ref );                                                \
    }                                                                           \
    e_forceinline_always const T& to##X()const{                                 \
      return m_##P##X;                                                          \
    }                                                                           \
    e_forceinline_always T& to##X(){                                            \
      return m_##P##X;                                                          \
    }                                                                           \
  private:                                                                      \
    T m_##P##X = ___populate##X()                                               \

    //}:                                          |
    //e_property_alias:{                          |

      /** \brief Define a property alias to a nested member variable.
        *
        * This macro adds a property to this object's root hierarchy that
        * references a member declared with e_var*. The entity and instance
        * objects, for example, use property aliasing to display the position,
        * rotation and scale of the qst3 member L2W instead of trying to
        * display the string'ized version of the qst.
        *
        * \param X The name of the outer member variable.
        * \param Y The name of the inner member variable.
        * \param READONLY Whether to treat this property as readonly or not.
        * \param F The callback function to be called if Y changes.
        \code
          struct MyExample:Object{

            e_reflect( MyExample, Object );

          private:

            e_property_alias( L2W, Position, false, IGNORE_CHANGES );
            e_property_alias( L2W, Rotation, false, IGNORE_CHANGES );
            e_property_alias( L2W, Scale,    false, IGNORE_CHANGES );

            e_var( qst3, t, L2W );
          };
        \endcode
        */

#define e_property_alias( X, Y, READONLY, F, TIP )                              \
  private:                                                                      \
    e_noinline_or_debug string ___getAliasString##Y##Of##X(                     \
        const::EON::gfc::Property* )const{                                      \
      return e_strof( to##X().to##Y() );                                        \
    }                                                                           \
    e_noinline_or_debug void __tickAliasProperty##Y##Of##X( Property* P ){      \
      P->setDescription(___getAliasString##Y##Of##X( P ));                      \
    }                                                                           \
    prop_ptr ___populate##Y##Of##X(){                                           \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #Y                                                                  \
          , &to##X().to##Y()                                                    \
          , u32( sizeof( to##X().to##Y() ))                                     \
          , e_classid( to##X().to##Y() ));                                      \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::__tickAliasProperty##Y##Of##X ));                          \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #Y )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return pProperty;                                                         \
    }                                                                           \
    prop_ptr ___property##Y##Of##X = ___populate##Y##Of##X()                    \

#define e_property_alias_as( T, X, Y, READONLY, F, TIP )                        \
  private:                                                                      \
    e_noinline_or_debug string ___getAliasString##Y##Of##X(                     \
        const::EON::gfc::Property* )const{                                      \
      return e_strof( reinterpret_cast<const T&>( to##X().to##Y() ));           \
    }                                                                           \
    e_noinline_or_debug void __tickAliasProperty##Y##Of##X( Property* P ){      \
      P->setDescription(___getAliasString##Y##Of##X( P ));                      \
    }                                                                           \
    prop_ptr ___populate##Y##Of##X(){                                           \
      prop_ptr pProperty = std::make_shared<::EON::gfc::Property>( this         \
          , #Y                                                                  \
          , &to##X().to##Y()                                                    \
          , u32( sizeof( T ))                                                   \
          , e_classid<T>() );                                                   \
      pProperty->toFlags()->bReadOnly = READONLY;                               \
      pProperty->setTooltip( TIP );                                             \
      pProperty->setTick(                                                       \
          ::EON::gfc::Property::OnTick(                                         \
              &self::__tickAliasProperty##Y##Of##X ));                          \
      if( !(READONLY) ){                                                        \
        pProperty->setChanged(                                                  \
            ::EON::gfc::Property::OnChanged(                                    \
                &self::F ));                                                    \
      }                                                                         \
      ___head_property->toChildren().set( e_hashstr64_const( #Y )               \
           , pProperty );                                                       \
             pProperty->setParent(___head_property.get() );                     \
      return pProperty;                                                         \
    }                                                                           \
    prop_ptr ___property##Y##Of##X = ___populate##Y##Of##X()                    \

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
