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

#if !e_compiling( microsoft )
  #include<sys/utsname.h>
#endif

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================+=============================
//Statics:{                                       |

  Workspace*  Workspace::wsp = nullptr;
  string      Workspace::out = "tmp/";
  string      Workspace::crossCc;
  string      Workspace::cpu;

//}:                                              |
//Globals:{                                       |

  strings EON::gfc::g_vIncludeStatements;

//}:                                              |
//Private:{                                       |
  //saveProject:{                                 |

    namespace{
      void a_saveProject(
            const string& filename
          , const Workspace::Target& wstar ){
        wstar.setup();

        //----------------------------------------------------------------------
        // Save out the Xcode project.
        //----------------------------------------------------------------------

        if(( Workspace::bmp->bXcode11||
             Workspace::bmp->bXcode12||
             Workspace::bmp->bXcode15||
             Workspace::bmp->bXcode16 ) && e_isa<Workspace::Xcode>( &wstar )){

          //--------------------------------------------------------------------
          // Write the PBX format project inside xcodeproj package.
          //--------------------------------------------------------------------

          #if e_compiling( microsoft )
            auto* ss = _strdup( filename.path() );
          #else
            auto* ss = strdup( filename.path() );
          #endif
          auto* ee = strchr( ss, 0 )-2;
          while( ee > ss ){
            if( *ee == '/' ){
              break;
            }
            --ee;
          }
          *ee = 0;
          const auto& xcodeProj = static_cast<const
            Workspace::Xcode&>( wstar );
          const auto& dirPath = string( ss, ee )
            + "/" + xcodeProj.toLabel()
            + ".xcodeproj";
          free( cp( ss ));
          Writer fs( dirPath
            + "/project.pbxproj"
            , kTEXT );
          wstar.serialize( fs );
          fs.save();

          //--------------------------------------------------------------------
          // Write the entitlements file.
          //--------------------------------------------------------------------

          if((( xcodeProj.toBuild() == "application"_64 ) &&
              ( xcodeProj.toFlags()->bDisablePageProtection ||
                xcodeProj.toFlags()->bDisableLibValidation  ||
                xcodeProj.toFlags()->bEnableJIT ))){
            xcodeProj.saveEntitlements( Workspace::out );
          }
        }

        //----------------------------------------------------------------------
        // Save out the Ninja project for Unix, Linux and Android Studio. This
        // isn't used for NDK and gradle stuff; it writes out too many files.
        //----------------------------------------------------------------------

        if( Workspace::bmp->bNinja &&
              e_isa<Workspace::Ninja>( &wstar )){
          const auto& ninja = static_cast<
            const Workspace::Ninja&>( wstar );
          Writer fs( filename, kTEXT );
          ninja.serialize( fs );
          fs.save();
        }

        //----------------------------------------------------------------------
        // Save out the Visual Studio 2019-2022 project.
        //----------------------------------------------------------------------

        if(( Workspace::bmp->bVS2019 ||
             Workspace::bmp->bVS2022 ) &&
              e_isa<Workspace::MSVC>( &wstar )){
          const auto& dirPath = filename.path();
          const auto& vcxproj = static_cast<const Workspace::MSVC&>( wstar );
          const auto& prjName = dirPath
            + vcxproj.toLabel()
            + ".vcxproj";
          Writer fs( prjName, kTEXT );
          wstar.serialize( fs );
          fs.save();
        }
        wstar.purge();
      }
    }

  //}:                                            |
//}:                                              |
//Extends:{                                       |

#ifdef __APPLE__
  #pragma mark - (extends)  -
#endif

  e_specialized_extends( Workspace::Project<PROJECT_SLOTS_XCODE> );
  e_specialized_extends( Workspace::Project<PROJECT_SLOTS_NINJA> );
  e_specialized_extends( Workspace::Project<PROJECT_SLOTS_MSVC> );
  e_extends( Workspace );

//}:                                              |
//Methods:{                                       |
  //[workspace]:{                                 |
    //serializeSln2022:{                          |

#ifdef __APPLE__
  #pragma mark - (methods) -
#endif

      void Workspace::serializeSln2022( Writer& fs )const{
        if( m_tStates->bVS2022 && ( fs.toFilename().ext().tolower().hash() == ".sln"_64 )){

          //--------------------------------------------------------------------
          // Construct vcxproj's header.
          //--------------------------------------------------------------------

          fs << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
          fs << "# Visual Studio 16\n";
          fs << "VisualStudioVersion = 16.0.29709.97\n";
          fs << "MinimumVisualStudioVersion = 10.0.40219.1\n";

          //--------------------------------------------------------------------
          // Construct vcxproj's for libraries and applications.
          //--------------------------------------------------------------------

          // https://www.codeproject.com/Reference/720512/List-of-Visual-Studio-Project-Type-GUIDs
          const string& wsguid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"/* C++ GUID */;
          auto it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<MSVC>() ){
              const auto& proj = it->as<MSVC>().cast();
              fs << "Project(\""
                + wsguid + "\") = \""
                + proj.toLabel()
                + "\", \""
                + proj.toLabel()
                + ".vcxproj\", \""
                + proj.toProjectGUID()
                + "\"\n";
              auto dependencies = proj.toLinkWith();
              auto projDependencies = proj.toDependencies();
              if( !dependencies.empty() && !projDependencies ){
                dependencies << ",";
              }
              dependencies << projDependencies;
              if( !dependencies.empty() ){
                fs << "\tProjectSection(ProjectDependencies) = postProject\n";
                dependencies.replace( "\t", "" );
                dependencies.replace( "\n", "" );
                dependencies.replace( " ", "" );
                auto linkages = dependencies.splitAtCommas();
                auto i2 = m_vTargets.getIterator();
                while( i2 ){
                  if( i2->isa<MSVC>() ){
                    if( (*it)->UUID != (*i2)->UUID ){
                      auto i3 = linkages.getIterator();
                      while( i3 ){
                        const auto& A = (*i2).as<MSVC>()->toLabel().base().tolower();
                        const auto& B = (*i3).base().tolower();
                        if( A == B ){
                          const auto& guid = i2->as<MSVC>()->toProjectGUID();
                          fs << "\t\t" + guid + " = " + guid + "\n";
                          break;
                        }
                        ++i3;
                      }
                    }
                  }
                  ++i2;
                }
                fs << "\tEndProjectSection\n";
              }else{
                fs << "\tProjectSection(ProjectDependencies) = postProject\n";
                fs << "\tEndProjectSection\n";
              }
              fs << "EndProject\n";
              a_saveProject( fs.toFilename(), proj );
            }
            ++it;
          }

          //--------------------------------------------------------------------
          // Write out the Global sections.
          //--------------------------------------------------------------------

          fs << "Global\n";
          fs << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
          fs << "\t\tDebug|x64 = Debug|x64\n";
          fs << "\t\tRelease|x64 = Release|x64\n";
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
          it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<MSVC>() ){
              const auto& proj = it->as<MSVC>().cast();
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Debug|"+proj.toArchitecture()+".ActiveCfg = Debug|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Debug|"+proj.toArchitecture()+".Build.0 = Debug|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Release|"+proj.toArchitecture()+".ActiveCfg = Release|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Release|"+proj.toArchitecture()+".Build.0 = Release|"+proj.toArchitecture()+"\n";
            }
            ++it;
          }
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ExtensibilityGlobals) = postSolution\n";
          const auto& slnguid = string::guid();
          fs << "\t\tSolutionGuid = "
            + slnguid
            + "\n";
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ExtensibilityAddIns) = postSolution\n";
          fs << "\tEndGlobalSection\n";
          fs << "EndGlobal\n";

          //--------------------------------------------------------------------
          // We're done with this target so turn it off for the rest of the run.
          //--------------------------------------------------------------------

          const_cast<Workspace*>( this )
            ->m_tStates->bVS2022 = 0;
          bmp->bVS2022 = 0;
        }
      }

    //}:                                          |
    //serializeSln2019:{                          |

#ifdef __APPLE__
  #pragma mark - (methods) -
#endif

      void Workspace::serializeSln2019( Writer& fs )const{
        if( m_tStates->bVS2019 && ( fs.toFilename().ext().tolower().hash() == ".sln"_64 )){

          //--------------------------------------------------------------------
          // Construct vcxproj's header.
          //--------------------------------------------------------------------

          fs << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
          fs << "# Visual Studio 16\n";
          fs << "VisualStudioVersion = 16.0.29709.97\n";
          fs << "MinimumVisualStudioVersion = 10.0.40219.1\n";

          //--------------------------------------------------------------------
          // Construct vcxproj's for libraries and applications.
          //--------------------------------------------------------------------

          // https://www.codeproject.com/Reference/720512/List-of-Visual-Studio-Project-Type-GUIDs
          const string& wsguid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"/* C++ GUID */;
          auto it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<MSVC>() ){
              const auto& proj = it->as<MSVC>().cast();
              fs << "Project(\""
                + wsguid + "\") = \""
                + proj.toLabel()
                + "\", \""
                + proj.toLabel()
                + ".vcxproj\", \""
                + proj.toProjectGUID()
                + "\"\n";
              auto dependencies = proj.toLinkWith();
              auto projDependencies = proj.toDependencies();
              if( !dependencies.empty() && !projDependencies ){
                dependencies << ",";
              }
              dependencies << projDependencies;
              if( !dependencies.empty() ){
                fs << "\tProjectSection(ProjectDependencies) = postProject\n";
                dependencies.replace( "\t", "" );
                dependencies.replace( "\n", "" );
                dependencies.replace( " ", "" );
                auto linkages = dependencies.splitAtCommas();
                auto i2 = m_vTargets.getIterator();
                while( i2 ){
                  if( i2->isa<MSVC>() ){
                    if( (*it)->UUID != (*i2)->UUID ){
                      auto i3 = linkages.getIterator();
                      while( i3 ){
                        const auto& A = (*i2).as<MSVC>()->toLabel().base().tolower();
                        const auto& B = (*i3).base().tolower();
                        if( A == B ){
                          const auto& guid = i2->as<MSVC>()->toProjectGUID();
                          fs << "\t\t" + guid + " = " + guid + "\n";
                          break;
                        }
                        ++i3;
                      }
                    }
                  }
                  ++i2;
                }
                fs << "\tEndProjectSection\n";
              }else{
                fs << "\tProjectSection(ProjectDependencies) = postProject\n";
                fs << "\tEndProjectSection\n";
              }
              fs << "EndProject\n";
              a_saveProject( fs.toFilename(), proj );
            }
            ++it;
          }

          //--------------------------------------------------------------------
          // Write out the Global sections.
          //--------------------------------------------------------------------

          fs << "Global\n";
          fs << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
          fs << "\t\tDebug|x64 = Debug|x64\n";
          fs << "\t\tRelease|x64 = Release|x64\n";
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
          it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<MSVC>() ){
              const auto& proj = it->as<MSVC>().cast();
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Debug|"+proj.toArchitecture()+".ActiveCfg = Debug|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Debug|"+proj.toArchitecture()+".Build.0 = Debug|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Release|"+proj.toArchitecture()+".ActiveCfg = Release|"+proj.toArchitecture()+"\n";
              fs << "\t\t"
                + proj.toProjectGUID()
                + ".Release|"+proj.toArchitecture()+".Build.0 = Release|"+proj.toArchitecture()+"\n";
            }
            ++it;
          }
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ExtensibilityGlobals) = postSolution\n";
          const auto& slnguid = string::guid();
          fs << "\t\tSolutionGuid = "
            + slnguid
            + "\n";
          fs << "\tEndGlobalSection\n";
          fs << "\tGlobalSection(ExtensibilityAddIns) = postSolution\n";
          fs << "\tEndGlobalSection\n";
          fs << "EndGlobal\n";
          bmp->bVS2019 = 0;
        }
      }

    //}:                                          |
    //serializeXcode:{                            |

      void Workspace::serializeXcode( Writer& fs )const{
        if(( m_tStates->bXcode11 ||
             m_tStates->bXcode12 ||
             m_tStates->bXcode15 ||
             m_tStates->bXcode16 )&&( fs
           . toFilename()
           . ext()
           . tolower()
           . hash() == ".xcworkspacedata"_64 )){
          fs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
          fs << "<Workspace\n";
          fs << "  version = \"1.0\">\n";

          //--------------------------------------------------------------------
          // Sort targets.
          //--------------------------------------------------------------------

          const auto& onSort = [](
                const auto& a
              , const auto& b )->bool{
            if( !a.template isa<Project<PROJECT_SLOTS_XCODE>>() )
              return false;
            if( !b.template isa<Project<PROJECT_SLOTS_XCODE>>() )
              return false;
            return(
                a.template as<Project<PROJECT_SLOTS_XCODE>>()->toLabel()
              < b.template as<Project<PROJECT_SLOTS_XCODE>>()->toLabel()
            );
          };
          auto& me = *const_cast<Workspace*>( this );
          me.m_vTargets.sort( onSort );

          //--------------------------------------------------------------------
          // Count the longest project name (line).
          //--------------------------------------------------------------------

          auto labelMax = 0u;
             { auto it = me.m_vTargets.getIterator();
               while( it ){
                 if( it->isa<Xcode>() ){
                   const auto& proj = it->as<Xcode>().cast();
                   labelMax = e_max<u32>( labelMax
                     , u32( proj.toLabel().len() )
                   );
                 }
                 ++it;
               }
             }

          //--------------------------------------------------------------------
          // Construct xcodeproj's for libraries.
          //--------------------------------------------------------------------

          fs << "  <Group\n";
          fs << "    location = \"container:\"\n";
          fs << "    name = \"Libraries\">\n";
          auto it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<Xcode>() ){
              const auto& proj = it->as<Xcode>().cast();
              switch( proj
                  . toBuild()
                  . tolower()
                  . hash() ){
                case"framework"_64:
                  [[fallthrough]];
                case"shared"_64:
                  [[fallthrough]];
                case"static"_64:
                  fs << "  <FileRef\n";
                  fs << "    location = \"group:" + proj.toLabel() + ".xcodeproj\">\n";
                  fs << "  </FileRef>\n";
                  a_saveProject( fs.toFilename(), proj );
                  break;
                default:
                  break;
              }
            }
            ++it;
          }
          fs << "  </Group>\n";

          //--------------------------------------------------------------------
          // Construct xcodeproj's for bundles.
          //--------------------------------------------------------------------

          fs << "  <Group\n";
          fs << "    location = \"container:\"\n";
          fs << "    name = \"Bundles\">\n";
          it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<Xcode>() ){
              const auto& proj = it->as<Xcode>().cast();
              switch( proj.toBuild().tolower().hash() ){
                case"bundle"_64:
                  fs << "  <FileRef\n";
                  fs << "    location = \"group:" + proj.toLabel() + ".xcodeproj\">\n";
                  fs << "  </FileRef>\n";
                  a_saveProject( fs.toFilename(), proj );
                  break;
              }
            }
            ++it;
          }
          fs << "  </Group>\n";

          //--------------------------------------------------------------------
          // Construct xcodeproj's for applications.
          //--------------------------------------------------------------------

          fs << "  <Group\n";
          fs << "    location = \"container:\"\n";
          fs << "    name = \"Apps\">\n";
          it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<Xcode>() ){
              const auto& proj = it->as<Xcode>().cast();
              switch( proj.toBuild().tolower().hash() ){
                case"application"_64:
                  [[fallthrough]];
                case"console"_64:
                  fs << "  <FileRef\n";
                  fs << "    location = \"group:"+proj.toLabel()+".xcodeproj\">\n";
                  fs << "  </FileRef>\n";
                  a_saveProject( fs.toFilename(), proj );
                  break;
              }
            }
            ++it;
          }
          fs << "  </Group>\n";
          fs << "</Workspace>\n";
          bmp->bXcode11 = 0;
          bmp->bXcode12 = 0;
          bmp->bXcode15 = 0;
          bmp->bXcode16 = 0;
        }
      }

    //}:                                          |
    //serializeNinja:{                            |

      void Workspace::serializeNinja( Writer& fs )const{
        if( bmp->bNinja ){

          //--------------------------------------------------------------------
          // Setup main ninja file.
          //--------------------------------------------------------------------

          const string commentLine = "#---------------------------------------"
            "----------------------------------------\n";
          const string jokeLine = "#                   The best method for acc"
            "elerating a computer\n#                      is the one that boos"
            "ts it by 9.8 m/s2.\n";
          fs << commentLine
             << jokeLine
             << commentLine
             << "# GENERATED FILE DO NOT EDIT IN ANY WAY SHAPE OR FORM SOMETHIN"
               "G BAD WILL HAPPEN\n"
             << commentLine
             << "\nninja_required_version = 1.11.1\n\n";

          //--------------------------------------------------------------------
          // Handle Ninja targets.
          //--------------------------------------------------------------------

          auto it = m_vTargets.getIterator();
          while( it ){
            if( it->isa<Ninja>() ){
              const auto& ninja_target = it->as<Ninja>().cast();
              fs << "include " + ninja_target.toLabel() + ".rules\n";
              a_saveProject(
                Workspace::out
                + ninja_target.toLabel()
                + ".rules"
                , ninja_target
              );
            }
            ++it;
          }

          //--------------------------------------------------------------------
          // Add build statements.
          //--------------------------------------------------------------------

          if( !m_vTargets.empty() )
            fs << "\n";
          it = m_vTargets.getIterator();
          using P = Project<PROJECT_SLOTS_NINJA>;
          P::Files files;
          while( it ){
            const auto& hTarget = *it;
            if( hTarget.isa<Ninja>() ){

              //----------------------------------------------------------------
              // Add files.
              //----------------------------------------------------------------

              const auto& ninja_target = hTarget.as<Ninja>().cast();
              ninja_target.inSources( Ninja::Type::kCpp ).foreach( [&]( const auto& fi ){ files.push( fi ); });
              ninja_target.inSources( Ninja::Type::kC   ).foreach( [&]( const auto& fi ){ files.push( fi ); });

              //----------------------------------------------------------------
              // Create the source file build step.
              //----------------------------------------------------------------

              fs << commentLine
                 << "# Project \""
                 << ninja_target.toLabel()
                 << "\"\n"
                 << commentLine
                 << "\n";
              const auto& bld = ninja_target.toBuild().tolower();
              const auto& lbl = ninja_target.toLabel();
              files.foreach(
                [&]( const File& file ){

                  //------------------------------------------------------------
                  // Switch on the lowercase extension.
                  //------------------------------------------------------------

                  const auto& str = static_cast<const string&>( file );
                  const auto& ext = str.ext().tolower();
                  switch( ext.hash() ){

                    //----------------------------------------------------------
                    // C++ builds.
                    //----------------------------------------------------------

                    case".cpp"_64:
                      fs << "build "
                         << ".intermediate/"
                         << lbl
                         << "/"
                         << str.filename()
                         << ".o: CXX_"
                         << lbl.toupper()
                         << " ../"
                         << str
                         << "\n";
                      switch( toLanguage() ){
                        case "c++23"_64:
                          fs << "  CXX_FLAGS =";
                          fs << " -Wc++23-extensions";
                          fs << " -std=c++23";
                          fs << "\n";
                          break;
                        case "c++20"_64:
                          fs << "  CXX_FLAGS =";
                          fs << " -Wc++20-extensions";
                          fs << " -std=c++20";
                          fs << "\n";
                          break;
                        case "c++17"_64:
                          fs << "  CXX_FLAGS =";
                          fs << " -Wc++17-extensions";
                          fs << " -std=c++17";
                          fs << "\n";
                          break;
                        case "c++14"_64:
                          fs << "  CXX_FLAGS =";
                          fs << " -Wc++14-extensions";
                          fs << " -std=c++14";
                          fs << "\n";
                          break;
                        case "c++11"_64:
                          fs << "  CXX_FLAGS =";
                          fs << " -std=c++11";
                          fs << "\n";
                          break;
                      }
                      break;

                    //----------------------------------------------------------
                    // C builds.
                    //----------------------------------------------------------

                    case".c"_64:
                      fs << "build "
                         << ".intermediate/"
                         << lbl
                         << "/"
                         << str.filename()
                         << ".o: C_"
                         << lbl.toupper()
                         << " ../"
                         << str
                         << "\n";
                      break;

                    //----------------------------------------------------------
                    // Exclude everything else.
                    //----------------------------------------------------------

                    default:
                      e_msgf( "Skipping extension: %s", ccp( ext ));
                      return;
                  }

                  //------------------------------------------------------------
                  // OBJECT files/directories.
                  //------------------------------------------------------------

                  if( !ninja_target.toIncludePaths().empty() ){
                    fs << "  OBJECT_DIR = "
                       << ".intermediate/"
                       << lbl
                       << "\n";
                    fs << "  OBJECT_FILE_DIR = "
                       << ".intermediate/"
                       << lbl
                       << "\n";
                    fs << "  TARGET_COMPILE_PDB = "
                       << ".intermediate/"
                       << lbl
                       << "\n";
                    fs << "  TARGET_PDB = "
                       << ".intermediate/"
                       << lbl
                       << ".pdb\n";
                    fs << "\n";
                  }
                }
              );

              //----------------------------------------------------------------
              // Handle the static/shared library build step.
              //----------------------------------------------------------------

              const auto& upr = lbl.toupper();
              const auto& lwr = lbl.tolower();
              switch( bld.hash() ){

                //--------------------------------+-----------------------------
                //Shared (dylib, dll, so):{       |

                  case"shared"_64:/**/{
                    e_msgf( "  Creating %s", ccp( lwr ));
                    fs << "build .output/lib"
                       << lbl;
                    if( bmp->bCrossCompile ){
                      if( crossCc.find( "linux" )){
                        fs << ".so: SHARED_LIB_";
                      }else if( crossCc.find( "pc" )){
                        fs << ".dll: SHARED_LIB_";
                      }else if( crossCc.find( "apple" )){
                        fs << ".dylib: SHARED_LIB_";
                      }else{
                        e_msg( "Bad cross-compiler triple: using this platform." );
                        #if e_compiling( osx )
                          fs << ".dylib: SHARED_LIB_";
                        #elif e_compiling( linux )
                          fs << ".so: SHARED_LIB_";
                        #elif e_compiling( microsoft )
                          fs << ".dll: SHARED_LIB_";
                        #else
                          fs << ": SHARED_LIB_";
                        #endif
                      }
                    }else{
                      #if e_compiling( osx )
                        fs << ".dylib: SHARED_LIB_";
                      #elif e_compiling( linux )
                        fs << ".so: SHARED_LIB_";
                      #elif e_compiling( microsoft )
                        fs << ".dll: SHARED_LIB_";
                      #else
                        fs << ": SHARED_LIB_";
                      #endif
                    }
                    fs << upr;
                    files.foreach(
                      [&]( const File& file ){
                        fs << " ";
                        const auto& lbl = static_cast<const string&>( file );
                        const auto& ext = lbl.ext().tolower();
                        switch( ext.hash() ){
                          case".cpp"_64:
                            [[fallthrough]];
                          case".c"_64:
                            fs << ".intermediate/"
                               << ninja_target.toLabel()
                               << "/"
                               << lbl.filename()
                               << ".o";
                            break;
                        }
                      }
                    );
                    fs << "\n  OBJECT_DIR = .output"
                       << "\n  POST_BUILD = :"
                       << "\n  PRE_LINK = :"
                       << "\n  TARGET_FILE = .output/lib"
                       << lbl;
                    if( bmp->bCrossCompile ){
                      if( crossCc.find( "linux" )){
                        fs << ".so";
                      }else if( crossCc.find( "pc" )){
                        fs << ".dll";
                      }else{
                        fs << ".dylib";
                      }
                    }else{
                      #if e_compiling( linux )
                        fs << ".so";
                      #elif e_compiling( osx )
                        fs << ".dylib";
                      #elif e_compiling( microsoft )
                        fs << ".dll";
                      #else
                        e_break( "Please define a platform!" );
                      #endif
                    }
                    fs << "\n  TARGET_PDB = "
                       << lbl
                       << ".so.dbg\n"
                       << "default .output/lib"
                       << lbl;
                    if( bmp->bCrossCompile ){
                      if( crossCc.find( "linux" )){
                        fs << ".so";
                      }else if( crossCc.find( "pc" )){
                        fs << ".dll";
                      }else{
                        fs << ".dylib";
                      }
                    }else{
                      #if e_compiling( linux )
                        fs << ".so";
                      #elif e_compiling( osx )
                        fs << ".dylib";
                      #elif e_compiling( microsoft )
                        fs << ".dll";
                      #else
                        e_break( "Please define a platform!" );
                      #endif
                    }
                    fs << "\n\n";
                    break;
                  }

                //}:                              |
                //Static (obj a):{                |

                  case"static"_64:/**/{
                    e_msgf( "  Creating %s", ccp( lbl.tolower() ));
                    fs << "build .output/lib"
                       << lbl
                       << ".a: STATIC_LIB_"
                       << upr;
                    files.foreach(
                      [&]( const File& file ){
                        const auto& lbl = static_cast<const string&>( file );
                        const auto& ext = lbl.ext().tolower();
                        switch( ext.hash() ){
                          case".cpp"_64:
                            [[fallthrough]];
                          case".c"_64:
                            fs << " .intermediate/"
                               << ninja_target.toLabel()
                               << "/"
                               << lbl.filename()
                               << ".o";
                            break;
                        }
                      }
                    );
                    fs << "\n  OBJECT_DIR = .output"
                       << "\n  POST_BUILD = :"
                       << "\n  PRE_LINK = :"
                       << "\n  TARGET_FILE = .output/lib"
                       << lbl
                       << ".a"
                       << "\n  TARGET_PDB = "
                       << lbl
                       << ".a.dbg\n"
                       << "default .output/lib"
                       << lbl
                       << ".a\n\n";
                    break;
                  }

                //}:                              |
                //--------------------------------+-----------------------------
              }
            }
            files.clear();
            ++it;
          }

          //--------------------------------------------------------------------
          // We're done making sources, static and shared libaries so it's time
          // to produce the applications at the end.
          //--------------------------------------------------------------------

          it = m_vTargets.getIterator();
          while( it ){
            const auto& hTarget = *it;
            if( hTarget.isa<Ninja>() ){

              //----------------------------------------------------------------
              // Sort files.
              //----------------------------------------------------------------

              const auto& ninja_target = hTarget.as<Ninja>().cast();
              ninja_target.inSources( Ninja::Type::kCpp ).foreach( [&]( const auto& fi ){ files.push( fi ); });
              ninja_target.inSources( Ninja::Type::kC   ).foreach( [&]( const auto& fi ){ files.push( fi ); });

              //----------------------------------------------------------------
              // Handle the console/application build step.
              //----------------------------------------------------------------

              const auto& lbl = ninja_target.toLabel();
              const auto& lwr = lbl.tolower();
              const auto& upr = lbl.toupper();
              const auto& bld = ninja_target.toBuild().tolower();
              switch( bld.hash() ){

                //--------------------------------+-----------------------------
                //Greetings Programs:{            |

                  case"application"_64:
                    break;

                  case"console"_64:/**/{

                    //----------------------------------------------------------
                    // Create 'build' line.
                    //----------------------------------------------------------

                    e_msgf( "  Creating %s", ccp( lwr ));
                    fs << commentLine
                       << "# Applications\n"
                       << commentLine
                       << "\n"
                       << "build .output/"
                       << lwr;
                    if( bmp->bWasm ){
                       fs << ": WASM_LINKER_" << upr;
                    }else if( bmp->bCrossCompile ){
                      if( crossCc.find( "linux" ))
                         fs << ": ELF_LINKER_" << upr;
                      else if( crossCc.find( "pc" ))
                         fs << ": PE_LINKER_" << upr;
                      else if( crossCc.find( "apple" ))
                         fs << ": MACHO_LINKER_" << upr;
                      else{
                        #if e_compiling( linux )
                           fs << ": ELF_LINKER_" << upr;
                        #elif e_compiling( osx )
                           fs << ": MACHO_LINKER_" << upr;
                        #elif e_compiling( microsoft )
                           fs << ": PE_LINKER_" << upr;
                        #else
                          e_break( "Please define a platform!" );
                        #endif
                      }
                    }else{
                      #if e_compiling( linux )
                         fs << ": ELF_LINKER_" << upr;
                      #elif e_compiling( osx )
                         fs << ": MACHO_LINKER_" << upr;
                      #elif e_compiling( microsoft )
                         fs << ": PE_LINKER_" << upr;
                      #else
                        e_break( "Please define a platform!" );
                      #endif
                    }

                    //----------------------------------------------------------
                    // Reference all files in the console project.
                    //----------------------------------------------------------

                    files.foreach(
                      [&]( const File& file ){
                        const auto& lbl = static_cast<const string&>( file );
                        const auto& ext = lbl.ext().tolower();
                        switch( ext.hash() ){
                          case".cpp"_64:
                            [[fallthrough]];
                          case".c"_64:
                            fs << " .intermediate/"
                               << m_sName
                               << "/"
                               << lbl.filename()
                               << ".o";
                            break;
                        }
                      }
                    );

                    //----------------------------------------------------------
                    // Link against libraries.
                    //----------------------------------------------------------

                    const auto libs = ninja_target.toLinkWith().splitAtCommas();
                    fs << "\n  LINK_LIBRARIES =";
                    libs.foreach(
                      [&]( const string& lib ){
                        string ext;
                        if( bmp->bCrossCompile ){
                          if( crossCc.find( "linux" )){
                            ext << ".so";
                          }else if( crossCc.find( "pc" )){
                            ext << ".dll";
                          }else if( crossCc.find( "apple" )){
                            ext << ".dylib";
                          }else{
                            #if e_compiling( linux )
                              ext << ".so";
                            #elif e_compiling( osx )
                              ext << ".dylib";
                            #elif e_compiling( microsoft )
                              ext << ".dll";
                            #else
                              e_break( "Unsupported OS target" );
                            #endif
                          }
                        }else{
                          #if e_compiling( linux )
                            ext << ".so";
                          #elif e_compiling( osx )
                            ext << ".dylib";
                          #elif e_compiling( microsoft )
                            ext << ".dll";
                          #else
                            e_break( "Unsupported OS target" );
                          #endif
                        }
                        if(( e_fexists( "/usr/lib/" + cpu + "-linux-gnu/lib" + lib + ".a" ))||
                           ( e_fexists( "/usr/lib/" + cpu + "-linux-gnu/lib" + lib + ext  ))){
                          fs << " -L/usr/lib/" + cpu + "-linux-gnu -l" << lib;
                        }else if(( e_fexists( "/usr/lib/lib" + lib + ".a" ))||
                                 ( e_fexists( "/usr/lib/lib" + lib + ext ))){
                          fs << " -L/usr/lib/lib -l" << lib;
                        }else if( e_fexists( "/usr/lib/" + lib )){
                          fs << " -l/usr/lib/" << lib;
                        }else if(( *lib != '/' )&&( *lib != '~' )&&( *lib != '.' )){
                          fs << " .output/" << lib;
                        }else{
                          fs << " " << lib;
                        }
                      }
                    );
                    fs << "\n  TARGET_FILE = .output/"
                       << lwr.base()
                       << "\n  OBJECT_DIR = .intermediate/"
                       << lwr.base()
                       << "\n  TARGET_PDB = "
                       << lwr.base()
                       << ".dbg"
                       << "\n  POST_BUILD = :"
                       << "\n  PRE_LINK = :"
                       << "\ndefault .output/"
                       << lwr.base()
                       << "\n\n";
                    break;
                  }

                //}:                              |
                //--------------------------------+-----------------------------
              }
            }
            files.clear();
            ++it;
          }
          bmp->bNinja = 0;
        }
      }

    //}:                                          |
    //serializeGradle:{                           |

      void Workspace::serializeGradle( Writer& fs )const{

        //----------------------------------------------------------------------
        // Bail conditions.
        //----------------------------------------------------------------------

        //https://developer.android.com/studio/build
        if( !bmp->bGradle )
          return;

        //----------------------------------------------------------------------
        // Write the tmp/settings.gradle and tmp/build.gradle project files.
        //----------------------------------------------------------------------

        fs << "pluginManagement{\n"
           << "  repositories{\n"
           << "    gradlePluginPortal()\n"
           << "    mavenCentral()\n"
           << "    google()\n"
           << "  }\n"
           << "}\n";
        const auto& ndk_root
          = fs
          . toFilename()
          . path();
        fs << "rootProject.name='"
           << toName()
           << "'\n";
        // Build the build.gradle file for the project root.
        Writer rootPrj( ndk_root
          + "/build.gradle"
          , kTEXT );
        e_msgf( "Generating %s", ccp( rootPrj.toFilename() ));
        rootPrj << "plugins{\n";
        auto targets( m_vTargets );
        #if 0
          targets.sort(
            []( const auto& a, const auto& b ){
              return(
                  a.template as<NDK>()->toLabel().len()
                > b.template as<NDK>()->toLabel().len()
              );
            }
          );
        #endif
        // Build the gradle.properties file; won't do much yet, fill as needed.
        Writer propPrj( ndk_root
          + "/gradle.properties"
          , kTEXT );
        propPrj << "org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8\n";
        propPrj << "android.nonTransitiveRClass=true\n";
        propPrj << "android.useAndroidX=true\n";
        propPrj.save();
        // Build the local.properties file; won't do much yet, fill as needed.
        Writer propLocal( ndk_root
          + "/local.properties"
          , kTEXT );
        propLocal.save();
        // Only pull in the stuff we need; scan targets to figure that out; one
        // can only write an id line once, hence the 'ap' and 'lb' booleans.
        auto it = targets.getIterator();
        auto ap = false;
        auto lb = false;
        while( it ){
          if( it->isa<NDK>() ){
            const auto& build = it->as<NDK>()->toBuild().tolower();
            if( !ap && build == "application"_64 ){
              rootPrj << "  id 'com.android.application' version '8.10.2' apply false\n";
              ap = true;
            }else if( !lb && build == "shared"_64 ){
              rootPrj << "  id 'com.android.library' version '8.10.2' apply false\n";
              lb = true;
            }else if( !lb && build == "static"_64 ){
              rootPrj << "  id 'com.android.library' version '8.10.2' apply false\n";
              lb = true;
            }
          }
          ++it;
        }
        rootPrj << "}\n";
        rootPrj.save();

        //----------------------------------------------------------------------
        // Add all sub-directories to gradle.settings file. This is logically a
        // workspace/solution file. THe anchor of the project.
        //----------------------------------------------------------------------

        it = targets.getIterator();
        while( it ){
          if( it->isa<NDK>() ){
            const auto& ndk_proj = it->as<NDK>().cast();
            const auto& ndk_name = ndk_proj.toLabel();
            fs << "include':"
               << ndk_name
               << "'\n";
            const auto& ndk_path
              = ndk_root
              + ndk_name;
            if( !ndk_proj.inSources( NDK::Type::kHpp ).empty()||
                !ndk_proj.inSources( NDK::Type::kH   ).empty() )
              e_mkdir( ndk_path + "/" + kSourceSet + "/public" );
            if( !ndk_proj.inSources( NDK::Type::kCpp ).empty() )
              e_mkdir( ndk_path + "/" + kSourceSet + "/cpp" );
            #if 0 // TODO: Need a big switch on hashed extension here.
              e_mkdir( ndk_path + "/" + kSourceSet + "/res" );
            #endif
            if( !ndk_proj.inSources( NDK::Type::kC ).empty() )
              e_mkdir( ndk_path + "/" + kSourceSet + "/c" );
            Writer subPrj( ndk_path
              + "/build.gradle"
              , kTEXT );
            e_msgf( "Generating %s"
              , ccp( subPrj
              . toFilename() ));
            ndk_proj
              . serialize( subPrj );
            subPrj.save();
          }
          ++it;
        }
        bmp->bGradle = 0;
      }

    //}:                                          |
    //serialize:{                                 |

      void Workspace::serialize( Writer& fs )const{
        serializeSln2019( fs );
        serializeSln2022( fs );
        serializeXcode(   fs );
        serializeNinja(   fs );
        serializeGradle(  fs );
      }

    //}:                                          |
    //isUnityBuild:{                              |

      bool Workspace::isUnityBuild(){
        auto it = IEngine::args.getIterator();
        while( it ){
          if( it->tolower().hash() == e_hashstr64_const( "--unity" )){
            return true;
          }
          ++it;
        }
        return false;
      }

    //}:                                          |
    //addToFiles:{                                |

      bool Workspace::addToFiles( Files& files, const Files& s ){
        auto it = s.getIterator();
        while( it ){
          File f( *it );
          if( !File::filerefs.find( f.toFileRef() ))
            File::filerefs.set( f.toFileRef(), string::streamId() );
          files.push( f );
          ++it;
        }
        return !files.empty();
      }

    //}:                                          |
    //isIgnored:{                                 |

      bool Workspace::isIgnored( const string& regex, const string& s ){
        if( regex.empty() )
          return false;
        const std::regex r( regex.c_str() );
        const std::string var( s );
        auto it = var.cbegin();
        std::smatch sm;
        return std::regex_search( it
          , var.cend()
          , sm
          , r
        );
      }

    //}:                                          |
    //ignore:{                                    |

      void Workspace::ignore( Files& files, const string& ignoring ){
        if( files.empty() )
          return;
        auto parts( ignoring.splitAtCommas() );
        auto pit = parts.getIterator();
        while( pit ){
          pit->erase( "\n" );
          pit->erase( "\t" );
          pit->erase( " " );
          auto it = files.getIterator();
          while( it ){
            const auto& splits = pit->splitAtCommas();
            auto ok = false;
            splits.foreachs(
              [&]( const auto& split ){
                if( isIgnored( split.tolower(), it->tolower() ))
                  ok = true;
                return!ok;
              }
            );
            if( ok ){
              it.erase();
              continue;
            }
            ++it;
          }
          ++pit;
        }
      }

    //}:                                          |
    //cleanup:{                                   |

      void Workspace::cleanup()const{
        const_cast<self*>( this )->m_vTargets.clear();
      }

    //}:                                          |
    //again:{                                     |

      void Workspace::again(){
        m_vTargets.foreach(
          [this]( auto& hTarget ){

            //------------------------------------------------------------------
            // Bail conditions.
            //------------------------------------------------------------------

            if( !hTarget )
              return;

            //------------------------------------------------------------------
            // Xcode projects get reset here.
            //------------------------------------------------------------------

            if( m_tStates->bXcode11 ||
                m_tStates->bXcode12 ||
                m_tStates->bXcode15 ||
                m_tStates->bXcode16 ){
              hTarget.template as<Xcode>()->clear();
            }
          }
        );
      }

    //}:                                          |
  //}:                                            |
  //getTargets:{                                  |

    strings Workspace::getTargets(){
      strings targets;
      if( bmp->osMac )
        targets.push( "macos" );
      if( bmp->osIphone )
        targets.push( "ios" );
      if( targets.empty() )
        targets.push( "macos" );
      return targets;
    }

  //}:                                            |
  //exists:{                                      |

    bool Workspace::exists( const u64 hash, const string& search, string& out ){

      //------------------------------------------------------------------------
      // Figure out various forms of the OS version and store in a f32.
      //------------------------------------------------------------------------

      static const auto osver( IEngine::osVersion() );
      static const auto osnum(
          string( osver.c_str()+8
        , strchr( osver.c_str()+8,' ' )).base() );
      static const f32 osverf( osnum.as<float>() );

      //------------------------------------------------------------------------
      // Run through various OS locations for frameworks and text-base-dylibs.
      //------------------------------------------------------------------------

      strings ejectors( search.splitAtCommas() );
      if( hash == "macos"_64 ){
        if( osverf >= 13.3 ){
          static constexpr ccp osMacXcodeLibDev13_3 =
            "/Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk/usr/lib/";
          ejectors.push( osMacXcodeLibDev13_3 );
        }else if( osverf >= 12.3 ){
          static constexpr ccp osMacXcodeLibDev12_3 =
            "/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/usr/lib/";
          ejectors.push( osMacXcodeLibDev12_3 );
        }
        static constexpr ccp osMacXcodeLibFrameworks =
          "/Applications/Xcode.app/Contents/Developer/Platforms/"
          "MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/"
          "Library/Frameworks/";
        static constexpr ccp osMacXcodeUsrLib =
          "/Applications/Xcode.app/Contents/Developer/Platforms/"
          "MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/";
        static constexpr ccp osLibManagingLib =
          "/Library/ManagedFrameworks/";
        ejectors.push( osMacXcodeLibFrameworks );
        ejectors.push( osMacXcodeUsrLib );//TBDs;
        ejectors.push( osLibManagingLib );//Libs;
        ejectors.push( "~/Library/Frameworks/" );
        ejectors.push(  "/Library/Frameworks/" );
      }else if( hash == "ios"_64 ){
        static constexpr const ccp kIosSdkUsrLib =
          "/Applications/Xcode.app/Contents/Developer/Platforms/"
          "iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/lib/";
        ejectors.push( kIosSdkUsrLib );
      }
      auto it = ejectors.getIterator();
      const auto path( out );
      out.clear();
      while( it ){
        string spec;
        if( it->back() != '/' ){
          spec = *it + "/" + path;
        }else{
          spec = *it + path;
        }
        if( e_dexists( spec )||
            e_fexists( spec )){
          out = std::move( spec );
          return true;
        }
        ++it;
      }
      return false;
    }

  //}:                                            |
  //system:{                                      |

    bool Workspace::File::isSystemFramework()const{
      const auto path=string( "/Applications/Xcode.app/Contents/Developer/Platforms/"
        "MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/"
        "Library/Frameworks/" ) + c_str() +
        ".framework";
      return e_dexists( path );
    }

  //}:                                            |
  //dir:{                                         |

    hashmap<u64,Workspace::Element> Workspace::dir( const ccp root ){
    hashmap<u64,Workspace::Element> ret;
      IEngine::dir( root/* from run directory */,
        [&]( const auto& subdir
           , const auto& label
           , const bool isDir ){
          if( !isDir ){
e_msg( subdir+label );
            const auto& cpPair = std::make_shared<std::pair<string,File>>( std::make_pair( label, File( subdir+label )));
            ret.set( label.os().tolower().hash(), cpPair );
          }
          return true;
        }
      );
      return ret;
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Ctor:{                                          |

  Workspace::Workspace()
      : m_tStates( bmp ){
    #if !e_compiling( microsoft )
      struct utsname buf;
      if( !uname( &buf ))
        cpu.cat( buf.machine );
    #elif e_compiling( intel )
      #if e_compiling( x64 )
        cpu.cat( "x86_64" );
      #else
        cpu.cat( "x86" );
      #endif
    #elif e_compiling( arm ) && e_compiling( x64 )
      cpu.cat( "arm64" );
    #endif
    wsp = this;
  }

//}:                                              |
//Dtor:{                                          |

  Workspace::~Workspace(){
    wsp = nullptr;
  }

//}:                                              |
//================================================+=============================
