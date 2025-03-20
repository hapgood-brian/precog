//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if !e_compiling( web )
  #if !e_compiling( win64 )
    #include<execinfo.h>
  #endif
  #include<atomic>
#endif

using namespace EON;
using namespace gfc;
using namespace atomic;

//================================================+=============================
//atomic:{                                        |
  //Defines:{                                     |

    #if !e_compiling( web )
      #if e_compiling( osx )
        #define USE_SPINLOCK 0
        #define USE_LOGGING  0
        #define USE_YIELDS   1
      #elif e_compiling( win64 )
        #define USE_SPINLOCK 1
        #define USE_LOGGING  0
        #define USE_YIELDS   4
      #endif
    #endif

  //}:                                            |
  //Private:{                                     |

    #if !e_compiling( web )
      #if USE_LOGGING
        namespace{
          __thread char msgBuffer[ 1024 ]{};
        }
      #endif
    #endif

  //}:                                            |
  //Structs:{                                     |
    //ShareLockRecursive:{                        |
      //Methods:{                                 |
        //tryAcquireRead:{                        |

          bool ShareLockRecursive::tryAcquireRead(){
            if( Thread::count() <= 1 ){
              return true;
            }
            #if e_compiling( web )
              return true;
            #else
              decltype( m_tLock )::LockGuard lockGuard( m_tLock );
              // If current thread has already acquired the share lock then
              // we're allowed to increment the reader count.
              const auto tid = Thread::tid();
              bool bResult = m_atomicReads.query( tid, nullptr/* just query */);
              // If there are any write locks belonging to a different thread
              // we cannot acquire the read lock.
              if( !bResult ){
                // If the writer thread id is non-zero and not equal to the
                // current thread id we'll return false because we cannot
                // obtain a write lock; another thread obviously owns it.
                const auto oth = m_atomicWrites.tid();
                const auto tid = Thread::tid();
                if( oth && tid != oth ){
                  // It's possible for oth to be invalid. Or in other words the
                  // thread id 'oth' is not a living thread anymore. Test here.
                  return false;
                }
              }
              // Ok, no write lock on another thread so acquire for reading.
              m_atomicReads.alter( tid,
                []( u32& out ){
                  ++out;
                }
              );
              #if USE_LOGGING
                #if e_compiling( win64 )
                  sprintf_s( msgBuffer
                   , sizeof( msgBuffer )
                #else
                  sprintf( msgBuffer
                #endif
                  , "[%u (%p)] tryAcquireRead: reads=%u writes=%u (%u)"
                  , tid
                  , this
                  , m_atomicReads.size()
                  , m_atomicWrites.ownership()
                  , m_atomicWrites.tid() );
                e_log( msgBuffer );
              #endif
              return true;
            #endif
          }

        //}:                                      |
        //releaseRead:{                           |

          void ShareLockRecursive::releaseRead(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              const auto tid = Thread::tid();
              m_atomicReads.condErase( tid,
                []( u32& refs ){
                  if( refs == 1 ){// yes, erase.
                    return true;
                  }
                  --refs;// no, decrement.
                  return false;
                }
              );
              #if USE_LOGGING
                #if e_compiling( win64 )
                  sprintf_s( msgBuffer
                   , sizeof( msgBuffer )
                #else
                  sprintf( msgBuffer
                #endif
                  , "[%u (%p)] releaseRead: reads=%u writes=%u (%u)"
                  , tid
                  , this
                  , m_atomicReads.size()
                  , m_atomicWrites.ownership()
                  , m_atomicWrites.tid() );
                e_log( msgBuffer );
              #endif
            #endif
          }

        //}:                                      |
        //acquireRead:{                           |

          void ShareLockRecursive::acquireRead(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              for( u32 i=0; !tryAcquireRead(); ++i ){
                #if USE_YIELDS
                  if( !( i % USE_YIELDS )){
                    Thread::yield();
                  }
                #endif
              }
            #endif
          }

        //}:                                      |
        //tryAcquire:{                            |

          bool ShareLockRecursive::tryAcquire(){
            if( Thread::count() <= 1 ){
              return true;
            }
            #if e_compiling( web )
              return true;
            #else
              const auto tid = Thread::tid();
              #if 0 // experimental attempt!
                u32 counts = 0;
                u32 theirs = 0;
                m_atomicReads.foreachKV(
                  [&]( const u32 key, const u32& value ){
                    if( key == tid ){
                      counts += value;
                    }else{
                      theirs += value;
                    }
                  }
                );
                bool bResult = false;
                if( !theirs ){
                  bResult = m_atomicWrites.tryAcquire();
                }
                return bResult;
              #else
                if( !m_tLock.tryAcquire() ){
                  return false;
                }
                // Count up the number of keys sharing this thread id and if that
                // count is nil return false again.
                const bool bEmpty = m_atomicReads.empty();
                if( !bEmpty ){
                  u32 count = 0;
                  m_atomicReads.foreachKV(
                    [&]( const u32 key, const u32& value ){
                      if( key == tid ){
                        count += value;
                      }
                    }
                  );
                  if( !count ){
                    m_tLock.release();
                    return false;
                  }
                }
                // If the writer thread id is non-zero and not equal to the
                // current thread id we'll return false because we cannot
                // obtain a write lock; another thread obviously owns it.
                bool bResult = true;
                const auto oth = m_atomicWrites.tid();
                if( oth && oth != tid ){
                  // It's possible for oth to be invalid. Or in other words the
                  // thread id 'oth' is not a living thread anymore. Test here.
                  bResult = false;
                }else{
                  // If the current thread matches then we can increment the write
                  // count and be on our way.
                  m_atomicWrites.tryAcquire();
                  bResult = true;
                }
                #if USE_LOGGING
                  #if e_compiling( win64 )
                    sprintf_s( msgBuffer
                     , sizeof( msgBuffer )
                  #else
                    sprintf( msgBuffer
                  #endif
                    , "[%u (%p)] tryAcquire: reads=%u writes=%u (%u)"
                    , tid
                    , this
                    , m_atomicReads.size()
                    , m_atomicWrites.ownership()
                    , m_atomicWrites.tid() );
                  e_log( msgBuffer );
                #endif
                m_tLock.release();
                return bResult;
              #endif
            #endif
          }

        //}:                                      |
        //acquire:{                               |

          void ShareLockRecursive::acquire(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              for( u32 i=0; !tryAcquire(); ++i ){
                #if USE_YIELDS
                  if( !( i % USE_YIELDS )){
                    Thread::yield();
                  }
                #endif
              }
            #endif
          }

        //}:                                      |
        //release:{                               |

          void ShareLockRecursive::release(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              m_atomicWrites.release();
              #if USE_LOGGING
                const auto tid = Thread::tid();
                #if e_compiling( win64 )
                  sprintf_s( msgBuffer
                   , sizeof( msgBuffer )
                #else
                  sprintf( msgBuffer
                #endif
                  , "[%u (%p)] release: reads=%u writes=%u (%u)"
                  , tid
                  , this
                  , m_atomicReads.size()
                  , m_atomicWrites.ownership()
                  , m_atomicWrites.tid() );
                e_log( msgBuffer );
              #endif
            #endif
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
    //SpinlockRecursive:{                         |
      //Methods:{                                 |
        //tryAcquire:{                            |

          bool SpinlockRecursive::tryAcquire(){
            if( Thread::count() <= 1 ){
              return true;
            }
            #if e_compiling( web )
              return true;
            #else
              const auto tid = Thread::tid();
              auto lck = m_uLockTID.load();
              if( !m_uLockTID.compare_exchange_weak( lck, tid, std::memory_order_acquire )){
                return false;
              }
              ++m_uWrites;
              return true;
            #endif
          }

        //}:                                      |
        //acquire:{                               |

          void SpinlockRecursive::acquire(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              for( u32 i=0; !tryAcquire(); ++i ){
                #if USE_YIELDS
                  if( !( i % USE_YIELDS )){
                    Thread::yield();
                  }
                #endif
              }
            #endif
          }

        //}:                                      |
        //release:{                               |

          void SpinlockRecursive::release(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              if( --m_uWrites < 1 ){
                m_uLockTID.store( 0, std::memory_order_release );
              }
            #endif
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
    //Spinlock:{                                  |
      //Methods:{                                 |
        //tryAcquire:{                            |

          bool Spinlock::tryAcquire(){
            if( Thread::count() <= 1 ){
              return true;
            }
            #if e_compiling( web )
              return true;
            #else
              u32 uExpected{};
              const auto tid = Thread::tid();
              return( m_tLock.compare_exchange_weak(
                  uExpected
                , tid
                , std::memory_order_acquire )
              );
            #endif
          }

        //}:                                      |
        //acquire:{                               |

          void Spinlock::acquire(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              for( u32 i=0; !tryAcquire(); ++i ){
                #if USE_YIELDS
                  if( !( i % USE_YIELDS )){
                    Thread::yield();
                  }
                #endif
              }
            #endif
          }

        //}:                                      |
        //release:{                               |

          void Spinlock::release(){
            if( Thread::count() <= 1 ){
              return;
            }
            #if !e_compiling( web )
              m_tLock.store( 0, std::memory_order_release );
            #endif
          }

        //}:                                      |
      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
