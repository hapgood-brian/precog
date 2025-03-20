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
#if e_compiling( win64 )
  //TODO: Figure out how we do symlinks on Windows.
#else
  #include<unistd.h>
#endif
#include<regex>

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================+=============================
//Extends:{                                       |

#ifdef __APPLE__
  #pragma mark - Extensions -
#endif

  e_extends( Workspace::NDK );

//}:                                              |
//Methods:{                                       |
  //[project]:{                                   |
    //extFromEnum:{                               |

      ccp Workspace::NDK::extFromEnum( const Type e )const{
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

      bool Workspace::NDK::sortingHat( const string& in_path ){

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

          case ".fablet"_64:
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

      void Workspace::NDK::serialize( Writer& fs )const{

        //----------------------------------------------------------------------
        // Ignore files.
        //----------------------------------------------------------------------

        const auto& onIgnore=[this]( auto it ){
          while( it ){
            auto ok = false;
            { auto parts = toIgnoreParts();
              parts.erase( "\n" );
              parts.erase( "\t" );
              parts.erase( " " );
              const auto& splits = parts.splitAtCommas();
              splits.foreachs(
                [&]( const auto& split ){
                  if( isIgnored( split
                      , *it )){
                    e_msgf( "  Ignoring %s"
                      , ccp( it->filename() ));
                    ok = true;
                  }
                  return!ok;
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
        onIgnore( const_cast<NDK*>( this )
          -> inSources( Type::kCpp ).getIterator() );
        onIgnore( const_cast<NDK*>( this )
          -> inSources( Type::kC ).getIterator() );

        //----------------------------------------------------------------------
        // Add my funny little calling card; should probably make jokes random.
        //----------------------------------------------------------------------

        const string commentLine = "//--------------------------------------"
          "----------------------------------------\n";
        const string jokeLine = "//                  The best method for acc"
          "elerating a computer\n//                     is the one that boos"
          "ts it by 9.8 m/s2.\n";
        fs << commentLine
           << jokeLine
           << commentLine
           << "// GENERATED FILE DO NOT EDIT IN ANY WAY SHAPE OR FORM SOMETHIN"
             "G BAD WILL HAPPEN\n"
           << commentLine
           << "\n";

        //----------------------------------------------------------------------
        // Write to the build.gradle file.
        //----------------------------------------------------------------------

        //https://docs.gradle.org/current/userguide/cpp_application_plugin.html
        //https://docs.gradle.org/current/userguide/building_cpp_projects.html
        //https://developer.android.com/studio/build
        switch( toBuild().hash() ){
          case"application"_64:
            [[fallthrough]];
          case"console"_64:
            fs << "plugins{\n  id 'com.android.application'\n}\n";
            break;
          //https://docs.gradle.org/current/userguide/cpp_library_plugin.html#cpp_library_plugin
          case"shared"_64:
            fs << "plugins{\n  id 'com.android.library'\n}\n";
            break;
          case"static"_64:
            fs << "plugins{\n  id 'com.android.library'\n}\n";
            break;
        }
        fs << "allprojects{\n";
        fs << "  repositories{\n";
        fs << "    mavenCentral()\n";
        fs << "    google()\n";
        fs << "  }\n";
        fs << "}\n";

        //----------------------------------------------------------------------
        // Generate the android{} section with all goodies.
        //----------------------------------------------------------------------

        fs << "android{\n";
        fs << "  namespace'com.bossfight."
           << toLabel().tolower()
           << "'\n";
        fs << "  compileSdk 33\n";
        fs << "  defaultConfig{\n";
        fs << "    targetSdk 33\n";
        fs << "    minSdk 27\n";
        fs << "    externalNativeBuild{\n";
        fs << "      cmake{\n";
        fs << "        cppFlags=[\n";
        switch( toLanguage().hash() ){
          case"c++20"_64:
            [[fallthrough]];
          case"c++17"_64:
            [[fallthrough]];
          case"c++14"_64:
            [[fallthrough]];
          case"c++11"_64:
            fs << "          '-std=" << toLanguage() << "',\n";
            break;
          default:
            e_break( e_xfs( "Unknown language \"%s\"", ccp( toLanguage() )));
        }
        fs << "        ]\n";
        fs << "      }\n";
        fs << "    }\n";
        fs << "  }\n";
        fs << "  externalNativeBuild{\n";
        fs << "    cmake{\n";
        fs << "      path \"src/main/cpp/CMakeLists.txt\"\n";
        fs << "      version \"3.30.2\"\n";
        fs << "    }\n";
        fs << "  }\n";
        fs << "}\n";

        //----------------------------------------------------------------------
        // Write out the gradle-wrapper.properties file.
        //----------------------------------------------------------------------

        const auto& gradleWrapperPath
          = Workspace::out
          + "gradle/wrapper/gradle-wrapper.properties";
        Writer gw( gradleWrapperPath
          , kTEXT );
        gw << "distributionBase=GRADLE_USER_HOME\n";
        gw << "distributionUrl=https://services.gradle.org/distributions/gradle-8.7.0-bin.zip\n";
        gw << "distributionPath=wrapper/dists\n";
        gw << "zipStorePath=wrapper/dists\n";
        gw << "zipStoreBase=GRADLE_USER_HOME\n";
        gw.save();

        //----------------------------------------------------------------------
        // Write out the CMakeLists.txt file.
        //----------------------------------------------------------------------

        auto cmakeLists
          = Workspace::out
          + toLabel()
          + "/src/main/cpp/CMakeLists.txt";
        Writer cm( cmakeLists
          , kTEXT );
        cm << "cmake_minimum_required( VERSION 3.30.2 )\n";
        cm << "set( CMAKE_CXX_COMPILER clang++ )\n";
        cm << "set( CMAKE_C_COMPILER clang )\n";
        cm << "project( \""
           << toLabel()
           << "\" )\n";
        cm << "add_library(\n"
           << "  "
           << toLabel()
           << "\n  "
           << toBuild().toupper()
           << "\n";
        vector<File>files;
        inSources( Type::kCpp ).foreach( [&]( const auto& fi ){ files.push( fi ); });
        inSources( Type::kC   ).foreach( [&]( const auto& fi ){ files.push( fi ); });
        auto ci = files
          . getIterator();
        while( ci ){
          const auto& cpp = ci->filename();
          if( !cpp.empty() )
            cm << "  " << cpp << "\n";
          ++ci;
        }
        cm << ")\n";
        //https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html?highlight=clang#cross-compiling-using-clang
        auto publicRoot = toIncludePaths().splitAtCommas();
        publicRoot.push( "." );
        if( !publicRoot.empty() ){
          auto it = publicRoot.getIterator();
          cm //Add all include directories.
            << "target_include_directories( "
            << toLabel()
            << " PUBLIC";
          while( it ){
            if( !it->empty() ){
              auto reldir = "../public/" + it->filename();
              auto cc =
                string( "\n  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/" )
                << reldir
                << ">";
              cc.replace( "/.>"
                , ">" );
              cm << cc;
            }
            ++it;
          }
          cm << " )\n";
        }
        auto& me = *const_cast<self*>( this );
        me.setDefinesRel( "ANDROID=1,"
          + toDefinesRel() );
        auto defines
          = toDefinesRel()
          . splitAtCommas();
        defines.sort(
          []( const auto& a, const auto& b ){
            return( a.len() > b.len() );
          }
        );
        cm // Add #define on the command line.
          << "target_compile_definitions( "
          << toLabel()
          << " PUBLIC";
        defines.foreach(
          [&]( const string& define ){
             cm << "\n  " << define;
          }
        );
        cm << " )\n";
        if( toBuild().hash() != "static"_64 ){
          cm << "find_library(\n  log-lib\n  log\n)\n"
             << "target_link_libraries( "
             << toLabel()
             << " PUBLIC"
             << "\n  ${log-lib}";
          if( toBuild() == "shared"_64 ){
            const auto& linkWith = toLinkWith();
            const auto& linkages = linkWith.splitAtCommas();
            auto it = linkages.getIterator();
            while( it ){
              if( !it->empty() ){
                auto name = *it;
                name.ltrim( 3 );
                name.trim( 2 );
                cm << "\n  " << name;
              }
              ++it;
            }
            cm << " )\n";
          }
        }
        cm.save();

        //----------------------------------------------------------------------
        // Write out the module depencies from LinkWith string.
        //----------------------------------------------------------------------

        if( toBuild().hash() != "static"_64 ){
          fs << "dependencies{\n"
             << "  implementation'androidx.appcompat:appcompat:1.6.1'\n";
          const auto& linksWith = toLinkWith()
            . splitAtCommas();
          auto it = linksWith.getIterator();
          while( it ){
            if( !it->empty() ){
              auto with = it->base();
              if( with.left( 3 )=="lib"_64 ){
                with.ltrim( 3 );
              } fs
                << "  implementation project( path:':"
                << with
                << "')\n"
              ;
            }
            ++it;
          }
          fs << "}\n";
        }

        //----------------------------------------------------------------------
        // Make symlinks to files.
        //----------------------------------------------------------------------

        const auto& onSymlink=[this](
              const auto& path
            , auto it ){
          while( it ){
            const auto& src = string( "./" ).os() + *it;
            const auto& dst = string( "./" ).os()
              + path
              + "/"
              + it->filename();
            if( e_getCvar( bool, "SPEW" )){
              e_msgf( "  Symlink: \"%s\" -> \"%s\""
                , ccp( src )
                , ccp( dst )
              );
            }
            #if !e_compiling( win64 )
              e_rm( dst );
              const auto err = symlink( src, dst );
              if( err )
                e_msg( "Not a symlink!" );
            #else
              //TODO: Make a 64-bit Windows 10/11 junction.
            #endif
            ++it;
          }
        };

        //----------------------------------------------------------------------
        // Create all the symlinks in public and cpp directories.
        //----------------------------------------------------------------------

        if( !inSources( Type::kCpp ).empty() ){
          auto publicFolder = fs
            . toFilename()
            . path()
            + kSourceSet
            + "/public";
          publicFolder.replace( "//", "/" );
          const auto& includes = toIncludePaths();
          if( !includes.empty() ){
            const auto& contents
              = includes
              . splitAtCommas();
            if( !contents.empty() ){
              auto it = contents
                . getIterator();
              const auto& pwd = string( "./" ).os();
              while( it ){
                if( !it->empty() ){
                  const auto& symLnk = pwd
                    + publicFolder
                    + "/"
                    + it->filename();
                  e_rm( symLnk );
                  const auto& srcDir = pwd
                    + *it;
                    + "/"
                    + publicFolder.filename();
                  #if !e_compiling( win64 )
                    const auto error = symlink(
                        srcDir
                      , symLnk );
                    if( error ){
                      e_msgf( "Symlink failed for \"%s\" -> \"%s\""
                        , ccp( srcDir )
                        , ccp( symLnk )
                      );
                    }
                  #endif
                }
                ++it;
              }
            }
          }
          // Do we ever want to do this? It just symlinks every header into the
          // same public directory.
          onSymlink( publicFolder
            , inSources( Type::kInl )
            . getIterator() );
          onSymlink( publicFolder
            , inSources( Type::kHpp )
            . getIterator() );
          onSymlink( publicFolder
            , inSources( Type::kH )
            . getIterator()
          );
        }
        if( !inSources( Type::kCpp ).empty() ){
          auto cppFolder = fs
            . toFilename()
            . path()
            + kSourceSet
            + "/cpp";
          cppFolder.replace( "//", "/" );
          onSymlink( cppFolder
            , inSources( Type::kCpp )
            . getIterator()
          );
        }
        if( !inSources( Type::kC ).empty() ){
          auto cFolder = fs
            . toFilename()
            . path()
            + kSourceSet
            + "/c";
          cFolder.replace( "//", "/" );
          onSymlink( cFolder
            , inSources( Type::kC )
            . getIterator()
          );
        }
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
