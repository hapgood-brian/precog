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
//Configs:{                                       |

  namespace{
    constexpr ccp a_aConfigs[2]{ "Debug", "Release" };
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Methods:{                                       |
  //[project]:{                                   |
    //extFromBuildString:{                        |

      //https://help.autodesk.com/view/MAXDEV/2021/ENU/?guid=__developer_writing_plug_ins_creating_a_plug_in_project_manually_creating_a_new_plug_in__plug_in_file_extensions_html
      ccp Workspace::MSVC::extFromBuildString()const{
        switch( toBuild().hash() ){
          case"application"_64:
            [[fallthrough]];
          case"console"_64: return"exe";
          case"static"_64:  return"lib";
          case"shared"_64:  return"dll";
        }
        return nullptr;
      }

    //}:                                          |
    //extFromSource<>:{                           |

      ccp Workspace::MSVC::extFromEnum( const Type e )const{
        switch( e ){
          case decltype( e )::kDef:
            return".def";
          case decltype( e )::kCpp:
            return".cpp";
          case decltype( e )::kC:
            return".c";
          default:
            return"";
        }
      }

    //}:                                          |
    //writePropGroup:{                            |

      void Workspace::MSVC::writePropGroup( Writer& fs, const string& group, const string& config )const{
        switch( group.hash() ){
          case"Condition"_64:
            fs << "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\" Label=\"Configuration\">\n";
            switch( toBuild().hash() ){
              case"application"_64:
                [[fallthrough]];
              case"console"_64:
                fs << "\t<ConfigurationType>Application</ConfigurationType>\n";
                break;
              case"shared"_64:
                fs << "\t<ConfigurationType>DynamicLibrary</ConfigurationType>\n";
                break;
              case"static"_64:
                fs << "\t<ConfigurationType>StaticLibrary</ConfigurationType>\n";
                break;
            }
            if( !bmp->bUTF16 ){
              fs << "\t<CharacterSet>MultiByte</CharacterSet>\n";
            }else{
              fs << "\t<CharacterSet>Unicode</CharacterSet>\n";
            }
            fs << "\t<PlatformToolset>"+toPlatformTools()+"</PlatformToolset>\n";
            fs << "</PropertyGroup>\n";
            break;
        }
      }

      void Workspace::MSVC::writePropGroup( Writer& fs, const string& group )const{

        //----------------------------------------------------------------------
        // Handle target extensions for both Debug and Release.
        //----------------------------------------------------------------------

        if( "TargetExt"_64 == group.hash() ){
          for( u32 n=e_sizeof( a_aConfigs ), i=0; i<n; ++i ){
            fs << "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"
              + string( a_aConfigs[ i ])
              + "|"
              + m_sArchitecture
              + "'\">";
            fs << "\t<TargetExt>dll</TargetExt>\n";
            fs << "</PropertyGroup>\n";
          }
          return;
        }

        //----------------------------------------------------------------------
        // Handle all other property group types.
        //----------------------------------------------------------------------

        if( "<arch>"_64 != group.hash() ){
          fs << "<PropertyGroup Label=\""+group+"\">\n";
        }else{
          fs << "<PropertyGroup>\n";
        }
        switch( group.hash() ){
          case"UserMacros"_64:
            // TODO: Put in user macros here.
            break;
          case"Globals"_64:
            fs << "\t<ProjectGuid>"+toProjectGUID()+"</ProjectGuid>\n";
            fs << "\t<WindowsTargetPlatformVersion>"+toWindowsSDK()+"</WindowsTargetPlatformVersion>\n";
            fs << "\t<Keyword>Win32Proj</Keyword>\n";
            fs << "\t<Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "\t<ProjectName>"+toLabel()+"</ProjectName>\n";
            fs << "\t<VCProjectUpgraderObjectName>NoUpgrade</VCProjectUpgraderObjectName>\n";
            break;
          // Anything in <> is a special case and not sent through to the vcxproj.
          case"<arch>"_64:
            fs << "\t<PreferredToolArchitecture>"+toPreferredArch()+"</PreferredToolArchitecture>\n";
            break;
        }
        fs << "</PropertyGroup>\n";
      }

    //}:                                          |
    //writeSetDirectory:{                         |

      void Workspace::MSVC::writeSetDirectory( Writer& fs, const string& tag, const string& config, const string& path )const{
        fs << "<"+tag+" Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">$(SolutionDir)"+path+"\\$(Configuration)\\$(TargetName)\\$(PlatformTarget)\\</"+tag+">\n";
      }

    //}:                                          |
    //writeManfestData:{                          |

      void Workspace::MSVC::writeManifestData( Writer& fs, const string& config )const{
        fs << "<GenerateManifest Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+m_sGenManifest+"</GenerateManifest>\n";
      }

    //}:                                          |
    //wrieItemDefGroup:{                          |

      void Workspace::MSVC::writeItemDefGroup( Writer& fs, const string& config )const{
        fs << "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">\n";
        fs << "\t<ClCompile>\n";
        fs << "\t\t<AdditionalIncludeDirectories>";
        if( !toIncludePaths().empty() ){
          const auto& syspaths = toIncludePaths().splitAtCommas();
          strings paths;
          syspaths.foreach(
            [&]( const string& syspath ){
              if( syspath.empty() ){
                return;
              }
              if( *syspath == '/' ){
                paths.push( syspath );
              }else if( *syspath == '.' ){
                paths.push( syspath );
              }else if( syspath[ 1 ] == ':' ){
                paths.push( syspath );
              }else{
                paths.push( "../" + syspath );
              }
            }
          );
          auto it = paths.getIterator();
          while( it ){
            const auto& s = *it++;
            fs << s.os() + ";";
          }
        }
        fs << "%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
        fs << "\t\t<AssemblerListingLocation>$(IntDir)</AssemblerListingLocation>\n";
        fs << "\t\t<BasicRuntimeChecks>"+m_sBasicRuntimeChk+"</BasicRuntimeChecks>\n";
        fs << "\t\t<CompileAs>Default</CompileAs>\n";
        fs << "\t\t<DebugInformationFormat>"+m_sDebugInfoFormat+"</DebugInformationFormat>\n";
        fs << "\t\t<ExceptionHandling>"+m_sExceptionHndlng+"</ExceptionHandling>\n";
        if( !toPrefixHeader().empty() ){
          string prefixFile;
          switch( *toPrefixHeader() ){
            case'$':
            case'/':
            case'~':
              prefixFile = toPrefixHeader();
              break;
            default:
              prefixFile = "$(SolutionDir)../" + toPrefixHeader();
              break;
          }
          fs << "\t\t<ForcedIncludeFiles>"+prefixFile.os()+"</ForcedIncludeFiles>\n";
        }
        fs << "\t\t<InlineFunctionExpansion>";
        switch( config.hash() ){
          case"Debug"_64:
            fs << "Disabled";
            break;
          case"Release"_64:
            fs << "AnySuitable";
            break;
        }
        fs << "</InlineFunctionExpansion>\n";
        fs << "\t\t<LanguageStandard>";
        switch( Workspace::wsp->toLanguage() ){
          case 20: // TODO: Which is right? In xcode.cpp we don't use 20!
            fs << "stdcpp20";
            break;
          case 17:
            fs << "stdcpp17";
            break;
          case 14:
            fs << "stdcpp14";
            break;
          case 11:
            fs << "stdcpp11";
            break;
          default:
            switch( toLanguage().hash() ){
              case"c++20"_64:
              case"cxx20"_64:
              case"cpp20"_64:
                switch( toPlatformTools().hash() ){
                  case"v143"_64:
                    fs << "stdcpp20";
                    break;
                  default:
                    e_warnsf( "C++20 not supported by current toolchain." );
                    break;
                }
              break;
              case"c++17"_64:
              case"cxx17"_64:
              case"cpp17"_64:
                fs << "stdcpp17";
              break;
              case"c++14"_64:
              case"cxx14"_64:
              case"cpp14"_64:
                fs << "stdcpp14";
              break;
              case"c++11"_64:
              case"cxx11"_64:
              case"cpp11"_64:
                switch( toPlatformTools().hash() ){
                  case"v140"_64:
                    fs << "stdcpp11";
                    break;
                  default:
                    e_warnsf( "C++11 not supported by current toolchain." );
                    break;
                }
              break;
            }
            break;
        }
        fs << "</LanguageStandard>\n";
        fs << "\t\t<Optimization>";
        switch( config.hash() ){
          case"Debug"_64:
            fs << "Disabled";
            break;
          case"Release"_64:
            fs << "MaxSpeed";
            break;
        }
        fs << "</Optimization>\n";
        fs << "\t\t<PrecompiledHeader>"+m_sPCH+"</PrecompiledHeader>\n";
        switch( config.hash() ){
          case"Debug"_64:
            if( Workspace::bmp->bVSMTNoDLL ){
              fs << "\t\t<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>\n";
            }else{
              fs << "\t\t<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n";
            }
            break;
          case"Release"_64:
            if( Workspace::bmp->bVSMTNoDLL ){
              fs << "\t\t<RuntimeLibrary>MultiThreaded</RuntimeLibrary>\n";
            }else{
              fs << "\t\t<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\n";
            }
            break;
        }
        if( !toPrefixHeader().empty() ){
          fs << "\t\t<PrecompiledHeader>Use</PrecompiledHeader>\n";
          string prefixPath;
          if( *toPrefixHeader() == '$' ){
            prefixPath = toPrefixHeader();
          }else if( *toPrefixHeader() == '~' ){
            prefixPath = toPrefixHeader();
          }else if( *toPrefixHeader() == '/' ){
            prefixPath = toPrefixHeader();
          }else{
            prefixPath = "$(SolutionDir)../" + toPrefixHeader();
          }
          fs << "\t\t<PrecompiledHeaderFile>" << prefixPath << "</PrecompiledHeaderFile>\n";
        }
        fs << "\t\t<RuntimeTypeInfo>"+m_sRTTI+"</RuntimeTypeInfo>\n";
        fs << "\t\t<UseFullPaths>false</UseFullPaths>\n";
        fs << "\t\t<WarningLevel>"+m_sWarningLevel+"</WarningLevel>\n";
        fs << "\t\t<PreprocessorDefinitions>";
        switch( config.hash() ){
          case"Debug"_64:/**/{
            string defs = toDefinesDbg() + ";__compiling_" + toLabel().tolower()+"__=1";
            defs.replace( "\t", "" );
            defs.replace( "\n", "" );
            defs.replace( ",", ";" );
            defs.replace( " ", "" );
            fs << defs + ";";
            break;
          }
          case"Release"_64:/**/{
            string defs = toDefinesRel() + ";__compiling_" + toLabel().tolower()+"__=1";
            defs.replace( "\t", "" );
            defs.replace( "\n", "" );
            defs.replace( ",", ";" );
            defs.replace( " ", "" );
            fs << defs + ";";
            break;
          }
        }
        fs << "%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
        fs << "\t\t<ObjectFileName>$(IntDir)</ObjectFileName>\n";
        if( config.hash() == "Release"_64 ){
          fs << "\t\t<StringPooling>true</StringPooling>\n";
          fs << "\t\t<FunctionLevelLinking>true</FunctionLevelLinking>\n";
          fs << "\t\t<EnableEnhancedInstructionSet>AdvancedVectorExtensions</EnableEnhancedInstructionSet>\n";
          fs << "\t\t<IntrinsicFunctions>true</IntrinsicFunctions>\n";
          fs << "\t\t<FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>\n";
        }
        fs << "\t\t<AdditionalOptions>/bigobj /utf-8 %(AdditionalOptions)</AdditionalOptions>\n";
        fs << "\t</ClCompile>\n";
        fs << "\t<Link>\n";

        //----------------------------------------------------------------------
        // Additional Dependencies.
        //
        // Here we list all the libraries we wish to link with. They must have
        // the Windows .lib extension unless we already added them.
        //----------------------------------------------------------------------

        fs << "\t\t<AdditionalDependencies>";
        auto libs = toLinkWith();
        libs.erase( "\t" );
        libs.erase( "\n" );
        libs.erase( " " );
        auto libList = libs.splitAtCommas();
        libList.foreach(
          [&]( string& lib ){
            switch( lib.ext().tolower().hash() ){
              case".lib"_64:
                break;
              default:
                lib += ".lib";
                break;
            }
            fs << lib << ";";
          }
        );
        fs << "kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib";
        fs << "</AdditionalDependencies>\n";

        //----------------------------------------------------------------------
        // Additional library directories.
        //
        // Directories can be specified with $(SolutionDir), $(ProjectDir) etc.
        // and ~ or .
        //----------------------------------------------------------------------

        auto dirs = toLibraryPaths() + "," + toFindLibsPaths();
        dirs.erase( "\t" );
        dirs.erase( "\n" );
        dirs.erase( " " );
        const auto& dirList = dirs.splitAtCommas();
        libs.erase( ".lib" );
        fs << "\t\t<AdditionalLibraryDirectories>";
        Class::foreach<MSVC>(
          [&]( const MSVC& msvc ){
            switch( msvc.toBuild().hash() ){
              case"shared"_64:
              case"static"_64:
              fs << "$(SolutionDir).output/"
                 << config
                 << "/"
                 << msvc.toLabel()
                 << "/x64;";
                break;
              default:
                break;
            }
          }
        );
        dirs.replace( ",", ";" );
        auto it = dirList.getIterator();
        string path;
        while( it ){
          if(( *it )[ 1 ]==':' ){
            path = *it;
          }else{
            switch( **it ){
              case'~':
                [[fallthrough]];
              case'$':
                [[fallthrough]];
              case'/':
                [[fallthrough]];
              case'.':
                path = *it;
                break;
              default:
                path = "../" + *it;
                break;
            }
          }
          fs << path << ";";
          ++it;
        }
        fs << "%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";

        //----------------------------------------------------------------------
        // The rest of the vcxproj.
        //----------------------------------------------------------------------

        fs << "\t\t<AdditionalOptions>%(AdditionalOptions) /machine:"+m_sArchitecture+"</AdditionalOptions>\n";
        fs << "\t\t<GenerateDebugInformation>"+m_sGenReleaseDBInf+"</GenerateDebugInformation>\n";
        fs << "\t\t<IgnoreSpecificDefaultLibraries>%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>\n";
        fs << "\t\t<ProgramDataBaseFile>$(IntDir)"+toLabel()+".pdb</ProgramDataBaseFile>\n";
        if( !m_sDefinition.empty() ){
          auto path = m_sDefinition;
          if( *path != '/' )
          if( *path != '~' )
          if( *path != '.' ){
            path = "../" + path;
          }
          fs << "\t\t<ModuleDefinitionFile>../" + path + "</ModuleDefinitionFile>\n";
        }
        switch( toBuild().hash() ){
          case"application"_64:
            fs << "\t\t<SubSystem>Windows</SubSystem>\n";
            break;
          case"console"_64:
            fs << "\t\t<SubSystem>Console</SubSystem>\n";
            break;
        }
        switch( toBuild().hash() ){
          case"application"_64:
            [[fallthrough]];
          case"console"_64:/**/{
            const string& path = "$(SolutionDir).output/$(Configuration)/$(TargetName)/$(PlatformTarget)";
            fs << "\t\t<OutputFile>" + path + "/" + toLabel() + ".exe</OutputFile>\n";
            break;
          }
        }
        fs << "\t</Link>\n";
        fs << "\t<ProjectReference>\n";
        fs << "\t\t<LinkLibraryDependencies>"
          + m_sLinkLibDepends
          + "</LinkLibraryDependencies>\n";
        fs << "\t</ProjectReference>\n";
        fs << "</ItemDefinitionGroup>\n";
      }

    //}:                                          |
    //writeProjVersion:{                          |

      void Workspace::MSVC::writeProjVersion( Writer& fs )const{
        fs << "<_ProjectFileVersion>"+m_sProjectVersion+"</_ProjectFileVersion>\n";
      }

    //}:                                          |
    //writeItemGroup:{                            |

      void Workspace::MSVC::writeItemGroup( Writer& fs, const string& group )const{
        switch( group.hash() ){

          //--------------------------------------------------------------------
          // ProjectConfigurations
          //--------------------------------------------------------------------

          case"ProjectConfigurations"_64:
            fs << "<ItemGroup Label=\"ProjectConfigurations\">\n";
            fs << "\t<ProjectConfiguration Include=\"Debug|"+m_sArchitecture+"\">\n";
            fs << "\t\t<Configuration>Debug</Configuration>\n";
            fs << "\t\t<Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "\t</ProjectConfiguration>\n";
            fs << "\t<ProjectConfiguration Include=\"Release|"+m_sArchitecture+"\">\n";
            fs << "\t\t<Configuration>Release</Configuration>\n";
            fs << "\t\t<Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "\t</ProjectConfiguration>\n";
            fs << "</ItemGroup>\n";
            break;

          //--------------------------------------------------------------------
          // Sources.
          //--------------------------------------------------------------------

          case"<source>"_64:/**/{
            fs << "<ItemGroup>\n";
            Files files;

            //------------------------------------------------------------------
            // Ignore files.
            //------------------------------------------------------------------

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
            onIgnore( const_cast<MSVC*>( this )
              -> inSources( Type::kCpp ).getIterator() );
            onIgnore( const_cast<MSVC*>( this )
              -> inSources( Type::kC   ).getIterator() );

            //------------------------------------------------------------------
            // Source files.
            //------------------------------------------------------------------

            files.clear();
            inSources( Type::kCpp ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kC   ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            auto it = files.getIterator();
            while( it ){
              if( *it ){
                string path( *it );
                if( e_fexists( path )){
                  // Need to handle all the pathing cases: including "c:/", "../", "/hello", etc.
                  path.replace( "&", "&amp;" );
                  string osPath;
                  if(( path[ 0 ] == '~' )||
                     ( path[ 0 ] == '/' )||
                     ( path[ 0 ] == '.' )||
                     ( path[ 1 ] == ':' )){
                    osPath = path.os();
                  }else{// Relative path if no absolute markers.
                    osPath=( "..\\"+path ).os();
                  }
                  // If the filename has a ampersand in it the final project will break
                  // because vcxproj's are XML files really.
                  const auto& ext = it->ext().tolower();
                  switch( ext.hash() ){
                    case".cpp"_64:
                    case".cxx"_64:
                    case".cc"_64:
                    case".c"_64:
                      if( !toPrefixHeader().empty() ){
                        const auto isCreating=( it->right( 10 ) == "stdafx.cpp"_64 );
                        const auto isNotUsing=( ext == ".c"_64 );
                        if( isCreating || isNotUsing ){
                          fs << "\t<ClCompile Include=\""+osPath+"\">\n";
                          if( isNotUsing ){
                            fs << "\t\t<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">NotUsing</PrecompiledHeader>\n";
                            fs << "\t\t<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">NotUsing</PrecompiledHeader>\n";
                          }else{
                            fs << "\t\t<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">Create</PrecompiledHeader>\n";
                            fs << "\t\t<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">Create</PrecompiledHeader>\n";
                          }
                          fs << "\t</ClCompile>\n";
                          break;
                        }
                      }
                      fs << "\t<ClCompile Include=\""+osPath+"\"/>\n";
                      break;
                    case".def"_64:
                      fs << "\t<None Include=\""+osPath+"\"/>\n";
                      break;
                    case".inl"_64:
                    case".hxx"_64:
                    case".hh"_64:
                    case".hpp"_64:
                    case".h"_64:
                      fs << "\t<ClInclude Include=\""+osPath+"\"/>\n";
                      break;
                    case".png"_64:
                    case".bmp"_64:
                    case".jpg"_64:
                    case".tga"_64:
                      // This should capture all the image data; we'll need something special for .rc files.
                      fs << "\t<Image Include=\""+osPath+"\">\n";
                      fs << "\t\t<ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">true</ExcludedFromBuild>\n";
                      fs << "\t\t<ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">true</ExcludedFromBuild>\n";
                      fs << "\t</Image>\n";
                      break;
                    default:
                      break;
                  }
                }
              }
              ++it;
            }
            fs << "</ItemGroup>\n";
            break;
          }
        }
      }

    //}:                                          |
    //writeTargetVar:{                            |

      void Workspace::MSVC::writeTargetVar( Writer& fs, const string& config, const string& tag )const{
        switch( tag.hash() ){
          case"Name"_64:
            fs << "<TargetName Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+toLabel()+"</TargetName>\n";
            break;
          case"Ext"_64:
            fs << "<TargetExt Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">."+extFromBuildString()+"</TargetExt>\n";
            break;
        }
      }

    //}:                                          |
    //writeLinkerVar:{                            |

      void Workspace::MSVC::writeLinkerVar( Writer& fs, const string& label, const string& config )const{
        switch( label.hash() ){
          case"Incremental"_64:
            fs << "<LinkIncremental Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+m_sLinkIncremental+"</LinkIncremental>\n";
            break;
        }
      }

    //}:                                          |
    //writeImport*:{                              |

      void Workspace::MSVC::writeImportGroup( Writer& fs, const string& label, const string& path )const{
        fs << "<ImportGroup Label=\""+label+"\">\n";
        switch( label.hash() ){
          case"PropertySheets"_64:
            fs << "<Import Project=\"$(UserRootDir)\\"+path+"\" Condition=\"exists('$(UserRootDir)\\"+path+"')\" Label=\"LocalAppDataPlatform\"/>\n";
            break;
        }
        fs << "</ImportGroup>\n";
      }

      void Workspace::MSVC::writeImportGroup( Writer& fs, const string& label )const{
        fs << "<ImportGroup Label=\""+label+"\">\n";
        switch( label.hash() ){
          case"ExtensionSettings"_64:
            break;
        }
        fs << "</ImportGroup>\n";
      }

      void Workspace::MSVC::writeImport( Writer& fs, const string& type, const string& path )const{
        fs << "<Import "+type+"=\"$(VCTargetsPath)\\"+path+"\"/>\n";
      }

    //}:                                          |
    //writeFilter:{                               |

      void Workspace::MSVC::writeFilter( Writer& fs )const{

        fs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        fs << "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.win64.com/developer/msbuild/2003\">\n";

        //----------------------------------------------------------------------
        // Base directory for filtering from. This should come from somewhere in
        // the project rather than just guessing;  will fail if user doesn't lay
        // out the project src/ and include/.
        //----------------------------------------------------------------------

        static constexpr ccp kIncludeFilterName = "include";

        //----------------------------------------------------------------------
        // Include filter.
        //----------------------------------------------------------------------

        Files includes;
        inSources( Type::kHpp ).foreach( [&]( const auto& fi ){ includes.push( fi ); });
        inSources( Type::kInl ).foreach( [&]( const auto& fi ){ includes.push( fi ); });
        inSources( Type::kH   ).foreach( [&]( const auto& fi ){ includes.push( fi ); });
        if( !includes.empty() ){
          fs << "\t<ItemGroup>\n";
          auto it = includes.getIterator();
          string path;
          while( it ){
            const auto& f = *it;
            if( f.left( 4 ).tolower() == Workspace::out ){
              path = "./" + f.os();
            }else if( f[ 1 ]==':' ){
              path = f.os();
            }else{
              switch( *f ){
                case'/':
                  [[fallthrough]];
                case'.':
                  path = f.os();
                  break;
                default:
                  path = "../" + f.os();
                  break;
              }
            }
            path.replace( "&", "&amp;" );
            fs << "    <ClInclude Include=\"" + path + "\">\n";
            fs << "      <Filter>"+string( strstr( f.path(), kIncludeFilterName )).trimmed( 1 ).os()+"</Filter>\n";
            fs << "    </ClInclude>\n";
            ++it;
          }
          fs << "\t</ItemGroup>\n";
        }

        //----------------------------------------------------------------------
        // Filter GUIDs.
        //----------------------------------------------------------------------

        fs << "  <ItemGroup>\n";
        if( !includes.empty() ){
          fs << "    <Filter Include=\""+string( kIncludeFilterName )+"\">\n";
          fs << "      <UniqueIdentifier>"+string::guid()+"</UniqueIdentifier>\n";
          fs << "    </Filter>\n";
          auto it = includes.getIterator();
          while( it ){
            const auto& f = string( strstr( *it, kIncludeFilterName ));
            ccp p = f;
            while( *p ){
              if( *p == '/' ){
                const string fn( f.c_str(), p );
                fs << "    <Filter Include=\""+fn.os()+"\">\n";
                fs << "      <UniqueIdentifier>"+string::guid()+"</UniqueIdentifier>\n";
                fs << "    </Filter>\n";
              }
              ++p;
            }
            ++it;
          }
        }
        fs << "    <Filter Include=\"images\">\n";
        fs << "      <UniqueIdentifier>"+string::guid()+"</UniqueIdentifier>\n";
        fs << "    </Filter>\n";
        fs << "    <Filter Include=\"defs\">\n";
        fs << "      <UniqueIdentifier>"+string::guid()+"</UniqueIdentifier>\n";
        fs << "    </Filter>\n";
        fs << "    <Filter Include=\"src\">\n";
        fs << "      <UniqueIdentifier>"+string::guid()+"</UniqueIdentifier>\n";
        fs << "    </Filter>\n";
        fs << "  </ItemGroup>\n";

        //----------------------------------------------------------------------
        // Images filter.
        //----------------------------------------------------------------------

        Files images;
        inSources( Type::kPng ).foreach( [&]( const auto& fi ){ images.push( fi ); });
        if( !images.empty() ){
          fs << "  <ItemGroup>\n";
          auto i2 = images.getIterator();
          while( i2 ){
            auto f = *i2;
            f.replace( "&", "&amp;" );
            const auto& path = "../" + f.os();
            fs << "    <Image Include=\""+path+"\">\n";
            fs << "      <Filter>images</Filter>\n";
            fs << "    </Image>\n";
            ++i2;
          }
          fs << "  </ItemGroup>\n";
        }

        //----------------------------------------------------------------------
        // DEF file filter.
        //----------------------------------------------------------------------

        Files defs;
        inSources( Type::kDef ).foreach( [&]( const auto& fi ){ defs.push( fi ); });
        if( !defs.empty() ){
          fs << "  <ItemGroup>\n";
          auto i2 = defs.getIterator();
          while( i2 ){
            auto f = *i2;
            f.replace( "&", "&amp;" );
            const auto& path = "../" + f.os();
            fs << "    <None Include=\""+path+"\">\n";
            fs << "      <Filter>defs</Filter>\n";
            fs << "    </None>\n";
            ++i2;
          }
          fs << "  </ItemGroup>\n";
        }

        //----------------------------------------------------------------------
        // Source filter.
        //----------------------------------------------------------------------

        Files c_cpp;
        inSources( Type::kCpp ).foreach( [&]( const auto& fi ){ c_cpp.push( fi ); });
        inSources( Type::kC   ).foreach( [&]( const auto& fi ){ c_cpp.push( fi ); });
        if( !c_cpp.empty() ){
          fs << "  <ItemGroup>\n";
          auto i2 = c_cpp.getIterator();
          while( i2 ){
            auto f = *i2;
            e_msgf( "   ClCompile: Filter \"%s\"", ccp( f ));
            f.replace( "&", "&amp;" );
            const auto& path = "../" + f.os();
            fs << "    <ClCompile Include=\""+path+"\">\n";
            fs << "      <Filter>src</Filter>\n";
            fs << "    </ClCompile>\n";
            ++i2;
          }
          fs << "  </ItemGroup>\n";
        }
        fs << "</Project>\n";
      }

    //}:                                          |
    //sortingHat:{                                |

      bool Workspace::MSVC::sortingHat( const string& in ){

        //----------------------------------------------------------------------
        // Solutions need Windows GUIDs. If we're running on Windows then you
        // are guaranteed to have a GUID that's universally unique. If running
        // on the Mac or Linux, you're only guaranteed to have file uniqueness.
        //----------------------------------------------------------------------

        string in_path( in );
        in_path.replace( "\\", "/" );
        File path( in_path );
             path.setBuildID( string::guid() );
             path.setRefMSVC( string::guid() );
        const auto& ext = path.ext().tolower();
        switch( ext.hash() ){

          //--------------------------------------------------------------------
          // Platform specific file types.
          //--------------------------------------------------------------------

          case".lib"_64:
            inSources( Type::kLib ).push( path );
            break;

          //--------------------------------------------------------------------
          // Source and header file types.
          //--------------------------------------------------------------------

          case".png"_64:
            inSources( Type::kPng ).push( path );
            break;
          case".inl"_64:
            inSources( Type::kInl ).push( path );
            break;
          case".hpp"_64:
          case".hxx"_64:
          case".hh"_64:
            inSources( Type::kHpp ).push( path );
            break;
          case".def"_64:
            inSources( Type::kDef ).push( path );
            break;
          case".cpp"_64:
          case".cxx"_64:
          case".cc"_64:
            inSources( Type::kCpp ).push( path );
            break;
          case".h"_64:
            inSources( Type::kH ).push( path );
            break;
          case".c"_64:
            inSources( Type::kC ).push( path );
            break;
          default:
            return false;
        }
        return true;
      }

    //}:                                          |
    //serialize:{                                 |

      void Workspace::MSVC::serialize( Writer& fs )const{

        //----------------------------------------------------------------------
        // Populate build files across unity space.
        //----------------------------------------------------------------------

        const auto isUnity = isUnityBuild();
        if( !isUnity||!Workspace::bmp->bUnity ){
          writeProject<MSVC>( fs, Type::kCpp );
          writeProject<MSVC>( fs, Type::kC );
        }else{
          const u32 cores = std::thread::hardware_concurrency();
          u32 i=0;
          const_cast<MSVC*>( this )->toUnity().resize( cores );
          const_cast<MSVC*>( this )->unifyProject<MSVC>( Type::kCpp, i );
          const_cast<MSVC*>( this )->unifyProject<MSVC>( Type::kC,   i );
          writeProject<MSVC>( fs, Type::kCpp );
          writeProject<MSVC>( fs, Type::kC );
        }

        //----------------------------------------------------------------------
        // Save MSVC project to XML format file.
        //----------------------------------------------------------------------

        fs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        fs << "<Project DefaultTargets=\"Build\" ToolsVersion=\"16.0\" xmlns=\"http://schemas.win64.com/developer/msbuild/2003\">\n";
        writePropGroup( fs, "<arch>" );
        writeItemGroup( fs, "ProjectConfigurations" );
        writeItemGroup( fs, "<source>" );
        writePropGroup( fs, "Globals" );
        writeImport(    fs, "Project", "Microsoft.Cpp.Default.props" );
        for( u32 n=e_sizeof( a_aConfigs ), i=0; i<n; ++i ){
          writePropGroup( fs, "Condition", a_aConfigs[ i ]);
        }
        writeImport(      fs, "Project", "Microsoft.Cpp.props" );
        writeImportGroup( fs, "ExtensionSettings" );
        writeImportGroup( fs, "PropertySheets", "Microsoft.Cpp.$(Platform).user.props" );
        writePropGroup(   fs, "UserMacros" );
        fs << "<PropertyGroup>\n";
          writeProjVersion(  fs );
          for( u32 n=e_sizeof( a_aConfigs ), i=0; i<n; ++i ){
            writeSetDirectory( fs, "OutDir",      a_aConfigs[ i ], m_sOutDir );
            writeSetDirectory( fs, "IntDir",      a_aConfigs[ i ], m_sIntDir );
            writeTargetVar(    fs, "Name",        a_aConfigs[ i ]);
            writeTargetVar(    fs, "Ext",         a_aConfigs[ i ]);
            writeLinkerVar(    fs, "Incremental", a_aConfigs[ i ]);
            writeManifestData( fs, "Debug" );
          }
        fs << "</PropertyGroup>\n";
        writePropGroup( fs, "TargetExt" );
        fs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>\n";
        fs << "\t<ImportGroup Label=\"ExtensionTargets\">\n";
        fs << "</ImportGroup>\n";
        fs << "</Project>\n";

        //----------------------------------------------------------------------
        // Create filters file parallel to this project.
        //----------------------------------------------------------------------

        Writer filters( fs.toFilename() + ".filters", kTEXT );
        writeFilter( filters );
        filters.save();
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
