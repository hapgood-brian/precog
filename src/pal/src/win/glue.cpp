//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRALEAN
#define VC_EXTRALEAN
#define NOMINMAX

#include<windows.h>
#include<Shlobj.h>
#include<direct.h>
#include<eon/eon.h>

using namespace EON;
using namespace gfc;

namespace{
  u32    s_uMainTID = Thread::tid();
  string s_sFallbackDir;
  string s_sPackagePath;
  char   s_aTemp4K[4096];
  string s_stdErr;
  string s_stdOut;
}

//================================================|=============================
//IEngine:{                                       |
  //Externs:{                                     |

    extern bool g_bTerminate;
    extern f32  g_fScale;
    extern f32  g_fH;
    extern f32  g_fW;

  //}:                                            |
  //Private:{                                     |

    namespace{
      deque<std::function<void()>> anon_dLambdas;
      pt2 anon_tCursor;
    }

  //}:                                            |
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
            s_sPackagePath = dir;
            while( s_sPackagePath.back() == '/' ){
              s_sPackagePath.trim( 1 );
            }
          }
        }

      //}:                                        |
      //toStreamPath:{                            |

        string IEngine::toStreamPath(){
          string bundlePath = toBundlePath();
          bundlePath += "Contents/Streams/";
          if( dexists( bundlePath )){
            return bundlePath;
          }
          return nullptr;
        }

      //}:                                        |
      //toPackagePath:{                           |

        string IEngine::toPackagePath(){
          if( s_sPackagePath.empty() ){
            return string( "~/" );
          }
          return s_sPackagePath + "/";
        }

      //}:                                        |
      //toHelpersPath:{                           |

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

        string IEngine::toBundlePath(){
          static string s_sBundlePath;
          if( s_sBundlePath.empty() ){
            s_sBundlePath = args[ 0 ].path() + "/";
            s_sBundlePath.replace( "\\", "/" );
            s_sBundlePath.replace( "//", "/" );
          }
          return s_sBundlePath;
        }

      //}:                                        |
      //toEonPath:{                               |

        string IEngine::toEonPath(){
          string eonPath;
          if( !s_sPackagePath.empty() ){
            if(( '.' == *s_sPackagePath ) && ( '/' == s_sPackagePath[ 1 ])){
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
          try{
            ::exit( 0 );
          }
          catch(...){
          }
        }

      //}:                                        |
    //}:                                          |
    //[ipc]:{                                     |
      //unshare:{                                 |

        bool IEngine::unshare( const string& key ){
          return false;
        }

      //}:                                        |
      //ssync:{                                   |

        bool IEngine::ssync( const string& key ){
          return false;
        }

      //}:                                        |
      //share:{                                   |

        cp IEngine::share( const gfc::string& path, const u64 req_bytes, const bool bServer ){
          return nullptr;
        }

      //}:                                        |
    //}:                                          |
    //[io]:{                                      |
      //tempPath:{                                |

        string IEngine::tempPath(){
          static string s_sTempPath;
          if( s_sTempPath.empty() ){
            GetTempPathA( e_dimof( s_aTemp4K ), s_aTemp4K );
            s_sTempPath  = string( s_aTemp4K ).os() + "\\";
            s_sTempPath.replace( "\\", "/" );
            s_sTempPath.replace( "//", "/" );
          }
          return s_sTempPath;
        }

      //}:                                        |
      //homePath:{                                |

        string IEngine::homePath(){
          static string s_sHomePath;
          if( s_sHomePath.empty() ){
            SHGetFolderPathA( NULL, CSIDL_PROFILE, NULL, 0, s_aTemp4K );
            string path = string( s_aTemp4K ) + '/';
            path.replace( "\\", "/" );
            path.replace( "//", "/" );
            s_sHomePath = path;
          }
          return s_sHomePath;
        }

      //}:                                        |
      //lexists:{                                 |

        bool IEngine::lexists( const string& path ){
          //TODO: Add support for junctions.
          return false;
        }

      //}:                                        |
      //dexists:{                                 |

        bool IEngine::dexists( const string& path ){
          DWORD dwAttrib = GetFileAttributes( path.os() );
          return( dwAttrib != INVALID_FILE_ATTRIBUTES )&&( dwAttrib & FILE_ATTRIBUTE_DIRECTORY );
        }

      //}:                                        |
      //fexists:{                                 |

        bool IEngine::fexists( const string& path ){
          DWORD dwAttrib = GetFileAttributes( path.os() );
          if( dwAttrib == INVALID_FILE_ATTRIBUTES )
            return false;
          if( dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
            return false;
          return true;
        }

      //}:                                        |
      //fopen:{                                   |

        FILE* IEngine::fopen( const string& path, ccp mode ){
          FILE* pFile = nullptr;
          const auto osPath = path.os();
          const auto e = fopen_s( &pFile, osPath, mode );
          if( !e ){
            return pFile;
          }
          return nullptr;
        }

      //}:                                        |
      //dir:{                                     |

        bool IEngine::dir( const string& path
            , const std::function<bool( const string& dir
            , const string& name
            , const bool bIsDirectory )>& lambda ){
          WIN32_FIND_DATA fd;
          HANDLE hFind = FindFirstFile(( path + "/*.*" ).os(), &fd );
          if( hFind == INVALID_HANDLE_VALUE )
            return false;
          do{
            const bool bIsDirectory=( 0 != ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ));
            if( bIsDirectory ){
              if(( *fd.cFileName != '.' ) && strcmp( fd.cFileName, ".." )){
                string out( path + "\\" + fd.cFileName );
                out.replace( "\\", "/" );
                if( !lambda( out.path(), out.filename(), true ))
                  return true;
                dir( out.path() + fd.cFileName, lambda );
              }
            }else{
              string out( path + "\\" + fd.cFileName );
              out.replace( "\\", "/" );
              if( !lambda( out.path(), out.filename(), false ))
                return true;
            }
          } while( FindNextFile( hFind, &fd ));
          if( GetLastError() != ERROR_NO_MORE_FILES ){
            FindClose( hFind );
            return false;
          }
          FindClose( hFind );
          return true;
        }

      //}:                                        |
    //}:                                          |
    //runOnMainThread:{                           |

      void IEngine::runOnMainThread( const std::function<void()>& lambda ){}

    //}:                                          |
    //isMainThread:{                              |

      bool IEngine::isMainThread(){
        return( s_uMainTID == Thread::tid() );
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

      bool IEngine::system( const string& cmd
          , const strings& cvArgs
          , const std::function<void()>& lambda ){
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
          new std::thread(
            [=](){
              syscall();
              lambda();
            }
          );
        }else{
          syscall();
        }
        return true;
      }

    //}:                                          |
    //spawn:{                                     |

      bool IEngine::spawn( const string& program
          , const strings& userArgs
          , const bool bBlocking
          , const std::function<void( const s32 )>& lambda ){
        if( !IEngine::isMainThread() ){
          return false;
        }
        const auto& onSpawn=[=]()->int{
          strings args;
          args.push( program );
          args.pushVector( userArgs );
          const int result = IPlugin::spawn( args );
          if( lambda ){
            lambda( result );
          }
          return result;
        };
        if( bBlocking ){
          return !onSpawn();
        }
        e_runAsync( onSpawn );
        return true;
      }

    //}:                                          |
    //exec:{                                      |

      bool IEngine::exec( const string& program
          , const strings& userArgs
          , const bool bBlocking
          , const std::function<void( const s32 )>& lambda ){
        // See also https://support.win64.com/en-us/kb/190351.
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        memset( &si, 0, sizeof( si ));
        memset( &pi, 0, sizeof( pi ));
        si.cb = sizeof( si );
        string arg;
        args.foreach( [&]( const string& s ){
          arg += " ";
          arg += s;
        });
        if( !CreateProcess(
              program,
              LPSTR( arg.c_str() ),
              NULL,
              NULL,
              FALSE,
              IsDebuggerPresent() ? 0 : CREATE_NO_WINDOW,
              NULL,
              NULL,
              &si,
              &pi )){
          #if e_compiling( debug )
            const DWORD dwError = GetLastError();
            if( dwError == ERROR_FILE_NOT_FOUND ){
              return false;
            }
            DEBUG_BREAK
          #endif
          return false;
        }
        if( bBlocking ){
          WaitForSingleObject( pi.hProcess, INFINITE );
          CloseHandle( pi.hProcess );
          CloseHandle( pi.hThread );
          if( lambda ){
            lambda( 0 );
          }
        }else{
          new std::thread( [=](){
            while( WAIT_OBJECT_0 != WaitForSingleObject( pi.hProcess, 0 )){
              Thread::yield();
            }
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
            if( lambda ){
              lambda( 0 );
            }
          });
        }
        return true;
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================|=============================
