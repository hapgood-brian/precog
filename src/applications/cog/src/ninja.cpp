//------------------------------------------------------------------------------
//                    Copyright 2022 Creepy Doll Software LLC.
//                            All rights reserved.
//
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
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

        const auto& path = File( in_path );
        const auto& ext = path.ext().tolower();
        switch( ext.hash() ){

          //--------------------------------------------------------------------
          // Platform specific file types.
          //--------------------------------------------------------------------

          case ".prefab"_64:
            inSources( Type::kPrefab ).push( path );
            break;
          case ".eon"_64:
            inSources( Type::kEon ).push( path );
            break;
          case ".a"_64:
            inSources( Type::kStaticlib ).push( path );
            break;

          //--------------------------------------------------------------------
          // Source and header file types.
          //--------------------------------------------------------------------

          case ".inl"_64:/**/{
            auto& inl = inSources( Type::kInl );
            if( ! inl.find( path )){
              inl.push( path );
            }
            break;
          }
          case ".hpp"_64:
          case ".hxx"_64:
          case ".hh"_64:/**/{
            auto& hpp = inSources( Type::kHpp );
            if( ! hpp.find( path )){
              hpp.push( path );
            }
            break;
          }
          case ".cpp"_64:
          case ".cxx"_64:
          case ".cc"_64:/**/{
            auto& cpp = inSources( Type::kCpp );
            if( ! cpp.find( path )){
              cpp.push( path );
            }
            break;
          }
          case ".h"_64:/**/{
            auto& h = inSources( Type::kH );
            if( ! h.find( path )){
              h.push( path );
            }
            break;
          }
          case ".c"_64:/**/{
            auto& c = inSources( Type::kC );
            if( ! c.find( path )){
              c.push( path );
            }
            break;
          }
          default:
            return false;
        }
        return true;
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
          const auto cores = u32( std::thread::hardware_concurrency() );
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
        if( bmp->bEmscripten ){
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
        if( !prefix.empty() ){
          cflags << " -include ../" << prefix;
        }
        switch( toBuild().tolower().hash() ){
          case"shared"_64:
            cflags << " -fPIC";
            break;
        }
        if( cstart != cflags ){
          fs << cflags << "\n";
        }

        //----------------------------------------------------------------------
        // Create LFLAGS variable in build ninja.
        //----------------------------------------------------------------------

        const auto llabel = toLabel().toupper() + "_LFLAGS";
        const auto lstart = llabel + " =";
        string lflags = lstart;
        if( bmp->bEmscripten ){
          switch( toBuild().hash() ){
            case"application"_64:
              [[fallthrough]];
            case"console"_64:
              if( e_getCvar( bool, "ENABLE_PTHREADS" )){
                lflags << " -Wemcc -pthread";
              }
              break;
            case"shared"_64:
              [[fallthrough]];
            case"static"_64:
              [[fallthrough]];
            default:
              break;
          }
        }
        if( lstart != lflags ){
          fs << lflags << "\n";
        }
        fs << "\n";

        //----------------------------------------------------------------------
        // Construct C++ command string based on environment.
        //----------------------------------------------------------------------

        const string cxx_start = "command = ";
        string cxx = cxx_start;
        if( cstart != cflags ){
          if( bmp->bEmscripten ){
            if( e_dexists( "~/emsdk" ) && e_fexists( "~/emsdk/upstream/emscripten/em++" )){
              cxx << "~/emsdk/upstream/emscripten/em++";
            }else{
              e_errorf( 981723, "Emscripten not found at ~/emsdk." );
              return;
            }
          }else if( e_fexists( "/usr/bin/clang++" )){
            cxx << "/usr/bin/clang++";
          }else if( e_fexists( "/usr/bin/g++" )){
            cxx << "/usr/bin/g++";
          }else{
            e_errorf( 870612, "Compiler not found." );
            return;
          }
          cxx << " $CXX_FLAGS $" << clabel << " -o $out -c $in\n";
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
          if( bmp->bEmscripten ){
            if( e_dexists( "~/emsdk" ) && e_fexists( "~/emsdk/upstream/emscripten/emcc" )){
              c << "~/emsdk/upstream/emscripten/emcc";
            }else{
              e_errorf( 981723, "Emscripten not found at ~/emsdk." );
              return;
            }
          }else if( e_fexists( "/usr/bin/clang" )){
            c << "/usr/bin/clang";
          }else if( e_fexists( "/usr/bin/gcc" )){
            c << "/usr/bin/gcc";
          }else{
            e_errorf( 761501, "Compiler not found." );
            return;
          }
          c << " $" << clabel << " -o $out -c $in\n";
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

          case"static"_64:
            if( bmp->bEmscripten ){
              fs << "rule STATIC_LIB_" << toLabel().toupper() + "\n";
            }else{
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "rule STATIC_LIB_" << toLabel().toupper() + "\n";
              #else
                fs << "rule COFF_LIB_" << toLabel().toupper() + "\n";
              #endif
            }
            fs << "  command = $PRE_LINK && ";
            if( bmp->bEmscripten ){
              fs << "~/emsdk/upstream/emscripten/emar rcs $TARGET_FILE ";
            }else if( e_fexists( "/usr/bin/ar" )){
              fs << "/usr/bin/ar qc $TARGET_FILE ";
              if( lstart != lflags ){
                fs << lflags << " ";
              }
            }else{
              e_errorf( 918723, "Compiler not found." );
              return;
            }
            fs << "$in && /usr/bin/ranlib $TARGET_FILE && $POST_BUILD\n";
            if( bmp->bEmscripten ){
                fs << "  description = Linking static (WASM) library $out\n";
            }else{
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "  description = Linking static library $out\n";
              #else
                fs << "  description = Linking COFF library $out\n";
              #endif
              fs << "  restat = $RESTAT\n";
            }
            break;

          //--------------------------------------------------------------------
          // Shared libraries of type so and dylib. Microsoft not supported.
          //--------------------------------------------------------------------

          case"shared"_64:
            if( bmp->bEmscripten ){
              fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
            }else{
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "rule SHARED_LIB_" << toLabel().toupper() + "\n";
              #else
                fs << "rule COFF_LIB_" << toLabel().toupper() + "\n";
              #endif
            }
            fs << "  command = $PRE_LINK && ";
            if( bmp->bEmscripten ){
              fs << "~/emsdk/upstream/emscripten/emcc --shared ";
            }else if( e_fexists( "/usr/bin/clang" )){
              fs << "/usr/bin/clang --shared ";
              if( lstart != lflags ){
                fs << lflags << " ";
              }
            }else{
              e_errorf( 918723, "Compiler not found." );
              return;
            }
            fs << "$in -o $out && $POST_BUILD\n";
            if( bmp->bEmscripten ){
                fs << "  description = Linking shared (WASM) library $out\n";
            }else{
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "  description = Linking shared library $out\n";
              #else
                fs << "  description = Linking native DLL $out\n";
              #endif
            }
            break;

          //--------------------------------------------------------------------
          // Executables.
          //--------------------------------------------------------------------

          case"application"_64:
            #if e_compiling( linux )
              [[fallthrough]];
            #else
              break;
            #endif

          case"console"_64:
            if( bmp->bEmscripten ){
              fs << "rule WASM_LINKER_" << toLabel().toupper() + "\n";
            }else{
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "rule ELF_LINKER_" << toLabel().toupper() + "\n";
              #else
                fs << "rule PE_LINKER_" << toLabel().toupper() + "\n";
              #endif
            }
            fs << "  command = $PRE_LINK && ";
            if( bmp->bEmscripten ){
              fs << "~/emsdk/upstream/emscripten/emcc";
            }else if( e_fexists( "/usr/bin/clang++" )){
              fs << "/usr/bin/clang++";
            }else if( e_fexists( "/usr/bin/g++" )){
              fs << "/usr/bin/g++";
            }else{
              e_errorf( 109283, "Compiler not found." );
              return;
            }
            if( lstart != lflags ){
              fs << " $" << llabel;
            }
            if( bmp->bEmscripten ){
              fs << " $in -o ${TARGET_FILE}.html $LINK_LIBRARIES && $POST_BUILD\n";
              fs << "  description = Linking $out\n";
            }else{
              fs << " $in -o $TARGET_FILE $LINK_LIBRARIES && $POST_BUILD\n";
              #if e_compiling( linux ) || e_compiling( osx )
                fs << "  description = Linking ELF binary $out\n";
              #else
                fs << "  description = Linking PE binary $out\n";
              #endif
            }
            break;
        }
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
