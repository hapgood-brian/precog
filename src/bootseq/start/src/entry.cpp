//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if !e_compiling( web )
  #include<boost/filesystem.hpp>
//#include<hash/MurmurHash3.h>
  #if e_compiling( osx )
    #include<sys/sysctl.h>
    #include<sys/stat.h>
    #include<unistd.h>
    #include<signal.h>
  #elif e_compiling( win64 )
    extern"C"{
      bool        __stdcall CreateDirectoryA( const char*, void* );
      const char* __stdcall GetCommandLineA();
    }
  #endif
  #include<sstream>
#endif

using namespace EON;
using namespace gfc;

//================================================+=============================
//IEngine:{                                       |
  //Methods:{                                     |
    //[app]:{                                     |
      //main:{                                    |

        /* Define entry points for Windows and Android */

        #if e_compiling( android )
          namespace{
            vp s_pAndroidNativeState = nullptr;
          }
          void android_main( android_app* state ){
            // Save off the android native state.
            s_pAndroidNativeState = state;
            // Get the package path; can't do anything if it's null.
            if( !e_getCvar( bool, "COMPILE_EDITOR_PACKAGE" )){
              const string& streamPath = IEngine::toStreamPath();
              if( !streamPath.empty() ){
                // Load all fablets and store locally for lifetime of program.
                IEngine::fablets = Prefab::get( streamPath );
              }
            }
            // Finally we can enter the application entry-point.
            IEngine::main( IEngine::args );
          }
        #elif e_compiling( win64 )
          int __stdcall WinMain( vp, vp, ccp, int ){
            // Before we can do anything with the engine we must load the args.
            { std::stringstream ss( GetCommandLineA() );
              std::string arg;
              while( ss >> arg ){
                string a( arg.c_str() );
                a.replace( "\\", "/" );
                if( *a == '"' ){
                  a.ltrim( 1 );
                  a.trim( 1 );
                }
                IEngine::args.push( a );
              }
            }
            IEngine::args.alter( 0,
              [&]( string& arg ){
                arg.replace( "\\", "/" );
              }
            );
            // Get the package path; can't do anything if it's null.
            if( !e_getCvar( bool, "COMPILE_EDITOR_PACKAGE" )){
              const string& streamPath = IEngine::toStreamPath();
              if( !streamPath.empty() ){
                // Load all fablets and store locally for lifetime of program.
                IEngine::fablets = Prefab::get( streamPath );
              }
            }
            // Run the game.
            return IEngine::main( IEngine::args );
          }
        #endif

        /* Define entry point for all the rest */

        int main( s32 argc, cp argv[] ){

          //--------------------------------------------------------------------
          // Setup the abort hook; so I can redirect to the e_break() function.
          //--------------------------------------------------------------------

          #if e_compiling( osx )
            static const auto& onAbort=[]( int signum ){
              e_break( e_xfs( "ABORT: %d - Caught SIGABRT signal!", signum ));
            }; signal( SIGABRT, onAbort );
          #endif

          //--------------------------------------------------------------------
          // First of all let's construct our string pairs of env vars.
          //--------------------------------------------------------------------

          // Before we can do anything with the engine we must load the args.
          for( s32 i=0; i<argc; ++i ){
            const string a( argv[ i ]);

            // Change cvars on the command line.
            if( a.left( 7 ).tolower().hash() == "--cvar="_64 ){
              const auto cvarName = a.ltrimmed( 7 );
              e_setCvar( cvarName, true );
              continue;
            }

            // Pass everything else to application.
            IEngine::args.push( a );
          }

          // Alter the arguments by changing path separators.
          #if e_compiling( win64 )
            IEngine::args.alter( 0,
              [&]( string& arg ){
                arg.replace( "\\", "/" );
              }
            );
          #endif

          // Run the game.
          return IEngine::main( IEngine::args );
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
