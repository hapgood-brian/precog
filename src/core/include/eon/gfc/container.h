//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

//http://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special/7189821#7189821

#pragma once

#if e_compiling( microsoft )
  #include<algorithm>
#endif
#include<unordered_map>
#include<deque>

/** \addtogroup engine
  * @{
  *   \addtogroup gfc
  *   @{
  *     \addtogroup containers
  *     @{
  */

#if e_compiling( clang )
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#elif e_compiling( microsoft )
  #pragma warning( disable:4200 )
#elif e_compiling( gcc )
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsometimes-uninitialized"
#endif

//================================================+=============================
//Functions:{                                     |

  namespace EON{

    namespace gfc{

      /** \brief Iterator caster.
        *
        * This routine will cast one iterator to another. It is used as a
        * common bit of code throughout the container system.
        *
        * \tparam _gfc_container One of the gfc container types.
        * \tparam _std_container The STL type of the same name.
        * \param ci A reference to the gfc container's const_iterator.
        * \param container A reference to the underlying STL container.
        *
        * \return Returns a new gfc iterator.
        */

      template<typename _gfc_container, typename _std_container> e_noinline typename _std_container::iterator e_castIterator( const typename _gfc_container::const_iterator& ci, const _std_container& container ){
        _std_container& stl = const_cast<_std_container&>( container );
        if( !container.empty() ){
          return typename _std_container::iterator( stl.begin()+ci.index() );
        }
        return typename _std_container::iterator( stl.end() );
      }
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//bitbucket:{                                     |

  namespace EON{

    namespace gfc{

#ifdef __APPLE__
  #pragma mark - bitbucket -
#endif

      /** \brief Bit bucket class.
        *
        * This class is an allocation strategy based on a bitmap of unsigned
        * 64-bit integers. Each bit in said size can be set, queried and reset.
        * Also there is a method for setting a bit in the next available bit
        * bucket, which you can use for controlling more structures in thread
        * local memory.  This is precisely how the ShareLockRecursive class
        * works, which relies heavily on bit-bucket objects. It has been
        * heavily tested and is guaranteed to be an extremely fast way of
        * synchronizing thread states.
        */

      template<u64 N> struct E_PUBLISH bitbucket final{

        //----------------------------------------+-----------------------------
        //Operate:{                               |

          /** \brief Bit bucket comparison operator.
            *
            * This routine checks the equality of two bitbuckets.
            */

          e_forceinline bool operator!=( const bitbucket& bb )const{
            return( 0!=memcmp( this, &bb, sizeof( *this )));
          }

          /** \brief Bit bucket comparison operator.
            *
            * This routine checks the equality of two bitbuckets.
            */

          e_forceinline bool operator==( const bitbucket& bb )const{
            return !memcmp( this, &bb, sizeof( *this ));
          }

          /** \brief Bit bucket copy operator.
            *
            * This routine will copy the lvalue bitbucket into this.
            *
            * \param lvalue The left value to copy.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_noinline bitbucket& operator=( const bitbucket& lvalue ){
            for( u32 i=0; i<m_uSize; ++i ){
              m_aBuckets[ i ].store( lvalue.m_aBuckets[ i ]);
            }
            return *this;
          }

          /** \brief Bit bucket move operator.
            *
            * This routine will copy the values of the rvalue buckets to this
            * and zero them out on the right.
            *
            * \param rvalue The rvalue reference bit bucket to move.
            *
            * \return Returns the dereferenced this point.
            */

          e_noinline bitbucket& operator=( bitbucket&& rvalue ){
            for( u32 i=0; i<m_uSize; ++i ){
              m_aBuckets[ i ].store( rvalue.m_aBuckets[ i ]);
            }
            rvalue.clear();
            return *this;
          }

          /** \brief Bracket operator.
            *
            * This routine will return the Nth element in the bitbucket. This
            * is the const version of the operator. Please note that this
            * method is not thread safe. Only use it if you KNOW another thread
            * isn't going to reallocate the bitbucket.
            *
            * \param i The Nth element in the bitbucket to dereference.
            *
            * \return Returns the i element in the bitbucket.
            */

          e_forceinline bool operator[]( const u64 i )const{
            return is( i );
          }

        //}:                                      |
        //Methods:{                               |

          /** \brief Is bit set?
            *
            * This routine will atomically test whether the bit with the given
            * incoming index is set in the buckets array.
            *
            * \param indx A bit index between zero and N.
            *
            * \return Returns true if the bit at the given bit array index is
            * one and false otherwise.
            */

          e_forceinline bool is( const u64 indx )const{
            if( indx >= m_uSize ){
              return false;
            }
            const u64 bckt = indx / 64ULL;
            const u64 offs = indx % 64ULL;
            const u64 mask = 1ULL << offs;
            return( mask==( m_aBuckets[ bckt ] & mask ));
          }

          /** \brief Reset bit in the bit array.
            *
            * This routine will reset (clear to zero) the bit with the given
            * index.  It does the test atomically.
            *
            * \param indx A bit index between zero and N.
            *
            * \return Returns true if the bit was set beforehand and false if
            * it was zero.
            */

          e_forceinline bool reset( const u64 indx ){
            const u64 bckt = indx / 64ULL;
            const u64 offs = indx % 64ULL;
            const u64 mask = 1ULL << offs;
            const u64 prev = m_aBuckets[ bckt ].fetch_and( ~mask, std::memory_order_release );
            return( mask == ( prev & mask ));
          }

          /** \brief Set a bit in the bucket.
            *
            * This routine will set a location in the bit array indexed by the
            * arg value. It will do this atomically so it's wonderfully thread
            * safe.
            *
            * \param indx A bit index between zero and N.
            *
            * \return Returns true if the bit was previously zero and was set
            * to one by this operation and false otherwise.
            */

          e_forceinline bool set( const u64 indx ){
            const u64 bckt = indx / 64ULL;
            const u64 offs = indx % 64ULL;
            const u64 mask = 1ULL << offs;
            const u64 prev = m_aBuckets[ bckt ].fetch_or( mask, std::memory_order_acquire );
            return !( prev & mask );
          }

          /** \brief Set the next available empty bit and return its index.
            *
            * This routine will atomically scan through the bit bucket looking
            * for a zero value, set it to one then return the index of the
            * location in the bit array.
            *
            * \return Will only return a valid index between zero and N. If
            * there are no other threads having acquired certains bits in the
            * array and there are no more buckets left with free bits in them
            * then the code spins for a time-out period of ten seconds then
            * fire off a trapping signal for the debugger. Release builds this
            * will crash the program.
            *
            * Returns the index in the bit array between zero and N where the
            * bit was set. This value can then be passed into either reset or
            * set later.
            */

          e_noinline u64 set(){
            for( u64 n=bitcount(), i=0; i<n; ++i ){
              const u64 j = m_uHead.fetch_add( 1 ) % n;
              if( set( j )){
                return j;
              }
            }
            e_unreachable( "Pool depleted!" );
          }

          constexpr static u64 bitcount(){
            return(( N+63ULL )&~63ULL );
          }

          e_forceinline u64 size()const{
            return m_uSize;
          }

          /** \brief Clear all bits.
            *
            * This routine will clear all bits in every bucket.
            */

          e_forceinline void clear(){
            memset( m_aBuckets, 0, sizeof( m_aBuckets ));
            m_uHead = 0;
          }

          /** \brief Is empty.
            *
            * This routine will return true if all buckets contain zero.
            */

          e_forceinline bool empty()const{
            for( u32 i=0; i<e_sizeof( m_aBuckets ); ++i ){
              if( m_aBuckets[ i ]){
                return false;
              }
            }
            return true;
          }

          /** \brief Is empty bucket.
            *
            * This routine will return true if an entire bucket is zero.
            */

          e_forceinline bool empty( const u64 bucket ){
            e_assert( bucket < bitcount()/64, "Bad bucket!" );
            return( m_aBuckets[ bucket ]==0ULL );
          }

          /** \brief Is full bucket.
            *
            * This routine will return true if an entire bucket is zero.
            */

          e_forceinline bool full( const u64 bucket ){
            e_assert( bucket < bitcount()/64, "Bad bucket!" );
            return( m_aBuckets[ bucket ]==~0ULL );
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        bitbucket( const bitbucket& lvalue ){
          for( u64 n=bitcount()/64, i=0; i<n; ++i ){
            m_aBuckets[ i ].store( lvalue.m_aBuckets[ i ]);
          }
        }

        bitbucket( bitbucket&& rvalue ){
          for( u64 n=bitcount()/64, i=0; i<n; ++i ){
            m_aBuckets[ i ].store( rvalue.m_aBuckets[ i ]);
          }
          rvalue.clear();
        }

      ~ bitbucket() = default;
        bitbucket(){
          clear();
        }

      private:

        const u64        m_uSize = N;
        std::atomic<u64> m_uHead;
        #if e_compiling( microsoft )
          std::atomic<u64> m_aBuckets[(( N+63 ) & ~63 )/64 ];
        #else
          std::atomic<u64> m_aBuckets[ bitcount()/64 ];
        #endif
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//hashmap:{                                       |

#ifdef __APPLE__
  #pragma mark - hashmap -
#endif

  namespace EON{

    namespace gfc{

      /** \brief GFC hashmap class.
        *
        * This template wraps the standard library hashmap<> and provides a
        * number of useful additions like easier-to-use iterators and helper
        * functions.
        */

      template<typename K,typename T> class E_PUBLISH hashmap final{

#ifdef __APPLE__
  #pragma mark (types)
#endif

        //----------------------------------------+-----------------------------
        //Structs:{                               |

          struct Y{
            template<typename E> std::size_t operator()( const E e )const{
              return std::size_t( e );
            }
          };

          struct UnorderedMap:std::unordered_map<K,T,Y>{
            using std::unordered_map<K,T,Y>::unordered_map;
          };

        //}:                                      |
        //Aliases:{                               |

          using X = UnorderedMap;

        //}:                                      |
        //----------------------------------------+-----------------------------

      public:

        //----------------------------------------+-----------------------------
        //Structs:{                               |
          //const_iterator:{                      |

#ifdef __APPLE__
  #pragma mark hashmap::const_iterator
#endif

            struct E_PUBLISH const_iterator final{

              //----------------------------------+-----------------------------
              //Operate:{                         |

                /** \brief Addition operator.
                  *
                  * This operator mimics the std::unordered_map::const_iterator
                  * in it's entirety.
                  *
                  * \param i The number of elements to step forward.
                  *
                  * \return Returns a new const_iterator that represents the
                  * new place in the table.
                  */

                e_noinline const_iterator operator+( const s32 i )const{
                  const_iterator r( *this );
                  r.cit += i;
                  return r;
                }

                /** \brief Subtraction operator.
                  *
                  * This operator mimics the std::unordered_map::const_iterator
                  * in it's entirety.
                  *
                  * \param i The number of elements to step backward.
                  *
                  * \return Returns a new const_iterator that represents the
                  * new place in the table.
                  */

                e_noinline const_iterator operator-( const s32 i )const{
                  const_iterator r( *this );
                  r.cit -= i;
                  return r;
                }

                e_forceinline void operator+=( const s32 i ){
                  cit += i;
                }

                e_forceinline void operator-=( const s32 i ){
                  cit -= i;
                }

                e_forceinline const T* operator->()const{
                  return cit;
                }

                e_forceinline const T& operator*()const{
                  return cit->second;
                }

                e_forceinline operator bool()const{
                  return( m_pOuter && ( cit != m_pOuter->m_mMap.end() ));
                }

                e_forceinline const_iterator operator++(int){
                  return const_iterator( *m_pOuter, cit++ );
                }

                e_forceinline const_iterator operator++(){
                  return const_iterator( *m_pOuter, ++cit );
                }

              //}:                                |
              //Methods:{                         |

                /** \brief Reset iterator to beginning.
                  *
                  * This routine will reset the iterator back to the beginning
                  * of the vector.
                  */

                e_forceinline void reset(){
                  cit = m_pOuter->m_mMap.begin();
                }

                e_forceinline K key()const{
                  return cit->first;
                }

                e_forceinline const T* value()const{
                  return &cit->second;
                }

                e_forceinline void detach(){
                  m_pOuter = nullptr;
                }

              //}:                                |
              //----------------------------------+-----------------------------

              e_noinline const_iterator( const const_iterator& ci )
                : cit( ci.cit )
                , m_tGuard( &ci.m_pOuter->m_tLock, true )
                , m_pOuter( ci.m_pOuter )
              {}

              e_noinline const_iterator( const_iterator&& rvalue )
                  : cit( std::move( rvalue.cit ))
                  , m_tGuard( &rvalue.m_pOuter->m_tLock, true )
                  , m_pOuter( rvalue.m_pOuter ){
                rvalue.m_pOuter = nullptr;
              }

              const_iterator() = default;
            ~ const_iterator() = default;

            private:

              e_noinline explicit const_iterator( const hashmap& outer, const typename X::const_iterator& ci )
                : cit( ci )
                , m_tGuard( &outer.m_tLock, true )
                , m_pOuter( &outer )
              {}

              e_noinline explicit const_iterator( const hashmap& outer )
                : cit( outer.m_mMap.begin() )
                , m_tGuard( &outer.m_tLock, true )
                , m_pOuter( &outer )
              {}

              atomic::ShareLockRecursive::LockGuard m_tGuard;
              const hashmap* m_pOuter = nullptr;
              typename X::const_iterator cit;
              friend class hashmap;
            };

          //}:                                    |
          //iterator:{                            |

#ifdef __APPLE__
  #pragma mark hashmap::iterator
#endif

            struct E_PUBLISH iterator final{

              //----------------------------------+-----------------------------
              //Operate:{                         |

                e_forceinline iterator operator+( const s32 i )const{
                  iterator r( *this );
                  r.it += i;
                  return r;
                }

                e_forceinline iterator operator-( const s32 i )const{
                  iterator r( *this );
                  r.it -= i;
                  return r;
                }

                e_forceinline void operator+=( const s32 i ){
                  it += i;
                }

                e_forceinline void operator-=( const s32 i ){
                  it -= i;
                }

                e_forceinline const T& operator*()const{
                  return it->second;
                }

                e_forceinline operator bool()const{
                  return m_pOuter && ( it != m_pOuter->m_mMap.end() );
                }

                e_forceinline T* operator->()const{
                  return &it->second;
                }

                e_forceinline iterator operator++(int){
                  return iterator( *m_pOuter, it++ );
                }

                e_forceinline iterator operator++(){
                  return iterator( *m_pOuter, ++it );
                }

              //}:
              //Methods:{

                /** \brief Erase element at iterator.
                  *
                  * This routine will nuke the object at the iterator's current
                  * position.  This is great for lists but DO NOT call it more
                  * than once in the same loop or you'll crash.
                  */

                e_forceinline void erase(){
                  m_pOuter->m_mMap.erase( it );
                }

                /** \brief Reset iterator to beginning.
                  *
                  * This routine will reset the iterator back to the beginning
                  * of the vector.
                  */

                e_forceinline void reset(){
                  it = m_pOuter->m_mMap.begin();
                }

                e_forceinline K key()const{
                  return it->first;
                }

                e_forceinline T* value(){
                  return &it->second;
                }

                e_forceinline void detach(){
                  m_pOuter = nullptr;
                }

              //}:                                |
              //----------------------------------+-----------------------------

              /** \brief Copy constructor.
                *
                * This routine will initialize the iterator by making a copy of
                * the outer class of type gfc::hashmap.
                *
                * \param ci The iterator to copy.
                */

              e_noinline iterator( const const_iterator& ci )
                : it( e_castIterator<hashmap>( ci, ci.m_pOuter->m_mMap ))
                , m_pOuter(  const_cast<hashmap*>( ci.m_pOuter ))
                , m_tGuard( &const_cast<hashmap*>( ci.m_pOuter )->m_tLock, false )
              {}

              /** \brief Copy constructor.
                *
                * This routine will initialize the iterator by making a copy of
                * the outer class of type gfc::hashmap.
                *
                * \param ci The iterator to copy.
                */

              e_noinline iterator( const iterator& ci )
                : m_tGuard( &ci.m_pOuter->m_tLock, false )
                , m_pOuter( ci.m_pOuter )
                , it( ci.it )
              {}

              /** \brief Move constructor.
                *
                * This routine will initialize the iterator by moving the outer
                * class of type gfc::hashmap.
                *
                * \param rvalue The iterator to move.
                */

              e_noinline iterator( iterator&& rvalue )
                  : m_tGuard( &rvalue.m_pOuter->m_tLock, false )
                  , m_pOuter( rvalue.m_pOuter )
                  , it( std::move( rvalue.it )){
                rvalue.m_pOuter = nullptr;
              }

              iterator() = default;
            ~ iterator() = default;

            private:

              e_forceinline iterator( hashmap& outer, const typename X::iterator& i )
                : m_tGuard( &outer.m_tLock, false )
                , m_pOuter( &outer )
                , it( i )
              {}

              e_forceinline explicit iterator( hashmap& outer )
                : m_tGuard( &outer.m_tLock, false )
                , m_pOuter( &outer )
                , it( outer.m_mMap.begin() )
              {}

              atomic::ShareLockRecursive::LockGuard m_tGuard;
              hashmap* m_pOuter = nullptr;
              typename X::iterator it;
              friend class hashmap;
            };

          //}:                                    |
        //}:                                      |
        //Operate:{                               |

#ifdef __APPLE__
  #pragma mark hashmap operators
#endif

          /** \brief Bracket operator.
            *
            * This routine will return the Nth element in the hashmap. This is
            * the const version of the operator. Please note that this method
            * is not thread safe. Only use it if you KNOW another thread isn't
            * going to reallocate the hashmap.
            *
            * \param key The Nth element in the hashmap to dereference.
            *
            * \return Returns the i element in the hashmap.
            */

          e_noinline const T operator[]( const K key )const{
            e_guardr( m_tLock );
            T outT{};
            const auto it = m_mMap.find( key );
            if( it != m_mMap.end() ){
              outT = it->second;
            }
            return outT;
          }

          /** \brief Copy assignment operator.
            *
            * This operator will copy assign the given hashmap to this hashmap.
            *
            * \param m The gfc::hashmap object to assign to this one.
            *
            * \return Returns a reference to this object.
            */

          e_forceinline hashmap& operator=( const hashmap& m ){
            e_guardw( m_tLock );
            if( this != &m ){
              m_mMap = m.m_mMap;
            }
            return *this;
          }

          /** \brief Move assignment operator.
            *
            * This operator will move assign the given hashmap to this hashmap.
            *
            * \param m The gfc::hashmap object to assign to this one.
            *
            * \return Returns a reference to this object.
            */

          e_forceinline hashmap& operator=( hashmap&& m ){
            e_guardw( m_tLock );
            if( this != &m ){
              m_mMap = std::move( m.m_mMap );
            }
            return *this;
          }

        //}:                                      |
        //Methods:{                               |
          //foreach*:{                            |
            //foreach?_pair:{                     |

#ifdef __APPLE__
  #pragma mark hashmap methods (foreach)
#endif

              e_noinline bool foreachsPair( const std::function<bool( const std::pair<K,T>& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      if( !lambda( a )){
                        break;
                      }
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreachPair( const std::function<void( const std::pair<K,T>& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      lambda( a );
                    }
                    return true;
                  }
                }
                return false;
              }

            //}:                                  |
            //foreach?_kv:{                       |

              e_noinline bool foreachsKV( const std::function<bool( const K, const T& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      if( !lambda( a.first, a.second )){
                        break;
                      }
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreachsKV( const std::function<bool( const K, T& )>& lambda ){
                if( lambda ){
                  e_guardw( m_tLock );
                  if( !empty() ){
                    for( auto& a : m_mMap ){
                      if( !lambda( a.first, a.second )){
                        break;
                      }
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreachKV( const std::function<void( const K, const T& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      lambda( a.first, a.second );
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreachKV( const std::function<void( const K, T& )>& lambda ){
                if( lambda ){
                  e_guardw( m_tLock );
                  if( !empty() ){
                    for( auto& a : m_mMap ){
                      lambda( a.first, a.second );
                    }
                    return true;
                  }
                }
                return false;
              }

            //}:                                  |
            //foreach?:{                          |

              e_noinline bool foreachs( const std::function<bool( const T& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      if( !lambda( a.second )){
                        break;
                      }
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreachs( const std::function<bool( T& )>& lambda ){
                if( lambda ){
                  e_guardw( m_tLock );
                  if( !empty() ){
                    for( auto& a : m_mMap ){
                      if( !lambda( a.second )){
                        break;
                      }
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreach( const std::function<void( const T& )>& lambda )const{
                if( lambda ){
                  e_guardr( m_tLock );
                  if( !empty() ){
                    for( const auto& a : m_mMap ){
                      lambda( a.second );
                    }
                    return true;
                  }
                }
                return false;
              }

              e_noinline bool foreach( const std::function<void( T& )>& lambda ){
                if( lambda ){
                  e_guardw( m_tLock );
                  if( !empty() ){
                    for( auto& a : m_mMap ){
                      lambda( a.second );
                    }
                    return true;
                  }
                }
                return false;
              }

            //}:                                  |
          //}:                                    |
          //alter:{                               |

#ifdef __APPLE__
  #pragma mark hashmap methods (alter/query)
#endif

            e_noinline bool alterKV( const K key, const std::function<void( const K key, T& value )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                auto it = m_mMap.find( key );
                if( it != m_mMap.end() ){
                  lambda( it->first, it->second );
                  return true;
                }
              }
              return false;
            }

            e_noinline bool alter( const K key, const std::function<void( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                auto it = m_mMap.find( key );
                if( it != m_mMap.end() ){
                  lambda( it->second );
                  return true;
                }
              }
              return false;
            }

          //}:                                        |
          //query:{                                   |

            e_noinline bool queryKV( const K key, const std::function<void( const K key, const T& value )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                auto it = m_mMap.find( key );
                if( it != m_mMap.end() ){
                  lambda( it->first, it->second );
                  return true;
                }
              }
              return false;
            }

            e_noinline bool query( const K key, const std::function<void( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                auto it = m_mMap.find( key );
                if( it != m_mMap.end() ){
                  lambda( it->second );
                  return true;
                }
              }
              return false;
            }

          //}:                                        |
          //empty:{                                   |

            e_forceinline bool empty()const{
              return m_mMap.empty();
            }

          //}:                                        |
          //erase:{                                   |

            e_noinline bool erase( const K key, const std::function<void( T& )>& lambda=nullptr ){
              if( find( key )){
                e_guardw( m_tLock );
                if( lambda ){
                  lambda( m_mMap[ key ]);
                }
                m_mMap.erase( key );
                return true;
              }
              return false;
            }

          //}:                                        |
          //clear:{                                   |

            e_forceinline void clear(){
              e_guardw( m_tLock );
              m_mMap.clear();
            }

          //}:                                        |
          //find:{                                    |

            e_noinline bool find( const K key, T& output )const{
              e_guardr( m_tLock );
              const auto it = m_mMap.find( key );
              if( it != m_mMap.end() ){
                output = it->second;
                return true;
              }
              return false;
            }

            e_noinline bool find( const K key )const{
              e_guardr( m_tLock );
              const auto it = m_mMap.find( key );
              return( it != m_mMap.end() );
            }

          //}:                                        |
          //size:{                                    |

            e_forceinline u32 size()const{
              return u32( m_mMap.size() );
            }

          //}:                                        |
          //set*:{                                    |

            /** \brief Set element via lambda.
              *
              * This routine will gain a reference to a new or existing element in
              * the map and pass it off to the lambda provided.
              *
              * \param key The key where we'd like to potentially allocate an item.
              * \param lambda The callback lambda that's invoked once we gain
              * access to the map element.
              *
              * \return Returns true if the item is new and false otherwise.
              */

            e_noinline bool setBy( const K key, const std::function<void( T& t )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                lambda( m_mMap[ key ]);
                return true;
              }
              return false;
            }

            /** \brief Set element on the map.
              *
              * This routine will assign the input value to a new or existing item
              * in the map.
              *
              * \param key The key of the element we're trying to change.
              * \param value The value we want to assign to that key.
              */

            e_forceinline void set( const K key, const T& value ){
              e_guardw( m_tLock );
              m_mMap[ key ] = value;
            }

            /** \brief Set element on the map.
              *
              * This routine will assign the input value to a new or existing item
              * in the map.
              *
              * \param key The key of the element we're trying to change.
              * \param rvalue The value we want to assign to that key.
              */

            e_forceinline void set( const K key, T&& rvalue ){
              e_guardw( m_tLock );
              m_mMap[ key ] = std::move( rvalue );
            }

          //}:                                        |
          //get*:{                                    |
            //getIterator:{                           |

              e_forceinline const_iterator getIterator()const{
                return const_iterator( *this );
              }

              e_forceinline iterator getIterator(){
                return iterator( *this );
              }

            //}:                                      |
            //get:{                                   |

              /** \brief Thread safe getter.
                *
                * This const getter will return a copy of the element indexed by
                * the given key. If the key doesn't exist then a std exception is
                * thrown.
                *
                * \param key The key into the map.
                *
                * \return Returns a const reference to the looked-up-by-key item.
                */

              e_noinline T get( const K key )const{
                e_guardr( m_tLock );
                T tOut;
                const auto it = m_mMap.find( key );
                if( it != m_mMap.end() ){
                  tOut = it->second;
                }
                return tOut;
              }

            //}:                                      |
          //}:                                        |
          //try*:{                                    |
            //tryForeach*:{                           |
              //tryForeach?_pair:{                    |

#ifdef __APPLE__
  #pragma mark hashmap methods (try)
#endif

                e_noinline s32 tryForeachsPair( const std::function<bool( const std::pair<K,T>& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    result = 0;
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        if( !lambda( a )){
                          break;
                        }
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeachPair( const std::function<void( const std::pair<K,T>& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        lambda( a );
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

              //}:                                    |
              //tryForeach?_kv:{                      |

                e_noinline s32 tryForeachsKV( const std::function<bool( const K, const T& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        if( !lambda( a.first, a.second )){
                          break;
                        }
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeachsKV( const std::function<bool( const K, T& )>& lambda ){
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryw( m_tLock ){
                    if( !empty() ){
                      for( auto& a : m_mMap ){
                        if( !lambda( a.first, a.second )){
                          break;
                        }
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeachKV( const std::function<void( const K, const T& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        lambda( a.first, a.second );
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeachKV( const std::function<void( const K, T& )>& lambda ){
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryw( m_tLock ){
                    if( !empty() ){
                      for( auto& a : m_mMap ){
                        lambda( a.first, a.second );
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

              //}:                                    |
              //tryForeach?:{                         |

                e_noinline s32 tryForeachs( const std::function<bool( const T& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        if( !lambda( a.second )){
                          break;
                        }
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeachs( const std::function<bool( T& )>& lambda ){
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryw( m_tLock ){
                    if( !empty() ){
                      for( auto& a : m_mMap ){
                        if( !lambda( a.second )){
                          break;
                        }
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeach( const std::function<void( const T& )>& lambda )const{
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryr( m_tLock ){
                    if( !empty() ){
                      for( const auto& a : m_mMap ){
                        lambda( a.second );
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

                e_noinline s32 tryForeach( const std::function<void( T& )>& lambda ){
                  if( !lambda ){
                    return 0;
                  }
                  s32 result = -1;
                  e_tryw( m_tLock ){
                    if( !empty() ){
                      for( auto& a : m_mMap ){
                        lambda( a.second );
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }
                  return result;
                }

              //}:                                    |
            //}:                                      |
            //tryAlter*:{                             |

              e_noinline s32 tryAlteringKV( const K key, const std::function<void( const K key, T& value )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  auto it = m_mMap.find( key );
                  if( it != m_mMap.end() ){
                    lambda( it->first, it->second );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              e_noinline bool tryAlter( const K key, const std::function<void( T& )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  auto it = m_mMap.find( key );
                  if( it != m_mMap.end() ){
                    lambda( it->second );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                      |
            //tryQuery*:{                             |

              e_noinline s32 tryQueryKV( const K key, const std::function<void( const K key, const T& value )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  auto it = m_mMap.find( key );
                  if( it != m_mMap.end() ){
                    lambda( it->first, it->second );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              e_noinline bool tryQuery( const K key, const std::function<void( const T& )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  auto it = m_mMap.find( key );
                  if( it != m_mMap.end() ){
                    lambda( it->second );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                      |
            //tryErase:{                              |

              e_noinline s32 tryErase( const K key, const std::function<void( T& )>& lambda=nullptr ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( find( key )){
                    if( lambda ){
                      lambda( m_mMap[ key ]);
                    }
                    m_mMap.erase( key );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                      |
            //tryClear:{                              |

              e_noinline s32 tryClear(){
                s32 result = -1;
                e_tryw( m_tLock ){
                  m_mMap.clear();
                  result = 1;
                }
                return result;
              }

            //}:                                      |
            //tryFind:{                               |

              e_noinline s32 tryFind( const K key, T& output )const{
                s32 result = -1;
                e_tryr( m_tLock ){
                  const auto it = m_mMap.find( key );
                  if( it != m_mMap.end() ){
                    output = it->second;
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              e_noinline s32 tryFind( const K key )const{
                s32 result = -1;
                e_tryr( m_tLock ){
                  if( m_mMap.find( key ) != m_mMap.end() ){
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                      |
            //trySet*:{                               |

              /** \brief Set element via lambda.
                *
                * This routine will gain a reference to a new or existing element
                * in the map and pass it off to the lambda provided.
                *
                * \param key The key where we'd like to potentially allocate an item.
                * \param lambda The callback lambda that's invoked once we gain
                * access to the map element.
                *
                * \return Returns 1 if the item is new, -1 if the spinlock couldn't
                * be acquired and 0 otherwise.
                */

              e_noinline s32 trySetBy( const K key, const std::function<void( T& t )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  const auto it = m_mMap.find( key );
                  if( it == m_mMap.end() ){
                    T& t = m_mMap[ key ];
                    lambda( t );
                    result = 1;
                  }else{
                    lambda( it->second );
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Set element on the map.
                *
                * This routine will assign the input value to a new or existing
                * item in the map.
                *
                * \param key The key of the element we're trying to change.
                * \param value The value we want to assign to that key.
                *
                * \return Returns 1 if the item is new, -1 if the spinlock couldn't
                * be acquired and 0 otherwise.
                */

              e_noinline s32 trySet( const K key, const T& value ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  const bool bNew=( m_mMap.find( key ) == m_mMap.end() );
                  m_mMap[ key ] = value;
                  result = bNew ? 1 : 0;
                }
                return result;
              }

            //}:                                      |
          //}:                                        |
        //}:                                          |
        //----------------------------------------+-----------------------------

        e_forceinline hashmap( const hashmap& m )
          : m_mMap( m.m_mMap )
        {}

        e_forceinline hashmap( hashmap&& m )
          : m_mMap( std::move( m.m_mMap ))
        {}

        hashmap() = default;
      ~ hashmap(){
          clear();
        }

      private:

        mutable atomic::ShareLockRecursive m_tLock;
        X m_mMap;
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//pool:{                                          |

#ifdef __APPLE__
  #pragma mark - pool -
#endif

  namespace EON{

    namespace gfc{

      /** \brief Wrapper for std::pool.
        *
        * This class defines the standard engine pool. It borrows from both the
        * array and vector classes, but uses a bitbucket to keep track of
        * allocations.
        */

      template<typename T, u64 N> struct E_PUBLISH pool final{

        //----------------------------------------+-----------------------------
        //Operate:{                               |

#ifdef __APPLE__
  #pragma mark pool operators
#endif

          /** \name Addition operators.
            *
            * These operators add pools together like they were math objects.
            *
            * @{
            */

          /** \brief Addition operator.
            *
            * This operator will add an element to the back of the pool as if
            * you had called push() on it.
            *
            * \param lvalue The object of type T to push onto the back of this
            * pool.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_forceinline u64 operator+=( const T& lvalue ){
            return set( lvalue );
          }

          /** \brief Addition operator.
            *
            * This operator will add an element to the back of the pool as if
            * you had called push() on it.
            *
            * \param rvalue The object of type T to push onto the back of this
            * pool.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_noinline u64 operator+=( T&& rvalue ){
            return set( std::move( rvalue ));
          }

          /** @}
            *
            * \next The pool assignment operators.
            *
            * These methods assign (copy) or move a pool [into] this one.
            *
            * @{
            */

          /** \brief Assignment operator.
            *
            * This routine will assign one gfc::pool to this one.
            *
            * \param lvalue The pool to assign to this one.
            *
            * \return Returns *this.
            */

          e_noinline pool& operator=( const pool& lvalue ){
            if( this != &lvalue ){
              clear();
              lvalue.foreach(
                [&]( const T& t ){
                  set( t );
                }
              );
            }
            return *this;
          }

          /** \brief Rvalue assignment operator.
            *
            * This routine will assign one gfc::pool to this one.
            *
            * \param rvalue The pool to assign to this one.
            *
            * \return Returns *this.
            */

          e_noinline pool& operator=( pool&& rvalue ){
            if( this != &rvalue ){
              m_tBits = std::move( rvalue.m_tBits );
              m_pData = rvalue.m_pData;
              rvalue.m_pData = nullptr;
              m_uSize = rvalue.m_uSize;
              rvalue.m_uSize = 0;
            }
            return *this;
          }

          /** @} */

        //}:                                      |
        //Methods:{                               |
          //capacity:{                            |

#ifdef __APPLE__
      #pragma mark pool methods
#endif

            constexpr static u64 capacity(){
              return N;
            }

          //}:                                    |
          //valid:{                               |

            e_noinline bool valid()const{
              bool bResult = true;
              e_guardr( m_tLock );
              // Verify head index.
              for( u64 i=0; i<m_tBits.bitcount(); ++i ){
                if( m_tBits[ i ]){
                  if( i != m_uHead ){
                    bResult = false;
                  }
                  break;
                }
              }
              // Verify tail index.
              for( s64 i=m_tBits.bitcount()-1; i>=0; --i ){
                if( m_tBits[ i ]){
                  if( i != m_uTail ){
                    bResult = false;
                  }
                  break;
                }
              }
              return bResult;
            }

          //}:                                    |
          //foreachs:{                            |

            /** \brief For each iterator (const).
              *
              * This routine will call the given function to every element in
              * the pool. This version of gfc::pool<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns.
              *
              * \param start The starting offset into the pool; not an IID.
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if the foreach was interrupted or false.
              */

            e_noinline bool foreachs(
                  const u64 start
                , const u64 count
                , const std::function<bool( const T& )>& lambda )const{
              if( start+count > m_tBits.bitcount() ){
                return false;
              }
              if( start > m_tBits.bitcount() ){
                return false;
              }
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardw( m_tLock );
              for( u64 i=start; i<start+count; ++i ){
                if( m_tBits[ i ]){
                  if( !lambda( m_pData[ i ])){
                    break;
                  }
                }
              }
              return true;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool. This version of gfc::pool<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns. Like iterators foreach() is perfectly thread safe.
              *
              * \param start Iteration begins at this starting index; not an
              * IID.
              * \param count The number of elements to walk over.
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreachs(
                  const u64 start
                , const u64 count
                , const std::function<bool( T& )>& lambda ){
              if( start+count > m_tBits.bitcount() ){
                return false;
              }
              if( start > m_tBits.bitcount() ){
                return false;
              }
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardw( m_tLock );
              for( u64 i=start; i<start+count; ++i ){
                if( m_tBits[ i ]){
                  if( !lambda( m_pData[ i ])){
                    break;
                  }
                }
              }
              return true;
            }

            /** \brief For each iterator (const).
              *
              * This routine will call the given function to every element in
              * the pool. This version of gfc::pool<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns.
              *
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreachs( const std::function<bool( const T& )>& lambda )const{
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardr( m_tLock );
              for( u64 i=m_uHead; i<=m_uTail; ++i ){
                if( m_tBits[ i ]){
                  if( !lambda( m_pData[ i ])){
                    break;
                  }
                }
              }
              return true;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool. This version of gfc::pool<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns. Like iterators foreach() is perfectly thread safe.
              *
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreachs( const std::function<bool( T& )>& lambda ){
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardw( m_tLock );
              for( u64 i=m_uHead; i<=m_uTail; ++i ){
                if( m_tBits[ i ]){
                  if( !lambda( m_pData[ i ])){
                    break;
                  }
                }
              }
              return true;
            }

          //}:                                    |
          //foreach:{                             |

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool.
              *
              * \param start The starting offset to start from; not an IID.
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreach( const u64 start
                  , const u64 count
                  , const std::function<void( const T& )>& lambda )const{
              if( start+count > m_tBits.bitcount() ){
                return false;
              }
              if( start > m_tBits.bitcount() ){
                return false;
              }
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardr( m_tLock );
              for( u64 i=start; i<=start+count; ++i ){
                if( m_tBits[ i ]){
                  lambda( m_pData[ i ]);
                }
              }
              return true;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool.
              *
              * \param start The starting offset into the pool; not an IID.
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreach( const u64 start
                  , const u64 count
                  , const std::function<void( T& )>& lambda ){
              if( start+count > m_tBits.bitcount() ){
                return false;
              }
              if( start > m_tBits.bitcount() ){
                return false;
              }
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardw( m_tLock );
              for( u64 i=start; i<=start+count; ++i ){
                if( m_tBits[ i ]){
                  lambda( m_pData[ i ]);
                }
              }
              return true;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool.
              *
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreach( const std::function<void( const T& )>& lambda )const{
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardr( m_tLock );
              for( u64 i=m_uHead; i<=m_uTail; ++i ){
                if( m_tBits[ i ]){
                  lambda( m_pData[ i ]);
                }
              }
              return true;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the pool.
              *
              * \param lambda The lambda to call for each element in the pool.
              *
              * \return Returns true if there were any elements in the pool and
              * false otherwise.
              */

            e_noinline bool foreach( const std::function<void( T& )>& lambda ){
              if( !lambda ){
                return false;
              }
              if( empty() ){
                return false;
              }
              e_sanity_check( valid() );
              e_guardw( m_tLock );
              for( u64 i=m_uHead; i<=m_uTail; ++i ){
                if( m_tBits[ i ]){
                  lambda( m_pData[ i ]);
                }
              }
              return true;
            }

          //}:                                    |
          //erase:{                               |

            /** \brief Try erasing element.
              *
              * This routine will erase an element via a lambda function.
              *
              * \param iid The item id in the pool.
              *
              * \param lambda The callback function to be invoked before the
              * object is freed from the pool.
              *
              * \return Returns -1 if the write lock couldn't be acquired, zero
              * if the iid didn't exist and 1 if the item was erased
              * successfully.
              */

            e_noinline s32 tryErase( u64 iid
                  , const std::function<void( T& )>& lambda ){
              s32 result = -1;
              if( iid ){
                --iid;
                e_tryw( m_tLock ){
                  if( m_tBits[ iid ]){
                    if( lambda ){
                        lambda( m_pData[ iid ]);
                    }
                    m_pData[ iid ].~T();
                    m_tBits.reset( iid );
                    collapseRange( iid );
                    result = 1;
                    --m_uSize;
                  }else{
                    result = 0;
                  }
                }
              }
              return result;
            }

            /** \brief Try erasing element.
              *
              * This routine will erase an element via a lambda function.
              *
              * \param iid The item id in the pool.
              *
              * \return Returns -1 if the write lock couldn't be acquired, zero
              * if the iid didn't exist and 1 if the item was erased
              * successfully.
              */

            e_noinline s32 tryErase( u64 iid ){
              s32 result = -1;
              if( iid ){
                --iid;
                e_tryw( m_tLock ){
                  if( m_tBits[ iid ]){
                    m_pData[ iid ].~T();
                    m_tBits.reset( iid );
                    collapseRange( iid );
                    result = 1;
                    --m_uSize;
                  }else{
                    result = 0;
                  }
                }
              }
              return result;
            }

            /** \brief Erase [remove] an element from the pool.
              *
              * This routine will delete an element in the pool. This is not a
              * fast operation. Use list<> instead if that's a problem.
              *
              * \param iid The index into the pool to delete.
              */

            e_forceinline bool erase( u64 iid ){
              bool bResult = false;
              if( m_uSize && iid ){ --iid;
                if( m_tBits[ iid ]){
                  e_guardw( m_tLock );
                  m_pData[ iid ].~T();
                  m_tBits.reset( iid );
                  collapseRange( iid );
                  bResult = true;
                  --m_uSize;
                }
              }
              return bResult;
            }

            /** \brief Erase [remove] an element from the pool.
              *
              * This routine will delete an element in the pool. This is not a
              * fast operation. Use list<> instead if that's a problem.
              *
              * \param t The object in the pool to delete.
              */

            e_noinline void erase( const T& t ){
              e_sanity_check(( &t >= m_pData )&&( &t <= m_pData + N ));
              erase( u64( &t - m_pData )+1 );
            }

          //}:                                    |
          //clear:{                                   |

            /** \brief Nuke all elements from orbit.
              *
              * This routine will obliterate all entries in the pool.
              */

            e_forceinline void clear(){
              if( m_uSize ){
                for( u64 i=0; i<N; ++i ){
                  erase( i+1 );
                }
              }
            }

          //}:                                    |
          //empty:{                               |

            /** \brief Check if the pool is empty.
             *
              * \return Like the std::pool this class wraps the empty() method
              * will return true if there are no elements in the pool and false
              * otherwise.
              */

            e_forceinline bool empty()const{
              return !m_uSize;
            }

          //}:                                    |
          //size:{                                |

            /** \brief Gets the number of elements in the pool.
              *
              * This routine will return the number of allocated elements in the
              * pool.
              *
              * \return Returns the number of entries in the pool.
              */

            e_forceinline u64 size()const{
              return m_uSize;
            }

          //}:                                    |
          //try*:{                                |
            //tryAlter:{                          |

              /** \brief Mutate an element in the pool.
                *
                * This routine will apply the given lambda to the pool at the
                * given element index. It's called tryAlter because it changes
                * or tryAlterings the element potentially in the lambda (it's
                * non-const).
                *
                * \param iid Index into the array whose element gets passed to
                * lambda.
                *
                * \param lambda The lambda function to apply the ith element in
                * the pool.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the pool was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryAlter( u64 iid
                    , const std::function<void( T& t )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                if( iid ){
                  --iid;
                  e_tryw( m_tLock ){
                    if( ! m_tBits[ iid ]){
                      result = 0;
                    }else{
                      lambda( m_pData[ iid ]);
                      result = 1;
                    }
                  }
                }
                return result;
              }

            //}:                                  |
            //tryQuery:{                          |

              /** \brief Query an element in the pool.
                *
                * This routine will run the given lambda on the pool at the
                * given element index i. This routine is perfectly thread safe
                * and because we pass the element to the lambda by const
                * reference we know you won't change it. Don't break the rules
                * and const_cast the value.  The method acquires a spinlock for
                * the lifetime of the lambda call so don't take too much time
                * or you'll potentially block other threads for a rather
                * significant amount of time. This is bad because spinlocks do
                * not yield the threads that block on them!
                *
                * The method is called "tryQuery" because we inspect an object
                * with it and do not change it.
                *
                * \param iid Index into the pool we want to dereference and
                * pass to the lambda function.
                *
                * \param lambda The lambda to apply the ith element in the pool.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the pool was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryQuery( u64 iid
                    , const std::function<void( const T& t )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                if( iid ){
                  --iid;
                  e_tryr( m_tLock ){
                    if( ! m_tBits[ iid ]){
                      result = 0;
                    }else{
                      lambda( *reinterpret_cast<const T*>( m_pData + iid ));
                      result = 1;
                    }
                  }
                }
                return result;
              }

            //}:                                  |
          //}:                                    |
          //alter:{                               |

            /** \brief Mutate an element in the pool.
              *
              * This routine will apply the given lambda to the pool at the
              * given element index. It's called alter because it changes or
              * alters the element potentially in the lambda (it's non-const).
              *
              * \param iid Index into the array whose element gets passed to
              * lambda.
              *
              * \param lambda The lambda function to apply the ith element in
              * the pool.
              *
              * \return Returns true if iid was in the range 0 to size()-1 or
              * false.
              */

            e_noinline bool alter( u64 iid
                  , const std::function<void( T& t )>& lambda ){
              bool bResult = false;
              if( iid && lambda ){
                --iid;
                e_guardw( m_tLock );
                if( m_tBits[ iid ]){
                  lambda( m_pData[ iid ]);
                  bResult = true;
                }
              }
              return bResult;
            }

          //}:                                        |
          //query:{                                   |

            /** \brief Query an element in the pool.
              *
              * This routine will run the given lambda on the pool at the given
              * element index i. This routine is perfectly thread safe and
              * because we pass the element to the lambda by const reference we
              * know you won't change it. Don't break the rules and const_cast
              * the value.  The method acquires a spinlock for the lifetime of
              * the lambda call so don't take too much time or you'll
              * potentially block other threads for a rather significant amount
              * of time. This is bad because spinlocks do not yield the threads
              * that block on them!
              *
              * The method is called "query" because we inspect an object with
              * it and do not change it.
              *
              * \param iid Index into the pool we want to dereference and pass
              * to the lambda function.
              *
              * \param lambda The lambda to apply the ith element in the pool.
              *
              * \return Returns true if iid was in the range 0 to size()-1 or
              * false.
              */

            e_noinline bool query( u64 iid
                  , const std::function<void( const T& t )>& lambda )const{
              bool bResult = false;
              if( iid && lambda ){
                --iid;
                e_guardr( m_tLock );
                if( m_tBits[ iid ]){
                  lambda( *reinterpret_cast<const T*>( m_pData + iid ));
                  bResult = true;
                }
              }
              return bResult;
            }

          //}:                                    |
          //find:{                                |

            e_forceinline bool find( const u64 iid )const{
              if( iid ){
                return m_tBits[ iid-1 ];
              }
              return false;
            }

          //}:                                    |
          //set:{                                 |

            /** \brief Try set by lambda.
              *
              * This routine will set the element via the lambda function
              * provided.
              */

            e_noinline s32 trySetBy( u64 iid
                  , const std::function<void( T& )>& lambda ){
              s32 result = -1;
              if( iid ){
                --iid;
                e_tryw( m_tLock ){
                  if( ! m_tBits[ iid ]){
                    m_tBits.set( iid );
                    ++m_uSize;
                    if( lambda ){
                      lambda( *new( m_pData + iid )T );
                    }else{
                      new( m_pData + iid )T;
                    }
                  }else if( lambda ){
                    lambda( m_pData[ iid ]);
                  }
                  expandRange( iid );
                  result = 1;
                }
              }
              return result;
            }

            /** \brief Get the data pointer.
              *
              * This routine will return the data pointer.
              */

            const T* data()const{
              return m_pData;
            }

            /** \brief Set element in the pool.
              *
              * This routine will set an element in the pool.
              *
              * \param iid The index at which you want to set the element.
              * \param lvalue The data to set the ith element to.
              */

            e_noinline bool set( u64 iid, const T& lvalue ){
              if( !iid ){
                return false;
              }
              --iid;
              e_guardw( m_tLock );
              if( m_tBits[ iid ]){
                m_pData[ iid ] = lvalue;
              }else{
                new( m_pData + iid )T( lvalue );
                m_tBits.set( iid );
                ++m_uSize;
              }
              expandRange( iid );
              return true;
            }

            /** \brief Set element in the pool.
              *
              * This routine will set an element in the pool.
              *
              * \param iid The index at which you want to set the element.
              * \param rvalue The data to set the ith element to.
              */

            e_noinline void set( u64 iid, T&& rvalue ){
              if( iid ){
                --iid;
                e_guardw( m_tLock );
                if( m_tBits[ iid ]){
                  m_pData[ iid ] = std::move( rvalue );
                }else{
                  m_tBits.set( iid );
                  ++m_uSize;
                  new( m_pData + iid )T( std::move( rvalue ));
                }
                expandRange( iid );
              }else{
                e_unreachable( "Zero IID!" );
              }
            }

            /** \brief Set element in the pool.
              *
              * This routine will set an element in the pool.
              *
              * \param lvalue The data to set the ith element to.
              *
              * \return Returns true if i was in the range 0 to size()-1 and
              * false otherwise.
              */

            e_noinline u64 set( const T& lvalue ){
              e_guardw( m_tLock );
              const u64 i = m_tBits.set();
              new( m_pData + i )T( lvalue );
              expandRange( i );
              ++ m_uSize;
              return i+1;
            }

            /** \brief Set element in the pool.
              *
              * This routine will set an element in the pool.
              *
              * \param rvalue The data to set the ith element to.
              *
              * \return Returns true if i was in the range 0 to size()-1 and
              * false otherwise.
              */

            e_noinline u64 set( T&& rvalue ){
              e_guardw( m_tLock );
              const u64 i = m_tBits.set();
              new( m_pData + i )T( std::move( rvalue ));
              expandRange( i );
              ++ m_uSize;
              return i+1;
            }

          //}:                                    |
        //}:                                      |
        //----------------------------------------+-----------------------------

        /** \brief Initializer list constructor.
          *
          * This constructor will build a pool from a variety of arrays.
          */

        e_noinline pool( const std::initializer_list<T>& ilist )
            : m_pData( reinterpret_cast<T*>( e_malloc( N * sizeof( T )))){
          e_sanity_check( ilist.size() < N );
          for( const T& t : ilist ){
            set( t );
          }
        }

        /** \brief Copy constructor.
          *
          * This constructor will duplicate the pool in this one.
          *
          * \param lvalue The pool to duplicate in this one.
          */

        e_noinline pool( const pool& lvalue )
            : m_pData( reinterpret_cast<T*>( e_malloc( N * sizeof( T ))))
            , m_tBits( lvalue.m_tBits ){
          lvalue.foreach(
            [this]( const T& t ){
              set( t );
            }
          );
        }

        /** \brief Move constructor.
          *
          * This routine will move the input pool into this one. After the move
          * the input is said to be owned by this one.
          *
          * \param rvalue The pool to move ownership.
          */

        e_noinline pool( pool&& rvalue )
            : m_tBits( std::move( rvalue.m_tBits ))
            , m_pData( rvalue.m_pData )
            , m_uSize( rvalue.m_uSize ){
          rvalue.m_pData = nullptr;
          rvalue.m_uSize = 0;
        }

        /** \brief Default constructor.
          *
          * This routine does nothing but apply defaults to member variables.
          */

        e_noinline pool()
          : m_pData( reinterpret_cast<T*>( e_malloc( N * sizeof( T ))))
        {}

        /** \brief Fixes a bug with std::pool.
          *
          * You cannot derive your own pools with std::pool but with the gfc
          * version you can. This virtual destructor allows proper inheritance
          * of pool containers.
          */

        e_noinline~pool(){
          clear();
          e_free( m_pData );
        }

      private:

        /* Private member variables */

        std::atomic<u64>           m_uSize = { 0ULL };
        std::atomic<u64>           m_uHead = {~0ULL };
        std::atomic<u64>           m_uTail = { 0ULL };
        bitbucket<N>               m_tBits;
        T*                         m_pData = nullptr;
        atomic::ShareLockRecursive m_tLock;

        /* Private methods */

        e_noinline void expandRange( const u64 iid ){
          m_uTail = e_max<u64>( m_uTail, iid );
          m_uHead = e_min<u64>( m_uHead, iid );
          e_sanity_check( valid() );
        }

        e_noinline void collapseRange( u64 iid ){
          // If uuid == head then walk forward to next set bit.
          if( iid == m_uHead ){
            const u64 n = m_tBits.bitcount();
            u64 i = iid;
            while( i < n-1 ){
              ++i;
              if( i >= m_uTail ){
                break;
              }
              if( m_tBits[ i ]){
                break;
              }
            }
            m_uHead = i;
          }
          // If uuid == tail then walk back to previous set bit.
          if( iid == m_uTail ){
            u64 i = iid;
            while( i > 0 ){
              --i;
              if( i <= m_uHead ){
                break;
              }
              if( m_tBits[ i ]){
                break;
              }
            }
            m_uTail = i;
          }
          e_sanity_check( valid() );
        }
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//================================================+=============================
//vector:{                                        |

#ifdef __APPLE__
  #pragma mark - vector -
#endif

  namespace EON{

    namespace gfc{

      /** \brief Wrapper for std::vector.
        *
        * This routine is a wrapper for the std::vector. It implements a simplified
        * interface and access to much more useable iterators. All methods are
        * thread safe unless otherwise stated.
        */

      template<typename T> struct E_PUBLISH vector{

        //----------------------------------------+-----------------------------
        //Structs:{                               |
          //const_iterator:{                      |

#ifdef __APPLE__
  #pragma mark vector::const_iterator
#endif

            /** \brief Const iterator object.
              *
              * This iterator simplifies using the normal
              * vector<>::const_iterator class. Simply get the iterator from
              * the gfc::vector container and test the iterator in a while
              * loop.
              *
              \code
                vector<s32>::const_iterator cit = myIntVector.getIterator();
                while( cit ){
                  ++cit;
                }
              \endcode
              *
              * But a much easier way of doing the same thing:
              \code
                myIntVector.foreach( []( const s32& i ){});
              \endcode
              */

            struct E_PUBLISH const_iterator final{

              friend struct vector;

              //----------------------------------+-----------------------------
              //Operate:{                         |

                e_forceinline const_iterator operator+( const s32 i )const{
                  const_iterator r( *this );
                  r.cit += i;
                  return r;
                }

                e_forceinline const_iterator operator-( const s32 i )const{
                  const_iterator r( *this );
                  r.cit -= i;
                  return r;
                }

                e_forceinline void operator+=( const s32 i ){
                  cit += i;
                }

                e_forceinline void operator-=( const s32 i ){
                  cit -= i;
                }

                /** \brief Assignment operator.
                  *
                  * This routine will assign one gfc::vector to this one.
                  *
                  * \param ci The vector to assign to this one.
                  *
                  * \return Returns *this.
                  */

                e_forceinline const_iterator& operator=(
                    const const_iterator& ci ){
                  if( this != &ci ){
                    m_pOuter = ci.m_pOuter;
                    cit = ci.cit;
                  }
                  return *this;
                }

                /** \brief Assignment operator.
                  *
                  * This routine will assign one gfc::vector to this one.
                  *
                  * \param rvalue The vector to assign to this one.
                  *
                  * \return Returns *this.
                  */

                e_forceinline const_iterator& operator=(
                    const_iterator&& rvalue ){
                  if( this != &rvalue ){
                    cit = std::move( rvalue.cit );
                    m_pOuter = rvalue.m_pOuter;
                    rvalue.m_pOuter = nullptr;
                  }
                  return *this;
                }

                /** \brief Pointer operator.
                  *
                  * This routine will return the current iterator value as a
                  * pointer.  It's a nice way of accessing the current value
                  * without assigning cit to a temporary first.
                  *
                  * \return Return a pointer to the current value.
                  */

                e_forceinline const T* operator->()const{
                  return &(*cit);
                }

                /** \brief Alive method.
                  *
                  * This routine checks whether the iterator is alive or not.
                  * It is called by the boolean operator below.
                  */

                e_forceinline operator bool()const{
                  return m_pOuter && ( cit != m_pOuter->m_vVector.end() );
                }

                /** \brief Return the current value.
                  *
                  * This routine will return the current value referenced by
                  * the iterator. This can be assigned to a temporary or cast.
                  *
                  * \return Returns the dereferenced inner iterator.
                  */

                e_forceinline const T& operator*()const{
                  return *cit;
                }

                /** \brief Pre-increment operator.
                  *
                  * This operator will increment the iterator to the next
                  * element in the array. Use the boolean operator to test if
                  * it has reached the end.
                  */

                e_forceinline const_iterator operator++(int){
                  return const_iterator( *m_pOuter, cit++ );
                }

                /** \brief Pre-increment operator.
                  *
                  * This operator will increment the iterator to the next
                  * element in the array. Use the boolean operator to test if
                  * cit has reached the end.
                  */

                e_forceinline const_iterator operator++(){
                  return const_iterator( *m_pOuter, ++cit );
                }

              //}:                                |
              //Methods:{                         |

                /** \brief Reset iterator to beginning.
                  *
                  * This routine will reset the iterator back to the beginning
                  * of the vector.
                  */

                e_forceinline void reset(){
                  cit = m_pOuter->m_vVector.begin();
                }

                e_forceinline void detach(){
                  m_pOuter = nullptr;
                }

                /** \brief Get iterator index.
                  *
                  * This routine will return the index of the iterator.
                  *
                  * \return Returns the index of the iterator by taking the
                  * distance between begin() and the current it. If it == end
                  * then ~0 is returned.
                  */

                e_forceinline s32 index()const{
                  return s32( std::distance( m_pOuter->m_vVector.begin(), cit ));
                }

              //}:                                    |
              //----------------------------------+-----------------------------

              /** \brief Copy constructor.
                *
                * This constructor will duplicate the vector iterator.
                *
                * \param ci The iterator to duplciate.
                */

              e_forceinline const_iterator( const const_iterator& ci )
                : cit( ci.cit )
                , m_tGuard( &ci.m_pOuter->m_tLock, true )
                , m_pOuter( ci.m_pOuter )
              {}

              /** \brief Move constructor.
                *
                * This constructor will move a vector into this one. After the move
                * ownership of the data is said to belong here.
                */

              e_forceinline const_iterator( const_iterator&& rvalue )
                  : cit( std::move( rvalue.cit ))
                  , m_tGuard( &rvalue.m_pOuter->m_tLock, true )
                  , m_pOuter( rvalue.m_pOuter ){
                rvalue.m_pOuter = nullptr;
              }

              /** \brief Destructor.
                *
                * All this destructor does is release the spinlock.
                */

              const_iterator() = default;
            ~ const_iterator() = default;

            private:

              e_forceinline const_iterator( const vector& outer, typename std::vector<T>::const_iterator& ci )
                : cit( ci )
                , m_tGuard( &outer.m_tLock, true )
                , m_pOuter( &outer )
              {}

              e_forceinline const_iterator( const vector& outer )
                : cit( outer.m_vVector.begin() )
                , m_tGuard( &outer.m_tLock, true )
                , m_pOuter( &outer )
              {}

              typename std::vector<T>::const_iterator cit;
              atomic::ShareLockRecursive::LockGuard m_tGuard;
              const vector* m_pOuter = nullptr;
            };

          //}:                                    |
          //iterator:{                            |

#ifdef __APPLE__
      #pragma mark vector::iterator
#endif

            /** \brief A std::iterator wrapper.
              *
              * This class represents a standard iterator object. It's a
              * wrapper class that's returned from gfc::vector<>::getIterator()
              * and is a fun easier way of walking over objects without using
              * begin() and end().
              */

            struct E_PUBLISH iterator final{

              friend struct vector;

              //----------------------------------+-----------------------------
              //Operate:{                         |

                e_forceinline iterator operator+( const s32 i )const{
                  iterator r( *this );
                  r.it += i;
                  return r;
                }

                e_forceinline iterator operator-( const s32 i )const{
                  iterator r( *this );
                  r.it -= i;
                  return r;
                }

                e_forceinline void operator+=( const s32 i ){
                  it += i;
                }

                e_forceinline void operator-=( const s32 i ){
                  it -= i;
                }

                /** \brief Assignment operator.
                  *
                  * This routine will assign one gfc::vector to this one.
                  *
                  * \param i The vector to assign to this one.
                  *
                  * \return Returns *this.
                  */

                e_forceinline iterator& operator=( const iterator& i ){
                  if( this != &i ){
                    m_pOuter = i.m_pOuter;
                    it = i.it;
                  }
                  return *this;
                }

                /** \brief Assignment operator.
                  *
                  * This routine will assign one gfc::vector to this one.
                  *
                  * \param i The vector to assign to this one.
                  *
                  * \return Returns *this.
                  */

                e_forceinline iterator& operator=( iterator&& i ){
                  if( this != &i ){
                    it = std::move( i.it );
                    m_pOuter = i.m_pOuter;
                    i.m_pOuter = nullptr;
                  }
                  return *this;
                }

                /** \brief Alive method.
                  *
                  * This routine checks whether the iterator is alive or not.
                  * It is called by the boolean operator below.
                  */

                e_forceinline operator bool()const{
                  return m_pOuter && ( it != m_pOuter->m_vVector.end() );
                }

                /** \brief Pre-increment operator.
                  *
                  * This operator will increment the iterator to the next
                  * element in the array. Use the boolean operator to test if
                  * it has reached the end.
                  */

                e_forceinline iterator operator++(int){
                  return iterator( *m_pOuter, it++ );
                }

                /** \brief Pre-increment operator.
                  *
                  * This operator will increment the iterator to the next
                  * element in the array. Use the boolean operator to test if
                  * cit has reached the end.
                  */

                e_forceinline iterator operator++(){
                  return iterator( *m_pOuter, ++it );
                }

                /** \brief Return the current value.
                  *
                  * This routine will return the current value referenced by
                  * the iterator. This can be assigned to a temporary or cast.
                  *
                  * \return Returns the dereferenced inner iterator.
                  */

                e_forceinline T& operator*(){
                  e_assert( !m_pOuter->empty() );
                  return *it;
                }

                /** \brief Pointer operator.
                  *
                  * This routine will return the current iterator value as a
                  * pointer.  It's a nice way of accessing the current value
                  * without assigning it to a temporary first.
                  *
                  * \return Return a pointer to the current value.
                  */

                e_forceinline T* operator->(){
                  T& t = *it;
                  return &t;
                }

              //}:                                |
              //Methods:{                         |

                /** \brief Reset iterator to beginning.
                  *
                  * This routine will reset the iterator back to the beginning
                  * of the vector.
                  */

                e_forceinline void reset(){
                  it = m_pOuter->m_vVector.begin();
                }

                /** \brief Erase element at iterator.
                  *
                  * This routine will nuke the object at the iterator's current
                  * position.  This is great for lists but DO NOT call it more
                  * than once in the same loop or you'll crash.
                  */

                e_noinline void erase(){
                  if( it != m_pOuter->m_vVector.end() ){
                    const u32 i = s32( std::distance( m_pOuter->m_vVector.begin(), it ));
                    if( i < m_pOuter->size() ){
                      m_pOuter->m_vVector.erase( it );
                      it = m_pOuter->m_vVector.begin()+i;
                    }else{
                      it = m_pOuter->m_vVector.end();
                    }
                  }
                }

                /** \brief Insert vector at iterator.
                  *
                  * This routine will insert a new vector at the location of
                  * the iterator.
                  */

                e_noinline void insertVector( const vector& v ){

                  //------------------------------------------------------------
                  // Append if iterator's at the end of the vector.
                  //------------------------------------------------------------

                  if( it == m_pOuter->m_vVector.end() ){
                    const u32 ix = m_pOuter->size();
                    m_pOuter->pushVector( v );
                    it = m_pOuter->m_vVector.begin() + ix;
                    return;
                  }

                  //------------------------------------------------------------
                  // Insert the vector at the insertion point it.
                  //------------------------------------------------------------

                  const u32 ix = s32( std::distance( m_pOuter->m_vVector.begin(), it ));
                  if( ix < m_pOuter->size() ){
                    e_guardr( v.m_tLock );
                    for( u32 n=v.size(), i=0; i<n; ++i ){
                      typename std::vector<T>::const_iterator it = m_pOuter->m_vVector.begin() + ix + i;
                      m_pOuter->m_vVector.insert( it, v.m_vVector[ i ]);
                    }
                  }
                  it = m_pOuter->m_vVector.begin() + ix;
                }

                /** \brief Insert element at iterator.
                  *
                  * This routine will insert a new element at the location of
                  * the iterator.
                  */

                e_noinline void insert( T&& t ){
                  if( it != m_pOuter->m_vVector.end() ){
                    const u32 i = u32( &*it-&*m_pOuter->m_vVector.begin() );
                    if( i < m_pOuter->size() ){
                      m_pOuter->m_vVector.insert( it, std::move( t ));
                      it = m_pOuter->m_vVector.begin()+i;
                      return;
                    }
                  }
                  const u32 i = m_pOuter->size();
                  m_pOuter->m_vVector.push_back( std::move( t ));
                  it = m_pOuter->m_vVector.begin()+i;
                }

                /** \brief Insert element at iterator.
                  *
                  * This routine will insert a new element at the location of
                  * the iterator.
                  */

                e_noinline void insert( const T& t ){
                  if( it != m_pOuter->m_vVector.end() ){
                    const u32 i = u32( &*it-&*m_pOuter->m_vVector.begin() );
                    if( i < m_pOuter->size() ){
                      m_pOuter->m_vVector.insert( it, t );
                      it = m_pOuter->m_vVector.begin()+i;
                      return;
                    }
                  }
                  const u32 i = m_pOuter->size();
                  m_pOuter->m_vVector.push_back( t );
                  it = m_pOuter->m_vVector.begin()+i;
                }

                e_forceinline void detach(){
                  m_pOuter = nullptr;
                }

                /** \brief Get iterator index.
                  *
                  * This routine will return the index of the iterator.
                  *
                  * \return Returns the index of the iterator by taking the
                  * distance between begin() and the current it. If it == end
                  * then ~0 is returned.
                  */

                e_forceinline s32 index()const{
                  return s32( std::distance( m_pOuter->m_vVector.begin(), it ));
                }

              //}:                                    |
              //--------------------------------------+-----------------------------

              e_forceinline iterator( const const_iterator& ci )
                : it( e_castIterator<vector>( ci, ci.m_pOuter->m_vVector ))
                , m_pOuter(  const_cast<vector*>( ci.m_pOuter ))
                , m_tGuard( &const_cast<vector*>( ci.m_pOuter )->m_tLock, false )
              {}

              e_forceinline iterator( const iterator& i )
                : it( i.it )
                , m_tGuard( &i.m_pOuter->m_tLock, false )
                , m_pOuter( i.m_pOuter )
              {}

              e_forceinline iterator( iterator&& rvalue )
                  : it( std::move( rvalue.it ))
                  , m_tGuard( &rvalue.m_pOuter->m_tLock, false )
                  , m_pOuter( rvalue.m_pOuter ){
                rvalue.m_pOuter = nullptr;
              }

            ~ iterator() = default;
              iterator() = default;

            private:

              e_forceinline iterator( vector& outer, const typename std::vector<T>::iterator& i )
                : it( i )
                , m_tGuard( &outer.m_tLock, false )
                , m_pOuter( &outer )
              {}

              e_forceinline iterator( vector& outer )
                : it( outer.m_vVector.begin() )
                , m_tGuard( &outer.m_tLock, false )
                , m_pOuter( &outer )
              {}

              typename std::vector<T>::iterator it;
              atomic::ShareLockRecursive::LockGuard m_tGuard;
              vector* m_pOuter = nullptr;
            };

          //}:                                    |
        //}:                                      |
        //Operate:{                               |

          /** \name Addition operators.
            *
            * These operators add vectors together like they were math objects.
            *
            * @{
            */

#ifdef __APPLE__
      #pragma mark vector operators
#endif

          /** \brief Addition operator.
            *
            * This operator will add an existing vector of the same type T to
            * this vector. It is a great way of merging vectors.
            *
            * \param cVector The vector to add to this one.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_forceinline vector& operator+=( const vector& cVector ){
            pushVector( cVector );
            return *this;
          }

          /** \brief Addition operator.
            *
            * This operator will add an existing vector of the same type T to
            * this vector. It is a great way of merging vectors.
            *
            * \param rvalue The vector to add to this one.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_noinline vector& operator+=( vector&& rvalue ){

            //------------------------------------------------------------------
            // If rvalue and this the same we cannot move! Just safely copy
            // across.
            //------------------------------------------------------------------

            e_guardw( m_tLock );
            if( this == &rvalue ){
              const u32 n = u32( m_vVector.size() );
              m_vVector.resize( n + n );
              for( u32 i=0; i<n; ++i ){
                m_vVector[ n+i ]=m_vVector[ i ];
              }
              return *this;
            }

            //------------------------------------------------------------------
            // We can safely and quickly move the rvalue to the end of this
            // vector.
            //------------------------------------------------------------------

            const u32 n = u32( rvalue.m_vVector.size() );
            if( n ){
              const u32 k = u32( m_vVector.size() );
              if( !k ){
                m_vVector = std::move( rvalue.m_vVector );
              }else{
                rvalue.query( 0, [&]( const T& t ){
                  m_vVector.resize( n + k );
                  if( std::is_pod<T>::value ){
                    memcpy( &m_vVector[ k ], &t, sizeof( T )*n );
                  }else{
                    for( u32 i=0; i<n; ++i ){
                      m_vVector[ k+i ]=rvalue.m_vVector[ i ];
                    }
                  }
                });
                rvalue.m_vVector.clear();
              }
            }
            return *this;
          }

          /** \brief Addition operator.
            *
            * This operator will add an element to the back of the vector as if
            * you had called push() on it.
            *
            * \param rvalue The object of type T to push onto the back of this
            * vector.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_noinline vector& operator+=( T&& rvalue ){
            e_guardw( m_tLock );
            m_vVector.push_back( std::move( rvalue ));
            return *this;
          }

          /** \brief Addition operator.
            *
            * This operator will add an element to the back of the vector as if
            * you had called push() on it.
            *
            * \param cT The object of type T to push onto the back of this
            * vector.
            *
            * \return Returns the dereferenced this pointer.
            */

          e_forceinline vector& operator+=( const T& cT ){
            push( cT );
            return *this;
          }

          /** @}
            *
            * \next The vector assignment operators.
            *
            * These methods assign (copy) or move a vector [into] this one.
            *
            * @{
            */

          /** \brief Assignment operator.
            *
            * This routine will assign one gfc::vector to this one.
            *
            * \param cVector The vector to assign to this one.
            *
            * \return Returns *this.
            */

          e_noinline vector& operator=( const vector& cVector ){
            e_guardr( cVector.m_tLock );
            e_guardw( m_tLock );
            if( this != &cVector ){
              m_vVector = cVector.m_vVector;
            }
            return *this;
          }

          /** \brief Rvalue assignment operator.
            *
            * This routine will assign one gfc::vector to this one.
            *
            * \param in The vector to assign to this one.
            *
            * \return Returns *this.
            */

          e_noinline vector& operator=( vector&& in ){
            e_guardr( in.m_tLock );
            e_guardw( m_tLock );
            if( this != &in ){
              m_vVector = std::move( in.m_vVector );
            }
            return *this;
          }

          /** @}
            *
            * \name Indexing operator.
            *
            * These operators index into the vector by zero-based index. Only
            * const versions of these operators exist that return the indexed
            * element by value for complete thread safety.
            *
            * @{
            */

          /** \brief Bracket operator.
            *
            * This routine will return the Nth element in the vector. This is
            * the const version of the operator. Please note that this method
            * is not thread safe. Only use it if you KNOW another thread isn't
            * going to reallocate the vector.
            *
            * \param e The Nth element in the array to dereference.
            *
            * \return Returns the i element in the vector.
            */

          template<typename E> e_noinline const T& operator[]( const E e )const{
            const u64 n = u64( m_vVector.size() );
            const u64 i = u64( e );
            e_guardr( m_tLock );
            if( i < n ){
              return m_vVector[ i ];
            }
            return m_vVector[ n-1 ];
          }

          /** @} */

        //}:                                      |
        //Methods:{                               |
          //getIterator:{                         |

#ifdef __APPLE__
  #pragma mark vector methods
#endif

            /** \brief Get constant iterator.
              *
              * \return This routine will return a constant iterator for the
              * vector.  With it one can easily walk over all elements.
              */

            e_forceinline const_iterator getIterator()const{
              return const_iterator( *this );
            }

            /** \brief Get iterator.
              *
              * \return This routine will return a constant iterator for the
              * vector.  With it one can easily walk over all elements.
              */

            e_forceinline iterator getIterator(){
              return iterator( *this );
            }

          //}:                                    |
          //foreachs:{                            |

            /** \brief For each iterator (const).
              *
              * This routine will call the given function to every element in
              * the vector. This version of gfc::vector<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns.
              *
              * \param start The starting offset into the vector.
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if the foreach was interrupted or false.
              */

            template<typename E> e_noinline bool foreachs(
                  const E start
                , const std::function<bool( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 s = u32( start );
                  for( u32 i=s; i<n; ++i ){
                    const T& t = m_vVector[ i ];
                    if( !lambda( t )){
                      break;
                    }
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector. This version of gfc::vector<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns. Like iterators foreach() is perfectly thread safe.
              *
              * \param start Iteration begins at this starting index.
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            template<typename E> e_noinline bool foreachs( const E start, const std::function<bool( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 s = u32( start );
                  for( u32 i=s; i<n; ++i ){
                    T& t = m_vVector[ i ];
                    if( !lambda( t )){
                      break;
                    }
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator (const).
              *
              * This routine will call the given function to every element in
              * the vector. This version of gfc::vector<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns.
              *
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            e_noinline bool foreachs( const std::function<bool( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  for( const T& a : m_vVector ){
                    if( !lambda( a )){
                      break;
                    }
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector. This version of gfc::vector<>::foreach() expects a
              * boolean to be returned by the lambda. If false is returned by
              * "lambda" then iteration will stop immediately and foreach()
              * returns. Like iterators foreach() is perfectly thread safe.
              *
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            e_noinline bool foreachs( const std::function<bool( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  for( T& a : m_vVector ){
                    if( !lambda( a )){
                      break;
                    }
                  }
                  return true;
                }
              }
              return false;
            }

          //}:                                    |
          //foreach:{                             |

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector.
              *
              * \param start The starting offset to start from.
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            template<typename E> e_noinline bool foreach( const E start, const std::function<void( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 s = u32( start );
                  for( u32 i=s; i<n; ++i ){
                    const T& t = m_vVector[ i ];
                    lambda( t );
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector.
              *
              * \param start The starting offset into the vector.
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            template<typename E> e_noinline bool foreach( const E start, const std::function<void( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 s = u32( start );
                  for( u32 i=s; i<n; ++i ){
                    T& t = m_vVector[ i ];
                    lambda( t );
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector.
              *
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            e_noinline bool foreach( const std::function<void( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  for( const T& a : m_vVector ){
                    lambda( a );
                  }
                  return true;
                }
              }
              return false;
            }

            /** \brief For each iterator.
              *
              * This routine will call the given function to every element in
              * the vector.
              *
              * \param lambda The lambda to call for each element in the
              * vector.
              *
              * \return Returns true if there were any elements in the vector
              * and false otherwise.
              */

            e_noinline bool foreach( const std::function<void( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  for( T& a : m_vVector ){
                    lambda( a );
                  }
                  return true;
                }
              }
              return false;
            }

          //}:                                    |
          //reserve:{                             |

            /** \brief Reserve N elements in the vector.
              *
              * This routine will reserve N elements in the vector. It's a pass
              * through.
              */

            template<typename E> e_forceinline void reserve( const E count ){
              e_guardw( m_tLock );
              m_vVector.reserve( size_t( count ));
            }

          //}:                                    |
          //resize:{                              |

            /** \brief Allocate N elements in the vector.
              *
              * This routine will allocate N elements in the vector. It is a
              * pass-through.
              */

            template<typename E> e_forceinline void resize( const E count ){
              e_guardw( m_tLock );
              m_vVector.resize( size_t( count ));
            }

          //}:                                    |
          //erase:{                               |

            /** \brief Erase [remove] an element from the vector.
              *
              * This routine will delete an element in the vector. This is not
              * a fast operation. Use list<> instead if that's a problem.
              *
              * \param index The index into the vector to delete.
              */

            template<typename E> e_forceinline bool erase( const E index ){
              e_guardw( m_tLock );
              const std::size_t i = std::size_t( index );
              if( i >= m_vVector.size() ){
                return false;
              }
              m_vVector.erase( m_vVector.begin() + i );
              return true;
            }

            /** \brief Erase [remove] an element from the vector.
              *
              * This routine will delete an element in the vector. This is not
              * a fast operation. Use list<> instead if that's a problem.
              *
              * \param t The object in the vector to delete.
              */

            e_noinline bool erase( const T& t ){
              e_guardw( m_tLock );
              const auto& it = std::find( m_vVector.begin(), m_vVector.end(), t );
              if( it != m_vVector.end() ){
                m_vVector.erase( it );
                return true;
              }
              return false;
            }

          //}:                                    |
          //clear:{                               |

            /** \brief Nuke all elements from orbit--it's the only way to be
              * sure.
              *
              * This routine will obliterate all entries in the vector.
              */

            e_forceinline void clear(){
              e_guardw( m_tLock );
              m_vVector.clear();
            }

          //}:                                    |
          //empty:{                               |

            /** \brief Check if the vector is empty.
              *
              * \return Like the std::vector this class wraps the empty()
              * method will return true if there are no elements in the vector
              * and false otherwise.
              */

            e_forceinline bool empty()const{
              return m_vVector.empty();
            }

          //}:                                    |
          //sort:{                                |

            /** \brief Sort vector using general lambda.
              *
              * This routine will sort the vector by calling the general <
              * operator.
              */

            e_forceinline bool sort(){
              e_guardw( m_tLock );
              if( !m_vVector.empty() ){
                std::sort( m_vVector.begin(), m_vVector.end() );
                return true;
              }
              return false;
            }

            /** \brief Sort vector with lambda.
              *
              * This routine will sort the vector by calling the lambda
              * provided. This just uses std::sort.
              *
              * \param lambda The lambda to use as a callback during sorting.
              * This is passed directly to std::sort.
              */

            e_noinline bool sort( const std::function<bool( const T&, const T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  std::sort( m_vVector.begin(), m_vVector.end(), lambda );
                  return true;
                }
              }
              return false;
            }

          //}:                                    |
          //find:{                                |

            e_noinline bool find( const T& t )const{
              e_guardr( m_tLock );
              const auto& it = std::find( std::begin( m_vVector ), std::end( m_vVector ), t );
              return( it != std::end( m_vVector ));
            }

          //}:                                    |
          //size:{                                |

            /** \brief Gets the number of elements in the vector.
              *
              * This routine will return the number of allocated elements in
              * the vector.
              *
              * \return Returns the number of entries in the vector.
              */

            e_forceinline u32 size()const{
              return u32( m_vVector.size() );
            }

          //}:                                    |
          //trim:{                                |

            /** \brief Trim N elements off the back of the vector.
              *
              * This routine will remove 'newcap' elements of the back of the
              * vector.
              *
              * \param enewcap The new capacity. The value must be LESS than
              * the current size of the vector otherwise a debug assertion will
              * fire.
              */

            template<typename E> e_noinline bool trim( const E enewcap ){
              e_guardw( m_tLock );
              const u32 newcap = u32( enewcap );
              const u32 curcap = u32( m_vVector.size() );
              if( newcap < curcap ){
                for( u32 n=curcap-newcap, i=0; i<n; ++i ){
                  m_vVector.pop_back();
                }
                return true;
              }
              return false;
            }

          //}:                                    |
          //unequal:{                             |

            /** \brief Inequality method.
              *
              * This routine will compare the ith element with the input t.
              *
              * \param e The index at which you want to compare.
              * \param t The T element to compare.
              *
              * \return Returns true if the two objects are equal or false.
              */

            template<typename E> e_noinline bool unequal( const E e, const T& t )const{
              e_guardr( m_tLock );
              const u32 i = u32( e );
              if( i < u32( m_vVector.size() )){
                return( m_vVector[ i ] != t );
              }
              return false;
            }

          //}:                                    |
          //equals:{                              |

            /** \brief Equality method.
              *
              * This routine will compare the ith element with the input t.
              *
              * \param e The index at which you want to compare.
              * \param t The T element to compare.
              *
              * \return Returns true if the two objects are equal or false.
              */

            template<typename E> e_noinline bool equals( const E e, const T& t )const{
              e_guardr( m_tLock );
              const u32 i = u32( e );
              if( i < u32( m_vVector.size() )){
                return( m_vVector[ i ]==t );
              }
              return false;
            }

          //}:                                    |
          //front*:{                              |

            e_noinline bool queryFront( const std::function<void( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  lambda( m_vVector.front() );
                  return true;
                }
              }
              return false;
            }

            e_noinline bool alterFront( const std::function<void( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  lambda( m_vVector.front() );
                  return true;
                }
              }
              return false;
            }

            /** \brief Get copy of the front object.
              *
              * This routine will return a copy of the front object. This
              * routine is absolutely NOT thread safe. Use query/alter if you
              * need it to block until other vector operations complete on
              * frontground threas.
              */

            e_noinline const T front()const{
              e_guardr( m_tLock );
              T outT{};
              if( !m_vVector.empty() ){
                outT = m_vVector.front();
              }
              return outT;
            }

          //}:                                    |
          //back*:{                               |

            e_noinline bool queryBack( const std::function<void( const T& )>& lambda )const{
              if( lambda ){
                e_guardr( m_tLock );
                if( !m_vVector.empty() ){
                  lambda( m_vVector.back() );
                  return true;
                }
              }
              return false;
            }

            e_noinline bool alterBack( const std::function<void( T& )>& lambda ){
              if( lambda ){
                e_guardw( m_tLock );
                if( !m_vVector.empty() ){
                  lambda( m_vVector.back() );
                  return true;
                }
              }
              return false;
            }

            /** \brief Get copy of the back object.
              *
              * This routine will return a copy of the back object. This
              * routine is absolutely NOT thread safe. Use query/alter if you
              * need it to block until other vector operations complete on
              * background threas.
              */

            e_noinline const T back()const{
              e_guardr( m_tLock );
              T outT{};
              if( !m_vVector.empty() ){
                outT = m_vVector.back();
              }
              return outT;
            }

          //}:                                    |
          //try*:{                                |
            //tryForeachs:{                       |

              /** \brief For each iterator (const).
                *
                * This routine will call the given function to every element in
                * the vector. This version of gfc::vector<>::foreach() expects
                * a boolean to be returned by the lambda. If false is returned
                * by "lambda" then iteration will stop immediately and
                * foreach() returns.
                *
                * \param start The starting offset into the vector.
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryForeachs(
                    const E start
                  , const std::function<bool( const T& )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    const u32 n = u32( m_vVector.size() );
                    const u32 s = u32( start );
                    for( u32 i=s; i<n; ++i ){
                      const T& t = m_vVector[ i ];
                      if( !lambda( t )){
                        break;
                      }
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector. This version of gfc::vector<>::foreach() expects
                * a boolean to be returned by the lambda. If false is returned
                * by "lambda" then iteration will stop immediately and
                * foreach() returns. Like iterators foreach() is perfectly
                * thread safe.
                *
                * \param start Iteration begins at this starting index.
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryForeachs( const E start, const std::function<bool( T& )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    const u32 n = u32( m_vVector.size() );
                    const u32 s = u32( start );
                    for( u32 i=s; i<n; ++i ){
                      T& t = m_vVector[ i ];
                      if( !lambda( t )){
                        break;
                      }
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator (const).
                *
                * This routine will call the given function to every element in
                * the vector. This version of gfc::vector<>::foreach() expects
                * a boolean to be returned by the lambda. If false is returned
                * by "lambda" then iteration will stop immediately and
                * foreach() returns.
                *
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryForeachs( const std::function<bool( const T& )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    for( const T& a : m_vVector ){
                      if( !lambda( a )){
                        break;
                      }
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector. This version of gfc::vector<>::foreach() expects
                * a boolean to be returned by the lambda. If false is returned
                * by "lambda" then iteration will stop immediately and
                * foreach() returns. Like iterators foreach() is perfectly
                * thread safe.
                *
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryForeachs( const std::function<bool( T& )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    for( T& a : m_vVector ){
                      if( !lambda( a )){
                        break;
                      }
                    }
                    result = 1;
                  }
                }
                return result;
              }

            //}:                                  |
            //tryForeach:{                        |

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector.
                *
                * \param start The starting offset to start from.
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryForeach( const E start, const std::function<void( const T& )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    const u32 n = u32( m_vVector.size() );
                    const u32 s = u32( start );
                    for( u32 i=s; i<n; ++i ){
                      const T& t = m_vVector[ i ];
                      lambda( t );
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector.
                *
                * \param start The starting offset into the vector.
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryForeach( const E start, const std::function<void( T& )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    const u32 n = u32( m_vVector.size() );
                    const u32 s = u32( start );
                    for( u32 i=s; i<n; ++i ){
                      T& t = m_vVector[ i ];
                      lambda( t );
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector.
                *
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryForeach( const std::function<void( const T& )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryr( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    for( const T& a : m_vVector ){
                      lambda( a );
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief For each iterator.
                *
                * This routine will call the given function to every element in
                * the vector.
                *
                * \param lambda The lambda to call for each element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryForeach( const std::function<void( T& )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( m_vVector.empty() ){
                    result = 0;
                  }else{
                    for( T& a : m_vVector ){
                      lambda( a );
                    }
                    result = 1;
                  }
                }
                return result;
              }

            //}:                                  |
            //tryAlter:{                          |

              /** \brief Mutate an element in the vector.
                *
                * This routine will apply the given lambda to the vector at the
                * given element index. It's called tryAlter because it changes
                * or tryAlterings the element potentially in the lambda (it's
                * non-const).
                *
                * \param e Index into the array whose element gets passed to
                * lambda.
                *
                * \param lambda The lambda function to apply the ith element in
                * the vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryAlter( const E e, const std::function<void( T& t )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  const u32 i = u32( e );
                  if( i < u32( m_vVector.size() )){
                    lambda( m_vVector[ i ]);
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                  |
            //tryQuery:{                          |

              /** \brief Query an element in the vector.
                *
                * This routine will run the given lambda on the vector at the
                * given element index i. This routine is perfectly thread safe
                * and because we pass the element to the lambda by const
                * reference we know you won't change it. Don't break the rules
                * and const_cast the value.  The method acquires a spinlock for
                * the lifetime of the lambda call so don't take too much time
                * or you'll potentially block other threads for a rather
                * significant amount of time. This is bad because spinlocks do
                * not yield the threads that block on them!
                *
                * The method is called "tryQuery" because we inspect an object
                * with it and do not change it.
                *
                * \param e Index into the vector we want to dereference and
                * pass to the lambda function.
                *
                * \param lambda The lambda to apply the ith element in the
                * vector.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryQuery( const E e, const std::function<void( const T& t )>& lambda )const{
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                const u32 i = u32( e );
                e_tryr( m_tLock ){
                  if( i < u32( m_vVector.size() )){
                    lambda( m_vVector[ i ]);
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                  |
            //tryInsert:{                         |

              /** \brief Insert a array of values.
                *
                * This routine will tryInsert a array of values into this
                * container.
                *
                * \param a The array to tryInsert at position i.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the array was empty or 1 if it was processed
                * correctly.
                */

              template<typename E,u32 N> e_noinline s32 tryInsertArray( const E e, const array<T,N>& a ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &a != this ){
                    const u32 n = u32( m_vVector.size() );
                    const u32 i = u32( e );
                    if( i == n ){
                      pushArray( a );
                    }else if( i < n ){
                      for( u32 n=a.size(), j=0; j<n; ++j ){
                        m_vVector.insert( m_vVector.begin()+i+j, a[ j ]);
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a array of values.
                *
                * This routine will tryInsert a array of values into this
                * container.
                *
                * \param rvalue The array to tryInsert at position i.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the array was empty or 1 if it was processed
                * correctly.
                */

              template<typename E,u32 N> e_noinline s32 tryInsertArray( const E e, array<T,N>&& rvalue ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &rvalue != this ){
                    const u32 n = u32( m_vVector.size() );
                    const u32 i = u32( e );
                    if( i == n ){
                      pushArray( std::move( rvalue ));
                      result = 1;
                    }else if( i < n ){
                      for( u32 n=rvalue.m_aArray.size(), j=0; j<n; ++j ){
                        m_vVector.insert( m_vVector.begin()+i+j, std::move( rvalue.m_aArray[ j ]));
                      }
                      rvalue.clear();
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a vector of values.
                *
                * This routine will tryInsert a vector of values into this
                * container.
                *
                * \param v The vector to tryInsert at position i.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryInsertVector( const E e, const vector<T>& v ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &v != this ){
                    const u32 n = u32( m_vVector.size() );
                    const u32 i = u32( e );
                    if( i == n ){
                      pushVector( v );
                    }else if( i < n ){
                      for( u32 n=u32( v.m_vVector.size() ), j=0; j<n; ++j ){
                        m_vVector.insert( m_vVector.begin()+i+j, v[ j ]);
                      }
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a vector of values.
                *
                * This routine will tryInsert a vector of values into this
                * container.
                *
                * \param rvalue The vector to tryInsert at position i.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryInsertVector( const E e, vector<T>&& rvalue ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &rvalue != this ){
                    const u32 n = u32( m_vVector.size() );
                    const u32 i = u32( e );
                    if( i == n ){
                      if( !n ){
                        m_vVector = std::move( rvalue.m_vVector );
                      }else{
                        pushVector( std::move( rvalue ));
                      }
                      result = 1;
                    }else if( i < n ){
                      for( u32 n=u32( rvalue.m_vVector.size() ), j=0; j<n; ++j ){
                        m_vVector.insert( m_vVector.begin()+i+j, std::move( rvalue.m_vVector[ j ]));
                      }
                      rvalue.m_vVector.clear();
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a new object at the given tryInsertion point.
                *
                * This routine will tryInsert an element at the given location.
                *
                * \param e The object to add.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryInsertBy( const E e, const std::function<void( T& t )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 i = u32( e );
                  if( i == n ){
                    m_vVector.push_back( T() );
                    lambda( m_vVector.back() );
                    result = 1;
                  }else if( i < n ){
                    m_vVector.insert( m_vVector.begin()+i, T() );
                    lambda( m_vVector[ i ]);
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a new object at the given tryInsertion point.
                *
                * This routine will tryInsert an element at the given location.
                *
                * \param t The object to add.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryInsert( const E e, const T& t ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 i = u32( e );
                  if( i == n ){
                    m_vVector.push_back( t );
                    result = 1;
                  }else if( i < n ){
                    m_vVector.insert( m_vVector.begin()+i, t );
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a new object at the given tryInsertion point.
                *
                * This routine will tryInsert an element at the given location.
                *
                * \param rvalue The object to add.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_noinline s32 tryInsert( const E e, T&& rvalue ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  const u32 n = u32( m_vVector.size() );
                  const u32 i = u32( e );
                  if( i == n ){
                    m_vVector.push_back( std::move( rvalue ));
                    result = 1;
                  }else if( i < n ){
                    m_vVector.insert( m_vVector.begin()+i, std::move( rvalue ));
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Insert a new object at the given tryInsertion point.
                *
                * This routine will tryInsert an element at the given location.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the vector was empty or 1 if it was processed
                * correctly.
                */

              template<typename E> e_forceinline s32 tryInsert( const E e ){
                return tryInsert( e, T() );
              }

            //}:                                  |
            //tryPush:{                           |

              /** \brief Push an array of values.
                *
                * This routine will push an array of values into this
                * container.
                *
                * \param a The array to push.
                *
                * \return Returns true if there was data pushed or false if the
                * rvalue is the same as this.
                */

              template<u32 N> e_noinline s32 tryPushArray( const array<T,N>& a ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  const bool bResult=( N > 0 );
                  if( bResult ){
                    a.foreach( [&]( T& t ){
                      m_vVector.push_back( t );
                    });
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Push an array of values.
                *
                * This routine will push an array of values into this
                * container.
                *
                * \param rvalue The array to push.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the array was empty or 1 if it was processed
                * correctly.
                */

              template<u32 N> e_noinline s32 tryPushArray( array<T,N>&& rvalue ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  const bool bResult=( N > 0 );
                  if( bResult ){
                    rvalue.foreach( [&]( T& t ){
                      m_vVector.push_back( std::move( t ));
                    });
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Push a vector of values.
                *
                * This routine will push a vector of values onto this
                * container.
                *
                * \param v The vector to push.
                *
                * \return Returns -1 if foreach couldn't acquire the spinlock,
                * zero if the array was empty or 1 if it was processed
                * correctly.
                */

              e_noinline s32 tryPushVector( const vector<T>& v ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &v != this ){
                    if( !v.empty() ){
                      v.foreach( [&]( const T& t ){
                        m_vVector.push_back( t );
                      });
                      result = 1;
                    }else{
                      result = 0;
                    }
                  }else if( !m_vVector.empty() ){
                    std::vector<T> temp( m_vVector );
                    for( const auto& t : temp ){
                      m_vVector.push_back( t );
                    }
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

              /** \brief Push a vector of values.
                *
                * This routine will push a vector of values onto this
                * container.
                *
                * \param rvalue The vector to push. This vector cannot be
                * 'this' or nothing will happen at all. The push will be
                * ignored because you cannot move a vector into itself!
                *
                * \return Returns true if there was data pushed or false if the
                * rvalue is the same as this.
                */

              e_noinline s32 tryPushVector( vector<T>&& rvalue ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( &rvalue != this ){
                    if( m_vVector.empty() ){
                      m_vVector = std::move( rvalue.m_vVector );
                    }else{
                      rvalue.foreach( [&]( T& t ){
                        m_vVector.push_back( std::move( t ));
                      });
                      rvalue.clear();
                    }
                    result = 1;
                  }
                }
                return result;
              }

              /** \brief Push a default value.
                *
                * This routine is not thread safe.
                *
                * \param lambda A lambda function to call in a thread safe way
                * with a reference to the newly pushed value for
                * initialization.
                *
                * \return Returns true if there was data pushed or false if the
                * &rvalue is the same as this.
                */

              e_noinline s32 tryPushBy( const std::function<void( T& t )>& lambda ){
                if( !lambda ){
                  return 0;
                }
                s32 result = -1;
                e_tryw( m_tLock ){
                  m_vVector.push_back( T() );
                  lambda( m_vVector.back() );
                  result = 1;
                }
                return result;
              }

              /** \brief Pushes a new value at the back of the vector.
                *
                * This method pushes the item and returns it's reference in the
                * std vector thereby it is utterly NOT thread safe and
                * shouldn't be used on vectors that are going to be read and
                * modified in background threads. It is provided for
                * convenience only. You have been warned.
                *
                * \param t The new value to push_back.
                *
                * \return Returns true if there was data pushed or false if the
                * &rvalue is the same as this.
                */

              e_noinline s32 tryPush( const T& t ){
                s32 result = -1;
                e_tryw( m_tLock ){
                  m_vVector.push_back( t );
                  result = 1;
                }
                return result;
              }

              /** \brief Pushes a new value at the back of the vector.
                *
                * This method pushes the item and returns it's reference in the
                * std vector thereby it is utterly NOT thread safe and
                * shouldn't be used on vectors that are going to be read and
                * modified in background threads. It is provided for
                * convenience only. You have been warned.
                *
                * \return Returns true if there was data pushed or false if the
                * &rvalue is the same as this.
                */

              e_noinline s32 tryPush(){
                s32 result = -1;
                e_tryw( m_tLock ){
                  m_vVector.push_back( T() );
                  result = 1;
                }
                return result;
              }

            //}:                                  |
            //tryPop:{                            |

              /** \brief Pop a value of the back of the vector.
                *
                * With this method and push() someone can simulate a stack
                * rather easily.  Removes the last element in the vector;
                * synonimous with pop_back().
                */

              e_noinline s32 tryPop(){
                s32 result = -1;
                e_tryw( m_tLock ){
                  if( !m_vVector.empty() ){
                    m_vVector.pop_back();
                    result = 1;
                  }else{
                    result = 0;
                  }
                }
                return result;
              }

            //}:                                  |
          //}:                                    |
          //insert:{                              |

            /** \brief Insert a vector of values.
              *
              * This routine will insert a vector of values into this
              * container.
              *
              * \param v The vector to insert at position i.
              *
              * \return Returns true if the vector was inserted or false.
              */

            template<typename E> e_noinline bool insertVector( const E e, const vector<T>& v ){
              if( &v != this ){
                e_guardw( m_tLock );
                const u32 n = u32( m_vVector.size() );
                const u32 i = u32( e );
                if( i == n ){
                  pushVector( v );
                  return true;
                }
                if( i < n ){
                  const u32 k = u32( v.m_vVector.size() );
                  if( k ){
                    for( u32 j=0; j<k; ++j ){
                      m_vVector.insert( m_vVector.begin()+i+j, v[ j ]);
                    }
                    return true;
                  }
                }
              }
              return false;
            }

            /** \brief Insert a vector of values.
              *
              * This routine will insert a vector of values into this
              * container.
              *
              * \param rvalue The vector to insert at position i.
              *
              * \return Returns true if the vector was inserted or false.
              */

            template<typename E> e_noinline bool insertVector( const E e, vector<T>&& rvalue ){
              if( &rvalue != this ){
                e_guardw( m_tLock );
                const u32 n = u32( m_vVector.size() );
                const u32 i = u32( e );
                if( i == n ){
                  pushVector( std::move( rvalue ));
                  return true;
                }
                if( i < n ){
                  const u32 k = u32( rvalue.m_vVector.size() );
                  if( k ){
                    if( !n ){
                      m_vVector = std::move( rvalue.m_vVector );
                    }else{
                      for( u32 j=0; j<k; ++j ){
                        m_vVector.insert( m_vVector.begin()+i+j, std::move( rvalue.m_vVector[ j ]));
                      }
                      rvalue.clear();
                    }
                    return true;
                  }
                }
              }
              return false;
            }

            /** \brief Insert a array of values.
              *
              * This routine will insert a array of values into this container.
              *
              * \param a The array to insert at position i.
              *
              * \return Returns true if the array was inserted or false.
              */

            template<typename E,u32 N> e_noinline bool insertArray( const E e, const array<T,N>& a ){
              if( &a != this ){
                e_guardw( m_tLock );
                const u32 n = u32( m_vVector.size() );
                const u32 i = u32( e );
                if( i == n ){
                  pushArray( a );
                  return true;
                }
                if( i < n ){
                  const u32 k = u32( a.m_aArray.size() );
                  if( k ){
                    for( u32 j=0; j<k; ++j ){
                      m_vVector.insert( m_vVector.begin()+i+j, a[ j ]);
                    }
                    return true;
                  }
                }
              }
              return false;
            }

            /** \brief Insert a array of values.
              *
              * This routine will insert a array of values into this container.
              *
              * \param rvalue The array to insert at position i.
              *
              * \return Returns true if the array was inserted or false.
              */

            template<typename E,u32 N> e_noinline bool insertArray( const E e, array<T,N>&& rvalue ){
              if( &rvalue != this ){
                e_guardw( m_tLock );
                const u32 n = u32( m_vVector.size() );
                const u32 i = u32( e );
                if( i == n ){
                  pushArray( std::move( rvalue ));
                  return true;
                }
                if( i < n ){
                  const u32 k = u32( rvalue.m_aArray.size() );
                  if( k ){
                    for( u32 j=0; j<k; ++j ){
                      m_vVector.insert( m_vVector.begin()+i+j, std::move( rvalue[ j ]));
                    }
                    rvalue.clear();
                    return true;
                  }
                }
              }
              return false;
            }

            /** \brief Insert a new object at the given insertion point.
              *
              * This routine will insert an element at the given location.
              *
              * \param e The index where to insert the object. A valid value is
              * between 0 <= size().
              *
              * \return Returns true if something was inserted or false.
              */

            template<typename E> e_noinline bool insertBy( const E e, const std::function<void( T& t )>& lambda ){
              if( !lambda ){
                return false;
              }
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              const u32 i = u32( e );
              if( i == n ){
                T t{};
                lambda( t );
                m_vVector.push_back( t );
                return true;
              }
              if( i < n ){
                T t{};
                lambda( t );
                m_vVector.insert( m_vVector.begin()+i, t );
                return true;
              }
              return false;
            }

            /** \brief Insert a new object at the given insertion point.
              *
              * This routine will insert an element at the given location.
              *
              * \param t The object to add.
              *
              * \return Returns true if something was inserted or false.
              */

            template<typename E> e_noinline bool insert( const E e, const T& t ){
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              const u32 i = u32( e );
              if( i == n ){
                m_vVector.push_back( t );
                return true;
              }
              if( i < n ){
                m_vVector.insert( m_vVector.begin()+i, t );
                return true;
              }
              return false;
            }

            /** \brief Insert a new object at the given insertion point.
              *
              * This routine will insert an element at the given location.
              *
              * \param e The index into the vector where to insert. May be any
              * value 0 <= size().
              *
              * \param rvalue The object to add.
              *
              * \return Returns true if something was inserted or false.
              */

            template<typename E> e_noinline bool insert( const E e, T&& rvalue ){
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              const u32 i = u32( e );
              if( i == n ){
                m_vVector.push_back( std::move( rvalue ));
                return true;
              }
              if( i < n ){
                m_vVector.insert( m_vVector.begin()+i, std::move( rvalue ));
                return true;
              }
              return false;
            }

            /** \brief Insert a new object at the given insertion point.
              *
              * This routine will insert an element at the given location.
              *
              * \return Returns true if something was inserted or false.
              */

            template<typename E> e_noinline bool insert( const E e ){
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              const u32 i = u32( e );
              if( i == n ){
                m_vVector.push_back( T() );
                return true;
              }
              if( i < n ){
                m_vVector.insert( m_vVector.begin()+i, T() );
                return true;
              }
              return false;
            }

          //}:                                    |
          //alter:{                               |

            /** \brief Mutate an element in the vector.
              *
              * This routine will apply the given lambda to the vector at the
              * given element index. It's called alter because it changes or
              * alters the element potentially in the lambda (it's non-const).
              *
              * \param e Index into the array whose element gets passed to
              * lambda.
              *
              * \param lambda The lambda function to apply the ith element in
              * the vector.
              *
              * \return Returns true if i was in the range 0 to size()-1 or
              * false.
              */

            template<typename E> e_noinline bool alter( const E e, const std::function<void( T& t )>& lambda ){
              if( !lambda ){
                return false;
              }
              e_guardw( m_tLock );
              const u32 i = u32( e );
              const u32 n = u32( m_vVector.size() );
              if( i >= n ){
                return false;
              }
              lambda( m_vVector[ i ]);
              return true;
            }

          //}:                                    |
          //query:{                               |

            /** \brief Query an element in the vector.
              *
              * This routine will run the given lambda on the vector at the
              * given element index i. This routine is perfectly thread safe
              * and because we pass the element to the lambda by const
              * reference we know you won't change it. Don't break the rules
              * and const_cast the value.  The method acquires a spinlock for
              * the lifetime of the lambda call so don't take too much time or
              * you'll potentially block other threads for a rather significant
              * amount of time. This is bad because spinlocks do not yield the
              * threads that block on them!
              *
              * The method is called "query" because we inspect an object with
              * it and do not change it.
              *
              * \param e Index into the vector we want to dereference and pass
              * to the lambda function.
              *
              * \param lambda The lambda to apply the ith element in the
              * vector.
              *
              * \return Returns true if i was in the range 0 to size()-1 or
              * false.
              */

            template<typename E> e_noinline bool query( const E e, const std::function<void( const T& t )>& lambda )const{
              if( !lambda ){
                return false;
              }
              e_guardr( m_tLock );
              const u32 i = u32( e );
              const u32 n = u32( m_vVector.size() );
              if( i >= n ){
                return false;
              }
              lambda( m_vVector[ i ]);
              return true;
            }

          //}:                                    |
          //push*:{                               |

            /** \brief Push an array of values.
              *
              * This routine will push an array of values into this container.
              *
              * \param a The array to push.
              */

            template<u32 N> e_noinline bool pushArray( const array<T,N>& a ){
              if( N > 0 ){
                e_guardw( m_tLock );
                a.foreach( [&]( T& t ){
                  m_vVector.push_back( t );
                });
                return true;
              }
              return false;
            }

            /** \brief Push an array of values.
              *
              * This routine will push an array of values into this container.
              *
              * \param rvalue The array to push.
              */

            template<u32 N> e_noinline bool pushArray( array<T,N>&& rvalue ){
              if( N > 0 ){
                e_guardw( m_tLock );
                rvalue.foreach( [&]( T& t ){
                  m_vVector.push_back( std::move( t ));
                });
                return true;
              }
              return false;
            }

            /** \brief Push a vector of values.
              *
              * This routine will push a vector of values onto this container.
              *
              * \param v The vector to push.
              */

            e_noinline bool pushVector( const vector<T>& v ){
              if( &v != this ){
                v.foreach( [&]( const T& t ){
                  e_guardw( m_tLock );
                  m_vVector.push_back( t );
                });
                return true;
              }
              if( v.empty() ){
                return false;
              }
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              m_vVector.resize( n + n );
              if( std::is_pod<T>::value ){
                memcpy( vp( &m_vVector[ n ]), vp( &m_vVector[ 0 ]), sizeof( T )*n );
              }else{
                for( u32 i=0; i<n; ++i ){
                  m_vVector[ n+i ] = m_vVector[ i ];
                }
              }
              return true;
            }

            /** \brief Push a vector of values.
              *
              * This routine will push a vector of values onto this container.
              *
              * \param rvalue The vector to push. This vector cannot be 'this'
              * or nothing will happen at all. The push will be ignored because
              * you cannot move a vector into itself!
              *
              * \return Returns true if there was data pushed or false if the
              * &rvalue is the same as this.
              */

            e_noinline bool pushVector( vector<T>&& rvalue ){
              if( &rvalue != this ){
                rvalue.foreach( [&]( T& t ){
                  e_guardw( m_tLock );
                  m_vVector.push_back( std::move( t ));
                });
                rvalue.clear();
                return true;
              }
              if( rvalue.empty() ){
                return false;
              }
              e_guardw( m_tLock );
              const u32 n = u32( m_vVector.size() );
              m_vVector.resize( n + n );
              if( std::is_pod<T>::value ){
                memcpy( vp( &m_vVector[ n ]), &m_vVector[ 0 ], sizeof( T )*n );
              }else{
                for( u32 i=0; i<n; ++i ){
                  m_vVector[ n+i ] = m_vVector[ i ];
                }
              }
              return true;
            }

            /** \brief Push a default value.
              *
              * This routine is not thread safe.
              *
              * \param lambda A lambda function to call in a thread safe way
              * with a reference to the newly pushed value for initialization.
              */

            e_noinline bool pushBy( const std::function<void( T& t )>& lambda ){
              if( !lambda ){
                return false;
              }
              T t{};
              lambda( t );
              e_guardw( m_tLock );
              m_vVector.push_back( t );
              return true;
            }

            /** \brief Pushes a new value at the back of the vector.
              *
              * This method pushes the item and returns it's reference in the
              * std vector thereby it is utterly NOT thread safe and shouldn't
              * be used on vectors that are going to be read and modified in
              * background threads. It is provided for convenience only. You
              * have been warned.
              *
              * \param t The new value to push_back.
              */

            e_forceinline void push( const T& t ){
              e_guardw( m_tLock );
              m_vVector.push_back( t );
            }

            /** \brief Pushes a new value at the back of the vector.
              *
              * This method pushes the item and returns it's reference in the
              * std vector thereby it is utterly NOT thread safe and shouldn't
              * be used on vectors that are going to be read and modified in
              * background threads. It is provided for convenience only. You
              * have been warned.
              *
              * \param rvalue The new value to push_back.
              */

            e_forceinline void push( T&& rvalue ){
              e_guardw( m_tLock );
              m_vVector.push_back( std::move( rvalue ));
            }

            /** \brief Pushes a new value at the back of the vector.
              *
              * This method pushes the item and returns it's reference in the
              * std vector thereby it is utterly NOT thread safe and shouldn't
              * be used on vectors that are going to be read and modified in
              * background threads. It is provided for convenience only. You
              * have been warned.
              */

            e_forceinline void push(){
              e_guardw( m_tLock );
              m_vVector.push_back( T() );
            }

          //}:                                    |
          //pop:{                                 |

            /** \brief Pop a value of the back of the vector.
              *
              * With this method and push() someone can simulate a stack rather
              * easily.  Removes the last element in the vector; synonimous
              * with pop_back().
              *
              * \return returns true if something was popped off the vector and
              * false otherwise.
              */

            e_forceinline bool pop(){
              e_guardw( m_tLock );
              if( m_vVector.empty() ){
                return false;
              }
              m_vVector.pop_back();
              return true;
            }

          //}:                                    |
          //set:{                                 |

            /** \brief Set element in the vector.
              *
              * This routine will set an element in the vector.
              *
              * \param e The index at which you want to set the element.
              * \param t The data to set the ith element to.
              */

            template<typename E> e_noinline bool set( const E e, const T& t ){
              const u32 i = u32( e );
              e_guardw( m_tLock );
              if( i >= m_vVector.size() ){
                return false;
              }
              m_vVector[ i ] = t;
              return true;
            }

            /** \brief Set element in the vector.
              *
              * This routine will set an element in the vector.
              *
              * \param e The index at which you want to set the element.
              * \param rvalue The data to set the ith element to.
              */

            template<typename E> e_noinline bool set( const E e, T&& rvalue ){
              const u32 i = u32( e );
              e_guardw( m_tLock );
              if( i >= m_vVector.size() ){
                return false;
              }
              m_vVector[ i ] = std::move( rvalue );
              return true;
            }

          //}:                                    |
          //lock:{                                |

            atomic::ShareLockRecursive& lock(){
              return m_tLock;
            }

          //}:                                    |
        //}:                                      |
        //----------------------------------------+-----------------------------

        /** \brief Reserve constructor.
          *
          * This routine will reserve so many elements on the new vector.
          */

        template<typename E> e_forceinline explicit vector( const E e ){
          resize( e );
        }

        /** \brief Initializer list constructor.
          *
          * This constructor will build a vector from a variety of arrays.
          */

        e_forceinline vector( std::initializer_list<T> v )
          : m_vVector( v )
        {}

        /** \brief Copy constructor.
          *
          * This constructor will duplicate the vector in this one.
          *
          * \param v The vector to duplicate in this one.
          */

        e_forceinline vector( const vector& v ){
          e_guardr( v.m_tLock );
          m_vVector = v.m_vVector;
        }

        /** \brief Move constructor.
          *
          * This routine will move the input vector into this one. After the
          * move the input is said to be owned by this one.
          *
          * \param v The vector to move ownership.
          */

        e_forceinline vector( vector&& v )
          : m_vVector( std::move( v.m_vVector ))
        {}

        /** \brief Default constructor.
          *
          * This routine does nothing but apply defaults to member variables.
          */

        vector() = default;

        /** \brief Fixes a bug with std::vector.
          *
          * You cannot derive your own vectors with std::vector but with the
          * gfc version you can. This virtual destructor allows proper
          * inheritance of vector containers.
          */

        virtual~vector(){
          clear();
        }

      private:

        std::vector<T> m_vVector;
        mutable atomic::ShareLockRecursive m_tLock;
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
namespace EON{
namespace gfc{
//                                                :
//================================================+=============================
//deque:{                                         |

#ifdef __APPLE__
  #pragma mark - deque -
#endif

  /** \brief Wrapper for std::deque.
    *
    * This routine is a wrapper for the std::deque. It implements a simplified
    * interface and access to much more useable iterators. All methods are
    * thread safe unless otherwise stated.
    */

  template<typename T> struct E_PUBLISH deque final{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |
      //const_iterator:{                          |

#ifdef __APPLE__
  #pragma mark deque::const_iterator
#endif

        /** \brief Const iterator object.
          *
          * This iterator simplifies using the normal deque<>::const_iterator
          * class. Simply get the iterator from the gfc::deque container and
          * test the iterator in a while loop.
          *
          \code
            deque<s32>::const_iterator cit = myIntDeque.getIterator();
            while( cit ){
              ++cit;
            }
          \endcode
          *
          * But a much easier way of doing the same thing:
          \code
            myIntDeque.foreach( []( const s32& i ){});
          \endcode
          */

        struct E_PUBLISH const_iterator final{

          friend struct deque;

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline const_iterator operator+( const s32 i )const{
              const_iterator r( *this );
              r.cit += i;
              return r;
            }

            e_forceinline const_iterator operator-( const s32 i )const{
              const_iterator r( *this );
              r.cit -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              cit += i;
            }

            e_forceinline void operator-=( const s32 i ){
              cit -= i;
            }

            /** \brief Assignment operator.
              *
              * This routine will assign one gfc::deque to this one.
              *
              * \param ci The deque to assign to this one.
              *
              * \return Returns *this.
              */

            e_forceinline const_iterator& operator=( const const_iterator& ci ){
              if( this != &ci ){
                m_pOuter = ci.m_pOuter;
                cit = ci.cit;
              }
              return *this;
            }

            /** \brief Assignment operator.
              *
              * This routine will assign one gfc::deque to this one.
              *
              * \param ci The deque to assign to this one.
              *
              * \return Returns *this.
              */

            e_forceinline const_iterator& operator=( const_iterator&& ci ){
              if( this != &ci ){
                cit = std::move( ci.cit );
                m_pOuter = ci.m_pOuter;
                ci.m_pOuter = nullptr;
              }
              return *this;
            }

            /** \brief Pointer operator.
              *
              * This routine will return the current iterator value as a
              * pointer.  It's a nice way of accessing the current value
              * without assigning cit to a temporary first.
              *
              * \return Return a pointer to the current value.
              */

            e_forceinline const T* operator->()const{
              return &(*cit);
            }

            /** \brief Alive method.
              *
              * This routine checks whether the iterator is alive or not. It is
              * called by the boolean operator below.
              */

            e_forceinline operator bool()const{
              return( m_pOuter && ( cit != m_pOuter->m_dDeque.end() ));
            }

            /** \brief Return the current value.
              *
              * This routine will return the current value referenced by the
              * iterator. This can be assigned to a temporary or cast.
              *
              * \return Returns the dereferenced inner iterator.
              */

            e_forceinline const T& operator*()const{
              return *cit;
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if it has
              * reached the end.
              */

            e_forceinline const_iterator operator++(int){
              return const_iterator( *m_pOuter, cit++ );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if cit has
              * reached the end.
              */

            e_forceinline const_iterator operator++(){
              return const_iterator( *m_pOuter, ++cit );
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Get container from iterator.
              *
              * This routine will return the container that owns this iterator.
              *
              * \return Returns a const reference to the outer container.
              */

            e_forceinline const deque* container()const{
              return m_pOuter;
            }

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the deque.
              */

            e_forceinline void reset(){
              cit = m_pOuter->m_dDeque.begin();
            }

            /** \brief Detatch iterator from deque.
              *
              * This routine will nullify the outer member pointer thereby
              * detaching the iterator from the container it references.
              */

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

            /** \brief Get iterator index.
              *
              * This routine will return the index of the iterator.
              *
              * \return Returns the index of the iterator by taking the
              * distance between begin() and the current it. If it == end then
              * ~0 is returned.
              */

            e_forceinline s32 index()const{
              return s32( std::distance( m_pOuter->m_dDeque.begin(), cit ));
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          /** \brief Copy constructor.
            *
            * This constructor will duplicate the deque iterator.
            *
            * \param ci The iterator to duplciate.
            */

          e_forceinline const_iterator( const const_iterator& ci )
            : cit( ci.cit )
            , m_tGuard( &ci.m_pOuter->m_tLock, true )
            , m_pOuter( ci.m_pOuter )
          {}

          /** \brief Move constructor.
            *
            * This constructor will move a deque into this one. After the move
            * ownership of the data is said to belong here.
            */

          e_forceinline const_iterator( const_iterator&& rvalue )
              : cit( std::move( rvalue.cit ))
              , m_tGuard( &rvalue.m_pOuter->m_tLock, true )
              , m_pOuter( rvalue.m_pOuter ){
            rvalue.m_pOuter = nullptr;
          }

          const_iterator() = default;
        ~ const_iterator() = default;

        private:

          e_forceinline const_iterator( const deque& outer, const typename std::deque<T>::const_iterator& i )
            : cit( i )
            , m_tGuard( &outer.m_tLock, true, false )
            , m_pOuter( &outer )
          {}

          e_forceinline const_iterator( const deque& outer )
            : cit( outer.m_dDeque.begin() )
            , m_tGuard( &outer.m_tLock, true )
            , m_pOuter( &outer )
          {}

          typename std::deque<T>::const_iterator cit;
          atomic::ShareLockRecursive::LockGuard m_tGuard;
          const deque* m_pOuter = nullptr;
        };

      //}:                                        |
      //iterator:{                                |

#ifdef __APPLE__
  #pragma mark deque::iterator
#endif

        /** \brief A std::iterator wrapper.
          *
          * This class represents a standard iterator object. It's a wrapper
          * class that's returned from gfc::deque<>::getIterator() and is a
          * fun easier way of walking over objects without using begin() and
          * end().
          */

        struct E_PUBLISH iterator final{

          friend struct deque;

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline iterator operator+( const s32 i )const{
              iterator r( *this );
              r.it += i;
              return r;
            }

            e_forceinline iterator operator-( const s32 i )const{
              iterator r( *this );
              r.it -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              it += i;
            }

            e_forceinline void operator-=( const s32 i ){
              it -= i;
            }

            /** \brief Assignment operator.
              *
              * This routine will assign one gfc::deque to this one.
              *
              * \param i The deque to assign to this one.
              *
              * \return Returns *this.
              */

            e_forceinline iterator& operator=( const iterator& i ){
              if( this != &i ){
                m_pOuter = i.m_pOuter;
                it = i.it;
              }
              return *this;
            }

            /** \brief Assignment operator.
              *
              * This routine will assign one gfc::deque to this one.
              *
              * \param i The deque to assign to this one.
              *
              * \return Returns *this.
              */

            e_forceinline iterator& operator=( iterator&& i ){
              if( this != &i ){
                it = std::move( i.it );
                m_pOuter = i.m_pOuter;
                i.m_pOuter = nullptr;
              }
              return *this;
            }

            /** \brief Alive method.
              *
              * This routine checks whether the iterator is alive or not. It is
              * called by the boolean operator below.
              */

            e_forceinline operator bool()const{
              return m_pOuter && ( it != m_pOuter->m_dDeque.end() );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if it has
              * reached the end.
              */

            e_forceinline iterator operator++(int){
              return iterator( *m_pOuter, it++ );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if cit has
              * reached the end.
              */

            e_forceinline iterator operator++(){
              return iterator( *m_pOuter, ++it );
            }

            /** \brief Return the current value.
              *
              * This routine will return the current value referenced by the
              * iterator. This can be assigned to a temporary or cast.
              *
              * \return Returns the dereferenced inner iterator.
              */

            e_forceinline const T& operator*()const{
              e_assert( !m_pOuter->empty() );
              return *it;
            }

            /** \brief Return the current value.
              *
              * This routine will return the current value referenced by the
              * iterator. This can be assigned to a temporary or cast.
              *
              * \return Returns the dereferenced inner iterator.
              */

            e_forceinline T& operator*(){
              e_assert( !m_pOuter->empty() );
              return *it;
            }

            /** \brief Pointer operator.
              *
              * This routine will return the current iterator value as a
              * pointer.  It's a nice way of accessing the current value
              * without assigning it to a temporary first.
              *
              * \return Return a pointer to the current value.
              */

            e_forceinline const T* operator->()const{
              const T& t = *it;
              return &t;
            }

            /** \brief Pointer operator.
              *
              * This routine will return the current iterator value as a
              * pointer.  It's a nice way of accessing the current value
              * without assigning it to a temporary first.
              *
              * \return Return a pointer to the current value.
              */

            e_forceinline T* operator->(){
              T& t = *it;
              return &t;
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Get container from iterator.
              *
              * This routine will return the container that owns this iterator.
              *
              * \return Returns a const reference to the outer container.
              */

            e_forceinline deque* container(){
              return m_pOuter;
            }

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the deque.
              */

            e_forceinline void reset(){
              it = m_pOuter->m_dDeque.begin();
            }

            /** \brief Erase element at iterator.
              *
              * This routine will nuke the object at the iterator's current
              * position.  This is great for lists but DO NOT call it more than
              * once in the same loop or you'll crash.
              */

            e_noinline void erase(){
              if( it != m_pOuter->m_dDeque.end() ){
                const u32 i = u32( &*it-&*m_pOuter->m_dDeque.begin() );
                if( i < m_pOuter->size() ){
                  m_pOuter->m_dDeque.erase( it );
                  it = m_pOuter->m_dDeque.begin()+i;
                }else{
                  it = m_pOuter->m_dDeque.end();
                }
              }
            }

            /** \brief Insert deque at iterator.
              *
              * This routine will insert a new vector at the location of the
              * iterator.
              */

            e_noinline void insertDeque( const deque& d ){

              //----------------------------------------------------------------
              // Append if iterator's at the end of the vector.
              //----------------------------------------------------------------

              if( it == m_pOuter->m_dDeque.end() ){
                const u32 ix = m_pOuter->size();
                m_pOuter->pushDeque( d );
                it = m_pOuter->m_dDeque.begin() + ix;
                return;
              }

              //----------------------------------------------------------------
              // Insert the vector at the insertion point it.
              //----------------------------------------------------------------

              const u32 ix = s32( std::distance( m_pOuter->m_dDeque.begin(), it ));
              if( ix < m_pOuter->size() ){
                e_guardr( d.m_tLock );
                for( u32 n=d.size(), i=0; i<n; ++i ){
                  typename std::vector<T>::const_iterator it = m_pOuter->m_dDeque.begin() + ix + i;
                  m_pOuter->m_dDeque.insert( it, d.m_dDeque[ i ]);
                }
              }
              it = m_pOuter->m_dDeque.begin() + ix;
            }

            /** \brief Insert element at iterator.
              *
              * This routine will insert a new element at the location of the
              * iterator.
              */

            e_noinline void insert( const T& t ){
              if( it != m_pOuter->m_dDeque.end() ){
                const u32 i = u32( &*it-&*m_pOuter->m_dDeque.begin() );
                if( i < m_pOuter->size() ){
                  m_pOuter->m_dDeque.insert( it, t );
                  it = m_pOuter->m_dDeque.begin()+i;
                  return;
                }
              }
              const u32 i = m_pOuter->size();
              m_pOuter->m_dDeque.push_back( t );
              it = m_pOuter->m_dDeque.begin()+i;
            }

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

            /** \brief Get iterator index.
              *
              * This routine will return the index of the iterator.
              *
              * \return Returns the index of the iterator by taking the
              * distance between begin() and the current it. If it == end then
              * ~0 is returned.
              */

            e_forceinline s32 index()const{
              return s32( std::distance( m_pOuter->m_dDeque.begin(), it ));
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          e_forceinline iterator( const const_iterator& ci )
            : it( e_castIterator<deque>( ci, ci.m_pOuter->m_dDeque ))
            , m_tGuard( &const_cast<deque*>( ci.m_pOuter )->m_tLock, false )
            , m_pOuter(  const_cast<deque*>( ci.m_pOuter ))
          {}

          e_forceinline iterator( const iterator& i )
            : it( i.it )
            , m_tGuard( &i.m_pOuter->m_tLock, false )
            , m_pOuter( i.m_pOuter )
          {}

          e_forceinline iterator( iterator&& rvalue )
              : it( std::move( rvalue.it ))
              , m_tGuard( &rvalue.m_pOuter->m_tLock, false )
              , m_pOuter( rvalue.m_pOuter ){
            rvalue.m_pOuter = nullptr;
          }

          iterator() = default;
        ~ iterator() = default;

        private:

          e_forceinline iterator( deque& outer, typename std::deque<T>::iterator& i )
            : it( i )
            , m_tGuard( &outer.m_tLock, false )
            , m_pOuter( &outer )
          {}

          e_forceinline iterator( deque& outer )
            : it( outer.m_dDeque.begin() )
            , m_tGuard( &outer.m_tLock, false )
            , m_pOuter( &outer )
          {}

          typename std::deque<T>::iterator it;
          atomic::ShareLockRecursive::LockGuard m_tGuard;
          deque* m_pOuter = nullptr;
        };

      //}:                                        |
    //}:                                          |
    //Operate:{                                   |

      /** \name Addition operators.
        *
        * These operators add deques together like they were math objects.
        *
        * @{
        */

      /** \brief Addition operator.
        *
        * This operator will add an existing deque of the same type T to this
        * deque. It is a great way of merging deques.
        *
        * \param cDeque The deque to add to this one.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline deque& operator+=( const deque& cDeque ){
        pushDeque( cDeque );
        return *this;
      }

      /** \brief Addition operator.
        *
        * This operator will add an existing deque of the same type T to this
        * deque. It is a great way of merging deques.
        *
        * \param rvalue The deque to add to this one.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_noinline deque& operator+=( deque&& rvalue ){

        //----------------------------------------------------------------------
        // If rvalue and this the same we cannot move! Just safely copy across.
        //----------------------------------------------------------------------

        e_guardw( m_tLock );
        if( this == &rvalue ){
          const u32 n = u32( m_dDeque.size() );
          m_dDeque.resize( n + n );
          for( u32 i=0; i<n; ++i ){
            m_dDeque[ n+i ]=m_dDeque[ i ];
          }
          return *this;
        }

        //----------------------------------------------------------------------
        // We can safely and quickly move the rvalue to the end of this deque.
        //----------------------------------------------------------------------

        rvalue.query( 0, [&]( const T& t ){
          const u32 n = u32( rvalue.m_dDeque.size() );
          const u32 k = u32( m_dDeque.size() );
          m_dDeque.resize( n + k );
          for( u32 i=0; i<n; ++i ){
            m_dDeque[ k+i ]=std::move( rvalue.m_dDeque[ i ]);
          }
        });
        rvalue.m_dDeque.clear();
        return *this;
      }

      /** \brief Addition operator.
        *
        * This operator will add an element to the back of the deque as if you
        * had called push() on it.
        *
        * \param rvalue The object of type T to push onto the back of this deque.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline deque& operator+=( T&& rvalue ){
        e_guardw( m_tLock );
        m_dDeque.push_back( std::move( rvalue ));
        return *this;
      }

      /** \brief Addition operator.
        *
        * This operator will add an element to the back of the deque as if you
        * had called push() on it.
        *
        * \param cT The object of type T to push onto the back of this deque.
        *
        * \return Returns the dereferenced this pointer.
        */

      e_forceinline deque& operator+=( const T& cT ){
        push( cT );
        return *this;
      }

      /** @}
        *
        * \next The deque assignment operators.
        *
        * These methods assign (copy) or move a deque [into] this one.
        *
        * @{
        */

      /** \brief Assignment operator.
        *
        * This routine will assign one gfc::deque to this one.
        *
        * \param cDeque The deque to assign to this one.
        *
        * \return Returns *this.
        */

      e_forceinline deque& operator=( const deque& cDeque ){
        e_guardw( m_tLock );
        if( this != &cDeque ){
          m_dDeque = cDeque.m_dDeque;
        }
        return *this;
      }

      /** \brief Rvalue assignment operator.
        *
        * This routine will assign one gfc::deque to this one.
        *
        * \param in The deque to assign to this one.
        *
        * \return Returns *this.
        */

      e_forceinline deque& operator=( deque&& in ){
        e_guardw( m_tLock );
        if( this != &in ){
          m_dDeque = std::move( in.m_dDeque );
        }
        return *this;
      }

      /** @}
        *
        * \name Indexing operator.
        *
        * These operators index into the deque by zero-based index. Only const
        * versions of these operators exist that return the indexed element by
        * value for complete thread safety.
        *
        * @{
        */

      /** \brief Bracket operator.
        *
        * This routine will return the Nth element in the deque. This is the
        * const version of the operator. Please note that this method is not
        * thread safe. Only use it if you KNOW another thread isn't going to
        * reallocate the deque.
        *
        * \param e The Nth element in the array to dereference.
        *
        * \return Returns the i element in the deque.
        */

      template<typename E> e_noinline const T operator[]( const E e )const{
        e_guardr( m_tLock );
        T outT{};
        const u32 i = u32( e );
        if( i < u32( m_dDeque.size() )){
          outT = m_dDeque[ i ];
        }
        return outT;
      }

      /** @} */

    //}:                                          |
    //Methods:{                                   |
      //getIterator:{                             |

#ifdef __APPLE__
  #pragma mark deque methods
#endif

        /** \brief Get constant iterator.
          *
          * \return This routine will return a constant iterator for the
          * deque.  With it one can easily walk over all elements.
          */

        e_forceinline const_iterator getIterator()const{
          return const_iterator( *this );
        }

        /** \brief Get iterator.
          *
          * \return This routine will return a constant iterator for the
          * deque.  With it one can easily walk over all elements.
          */

        e_forceinline iterator getIterator(){
          return iterator( *this );
        }

      //}:                                        |
      //binarySearch:{                            |

        /** \brief Perform a binary search on the deque.
          *
          * This routine will perform a binary search on this deque by calling
          * the lambda provided. It is assumed that the deque has been sorted
          * previously otherwise the results are not guaranteed to be in any
          * way sensible.
          *
          * \param d The deque to perform binary search on.
          *
          * \param lambda The comparison lambda that checks whether the pair of
          * objects are less than each other.
          *
          * \return Returns an iterator to the found object.
          */

        e_forceinline const_iterator e_binarySearch( const deque& d, const std::function<s32( const T& )>& lambda ){
          return d.binarySearch( lambda );
        }

        /** \brief Perform a binary search on the deque.
          *
          * This routine will perform a binary search on this deque by calling
          * the lambda provided. It is assumed that the deque has been sorted
          * previously otherwise the results are not guaranteed to be in any
          * way sensible.
          *
          * \param d The deque to perform binary search on.
          *
          * \param lambda The comparison lambda that checks whether the pair of
          * objects are less than each other.
          *
          * \return Returns an iterator to the found object.
          */

        e_forceinline iterator e_binarySearch( deque& d, const std::function<s32( const T& )>& lambda ){
          return d.binarySearch( lambda );
        }

        /** \brief Perform a binary search on the deque.
          *
          * This routine will perform a binary search on this deque by calling
          * the lambda provided. It is assumed that the deque has been sorted
          * previously otherwise the results are not guaranteed to be in any
          * way sensible.
          *
          * \param lambda The comparison lambda that checks whether the pair of
          * objects are less than each other.
          *
          * \return Returns an iterator to the found object.
          */

        e_noinline const_iterator binarySearch( const std::function<s32( const T& )>& lambda )const{
          e_guardr( m_tLock );
          const s32 n = size();
          if( n ){
            s32 h = n >> 1;
            if( h ){
              const_iterator bit = getIterator();
              const_iterator eit = bit+n-1;
              const_iterator hit = bit+h;
              for(;;){
                const s32 cmp = lambda( *hit );
                if( -1 == cmp && h ){
                  eit = hit;
                  hit = bit+( h >>= 1 );
                }else if( 1 == cmp && h ){
                  bit = hit;
                  hit = eit-( h >>= 1 );
                }else if( !cmp ){
                  return hit;
                }else{
                  break;
                }
              }
            }else{
              const_iterator cit = getIterator();
              if( !lambda( *cit )){
                return cit;
              }
            }
          }
          return const_iterator( *this, m_dDeque.end() );
        }

        /** \brief Perform a binary search on the deque.
          *
          * This routine will perform a binary search on this deque by calling
          * the lambda provided. It is assumed that the deque has been sorted
          * previously otherwise the results are not guaranteed to be in any
          * way sensible.
          *
          * \param lambda The comparison lambda that checks whether the pair of
          * objects are less than each other.
          *
          * \return Returns an iterator to the found object.
          */

        e_noinline iterator binarySearch( const std::function<s32( const T& )>& lambda ){
          e_guardr( m_tLock );
          const s32 n = size();
          if( n ){
            s32 h = n >> 1;
            if( h ){
              iterator bit = getIterator();
              iterator eit = bit+n-1;
              iterator hit = bit+h;
              for(;;){
                const s32 cmp = lambda( *hit );
                if( -1 == cmp && h ){
                  eit = hit;
                  hit = bit+( h >>= 1 );
                }else if( 1 == cmp && h ){
                  bit = hit;
                  hit = eit-( h >>= 1 );
                }else if( !cmp ){
                  return hit;
                }else{
                  break;
                }
              }
            }else{
              iterator it = getIterator();
              if( !lambda( *it )){
                return it;
              }
            }
          }
          return iterator( *this, m_dDeque.end() );
        }

      //}:                                        |
      //foreachs:{                                |

        /** \brief For each iterator (const).
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns.
          *
          * \param start The starting offset into the deque.
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if the foreach was interrupted or false.
          */

        template<typename E> friend e_forceinline bool e_foreachs( const deque& d, const E start, const std::function<bool( const T& )>& lambda ){
          return d.foreachs( start, lambda );
        }

        /** \brief For each iterator (const).
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns.
          *
          * \param start The starting offset into the deque.
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if the foreach was interrupted or false.
          */

        template<typename E> e_noinline bool foreachs( const E start, const std::function<bool( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          e_guardr( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          const u32 n = u32( m_dDeque.size() );
          const u32 s = u32( start );
          for( u32 i=s; i<n; ++i ){
            const T& t = m_dDeque[ i ];
            if( !lambda( t )){
              break;
            }
          }
          return true;
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns. Like
          * iterators foreach() is perfectly thread safe.
          *
          * \param d The deque to perform iteration on.
          *
          * \param start Iteration begins at this starting index.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> friend e_forceinline bool e_foreachs( deque& d, const E start, const std::function<bool( T& )>& lambda ){
          return d.foreachs( start, lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns. Like
          * iterators foreach() is perfectly thread safe.
          *
          * \param start Iteration begins at this starting index.
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> e_noinline bool foreachs( const E start, const std::function<bool( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          const u32 n = u32( m_dDeque.size() );
          const u32 s = u32( start );
          for( u32 i=s; i<n; ++i ){
            T& t = m_dDeque[ i ];
            if( !lambda( t )){
              break;
            }
          }
          return true;
        }

        /** \brief For each iterator (const).
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        friend e_forceinline bool e_foreachs( const deque& d, const std::function<bool( const T& )>& lambda ){
          return d.foreachs( lambda );
        }

        /** \brief For each iterator (const).
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_noinline bool foreachs( const std::function<bool( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          e_guardr( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          for( const T& a : m_dDeque ){
            if( !lambda( a )){
              break;
            }
          }
          return true;
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns. Like
          * iterators foreach() is perfectly thread safe.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        friend e_forceinline bool e_foreachs( deque& d, const std::function<bool( T& )>& lambda ){
          return d.foreachs( lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque. This version of gfc::deque<>::foreach() expects a boolean
          * to be returned by the lambda. If false is returned by "lambda" then
          * iteration will stop immediately and foreach() returns. Like
          * iterators foreach() is perfectly thread safe.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_noinline bool foreachs( const std::function<bool( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          for( T& a : m_dDeque ){
            if( !lambda( a )){
              break;
            }
          }
          return true;
        }

      //}:                                        |
      //foreach:{                                 |

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param d The deque to iterate over.
          *
          * \param start The starting offset to start from.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> friend e_forceinline bool e_foreach( const deque& d, const E start, const std::function<void( const T& )>& lambda ){
          return d.foreach( start, lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param start The starting offset to start from.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> e_noinline bool foreach( const E start, const std::function<void( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          e_guardr( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          const u32 n = u32( m_dDeque.size() );
          const u32 s = u32( start );
          for( u32 i=s; i<n; ++i ){
            const T& t = m_dDeque[ i ];
            lambda( t );
          }
          return true;
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param d The deque to iterate over.
          *
          * \param start The starting offset to start from.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> friend e_forceinline bool e_foreach( deque& d, const E start, const std::function<void( const T& )>& lambda ){
          return d.foreach( start, lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param start The starting offset into the deque.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        template<typename E> e_noinline bool foreach( const E start, const std::function<void( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          const u32 n = u32( m_dDeque.size() );
          const u32 s = u32( start );
          for( u32 i=s; i<n; ++i ){
            T& t = m_dDeque[ i ];
            lambda( t );
          }
          return true;
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param d The deque to iterate over.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_forceinline bool e_foreach( const deque& d, const std::function<void( const T& )>& lambda ){
          return d.foreach( lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_noinline bool foreach( const std::function<void( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          e_guardr( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          for( const T& a : m_dDeque ){
            lambda( a );
          }
          return true;
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param d The deque to iterate over.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_forceinline friend bool e_foreach( deque& d, const std::function<void( const T& )>& lambda ){
          return d.foreach( lambda );
        }

        /** \brief For each iterator.
          *
          * This routine will call the given function to every element in the
          * deque.
          *
          * \param lambda The lambda to call for each element in the deque.
          *
          * \return Returns true if there were any elements in the deque and
          * false otherwise.
          */

        e_noinline bool foreach( const std::function<void( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          for( T& a : m_dDeque ){
            lambda( a );
          }
          return true;
        }

      //}:                                        |
      //resize:{                                  |

        /** \brief Allocate N elements in the deque.
          *
          * This routine will allocate N elements in the deque. It is a
          * pass-through.
          */

        template<typename E> e_forceinline void resize( const E count ){
          e_guardw( m_tLock );
          m_dDeque.resize( std::size_t( count ));
        }

      //}:                                        |
      //alter/Query:{                             |

        /** \brief Mutate an element in the deque.
          *
          * This routine will apply the given lambda to the deque at the
          * given element index. It's called tryAlter because it changes
          * or tryAlterings the element potentially in the lambda (it's
          * non-const).
          *
          * \param e Index into the array whose element gets passed to
          * lambda.
          * \param lambda The lambda function to apply the ith element in the
          * vector.
          *
          * \return Returns -1 if foreach couldn't acquire the spinlock, zero
          * if the vector was empty or 1 if it was processed correctly.
          */

        template<typename E> e_noinline s32 tryAlter( const E e, const std::function<void( T& t )>& lambda ){
          if( !lambda ){
            return 0;
          }
          s32 result = -1;
          e_tryw( m_tLock ){
            const u32 i = u32( e );
            if( i < u32( m_dDeque.size() )){
              lambda( m_dDeque[ i ]);
              result = 1;
            }else{
              result = 0;
            }
          }
          return result;
        }

        /** \brief Query an element in the deque.
          *
          * This routine will run the given lambda on the deque at the given
          * element index i. This routine is perfectly thread safe and
          * because we pass the element to the lambda by const reference we
          * know you won't change it. Don't break the rules and const_cast
          * the value.  The method acquires a spinlock for the lifetime of
          * the lambda call so don't take too much time or you'll potentially
          * block other threads for a rather significant amount of time. This
          * is bad because spinlocks do not yield the threads that block on
          * them!
          *
          * The method is called "tryQuery" because we inspect an object
          * with it and do not change it.
          *
          * \param e Index into the vector we want to dereference and pass to
          * the lambda function.
          * \param lambda The lambda to apply the ith element in the vector.
          *
          * \return Returns -1 if foreach couldn't acquire the spinlock, zero
          * if the vector was empty or 1 if it was processed correctly.
          */

        template<typename E> e_noinline s32 tryQuery( const E e, const std::function<void( const T& t )>& lambda )const{
          if( !lambda ){
            return 0;
          }
          s32 result = -1;
          const u32 i = u32( e );
          e_tryr( m_tLock ){
            if( i < u32( m_dDeque.size() )){
              lambda( m_dDeque[ i ]);
              result = 1;
            }else{
              result = 0;
            }
          }
          return result;
        }

      //}:                                        |
      //erase:{                                   |

        /** \brief Erase [remove] an element from the deque.
          *
          * This routine will delete an element in the deque. This is not a
          * fast operation. Use list<> instead if that's a problem.
          *
          * \param index The index into the deque to delete.
          */

        template<typename E> e_forceinline bool erase( const E index ){
          e_guardw( m_tLock );
          const std::size_t i = std::size_t( index );
          if( i >= m_dDeque.size() ){
            return false;
          }
          m_dDeque.erase( m_dDeque.begin() + i );
          return true;
        }

        /** \brief Erase [remove] an element from the deque.
          *
          * This routine will delete an element in the deque. This is not a
          * fast operation. Use list<> instead if that's a problem.
          *
          * \param t The object in the deque to delete.
          */

        e_noinline bool erase( const T& t ){
          e_guardw( m_tLock );
          const auto& it = std::find( m_dDeque.begin(), m_dDeque.end(), t );
          if( it != m_dDeque.end() ){
            m_dDeque.erase( it );
            return true;
          }
          return false;
        }

      //}:                                        |
      //clear:{                                   |

        /** \brief Nuke all elements from orbit--it's the only way to be sure.
          *
          * This routine will obliterate all entries in the deque.
          */

        e_forceinline void clear(){
          e_guardw( m_tLock );
          m_dDeque.clear();
        }

      //}:                                        |
      //empty:{                                   |

        /** \brief Check if the deque is empty.
          *
          * \return Like the std::deque this class wraps the empty() method
          * will return true if there are no elements in the deque and false
          * otherwise.
          */

        e_forceinline bool empty()const{
          e_guardr( m_tLock );
          return m_dDeque.empty();
        }

      //}:                                        |
      //sort:{                                    |

        /** \brief Sort deque with lambda.
          *
          * This routine will sort the array by calling the lambda provided.
          * This just uses std::sort.
          *
          * \param lambda The lambda to use as a callback during sorting. This
          * is passed directly to std::sort.
          */

        e_forceinline friend bool e_sort( deque& d, const std::function<bool( const T&, const T& )>& lambda ){
          return d.sort( lambda );
        }

        /** \brief Sort deque with lambda.
          *
          * This routine will sort the array by calling the lambda provided.
          * This just uses std::sort.
          *
          * \param lambda The lambda to use as a callback during sorting. This
          * is passed directly to std::sort.
          */

        e_noinline bool sort( const std::function<bool( const T&, const T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          std::sort( m_dDeque.begin(), m_dDeque.end(), lambda );
          return true;
        }

      //}:                                        |
      //find:{                                    |

        e_forceinline friend bool e_find( const deque& d, const T& value ){
          return d.find( value );
        }

        e_noinline bool find( const T& value )const{
          e_guardr( m_tLock );
          const auto& cit = std::find( m_dDeque.begin(), m_dDeque.end(), value );
          return( m_dDeque.end() != cit );
        }

      //}:                                        |
      //size:{                                    |

        /** \brief Gets the number of elements in the deque.
          *
          * This routine will return the number of allocated elements in the
          * deque.
          *
          * \return Returns the number of entries in the deque.
          */

        e_forceinline u32 size()const{
          return u32( m_dDeque.size() );
        }

      //}:                                        |
      //trim:{                                    |

        /** \brief Trim N elements off the back of the deque.
          *
          * This routine will remove 'newcap' elements of the back of the
          * deque.
          *
          * \param enewcap The new capacity. The value must be LESS than the
          * current size of the deque otherwise a debug assertion will fire.
          */

        template<typename E> e_noinline bool trim( const E enewcap ){
          e_guardw( m_tLock );
          const u32 curcap = u32( m_dDeque.size() );
          const u32 newcap = u32( enewcap );
          if( newcap < curcap ){
            for( u32 n=curcap-newcap, i=0; i<n; ++i ){
              m_dDeque.pop_back();
            }
            return true;
          }
          return false;
        }

      //}:                                        |
      //set:{                                     |

        /** \brief Set element in the deque.
          *
          * This routine will set an element in the deque.
          *
          * \param e The index at which you want to set the element.
          * \param t The data to set the ith element to.
          */

        template<typename E> e_noinline void set( const E e, const T& t ){
          e_guardw( m_tLock );
          const auto i = u32( e );
          if( i >= u32( m_dDeque.size() )){
            e_unreachable( "Out of bounds!" );
          }
          m_dDeque[ i ] = t;
        }

        /** \brief Set element in the deque.
          *
          * This routine will set an element in the deque.
          *
          * \param e The index at which you want to set the element.
          * \param t The data to set the ith element to.
          */

        template<typename E> e_noinline void set( const E e, T&& t ){
          e_guardw( m_tLock );
          const auto i = u32( e );
          if( i >= u32( m_dDeque.size() )){
            e_unreachable( "Out of bounds!" );
          }
          m_dDeque[ i ] = std::move( t );
        }

      //}:                                        |
      //alter:{                                   |

        /** \brief Mutate an element in the deque.
          *
          * This routine will apply the given lambda to the deque at the given
          * element index. It's called alter because it changes or alters the
          * element potentially in the lambda (it's non-const).
          *
          * \param e Index into the array whose element gets passed to lambda.
          * \param lambda The lambda function to apply the ith element in the
          * deque.
          *
          * \return Returns true if i was in the range 0 to size()-1 or false.
          */

        template<typename E> e_noinline bool alter( const E e, const std::function<void( T& t )>& lambda ){
          if( !lambda ){
            return false;
          }
          const u32 i = u32( e );
          if( i < size() ){
            e_guardw( m_tLock );
            lambda( m_dDeque[ i ]);
            return true;
          }
          return false;
        }

      //}:                                        |
      //query:{                                   |

        /** \brief Query an element in the deque.
          *
          * This routine will run the given lambda on the deque at the given
          * element index i. This routine is perfectly thread safe and because
          * we pass the element to the lambda by const reference we know you
          * won't change it. Don't break the rules and const_cast the value.
          * The method acquires a spinlock for the lifetime of the lambda call
          * so don't take too much time or you'll potentially block other
          * threads for a rather significant amount of time. This is bad
          * because spinlocks do not yield the threads that block on them!
          *
          * The method is called "query" because we inspect an object with it
          * and do not change it.
          *
          * \param e Index into the deque we want to dereference and pass to
          * the lambda function.
          * \param lambda The lambda to apply the ith element in the deque.
          *
          * \return Returns true if i was in the range 0 to size()-1 or false.
          */

        template<typename E> e_noinline bool query( const E e, const std::function<void( const T& t )>& lambda )const{
          if( !lambda ){
            return false;
          }
          const u32 i = u32( e );
          if( i < size() ){
            T t{};
            { e_guardr( m_tLock );
              t = m_dDeque[ i ];
            }
            lambda( t );
            return true;
          }
          return false;
        }

      //}:                                        |
      //front:{                                   |

        e_noinline bool queryFront( const std::function<void( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          if( !empty() ){
            T t{};
            { e_guardr( m_tLock );
              t = m_dDeque.front();
            }
            lambda( t );
            return true;
          }
          return false;
        }

        e_noinline bool alterFront( const std::function<void( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( !m_dDeque.empty() ){
            lambda( m_dDeque.front() );
            return true;
          }
          return false;
        }

        /** \brief Get copy of the front object.
          *
          * This routine will return a copy of the front object. This routine is
          * absolutely NOT thread safe. Use query/alter if you need it to
          * block until other deque operations complete on frontground threas.
          */

        e_noinline const T front()const{
          e_guardr( m_tLock );
          T outT{};
          if( !m_dDeque.empty() ){
            outT = m_dDeque.front();
          }
          return outT;
        }

      //}:                                        |
      //back:{                                    |

        e_noinline bool queryBack( const std::function<void( const T& )>& lambda )const{
          if( !lambda ){
            return false;
          }
          if( !empty() ){
            T t{};
            { e_guardr( m_tLock );
              t = m_dDeque.back();
            }
            lambda( t );
            return true;
          }
          return false;
        }

        e_noinline bool alterBack( const std::function<void( T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          e_guardw( m_tLock );
          if( !m_dDeque.empty() ){
            lambda( m_dDeque.back() );
            return true;
          }
          return false;
        }

        /** \brief Get copy of the back object.
          *
          * This routine will return a copy of the back object. This routine is
          * absolutely NOT thread safe. Use query/alter if you need it to
          * block until other deque operations complete on background threas.
          */

        e_noinline const T back()const{
          e_guardr( m_tLock );
          T outT{};
          if( !m_dDeque.empty() ){
            outT = m_dDeque.back();
          }
          return outT;
        }

      //}:                                        |
      //unequal:{                                 |

        /** \brief Inequality method.
          *
          * This routine will compare the ith element with the input t.
          *
          * \param e The index at which you want to compare.
          * \param t The T element to compare.
          *
          * \return Returns true if the two objects are equal or false.
          */

        template<typename E> e_noinline bool unequal( const E e, const T& t )const{
          const u32 i = u32( e );
          if( i < size() ){
            e_guardr( m_tLock );
            return( m_dDeque[ i ] != t );
          }
          return false;
        }

      //}:                                        |
      //equals:{                                  |

        /** \brief Equality method.
          *
          * This routine will compare the ith element with the input t.
          *
          * \param e The index at which you want to compare.
          * \param t The T element to compare.
          *
          * \return Returns true if the two objects are equal or false.
          */

        template<typename E> e_noinline bool equals( const E e, const T& t )const{
          const u32 i = u32( e );
          if( i < size() ){
            e_guardr( m_tLock );
            return( m_dDeque[ i ]==t );
          }
          return false;
        }

      //}:                                        |
      //insert:{                                  |

        /** \brief Insert a deque of values.
          *
          * This routine will insert a deque of values into this container.
          *
          * \param v The deque to insert at position i.
          *
          * \return Returns true if the deque was inserted or false.
          */

        template<typename E> e_noinline bool insertDeque( const E e, const deque<T>& v ){
          if( &v != this ){
            const u32 i = u32( e );
            const u32 n = size();
            if( i >= n ){
              pushDeque( v );
              return true;
            }
            for( u32 j=0; j<n; ++j ){
              e_guardw( m_tLock );
              m_dDeque.insert( m_dDeque.begin()+i+j, v[ j ]);
            }
            return true;
          }
          return false;
        }

        /** \brief Insert a new object at the given insertion point.
          *
          * This routine will insert an element at the given location.
          *
          * \param e The object to add.
          */

        template<typename E> e_noinline bool insertBy( const E e, const std::function<void( T& t )>& lambda ){
          if( lambda ){
            e_guardw( m_tLock );
            m_dDeque.insert( m_dDeque.begin() + size_t( e ), T() );
            lambda( m_dDeque[ size_t( e )]);
            return true;
          }
          return false;
        }

        /** \brief Insert a new object at the given insertion point.
          *
          * This routine will insert an element at the given location.
          *
          * \param t The object to add.
          */

        template<typename E> e_forceinline void insert( const E e, const T& t ){
          e_guardw( m_tLock );
          m_dDeque.insert( m_dDeque.begin() + size_t( e ), t );
        }

        /** \brief Insert a new object at the given insertion point.
          *
          * This routine will insert an element at the given location.
          *
          * \param rvalue The object to add.
          */

        template<typename E> e_forceinline void insert( const E e, T& rvalue ){
          e_guardw( m_tLock );
          m_dDeque.insert( m_dDeque.begin() + size_t( e ), std::move( rvalue ));
        }

        /** \brief Insert a new object at the given insertion point.
          *
          * This routine will insert an element at the given location.
          *
          * \param e The object to add.
          */

        template<typename E> e_forceinline void insert( const E e ){
          e_guardw( m_tLock );
          m_dDeque.insert( m_dDeque.begin() + size_t( e ), T() );
        }

      //}:                                        |
      //pushf:{                                   |

        /** \brief Push a default value.
          *
          * This routine is not thread safe.
          *
          * \param lambda A lambda function to call in a thread safe way with a
          * reference to the newly pushfed value for initialization.
          */

        e_noinline bool pushfBy( const std::function<void( T& t )>& lambda ){
          if( !lambda ){
            return false;
          }
          T t{};
          lambda( t );
          e_guardw( m_tLock );
          m_dDeque.push_front( t );
          return true;
        }

        /** \brief Push an array of values.
          *
          * This routine will pushf an array of values into this container.
          *
          * \param v The vector to add to this deque.
          */

        e_noinline void pushfVector( const vector<T>& v ){
          v.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_front( t );
          });
        }

        /** \brief Push an array of values.
          *
          * This routine will pushf an array of values into this container.
          *
          * \param rvalue The vector to add to this deque.
          */

        e_noinline void pushfVector( vector<T>&& rvalue ){
          rvalue.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_front( std::move( t ));
          });
          rvalue.clear();
        }

        /** \brief Push an array of values.
          *
          * This routine will pushf an array of values into this container.
          *
          * \param a The array to prepend to this one.
          */

        template<u32 N> e_noinline void pushfArray( const array<T,N>& a ){
          a.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_front( t );
          });
        }

        /** \brief Push an array of values.
          *
          * This routine will pushf an array of values into this container.
          *
          * \param rvalue The array to prepend to this one.
          */

        template<u32 N> e_noinline void pushfArray( array<T,N>&& rvalue ){
          rvalue.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_front( std::move( t ));
          });
        }

        /** \brief Push a deque of values.
          *
          * This routine will pushf a deque of values onto this container.
          *
          * \param d The deque to prepend to this one.
          */

        e_noinline bool pushfDeque( const deque<T>& d ){
          if( &d != this ){
            const_iterator it = d.getIterator();
            while( it ){
              e_guardw( m_tLock );
              m_dDeque.push_front( it++ );
            }
            return true;
          }
          return false;
        }

        /** \brief Push a deque of values.
          *
          * This routine will pushf a deque of values onto this container.
          *
          * \param rvalue The deque to prepend to this one.
          */

        e_noinline bool pushfDeque( deque<T>&& rvalue ){
          if( &rvalue != this ){
            rvalue.foreach( [&]( T& t ){
              e_guardw( m_tLock );
              m_dDeque.push_front( std::move( t ));
            });
            rvalue.clear();
            return true;
          }
          return false;
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushfes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          *
          * \param t The new value to pushf_back.
          */

        e_forceinline void pushf( const T& t ){
          e_guardw( m_tLock );
          m_dDeque.push_front( t );
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushfes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          *
          * \param rvalue The new value to pushf_back.
          */

        e_forceinline void pushf( T&& rvalue ){
          e_guardw( m_tLock );
          m_dDeque.push_front( std::move( rvalue ));
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushfes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          */

        e_forceinline void pushf(){
          e_guardw( m_tLock );
          m_dDeque.push_front( T() );
        }

      //}:                                        |
      //push:{                                    |

        /** \brief Push a default value.
          *
          * This routine is not thread safe.
          *
          * \param lambda A lambda function to call in a thread safe way with a
          * reference to the newly pushed value for initialization.
          */

        e_noinline bool pushBy( const std::function<void( T& t )>& lambda ){
          if( !lambda ){
            return false;
          }
          T t{};
          lambda( t );
          e_guardw( m_tLock );
          m_dDeque.push_back( t );
          return true;
        }

        /** \brief Push an array of values.
          *
          * This routine will push an array of values into this container.
          *
          * \param v The vector to add to this deque.
          */

        template<u32 N> e_noinline void pushVector( const vector<T>& v ){
          v.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_back( t );
          });
        }

        /** \brief Push an array of values.
          *
          * This routine will push an array of values into this container.
          *
          * \param rvalue The vector to add to this deque.
          */

        template<u32 N> e_noinline void pushVector( vector<T>&& rvalue ){
          rvalue.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_back( std::move( t ));
          });
          rvalue.clear();
        }

        /** \brief Push an array of values.
          *
          * This routine will push an array of values into this container.
          *
          * \param a The array to prepend to this one.
          */

        template<u32 N> e_noinline void pushArray( const array<T,N>& a ){
          a.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_back( t );
          });
        }

        /** \brief Push an array of values.
          *
          * This routine will push an array of values into this container.
          *
          * \param rvalue The array to prepend to this one.
          */

        template<u32 N> e_noinline void pushArray( array<T,N>&& rvalue ){
          rvalue.foreach( [&]( const T& t ){
            e_guardw( m_tLock );
            m_dDeque.push_back( std::move( t ));
          });
        }

        /** \brief Push a deque of values.
          *
          * This routine will push a deque of values onto this container.
          *
          * \param d The deque to prepend to this one.
          */

        e_noinline bool pushDeque( const deque<T>& d ){
          if( &d != this ){
            const_iterator it = d.getIterator();
            while( it ){
              e_guardw( m_tLock );
              m_dDeque.push_back( it++ );
            }
            return true;
          }
          return false;
        }

        /** \brief Push a deque of values.
          *
          * This routine will push a deque of values onto this container.
          *
          * \param rvalue The deque to prepend to this one.
          */

        e_noinline bool pushDeque( deque<T>&& rvalue ){
          if( &rvalue != this ){
            rvalue.foreach( [&]( T& t ){
              e_guardw( m_tLock );
              m_dDeque.push_back( std::move( t ));
            });
            rvalue.clear();
            return true;
          }
          return false;
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          *
          * \param t The new value to push_back.
          */

        e_forceinline void push( const T& t ){
          e_guardw( m_tLock );
          m_dDeque.push_back( t );
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          *
          * \param rvalue The new value to push_back.
          */

        e_forceinline void push( T&& rvalue ){
          e_guardw( m_tLock );
          m_dDeque.push_back( std::move( rvalue ));
        }

        /** \brief Pushes a new value at the back of the deque.
          *
          * This method pushes the item and returns it's reference in the std
          * deque thereby it is utterly NOT thread safe and shouldn't be used
          * on deques that are going to be read and modified in background
          * threads. It is provided for convenience only. You have been warned.
          */

        e_forceinline void push(){
          e_guardw( m_tLock );
          m_dDeque.push_back( T() );
        }

      //}:                                        |
      //popf:{                                    |

        /** \brief Pop a value of the front of the deque.
          *
          * With this method and push() someone can simulate a queue rather
          * easily.  Removes the last element in the deque; synonimous with
          * pop_front().
          */

        e_forceinline bool popf(){
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          m_dDeque.pop_front();
          return true;
        }

        /** \brief Pop a value of the front of the deque.
          *
          * With this method and push() someone can simulate a queue rather
          * easily.  Removes the last element in the deque; synonimous with
          * pop_front().
          */

        e_noinline bool popfBy( const std::function<void( const T& )>& lambda ){
          if( !lambda ){
            return false;
          }
          T t{};
          { e_guardw( m_tLock );
            if( m_dDeque.empty() ){
              return false;
            }
            t = m_dDeque.front();
            m_dDeque.pop_front();
          }
          lambda( t );
          return true;
        }

      //}:                                        |
      //pop:{                                     |

        /** \brief Pop a value of the back of the deque.
          *
          * With this method and push() someone can simulate a stack rather
          * easily.  Removes the last element in the deque; synonimous with
          * pop_back().
          */

        e_noinline bool pop(){
          e_guardw( m_tLock );
          if( m_dDeque.empty() ){
            return false;
          }
          m_dDeque.pop_back();
          return true;
        }

      //}:                                        |
    //}:                                          |
    //--------------------------------------------+-----------------------------

    /** \brief Initializer list constructor.
      *
      * This constructor will build a deque from a variety of arrays.
      */

    e_forceinline deque( const std::initializer_list<T>& d )
      : m_dDeque( d )
    {}

    /** \brief Copy constructor.
      *
      * This constructor will duplicate the deque in this one.
      *
      * \param d The deque to duplicate in this one.
      */

    e_forceinline deque( const deque& d ){
      e_guardr( d.m_tLock );
      m_dDeque = d.m_dDeque;
    }

    /** \brief Move constructor.
      *
      * This routine will move the input deque into this one. After the move
      * the input is said to be owned by this one.
      *
      * \param d The deque to move ownership.
      */

    e_forceinline deque( deque&& d ){
      e_guardr( d.m_tLock );
      m_dDeque = std::move( d.m_dDeque );
    }

    /** \brief Default constructor.
      *
      * This routine does nothing but apply defaults to member variables.
      */

    deque() = default;

    /** \brief Fixes a bug with std::deque.
      *
      * You cannot derive your own deques with std::deque but with the gfc
      * version you can. This virtual destructor allows proper inheritance
      * of deque containers.
      */

    ~deque(){
      clear();
    }

  private:

    mutable atomic::ShareLockRecursive m_tLock;
    std::deque<T> m_dDeque;
  };

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//array:{                                         |

#ifdef __APPLE__
  #pragma mark -
  #pragma mark array
  #pragma mark -
#endif

  /** \brief Wrapper for std::array.
    *
    * This routine is a wrapper for the std::array. It implements a simplified
    * interface and access to much more useable iterators. It is also, like the
    * gfc array class, completely thread safe. Be aware that whenever you grab
    * an iterator or call foreach on the array a spinlock will be acquired and
    * released when you finish the loop or the iterator goes out of scope.
    */

  template<typename T, u32 N> struct E_PUBLISH array final{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |
      //const_iterator:{                          |

#ifdef __APPLE__
  #pragma mark array::const_iterator
#endif

        /** \brief Const iterator object.
          *
          * This iterator simplifies using the normal array<>::const_iterator
          * class. Simply get the iterator from the gfc::array container and
          * test the iterator in a while loop.
          *
          \code
            array<s32>::const_iterator cit = myIntVector.getIterator();
            while( cit ){
              ++cit;
            }
          \endcode
          *
          * But a much easier way of doing the same thing:
          \code
            myIntVector.foreach( []( const s32& i ){});
          \endcode
          */

        struct E_PUBLISH const_iterator final{

          friend struct array;

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline const_iterator operator+( const s32 i )const{
              const_iterator r( *this );
              r.cit += i;
              return r;
            }

            e_forceinline const_iterator operator-( const s32 i )const{
              const_iterator r( *this );
              r.cit -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              cit += i;
            }

            e_forceinline void operator-=( const s32 i ){
              cit -= i;
            }

            /** \brief Pointer operator.
              *
              * This routine will return the current iterator value as a
              * pointer.  It's a nice way of accessing the current value
              * without assigning cit to a temporary first.
              *
              * \return Return a pointer to the current value.
              */

            e_forceinline const T* operator->()const{
              return &(*cit);
            }

            /** \brief Alive method.
              *
              * This routine checks whether the iterator is alive or not. It is
              * called by the boolean operator below.
              */

            e_forceinline operator bool()const{
              return m_pOuter && ( cit != m_pOuter->m_aArray.end() );
            }

            /** \brief Return the current value.
              *
              * This routine will return the current value referenced by the
              * iterator. This can be assigned to a temporary or cast.
              *
              * \return Returns the dereferenced inner iterator.
              */

            e_forceinline const T& operator*()const{
              return *cit;
            }

            /** \brief Post-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if cit has
              * reached the end.
              */

            e_forceinline const_iterator operator++(int){
              return const_iterator( *m_pOuter, cit++ );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if cit has
              * reached the end.
              */

            e_forceinline const_iterator operator++(){
              return const_iterator( *m_pOuter, ++cit );
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the array.
              */

            e_forceinline void reset(){
              cit = m_pOuter->m_aArray.begin();
            }

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

          //}:
          //--------------------------------------+-----------------------------

          /** \brief Copy constructor.
            *
            * This constructor will duplicate the array iterator.
            *
            * \param ci The iterator to duplciate.
            */

          e_forceinline const_iterator( const const_iterator& ci )
            : cit( ci.cit )
            , m_tGuard( &ci.m_pOuter->m_tLock, true )
            , m_pOuter( ci.m_pOuter )
          {}

          /** \brief Move constructor.
            *
            * This constructor will move an array into this one. After the move
            * ownership of the data is said to belong here.
            */

          e_forceinline const_iterator( const_iterator&& rvalue )
              : cit( std::move( rvalue.cit ))
              , m_tGuard( &rvalue.m_pOuter->m_tLock, true )
              , m_pOuter( rvalue.m_pOuter ){
            rvalue.m_pOuter = nullptr;
          }

          const_iterator() = default;
        ~ const_iterator() = default;

        private:

          e_forceinline const_iterator( const array& outer, const typename std::array<T,N>::const_iterator& ci )
            : cit( ci )
            , m_tGuard( &outer.m_tLock, true )
            , m_pOuter( &outer )
          {}

          e_forceinline const_iterator( const array& outer )
            : cit( outer.begin() )
            , m_tGuard( &outer.m_tLock, true )
            , m_pOuter( &outer )
          {}

          typename std::array<T,N>::const_iterator cit;
          atomic::ShareLockRecursive::LockGuard m_tGuard;
          const array* m_pOuter = nullptr;
        };

      //}:                                        |
      //iterator:{                                |

#ifdef __APPLE__
  #pragma mark array::iterator
#endif

        /** \brief A std::iterator wrapper.
          *
          * This class represents a standard iterator object. It's a wrapper
          * class that's returned from gfc::array<>::getIterator() and is a
          * fun easier way of walking over objects without using begin() and
          * end().
          */

        struct E_PUBLISH iterator final{

          friend struct array;

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline iterator operator+( const s32 i )const{
              iterator r( *this );
              r.it += i;
              return r;
            }

            e_forceinline iterator operator-( const s32 i )const{
              iterator r( *this );
              r.it -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              it += i;
            }

            e_forceinline void operator-=( const s32 i ){
              it -= i;
            }

            /** \brief Alive method.
              *
              * This routine checks whether the iterator is alive or not. It is
              * called by the boolean operator below.
              */

            e_forceinline operator bool()const{
              return m_pOuter && ( it != m_pOuter->m_aArray.end() );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if it has
              * reached the end.
              */

            e_forceinline iterator operator++(int){
              return iterator( *m_pOuter, it++ );
            }

            /** \brief Pre-increment operator.
              *
              * This operator will increment the iterator to the next element
              * in the array. Use the boolean operator to test if cit has
              * reached the end.
              */

            e_forceinline iterator operator++(){
              return iterator( *m_pOuter, ++it );
            }

            /** \brief Return the current value.
              *
              * This routine will return the current value referenced by the
              * iterator. This can be assigned to a temporary or cast.
              *
              * \return Returns the dereferenced inner iterator.
              */

            e_forceinline T& operator*(){
              return *it;
            }

            /** \brief Pointer operator.
              *
              * This routine will return the current iterator value as a
              * pointer.  It's a nice way of accessing the current value
              * without assigning it to a temporary first.
              *
              * \return Return a pointer to the current value.
              */

            e_forceinline T* operator->(){
              T& t = *it;
              return &t;
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the array.
              */

            e_forceinline void reset(){
              it = m_pOuter->m_aArray.begin();
            }

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          e_forceinline iterator( const const_iterator& ci )
            : it( e_castIterator<array>( ci, ci.m_pOuter->m_aArray ))
            , m_pOuter(  const_cast<array*>( ci.m_pOuter ))
            , m_tGuard( &const_cast<array*>( ci.m_pOuter )->m_tLock, false )
          {}

          e_forceinline iterator( const iterator& i )
            : it( i.it )
            , m_tGuard( &i.m_pOuter->m_tLock, false )
            , m_pOuter( i.m_pOuter )
          {}

          e_forceinline iterator( iterator&& rvalue )
              : it( std::move( rvalue.it ))
              , m_tGuard( &rvalue.m_pOuter->m_tLock, false )
              , m_pOuter( rvalue.m_pOuter ){
            rvalue.m_pOuter = nullptr;
          }

          iterator() = default;
        ~ iterator() = default;

        private:

          e_forceinline iterator( array& outer, const typename std::array<T,N>::iterator& i )
            : it( i )
            , m_tGuard( &outer.m_tLock, false )
            , m_pOuter( &outer )
          {}

          e_forceinline iterator( array& outer )
            : it( outer.m_aArray.begin() )
            , m_tGuard( &outer.m_tLock, false )
            , m_pOuter( &outer )
          {}

          typename std::array<T,N>::iterator it;
          atomic::ShareLockRecursive::LockGuard m_tGuard;
          array* m_pOuter = nullptr;
        };

      //}:                                        |
    //}:                                          |
    //Operate:{                                   |

#ifdef __APPLE__
  #pragma mark array operators
#endif

      /** \brief Conversation operator.
        *
        * This operator will convert the array object to a T pointer.
        */

      e_forceinline operator const T*()const{
        return &m_aArray[0];
      }

      /** \brief Conversation operator.
        *
        * This operator will convert the array object to a T pointer.
        */

      e_forceinline operator T*(){
        return &m_aArray[0];
      }

      /** \brief Assignment operator.
        *
        * This routine will assign one gfc::array to this one.
        *
        * \param a The array to assign to this one.
        *
        * \return Returns *this.
        */

      e_forceinline array& operator=( const array& a ){
        if( this != &a ){
          e_guardw( m_tLock );
          m_aArray = a.m_aArray;
        }
        return *this;
      }

      /** \brief Rvalue assignment operator.
        *
        * This routine will assign one gfc::array to this one.
        *
        * \param a The array to assign to this one.
        *
        * \return Returns *this.
        */

      e_forceinline array& operator=( array&& a ){
        if( this != &a ){
          e_guardw( m_tLock );
          m_aArray = std::move( a.m_aArray );
        }
        return *this;
      }

      /** \brief Bracket operator.
        *
        * This routine will return the Nth element in the array. This is the
        * const version of the operator. Please note that this method is not
        * thread safe. Only use it if you KNOW another thread isn't going to
        * reallocate the array.
        *
        * \param e The Nth element in the array to dereference.
        *
        * \return Returns the i element in the array.
        */

      template<typename E> e_forceinline const T& operator[]( const E e )const{
        e_assert( u32( e ) < N );
        return m_aArray[ u32( e )];
      }

      /** \brief Bracket operator.
        *
        * This routine will return the Nth element in the array. This code is
        * absolutely not thread safe so be cautious when you use it. Make
        * absolutely sure that your array is not being modified by another
        * thread.
        *
        * \param e The Nth element in the array to dereference.
        *
        * \return Returns the i element in the array.
        */

      template<typename E> e_forceinline T& operator[]( const E e ){
        e_assert( u32( e ) < N );
        return m_aArray[ u32( e )];
      }

    //}:                                          |
    //Methods:{                                   |

#ifdef __APPLE__
  #pragma mark array methods
#endif

      /** \brief Get constant iterator.
        *
        * \return This routine will return a constant iterator for the array.
        * With it one can easily walk over all elements.
        */

      e_forceinline const_iterator getIterator()const{
        return const_iterator( *this );
      }

      /** \brief Get iterator.
        *
        * \return This routine will return a constant iterator for the array.
        * With it one can easily walk over all elements.
        */

      e_forceinline iterator getIterator(){
        return iterator( *this );
      }

      /** \brief Try querying an element in this array.
        *
        * This routine will run the given lambda to the array at the given
        * element index.
        *
        * \param i Index into the
        * \param lambda The lambda function to apply the ith element in the
        * array.
        *
        * \return Returns true if the spinlock was acquired or false.
        */

      e_noinline s32 tryQuery( const u32 i, const std::function<void( const T& t )>& lambda )const{
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryr( m_tLock ){
          if( i < size() ){
            lambda( m_aArray[ i ]);
            result = 1;
          }else{
            result = 0;
          }
        }
        return result;
      }

      /** \brief Query an element in the array.
        *
        * This routine will run the given lambda to the array at the given
        * element index.
        *
        * \param i Index into the
        * \param lambda The lambda function to apply the ith element in the
        * array.
        *
        * \return Returns true if i was in the range 0 to size()-1 or false.
        */

      e_noinline bool query( const u32 i, const std::function<void( const T& t )>& lambda )const{
        if( lambda ){
          e_guardr( m_tLock );
          if( i < size() ){
            lambda( m_aArray[ i ]);
            return true;
          }
        }
        return false;
      }

      /** \brief Try Mutating an element in the array.
        *
        * This routine will apply the given lambda to the array at the given
        * element index.
        *
        * \param i Index into the
        * \param lambda The lambda function to apply the ith element in the
        * array.
        *
        * \return Returns true if spinlock was acquired and the operation was
        * attempted or false.
        */

      e_noinline s32 tryAlter( const u32 i, const std::function<void( T& t )>& lambda ){
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryw( m_tLock ){
          if( i < size() ){
            lambda( m_aArray[ i ]);
            result = 1;
          }else{
            result = 0;
          }
        }
        return result;
      }

      /** \brief Mutate an element in the array.
        *
        * This routine will apply the given lambda to the array at the given
        * element index.
        *
        * \param i Index into the
        * \param lambda The lambda function to apply the ith element in the
        * array.
        *
        * \return Returns true if i was in the range 0 to size()-1 or false.
        */

      e_noinline bool alter( const u32 i, const std::function<void( T& t )>& lambda ){
        if( lambda ){
          e_guardw( m_tLock );
          if( i < size() ){
            lambda( m_aArray[ i ]);
            return true;
          }
        }
        return false;
      }

      /** \brief For each iterator (const).
        *
        * This routine will call the given function to every element in the
        * array. This version of gfc::array<>::foreach() expects a boolean to
        * be returned by the lambda. If false is returned by "f" then iteration
        * will stop immediately and foreach() returns.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline s32 tryForeachs( const std::function<bool( const T& )>& lambda )const{
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryr( m_tLock ){
          for( const T& a : m_aArray ){
            if( !lambda( a )){
              break;
            }
          }
          result = 1;
        }
        return result;
      }

      /** \brief For each iterator (const).
        *
        * This routine will call the given function to every element in the
        * array. This version of gfc::array<>::foreach() expects a boolean to
        * be returned by the lambda. If false is returned by "f" then iteration
        * will stop immediately and foreach() returns.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline bool foreachs( const std::function<bool( const T& )>& lambda )const{
        if( !lambda ){
          return false;
        }
        e_guardr( m_tLock );
        for( const T& a : m_aArray ){
          if( !lambda( a )){
            break;
          }
        }
        return true;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array. This version of gfc::array<>::foreach() expects a boolean to
        * be returned by the lambda. If false is returned by "f" then iteration
        * will stop immediately and foreach() returns. Like iterators foreach()
        * is perfectly thread safe.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline s32 tryForeachs( const std::function<bool( T& )>& lambda ){
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryw( m_tLock ){
          for( T& a : m_aArray ){
            if( !lambda( a )){
              break;
            }
          }
          result = 1;
        }
        return result;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array. This version of gfc::array<>::foreach() expects a boolean to
        * be returned by the lambda. If false is returned by "f" then iteration
        * will stop immediately and foreach() returns. Like iterators foreach()
        * is perfectly thread safe.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline bool foreachs( const std::function<bool( T& )>& lambda ){
        if( !lambda ){
          return false;
        }
        e_guardw( m_tLock );
        for( T& a : m_aArray ){
          if( !lambda( a )){
            break;
          }
        }
        return true;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline s32 tryForeach( const std::function<void( const T& )>& lambda )const{
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryr( m_tLock ){
          for( const T& a : m_aArray ){
            lambda( a );
          }
          result = 1;
        }
        return result;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline bool foreach( const std::function<void( const T& )>& lambda )const{
        if( !lambda ){
          return false;
        }
        e_guardw( m_tLock );
        for( const T& a : m_aArray ){
          lambda( a );
        }
        return true;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline s32 tryForeach( const std::function<void( T& )>& lambda ){
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryw( m_tLock ){
          for( T& a : m_aArray ){
            lambda( a );
          }
          result = 1;
        }
        return result;
      }

      /** \brief For each iterator.
        *
        * This routine will call the given function to every element in the
        * array.
        *
        * \param lambda The lambda to call for each element in the array.
        */

      e_noinline bool foreach( const std::function<void( T& )>& lambda ){
        if( !lambda ){
          return false;
        }
        e_guardw( m_tLock );
        for( T& a : m_aArray ){
          lambda( a );
        }
        return true;
      }

      /** \brief Sort array with lambda.
        *
        * This routine will sort the array by calling the lambda provided.
        * This just uses std::sort.
        *
        * \param lambda The lambda to use as a callback during sorting. This
        * is passed directly to std::sort.
        */

      e_noinline s32 trySort( const std::function<bool( const T&, const T& )>& lambda ){
        if( !lambda ){
          return 0;
        }
        s32 result = -1;
        e_tryw( m_tLock ){
          std::sort( m_aArray.begin(), m_aArray.end(), lambda );
          result = 1;
        }
        return result;
      }

      /** \brief Sort array with lambda.
        *
        * This routine will sort the array by calling the lambda provided.
        * This just uses std::sort.
        *
        * \param lambda The lambda to use as a callback during sorting. This
        * is passed directly to std::sort.
        */

      e_noinline bool sort( const std::function<bool( const T&, const T& )>& lambda ){
        if( !lambda ){
          return false;
        }
        e_guardw( m_tLock );
        std::sort( m_aArray.begin(), m_aArray.end(), lambda );
        return true;
      }

      /** \brief Gets the total number of elements in the array.
        *
        * This routine will return the total number of elements in the array.
        *
        * \return Returns the number of possible entries in the array.
        */

      e_forceinline constexpr u32 size()const{
        return N;
      }

      /** \brief Get the actual size.
        *
        * This routine will return the actual allocated size. This is so you
        * can alias something like array<T,0x7FFFFFFF> to another array type
        * (see property serialization) and size() still returns the actual
        * size of the array or the original N value.
        */

     e_forceinline u32 capacity()const{
       return m_cCapacity;
     }

     /** \brief Get the actual stride or sizeof T.
      *
      * This routine just returns the stride of each element in the array.
      */

    e_forceinline u32 stride()const{
      return m_cStride;
    }

    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline array( std::initializer_list<T> l ){
      u32 i = 0;
      for( auto& a : l ){
        e_assert( i<N );
        m_aArray[ i++ ] = a;
      }
    }

    e_forceinline array( const vector<T>& v ){
      u32 i = 0;
      while( i < N && i < v.size() ){
        m_aArray[ i++ ] = v[ i ];
      }
    }

    e_forceinline array( const deque<T>& d ){
      u32 i = 0;
      while( i < N && i < d.size() ){
        m_aArray[ i++ ] = d[ i ];
      }
    }

    e_forceinline array( const array& lvalue )
      : m_aArray( lvalue.m_aArray )
    {}

    e_forceinline array( array&& rvalue )
      : m_aArray( std::move( rvalue.m_aArray ))
    {}

  ~ array() = default;
    array(){
      if( std::is_pod<T>::value ){
        memset( vp( m_aArray.data() ), 0, sizeof( T )*N );
      }
    }

  private:

    const u32                          m_cCapacity = N;
    const u32                          m_cStride   = sizeof( T );
    mutable atomic::ShareLockRecursive m_tLock;//Must come before array.
    std::array<T,N>                    m_aArray;
  };

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//map:{                                           |

#ifdef __APPLE__
  #pragma mark -
  #pragma mark map
  #pragma mark -
#endif

  /** \brief GFC map class.
    *
    * This template wraps the standard library map<> and provides a number of
    * useful additions like easier-to-use iterators and helper functions.
    */

  template<typename K,typename T> struct E_PUBLISH map final{

    //--------------------------------------------+-----------------------------
    //Structs:{                                   |
      //const_iterator:{                          |

        struct E_PUBLISH const_iterator final{

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline const_iterator operator+( const s32 i )const{
              const_iterator r( *this );
              r.cit += i;
              return r;
            }

            e_forceinline const_iterator operator-( const s32 i )const{
              const_iterator r( *this );
              r.cit -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              cit += i;
            }

            e_forceinline void operator-=( const s32 i ){
              cit -= i;
            }

            e_forceinline const T* operator->()const{
              return cit;
            }

            e_forceinline const T& operator*()const{
              return cit->second;
            }

            e_forceinline operator bool()const{
              return m_pOuter && ( cit != m_pOuter->m_mMap.end() );
            }

            e_forceinline void operator++(){
              ++cit;
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the vector.
              */

            e_forceinline void reset(){
              cit = m_pOuter->m_mMap.begin();
            }

            e_forceinline K key()const{
              return cit->first;
            }

            e_forceinline const T* value()const{
              return &cit->second;
            }

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          e_forceinline const_iterator( const const_iterator& i )
            : cit( i.cit )
            , m_pOuter( i.m_pOuter )
          {}

          e_forceinline const_iterator( const_iterator&& i )
              : cit( std::move( i.cit ))
              , m_pOuter( i.m_pOuter ){
            i.m_pOuter = nullptr;
          }

          e_forceinline~const_iterator(){
            release();
          }

          const_iterator() = default;

        private:

          e_forceinline explicit const_iterator( map& outer )
              : cit( outer.m_mMap.begin() )
              , m_pOuter( &outer ){
            acquire();
          }

          e_forceinline bool acquire(){
            if( m_pOuter ){
              m_pOuter->m_tLock.acquire();
              return true;
            }
            return false;
          }

          e_forceinline bool release(){
            if( m_pOuter ){
              m_pOuter->m_tLock.release();
              return true;
            }
            return false;
          }

          typename std::map<K,T>::const_iterator cit;
          map* m_pOuter = nullptr;
          friend struct map;
        };

      //}:                                        |
      //iterator:{                                |

        struct E_PUBLISH iterator final{

          //--------------------------------------+-----------------------------
          //Operate:{                             |

            e_forceinline iterator operator+( const s32 i )const{
              iterator r( *this );
              r.it += i;
              return r;
            }

            e_forceinline iterator operator-( const s32 i )const{
              iterator r( *this );
              r.it -= i;
              return r;
            }

            e_forceinline void operator+=( const s32 i ){
              it += i;
            }

            e_forceinline void operator-=( const s32 i ){
              it -= i;
            }

            e_forceinline const T& operator*()const{
              return it->second;
            }

            e_forceinline operator bool()const{
              return m_pOuter && ( it != m_pOuter->m_mMap.end() );
            }

            e_forceinline T* operator->()const{
              return it;
            }

            e_forceinline void operator++(){
              ++it;
            }

          //}:                                    |
          //Methods:{                             |

            /** \brief Erase element at iterator.
              *
              * This routine will nuke the object at the iterator's current
              * position.  This is great for lists but DO NOT call it more than
              * once in the same loop or you'll crash.
              */

            e_forceinline void erase(){
              m_pOuter->m_mMap.erase( it );
            }

            /** \brief Reset iterator to beginning.
              *
              * This routine will reset the iterator back to the beginning of
              * the vector.
              */

            e_forceinline void reset(){
              it = m_pOuter->m_mMap.begin();
            }

            e_forceinline K key()const{
              return it->first;
            }

            e_forceinline T* value(){
              return &it->second;
            }

            e_forceinline void detach(){
              m_pOuter = nullptr;
            }

          //}:                                    |
          //--------------------------------------+-----------------------------

          /** \brief Copy constructor.
            *
            * This routine will initialize the iterator by making a copy of the
            * outer class of type gfc::map.
            *
            * \param ci The iterator to copy.
            */

          e_forceinline iterator( const const_iterator& ci )
            : it( e_castIterator<map>( ci, ci.m_pOuter->m_mMap ))
            , m_pOuter( const_cast<map*>(  ci.m_pOuter ))
          {}

          /** \brief Copy constructor.
            *
            * This routine will initialize the iterator by making a copy of the
            * outer class of type gfc::map.
            *
            * \param i The iterator to copy.
            */

          e_forceinline iterator( const iterator& i )
            : it( i.it )
            , m_pOuter( i.m_pOuter )
          {}

          /** \brief Move constructor.
            *
            * This routine will initialize the iterator by moving the outer
            * class of type gfc::map.
            *
            * \param rvalue The iterator to move.
            */

          e_forceinline iterator( iterator&& rvalue )
              : it( std::move( rvalue.it ))
              , m_pOuter( rvalue.m_pOuter ){
            rvalue.m_pOuter = nullptr;
          }

          /** \brief Iterator destructor.
            *
            * This destructor simply releases the spinlock.
            */

          e_forceinline~iterator(){
            release();
          }

          iterator() = default;

        private:

          e_forceinline explicit iterator( map& outer, const typename std::map<K,T>::iterator& i )
              : m_pOuter( &outer )
              , it( i ){
            acquire();
          }

          e_forceinline explicit iterator( map& outer )
              : it( outer.m_mMap.begin() )
              , m_pOuter( &outer ){
            acquire();
          }

          e_forceinline bool acquire(){
            if( m_pOuter ){
              m_pOuter->m_tLock.acquire();
              return true;
            }
            return false;
          }

          e_forceinline bool release(){
            if( m_pOuter ){
              m_pOuter->m_tLock.release();
              return true;
            }
            return false;
          }

          typename std::map<K,T>::iterator it;
          map* m_pOuter = nullptr;
          friend struct map;
        };

      //}:                                        |
    //}:                                          |
    //Operate:{                                   |

      /** \brief Bracket operator.
        *
        * This routine will return the Nth element in the map. This is the
        * const version of the operator. Please note that this method is not
        * thread safe. Only use it if you KNOW another thread isn't going to
        * reallocate the map.
        *
        * \param key The Nth element in the map to dereference.
        *
        * \return Returns the i element in the map.
        */

      e_forceinline const T operator[]( const K& key )const{
        e_guardw( m_tLock );
        T outT{};
        const auto it = m_mMap.find( key );
        if( it != m_mMap.end() ){
          outT = it->second;
        }
        return outT;
      }

      /** \brief Copy assignment operator.
        *
        * This operator will copy assign the given map to this map.
        *
        * \param m The gfc::map object to assign to this one.
        *
        * \return Returns a reference to this object.
        */

      e_forceinline map& operator=( const map& m ){
        e_guardw( m_tLock );
        if( this != &m ){
          m_mMap = m.m_mMap;
        }
        return *this;
      }

      /** \brief Move assignment operator.
        *
        * This operator will move assign the given map to this map.
        *
        * \param m The gfc::map object to assign to this one.
        *
        * \return Returns a reference to this object.
        */

      e_forceinline map& operator=( map&& m ){
        e_guardw( m_tLock );
        if( this != &m ){
          m_mMap = std::move( m.m_mMap );
        }
        return *this;
      }

    //}:                                          |
    //Methods:{                                   |
      //getIterator:{                             |

        e_forceinline const_iterator getIterator()const{
          return const_iterator( *this );
        }

        e_forceinline iterator getIterator(){
          return iterator( *this );
        }

      //}:                                        |
      //foreach*:{                                |
        //foreach?_pair:{                         |

          e_noinline bool foreachsPair( const std::function<bool( const std::pair<K,T>& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              if( !f( a )){
                break;
              }
            }
            return true;
          }

          e_noinline bool foreachPair( const std::function<void( const std::pair<K,T>& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              f( a );
            }
            return true;
          }

        //}:                                      |
        //foreach?_kv:{                           |

          e_noinline bool foreachsKV( const std::function<bool( const K&, const T& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              if( !f( a.first, a.second )){
                break;
              }
            }
            return true;
          }

          e_noinline bool foreachsKV( const std::function<bool( const K&, T& )>& f ){
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( auto& a : m_mMap ){
              if( !f( a.first, a.second )){
                break;
              }
            }
            return true;
          }

          e_noinline bool foreachKV( const std::function<void( const K&, const T& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              f( a.first, a.second );
            }
            return true;
          }

          e_noinline bool foreachKV( const std::function<void( const K&, T& )>& f ){
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( auto& a : m_mMap ){
              f( a.first, a.second );
            }
            return true;
          }

        //}:                                      |
        //foreach?:{                              |

          e_noinline bool foreachs( const std::function<bool( const T& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              if( !f( a.second )){
                break;
              }
            }
            return true;
          }

          e_noinline bool foreachs( const std::function<bool( T& )>& f ){
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( auto& a : m_mMap ){
              if( !f( a.second )){
                break;
              }
            }
            return true;
          }

          e_noinline bool foreach( const std::function<void( const T& )>& f )const{
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( const auto& a : m_mMap ){
              f( a.second );
            }
            return true;
          }

          e_noinline bool foreach( const std::function<void( T& )>& f ){
            e_guardw( m_tLock );
            if( empty() ){
              return false;
            }
            for( auto& a : m_mMap ){
              f( a.second );
            }
            return true;
          }

        //}:                                      |
      //}:                                        |
      //alter:{                                   |

        e_forceinline bool alterKV( const K& key, const std::function<void( const K& key, T& value )>& lambda ){
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          if( it != m_mMap.end() ){
            lambda( it->first, it->second );
            return true;
          }
          return false;
        }

        e_forceinline bool alter( const K& key, const std::function<void( T& )>& lambda ){
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          if( it != m_mMap.end() ){
            lambda( it->second );
            return true;
          }
          return false;
        }

      //}:                                        |
      //query:{                                   |

        e_forceinline bool queryKV( const K& key, const std::function<void( const K& key, const T& value )>& lambda )const{
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          if( it != m_mMap.end() ){
            lambda( it->first, it->second );
            return true;
          }
          return false;
        }

        e_forceinline bool query( const K& key, const std::function<void( const T& )>& lambda )const{
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          if( it != m_mMap.end() ){
            lambda( it->second );
            return true;
          }
          return false;
        }

      //}:                                        |
      //empty:{                                   |

        e_forceinline bool empty()const{
          e_guardw( m_tLock );
          return m_mMap.empty();
        }

      //}:                                        |
      //erase:{                                   |

        e_forceinline void erase( const K& key, const std::function<void()>& lambda=nullptr ){
          e_guardw( m_tLock );
          m_mMap.erase( key );
          if( lambda ){
            lambda();
          }
        }

      //}:                                        |
      //clear:{                                   |

        e_forceinline void clear(){
          e_guardw( m_tLock );
          m_mMap.clear();
        }

      //}:                                        |
      //size:{                                    |

        e_forceinline u32 size()const{
          e_guardw( m_tLock );
          return u32( m_mMap.size() );
        }

      //}:                                        |
      //find:{                                    |

        e_forceinline bool find( const K& key, T& output )const{
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          if( it != m_mMap.end() ){
            output = it->second;
            return true;
          }
          return false;
        }

        e_forceinline bool find( const K& key )const{
          e_guardw( m_tLock );
          const auto it = m_mMap.find( key );
          return( it != m_mMap.end() );
        }

      //}:                                        |
      //set*:{                                    |

        /** \brief Set element via lambda.
          *
          * This routine will gain a reference to a new or existing element in
          * the map and pass it off to the lambda provided.
          *
          * \param key The key where we'd like to potentially allocate an item.
          * \param lambda The callback lambda that's invoked once we gain
          * access to the map element.
          *
          * \return Returns true if the item is new and false otherwise.
          */

        e_forceinline bool setBy( const K& key, const std::function<void( T& t )>& lambda ){
          e_guardw( m_tLock );
          const bool bNew=( m_mMap.find( key ) == m_mMap.end() );
          T& t = m_mMap[ key ];
          lambda( t );
          return bNew;
        }

        /** \brief Set element on the map.
          *
          * This routine will assign the input value to a new or existing item
          * in the map.
          *
          * \param key The key of the element we're trying to change.
          * \param value The value we want to assign to that key.
          *
          * \return Returns true if the element was newly created or didn't
          * exist before this call to set, or false if it already existed.
          */

        e_forceinline bool set( const K& key, const T& value ){
          e_guardw( m_tLock );
          const bool bNew=( m_mMap.find( key ) == m_mMap.end() );
          m_mMap[ key ] = value;
          return bNew;
        }

      //}:                                        |
      //get:{                                     |

        e_forceinline T get( const K& key )const{
          return( *this )[ key ];
        }

      //}:                                        |
    //}:                                          |
    //--------------------------------------------+-----------------------------

    e_forceinline map( const map& m ){
      m_mMap = m.m_mMap;
    }

    e_forceinline map( map&& m )
      : m_mMap( std::move( m.m_mMap ))
    {}

    map() = default;
  ~ map(){
      clear();
    }

  private:

    mutable atomic::ShareLockRecursive m_tLock;
    std::map<K,T> m_mMap;
  };

//}:                                              |
//================================================+=============================

}}

#if e_compiling( clang )
  #pragma clang diagnostic pop
#elif e_compiling( gcc )
  #pragma GCC diagnostic pop
#elif e_compiling( microsoft )
  #pragma warning( default:4200 )
#endif

/**     @}
  *   @}
  * @}
  */
