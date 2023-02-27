//------------------------------------------------------------------------------
//                    Copyright 2022 Creepy Doll Software LLC.
//                            All rights reserved.
//
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------

#include"generators.h"
#include"luacore.h"
#include"std.h"
#include"ws.h"
#if e_compiling( microsoft )
  //TODO: Figure out how we do symlinks on Windows.
#else
  #include<unistd.h>
#endif
#include<regex>

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================|=============================
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
                  if( isIgnoreFile( split
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
            fs << "plugins{ id 'com.android.application' }\n";
            break;
          //https://docs.gradle.org/current/userguide/cpp_library_plugin.html#cpp_library_plugin
          case"shared"_64:
            fs << "plugins{ id 'com.android.library' }\n";
            break;
          case"static"_64:
            fs << "plugins{ id 'com.android.library' }\n";
            break;
        }

        //----------------------------------------------------------------------
        // Generate the android{} section with all goodies.
        //----------------------------------------------------------------------

        fs << "android{\n";
        fs << "  namespace'com.bossfight."
           << toLabel()
           << "'\n";
        fs << "  compileSdk 33\n";
        fs << "  defaultConfig{\n";
        fs << "    targetSdk 33\n";
        fs << "    minSdk 27\n";
        fs << "    externalNativeBuild{\n";
        fs << "      cmake{\n";
        fs << "        cppFlags=[ ";
        switch( toLanguage().hash() ){
          case"c++20"_64:
            [[fallthrough]];
          case"c++17"_64:
            [[fallthrough]];
          case"c++14"_64:
            [[fallthrough]];
          case"c++11"_64:
            fs << "'-std=" << toLanguage();
            break;
          default: e_errorf( 1092
            , "Unknown language \"%s\""
            , ccp( toLanguage() )
          );
        }
        fs << "' ]\n";
        fs << "      }\n";
        fs << "    }\n";
        fs << "  }\n";
        fs << "  externalNativeBuild{\n";
        fs << "    cmake{\n";
        fs << "      path \"src/main/cpp/CMakeLists.txt\"\n";
        fs << "      version \"3.22.1\"\n";
        fs << "    }\n";
        fs << "  }\n";
        fs << "  compileOptions{\n"
           << "    sourceCompatibility JavaVersion.VERSION_1_8\n"
           << "    targetCompatibility JavaVersion.VERSION_1_8\n"
           << "  }\n";
        fs << "}\n";

        //----------------------------------------------------------------------
        // Write out the CMakeLists.txt file.
        //----------------------------------------------------------------------

        auto cmakeLists
          = "tmp/"
          + toLabel()
          + "/src/main/cpp/CMakeLists.txt";
        Writer cm( cmakeLists, kTEXT );
        cm << "cmake_minimum_required( VERSION 3.22.1 )\n";
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
        files.pushVector( inSources( Type::kCpp ));
        files.pushVector( inSources( Type::kC ));
        auto ci = files
          . getIterator();
        while( ci ){
          const auto& cpp = ci->filename();
          if( !cpp.empty() ){
            cm << "  "
               << cpp
               << "\n";
          }
          ++ci;
        }
        cm << ")\n";
        if( toBuild().hash() != "static"_64 ){
          cm << "find_library(\n  log-lib\n  log\n)\n"
             << "target_link_libraries(\n"
             << "  "
             << toLabel();
          if( toBuild() == "shared"_64 ){
            const auto& linkWith = toLinkWith();
            const auto& linkages = linkWith.splitAtCommas();
            auto it = linkages.getIterator();
            while( it ){
              if( !it->empty() ){
                cm << "\n  " << *it;
              }
              ++it;
            }
          }
          cm << "\n  ${log-lib}" << "\n)";
        }
        cm.save();

        //----------------------------------------------------------------------
        // Write out the module depencies from LinkWith string.
        //----------------------------------------------------------------------

        fs << "dependencies{\n"
           << "  implementation 'com.android.support:appcompat-v7:28.0.0'\n";
        const auto& linksWith = toLinkWith()
          . splitAtCommas();
        auto it = linksWith.getIterator();
        while( it ){
          if( !it->empty() ){
            auto with = it->basename();
            if( with.left( 3 )=="lib"_64 ){
              with.ltrim( 3 );
            } fs
              << "  implementation project(':"
              << with
              << "')\n"
            ;
          }
          ++it;
        }
        fs << "}\n";

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
            if( e_getCvar( bool, "VERBOSE_LOGGING" )){
              e_msgf( "symlink: \"%s\" -> \"%s\""
                , ccp( src )
                , ccp( dst )
              );
            }
            #if!e_compiling( microsoft )
              if( !e_lexists( dst )){
                const auto err = symlink( src, dst );
                if( err ){
                  e_errorf( 20394
                    , "Couldn't make symlink: \"%s\" -> \"%s\""
                    , ccp( src )
                    , ccp( dst )
                  );
                }
              }
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
                  const auto& srcDir = pwd
                    + *it;
                    + "/"
                    + publicFolder.filename();
                  const auto error = symlink(
                      srcDir
                    , symLnk );
                  if( error ){
                    e_errorf( 1500
                      , "Symlink failed for \"%s\" -> \"%s\""
                      , ccp( srcDir )
                      , ccp( symLnk )
                    );
                  }
                }
                ++it;
              }
            }
            goto sk;
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
    sk: if( !inSources( Type::kCpp ).empty() ){
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
//================================================|=============================
