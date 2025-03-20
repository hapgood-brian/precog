//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if !e_compiling( web )
  #if e_compiling( win64 )
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include<windows.h>
  #elif e_compiling( android )
    #include<signal.h>
    #include<unistd.h>
  #elif e_compiling( linux )
    #include<sys/syscall.h>
  #elif e_compiling( osx )||e_compiling( ios )
    #include<pthread.h>
    #include<sched.h>
  #endif
  using namespace EON;
  using namespace gfc;
#endif

thread_local string text;

//================================================+=============================
//Thread:{                                        |
#if !e_compiling( web )
  //Global:{                                      |

    void e_backoff( const double start ){
      #if !e_compiling( win64 )
        const double now = e_seconds();
        const double dur = now - start;
        if( dur > .20 ){
          Thread::sleep( 2 );
          return;
        }
        if( dur > .10 ){
          Thread::sleep( 1 );
          return;
        }
        if( dur > .01 ){
          Thread::yield();
          return;
        }
      #endif
    }

  //}:                                            |
  //Private:{                                     |
    //makeUid:{                                   |

      namespace{
        auto             __masters = Thread::tid();
        __thread Thread* __context = nullptr;
        std::atomic<s32> __count{};
        std::atomic<s32> __peak{};
        bitbucket<65536> __masks;
        u32 makeUid(){
          return u32(__masks.set() & 0xFFFFFFFF );
        }
        hashmap<u32,Thread*>& getThreadMap(){
          static hashmap<u32,Thread*>m;
          return m;
        }
      }

    //}:                                          |
  //}:                                            |
  //Methods:{                                     |
    //RedWorker:{                                 |

      void Thread::RedWorker( Thread* pThread ){

        //----------------------------------------------------------------------
        // Hook thread into database.
        //----------------------------------------------------------------------

        // Grab the platform thread identifier.
        const auto tid = Thread::tid();

        // Save off a pointer to incoming thread.
        pThread->toFlags()->bStarted = 1;
        pThread->setTID( tid );
        __context = pThread;

        // Add thread to the database.
        __peak = e_max<s32>( __peak, ++__count );
        IEngine::runOnMainThread(
          [=](){
            getThreadMap().set( tid, pThread );
          }
        );

        //----------------------------------------------------------------------
        // Execute lambda and/or run method.
        //----------------------------------------------------------------------

        #if e_compiling( osx )
          text.catf( "EON Worker:: %4u", tid );
          pthread_setname_np( text );
        #elif e_compiling( win64 )
          SetThreadDescription(
              GetCurrentThread()
            , L"EON Worker"
          );
        #endif
        try{
          if( pThread->m_onLambda ){
              pThread->m_onLambda();
          }
          pThread->run();
        }
        catch( std::exception e ){
          e_warnsf(
              "Engine caught an exception (%s) in thread %u!"
            , e.what()
            , tid
          );
        }

        //----------------------------------------------------------------------
        // Detach from threading database.
        //----------------------------------------------------------------------

        --__count;
        if( pThread->m_tFlags->bDeleteOnExit ){
          delete pThread;
        }else{
          pThread->toFlags()->bIsDead = 1;
        }
        IEngine::runOnMainThread(
          [=](){
            getThreadMap().erase( tid );
          }
        );
      }

    //}:                                          |
    //isCurrent:{                                 |

      bool Thread::isCurrent()const{
        return( tid()==m_uTID );
      }

    //}:                                          |
    //foreach:{                                   |

      void Thread::foreach( const std::function<void( const IThread* )>& lambda ){
        auto it = getThreadMap().getIterator();
        while( it ){
          lambda( *it );
          ++it;
        }
      }

    //}:                                          |
    //acquire:{                                   |

      bool Thread::acquire(){
        //acquire() must succeed: start thread if it hasn't been already.
        if( !m_pThread ){
          start();
          yield();
        }
        //block until signaled or in other words join the thread.
        if( !isCurrent() ){
          try{
            m_pThread->join();
          }
          catch(...){
          }
          return true;
        }
        return false;
      }

    //}:                                          |
    //release:{                                   |

      void Thread::release(){
        // Do not start in acquire if not already started.
        if( m_pThread ){
          if( !m_tFlags->bIsDead ){
            // Kill the thread at next convenience.
            m_tFlags->bIsDying = 1;
            // Block until thread dies (returns).
            if( acquire() ){
              // This pointer has been deleted in acquire.
              m_pThread = nullptr;
            }
          }
        }else{
          m_tFlags->bIsDead = 1;
        }
      }

    //}:                                          |
    //context:{                                   |

      Thread& Thread::context(){
        if( !__context ){
          static Thread main;
          __context = &main;
        }
        return *__context;
      }

    //}:                                          |
    //exists:{                                    |

      bool Thread::exists( const u32 tid ){
        auto bResult=( tid==__masters );
        if( !bResult ){
          getThreadMap().foreachs(
            [&]( const IThread* pThread ){
              if( static_cast<const Thread*>( pThread )->tid() == tid ){
                bResult = true;
              }
              return!bResult;
            }
          );
        }
        return bResult;
      }

    //}:                                          |
    //yield:{                                     |

      void Thread::yield(){
        std::this_thread::yield();
      }

    //}:                                          |
    //sleep:{                                     |

      void Thread::sleep( const s32 ms ){
        std::this_thread::sleep_for( std::chrono::milliseconds( ms ));
      }

    //}:                                          |
    //start:{                                     |

      void Thread::start(){
        if( !m_pThread ){
          m_pThread = new std::thread( RedWorker, this );
        }
      }

    //}:                                          |
    //count:{                                     |

      s32 Thread::count(){
        return __count;
      }

    //}:                                          |
    //count:{                                     |

      s32 Thread::peak(){
        return __peak;
      }

    //}:                                          |
    //alive:{                                     |

      bool Thread::alive()const{
        for(;;){
          // If the thread is dying return false.
          if( m_tFlags->bIsDying ){
            return false;
          }
          // If the thread is dead return false.
          if( m_tFlags->bIsDead ){
            return false;
          }
          // If thread hasn't started then return false.
          if( !m_pThread ){
            return false;
          }
          // We cannot pause the main thread!
          if( !IEngine::isMainThread() && m_tFlags->bPaused ){
            sleep( 10 );
            continue;
          }
          // Reset scratch memory buffer.
          e_sreset();
          // Thread is still alive!
          return true;
        }
      }

    //}:                                          |
    //uid:{                                       |

      u32 Thread::uid(){
        if( __context ){
          return __context->m_uUID;
        }
        return 0;
      }

    //}:                                          |
    //tid:{                                       |

      u32 Thread::tid(){
        u32 tidResult = 0;
        #if e_compiling( web )
          static __thread u32 tid = 1;
          tidResult = tid++;
        #elif e_compiling( win64 )
          tidResult = GetCurrentThreadId();
        #elif e_compiling( android )
          tidResult = u32( gettid() );
        #elif e_compiling( ios )||e_compiling( osx )
          tidResult = u32( pthread_mach_thread_np( pthread_self() ));
        #elif e_compiling( linux )
          #ifdef SYS_gettid
            tidResult = u32( syscall( SYS_gettid ));
          #endif
        #else
          pthread_id_np_t curId;
          pthread_getunique_np( &pthread_self(), &curId );
          tidResult = u32( curId );
        #endif
        return tidResult;
      }

    //}:                                          |
  //}:                                            |
  //Ctor:{                                        |

    Thread::Thread( Thread&& rvalue     )
        : m_pPayload( rvalue.m_pPayload )
        , m_pThread(  rvalue.m_pThread  )
        , m_uTID(     rvalue.m_uTID     )
        , m_uUID(     rvalue.m_uUID     )
        , m_tFlags(   rvalue.m_tFlags   ){
      rvalue.m_pPayload    = nullptr;
      rvalue.m_pThread     = nullptr;
      rvalue.m_tFlags.all  = 0;
      rvalue.m_uUID        = 0;
      rvalue.m_uTID        = 0;
    }

    Thread::Thread( const Lambda& lambda )
      : m_onLambda( lambda )
    {}

    Thread::Thread( std::thread* pThread )
        : m_pPayload( nullptr )
        , m_pThread( pThread )
        , m_uTID( tid() )
        , m_uUID( makeUid() ){
      m_tFlags->bCurrent = 1;
    }

    Thread::Thread( const Thread& thread )
        : m_uUID( makeUid() ){
      *this = thread;
    }

    Thread::Thread()
      : m_uUID( makeUid() )
    {}

  //}:                                            |
  //Dtor:{                                        |

    Thread::~Thread(){
      __masks.reset( m_uUID );
      release();
      m_onLambda = nullptr;
    }

  //}:                                            |
#else
  //Global:{                                      |

    void e_backoff( const double ){}

  //}:                                            |
#endif
//}:                                              |
//================================================+=============================
