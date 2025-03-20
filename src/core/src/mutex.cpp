//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if e_compiling( win64 )
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include<windows.h>
#else
  #include<pthread.h>
  #include<sched.h>
#endif

using namespace EON;
using namespace gfc;

//================================================+=============================
//Mutex:{                                         |
  //acquire:{                                     |

    bool Mutex::acquire(){
      #if e_compiling( win64 )
        LPCRITICAL_SECTION pcs = reinterpret_cast<LPCRITICAL_SECTION>( m_pNative );
        EnterCriticalSection( pcs );
      #else
        pthread_mutex_t* pMutex = reinterpret_cast<pthread_mutex_t*>( m_pNative );
        pthread_mutex_lock( pMutex );
      #endif
      return true;
    }

  //}:                                            |
  //release:{                                     |

    void Mutex::release(){
      #if e_compiling( win64 )
        LPCRITICAL_SECTION pcs = reinterpret_cast<LPCRITICAL_SECTION>( m_pNative );
        LeaveCriticalSection( pcs );
      #else
        pthread_mutex_t* pMutex = reinterpret_cast<pthread_mutex_t*>( m_pNative );
        pthread_mutex_unlock( pMutex );
      #endif
    }

  //}:                                            |
  //[ctor]:{                                      |

    Mutex::Mutex(){
      #if e_compiling( win64 )
        LPCRITICAL_SECTION pcs = new CRITICAL_SECTION;
        ZeroMemory( pcs, sizeof( CRITICAL_SECTION ));
        InitializeCriticalSection( pcs );
        m_pNative = pcs;
      #else
        pthread_mutex_t* pMutex = new pthread_mutex_t;
        pthread_mutex_init( pMutex, NULL );
        m_pNative = pMutex;
      #endif
    }

  //}:                                            |
  //[dtor]:{                                      |

    Mutex::~Mutex(){
      #if e_compiling( win64 )
        LPCRITICAL_SECTION pcs = reinterpret_cast<LPCRITICAL_SECTION>( m_pNative );
        e_assert( pcs, "Native pointer is nullptr!" );
        DeleteCriticalSection( pcs );
      #else
        pthread_mutex_t* pMutex = reinterpret_cast<pthread_mutex_t*>( m_pNative );
        pthread_mutex_destroy( pMutex );
        delete pMutex;
      #endif
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
