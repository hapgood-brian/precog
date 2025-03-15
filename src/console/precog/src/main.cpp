//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#include"luacore.h"
#include"ws.h"

using namespace EON;
using namespace gfc;
using namespace fs;

//------------------------------------------------+-----------------------------
//Statics:{                                       |

  hashmap<u64,string> Workspace::File::filerefs;

//}:                                              |
//Private:{                                       |
  //[workspace]:{                                 |

    namespace{
      constexpr ccp kWorkspace = "workspace=class'workspace'{\n"
        //----------------------------------------+-----------------------------
        //new:{                                   |
          "  new=function(self,name)\n"
          "    return class'workspace'{\n"
          "      m_tProjects={},\n"
          "      m_sName=name,\n"
          "      new=function(self,label)\n"
          "        local t=class'project'{\n"
          //--------------------------------------+-----------------------------
          //Microsoft:{                           |
          "          dependencies=function(self,dependsOn)\n"
          "            self.m_dependencies=dependsOn\n"
          "          end,\n"
          "          winsdk=function(self,version)\n"
          "            self.m_winsdk=version\n"
          "            return self\n"
          "          end,\n"
          "          toolchain=function(self,version)\n"
          "            self.m_toolchain=version\n"
          "            return self\n"
          "          end,\n"
          //Creates a .def file for resources in a DLL.
          "          def=function(self,path)\n"
          "            self.m_def=path\n"
          "            return self\n"
          "          end,\n"
          //}:                                    |
          //Common:{                              |
            //set_include_paths:{                 |
            //TODO: Check this doesn't double up files like find_includes did.
            "          set_include_paths=function(self,paths)\n"
            "            if nil~=self.m_includePaths then\n"
            "              self.m_includePaths=self.m_includePaths..','..paths\n"
            "            else\n"
            "              self.m_includePaths=paths\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //find_resources:{                    |
            //TODO: Check this doesn't double up files like find_includes did.
            "          find_resources=function(self,paths)\n"
            "            if nil~=self.m_resPaths then\n"
            "              self.m_resPaths=self.m_resPaths..','..paths\n"
            "            else\n"
            "              self.m_resPaths=paths\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //find_libraries:{                    |
            "          find_libraries=function(self,paths)\n"
            "            if nil~=self.m_libraryPaths then\n"
            "              self.m_libraryPaths=self.m_libraryPaths..','..paths\n"
            "            else\n"
            "              self.m_libraryPaths=paths\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //find_sources:{                      |
            "          find_sources=function(self,paths)\n"
            "            if nil~=self.m_srcPaths then\n"
            "              self.m_srcPaths=self.m_srcPaths..','..paths\n"
            "            else\n"
            "              self.m_srcPaths=paths\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //skip_unity:{                        |
            "          skip_unity=function(self,files)\n"
            "            self.m_skipUnity=files\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //link_with:{                         |
            "          link_with=function(self,libs)\n"
            "            if type(libs)=='table'then\n"
            "              self.m_linkWith=libs\n"
            "            elseif type(libs)=='string'then\n"
            "              self.m_linkWith=libs\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //install:{                           |
            "          install=function(self,script)\n"
            "            self.m_installScript=script\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //defines:{                           |
            //         Usage: defines{...,...} or defines( debug, release )
            "          defines=function(self,t,r)\n"
            //           Process the debug build defines first.
            "            if type(t)=='table'then\n"
            "              if nil~=self.m_definesDbg then\n"
            "                self.m_definesDbg=self.m_definesDbg..','..t[1]\n"
            "              else\n"
            "                self.m_definesDbg=tostring(t[1])\n"
            "              end if nil~=self.m_definesRel then\n"
            "                self.m_definesRel=self.m_definesRel..','..t[2]\n"
            "              else\n"
            "                self.m_definesRel=t[2]\n"
            "              end\n"
            "            elseif type(t)=='string'then\n"
            "              if nil~=self.m_definesDbg then\n"
            "                self.m_definesDbg=self.m_definesDbg..','..t\n"
            "              else\n"
            "                self.m_definesDbg=t\n"
            "              end\n"
            "            elseif nil~=self.m_definesDbg then\n"
            "              self.m_definesDbg=self.m_definesDbg..','..t\n"
            "            else\n"
            "              self.m_definesDbg=tostring(t)\n"
            "            end\n"
            //           Process the release build defines next.
            "            if nil~=r then\n"
            "              if type(r)=='string'then\n"
            "                if nil~=self.m_definesRel then\n"
            "                  self.m_definesRel=self.m_definesRel..','..r\n"
            "                else\n"
            "                  self.m_definesRel=r\n"
            "                end\n"
            "              elseif nil~=self.m_definesRel then\n"
            "                self.m_definesRel=self.m_definesRel..','..r\n"
            "              else\n"
            "                self.m_definesRel=r\n"
            "              end\n"
            "            end\n"
            //           Return the self reference to caller so we can continue chaining.
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //disable:{                           |
            "          disable=function(self, options)\n"
            "            self.m_disableOpts=options\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //target:{                            |
            "          target=function(self,build)\n"
            "            self.m_build=build\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //prefix:{                            |
            "          prefix=function(self,header)\n"
            "            self.m_prefixHeader=header\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //ignore:{                            |
            "          ignore=function(self,regexes)\n"
            "            if nil~=self.m_ignore then\n"
            "              self.m_ignore=self.m_ignore..','..regexes\n"
            "            else\n"
            "              self.m_ignore=regexes\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
            //c_lang:{                            |
            //This is a shortcut function; for brevity (or not).
            "          c_lang=function(self,lang)\n"
            "            self.m_clanguage=lang\n"
            "            return self\n"
            "          end,\n"
            "          c_language=function(self,l)\n"
            "            return self:c_lang( l )\n"
            "          end,\n"
            //This is a shortcut function; for brevity (or not).
            "          lang=function(self,lang)\n"
            "            self.m_language=lang\n"
            "            return self\n"
            "          end,\n"
            "          language=function(self,l)\n"
            "            return self:lang( l )\n"
            "          end,\n"
            //}:                                  |
            //unity:{                             |
            "          unity=function(self,enable)\n"
            "            self.m_bUnity=enable\n"
            "          end,\n"
            //}:                                  |
            //libs:{                              |
            "          libs=function(self,paths)\n"
            "            if nil~=self.m_resPaths then\n"
            "              self.m_libs=self.m_libs..','..paths\n"
            "            else\n"
            "              self.m_libs=paths\n"
            "            end\n"
            "            return self\n"
            "          end,\n"
            //}:                                  |
          //}:                                    |
          //Apple:{                               |
          "          disableLibraryValidation=function(self,disable)\n"
          "            self.m_disableLibValidation=disable\n"
          "            return self\n"
          "          end,\n"
          "          enableJIT=function(self,enable)\n"
          "            self.m_enableJIT=enable\n"
          "            return self\n"
          "          end,\n"
          "          appleSilicon=function(self, enable)\n"
          "            self.m_enableAppleSilicon=enable\n"
          "            return self\n"
          "          end,\n"
          "          universal=function(self, enable)\n"
          "            self.m_enableUniversal=enable\n"
          "            return self\n"
          "          end,\n"
          "          not_embedded=function(self)\n"
          "            self.m_noEmbedAndSign=true\n"
          "            return self\n"
          "          end,\n"
          "          embed_and_sign=function(self,files)\n"
          "            self.m_filesToEmbedAndSign=files\n"
          "            return self\n"
          "          end,\n"
          "          load_all_symbols=function(self,enable)\n"
          "            self.m_loadAllSymbols=enable\n"
          "            return self\n"
          "          end,\n"
          "          harden=function(self,hardenedRuntime)\n"
          "            self.m_hardenedRuntime=hardenedRuntime\n"
          "            return self\n"
          "          end,\n"
          "          deployment=function(self,version)\n"
          "            self.m_deployTo=version\n"
          "            return self\n"
          "          end,\n"
          "          sdk_version=function(self,version)\n"
          "            self.m_sdkVersion=version\n"
          "            return self\n"
          "          end,\n"
          "          enableARC=function(self,enable)\n"
          "            self.m_arcEnabled=enable\n"
          "            return self\n"
          "          end,\n"
          "          export_headers=function(self,dirsAndFiles)\n"
          "            self.m_exportHeaders=dirsAndFiles\n"
          "            return self\n"
          "          end,\n"
          "          export_refs=function(self,dirsAndFiles)\n"
          "            self.m_exportRefs=dirsAndFiles\n"
          "            return self\n"
          "          end,\n"
          "          organization=function(self,name)\n"
          "            self.m_orgName=name\n"
          "            return self\n"
          "          end,\n"
          "          identifier=function(self,id)\n"
          "            self.m_bundleId=id\n"
          "            return self\n"
          "          end,\n"
          "          team=function(self,name)\n"
          "            self.m_teamName=name\n"
          "            return self\n"
          "          end,\n"
          "          find_frameworks=function(self,paths)\n"
          "            self.m_frameworkPaths=paths\n"
          "            return self\n"
          "          end,\n"
          "          os_target=function(self,osTarget)\n"
          "            self.m_osTarget=osTarget\n"
          "            return self\n"
          "          end,\n"
          //}:                                    |
          //--------------------------------------+-----------------------------
          "        }\n"
          "        self.m_tProjects[label]=t\n"
          "        t.label=label\n"
          "        return t\n"
          "      end,\n"
          "    }\n"
          "  end,\n"

        //}:                                      |
        //----------------------------------------+-----------------------------
      "}\n";
    }

  //}:                                            |
  //[platform]:{                                  |

    namespace{

      //------------------------------------------------------------------------
      // Get the platform name; for the user to query the build name.
      //------------------------------------------------------------------------

      string getPlatformName(){
        if( Workspace::bmp->bWasm ){
          return"  return'wasm'";
        }
        if( Workspace::bmp->bXcode11 ||
            Workspace::bmp->bXcode12 ||
            Workspace::bmp->bXcode15 ||
            Workspace::bmp->bXcode16 ){
          return"  return'macos'";
        }
        if( Workspace::bmp->bVS2019 ||
            Workspace::bmp->bVS2022 ){
          return"  return'win64'";
        }
        if( Workspace::bmp->bNinja ){
          return"  return'linux'";
        }
        if( Workspace::bmp->bNDK ){
          return"  return'android'";
        }
        return nullptr;
      }

      //------------------------------------------------------------------------
      // Get the platform class; for the user to query the platform.
      //------------------------------------------------------------------------

      string platformClass(){
        string out;
        out << "platform=class'platform'{\n";

        //----------------------------------------+-----------------------------
        //builds:{                                |

          out << "builds=function()\n";
          out << "  return{ getBuildSystems() }\n";
          out << "end,\n";

        //}:                                      |
        //vendor:{                                |

          out << "vendor = function()\n";
          if( Workspace::bmp->bXcode11 ||
              Workspace::bmp->bXcode12 ||
              Workspace::bmp->bXcode15 ||
              Workspace::bmp->bXcode16 ){
            out << "  return'apple'\n";
          }else if( Workspace::bmp->bXcode11 ||
                    Workspace::bmp->bXcode12 ||
                    Workspace::bmp->bXcode15 ||
                    Workspace::bmp->bXcode16 ){
            out << "  return'apple'\n";
          }else if( Workspace::bmp->bVS2019 ||
                    Workspace::bmp->bVS2022 ){
            out << "  return'microsoft'\n";
          }else{
            out << "  return'community'\n";
          }
          out << "end,\n";

        //}:                                      |
        //save:{                                  |

          out << "save = function(self)\n";
          out << "  out.save(out.generate(self),'";
          out << Workspace::out;
          out << "')\n";
          out << "end,\n";

        //}:                                      |
        //name:{                                  |

          out << "name = function()\n";
          out << "  " << getPlatformName() << "\n";
          out << "end,\n";

        //}:                                      |
        //is:{                                    |

          out << "is = function(name)\n";
          #if e_compiling( osx )
            out << "  return( name=='apple' )\n";
          #elif e_compiling( microsoft )
            out << "  return( name=='win64' )\n";
          #elif e_compiling( linux )
            out << "  return( name=='linux' )\n";
          #else
            out << "  return nil\n";
          #endif
          out << "end,\n";

        //}:                                      |
        //----------------------------------------+-----------------------------

        out << "}\n";
        return out;
      }
    }

  //}:                                            |
  //[globals]:{                                   |

    string            Workspace::gen;
    Workspace::States Workspace::bmp;

  //}:                                            |
  //generate:{                                    |

    namespace{
      int generate( const string& cgf ){

        //----------------------------------------------------------------------
        // Must always create the tmp directory.
        //----------------------------------------------------------------------

        if( !IEngine::dexists( Workspace::out ))
             IEngine::mkdir(   Workspace::out );

        //----------------------------------------------------------------------
        // Create Lua context, setup options on it, and run sandboxed script.
        //----------------------------------------------------------------------

        const auto& createLua=[&]( const ccp pBuffer ){
          Lua::handle hLua = e_new<Lua>();
          auto& lua = hLua.cast();
          lua.initialise();
          lua.sandbox( platformClass() );
          lua.sandbox( kWorkspace );
          string sBuffer( pBuffer );
          sBuffer.replace( "${RELEASE}"
            , "release" );
          sBuffer.replace( "${DEBUG}"
            , "debug" );
          if( Workspace::bmp->bXcode11 ||
              Workspace::bmp->bXcode12 ||
              Workspace::bmp->bXcode15 ||
              Workspace::bmp->bXcode16 ){
            if( Workspace::bmp->osMac ){
              sBuffer.replace( "${PLATFORM}"
                , "macos"
              );
            }else{
              sBuffer.replace( "${SUBPLATFORM}"
                , "ios" );
              sBuffer.replace( "${PLATFORM}"
                , "macos"
              );
            }
          }else if( Workspace::bmp->bGradle ){
            if( Workspace::bmp->bNDK ){
              sBuffer.replace( "${PLATFORM}"
                , "android"
              );
            }
          }else if( Workspace::bmp->bNinja ){
            sBuffer.replace( "${PLATFORM}"
              , "linux"
            );
          }else if( Workspace::bmp->bVS2019 ||
                    Workspace::bmp->bVS2022 ){
            sBuffer.replace( "${PLATFORM}"
              , "windows"
            );
          }
          lua.sandbox( sBuffer );
          return hLua;
        };

        //----------------------------------------------------------------------
        // Load the script and run it in Lua VM.
        //----------------------------------------------------------------------

        const auto& st = e_fload( cgf );
        if( st.empty() )
          return -1;
        vector<Lua::handle> vlua;
        st.query(
          [&]( ccp pBuffer ){

            //------------------------------------------------------------------
            // Make local options.
            //------------------------------------------------------------------

            string equ;
            equ << "\nlocal options={";
            if( Workspace::bmp->bXcode16 ){
              equ << "\n  xcode16 = true,";
            }else{
              equ << "\n  xcode16 = false,";
            }
            if( Workspace::bmp->bXcode15 ){
              equ << "\n  xcode15 = true,";
            }else{
              equ << "\n  xcode15 = false,";
            }
            if( Workspace::bmp->bXcode12 ){
              equ << "\n  xcode12 = true,";
            }else{
              equ << "\n  xcode12 = false,";
            }
            if( Workspace::bmp->bXcode11 ){
              equ << "\n  xcode11 = true,";
            }else{
              equ << "\n  xcode11 = false,";
            }
            if( Workspace::bmp->bVS2022 ){
              equ << "\n  vs2022 = true,";
            }else{
              equ << "\n  vs2022 = false,";
            }
            if( Workspace::bmp->bVS2019 ){
              equ << "\n  vs2019 = true,";
            }else{
              equ << "\n  vs2019 = false,";
            }
            if( Workspace::bmp->bWasm ){
              equ << "\n  emscripten = true,";
              equ << "\n  wasm = true,";
            }else{
              equ << "\n  emscripten = false,";
              equ << "\n  wasm = false,";
            }
            if( Workspace::bmp->bNinja ){
              equ << "\n  ninja = true,";
            }else{
              equ << "\n  ninja = false,";
            }
            if( Workspace::bmp->bNDK ){
              equ << "\n  ndk = true,";
            }else{
              equ << "\n  ndk = false,";
            }
            equ << "\n}\n";

            //------------------------------------------------------------------
            // Create the targeted script and sandbox it. Gets called twice if
            // generating for ios and macos simultaneously.
            //------------------------------------------------------------------

            const auto& targets = Workspace::getTargets();
            auto targetedScript = equ + pBuffer;
            auto it = targets.getIterator();
            while( it ){
              auto hLua = createLua( targetedScript );
              vlua.push( hLua );
              ++it;
            }
          }
        );
        vlua.clear();
        e_msgf(
          "ok" );
        return 0;
      }
    }

  //}:                                            |
//}:                                              |
//Program:{                                       |
  //main:{                                        |

    int IEngine::main( const strings& args ){

      //------------------------------------------+-----------------------------
      //Versioning:{                              |

        //----------------------------------------------------------------------
        // Setup the versioning.
        //----------------------------------------------------------------------

        // Each has 256 steps: 0x00 thru 0xFF.
        static constexpr u8 major = 0x03; // Major version number [majrelease]
        static constexpr u8 minor = 0x00; // Minor version number [minrelease]
        static constexpr u8 rev   = 0x00; // Revision
        static constexpr u8 build = 0x00; // Build (Reg bvilds).
        static constexpr u8 patch = 0x00; // Patch (bug fixes).

        //----------------------------------------------------------------------
        // Message out the version.
        //----------------------------------------------------------------------

        string title; title.catf(
          "Precog v%u.%u.%u"
            , major
            , minor
            , rev );
        if( build )
          title.catf( " b%u"
            , build );
        if( patch )
          title.catf( " p%u"
            , patch );
        if( args.size() == 1u )
          title << "\n  -? helps";
        e_msg( title );
        if( args.size() == 1u )
          return 0;

      //}:                                        |
      //------------------------------------------+-----------------------------
      //                                          :
      //                                          :
      //                                          :
      //------------------------------------------+-----------------------------
      //Options:{                                 |
        //Projects options:{                      |

          //--------------------------------------------------------------------
          // Run through all the arguments seting options as we go. Final non
          // long or short option will generate from arg as filename.
          //--------------------------------------------------------------------

          auto it = args.getIterator();
          while( ++it ){
            const auto& key = it->tolower();
            if( key.left( 2 ).hash() == "-o"_64 ){
              Workspace::out = key.right( key.len()-2 ) + "/";
              continue;
            }
            switch(( key.hash() )){

              //----------------------------------------------------------------
              // Export an Android gradle project.
              //----------------------------------------------------------------

              case"gradle"_64:
                Workspace::bmp->bGradle = 1;// and Cmake...?
                Workspace::bmp->bNDK    = 1;
                continue;

              //----------------------------------------------------------------
              // Export a Visual Studio 2022 project instead of default 2019.
              //----------------------------------------------------------------

              case"vs2022=v143"_64:
                Workspace::bmp->bVSTools143 = 1;
                Workspace::bmp->bVS2022     = 1;
                continue;
              case"vs2022"_64:
                Workspace::bmp->bVSTools143 = 1;
                Workspace::bmp->bVS2022     = 1;
                continue;
              case"vs2019"_64:
                Workspace::bmp->bVS2019 = 1;
                continue;

              //----------------------------------------------------------------
              // Export to Ninja using emscripten and web assembly not C++.
              //----------------------------------------------------------------

              // Handle emscripten and wasm options.
              case"emscripten"_64:
                [[fallthrough]];
              case"wasm"_64:
                Workspace::bmp->bNinja = 1;
                Workspace::bmp->bWasm  = 1;
                continue;

              // Handle ninja option except on linux where it is the default.
              case"ninja"_64:
                Workspace::bmp->bNinja = 1;
                continue;

              //----------------------------------------------------------------
              // Enable iOS builds with or without macOS support too. Note to
              // self: cog does not support AppleTV (tvOS) in any form.
              //----------------------------------------------------------------

              case"xcode=ios,macos"_64:
                [[fallthrough]];
              case"xcode=macos,ios"_64:
                [[fallthrough]];
              case"xcode=macos"_64:
                [[fallthrough]];
              case"xcode=ios"_64:
                [[fallthrough]];
              case"xcode"_64:
                /* xcode xcworkspace and PBX xcodeproj */{
                string s( key );
                if( s.erase( "macos" )){
                  Workspace::bmp->bXcode16 = 1;
                  Workspace::bmp->osMac    = 1;
                }else if( s.erase( "ios" )){
                  Workspace::bmp->bXcode16 = 1;
                  Workspace::bmp->osIphone = 1;
                }else{
                  Workspace::bmp->bXcode16 = 1;
                  Workspace::bmp->osMac    = 1;
                }
                continue;
              }

              //----------------------------------------------------------------
              // Everything else runs generator.
              //----------------------------------------------------------------

              default:/**/{
                if( *key != '-' )
                  break;

                //--------------------------------------------------------------
                // Handling shortcuts for platform.
                //--------------------------------------------------------------

                if( it->tolower().hash() == "--macho"_64 ){
                  Workspace::bmp->bExtMacho = 1;
                  break;
                }
                if( it->tolower().hash() == "--elf"_64 ){
                  Workspace::bmp->bExtElf = 1;
                  break;
                }
                if( it->tolower().hash() == "--pe"_64 ){
                  Workspace::bmp->bExtPE = 1;
                  break;
                }

                //--------------------------------------------------------------
                // Enable Unicode for Windows.
                //--------------------------------------------------------------

                if( it->tolower().hash() == "--utf16"_64 ){
                  Workspace::bmp->bUTF16 = 1;
                  continue;
                }

                //--------------------------------------------------------------
                // Cross compiling option.
                //--------------------------------------------------------------

                if( it->tolower().left( 8 ).hash() == "--cross="_64 ){
                  Workspace::crossCompileTriple = it->ltrimmed( 8 );
                  Workspace::bmp->bCrossCompile = 1;
                  continue;
                }

                if( it->left( 2 ).hash() == "-x"_64 ){
                  Workspace::crossCompileTriple = it->ltrimmed( 2 );
                  Workspace::bmp->bCrossCompile = 1;
                  continue;
                }

                //--------------------------------------------------------------
                // Enable unity builds.
                //--------------------------------------------------------------

                if( it->tolower().hash() == "--unity"_64 ){
                  Workspace::bmp->bUnity = 1;
                  continue;
                }

                //--------------------------------------------------------------
                // Delete tmp directory.
                //--------------------------------------------------------------

                if( it->tolower().hash() == "--clean"_64 ){
                  IEngine::rm( Workspace::out );
                  continue;
                }

                //--------------------------------------------------------------
                // C++ langauge options.
                //--------------------------------------------------------------

                switch( key.hash() ){
                  case"--c++2b"_64:
                    [[fallthrough]];
                  case"--cxx2b"_64:
                    [[fallthrough]];
                  case"--cpp2b"_64:
                    [[fallthrough]];
                  case"--c++23"_64:
                    [[fallthrough]];
                  case"--cxx23"_64:
                    [[fallthrough]];
                  case"--cpp23"_64:
                    Workspace::wsp->setLanguage( "c++23"_64 );
                    break;
                  case"--c++20"_64:
                    [[fallthrough]];
                  case"--cxx20"_64:
                    [[fallthrough]];
                  case"--cpp20"_64:
                    Workspace::wsp->setLanguage( "c++20"_64 );
                    break;
                  case"--c++17"_64:
                    [[fallthrough]];
                  case"--cxx17"_64:
                    [[fallthrough]];
                  case"--cpp17"_64:
                    Workspace::wsp->setLanguage( "c++17"_64 );
                    break;
                  case"--c++14"_64:
                    [[fallthrough]];
                  case"--cxx14"_64:
                    [[fallthrough]];
                  case"--cpp14"_64:
                    Workspace::wsp->setLanguage( "c++14"_64 );
                    break;
                  case"--c++11"_64:
                    [[fallthrough]];
                  case"--cxx11"_64:
                    [[fallthrough]];
                  case"--cpp11"_64:/**/{
                    Workspace::wsp->setLanguage( "c++11"_64 );
                    break;
                  }
                }

                //--------------------------------------------------------------
                // Handle the "--once:name" case. Only produces one new project
                // without touching the existing ones. It's awesome for getting
                // a new .vcxproj or .xcodeproj file/package, without having to
                // absolutely knacker the existing project. That way if there's
                // problems with a build, a corner case it doesn't support, you
                // can make a .bundle, .dylib, or .a like a charm.
                //--------------------------------------------------------------

                //--------------------------------------------------------------
                // Export an Xcode 1x project instead of the default 12.
                //--------------------------------------------------------------

                if( it->hash() == "--xc16"_64 ){
                  Workspace::bmp.all       = 0;
                  Workspace::bmp->bXcode16 = 1;
                  break;
                }
                if( it->hash() == "--xc15"_64 ){
                  Workspace::bmp.all       = 0;
                  Workspace::bmp->bXcode15 = 1;
                  break;
                }
                if( it->hash() == "--xc12"_64 ){
                  Workspace::bmp.all       = 0;
                  Workspace::bmp->bXcode12 = 1;
                  break;
                }
                if( it->hash() == "--xc11"_64 ){
                  Workspace::bmp.all       = 0;
                  Workspace::bmp->bXcode11 = 1;
                  break;
                }

                //--------------------------------------------------------------
                // Help option.
                //--------------------------------------------------------------

                if( it->hash() == "--help"_64 || it->hash() == "-?"_64 ){
                  e_msg( "  Usage \"precog* target [globals|options|cvars] [something.lua]\"" );
                  e_msg( "    Targets:" );
                  e_msg( "      xcode{=[macos|ios]}" );
                  e_msg( "      vs2022{=[v143]}" );
                  e_msg( "      emscripten" );
                  e_msg( "      gradle" );
                  e_msg( "      ninja" );
                  e_msg( "      wasm" );
                  e_msg( "    Compiling:" );
                  e_msg( "      {--cross=|-x}<arch,<sub>,<vendor>,<sys>,<env>" );
                  e_msg( "        arch   : x86_64 i386 arm, etc." );
                  e_msg( "        sub    : v6m, etc." );
                  e_msg( "        vendor : pc apple nvidia ibm, etc." );
                  e_msg( "        sys    : none linux win32 darwin cuda, etc." );
                  e_msg( "        env    : eabi gnu android macho elf, etc." );
                  e_msg( "      --{macho|elf|pe}" );
                  e_msg( "      --{sse|neon}" );
                  #if e_compiling( experimental )
                    e_msg( "    Imports:" );
                    e_msg( "      {--import=|-i}<file[.text|.lua|.py]>" );
                  #endif
                  e_msg( "    Globals:" );
                  e_msg( "      --c{++|pp|xx}{23|20|17|14|11}" );
                  e_msg( "      --unity" );
                  e_msg( "      --clean" );
                  e_msg( "      -opath # Write projects to 'path' dir+file" );
                  e_msg( "    Options:" );
                  e_msg( "      when \"xcode\"" );
                  e_msg( "        --xcode-v15" );
                  e_msg( "        --xcode-v12" );
                  e_msg( "        --xcode-v11" );
                  e_msg( "      when \"vs2022[=v143]\"" );
                  e_msg( "        --maxplugin=ext" );
                  e_msg( "      when \"ninja\"" );
                  e_msg( "      when \"emscripten\" \\__ Web Assembly" );
                  e_msg( "      when \"wasm\"       /" );
                  e_msg( "    Cvars:" );
                  e_msg( "      --cvar=ENABLE_PTHREADS" );
                  e_msg( "      --cvar=VERBOSE_LOGGING" );
                  e_msg( "      --cvar=SPEW" );
                  #if e_compiling( experimental )
                    // TODO: Add a bunch of functionality for packing (Writer::pack() )
                    // TODO: ...plus saving to a .c datafile.
                    // TODO: These are python programs bounds together or just a
                    // TODO: text file that gets unpacked to a file.
                    e_msg( "    Tasks:" );
                    e_msg( "      {--task=|-t}<pattern>" );
                    e_msg( "        vimscript     : Generate vim scripts to include in your .vimrc." );
                    e_msg( "        \"pack path\" : Pack a text file and convert to C." );
                    e_msg( "        vulkan        : Generate boilerplate for Vulkan." );
                    e_msg( "        dx12          : Generate boilerplate for DX12." );
                  #endif
                  e_msg( "\n  * Relatively no relation to Minority Report" );
                  return 0;
                }
                break;
              }
            }
          }

        //}:                                      |
      //}:                                        |
      //------------------------------------------+-----------------------------

      if( !Workspace::bmp.all )
        return-1;
      generate( "cogfile.lua" );
      return 0;
    }

  //}:                                            |
//}:                                              |
//------------------------------------------------+-----------------------------
