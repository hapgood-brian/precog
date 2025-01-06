//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#include<unistd.h>
#include<dirent.h>
#define _SVID_SOURCE
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>
#include<errno.h>

using namespace EON;
using namespace gfc;

using OnOpenCompletion = std::function<void( const strings& paths )>;
using OnSaveCompletion = std::function<void( const string&  path )>;
using OnCancel         = std::function<void()>;
using OnOK             = std::function<void()>;

#define USE_METAL1 0
#define USE_METAL2 0
#define USE_METAL3 0
#define USE_OPENGL 0

//================================================|=============================
//Glue data:{                                     |
  //Structs:{                                     |
    //ShmInfo:{                                   |

      class ShmInfo{
        e_var_ptr( char, Ptr  ) = nullptr;
        e_var_string(    Key  );
        e_var( u64,   u, Size ) = 0;
        e_var( s32,   s, Shm  ) =-1;
        e_var_bits(      Flags
          , bServer:1
        );
      };

    //}:                                          |
  //}:                                            |
  //Globals:{                                     |

    bool g_bTerminate = false;
    f32  g_fScale     = 0.f;
    f32  g_fW         = 0.f;
    f32  g_fH         = 0.f;

  //}:                                            |
  //Private:{                                     |

    namespace{
      hashmap<u64,ShmInfo> s_mSharedMemory;
      OnOpenCompletion     s_onOpenCompletion;
      OnSaveCompletion     s_onSaveCompletion;
      OnCancel             s_onCancel;
      OnOK                 s_onOK;
      string               s_sPackagePath;
      string               s_sTitle;
      string               s_stdOut;
      string               s_stdErr;
    }

  //}:                                            |
//}:                                              |
//================================================|=============================
//                                                :
//                                                :
//                                                :
//================================================|=============================
//IEngine:{                                       |
  //Methods:{                                     |
    //[paths]:{                                   |
      //setPackagePath:{                          |

#ifdef __APPLE__
  #pragma mark - IEngine methods -
#endif

        void IEngine::setPackagePath( const string& dir ){
          if( dir.empty() ){
            s_sPackagePath.clear();
          }else{
            s_sPackagePath = dir.os();
            while( s_sPackagePath.back() == '/' ){
              s_sPackagePath.trim( 1 );
            }
          }
        }

      //}:                                        |
      //toStreamPath:{                          |

        string IEngine::toStreamPath(){
          string bundlePath = toBundlePath();
          bundlePath += "Contents/Streams/";
          return bundlePath;
        }

      //}:                                        |
      //toPackagePath:{                           |

        string IEngine::toPackagePath(){
          if( s_sPackagePath.empty() ){
            return string( "~/" ).os();
          }
          return s_sPackagePath + "/";
        }

      //}:                                        |
      //toHelpersPath:{                           |

        //https://developer.apple.com/library/mac/technotes/tn2206/_index.html#//apple_ref/doc/uid/DTS40007919-CH1-TNTAG20

        string IEngine::toHelpersPath(){
          return toBundlePath() + "Contents/Helpers/";
        }

      //}:                                        |
      //toPluginsPath:{                           |

        string IEngine::toPluginsPath(){
          return toBundlePath() + "Contents/PlugIns/";
        }

      //}:                                        |
      //toBundlePath:{                            |

        //https://developer.apple.com/library/mac/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html#//apple_ref/doc/uid/10000123i-CH101-SW1
        //https://developer.apple.com/library/mac/technotes/tn2206/_index.html#//apple_ref/doc/uid/DTS40007919-CH1-TNTAG20

        string IEngine::toBundlePath(){
          static char bundlePath[ 384 ]={ 0 };
          if( !*bundlePath ){
            strcpy( bundlePath, "~/.eon/bundles" );
            strcat( bundlePath, "/" );
          }
          return bundlePath;
        }

      //}:                                        |
      //toEonPath:{                               |

        string IEngine::toEonPath(){
          string eonPath;
          if( !s_sPackagePath.empty() ){
            if(( '.' == *s_sPackagePath )&&( '/' == s_sPackagePath[ 1 ])){
              eonPath += toStreamPath();
              eonPath += s_sPackagePath;
            }else{
              eonPath += s_sPackagePath;
            }
            eonPath += "/.eon/";
          }else{
            eonPath = toStreamPath();
          }
          return eonPath;
        }

      //}:                                        |
      //exit:{                                    |

        void IEngine::exit(){
          //[NSApp terminate: nil];
          ::exit( 0 );
        }

      //}:                                        |
    //}:                                          |
    //[ipc]:{                                     |
      //unshare:{                                 |

        bool IEngine::unshare( const string& key ){
          // TODO: Implement for Linux here.
          return false;
        }

      //}:                                        |
      //ssync:{                                   |

        bool IEngine::ssync( const string& key ){
          // TODO: Implement for Linux here.
          return false;
        }

      //}:                                        |
      //share:{                                   |

        //http://www.cse.psu.edu/~deh25/cmpsc473/notes/OSC/Processes/shm-posix-producer.c
        //http://www.cse.psu.edu/~deh25/cmpsc473/notes/OSC/Processes/shm-posix-consumer.c
        //http://www.cse.psu.edu/~deh25/cmpsc473/notes/OSC/Processes/shm.html

        cp IEngine::share( const gfc::string& path, const u64 req_bytes, const bool bServer ){
          // TODO: Implement for Linux here.
          return nullptr;
        }

      //}:                                        |
    //}:                                          |
    //[i/o]:{                                     |
      //tempPath:{                                |

        string IEngine::tempPath(){
          return "/tmp";
        }

      //}:                                        |
      //homePath:{                                |

        string IEngine::homePath(){
          static string s_sHomePath;
          if( s_sHomePath.empty() )
            s_sHomePath = e_getenv( "HOME" ) + "/";
          return s_sHomePath;
        }

      //}:                                        |
      //dexists:{                                 |

        bool IEngine::dexists( const string& path ){
          struct stat statbuf;
          if( !stat( path, &statbuf )){
            if( S_ISDIR( statbuf.st_mode )){
              return true;
            }
          }
          return false;
        }

      //}:                                        |
      //lexists:{                                 |

        bool IEngine::lexists( const string& path ){
          struct stat st;
          const auto x = stat( path, &st );
          if( S_ISLNK( st.st_mode ))
            return true;
          return false;
        }

      //}:                                        |
      //fexists:{                                 |

        bool IEngine::fexists( const string& path ){
          struct stat statbuf;
          if( !access( path, F_OK )){
            return true;
          }
          return false;
        }

      //}:                                        |
      //osVersion:{                               |

        string IEngine::osVersion(){
          return nullptr;
        }

      //}:                                        |
      //fopen:{                                   |

        FILE* IEngine::fopen( const string& in_path, ccp mode ){
          const string& path = in_path.os();

          //--------------------------------------------------------------------
          // Recursively follow symbolic links. https://stackoverflow.com/questions/5525668/how-to-implement-readlink-to-find-the-path
          //--------------------------------------------------------------------

          struct stat st;
          lstat( path, &st );
          if( S_ISLNK( st.st_mode )){
            char buf[ PATH_MAX ];
            ssize_t len = readlink( path, buf, sizeof( buf )-1 );
            if( len == -1 ){
              return nullptr;
            }
            buf[ len ]=0;
            auto* f = fopen( s_sPackagePath + "/" + buf, mode );
            if( !f ){
              f = fopen( buf, mode );
            }
            return f;
          }

          //--------------------------------------------------------------------
          // Create write dirctory and open the file.
          //--------------------------------------------------------------------

          return::fopen( path.os(), mode );
        }

      //}:                                        |
      //dir:{                                     |

        bool IEngine::dir( const string& cPath
            , const std::function<bool( const string&
            , const string&
            , const bool )>& lambda ){
          if( cPath.empty() )
            return false;
          string path = cPath;
          if( '/' != *path.right( 1 ))
            path += '/';
          DIR* D = opendir( path.os() );
          if( !D )
            return false;
          dirent* ent;
          while(( ent = readdir( D )) != nullptr ){
            const auto& subpath = path + ent->d_name;
            if( e_lexists( subpath ))
              continue;
            auto* tmp = opendir( subpath );
            if( tmp ){
              if(( *ent->d_name != '.' )&&( *ent->d_name != '_' )){
                if( !lambda( path, ent->d_name, true ))
                  return true;
                dir( subpath, lambda );
              }
              closedir( tmp );
            }else if( !lambda( path
                , ent->d_name
                , false )){
              return true;
            }
          }
          closedir( D );
          return true;
        }

      //}:                                        |
    //}:                                          |
    //runOnMainThread:{                           |

      void IEngine::runOnMainThread( const std::function<void()>& lambda ){
        // TODO: Implement for Linux please.
      }

    //}:                                          |
    //isMainThread:{                              |

      bool IEngine::isMainThread(){
        // TODO: Implement for Linux please.
        return false;
      }

    //}:                                          |
    //stderr:{                                    |

      string& IEngine::getStderr(){
        return s_stdErr;
      }

    //}:                                          |
    //stdout:{                                    |

      string& IEngine::getStdout(){
        return s_stdOut;
      }

    //}:                                          |
    //system:{                                    |

      bool IEngine::system( const string& cmd, const strings& cvArgs, const std::function<void()>& lambda ){
        const auto syscall=[=](){
          string arg;
          cvArgs.foreach( [&]( const string& cArg ){
            string s( cArg );
            s.replace( " ", "\\ " );
            arg += " ";
            arg += s;
          });
          system( e_xfs( "%s %s ", cmd.c_str(), arg.c_str() ));
        };
        if( lambda ){
          std::thread( [=](){
            syscall();
            lambda();
          });
        }else{
          syscall();
        }
        return true;
      }

    //}:                                          |
    //spawn:{                                     |

      bool IEngine::spawn( const string& program, const strings& userArgs, const bool bBlocking, const std::function<void( const s32 )>& lambda ){
        const auto& onSpawn=[=]()->int{
          strings args;
          args.push( program );
          args.pushVector( userArgs );
          const int result = IPlugin::spawn( args );
          if( lambda )
              lambda( result );
          return result;
        };
        if( bBlocking )
          return !onSpawn();
        e_runAsync( onSpawn );
        return true;
      }

    //}:                                          |
    //exec:{                                      |

      bool IEngine::exec( const string& program, const strings& userArgs, const bool bBlocking, const std::function<void( const s32 )>& lambda ){

        //----------------------------------------------------------------------
        // Create standard error and output pipes.
        //----------------------------------------------------------------------

        s32    STDOUT[ 2 ]{};
        s32    STDERR[ 2 ]{};
        pipe ( STDOUT );
        pipe ( STDERR );

        //----------------------------------------------------------------------
        // System call lambda does hard work of launching program in "program".
        //----------------------------------------------------------------------

        const auto& syscall=[=]( const bool is_blocking )->s32{

          u32 retryIndex = 0;
          pid_t processId;

          //--------------------------------------------------------------------
          // This code runs in the child (forked) process.
          //--------------------------------------------------------------------

          retry:processId = fork();
          if( ! processId ){

            // Build argument list to send to execv.
            std::vector<ccp> argv;
            ccp pProgram = strdup( program );
            argv.push_back( pProgram );
            string args;
            for( u32 n=userArgs.size(), i=0; i<n; ++i ){
              ccp pArg = strdup( userArgs[ i ]);
              args += pArg;
              if( i + 1 < n ){
                args += " ";
              }
              argv.push_back( pArg );
            }
            argv.push_back( nullptr );
            e_logf( "Spawning: %s %s", program.c_str(), args.c_str() );

            // Close read end of the pipe.
            close( STDOUT[ 0 ]);
            close( STDERR[ 0 ]);

            // Redirect stdout and stderr.
            dup2( STDOUT[ 1 ], STDOUT_FILENO );
            dup2( STDERR[ 1 ], STDERR_FILENO );

            // Close write end of the pipe.
            close( STDOUT[ 1 ]);
            close( STDOUT[ 1 ]);

            // Execute the child process.
            const s32 err = execv( program, (char*const*)&argv[ 0 ]);
            if( err < 0 ){
              perror( "execv error" );
              return -1;
            }
            return 0;
          }

          //--------------------------------------------------------------------
          // This code runs if the fork failed.
          //--------------------------------------------------------------------

          if( processId < 0 ){
            perror( "fork error" );
            return -2;
          }

          //--------------------------------------------------------------------
          // This code runs in the parent process.
          //--------------------------------------------------------------------

          if( is_blocking ){
            static const u32 bufferSize = 64;
            char buffer[ bufferSize ];
            close( STDOUT[ 1 ]);
            close( STDERR[ 1 ]);
            for(;;){

              //----------------------------------------------------------------
              // Read from standard error after syscall.
              //----------------------------------------------------------------

              struct stat st;
              fstat( STDERR[ 0 ], &st );
              if( st.st_size > 0 ){
                s64 z = st.st_size;
                while( z > 0 ){
                  const s64 n = read( STDERR[ 0 ], buffer, bufferSize );
                  if( n ){
                    s_stdOut.cat( buffer, n );
                    z -= n;
                  }
                }
              }

              //----------------------------------------------------------------
              // Read from standard error after syscall.
              //----------------------------------------------------------------

              fstat( STDOUT[ 0 ], &st );
              if( st.st_size > 0 ){
                s64 z = st.st_size;
                while( z > 0 ){
                  const s64 n = read( STDOUT[ 0 ], buffer, bufferSize );
                  if( n ){
                    s_stdOut.cat( buffer, n );
                    z -= n;
                  }
                }
              }

              //----------------------------------------------------------------
              // Handle termination and signalling!
              //----------------------------------------------------------------

              const int status = 0;
              if( WIFSIGNALED( status )){
                if( ++retryIndex > 4 ){
                  e_logf( "%s is hopelessly broken: fix it and try again", program.c_str() );
                  close( STDERR[ 0 ]);
                  close( STDOUT[ 0 ]);
                  return -3;
                }
                e_logf( "Child process crashed: retrying (%u)", retryIndex );
                goto retry;
              }
              if( WIFEXITED( status )){
                close( STDERR[ 0 ]);
                close( STDOUT[ 0 ]);
                return WEXITSTATUS( status );
              }
              usleep( 33000 );
            }
          }
          return -4;
        };

        //----------------------------------------------------------------------
        // Though fork+exec is naturally asynchronous we must invoke the lambda
        // when the process finishes. If this is a non-blocking spawn() request
        // then we'll launch a background thread to execute the system call and
        // invoke the callback when it completes, otherwise we'll just run that
        // callback immediately.
        //----------------------------------------------------------------------

        Thread* pThread = new Thread( [=](){
          const auto retcode = syscall( true );
          if( lambda )
              lambda( retcode );
        });
        if( !bBlocking ){
          pThread->autodelete()->start();
        }else{
          pThread->acquire();
          delete pThread;
        }
        return true;
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================|=============================
//                                                                vim: ft=objcpp
