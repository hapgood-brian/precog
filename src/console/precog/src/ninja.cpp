//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#include"generators.h"
#include"luacore.h"
#include"std.h"
#include"ws.h"
#include<regex>

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================+=============================
//Extends:{                                       |

#ifdef __APPLE__
  #pragma mark - Extensions -
#endif

  e_extends( Workspace::Ninja );

//}:                                              |
//Methods:{                                       |
  //[project]:{                                   |
    //extFromEnum:{                               |

      ccp Workspace::Ninja::extFromEnum( const Type e )const{
        switch( e ){
          case decltype( e )::kCpp:
            return".cpp";
          case decltype( e )::kC:
            return".c";
          default:
            return"";
        }
      }

    //}:                                          |
    //sortingHat:{                                |

      bool Workspace::Ninja::sortingHat( const string& in_path ){

        //----------------------------------------------------------------------
        // Gather together all the sources and store them off. Ignore them at
        // the end of the function.
        //----------------------------------------------------------------------

        File fi;
             fi.setWhere( in_path );
             fi = in_path;
        const auto& ext = fi.ext().tolower();
        switch( ext.hash() ){

          //--------------------------------------------------------------------
          // Platform specific file types.
          //--------------------------------------------------------------------

          case ".fablet"_64:
            inSources( Type::kPrefab ).push( fi );
            break;
          case ".eon"_64:
            inSources( Type::kEon ).push( fi );
            break;
          case ".a"_64:
            inSources( Type::kStaticlib ).push( fi );
            break;

          //--------------------------------------------------------------------
          // Source and header file types.
          //--------------------------------------------------------------------

          case ".inl"_64:/**/{
            auto& inl = inSources( Type::kInl );
            if( ! inl.find( fi )){
              inl.push( fi );
            }
            break;
          }
          case ".hpp"_64:
          case ".hxx"_64:
          case ".hh"_64:/**/{
            auto& hpp = inSources( Type::kHpp );
            if( ! hpp.find( fi )){
              hpp.push( fi );
            }
            break;
          }
          case ".cpp"_64:
          case ".cxx"_64:
          case ".cc"_64:/**/{
            auto& cpp = inSources( Type::kCpp );
            if( ! cpp.find( fi )){
              cpp.push( fi );
            }
            break;
          }
          case ".h"_64:/**/{
            auto& h = inSources( Type::kH );
            if( ! h.find( fi )){
              h.push( fi );
            }
            break;
          }
          case ".c"_64:/**/{
            auto& c = inSources( Type::kC );
            if( ! c.find( fi )){
              c.push( fi );
            }
            break;
          }
          default:
            return false;
        }
        return true;
      }

    //}:                                          |
    //serializeCrossPlatformTarget:{              |

      void Workspace::Ninja::serializeCrossPlatformTarget( string& cxx )const{
        // https://stackoverflow.com/questions/4390752/what-arm-architectures-does-llvm-support
        // https://clang.llvm.org/docs/CrossCompilation.html
        // https://mcilloni.ovh/2021/02/09/cxx-cross-clang/
        if( !bmp->bCrossCompile )
          return;
        // Build x-platform TRIPLE <arch><sub>-<vendor>-<sys>-<env>
        cxx << " -target ";

        //----------------------------------------------------------------------
        // <arch>
        //----------------------------------------------------------------------

        if( crossCc.find( "x86_64" )){
          cxx << "x86_64";
        }else if(( crossCc.find( "i386" ))||
                   crossCc.find( "x86" )){
          cxx << "i386";
        }else if( crossCc.find( "mips" )){
          cxx << "mips";
        }else if( crossCc.find( "arm" )){
          cxx << "arm";
        }else{
          if( cpu.hash() == "x86_64"_64 ){
            cxx << "x86_64";
          }else if( cpu.hash() == "arm64"_64 ){
            cxx << "arm";
          }
        }

        //----------------------------------------------------------------------
        // <sub>
        //----------------------------------------------------------------------

        if( crossCc.find( "arm" )){
          const ccp subs[]{
            "64", "v4t", "v5t", "v5te", "v6", "v6m", "v6t2", "v7a", "v7m",
          };
          const auto n = e_sizeof( subs );
          for( auto i=0u; i<n; ++i ){
            if( !crossCc.find( subs[ i ]))
              continue;
            cxx << subs[ i ];
            break;
          }
        }

        //----------------------------------------------------------------------
        // <vendor>
        //----------------------------------------------------------------------

        if( crossCc.find( "apple" )){
          cxx << "-apple";
        }else if( crossCc.find( "nvidia" )){
          cxx << "-nvidia";
        }else if( crossCc.find( "pc" )){
          cxx << "-pc";
        }

        //----------------------------------------------------------------------
        // <sys>
        //----------------------------------------------------------------------

        if( crossCc.find( "none" )){
          cxx << "-none";
        }else if( crossCc.find( "freebsd" )){
          cxx << "-freebsd";
        }else if( crossCc.find( "win32" )){
          cxx << "-win32";
        }else if( crossCc.find( "darwin" )){
          cxx << "-darwin";
        }else if( crossCc.find( "cuda" )){
          cxx << "-cuda";
        }else if( crossCc.find( "linux" )){
          cxx << "-linux";
        }

        //----------------------------------------------------------------------
        // <env>
        //----------------------------------------------------------------------

        if( crossCc.find( "eabi" )){
          cxx << "-eabi";
        }else if( crossCc.find( "gnu" )){
          cxx << "-gnu";
        }else if( crossCc.find( "android" )){
          cxx << "-android";
        }else if( crossCc.find( "macho" )){
          cxx << "-macho";
        }else if( crossCc.find( "elf" )){
          cxx << "-elf";
        }
      }

    //}:                                          |
    //serializeCrossPlatformRules:{               |

      void Workspace::Ninja::serializeCrossPlatformRules( string& cxx )const{

        //----------------------------------------------------------------------
        // Figure out what platform we're on.
        //----------------------------------------------------------------------

        if( toLabel().empty() )
          e_break( "Empty label" );
        if( bmp->bExtMacho ){
          cxx << "rule MACHO_LINKER_" << toLabel().toupper() + "\n";
        }else if( bmp->bExtElf ){
          cxx << "rule ELF_LINKER_" << toLabel().toupper() + "\n";
        }else if( bmp->bExtPE ){
          cxx << "rule PE_LINKER_" << toLabel().toupper() + "\n";
        }else if( bmp->bCrossCompile ){
          if( crossCc.find( "linux" )){
            cxx << "rule ELF_LINKER_" << toLabel().toupper() + "\n";
            return;
          }
          if( crossCc.find( "apple" )){
            cxx << "rule MACHO_LINKER_" << toLabel().toupper() + "\n";
            return;
          }
          if( crossCc.find( "pc" )){
            cxx << "rule PE_LINKER_" << toLabel().toupper() + "\n";
            return;
          }
          e_break( "Unknown platform: cross compile with option \"-x\"!" );
        }
      }

    //}:                                          |
    //serialize:{                                 |

      void Workspace::Ninja::serialize( Writer& fs )const{

        //----------------------------------------------------------------------
        // Ignore files.
        //----------------------------------------------------------------------

        const auto& onIgnore = [this]( vector<File>::iterator it ){
          while( it ){
            auto ok = false;
            { auto parts = toIgnoreParts();
              parts.erase( "\n" );
              parts.erase( "\t" );
              parts.erase( " " );
              const auto& splits = parts.splitAtCommas();
              splits.foreachs(
                [&]( const string& split ){
                  if( isIgnored( split, *it )){
                    e_msgf( "  Ignoring %s", ccp( it->filename() ));
                    ok = true;
                    return false;
                  }
                  return true;
                }
              );
            }
            if( ok ){
              it.erase();
              continue;
            }
            ++it;
          }
        };
        onIgnore( const_cast<Ninja*>( this )
          -> inSources( Type::kCpp ).getIterator() );
        onIgnore( const_cast<Ninja*>( this )
          -> inSources( Type::kC ).getIterator() );

        //----------------------------------------------------------------------
        // Populate build files across unity space.
        //----------------------------------------------------------------------

        const auto isUnity = isUnityBuild();
        if( !isUnity||!Workspace::bmp->bUnity ){
          writeProject<Ninja>( fs, Type::kCpp );
          writeProject<Ninja>( fs, Type::kC );
        }else{
          const auto cores = u32(
            std::thread::hardware_concurrency() );
          auto i=0u;
          const_cast<Ninja*>( this )->toUnity().resize( cores );
          const_cast<Ninja*>( this )->unifyProject<Ninja>( Type::kCpp, i );
          const_cast<Ninja*>( this )->unifyProject<Ninja>( Type::kC,   i );
          writeProject<Ninja>( fs, Type::kCpp );
          writeProject<Ninja>( fs, Type::kC );
        }

        //----------------------------------------------------------------------
        // Create CFLAGS variable in build ninja.
        //----------------------------------------------------------------------

        const auto clabel = toLabel().toupper() + "_CFLAGS";
        const auto cstart = clabel + " = ";
        string cflags = cstart;
        if( bmp->bWasm ){
          if( e_getCvar( bool, "ENABLE_PTHREADS" )){
            cflags << "-O3 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -s PROXY_TO_PTHREAD";
          }else{
            cflags << "-O3";
          }
        }else{
          cflags << "-O3";
        }
        if( !toIncludePaths().empty() ){
          const auto& includePaths = toIncludePaths().splitAtCommas();
          includePaths.foreach(
            [&]( const string& path ){
              if( path.empty() ){
                return;
              }
              if(( *path != '/' ) && ( *path != '~' ) && ( *path != '.' )){
                cflags << " -I../" << path;
              }else{
                cflags << " -I" << path;
              }
            }
          );
        }
        if( !toDefinesRel().empty() ){
          const auto& defines = toDefinesRel().splitAtCommas();
          defines.foreach(
            [&]( const string& define ){
              cflags << " -D" << define;
            }
          );
        }
        const auto& prefix = toPrefixHeader();
        if( !prefix.empty() )
          cflags << " -include ../" << prefix;
        switch( toBuild().tolower().hash() ){
          case"shared"_64:
            cflags << " -fPIC";
            break;
        }
        serializeCrossPlatformTarget( cflags );
        if( cstart != cflags )
          fs << cflags << "\n";

        //----------------------------------------------------------------------
        // Create LFLAGS variable in build ninja.
        //----------------------------------------------------------------------

        const auto llabel = toLabel().toupper() + "_LFLAGS";
        const auto lstart = llabel + " =";
        string lflags = lstart;
        if( bmp->bWasm ){
          switch( toBuild().hash() ){
            case"application"_64:
              [[fallthrough]];
            case"console"_64:/**/{
              if( e_getCvar( bool, "ENABLE_PTHREADS" ))
                lflags << " -Wemcc -pthread";
              break;
            }
          }
        }
        if( lstart != lflags )
          fs << lflags << "\n";
        fs << "\n";

        //----------------------------------------------------------------------
        // Construct C++ command string based on environment.
        //----------------------------------------------------------------------

        const string cxx_start = "command = ";
        string cxx = cxx_start;
        if( cstart != cflags ){
          if( bmp->bWasm ){
            if( e_dexists( "~/emsdk" ) && e_fexists( "~/emsdk/upstream/emscripten/em++" )){
              cxx << "~/emsdk/upstream/emscripten/em++";
            }else{// TODO: Search around on our $PATH yeahs!
              e_break( " * Emscripten not found at ~/emsdk." );
              return;
            }
          }else{
            cxx << "/usr/bin/clang++";
          }
          cxx << " $CXX_FLAGS $" << clabel << " ";
          switch( toLanguage().hash() ){
            case "c++23"_64:
              [[fallthrough]];
            case "cpp23"_64:
              [[fallthrough]];
            case "cxx23"_64:
              cxx << " -std=c++23";
              break;
            case "c++20"_64:
              [[fallthrough]];
            case "cpp20"_64:
              [[fallthrough]];
            case "cxx20"_64:
              cxx << "-Wc++20-extensions";
              cxx << " -std=c++20";
              break;
            case "c++17"_64:
              [[fallthrough]];
            case "cpp17"_64:
              [[fallthrough]];
            case "cxx17"_64:
              cxx << " -std=c++17";
              break;
            case "c++14"_64:
              [[fallthrough]];
            case "cpp14"_64:
              [[fallthrough]];
            case "cxx14"_64:
              cxx << " -std=c++14";
              break;
            case "c++11"_64:
              [[fallthrough]];
            case "cpp11"_64:
              [[fallthrough]];
            case "cxx11"_64:
              cxx << "-std=c++11";
              break;
            default:
              e_break( "C++11 is the minimum version." );
          }
          cxx << " -lstdc++"
              << " -o"
              << " $out"
              << " -c"
              << " $in\n";
        }

        //----------------------------------------------------------------------
        // Write CXX compilation rule string.
        //----------------------------------------------------------------------

        if( cxx != cxx_start ){
          fs << "rule CXX_" << toLabel().toupper() + "\n";
          fs << "  " + cxx;
          fs << "  description = Building C++ object $out\n";
          fs << "\n";
        }

        //----------------------------------------------------------------------
        // Construct C command string based on environment.
        //----------------------------------------------------------------------

        const string c_start = "command = ";
        string c = c_start;
        if( cstart != cflags ){
          if( bmp->bWasm ){
            if( e_dexists( "~/emsdk" ) && e_fexists( "~/emsdk/upstream/emscripten/emcc" )){
              c << "~/emsdk/upstream/emscripten/emcc";
            }else{
              e_break( "Emscripten not found at ~/emsdk." );
              return;
            }
          }else{
            c << "clang $" << clabel << " -o $out -c $in\n";
          }
        }

        //----------------------------------------------------------------------
        // Write C compilation rule string.
        //----------------------------------------------------------------------

        if( c != c_start ){
          fs << "rule C_" << toLabel().toupper() + "\n";
          fs << "  "
             << c;
          fs << "  description = Building C object $out\n";
          fs << "\n";
        }

        //----------------------------------------------------------------------
        // Write CXX linking rule string. Favors clang over gcc.
        //----------------------------------------------------------------------

        switch( toBuild().hash() ){

          //--------------------------------------------------------------------
          // Static libraries of type a (Microsoft not supported by Ninja path)
          //--------------------------------------------------------------------

          case"static"_64:// TODO: Are we missing -target here for archives?
            fs << "rule STATIC_LIB_" << toLabel().toupper() + "\n";
            fs << "  command = $PRE_LINK && ";
            if( bmp->bWasm ){
              fs << "~/emsdk/upstream/emscripten/emar rcs $TARGET_FILE ";
            }else if( e_fexists( "/usr/bin/ar" )){
              fs << "/usr/bin/ar qc $TARGET_FILE ";
              if( lstart != lflags ){
                fs << lflags << " ";
              }
            }else{
              e_break( "Archiver not found." );
              return;
            }
            fs << "$in && /usr/bin/ranlib $TARGET_FILE && $POST_BUILD\n";
            if( bmp->bWasm ){
                fs << "  description = Linking static Wasm library $out\n";
            }else{
              fs << "  description = Linking static library $out\n";
              fs << "  restat = $RESTAT\n";
            }
            break;

          //--------------------------------------------------------------------
          // Shared libraries of type so and dylib. Microsoft not supported.
          //--------------------------------------------------------------------

          case"shared"_64:/**/{
            if( bmp->bWasm ){
              fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
            }else{
              if( bmp->bCrossCompile ){
                if( crossCc.find( "linux" )){
                  fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                }else if( crossCc.find( "apple" )){
                  fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                }else if( crossCc.find( "pc" )){
                  fs << "rule SHARED_DLL_" << toLabel().toupper() + "\n";
                }else{
                  #if e_compiling( linux )
                    fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                  #elif e_compiling( osx )
                    fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                  #elif e_compiling( win64 )
                    fs << "rule SHARED_DLL_" << toLabel().toupper() + "\n";
                  #else
                    e_break( "Must define linux, macos or win64" );
                  #endif
                }
              }else{
                #if e_compiling( linux )
                  fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                #elif e_compiling( osx )
                  fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
                #elif e_compiling( win64 )
                  fs << "rule SHARED_DLL_" << toLabel().toupper() + "\n";
                #else
                  e_break( "Must define linux, macos or win64" );
                #endif
              }
            }
            fs << "  command = $PRE_LINK && ";
            if( bmp->bWasm ){// TODO: Search on path first and use e_dexists().
              fs << "~/emsdk/upstream/emscripten/emcc --shared ";
            }else{
              fs << "clang --shared ";
              if( lstart != lflags ){
                fs << lflags << " ";
              }
            }
            fs << " -lstdc++ $in -o $out && $POST_BUILD\n";
            if( bmp->bWasm )
                 fs << "  description = Linking shared (WASM) library $out\n";
            else fs << "  description = Linking shared library $out\n";
            break;
          }

          //--------------------------------------------------------------------
          // Executables.
          //--------------------------------------------------------------------

          case"application"_64:
            // TODO: Implement Linux Desktop for Vulkan.
            break;

          case"console"_64:/**/{
            if( bmp->bWasm )
              fs << "rule WASM_LINKER_" << toLabel().toupper() + "\n";
            else{
              string cxx;
              serializeCrossPlatformRules( cxx );
              fs << cxx;
            }
            fs << "  command = $PRE_LINK && ";
            if( bmp->bWasm )// TODO: Check different locations with e_fexists.
                  fs << "~/emsdk/upstream/emscripten/emcc";
            else fs << "clang";
            if( lstart != lflags )
              fs << " $" << llabel;
            if( bmp->bWasm ){
              fs << " $in -o ${TARGET_FILE}.html $LINK_LIBRARIES && $POST_BUILD\n";
              fs << "  description = Linking $out\n";
            }else{
              fs << " $in -o $TARGET_FILE $LINK_LIBRARIES && $POST_BUILD\n";
              if( bmp->bCrossCompile ){
                if( crossCc.find( "linux" )){
                  fs << "  description = Compiling ELF binary $out\n";
                }else if( crossCc.find( "apple" )){
                  fs << "  description = Compiling MACHO binary $out\n";
                }else if( crossCc.find( "pc" )){
                  fs << "  description = Compiling PE binary $out\n";
                }else{
                  #if e_compiling( linux )
                    fs << "  description = Compiling ELF binary $out\n";
                  #elif e_compiling( osx )
                    fs << "  description = Compiling MACHO binary $out\n";
                  #elif e_compiling( win64 )
                    fs << "  description = Compiling PE binary $out\n";
                  #else
                    e_break( "Must define linux, macos or win64" );
                  #endif
                }
              }else{
                #if e_compiling( linux )
                  fs << "  description = Compiling ELF binary $out\n";
                #elif e_compiling( osx )
                  fs << "  description = Compiling MACHO binary $out\n";
                #elif e_compiling( win64 )
                  fs << "  description = Compiling PE binary $out\n";
                #else
                  e_break( "Must define linux, macos or win64" );
                #endif
              }
            }
            break;
          }
        }
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
