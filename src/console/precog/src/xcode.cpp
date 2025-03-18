//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

//http://www.monobjc.net/xcode-project-file-format.html
//libtool -static -o new.a old1.a old2.a

#include"generators.h"
#include"luacore.h"
#include"std.h"
#include"ws.h"
#include<set>
#include<regex>

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================+=============================
//Extends:{                                       |

#ifdef __APPLE__
  #pragma mark - Extensions -
#endif

  e_extends( Workspace::Xcode );

//}:                                              |
//Defines:{                                       |

  #define var auto

//}:                                              |
//Statics:{                                       |

  namespace{
    hashmap<u64,s8>keyCache;
    hashmap<u64,s8>libCache;
    hashmap<u64,s8>grpCache;
  }

//}:                                              |
//Private:{                                       |
  //normalizeInstallScript:{                      |

#ifdef __APPLE__
  #pragma mark (private)
#endif

    namespace{
      string a_normalizeInstallScript( const string& inScript ){
        string r( inScript );
        r.replace( "\"", "\\\"" );
        r.replace( "\n", "\\n" );
        return r;
      }
    }

  //}:                                            |
//}:                                              |
//Methods:{                                       |
  //[resources]:{                                 |
    //setup:{                                     |

#ifdef __APPLE__
  #pragma mark - Xcode -
#endif

      void Workspace::Xcode::setup()const{
        for( auto n=e_dimof( super::toSources() ), i=0u; i<n; ++i ){
          const auto& list = super::inSources( i );
          auto it = list.getIterator();
          while( it ){
            m_maxWidth = e_max<s32>( m_maxWidth
              , s32( it->len() ));
            ++it;
          }
        }
      }

    //}:                                          |
    //purge:{                                     |

      void Workspace::Xcode::purge()const{
        m_mLibCache.clear();
        m_maxWidth = 0;
      }

    //}:                                          |
  //}:                                            |
  //[project]:{                                   |
    //addToPBX*PhaseSection:{                     |

      void Workspace::Xcode::addToPBXSourcesBuildPhaseSection( Writer& fs
            , const std::function<void( const string& source )>& lambda )const{
        const auto& targets = getTargets();
        auto it = targets.getIterator();
        while( it ){
          auto target( *it );
          if( target == "macos"_64 ){
            lambda( m_aSourcesBuildPhase[ Target::macOS ]);
          }else{
            lambda( m_aSourcesBuildPhase[ Target::iOS ]);
          }
          ++it;
        }
      }

      void Workspace::Xcode::addToPBXNativeTargetSection( Writer& fs
        , const std::function<void( const string& target
                                  , const string& label
                                  , const string& build
                                  , const string& frame
                                  , const string& phaseFramework
                                  , const string& phaseResources
                                  , const string& phaseHeaders
                                  , const string& phaseSources
                                  , const string& phaseScript
                                  , const string& embedFrameworks
                                  , const string& embedPlugins
                                  , const string& productFileRef
                                  , const string& copyRefs )>& lambda )const{

        //----------------------------------------------------------------------
        // Select platform from macOS or iOS.
        //----------------------------------------------------------------------

        const auto& targets = getTargets();
        auto it = targets.getIterator();
        while( it ){
          auto target( *it );
          string buildNativeTarget;
          string frameworkNativeTarget;
          string phaseNativeFramework;
          string phaseResources;
          string phaseNativeHeaders;
          string embedNativeFrameworks;
          string embedNativePlugins;
          string phaseNativeSources;
          string phaseNativeScript;
          string productFileRef;
          string copyRefs;
          auto label( toLabel() );
          if( target == "macos"_64 ){
            frameworkNativeTarget = m_aFrameNativeTarget  [ Target::macOS ];
            buildNativeTarget     = m_aBuildNativeTarget  [ Target::macOS ];
            phaseNativeFramework  = m_aFrameworkBuildPhase[ Target::macOS ];
            phaseResources        = m_aResourcesBuildPhase[ Target::macOS ];
            phaseNativeHeaders    = m_aHeadersBuildPhase  [ Target::macOS ];
            phaseNativeSources    = m_aSourcesBuildPhase  [ Target::macOS ];
            embedNativeFrameworks = m_aFrameworksEmbed    [ Target::macOS ];
            embedNativePlugins    = m_aPluginsEmbed       [ Target::macOS ];
            productFileRef        = m_aProductFileRef     [ Target::macOS ];
            copyRefs              = m_aCopyRefs           [ Target::macOS ];
          }else if( target == "ios"_64 ){
            frameworkNativeTarget = m_aFrameNativeTarget  [ Target::iOS ];
            buildNativeTarget     = m_aBuildNativeTarget  [ Target::iOS ];
            phaseNativeFramework  = m_aFrameworkBuildPhase[ Target::iOS ];
            phaseResources        = m_aResourcesBuildPhase[ Target::iOS ];
            phaseNativeHeaders    = m_aHeadersBuildPhase  [ Target::iOS ];
            phaseNativeSources    = m_aSourcesBuildPhase  [ Target::iOS ];
            embedNativeFrameworks = m_aFrameworksEmbed    [ Target::iOS ];
            embedNativePlugins    = m_aPluginsEmbed       [ Target::iOS ];
            productFileRef        = m_aProductFileRef     [ Target::iOS ];
            copyRefs              = m_aCopyRefs           [ Target::iOS ];
          } ++it;
          lambda(
              target
            , label
            , buildNativeTarget
            , frameworkNativeTarget
            , phaseNativeFramework
            , phaseResources
            , phaseNativeHeaders
            , phaseNativeSources
            , phaseNativeScript
            , embedNativeFrameworks
            , embedNativePlugins
            , productFileRef
            , copyRefs
          );
        }
      }

      void Workspace::Xcode::addToPBXGroupSection( Writer& fs
          , const std::function<void(
            const string& product
          , const string& target
          , const string& label )>& lambda )const{
        const auto& targets = getTargets();
        auto it = targets.getIterator();
        while( it ){
          auto target( *it );
          string product;
          string label( toLabel() );
          if( target == "macos"_64 ){
            product = m_aProductFileRef[ Target::macOS ];
          }else{
            product = m_aProductFileRef[ Target::iOS ];
            label = "ios";
          }
          lambda( product
            , target
            , label );
          ++it;
        }
      }

      void Workspace::Xcode::addToPBXShellScriptBuildPhaseSection( Writer& fs
          , const std::function<void(
            const string& target
          , const string& shellScript )>& lambda )const{
        const auto& targets = getTargets();
        auto it = targets.getIterator();
        while( it ){
          auto target( *it );
          //TODO: Do something!
          ++it;
        }
      }

      void Workspace::Xcode::addToPBXFileReferenceSection( Writer& fs,
          const std::function<void(
              const string& target
            , const string& label
            , const string& prod )>& lambda )const{
        const auto& targets = getTargets();
        auto it = targets.getIterator();
        while( it ){
          auto target( *it );
          string label;
          string prod;
          if( target == "macos"_64 ){
            prod = m_aProductFileRef[ Target::macOS ];
          }else{
            prod = m_aProductFileRef[ Target::iOS ];
            label = "ios";
          }
          lambda( target
            , label
            , prod );
          ++it;
        }
      }

    //}:                                          |
    //saveEntitlements:{                          |

      void Workspace::Xcode::saveEntitlements( const string& path )const{
        if( !hasEntitlements() )
          return;
        Writer wr( path
          + "/"// tmp directory
          + toLabel()
          + ".entitlements"
          , kTEXT );
        wr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        wr << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
        wr << "<plist version=\"1.0\">\n";
        wr << "<dict>\n";
        if( toFlags()->bEnableJIT ){
          wr << "  	<key>com.apple.security.cs.allow-jit</key>\n";
          wr << "  <true/>\n";
        }
        if( toFlags()->bDisablePageProtection ){
          wr << "  <key>com.apple.security.cs.disable-executable-page-protection</key>\n";
          wr << "  <true/>\n";
        }
        if( toFlags()->bDisableLibValidation ){
          wr << "  <key>com.apple.security.cs.disable-library-validation</key>\n";
          wr << "  <true/>\n";
        }
        wr << "</dict>\n";
        wr << "</plist>\n";
        wr.save( nullptr );
      }

    //}:                                          |
    //hasEntitlements:{                           |

      bool Workspace::Xcode::hasEntitlements()const{
        const auto hasAnyEntitlement =
          toFlags()->bDisablePageProtection ||
          toFlags()->bDisableLibValidation  ||
          toFlags()->bEnableJIT;
        return hasAnyEntitlement;
      }

    //}:                                          |
    //extFromSource<>:{                           |

      ccp Workspace::Xcode::extFromEnum( const Type e )const{
        switch( e ){
          case decltype( e )::kCpp:
            return ".cpp";
          case decltype( e )::kMm:
            return ".mm";
          case decltype( e )::kC:
            return ".c";
          case decltype( e )::kM:
            return ".m";
          default:
            return "";
        }
      }

    //}:                                          |
    //sortingHat:{                                |

      bool Workspace::Xcode::sortingHat( const string& in_path ){
        auto f = File( in_path.filename() );
        f.setWhere( in_path.path() );
        const auto& ext = f
          . ext()
          . tolower();
        static auto isLogging = e_getCvar( bool, "SPEW" );
        if( isLogging ){
          static constexpr auto kGap = 11ull;
          const auto spaces = string::spaces(
            ( u32(      in_path.ext().ltrimmed( 1 ).len() ) < kGap )
            ? u32( kGap-in_path.ext().ltrimmed( 1 ).len() )
            : u32(      in_path.ext().ltrimmed( 1 ).len()+1ull )-u32( in_path.ext().ltrimmed( 1 ).len() ));
          e_msgf( "  Found %s%s \"%s\" @ %s"
            , ccp( in_path.ext().ltrimmed( 1 ).toupper() )
            , ccp( spaces )
            , ccp( f.toWhere().path() )
            , ccp( toLabel() )
          );
        }
        switch( ext.hash() ){

          //--------------------------------------------------------------------
          // Platform specific file types.
          //--------------------------------------------------------------------

          case".framework"_64:
            inSources( Type::kFramework ).push( f );
            break;
          case".bundle"_64:
            inSources( Type::kBundle ).push( f );
            break;
          case".storyboard"_64:
            inSources( Type::kStoryboard ).push( f );
            break;
          case".xcassets"_64:
            inSources( Type::kXcasset ).push( f );
            break;
          case".fablet"_64:
            inSources( Type::kPrefab ).push( f );
            break;
          case".lproj"_64:
            inSources( Type::kLproj ).push( f );
            break;
          case".plist"_64:/* There can be only one */{
            File fi( in_path.filename() );
            (( string& ) fi )=( in_path );
            fi.setWhere( fi.path() );
            inSources( Type::kPlist ).push( fi );
            e_msgf( "  Found PLIST %s @ %s for project %s."
              , ccp( fi.filename() )
              , ccp( fi.toWhere().path() )
              , ccp( toLabel() ));
            break;
          }
          case".rtf"_64:
            inSources( Type::kRtf ).push( f );
            break;
          case".dylib"_64:
            inSources( Type::kSharedLib ).push( f );
            break;
          case".a"_64:
            inSources( Type::kStaticLib ).push( f );
            break;
          case".mm"_64:
            inSources( Type::kMm ).push( f );
            break;
          case".m"_64:
            inSources( Type::kM ).push( f );
            break;

          //--------------------------------------------------------------------
          // Source and header file types.
          //--------------------------------------------------------------------

          case".png"_64:
            inSources( Type::kPng ).push( f );
            break;
          case".inl"_64:
            inSources( Type::kInl ).push( f );
            break;
          case".hpp"_64:
            [[fallthrough]];
          case".hxx"_64:
            [[fallthrough]];
          case".hh"_64:
            inSources( Type::kHpp ).push( f );
            break;
          case".cpp"_64:
            [[fallthrough]];
          case".cxx"_64:
            [[fallthrough]];
          case".cc"_64:
            inSources( Type::kCpp ).push( f );
            break;
          case".h"_64:
            inSources( Type::kH ).push( f );
            break;
          case".c"_64:
            inSources( Type::kC ).push( f );
            break;
          default:
            return false;
        }
        return true;
      }

    //}:                                          |
    //serialize:{                                 |

      void Workspace::Xcode::serialize( Writer& fs )const{

        //----------------------------------------------------------------------
        // Populate build files across unity space.
        //----------------------------------------------------------------------

        const auto isUnity = isUnityBuild();
        if( !isUnity && !Workspace::bmp->bUnity ){
          writeProject<Xcode>( fs, Type::kCpp );
          writeProject<Xcode>( fs, Type::kMm );
          writeProject<Xcode>( fs, Type::kC );
          writeProject<Xcode>( fs, Type::kM );
        }else{
          u32 i = 0;
          const u32 cores = std::thread::hardware_concurrency();
          const_cast<Xcode*>( this )->toUnity().resize( cores );
          const_cast<Xcode*>( this )->unifyProject<Xcode>( Type::kCpp, i );
          const_cast<Xcode*>( this )->unifyProject<Xcode>( Type::kMm,  i );
          const_cast<Xcode*>( this )->unifyProject<Xcode>( Type::kC,   i );
          const_cast<Xcode*>( this )->unifyProject<Xcode>( Type::kM,   i );
          writeProject<Xcode>( fs, Type::kCpp );
          writeProject<Xcode>( fs, Type::kMm  );
          writeProject<Xcode>( fs, Type::kC   );
          writeProject<Xcode>( fs, Type::kM   );
        }

        //----------------------------------------------------------------------
        // Save Xcode project to pbx format bundle.
        //----------------------------------------------------------------------

        fs << "// !$*UTF8*$!\n";
        fs << "{\n";
        fs << "  archiveVersion = 1;\n";
        fs << "  classes = {\n";
        fs << "  };\n";
        if( Workspace::bmp->bXcode11 ){
          fs << "  objectVersion = 50;\n";// Version 9.3 compatible.
        }else if( Workspace::bmp->bXcode12 ){
          fs << "  objectVersion = 54;\n";
        }else if( Workspace::bmp->bXcode15 ){
          fs << "  objectVersion = 60;\n";
        }else if( Workspace::bmp->bXcode16 ){
          fs << "  objectVersion = 77;\n";
        }
        fs << "  objects = {\n";
        writePBXBuildFileSection(             fs );
        writePBXCopyFilesBuildPhaseSection(   fs );
        writePBXFileReferenceSection(         fs );
        writePBXShellScriptBuildPhaseSection( fs );
        writePBXFrameworksBuildPhaseSection(  fs );
        writePBXGroupSection(                 fs );
        writePBXVariantGroupSection(          fs );
        writePBXNativeTargetSection(          fs );
        writePBXProjectSection(               fs );
        writePBXResourcesBuildPhaseSection(   fs );
        writePBXHeadersBuildPhaseSection(     fs );
        writePBXSourcesBuildPhaseSection(     fs );
        writeXCBuildConfigurationSection(     fs );
        writeXCConfigurationListSection(      fs );
        fs << "  };\n";
        fs << "  rootObject = " + m_sProjectObject + " /* Project object */;\n";
        fs << "}\n\n// vim:ft=cpp";
      }

    //}:                                          |
    //writeFileReferenceGroup*:{                  |

      void Workspace::Xcode::writeFileReferenceGroup( Writer& fs
          , const string& type
          , const string& name
          , const string& explicitT // expliciteFileType, etc.
          , const string& tree
          , const File&   file )const{
        // Note _path ends with /
        auto sourceTree( tree );
        // Not added more than 1.
        static std::set<u64>___;
        string key;
        key.catf( "%s:%s"
          , ccp( file )
          , ccp( file.toWhere() ));
        if(___.find(    key.hash() )==___.end() )
           ___.emplace( key.hash() );
        else return;
        // Produce a forced reference: always succeeds!
        const auto forcedRef = e_forceref( file );
        fs << "    "
           << forcedRef
           << " /* "
           << file.filename().c_str()
           << " */"
           << " = {isa = PBXFileReference; "
           << explicitT
           << " = "
           << type;
        fs << "; ";
        File f( file );
        fs << "name = " << file.filename() << "; ";
        const auto& osextra = f.os().ext().tolower();
        if( tree.hash() != "BUILT_PRODUCTS_DIR"_64 ){
          if( osextra.hash() != ".entitlements"_64 ){
            fs << "path = "
               << ( f.toWhere().empty()
                ? ( "../" + f )
                : ( "../" + f.toWhere() + f ))
               << "; ";
          }else{
            fs << "path = " << f.os() << "; ";
          }
        }else{
           if( f.toWhere().empty() ){
            switch( osextra.hash() ){
              case".framework"_64:
                [[fallthrough]];
              case".bundle"_64:
                [[fallthrough]];
              case".dylib"_64:
                [[fallthrough]];
              case".a"_64:/**/{
                string out;
                const auto paths = toFindLibsPaths().splitAtCommas();
                auto it = paths.getIterator();
                while( it ){
                  if( e_fexists( *it + "/" + f )){
                    sourceTree = "SOURCE_ROOT";
                    f.setWhere( "../"
                      + *it
                      + "/"
                      + f );
                    break;
                  }
                  ++it;
                }
                break;
              }
            }
          }
          fs << "path = "
             << ( !f.toWhere().empty() ? f.toWhere().os() : f );
          fs << "; ";
        }
        fs << "sourceTree = "
           << sourceTree;
        fs << "; };\n";
      }

      void Workspace::Xcode::writeFileReferenceGroups( Writer& fs
          , Files& paths
          , const string& type
          , const string& word // LastKnownFileType, etc.
          , const string& tree )const{
        if( paths.empty() )
          return;
        ignore( paths, toIgnoreParts() );
        paths.sort(
          []( const auto& a, const auto& b ){
            return(
                a.filename().tolower()
              < b.filename().tolower()
            );
          }
        );
        const auto& targets = getTargets();
        paths.foreach(
          [&]( File& f ){
            auto it = targets.getIterator();
            while( it ){
              const auto& name = f.os();
              const auto& target = *it;
              switch( target.hash() ){
                case "macos"_64:
                  writeFileReferenceGroup( fs
                    , type
                    , name
                    , word
                    , tree
                    , f );
                  break;
                case "ios"_64:/**/{
                  const auto& ext=( !f.toWhere().empty(/* no os call */)
                    ? f.toWhere().os().ext().tolower()
                    : f.os().ext().tolower() );
                  switch( ext.hash() ){
                    case".framework"_64: [[fallthrough]];
                    case".bundle"_64:    [[fallthrough]];
                    case".dylib"_64:
                      // No support on iOS for frameworks, bundles, dylibs,
                      // or text-based-dylibs.
                      break;
                    case".a"_64:
                      [[fallthrough]];
                    default:
                      writeFileReferenceGroup( fs
                        , type
                        , name
                        , word
                        , tree
                        , f );
                      break;
                  }
                  break;
                }
              }
              ++it;
            }
          }
        );
      }

    //}:                                          |
    //writePBX*:{                                 |
      //addToPBXFrameworksBuildPhaseSection:{     |

        void Workspace::Xcode::addToPBXFrameworksBuildPhaseSection( Writer& fs
            , const std::function<void(
                const string& target
              , const string& buildPhase )>& lambda )const{
          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            auto target( *it );
            string buildPhase;
            if( target == "macos"_64 ){
              buildPhase = m_aFrameworkBuildPhase[
                Target::macOS
              ];
            }else{
              buildPhase = m_aFrameworkBuildPhase[
                Target::iOS
              ];
            }
            lambda( target
              , buildPhase );
            ++it;
          }
        }

      //}:                                        |
      //writePBXFrameworksBuildPhaseSection:{     |

        void Workspace::Xcode::writePBXFrameworksBuildPhaseSection( Writer& fs )const{
          fs << "\n    /* Begin PBXFrameworksBuildPhase section */\n";
          addToPBXFrameworksBuildPhaseSection( fs,
            [&]( const auto& target
               , const auto& frameworkBuildPhase ){
              fs << "    " + frameworkBuildPhase + " /* frameworks */ = {\n";
              fs << "      isa = PBXFrameworksBuildPhase;\n"
                 << "      buildActionMask = 2147483647;\n"
                 << "      files = (\n";
              Files collection;
              if( !inSources( Type::kPlatform ).empty() )
                   inSources( Type::kPlatform ).foreach(
                [&]( const auto& f ){
                  auto fb = f.base();
                  auto ok = false;
                  Class::foreachs<Xcode>(
                    [&]( const auto& x ){
                      auto fl( f );
                      fl.erase( "lib" );
                      fl.erase( ".framework" );
                      fl.erase( ".bundle" );
                      fl.erase( ".dylib" );
                      fl.erase( ".a" );
                      if( !x.isLabel( fl ))
                        return true;
                      e_msgf(// Display what we're going to link with.
                          "  Linking %s"
                        , ccp( fl ));
                      ok = true;
                      return!ok;
                    }
                  );
                  if( !ok )
                    return;
                  const auto fbr = f.right( sizeof".bundle"-1 );
                  const auto fdr = f.right( sizeof".dylib"-1 );
                  if((( fbr.hash() == ".bundle"_64 )||
                      ( fdr.hash() ==  ".dylib"_64 )))
                    return;
                  collection.push( f );
                }
              );
              if( !toLibFiles().empty() )
                toLibFiles().foreach( [&]( const auto& fi ){ collection.push( fi ); });
              collection.foreach(
                [&]( const auto& _f ){
                  const auto& ext = _f.ext().tolower();
                  File f( _f );
                  if( ext.empty() )
                   f << ".framework";
                  fs << "        "
                     << + f.toBuildID()
                     << " /* "
                     << f.filename();
                  fs << " */,\n";
                }
              );
              fs << string( "      );\n" )
                + "      runOnlyForDeploymentPostprocessing = 0;\n"
                + "    };\n"
              ;
            }
          );
          fs << "    /* End PBXFrameworksBuildPhase section */\n";
        }

      //}:                                        |
      //writePBXResourcesBuildPhaseSection:{      |

        void Workspace::Xcode::writePBXResourcesBuildPhaseSection( Writer& fs )const{
          fs << "\n    /* Begin PBXResourcesBuildPhase section */\n";
          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            Target t;
            if( it->hash() == "ios"_64 ){
              t = Target::iOS;
            }else{
              t = Target::macOS;
            }
            fs << "    " + m_aResourcesBuildPhase[ t ] + " /* Resources */ = {\n"
                + "      isa = PBXResourcesBuildPhase;\n"
                + "      buildActionMask = 2147483647;\n"
                + "      files = (\n";
            Files files;
            // TODO: This assumes that the iOS and macOS builds have the same resources.
            // TODO: This might not be correct; so, keep an eye on it.
            inSources( Type::kStoryboard ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kXcasset    ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kPrefab     ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kLproj      ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            files.foreach(
              [&]( const File& f ){
                if( f.empty() ){
                  return;
                }
                fs << "        " + f.toBuildID() + " /* " + f.filename() + " in Resources */,\n";
              }
            );
            fs << "      );\n";
            fs << "      runOnlyForDeploymentPostprocessing = 0;\n";
            fs << "    };\n";
            ++it;
          }
          fs << "    /* End PBXResourcesBuildPhase section */\n";
        }

      //}:                                        |
      //writePBXCopyFilesBuildPhaseSection:{      |

      void Workspace::Xcode::writePBXCopyFilesBuildPhaseSection( Writer& fs )const{
        fs << "\n    /* Begin PBXCopyFilesBuildPhase section */\n";

          //--------------------------------------------------------------------
          // Local lambda function to embed files.
          //--------------------------------------------------------------------

          const auto& writePBXCopyFilesBuildPhase=[&](
                const auto& subfolderSpec
              , const auto& files
              , const auto& id
              , const auto& comment
              , const std::function<void( const File& )>& lbuild
              , const std::function<void()>& lambda ){
            if( files.empty() )
              return;
            fs << "    " + id + " /* " + comment + " */ = {\n";
            fs << "      isa = PBXCopyFilesBuildPhase;\n";
            fs << "      buildActionMask = 2147483647;\n";
            fs << "      dstPath = \"\";\n";
            fs << "      dstSubfolderSpec = " + subfolderSpec + ";\n";
            fs << "      files = (\n";
            files.foreach(
              [&]( const File& file ){
                lbuild( file );
              }
            );
            fs << "      );\n";
            if( lambda )
                lambda();
            fs << "      runOnlyForDeploymentPostProcessing = 0;\n";
            fs << "    };\n";
          };

          //--------------------------------------------------------------------
          // Declare a lambda to do the heavy lifting of the copy build phase.
          //--------------------------------------------------------------------

          const auto& onCopy=[&](
                const string& target
              , const auto& frameworks
              , const auto& copyRefs
              , const auto& plugins ){

            //------------------------------------------------------------------
            // Copy dylib references into frameworks folder.
            //------------------------------------------------------------------

            Files files;
            inSources( Type::kPlatform ).foreach(
              [&]( const auto& fi ){ files.push( fi ); });
            toEmbedFiles().foreach(
              [&]( const auto& fi ){ files.push( fi ); });
            writePBXCopyFilesBuildPhase(
                string( "6"/* CopyFiles */)
              , toEmbedFiles()
              , copyRefs
              , string( "CopyFiles" )
              , [&]( const File& f ){
                  if( f.ext().tolower().hash() != ".dylib"_64 )
                    return;
                  fs << "        ";
                  fs << f.toBuildID();
                  fs << " /* [BuildID] "
                    + f.filename()
                    + " in CopyFiles */,\n"
                  ;
                }
              , nullptr );

            //------------------------------------------------------------------
            // Copy references into resources folder.
            //------------------------------------------------------------------

            writePBXCopyFilesBuildPhase(
                string( "7"/* CopyFiles */)
              , toPublicRefs()
              , copyRefs
              , string( "CopyFiles" )
              , [&]( const File& f ){
                  fs << "        ";
                  fs << f.toBuildID();
                  fs << " /* [BuildID] "
                    + f.filename()
                    + " in CopyFiles */,\n"
                  ;
                }
              , nullptr
            );

            //------------------------------------------------------------------
            // Copy embedded frameworks and dylibs etc into Frameworks folder.
            // DB0FA58B2758022D00CA287A /* Embed Frameworks */ = {
            //   isa = PBXCopyFilesBuildPhase;
            //   buildActionMask = 2147483647;
            //   dstPath = "";
            //   dstSubfolderSpec = 10;
            //   files = (
            //      DB0FA58C2758023000CA287A /* libfbxsdk.dylib in Embed Frameworks */,
            //      DB0FA58A2758022D00CA287A /* eon.framework in Embed Frameworks */,
            //   );
            //   name = "Embed Frameworks";
            //   runOnlyForDeploymentPostprocessing = 0;
            // };
            //------------------------------------------------------------------

            writePBXCopyFilesBuildPhase(
                string( "10"/* Embed Frameworks */)
              , toEmbedFiles()
              , frameworks
              , string( "Embed Frameworks" )
              , [&]( const File& f ){
                  switch( f.ext().tolower().hash() ){
                    case".framework"_64:
                      [[fallthrough]];
                    case".bundle"_64:
                      [[fallthrough]];
                    case".dylib"_64:/**/{
                      // Shared libraries are forbidden on iOS so skip them.
                      if( target.tolower().hash() != "macos"_64 )
                        break;
                      fs << "        ";
                      fs << f.toBuildID2()
                         << " /* [BuildID2] "
                         << f.filename()
                         << " in Embed Frameworks */,\n";
                      break;
                    }
                  }
                }
                , [&](){ fs << "      name = \"Embed Frameworks\";\n";
              }
            );

            //------------------------------------------------------------------
            // Copy embedded frameworks and dylibs etc, into Frameworks folder.
            //------------------------------------------------------------------

            writePBXCopyFilesBuildPhase(
                string( "13"/* CopyFiles (PlugIns) */)
              , toEmbedFiles()
              , plugins
              , string( "CopyFiles" )
              , [&]( const File& f ){
                  if( f.ext().tolower() == ".bundle"_64 ){
                    // Bundles don't exist on iOS so skip them.
                    if( target.tolower().hash() != "macos"_64 )
                      return;
                    fs << "        ";
                    fs << f.toBuildID2();
                    fs << " /* [BuildID2] " + f.filename();
                    fs << " in CopyFiles */,\n";
                  }
                }
              , nullptr
            );
          };

          //--------------------------------------------------------------------
          // Copy references into resources folder.
          //--------------------------------------------------------------------

          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            auto target( *it );
            string embedFrameworks;
            string embedPlugins;
            string copyRefs;

            //------------------------------------------------------------------
            // Gather all strings.
            //------------------------------------------------------------------

            if( target == "macos"_64 ){
              embedFrameworks = m_aFrameworksEmbed[ Target::macOS ];
              embedPlugins    = m_aPluginsEmbed[    Target::macOS ];
              copyRefs        = m_aCopyRefs[        Target::macOS ];
            }else{
              embedFrameworks = m_aFrameworksEmbed[ Target::iOS ];
              embedPlugins    = m_aPluginsEmbed[    Target::iOS ];
              copyRefs        = m_aCopyRefs[        Target::iOS ];
            }

            //------------------------------------------------------------------
            // Run action to write to PBX section.
            //------------------------------------------------------------------

            onCopy( target
              , embedFrameworks
              , embedPlugins
              , copyRefs );
            ++it;
          }

        fs << "    /* End PBXCopyFilesBuildPhase section */\n";
      }

      //}:                                        |
      //writePBXSourcesBuildPhaseSection:{        |

        void Workspace::Xcode::writePBXSourcesBuildPhaseSection( Writer& fs )const{
          fs << "\n    /* Begin PBXSourcesBuildPhase section */\n";
          addToPBXSourcesBuildPhaseSection( fs,
            [&]( const string& source ){
              fs << "    " + source + " /* Sources */ = {\n"
                 << "      isa = PBXSourcesBuildPhase;\n"
                 << "      buildActionMask = 2147483647;\n"
                 << "      files = (\n";
              Files files;
              inSources( Type::kCpp ).foreach(
                [&]( const auto& fi ){ files.push( fi ); });
              inSources( Type::kMm ).foreach(
                [&]( const auto& fi ){ files.push( fi ); });
              inSources( Type::kM ).foreach(
                [&]( const auto& fi ){ files.push( fi ); });
              inSources( Type::kC ).foreach(
                [&]( const auto& fi ){ files.push( fi ); });
              files.foreach(
                [&]( const File& f ){
                  if( f.empty() )
                    return;
                  fs << "        " + f.toBuildID() + " /* " + f.filename() + " in Sources */,\n";
                }
              );
              fs << "      );\n";
              fs << "      runOnlyForDeploymentPostprocessing = 0;\n";
              fs << "    };\n";
            }
          );
          fs << "    /* End PBXSourcesBuildPhase section */\n";
        }

      //}:                                        |
      //writePBXHeadersBuildPhaseSection:{        |

        void Workspace::Xcode::writePBXHeadersBuildPhaseSection( Writer& fs )const{
          fs << "\n    /* Begin PBXHeadersBuildPhase section */\n";
            const auto& onHeader=[&]( const string& target ){
              if( target == "ios"_64 ){ fs
                << "    "
                << m_aHeadersBuildPhase[ Target::iOS ]
                << " /* Headers */ = {\n";
              }else{ fs
                << "    "
                << m_aHeadersBuildPhase[ Target::macOS ]
                << " /* Headers */ = {\n";
              }
              fs << "      isa = PBXHeadersBuildPhase;\n"
                 << "      buildActionMask = 2147483647;\n"
                 << "      files = (\n";
              Files files;
              toPublicHeaders().foreach( [&]( const auto& fi ){ files.push( fi ); });
              files.foreach(
                [&]( const File& f ){
                  fs << "        " + f.toBuildID() + " /* " + f.filename() + " in Headers */,\n";
                }
              );
              fs << "      );\n";
              fs << "      runOnlyForDeploymentPostprocessing = 0;\n";
              fs << "    };\n";
            };
            const auto& targets = getTargets();
            auto it = targets.getIterator();
            while( it ){
              onHeader( *it );
              ++it;
            }
          fs << "    /* End PBXHeadersBuildPhase section */\n";
        }

      //}:                                        |
      //writePBXShellBuildPhaseSection:{          |

        void Workspace::Xcode::writePBXShellScriptBuildPhaseSection( Writer& fs )const{
          if( toInstallScript().empty() )
            return;
          fs << "\n    /* Begin PBXShellScriptBuildPhase section */\n";
          addToPBXShellScriptBuildPhaseSection( fs,
            [&]( const auto& target
               , const auto& shellScript ){
              fs << "    " + shellScript
                + " /* ShellScript */ = {\n"
                + "      isa = PBXShellScriptBuildPhase;\n"
                + "      buildActionMask = 2147483647;\n"
                + "      files = (\n"
                + "      );\n"
                + "      inputFileListPaths = (\n"
                + "      );\n"
                + "      inputPaths = (\n"
                + "      );\n"
                + "      outputFileListPaths = (\n"
                + "      );\n"
                + "      outputPaths = (\n"
                + "      );\n"
                + "      runOnlyForDeploymentPostprocessing = 0;\n"
                + "      shellPath = /bin/sh;\n"
                + "      shellScript = \""
                + a_normalizeInstallScript( toInstallScript() )
                + "\";\n"
                + "    };\n"
              ;
            }
          );
          fs << "    /* End PBXShellScriptBuildPhase section */\n";
        }

      //}:                                        |
      //writePBXFileReferenceSources:{            |

        void Workspace::Xcode::writePBXFileReferenceSources( Writer& out )const{
          auto& T = *const_cast<self*>( this );
          writeFileReferenceGroups( out, T.inSources( Type::kStoryboard ), "file.storyboard",     "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kXcasset    ), "folder.assetcatalog", "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kPrefab     ), "file",                "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kLproj      ), "folder",              "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kPlist      ), "text.plist.xml",      "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kCpp        ), "sourcecode.cpp.cpp",  "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kMm         ), "sourcecode.cpp.objc", "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kHpp        ), "sourcecode.cpp.h",    "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kInl        ), "sourcecode.cpp.h",    "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kM          ), "sourcecode.c.objc",   "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kH          ), "sourcecode.c.h",      "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.inSources( Type::kC          ), "sourcecode.c.c",      "lastKnownFileType", "\"<group>\"" );
          writeFileReferenceGroups( out, T.toPublicRefs(                ), "folder",              "lastKnownFileType", "\"<group>\"" );
        }

      //}:                                        |
      //writePBXFileReferenceLibrary:{            |

        // THIS FUNCTION IS FROZEN: "when you're heart's not broken"
        void Workspace::Xcode::writePBXFileReferenceLibrary( Writer& out )const{

          //--------------------------------------------------------------------
          // Walk all the linker candidates including frameworks and .tbd's.
          //--------------------------------------------------------------------

          Files files;
          if( !toLinkWith().empty() ){
            const auto& vLibraries = toLinkWith().splitAtCommas();
            vLibraries.foreach(
              [&]( const auto& cref ){
                if( cref.empty() )
                  return;

                //--------------------------------------------------------------
                // Detect other product libs and add them to products vector.
                //--------------------------------------------------------------

                switch( cref.ext().tolower().hash() ){
                  case".dylib"_64:
                    [[fallthrough]];
                  case".a"_64:/**/{
                    string found;
                    const strings paths = toFindLibsPaths().splitAtCommas();
                    paths.foreachs(
                      [&]( const auto& _in ){
                          string path( _in );
                        if( path.back()!='/' )
                          path << "/" << cref.filename();
                        else
                          path << cref.filename();
                        if( e_fexists( path ))
                          found = path;
                        return found.empty();
                      }
                    );
                    m_vProducts.push( !found.empty()
                      ? File( found )
                      : File( cref ));
                    return;
                  }
                }

                //**************************************************************

                //--------------------------------------------------------------
                // Link against all library types, including .tbd, .a and .dylib.
                //--------------------------------------------------------------

                static const auto xcodeExists =
                    e_dexists( "/Applications/Xcode.app" );
                const auto& ext = cref.ext().tolower();
                if( xcodeExists ){

                  //------------------------------------------------------------
                  // Test whether intent was to link with text-based-dylibs among
                  // the plethora of Mac based library formats.
                  //------------------------------------------------------------

                  auto isTBD = true;
                  auto tbd = string();
                  const auto& ext = cref.ext().tolower();
                  switch( ext.hash() ){
                    case".framework"_64:
                      [[fallthrough]];
                    case".tbd"_64:
                      break;
                    case".dylib"_64:
                      [[fallthrough]];
                    case".a"_64:
                      [[fallthrough]];
                    case 0/* system frameworks */:
                      isTBD = false;
                      break;
                    default:/**/{
                      tbd = "lib" + cref + ".tbd";
                      const auto& targets = getTargets();
                      auto it = targets.getIterator();
                      while( it ){
                        if( !exists( it->hash()
                            , toFindLibsPaths(), tbd )){
                          tbd.clear();
                          break;
                        }
                        ++it;
                      }
                      break;
                    }
                  }

                  //------------------------------------------------------------
                  // Simple local function to set where and setup embed and sign.
                  //------------------------------------------------------------

                  static const auto& finalize=[](
                        const auto& library
                      , const auto& flags
                      , const auto& out )->File{
                    File f( library );
                    f.setWhere( out );
                    if( !flags->bNoEmbedAndSign ){
                      f.setEmbed( true );
                      f.setSign( true );
                    } return f;
                  };

                  //------------------------------------------------------------
                  // Try and find the library in one of all the many locations.
                  //------------------------------------------------------------

                  if( isTBD )/* only .framework / .tbd */{
                    const auto& targets = getTargets();
                    auto it = targets.getIterator();
                    auto ok = false;
                    string out;
                    while( it ){
                      out = cref;
                      if( ext.empty() ){
                        out << ".framework";
                        if( exists( it->hash(), toFindLibsPaths(), out )){
                          const auto& f2a = finalize(
                              cref
                            , m_tFlags
                            , out );
                          const_cast<Xcode*>( this )
                            -> toEmbedFiles().push( f2a );
                          files.push( f2a );
                          ok = true;
                          break;
                        }
                        out = "lib" + cref + ".tbd";
                        if( exists( it->hash(), toFindLibsPaths(), out )){
                          if( !m_mLibCache.find( out.hash() )){
                            m_mLibCache.set( out.hash(), 01 );
                            const auto& f2a = finalize(
                                cref
                              , m_tFlags
                              , out );
                            const_cast<Xcode*>( this )
                              -> toEmbedFiles().push( f2a );
                            files.push( f2a );
                            ok = true;
                            break;
                          }
                        }
                      }else{
                        out = "lib" + cref + ".tbd";
                        if( exists( it->hash(), toFindLibsPaths(), out )){
                          if( !m_mLibCache.find( out.hash() )){
                            m_mLibCache.set( out.hash(), 01 );
                            const auto& f2a = finalize( cref
                              , m_tFlags
                              , out );
                            const_cast<Xcode*>( this )
                              -> toEmbedFiles().push( f2a );
                            files.push( f2a );
                            ok = true;
                            break;
                          }
                        }
                      }
                      ++it;
                    }
                    if( ok ){
                      return;
                    }
                  }
                }
              }
            );

            //------------------------------------------------------------------
            // Write out the file and embedding line.
            //------------------------------------------------------------------

            // TODO: This is shitty! All the files are in m_vEmbedFiles with
            // TODO: all the flags set for embedding and signing!
            const auto embedAndSign = toEmbedFiles();
            files.foreach(
              [&]( File& f ){
                if( f.empty() )
                  return;
                m_vLibFiles.push( f );
                if( libCache.find( f.hash() ))
                  return;
                libCache.set( f.hash(), 1 );
                const auto/* no & */ext=(
                  ! f.toWhere().empty()
                  ? f.toWhere()
                  : f )
                  . ext()
                  . tolower()
                  . hash();
                if( f.isEmbed() ){

                  //------------------------------------------------------------
                  // Bail conditions.
                  //------------------------------------------------------------

                  const auto isHex = f.toBuildID().is_hex();
                  if( isHex )
                    return;

                  //------------------------------------------------------------
                  // Reference in frameworks.
                  //------------------------------------------------------------

                  out << "    "
                    + f.toBuildID()
                    + " /* [BuildID] "
                    + f.filename();
                  switch( ext ){
                    case".framework"_64:
                      out << " in Frameworks */ = {isa = PBXBuildFile; fileRef = ";
                      out << e_saferef( f )
                          << " /* "
                          << f.toWhere().os(/* expands $... */).filename();
                      out << " */; };\n";
                      break;
                    case".bundle"_64:
                      out << " in PlugIns */ = {isa = PBXBuildFile; fileRef = ";
                      break;
                    case".tbd"_64:
                      out << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                          << e_saferef( f )
                          << " /* "
                          << f.toWhere().os(/* expands $... */).filename();
                      out << " */; };\n";
                      break;
                    default:/**/{
                      out << " */ = {isa = PBXBuildFile; fileRef = "
                          << e_saferef( f )
                          << " /* "
                          << f.os(/* expands $... */).filename();
                      out << " */; };\n";
                      break;
                    }
                  }

                  //------------------------------------------------------------
                  // Local lambda function to add embedding syntax.
                  //------------------------------------------------------------

                  const auto& stayOnTarget=[&]( const string& target ){
                    if( target.hash() == "ios"_64 ){
                      if( ext == ".bundle"_64 ){
                        e_msgf(
                          "  Failed embedding %s for %s"
                          , ccp( f )
                          , ccp( f.toWhere() ));
                        return;
                      }
                      if( ext == ".dylib"_64 ){
                        e_msgf(
                          "  Failed embedding %s for %s"
                          , ccp( f )
                          , ccp( f.toWhere() ));
                        return;
                      }
                    }
                    out << "    "
                      + f.toEmbedID()
                      + " /* [EmbedID] "
                      + f.filename();
                    if( ext == ".framework"_64 ){
                      out << " in Embed Frameworks */ = {isa = PBXBuildFile; fileRef = ";
                    }else if(( target != "ios" )&&( ext == ".bundle"_64 )){
                      out << " in Embed Bundles */ = {isa = PBXBuildFile; fileRef = ";
                    }else if(( target != "ios" )&&( ext == ".dylib"_64 )){
                      out << " in Embed Dylibs */ = {isa = PBXBuildFile; fileRef = ";
                    }else{
                      out << " */ = {isa = PBXBuildFile; fileRef = ";
                    }
                    out << e_saferef( f )
                      + " /* "
                      + f.filename()
                      + " */; settings = {ATTRIBUTES = (";
                    if( !f.toWhere().empty() ){
                      out << " ); }; };\n";
                    }else{
                      if( f.isSign() )
                        out << "CodeSignOnCopy, ";
                      if(( ext == ".framework"_64 ) && f.isStrip() )
                        out << "RemoveHeadersOnCopy, ";
                      out << "); }; };\n";
                    }
                  };

                  //------------------------------------------------------------
                  // Reference in embedded frameworks.
                  //------------------------------------------------------------

                  const auto& targets = getTargets();
                  auto it = targets.getIterator();
                  while( it ){
                    stayOnTarget( *it );
                    ++it;
                  }
                  return;
                }

                //--------------------------------------------------------------
                // Handle non-embeddable targets.
                //--------------------------------------------------------------

                const auto& targets = getTargets();
                auto it = targets.getIterator();
                while( it ){
                  if( it->hash() == "macos"_64 ){
                    switch( ext ){
                      case".framework"_64:
                        [[fallthrough]];
                      case".bundle"_64:
                        [[fallthrough]];
                      case".dylib"_64:
                        [[fallthrough]];
                      case".tbd"_64:
                        [[fallthrough]];
                      case".a"_64:/**/{
                        out << "    "
                          + f.toBuildID()
                          + " /* [BuildID] "
                          + f.filename();
                        break;
                      }
                      default:/**/{
                        return;
                      }
                    }
                  }else{
                    switch( ext ){
                      case".framework"_64:
                        [[fallthrough]];
                      case".tbd"_64:
                        [[fallthrough]];
                      case".a"_64:/**/{
                        out << "    "
                          + f.toBuildID()
                          + " /* "
                          + f.filename();
                        break;
                      }
                      default:/**/{
                        return;
                      }
                    }
                  }
                  if( it->hash() == "macos"_64 ){
                    static const auto isVerbose =
                        e_getCvar( bool, "VERBOSE_LOGGING" );
                    if( isVerbose ){
                      e_msgf( "  Links \"%s\" (%s)",
                          ccp( f )
                        , ccp( f.toWhere() )
                      );
                    }
                    switch( ext ){
                      case".framework"_64:
                        out << " in Frameworks */ = {isa = PBXBuildFile; fileRef = ";
                        break;
                      case".tbd"_64:
                        out << " in TBDs */ = {isa = PBXBuildFile; fileRef = ";
                        break;
                      case".bundle"_64:
                        out << " in PlugIns */ = {isa = PBXBuildFile; fileRef = ";
                        break;
                      case".dylib"_64:
                        out << " in Dynamics */ = {isa = PBXBuildFile; fileRef = ";
                        break;
                      case".a"_64:
                        out << " in Statics */ = {isa = PBXBuildFile; fileRef = ";
                        break;
                    }
                  }else switch( ext ){
                    case".framework"_64:
                      out << " in Frameworks */ = {isa = PBXBuildFile; fileRef = ";
                      break;
                    case".tbd"_64:
                      out << " in TBDs */ = {isa = PBXBuildFile; fileRef = ";
                      break;
                    case".a"_64:
                      out << " in Statics */ = {isa = PBXBuildFile; fileRef = ";
                      break;
                  }
                  out << e_saferef( f )
                    + " /* [e_saferef] "
                    + f.filename()
                    + " */; };\n";
                  ++it;
                }
              }
            );
          }
        }

      //}:                                        |
      //writePBXFileReferenceSection:{            |

      void Workspace::Xcode::writePBXFileReferenceSection( Writer& out )const{

        //----------------------------------------------------------------------
        // Begin the section here; clearing file references too.
        //----------------------------------------------------------------------

        out << "\n    /* Begin PBXFileReference section */\n";
        auto& T = *const_cast<self*>( this );

        //----------------------------------------------------------------------
        // We need to take everything in m_aSources[ Type::kPlatform ] and make
        // the PBXFileReference section statement like the next. That should
        // fix everything to do with linking against system frameworks etc.
        // Anything in said m_aSources[ i ]. SYSTEM FRAMEWORKS HERE!
        //----------------------------------------------------------------------

        Files files;
        const auto& linksWith = toLinkWith().splitAtCommas();
        linksWith.foreach(
          [&]( const auto& lw ){
            File fi( lw );
            if( !fi.isSystemFramework() )
              return;
            const auto path=string(
              "/Applications/Xcode.app/Contents/Developer/Platforms/"
              "MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/"
              "Library/Frameworks/" ) + lw +
              ".framework";
            fi.setWhere( path );
            files.push( fi );
          }
        );

        //----------------------------------------------------------------------
        // Platform sources go HERE.
        //----------------------------------------------------------------------

        inSources( Type::kPlatform ).foreach(
          [&]( const auto& fi ){
            files.push( fi );
          }
        );

        //----------------------------------------------------------------------
        // Finally we can check file flags and write a FILE REF.
        //----------------------------------------------------------------------

        files.foreach(
          [&]( const auto& f ){
            const auto& ext = f.ext().tolower();
            const auto file = f.filename();
            const auto hash = ext.hash();
            // Type archive.a
            switch( hash ){

              //----------------------------------------------------------------
              // Handle shared libs.
              //----------------------------------------------------------------

              case".dylib"_64:/* dynamic library */{
                const auto& paths = toFindLibsPaths().splitAtCommas();
                string certainPath( f );
                string sourceTree = "BUILT_PRODUCTS_DIR";
                paths.foreachs(
                  [&]( const auto& path ){
                    const auto cp = path + "/" + file;
                    if( !e_fexists( cp ))
                      return true;
                    sourceTree = "\"<group>\"";
                    certainPath = "../" + cp;
                    return false;
                  }
                );
                out << "    "
                    << e_saferef( f )
                    << " /* [e_saferef] "
                    << file
                    << " in Frameworks"
                    << " */ = "
                    << "{isa = PBXFileReference;"
                    << " lastKnownFileType = \"compiled.mach-o.dylib\"; name = "
                    << file
                    << "; path = "
                    << certainPath
                    << "; "
                    << "sourceTree = "
                    << sourceTree
                    << ";";
                out << " };\n";
                break;
              }

              //----------------------------------------------------------------
              // Handle static libs.
              //----------------------------------------------------------------

              case".a"_64:/* static library */{
                const auto& paths = toFindLibsPaths().splitAtCommas();
                string certainPath( f );
                string sourceTree = "BUILT_PRODUCTS_DIR";
                paths.foreachs(
                  [&]( const auto& path ){
                    const auto cp = path + "/" + file;
                    if( !e_fexists( cp ))
                      return true;
                    sourceTree = "\"<group>\"";
                    certainPath = "../" + cp;
                    return false;
                  }
                );
                out << "    "
                    << e_saferef( f )
                    << " /* [e_saferef] "
                    << file
                    << " in Frameworks"
                    << " */ = "
                    << "{isa = PBXFileReference;"
                    << " lastKnownFileType = archive.a;"
                    << " name = "
                    << file
                    << "; path = "
                    << certainPath
                    << "; "
                    << "sourceTree = "
                    << sourceTree
                    << ";";
                out << " };\n";
                break;
              }

              //----------------------------------------------------------------
              // Handle source files.
              //----------------------------------------------------------------

              case".cpp"_64:
                [[fallthrough]];
              case".cxx"_64:
                [[fallthrough]];
              case".cc"_64:
                [[fallthrough]];
              case".mm"_64:
                [[fallthrough]];
              case".m"_64:
                [[fallthrough]];
              case".c"_64:/* C++ only */{
                string lastKnown;
                Files paths;
                switch( hash ){
                  case".cpp"_64:
                    [[fallthrough]];
                  case".cxx"_64:
                    [[fallthrough]];
                  case".cc"_64:
                    paths = inSources( Type::kCpp );
                    lastKnown = "sourcecode.cpp.cpp";
                    break;
                  case".mm"_64:
                    paths = inSources( Type::kMm );
                    lastKnown = "sourcecode.cpp.objc";
                    break;
                  case".m"_64:
                    lastKnown = "sourcecode.c.objc";
                    paths = inSources( Type::kM );
                    break;
                  case".c"_64:
                    paths = inSources( Type::kC );
                    lastKnown = "sourcecode.c.c";
                    break;
                }
                if( lastKnown.empty() )
                  break;
                if( paths.empty() )
                  break;
                const auto& fb = f.base();
                string certainPath( f );
                string sourceTree;
                paths.foreachs(
                  [&]( const auto& path ){
                    if( path.base() != fb )
                      return true;
                    certainPath = "../" + path.toWhere() + path;
                    sourceTree = "SOURCE_ROOT";
                    return false;
                  }
                );
                if( sourceTree.empty() )
                  break;
                out << "    "
                    << e_saferef( f )
                    << " /* [e_saferef] "
                    << file
                    << " in Sources"
                    << " */ = "
                    << "{isa = PBXFileReference;"
                    << " lastKnownFileType = "
                    << lastKnown
                    << "; name = "
                    << file
                    << "; path = "
                    << certainPath
                    << "; "
                    << "sourceTree = "
                    << sourceTree
                    << ";";
                out << " };\n";
                break;
              }

              //----------------------------------------------------------------
              // Handle TBD files.
              //----------------------------------------------------------------

              case".tbd"_64:
                e_break( "TBD linking is incomplete." );

              //----------------------------------------------------------------
              // Frameworks with and without suffix.
              //----------------------------------------------------------------

              case".framework"_64:/* frameworks */{
                const auto& paths = toFindLibsPaths().splitAtCommas();
                string certainPath( f );
                string sourceTree = "BUILT_PRODUCTS_DIR";
                paths.foreachs(
                  [&]( const auto& path ){
                    const auto cp = path + "/" + file;
                    if( !e_fexists( cp ))
                      return true;
                    sourceTree = "\"<group>\"";
                    certainPath = "../" + cp;
                    return false;
                  }
                );
                const auto& embedRef( e_saferef( f ));
                toEmbedFiles().foreachs(
                  [&]( const auto& _f ){
                    if( !_f.isEmbed() )
                      return true;
                    const_cast<File&>( f ).setEmbedRef( embedRef );
                    return false;
                  }
                );
                out << "    "
                    << embedRef
                    << " /* [e_saferef] "
                    << file
                    << " in Embed Frameworks"
                    << " */ = "
                    << "{isa = PBXFileReference;"
                    << " lastKnownFileType = wrapper.framework; "
                    << " name = "
                    << file
                    << "; path = "
                    << certainPath
                    << "; "
                    << "sourceTree = "
                    << sourceTree
                    << ";";
                out << " };\n";
                auto found = false;
                Class::foreachs<Xcode>(
                  [&]( const auto& p ){
                    if( p.isLabel( file.base() ))
                      found = true;
                    return !found;
                  }
                );
                if( found )
                  break;
                [[fallthrough]];
              }
              case 0:/* ie, CoreFoundation[.framework] */{
                string ext;
                ext << ".framework";
                out << "    "
                    << e_saferef( f )
                    << " /* [e_saferef] "
                    << file
                    << " in Frameworks"
                    << " */ = "
                    << "{isa = PBXFileReference;"
                    << " lastKnownFileType = wrapper.framework;"
                    << " name = "
                    << file
                    << ".framework; path = "
                    << "/Applications/Xcode.app/Contents/Developer/Platforms/"
                    << "MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/"
                    << "Library/Frameworks/"
                    << file
                    << ext
                    << "; sourceTree = \"<absolute>\";";
                out << " };\n";
                break;
              }
            }
          }
        );

        //----------------------------------------------------------------------
        // Links with (again?) This is where I pull in all the parallel project
        // libs including ".framework", ".dylib", and ".a".
        //----------------------------------------------------------------------

        toEmbedFiles().foreach(
          [&]( const auto& f ){
            Files fileVector{ f };
            switch( f.ext().tolower().hash() ){
              case".framework"_64:/**/{
                writeFileReferenceGroups( out
                  , fileVector
                  , "wrapper.framework"
                  , "explicitFileType"
                  , "BUILT_PRODUCTS_DIR" );
                break;
              }
              case".bundle"_64:/**/{
                writeFileReferenceGroups( out
                  , fileVector
                  , "wrapper.cfbundle"
                  , "explicitFileType"
                  , "BUILT_PRODUCTS_DIR" );
                break;
              }
              case".dylib"_64:/**/{
                writeFileReferenceGroups( out
                  , fileVector
                  , "\"compiled.mach-o.dylib\""
                  , "explicitFileType"
                  , "BUILT_PRODUCTS_DIR" );
                break;
              }
              case".a"_64:/**/{
                writeFileReferenceGroups( out
                  , fileVector
                  , "archive.a"
                  , "explicitFileType"
                  , "BUILT_PRODUCTS_DIR" );
                break;
              }
            }
          }
        );
        writeFileReferenceGroups( out
          , T.inSources( Type::kFramework )
          , "wrapper.framework"
          , "explicitFileType"
          , "BUILT_PRODUCTS_DIR" );
        writeFileReferenceGroups( out
          , T.inSources( Type::kBundle )
          , "wrapper.cfbundle"
          , "explicitFileType"
          , "BUILT_PRODUCTS_DIR" );
        writeFileReferenceGroups( out
          , T.inSources( Type::kSharedLib )
          , "\"compiled.mach-o.dylib\""
          , "explicitFileType"
          , "BUILT_PRODUCTS_DIR" );
        writeFileReferenceGroups( out
          , T.inSources( Type::kStaticLib )
          , "archive.a"
          , "explicitFileType"
          , "BUILT_PRODUCTS_DIR" );

        //----------------------------------------------------------------------
        // Entitlements.
        //----------------------------------------------------------------------

        if( hasEntitlements() ){
          File f( toLabel() + ".entitlements" );
          File::filerefs.set( f.hash(), m_sEntFileRefID );
          f.setBuildID( m_sEntBuildID );
          f.setFileRef( f.hash() );
          Files fileVector{ f };
          writeFileReferenceGroups( out
            , fileVector
            , "text.plist.entitlements"
            , "lastKnownFileType"
            , "\"<group>\""
          );
        }

        //----------------------------------------------------------------------
        // Header files.
        //----------------------------------------------------------------------

        toPublicHeaders().foreach(
          [&]( const auto& _f ){
            if( _f.empty() )
              return;
            auto& f = const_cast<File&>( _f );
            string lastKnownFileType;
            switch( f.os(/* expands [~|/|.] */).ext().tolower().hash() ){
              case".h"_64:
                lastKnownFileType = "sourcecode.c.h";
                break;
              default:
                lastKnownFileType = "folder";
                break;
            }
            const auto& osFilename = f.os().filename();
            const auto& osWhere    = f.toWhere().os();
            out << "    "
                << e_saferef( f )
                << " /* e_saferef] "
                << f.os().filename()
                << " */ = {isa = PBXFileReference; lastKnownFileType = "
                << lastKnownFileType
                << "; name = "    << osFilename
                << "; path = ../" << osWhere << osFilename
                << "; sourceTree = "
                << "\"<group>\"; ";
            out << "};\n";
          }
        );

        //----------------------------------------------------------------------
        // Library files.
        //----------------------------------------------------------------------

        addToPBXFileReferenceSection( out,
          [&]( const auto& target
             , const auto& label
             , const auto& prod ){

            //------------------------------------------------------------------
            // Everything we're linking against.
            //------------------------------------------------------------------

            toLibFiles().foreach(
              [&]( const auto& lib ){
                if( lib.empty() )
                  return;
                auto isProduct = false;
                Class::foreachs<Xcode>(
                  [&]( const auto& xcode ){
                    if( this == &xcode )
                      return true;
                    auto l( "lib"
                      + xcode.toLabel()
                      + ".a" );
                    if( l == lib ){
                      isProduct = true;
                    }else{
                      auto l( xcode.toLabel()
                        + ".bundle" );
                      if( l == lib ){
                        isProduct = true;
                      }else{
                        auto l( xcode.toLabel()
                          + ".framework" );
                        if( l == lib ){
                          isProduct = true;
                        }else{
                          auto l( xcode.toLabel()
                            + ".dylib" );
                          if( l == lib ){
                            isProduct = true;
                          }
                        }
                      }
                    }
                    return!isProduct;
                  }
                );
                File f( lib );
                const auto _lib = f.toWhere().os();
                if( _lib.empty() )
                  e_break( "Empty string encountered!" );
                const auto _ext =_lib.ext().tolower();
                const auto hash =_ext.hash();
                string fileType;
                if( target == "macos"_64 ){
                  switch( hash ){
                    case".framework"_64:
                      fileType = "wrapper.framework";
                      break;
                    case".bundle"_64:
                      fileType = "wrapper.cfbundle";
                      break;
                    case".dylib"_64:
                      fileType = "\"compiled.mach-o.dylib\"";
                      break;
                    case".tbd"_64:
                      fileType = "\"sourcecode.text-based-dylib-definition\"";
                      break;
                    case".a"_64:
                      fileType = "archive.ar";
                      break;
                    default:
                      e_break( e_xfs( "Unhandled file type (%s)!", ccp( _ext )));
                  }
                }else if( target == "ios" ){
                  switch( hash ){
                    case".framework"_64:
                      fileType = "wrapper.framework";
                      break;
                    case".tbd"_64:
                      fileType = "\"sourcecode.text-based-dylib-definition\"";
                      break;
                    case".a"_64:
                      fileType = "archive.ar";
                      break;
                    default:
                      break;
                  }
                }
                out << "    " + e_saferef( f );
                if( !isProduct ){
                  out << " /* [e_saferef] " << _lib << " */ = {isa = PBXFileReference; lastKnownFileType = ";
                }else{
                  out << " /* " << _lib << " */ = {isa = PBXFileReference; explicitFileType = ";
                }
                out << fileType
                    << "; name = "
                    << _lib.filename()
                    << "; path = ";
                out << ( !f.toWhere().empty() ? f.toWhere() : f.filename() );
                switch( hash ){
                  case".framework"_64:
                    if( isProduct ){
                      out << "; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                    }else{
                      out << "; sourceTree = \"<absolute>\"; };\n";
                    }
                    break;
                  case".bundle"_64:
                    if( isProduct ){
                      out << "; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                    }else{
                      out << "; sourceTree = \"<group>\"; };\n";
                    }
                    break;
                  case".tbd"_64:
                    // Don't change source tree, it'll knacker the PBX code.
                    out << "; sourceTree = \"<absolute>\"; };\n";
                    break;
                  case".dylib"_64:
                    [[fallthrough]];
                  case".a"_64:
                    if( isProduct ){
                      out << "; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                      break;
                    }
                    [[fallthrough]];
                  default:
                    out << "; sourceTree = \"<group>\"; };\n";
                    break;
                }
              }
            );
          }
        );

        //----------------------------------------------------------------------
        // The project.
        //----------------------------------------------------------------------

        addToPBXFileReferenceSection( out,
          [&]( const auto& target
             , const auto& label
             , const auto& prod ){
            switch( toBuild().hash() ){
              case"framework"_64: out << "    "
                  << prod
                  << " /* "
                  << toLabel()
                  << label
                  << ".framework */ = {isa = PBXFileReference; explicitFileType = wrapper.framework; includeInIndex = 0; path = "
                  << toLabel()
                  << label
                  << ".framework; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
              case"bundle"_64:
                if( target.hash() != "ios"_64 ) out << "    "
                  << prod
                  << " /* "
                  << toLabel()
                  << label
                  << ".bundle */ = {isa = PBXFileReference; explicitFileType = wrapper.cfbundle; includeInIndex = 0; path = "
                  << toLabel()
                  << label
                  << ".bundle; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
              case"shared"_64:
                if( target.hash() != "ios"_64 ) out << "    "
                  << prod
                  << " /* lib"
                  << toLabel()
                  << label
                  << ".dylib */ = {isa = PBXFileReference; explicitFileType = \"compiled.mach-o.dylib\"; includeInIndex = 0; path = lib"
                  << toLabel()
                  << label
                  << ".dylib; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
              case"static"_64: out << "    "
                  << prod
                  << " /* lib"
                  << toLabel()
                  << label
                  << ".a */ = {isa = PBXFileReference; explicitFileType ="
                  << " archive.ar; includeInIndex = 0; path = lib"
                  << toLabel()
                  << label
                  << ".a; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
              case"application"_64: out << "    "
                  << prod
                  << " /* "
                  << toLabel()
                  << label
                  << " */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = "
                  << toLabel()
                  << label
                  << ".app; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
              case"console"_64:
                if( target.hash() != "ios"_64 ) out << "    "
                  << prod
                  << " /* "
                  << toLabel()
                  << label
                  << " */ = {isa = PBXFileReference; explicitFileType = compiled.mach-o.executable; includeInIndex = 0; path = "
                  << toLabel()
                  << label
                  << "; sourceTree = BUILT_PRODUCTS_DIR; };\n";
                break;
            }
          }
        );

        //----------------------------------------------------------------------
        // Library files; this path is officially FROZEN so "let it go" or for
        // an even older reference: "that'll do pig, that'll do."
        //----------------------------------------------------------------------

        writePBXFileReferenceLibrary( out );
        writePBXFileReferenceSources( out );

        out << "    /* End PBXFileReference section */\n";
      }

      //}:                                        |
      //writePBXVariantGroupSection:{             |

        void Workspace::Xcode::writePBXVariantGroupSection( Writer& fs )const{
          fs << "\n    /* Begin PBXVariantGroup section */\n";
          fs << "    /* End PBXVariantGroup section */\n";
        }

      //}:                                        |
      //writePBXNativeTargetSection:{             |

        void Workspace::Xcode::writePBXNativeTargetSection( Writer& fs )const{
          fs << "\n    /* Begin PBXNativeTarget section */\n";
          addToPBXNativeTargetSection( fs,
            [&]( const auto& target // e.g. macos, ios, ipados
               , const auto& label // e.g. SisuXD, iSisuXD
               , const auto& targetBuild
               , const auto& targetFramework
               , const auto& phaseFramework
               , const auto& phaseResources
               , const auto& phaseHeaders
               , const auto& phaseSources
               , const auto& phaseScript
               , const auto& embedFrameworks
               , const auto& embedPlugins
               , const auto& productFileRef
               , const auto& copyRefs ){
              fs << "    "
                  + targetFramework
                  + " /* framework */ = {\n"
                  + "      isa = PBXNativeTarget;\n"
                  + "      buildConfigurationList = "
                  + targetBuild
                  + " /* Build configuration list for PBXNativeTarget \""
                  + label
                  + "\" */;\n"
                  + "      buildPhases = (\n"
                  + "        "
                  + phaseFramework
                  + " /* Frameworks */,\n"
                  + "        "
                  + phaseResources
                  + " /* Resources */,\n";
              if(( toBuild() == "application"_64 )||(
                   toBuild() == "framework"_64 )||((
                   toBuild() == "bundle"_64 ))){
                fs << "        " + copyRefs + " /* CopyFiles */,\n";
                fs << "        " + embedFrameworks + " /* Embed Frameworks */,\n";
              }
              if( !phaseHeaders.empty() )
                fs << "        " + phaseHeaders + " /* Headers */,\n";
              fs << "        " + phaseSources + " /* Sources */,\n";
              fs << "        " + embedPlugins + " /* CopyFiles (PlugIns) */,\n";
              if( !toInstallScript().empty() )
                fs << "        " + phaseScript + " /* Script */,\n";
              fs << string( "      );\n" )
                  + "      buildRules = (\n"
                  + "      );\n"
                  + "      dependencies = (\n"
                  + "      );\n"
                  + "      name = \"" + label + "\";\n"
                  + "      productName = \"" + label + "\";\n";
              switch( toBuild().hash() ){
                case"framework"_64:
                  fs << "      productReference = " + productFileRef + " /* " + label + ".framework */;\n";
                  fs << "      productType = \"com.apple.product-type.framework\";\n";
                  break;
                case"bundle"_64:
                  if( target.hash() != "macos"_64 )
                    e_break( "Cannot create a bundle for iOS targets." );
                  fs << "      productReference = " + productFileRef + " /* " + label + ".bundle */;\n";
                  fs << "      productType = \"com.apple.product-type.bundle\";\n";
                  break;
                case"shared"_64:
                  if( target.hash() != "macos"_64 )
                    e_break( "Cannot create a shared library for iOS targets." );
                  fs << "      productReference = " + productFileRef + " /* lib" + label + ".a */;\n";
                  fs << "      productType = \"com.apple.product-type.library.dynamic\";\n";
                  break;
                case"static"_64:
                  fs << "      productReference = " + productFileRef + " /* lib" + label + ".a */;\n";
                  fs << "      productType = \"com.apple.product-type.library.static\";\n";
                  break;
                case"application"_64:
                  fs << "      productReference = " + productFileRef + " /* " + label + ".app */;\n";
                  fs << "      productType = \"com.apple.product-type.application\";\n";
                  break;
                case"console"_64:
                  if( target.hash() != "macos"_64 )
                    e_break( "Cannot create a shared library for iOS targets!" );
                  fs << "      productReference = " + productFileRef + " /* " + label + " */;\n";
                  fs << "      productType = \"com.apple.product-type.tool\";\n";
                  break;
              }
              fs << "    };\n";
            }
          );
          fs << "    /* End PBXNativeTarget section */\n";
        }

      //}:                                        |
      //writePBXBuildFileSection:{                |

        void Workspace::Xcode::writePBXBuildFileSection( Writer& out )const{
          out << "\n    /* Begin PBXBuildFile section */\n";

          //--------------------------------------------------------------------
          // Moved "Generating" here so it's the first thing we do.
          //--------------------------------------------------------------------

          e_msgf( "Generating %s" , ccp( toLabel().tolower() ));
          File::filerefs.clear();

          //--------------------------------------------------------------------
          // LINK WITH system frameworks when desired. This only handles a
          // system framework, including TBD( text based dylibs ).
          //--------------------------------------------------------------------

          if( !toLinkWith().empty() ){
            const auto& with = toLinkWith().splitAtCommas();
            std::set<u64> hits;
            with.foreach(
              [&]( const auto& w ){
                if( hits.find(    w.hash() )==hits.end() )
                    hits.emplace( w.hash() );
                else return;
                File f( w );

                //--------------------------------------------------------------
                // Append extension to filename: necessary in case of system f.
                //--------------------------------------------------------------

                if( f.isSystemFramework() ){
                  (( Xcode* )this )->inSources( Type::kPlatform ).push( f );
                  out << "    "
                      << f.toBuildID()
                      << " /* [LinkWith] "
                      << f.filename()
                      << ".framework in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                      << e_forceref( f )// Force don't just trap Hapgood.
                      << " /* "
                      << f.filename();
                  out << " */; };\n";
                }else if( f.ext().empty() ){// <-- ie, the eon framework.
                  auto found = false;
                  Class::foreachs<Xcode>(
                    [&]( const auto& p ){
                      if( !p.isLabel( f ))
                        return true;
                      switch( p.toBuild().hash() ){
                        case"framework"_64:
                          f << ".framework";
                          const_cast<Xcode*>( this )
                            -> inSources( Type::kPlatform ).push( f );
                          out << "    "
                              << f.toBuildID()
                              << " /* [BuildID] "
                              << f.filename()
                              << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                              << e_saferef( f ) // traps bugs for ya, Hapgood!
                              << " /* "
                              << f.filename();
                          out << " */; };\n";
                          break;
                        case"bundle"_64:
                          f << ".bundle";
                          const_cast<Xcode*>( this )
                            -> inSources( Type::kPlatform ).push( f );
                          out << "    "
                              << f.toBuildID()
                              << " /* "
                              << f.filename()
                              << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                              << e_saferef( f ) // traps bugs for ya, Hapgood!
                              << " /* "
                              << f.filename();
                          out << " */; };\n";
                          break;
                        case"shared"_64:
                          f << ".dylib";
                          const_cast<Xcode*>( this )
                            -> inSources( Type::kPlatform ).push( f );
                          break;
                        case"static"_64:
                          f << ".a";
                          break;
                        default:
                          return true;
                      }
                      found = true;
                      return false;
                    }
                  );
                  if( !found ){
                    f << ".framework";
                  }
                }
                const auto& ext_hash = f.ext().tolower().hash();
                switch( ext_hash ){
                  case".framework"_64:
                    [[fallthrough]];
                  case".bundle"_64:
                    [[fallthrough]];
                  case".dylib"_64:
                    return;
                  default:
                    break;
                }
                if( !ext_hash )
                  return;
                (( Xcode* )this )->inSources( Type::kPlatform ).push( f );
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Now copy all EMBEDDED LIB references.
          //--------------------------------------------------------------------

          { std::set<u64> hits;
            const_cast<Xcode*>( this )->
            toEmbedFiles().foreach(
              [&]( File& f ){
                if( hits.find(    f.hash() )==hits.end() )
                    hits.emplace( f.hash() );
                else return;
                if( f.ext().empty() )
                  return;

                //--------------------------------------------------------------
                // Embedding and copying libraries.
                //--------------------------------------------------------------

                if( f.isEmbed() )
                  e_msgf( "  Embedding %s", ccp( f ));
                const auto& ext = f.ext().tolower();
                const auto hash = ext.hash();
                switch( hash ){
                  case".framework"_64:
                    break;
                  case".bundle"_64:
                    break;
                  case".dylib"_64:
                    break;
                  default:/* Everything else */{
                    out << "    "
                        << f.toBuildID()
                        << " /* [BuildID (EmbedFiles)] "
                        << f.filename()
                        << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                        << f.toBuildID2()
                        << " /* [BuildID2] "
                        << f.filename();
                    out << " */; };\n";
                    break;
                  }
                }
                if( f.isEmbed() )
                  out << "    "
                      << f.toBuildID2()
                      << " /* [BuildID2 (EmbedFiles)] "
                      << f.filename()
                      << " in Embed Frameworks */ = {isa = PBXBuildFile; fileRef = "
                      << f.toBuildID()
                      << " /* [BuildID] "
                      << f.filename();
             else out << "    "
                      << f.toBuildID2()
                      << " /* [buildID2] "
                      << f.filename()
                      << " in CopyFiles */ = {isa = PBXBuildFile; fileRef = "
                      << f.toBuildID()
                      << " /* [BuildID] "
                      << f.filename();
                out << " */;";
                auto stripper = false;
                switch( hash ){
                  case".framework"_64:
                    stripper = true;
                    [[fallthrough]];
                  case".bundle"_64:
                    out << " settings = {ATTRIBUTES = (";
                    if( f.isSign() )
                      out << "CodeSignOnCopy, ";
                    if( stripper )
                      out << "RemoveHeadersOnCopy, ";
                    out << "); };";
                    break;
                  case".dylib"_64:
                    out << " settings = {ATTRIBUTES = (";
                    if( f.isSign() )
                      out << "CodeSignOnCopy,";
                    out << " ); };";
                    break;
                  case".a"_64:
                    break;
                }
                out << " };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Add and filter all static/shared files by type.
          //--------------------------------------------------------------------

          Files files;
          addToFiles( files, inSources( Type::kSharedLib ));
          addToFiles( files, inSources( Type::kStaticLib ));
          if( !files.empty() ){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Frameworks */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Add and filter all resource files by known type.
          //--------------------------------------------------------------------

          files.clear();
          addToFiles( files, inSources( Type::kStoryboard ));
          addToFiles( files, inSources( Type::kXcasset ));
          addToFiles( files, inSources( Type::kPrefab ));
          addToFiles( files, inSources( Type::kLproj ));
          addToFiles( files, inSources( Type::kPlist ));
          if( !files.empty() ){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                if( f.empty() )
                  return;
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Resource */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // This is the file references to add to a framewwork or bundle. Like
          // eon.frameworks public files, I believe, private and public headers
          // are referenced by the PrivateHeaders and PublicHeaders vectors.
          //--------------------------------------------------------------------

          files.clear();
          if( addToFiles( files, toPublicRefs() )){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in CopyFiles */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Now add all the private header files.
          //--------------------------------------------------------------------

          files.clear();
          if( addToFiles( files, toPrivateHeaders() )){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Headers */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; settings = {ATTRIBUTES = (Private, ); }; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Now add all the public header files.
          //--------------------------------------------------------------------

          files.clear();
          if( addToFiles( files, toPublicHeaders() )){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Headers */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; settings = {ATTRIBUTES = (Public, ); }; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Source files.
          //--------------------------------------------------------------------

          files.clear();
          addToFiles( files, inSources( Type::kCpp ));
          addToFiles( files, inSources( Type::kMm  ));
          addToFiles( files, inSources( Type::kM   ));
          addToFiles( files, inSources( Type::kC   ));
          if( !files.empty() ){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                if( f.empty() )
                  return;
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Sources */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Header files.
          //--------------------------------------------------------------------

          files.clear();
          addToFiles( files, inSources( Type::kHpp ));
          addToFiles( files, inSources( Type::kH   ));
          if( !files.empty() ){
            ignore( files, toIgnoreParts() );
            files.foreach(
              [&]( auto& f ){
                if( f.empty() )
                  return;
                out << "    "
                    << f.toBuildID()
                    << " /* "
                    << f.filename()
                    << " in Headers */ = {isa = PBXBuildFile; fileRef = "
                    << e_saferef( f )
                    << " /* "
                    << f.filename();
                out << " */; };\n";
              }
            );
          }

          //--------------------------------------------------------------------
          // Ending comment.
          //--------------------------------------------------------------------

          out << "    /* End PBXBuildFile section */\n";
        }

      //}:                                        |
      //writePBXProjectSection:{                  |

        void Workspace::Xcode::writePBXProjectSection( Writer& fs )const{
          fs << "\n    /* Begin PBXProject section */\n";
          fs << "    " + m_sProjectObject + " /* Project object */ = {\n"
              + "      isa = PBXProject;\n"
              + "      attributes = {\n";
          if( bmp->bXcode11 ){
            fs << "        LastUpgradeCheck = 1120;\n";
          }else if( bmp->bXcode12 ){
            fs << "        LastUpgradeCheck = 1200;\n";
          }else if( bmp->bXcode15 ){
            fs << "        LastUpgradeCheck = 1500;\n";
          }else if( bmp->bXcode16 ){
            fs << "        LastUpgradeCheck = 1600;\n";
          }
          if( !toOrgName().empty() ){ fs
            << "        ORGANIZATIONNAME = \"" + toOrgName() + "\";\n";
          }
          fs << "        TargetAttributes = {\n"
             << "          ";
          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            const auto& target = *it;
            string label;
            if( target == "macos"_64 ){
              fs << m_aFrameNativeTarget[ Target::macOS ];
            }else if( target == "ios"_64 ){
              fs << m_aFrameNativeTarget[ Target::iOS ];
              label = toLabel() + "ios";
            }else{
              fs << m_aFrameNativeTarget[ Target::macOS ];
            }
            fs << " = {\n";
            if( bmp->bXcode11 ){
              fs << "            CreatedOnToolsVersion = 11.2.1;\n";
            }else if( bmp->bXcode12 ){
              fs << "            CreatedOnToolsVersion = 12;\n";
            }else if( bmp->bXcode15 ){
              fs << "            CreatedOnToolsVersion = 15;\n";
            }else if( bmp->bXcode16 ){
              fs << "            CreatedOnToolsVersion = 16;\n";
            }
            fs << "          };\n        };\n      };\n";
            fs << "      buildConfigurationList = ";
            if( target == "macos"_64 ){
              fs << m_aBuildConfigurationList[ Target::macOS ];
            }else{
              fs << m_aBuildConfigurationList[ Target::iOS ];
            }
            fs << " /* Build configuration list for PBXProject \""
               << "\" */;\n";
            if( bmp->bXcode11 ){
              fs << "      compatibilityVersion = \"Xcode 9.3\";\n";
            }else if( bmp->bXcode12 ){
              fs << "      compatibilityVersion = \"Xcode 12.0\";\n";
            }else if( bmp->bXcode15 ){
              fs << "      compatibilityVersion = \"Xcode 15.0\";\n";
            }else if( bmp->bXcode16 ){
              fs << "      compatibilityVersion = \"Xcode 16.0\";\n";
            }
            fs << "      developmentRegion = en;\n"
               << "      hasScannedForEncodings = 0;\n"
               << "      knownRegions = (\n"
               << "        en,\n"
               << "        Base,\n"
               << "      );\n"
               << "      mainGroup = " + m_sMainGroup + ";\n"
               << "      productRefGroup = " + m_sProductsGroup + " /* Products */;\n"
               << "      projectDirPath = \"\";\n"
               << "      projectRoot = \"\";\n"
               << "      targets = (\n";
            if( target == "macos"_64 ){
              fs << "        "
                + m_aFrameNativeTarget[ Target::macOS ]
                + " /* "
                + toLabel()
                + " */,\n"
              ;
            }else{
              const auto& framework = m_aFrameNativeTarget[ Target::iOS ];
              fs << "        "
                + framework
                + " /* "
                + toLabel()
                + target
                + " */,\n"
              ;
            }
            fs << "      );\n"
               << "    };\n";
            ++it;
          }
          fs << "    /* End PBXProject section */\n";
        }

      //}:                                        |
      //writePBXGroupSection:{                    |

      void Workspace::Xcode::writePBXGroupSection( Writer& fs )const{
        fs << "\n    /* Begin PBXGroup section */\n";

          //--------------------------------------------------------------------
          // Top level group.
          //--------------------------------------------------------------------

          fs << "    " + m_sMainGroup + " = {\n"
             << "      isa = PBXGroup;\n"
             << "      children = (\n";
          if( hasEntitlements() ){
            fs << "        "
               << m_sEntFileRefID
               << " /* "
               << toLabel();
            fs << ".entitlements */,\n";
          }
          fs << "        " + m_sFrameworkGroup + " /* Frameworks */,\n"
             << "        " + m_sProductsGroup  + " /* Products */,\n"
             << "        " + m_sPluginsGroup   + " /* Plugins */,\n"
             << "        " + m_sCodeGroup      + " /* Code */,\n"
             << "      );\n"
             << "      sourceTree = \"<group>\";\n"
             << "    };\n";

          //--------------------------------------------------------------------
          // Add PBX groups.
          //--------------------------------------------------------------------

          Files files;
          addToPBXGroupSection( fs,
            [&]( const string& product
               , const string& target
               , const string& label ){

              //----------------------------------------------------------------
              // Frameworks group.
              //----------------------------------------------------------------

              // Write out the Group SID first.
              fs << "    "
                 << m_sFrameworkGroup
                 << " /* Frameworks */ = {\n"
                 << "      isa = PBXGroup;\n"
                 << "      children = (\n";
              // Collect everything we want to embed.
              Files collection;
              inSources( Type::kPlatform ).foreach(
                [&]( const auto& fi ){
                  collection.push( fi );
                }
              );
              collection.sort(
                []( const auto& a, const auto& b ){
                  return( a.len() > b.len() );
                }
              );
              hashmap<u64,s8>hit;
              collection.foreach(
                [&]( const auto& f ){
                  if( !hit.find( f.hash() ))
                    hit.set( f.hash(), 1 );
                  else return;
                  fs << "        " // Library reference per child.
                     << e_saferef( f )
                     << " /* [e_saferef] "
                     << f.filename();
                  fs << " */,\n";
                }
              );
              fs << string( "      );\n" )
                 << "      name = Frameworks;\n"
                 << "      sourceTree = \"<group>\";\n";
              fs << "    };\n";

              //----------------------------------------------------------------
              // Plugins group.
              //----------------------------------------------------------------

              // Write out the Group SID first.
              fs << "    "
                 << m_sPluginsGroup
                 << " /* Plugins */ = {\n"
                 << "      isa = PBXGroup;\n"
                 << "      children = (\n";
              // Collect everything we want to embed.
              Files plugins;
              toEmbedFiles().foreach(
                [&]( const var& f ){
                  plugins.push( f );
                }
              );
              plugins.sort(
                []( const auto& a, const auto& b ){
                  return( a.len() > b.len() );
                }
              );
              std::set<u64>___;
              plugins.foreach(
                [&]( const auto& f ){
                  if(___.find( f.hash() )==___.end() )
                    ___.emplace( f.hash() );
                  else return;
                  fs << "        " // Library reference per child.
                     << f.toBuildID()
                     << " /* [BuildID] "
                     << f.filename();
                  fs << " */,\n";
                }
              );
              fs << string( "      );\n" )
                 << "      name = Plugins;\n"
                 << "      sourceTree = \"<group>\";\n";
              fs << "    };\n";

              //----------------------------------------------------------------
              // Products group.
              //----------------------------------------------------------------

              auto build( toBuild() );
              switch( build.hash( ) ){
                case"shared"_64:
                  build = ".dylib";
                  break;
                case"static"_64:
                  build = ".a";
                  break;
                default:
                  build = "." + build;
                  break;
              }

              //----------------------------------------------------------------
              // Save out the Products group.
              //----------------------------------------------------------------

              fs << "    "
                 << m_sProductsGroup
                 << " /* Products */ = {\n"
                 << "      isa = PBXGroup;\n"
                 << "      children = (\n"
                 << "        "
                 << product
                 << " /* "
                 << label
                 << build
                 << " */,\n"
                 << "      );\n"
                 << "      name = Products;\n"
                 << "      sourceTree = \"<group>\";\n"
                 << "    };\n";

              //----------------------------------------------------------------
              // Compute header inclusions.
              //----------------------------------------------------------------

              const auto n_headers = 0u
                 + inSources( Type::kHpp ).size()
                 + inSources( Type::kInl ).size()
                 + inSources( Type::kH   ).size();
              if( n_headers ){
                  fs << "    "
                     << m_sIncludeGroup
                     << " /* include */ = {\n"
                     << "      isa = PBXGroup;\n"
                     << "      children = (\n";
                inSources( Type::kHpp ).foreach(
                  [&]( const auto& fi ){ files.push( fi ); });
                inSources( Type::kInl ).foreach(
                  [&]( const auto& fi ){ files.push( fi ); });
                inSources( Type::kH ).foreach(
                  [&]( const auto& fi ){ files.push( fi ); });
                files.sort(
                  []( const auto& a, const auto& b ){
                    return( a < b );
                  }
                );
                files.foreach(
                  [&]( const File& file ){
                    // File reference added per child.
                    fs << "        "
                       << e_forceref( file )
                       << " /* " + file.filename()
                       << " */,\n";
                  }
                );
                fs << "      );\n";
                fs << "      name = include;\n";
                fs << "      sourceTree = \"<group>\";\n";
                fs << "    };\n";
              }
            }
          );

          //--------------------------------------------------------------------
          // Resources group.
          //--------------------------------------------------------------------

          const auto n_resources = 0u
             + inSources( Type::kStoryboard ).size()
             + inSources( Type::kXcasset    ).size()
             + inSources( Type::kPrefab     ).size()
             + inSources( Type::kLproj      ).size();
          if( n_resources ){
            fs << "    " + m_sResourcesGroup + " /* resources */ = {\n"
               << "      isa = PBXGroup;\n"
               << "      children = (\n";
            files.clear();
            inSources( Type::kStoryboard ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kXcasset    ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kPrefab     ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kLproj      ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            files.sort(
              []( const auto& a, const auto& b ){
                return( a < b );
              }
            );
            files.foreach(
              [&]( const File& file ){
                fs << "        "
                   << e_saferef( file )
                   << " /* "
                   << file.filename();
                fs << " */,\n";
              }
            );
            fs << "      );\n";
            fs << "      name = Resources;\n";
            fs << "      sourceTree = \"<group>\";\n";
            fs << "    };\n";
          }

          //--------------------------------------------------------------------
          // Code group.
          //--------------------------------------------------------------------

          const auto hasReferences=( !toPublicHeaders().empty()||!toPublicRefs().empty() );
          fs << "    " + m_sCodeGroup + " /* Code */ = {\n"
             << "      isa = PBXGroup;\n"
             << "      children = (\n"
             << "        " + m_sReferencesGroup + " /* references */,\n"
             << "        " + m_sResourcesGroup + " /* resources */,\n"
             << "        " + m_sIncludeGroup + " /* include */,\n"
             << "        " + m_sSrcGroup + " /* src */,\n"
             << "      );\n"//don't with prev or next line.
             << "      name = Code;\n"
             << "      sourceTree = \"<group>\";\n";
          fs << "    };\n";

          //--------------------------------------------------------------------
          // Exporting public headers/references from framework.
          //--------------------------------------------------------------------

          if( hasReferences ){
            const auto n_refs = 0u
                + toPublicHeaders().size()
                + toPublicRefs().size();
            if( n_refs ){
              fs << "    " + m_sReferencesGroup + " /* references */ = {\n"
                 << "      isa = PBXGroup;\n"
                 << "      children = (\n";
              files.clear();
              toPublicHeaders().foreach( [&]( const auto& fi ){ files.push( fi ); });
              toPublicRefs   ().foreach( [&]( const auto& fi ){ files.push( fi ); });
              files.sort(
                []( const auto& a, const auto& b ){
                  return( a < b );
                }
              );
              files.foreach(
                [&]( const auto& f ){ fs
                  << "        "
                  << e_saferef( f )
                  << " /* "
                  << ccp( f )
                  << " */,\n";
                }
              );
              fs << "      );\n";
              fs << "      name = references;\n";
              fs << "      path = \"\";\n";
              fs << "      sourceTree = \"<group>\";\n";
              fs << "    };\n";
            }
          }

          //--------------------------------------------------------------------
          // Source group.
          //--------------------------------------------------------------------

          const auto n_sources = 0u
              + inSources( Type::kCpp ).size()
              + inSources( Type::kMm  ).size()
              + inSources( Type::kC   ).size()
              + inSources( Type::kM   ).size();
          if( n_sources ){
            fs << "    " + m_sSrcGroup + " /* src */ = {\n"
                + "      isa = PBXGroup;\n"
                + "      children = (\n";
            files.clear();
            inSources( Type::kCpp ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kMm  ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kC   ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            inSources( Type::kM   ).foreach( [&]( const auto& fi ){ files.push( fi ); });
            files.sort(
              []( const auto& a, const auto& b ){
                return( a < b );
              }
            );
            files.foreach(
              [&]( const File& file ){
                fs << "        " + e_saferef( file ) + " /* " + file.filename() + " */,\n";
              }
            );
            fs << "      );\n";
            fs << "      name = src;\n";
            fs << "      path = \"\";\n";
            fs << "      sourceTree = \"<group>\";\n";
            fs << "    };\n";
          }

        fs << "    /* End PBXGroup section */\n";
      }

      //}:                                        |
    //}:                                          |
    //writeXC*:{                                  |
      //writeXCBuildConfigSystemHeaderSearch:{    |

        strings Workspace::Xcode::writeXCBuildConfigSystemHeaderSearch( Writer& fs )const{
          fs << "        SYSTEM_HEADER_SEARCH_PATHS = (\n";
          auto syspaths = toIncludePaths().splitAtCommas();
          if( ! toIncludePaths().empty() ){
            const auto n=syspaths.size();
            for( auto i=0u; i<n; ++i ){
              syspaths.alter( i,
                [&]( string& path ){
                  path = "../"+path;
                }
              );
            }
          }
          syspaths.foreach(
            [&]( const auto& path ){
              fs << "          " + path + ",\n";
            }
          );
          fs << "        );\n";
          return syspaths;
        }

      //}:                                        |
      //addToXCBuildConfigurationSection:{        |

        void Workspace::Xcode::addToXCBuildConfigurationSection( fs::Writer&
            , const std::function<void(
              const string& target
            , const string& relNative
            , const string& dbgNative
            , const string& relBuild
            , const string& dbgBuild )>& lambda )const{
          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            const auto& target = *it;
            if( target == "macos"_64 ){
              lambda( "macos"
                , m_aReleaseBuildConfiguration[ Target::macOS ]
                , m_aDebugBuildConfiguration  [ Target::macOS ]
                , m_aReleaseNativeBuildConfig [ Target::macOS ]
                , m_aDebugNativeBuildConfig   [ Target::macOS ]
              );
            }else{
              lambda( "ios"
                , m_aReleaseBuildConfiguration[ Target::iOS ]
                , m_aDebugBuildConfiguration  [ Target::iOS ]
                , m_aReleaseNativeBuildConfig [ Target::iOS ]
                , m_aDebugNativeBuildConfig   [ Target::iOS ]
              );
            }
            ++it;
          }
        }

      //}:                                        |
      //writeXCBuileConfigurationSection:{        |

      void Workspace::Xcode::writeXCBuildConfigurationSection( Writer& fs )const{

        //----------------------------------------------------------------------
        // Local lambda to write out the LDFLAGS section.
        //----------------------------------------------------------------------

        const auto& addOtherCppFlags = [&]( const string& config ){};
        const auto& addOtherLDFlags  = [&](
              const string& config
            , const string& target ){
          auto libs = toLibraryPaths();
          libs.replace( "$(CONFIGURATION)", config );
          libs.splitAtCommas().foreach(
            [&]( const string& f ){
              auto dir = f;
              if(( *dir != '/' )&&( *dir != '~' )&&( *dir != '.' )){
                dir = "../" + f;
              }
              // Some verbose logging to debug the directory 'dir'.
              if( e_getCvar( bool, "VERBOSE_LOGGING" )){
                e_msgf( "      dir: \"%s\""
                  , ccp( dir )
                );
              }
              fs << "          -L" + dir + ",\n";
            }
          );
        };

        //----------------------------------------------------------------------
        // Begin build configuration section.
        //----------------------------------------------------------------------

        fs << "\n    /* Begin XCBuildConfiguration section */\n";
        addToXCBuildConfigurationSection( fs,
          [&]( const string& target
             , const string& relNative
             , const string& dbgNative
             , const string& relConfig
             , const string& dbgConfig ){

            //------------------------------------------------------------------
            // DEBUG build configuration.
            //------------------------------------------------------------------

            fs << "    " + dbgNative + " /* Debug */ = {\n"
                + "      isa = XCBuildConfiguration;\n"
                + "      buildSettings = {\n";
            string lang;
            switch( wsp->toLanguage() ){
              case"c++23"_64:
                lang = "c++2b";
                break;
              case"c++20"_64:// TODO: Which is right? In msvc.cpp we use 20!
                lang = "c++20";
                break;
              case"c++17"_64:
                lang = "c++17";
                break;
              case"c++14"_64:
                lang = "c++14";
                break;
              case"c++11"_64:
                lang = "c++11";
                break;
            }
            fs << string( "        ALWAYS_SEARCH_USER_PATHS = NO;\n" )
                + "        CLANG_ANALYZER_NONNULL = YES;\n"
                + "        CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;\n"
                + "        CLANG_CXX_LANGUAGE_STANDARD = \"" + lang + "\";\n"
                + "        CLANG_CXX_LIBRARY = \"libc++\";\n"
                + "        CLANG_ENABLE_MODULES = YES;\n";
            string enableARC;
            if( toFlags()->bEnableARC ){
              enableARC = "YES";
            }else{
              enableARC = "NO";
            }
            fs << "        CLANG_ENABLE_OBJC_ARC = " + enableARC + ";\n";
            fs << "        CLANG_ENABLE_OBJC_WEAK = YES;\n";
            if( !bmp->bXcode11 ){
              fs << "        CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;\n";
            }
            fs << "        CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;\n"
               << "        CLANG_WARN_BOOL_CONVERSION = YES;\n"
               << "        CLANG_WARN_COMMA = NO;\n"
               << "        CLANG_WARN_CONSTANT_CONVERSION = YES;\n"
               << "        CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;\n"
               << "        CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;\n"
               << "        CLANG_WARN_DOCUMENTATION_COMMENTS = NO;\n"
               << "        CLANG_WARN_EMPTY_BODY = YES;\n"
               << "        CLANG_WARN_ENUM_CONVERSION = YES;\n"
               << "        CLANG_WARN_INFINITE_RECURSION = YES;\n"
               << "        CLANG_WARN_INT_CONVERSION = YES;\n"
               << "        CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;\n"
               << "        CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;\n"
               << "        CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;\n"
               << "        CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;\n"
               << "        CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;\n"
               << "        CLANG_WARN_STRICT_PROTOTYPES = YES;\n"
               << "        CLANG_WARN_SUSPICIOUS_MOVE = YES;\n"
               << "        CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;\n"
               << "        CLANG_WARN_UNREACHABLE_CODE = YES;\n"
               << "        CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;\n"
               << "        COPY_PHASE_STRIP = NO;\n"
               << "        CURRENT_PROJECT_VERSION = 1;\n"
               << "        DEBUG_INFORMATION_FORMAT = dwarf;\n"
               << "        ENABLE_STRICT_OBJC_MSGSEND = YES;\n"
               << "        ENABLE_TESTABILITY = YES;\n"
               << "        GCC_C_LANGUAGE_STANDARD = " + toLanguageC() + ";\n"
               << "        GCC_DYNAMIC_NO_PIC = NO;\n"
               << "        GCC_NO_COMMON_BLOCKS = YES;\n"
               << "        GCC_OPTIMIZATION_LEVEL = 0;\n"
               << "        GCC_PREPROCESSOR_DEFINITIONS = (\n";
            fs << "          \"$(inherited)\",\n";
            string dbgDefines = toDefinesDbg();
            const auto& dbgVector = dbgDefines.splitAtCommas();
            dbgVector.foreach(
              [&]( const auto& define ){
                if( define.empty() ){
                  return;
                }
                if( *define == '#' ){
                  return;
                }
                fs << "          \"" + define + "\",\n";
              }
            );
            fs << string( "        );\n" )
                + "        GCC_WARN_64_TO_32_BIT_CONVERSION = YES;\n"
                + "        GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;\n"
                + "        GCC_WARN_UNDECLARED_SELECTOR = YES;\n"
                + "        GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;\n"
                + "        GCC_WARN_UNUSED_FUNCTION = YES;\n"
                + "        GCC_WARN_UNUSED_VARIABLE = YES;\n";
            if( !toPrefixHeader().empty() ){
              fs << "        GCC_PRECOMPILE_PREFIX_HEADER = YES;\n";
              fs << "        GCC_PREFIX_HEADER = \"../" + toPrefixHeader() + "\";\n";
            }
            fs << string( "        MACOSX_DEPLOYMENT_TARGET = " + toDeployment() + ";\n" )
                + "        MTL_ENABLE_DEBUG_INFO = INCLUDE_SOURCE;\n"
                + "        MTL_FAST_MATH = YES;\n"
                + "        ONLY_ACTIVE_ARCH = YES;\n";
            if( target == "macos"_64 ){
              fs << "        SDKROOT = macosx;\n";
            }else{
              fs << "        SDKROOT = iphoneos;\n";
            }
            fs << "        VERSIONING_SYSTEM = \"apple-generic\";\n"
               << "        VERSION_INFO_PREFIX = \"\";\n"
               << "      };\n"
               << "      name = Debug;\n"
               << "    };\n";

            //------------------------------------------------------------------
            // RELEASE build configuration.
            //------------------------------------------------------------------

            fs << "    " + relConfig + " /* Release */ = {\n"
                + "      isa = XCBuildConfiguration;\n"
                + "      buildSettings = {\n";
            if( toFlags()->bUniversalBinary ){
              //Note: no ARCHS = ? gives us a universal binary.
            }else if(( target == "ios" )||toFlags()->bAppleSilicon ){
              fs << "        VALID_ARCHS = arm64;\n";
              fs << "        ARCHS = arm64;\n";
            }else{
              fs << "        VALID_ARCHS = x86_64;\n";
              fs << "        ARCHS = x86_64;\n";
            }
            fs << "        ALWAYS_SEARCH_USER_PATHS = NO;\n"
               << "        CLANG_ANALYZER_NONNULL = YES;\n"
               << "        CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;\n"
               << "        CLANG_CXX_LANGUAGE_STANDARD = \"" + toLanguage() + "\";\n"
               << "        CLANG_CXX_LIBRARY = \"libc++\";\n"
               << "        CLANG_ENABLE_MODULES = YES;\n"
               << "        CLANG_ENABLE_OBJC_ARC = " + enableARC + ";\n"
               << "        CLANG_ENABLE_OBJC_WEAK = YES;\n";
            if( !bmp->bXcode11 ){
              fs << "        CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;\n";
            }
            fs << "        CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;\n"
               << "        CLANG_WARN_BOOL_CONVERSION = YES;\n"
               << "        CLANG_WARN_COMMA = YES;\n"
               << "        CLANG_WARN_CONSTANT_CONVERSION = YES;\n"
               << "        CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;\n"
               << "        CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;\n"
               << "        CLANG_WARN_DOCUMENTATION_COMMENTS = YES;\n"
               << "        CLANG_WARN_EMPTY_BODY = YES;\n"
               << "        CLANG_WARN_ENUM_CONVERSION = YES;\n"
               << "        CLANG_WARN_INFINITE_RECURSION = YES;\n"
               << "        CLANG_WARN_INT_CONVERSION = YES;\n"
               << "        CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;\n"
               << "        CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;\n"
               << "        CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;\n"
               << "        CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;\n"
               << "        CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;\n"
               << "        CLANG_WARN_STRICT_PROTOTYPES = YES;\n"
               << "        CLANG_WARN_SUSPICIOUS_MOVE = YES;\n"
               << "        CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;\n"
               << "        CLANG_WARN_UNREACHABLE_CODE = YES;\n"
               << "        CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;\n"
               << "        COPY_PHASE_STRIP = NO;\n"
               << "        CURRENT_PROJECT_VERSION = 1;\n"
               << "        DEBUG_INFORMATION_FORMAT = \"dwarf-with-dsym\";\n"
               << "        ENABLE_NS_ASSERTIONS = NO;\n"
               << "        ENABLE_STRICT_OBJC_MSGSEND = YES;\n"
               << "        GCC_C_LANGUAGE_STANDARD = " + toLanguageC() + ";\n"
               << "        GCC_NO_COMMON_BLOCKS = YES;\n"
               << "        GCC_OPTIMIZATION_LEVEL = fast;\n"
               << "        GCC_PREPROCESSOR_DEFINITIONS = (\n";
            fs << "          \"$(inherited)\",\n";

            //------------------------------------------------------------------
            // RELEASE stuff.
            //------------------------------------------------------------------

            string relDefines = toDefinesRel();
            const auto& vrel = relDefines.splitAtCommas();
            vrel.foreach(
              [&]( const string& define ){
                if( define.empty() ){
                  return;
                }
                if( *define == '#' ){
                  return;
                }
                fs << "          \"" + define + "\",\n";
              }
            );
            fs << string( "        );\n" )
                + "        GCC_WARN_64_TO_32_BIT_CONVERSION = YES;\n"
                + "        GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;\n"
                + "        GCC_WARN_UNDECLARED_SELECTOR = YES;\n"
                + "        GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;\n"
                + "        GCC_WARN_UNUSED_FUNCTION = YES;\n"
                + "        GCC_WARN_UNUSED_VARIABLE = YES;\n";
            if( !toPrefixHeader().empty() ){
              fs << "        GCC_PRECOMPILE_PREFIX_HEADER = YES;\n";
              fs << "        GCC_PREFIX_HEADER = \"../" + toPrefixHeader() + "\";\n";
            }
            if( target == "ios"_64 ){
              fs << string( "        IPHONEOS_DEPLOYMENT_TARGET = " + toDeployment() + ";\n" );
            }else{
              fs << string( "        MACOSX_DEPLOYMENT_TARGET = " + toDeployment() + ";\n" );
            }
            fs << "        MTL_ENABLE_DEBUG_INFO = NO;\n"
               << "        MTL_FAST_MATH = YES;\n";
            if( target == "ios"_64 ){
              fs << "        SDKROOT = iphoneos;\n";
            }else{
              fs << "        SDKROOT = macosx;\n";
            }
            fs << "        VERSIONING_SYSTEM = \"apple-generic\";\n"
               << "        VERSION_INFO_PREFIX = \"\";\n"
               << "      };\n"
               << "      name = Release;\n"
               << "    };\n";
            fs << "    " + dbgNative + " /* Debug */ = {\n"
                + "      isa = XCBuildConfiguration;\n"
                + "      buildSettings = {\n";
            if( toFlags()->bUniversalBinary ){
              //Note: no ARCHS = ? gives us a universal binary.
            }else if(( target == "ios" )||toFlags()->bAppleSilicon ){
              fs << "        VALID_ARCHS = arm64;\n";
              fs << "        ARCHS = arm64;\n";
            }else{
              fs << "        VALID_ARCHS = x86_64;\n";
              fs << "        ARCHS = x86_64;\n";
            }
            if( hasEntitlements() ){
              fs << "        CODE_SIGN_ENTITLEMENTS = ./"
                 << toLabel()
                 << ".entitlements;\n"
              ;
            }
            fs << "        CODE_SIGN_STYLE = Automatic;\n";
            if( !toTeamName().empty() ){
              fs << "        DEVELOPMENT_TEAM = " + toTeamName() + ";\n";
            }
            fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
            fs << "          \"$(inherited)\",\n";
            fs << "          \"@executable_path/../Frameworks\",\n";
            fs << "        );\n";
            fs << "        LIBRARY_SEARCH_PATHS = (\n";
            auto libraryPaths = toFindLibsPaths().splitAtCommas();
            libraryPaths.foreach(
              [&]( const string& f ){
                auto dir = f;
                if(( *dir != '/' )&&( *dir != '~' )&&( *dir != '.' )){
                  dir = "../" + f;
                }
                dir.replace( "$(CONFIGURATION)", "Debug" );
                // Fix corner case where 'dir' is destroyed.
                ccp end = strstr( dir, ".framework" );
                if( end ){
                  dir = string( dir, end );
                }
                fs << "          " + dir + ",\n";
              }
            );
            fs << "        );\n";
            fs << "        FRAMEWORK_SEARCH_PATHS = (\n";
            auto frameworkSearchPaths
              = toFrameworkPaths()
              . splitAtCommas();
            frameworkSearchPaths.foreach(
              [&]( const auto& _dir ){
                File dir( _dir );
                dir.replace( "$(CONFIGURATION)", "Debug" );
                fs << "          " + dir + ",\n";
              }
            );
            fs << "        );\n";
            auto paths = writeXCBuildConfigSystemHeaderSearch( fs );
            switch( toBuild().hash() ){
              //----------------------------------+-----------------------------
              //application:{                     |

                case"application"_64:
                  fs << "        ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INFOPLIST_KEY_CFBundleDisplayName = \"SisuXD\";\n";
                  fs << "        INFOPLIST_KEY_LSApplicationCategoryType = \"public.app-category.graphics-design\";\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        ENABLE_HARDENED_RUNTIME = ";
                  fs <<  string( toFlags()->bHardenedRuntime
                    ? "YES"
                    : "NO" );
                  fs << + ";\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  break;

              //}:                                |
              //framework:{                       |

                case"framework"_64:
                  fs << "        COMBINE_HIDPI_IMAGES = YES;\n";
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INFOPLIST_KEY_CFBundleDisplayName = \"SisuXD\";\n";
                  fs << "        INFOPLIST_KEY_LSApplicationCategoryType = \"public.app-category.graphics-design\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/Frameworks\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../Frameworks\",\n";
                  fs << "          \"@loader_path/Frameworks\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //console:{                         |

                case"console"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        ENABLE_HARDENED_RUNTIME = ";
                  fs << string( toFlags()->bHardenedRuntime
                    ? "YES"
                    : "NO" );
                  fs << ";\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  break;

              //}:                                |
              //bundle:{                          |

                case"bundle"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        COMBINE_HIDPI_IMAGES = YES;\n";
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../"
                       << inSources( Type::kPlist )[ 0 ].toWhere()
                       << inSources( Type::kPlist )[ 0 ]
                       << "\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/PlugIns\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../PlugIns\",\n";
                  fs << "          \"@loader_path/PlugIns\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //shared:{                          |

                case"shared"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/PlugIns\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../PlugIns\",\n";
                  fs << "          \"@loader_path/PlugIns\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //static:{                          |

                case"static"_64:
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        EXECUTABLE_PREFIX = lib;\n";
                  break;

              //}:                                |
              //----------------------------------+-----------------------------
            }
            fs << "        SKIP_INSTALL = YES;\n";
            if( toBuild() == "bundle"_64 ){
              fs << "        WRAPPER_EXTENSION = bundle;\n";
            }
            fs << "      };\n";
            fs << "      name = Debug;\n";
            fs << "    };\n";

            //------------------------------------------------------------------
            // Handle all the build types: Release.
            //------------------------------------------------------------------

            fs << "    " + relNative + " /* Release */ = {\n"
               << "      isa = XCBuildConfiguration;\n"
               << "      buildSettings = {\n";
            if( hasEntitlements() ){
              fs << "        CODE_SIGN_ENTITLEMENTS = "
                 << toLabel()
                 << ".entitlements;\n"
              ;
            }
            fs << "        CODE_SIGN_STYLE = Automatic;\n";
            if( !toTeamName().empty() ){
              fs << "        DEVELOPMENT_TEAM = " + toTeamName() + ";\n";
            }
            fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
            fs << "          \"$(inherited)\",\n";
            fs << "          \"@executable_path/../Frameworks\",\n";
            fs << "        );\n";
            fs << "        LIBRARY_SEARCH_PATHS = (\n";
            libraryPaths.foreach(
              [&]( const string& f ){
                auto dir = f;
                if(( *dir != '/' )&&( *dir != '~' )&&( *dir != '.' )){
                  dir = "../" + f;
                }
                // Fix corner case where 'dir' is destroyed.
                ccp end = strstr( dir, ".framework" );
                if( end ){
                  dir = string( dir, end );
                }
                dir.replace( "$(CONFIGURATION)", "Release" );
                fs << "          " + dir + ",\n";
              }
            );
            fs << "        );\n";
            fs << "        FRAMEWORK_SEARCH_PATHS = (\n";
            frameworkSearchPaths.foreach(
              [&]( const string& f ){
                auto dir = f;
                if(( *dir != '/' )&&( *dir != '~' )&&( *dir != '.' )){
                  dir = "../" + f;
                }
                dir.replace( "$(CONFIGURATION)", "Release" );
                fs << "          " + dir + ",\n";
              }
            );
            fs << "        );\n";
            paths = writeXCBuildConfigSystemHeaderSearch( fs );
            switch( toBuild().hash() ){
              //----------------------------------+-----------------------------
              //application:{                     |

                case"application"_64:
                  fs << "        ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        ENABLE_HARDENED_RUNTIME = ";
                  fs << string( toFlags()->bHardenedRuntime ? "YES" : "NO" );
                  fs << ";\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Release" );
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Release", target );
                  fs << "        );\n";
                  break;

              //}:                                |
              //framework:{                       |

                case"framework"_64:
                  fs << "        COMBINE_HIDPI_IMAGES = YES;\n";
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/Frameworks\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../Frameworks\",\n";
                  fs << "          \"@loader_path/Frameworks\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Release" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Release", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //console:{                         |

                case"console"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        ENABLE_HARDENED_RUNTIME = ";
                  fs << string( toFlags()->bHardenedRuntime ? "YES" : "NO" ) + ";\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Release" );
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Release", target );
                  fs << "        );\n";
                  break;

              //}:                                |
              //bundle:{                          |

                case"bundle"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        COMBINE_HIDPI_IMAGES = YES;\n";
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/PlugIns\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../PlugIns\",\n";
                  fs << "          \"@loader_path/PlugIns\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Debug" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Debug", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //shared:{                          |

                case"shared"_64:
                  if( target == "ios"_64 )
                    e_break( "Cannot set the target to \"console\" for iOS." );
                  fs << "        DEFINES_MODULE = YES;\n";
                  fs << "        DYLIB_COMPATIBILITY_VERSION = 1;\n";
                  fs << "        DYLIB_CURRENT_VERSION = 1;\n";
                  fs << "        DYLIB_INSTALL_NAME_BASE = \"@rpath\";\n";
                  if( !inSources( Type::kPlist ).empty() )
                    fs << "        INFOPLIST_FILE = \"$(SRCROOT)/../" + inSources( Type::kPlist )[ 0 ] + "\";\n";
                  fs << "        INSTALL_PATH = \"$(LOCAL_LIBRARY_DIR)/Frameworks\";\n";
                  fs << "        LD_RUNPATH_SEARCH_PATHS = (\n";
                  fs << "          \"$(inherited)\",\n";
                  fs << "          \"@executable_path/../Frameworks\",\n";
                  fs << "          \"@loader_path/Frameworks\",\n";
                  fs << "        );\n";
                  fs << "        OTHER_CPLUSPLUSFLAGS = (\n";
                  addOtherCppFlags( "Release" );
                  fs << "        );\n";
                  fs << "        OTHER_CFLAGS = (\n";
                  fs << "        );\n";
                  fs << "        OTHER_LDFLAGS = (\n";
                  if( toFlags()->bLoadAllSymbols )
                    fs << "          -all_load,\n";
                  addOtherLDFlags( "Release", target );
                  fs << "        );\n";
                  fs << "        PRODUCT_BUNDLE_IDENTIFIER = \"" + m_sProductBundleId.tolower() + "\";\n";
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME:c99extidentifier)\";\n";
                  break;

              //}:                                |
              //static:{                          |

                case"static"_64:
                  fs << "        PRODUCT_NAME = \"$(TARGET_NAME)\";\n";
                  fs << "        EXECUTABLE_PREFIX = lib;\n";
                  break;

              //}:                                |
              //----------------------------------+-----------------------------
            }
            fs << "        SKIP_INSTALL = YES;\n";
            if( toBuild() == "bundle"_64 ){
              fs << "        WRAPPER_EXTENSION = bundle;\n";
            }
            fs << "      };\n";
            fs << "      name = Release;\n";
            fs << "    };\n";
          }
        );
        fs << "    /* End XCBuildConfiguration section */\n";
      }

      //}:                                        |
      //addToXCConfigurationListSection:{         |

        void Workspace::Xcode::addToXCConfigurationListSection( Writer& fs
            , const std::function<void(
                const string& target
              , const string& config
              , const string& build
              , const string& relNative
              , const string& dbgNative
              , const string& relBuild
              , const string& dbgBuild
              , const string& label )>& lambda )const{
          const auto& targets = getTargets();
          auto it = targets.getIterator();
          while( it ){
            auto target( *it );
            string config;
            string build;
            string relN;
            string relB;
            string dbgN;
            string dbgB;
            string lbl;
            if( target == "macos"_64 ){
              config = m_aBuildConfigurationList   [ Target::macOS ];
              build  = m_aBuildNativeTarget        [ Target::macOS ];
              relN   = m_aReleaseNativeBuildConfig [ Target::macOS ];
              dbgN   = m_aDebugNativeBuildConfig   [ Target::macOS ];
              relB   = m_aReleaseBuildConfiguration[ Target::macOS ];
              dbgB   = m_aDebugBuildConfiguration  [ Target::macOS ];
            }else{//iPhone and iPad.
              config = m_aBuildConfigurationList   [ Target::iOS ];
              build  = m_aBuildNativeTarget        [ Target::iOS ];
              relN   = m_aReleaseNativeBuildConfig [ Target::iOS ];
              dbgN   = m_aDebugNativeBuildConfig   [ Target::iOS ];
              relB   = m_aReleaseBuildConfiguration[ Target::iOS ];
              dbgB   = m_aDebugBuildConfiguration  [ Target::iOS ];
              lbl    = "ios";
            }
            lambda( target
              , config
              , build
              , relN
              , dbgN
              , relB
              , dbgB
              , toLabel() + lbl );
            ++it;
          }
        }

      //}:                                        |
      //writeXCConfigurationListSection:{         |

        void Workspace::Xcode::writeXCConfigurationListSection( Writer& fs )const{
          fs << "\n    /* Begin XCConfigurationList section */\n";
          addToXCConfigurationListSection( fs,
            [&]( const string& target
               , const string& config
               , const string& build
               , const string& relN
               , const string& dbgN
               , const string& relB
               , const string& dbgB
               , const string& lbl ){
              fs << "    "
                  + build
                  + " /* Build configuration list for PBXProject \""
                  + lbl
                  + "\" */ = {\n"
                  + "      isa = XCConfigurationList;\n"
                  + "      buildConfigurations = (\n"
                  + "        " + relB + " /* Release */,\n"
                  + "        " + dbgB + " /* Debug */,\n"
                  + "      );\n"
                  + "      defaultConfigurationIsVisible = 0;\n"
                  + "      defaultConfigurationName = Release;\n"
                  + "    };\n";
              fs << "    "
                  + config
                  + " /* Build configuration list for PBXNativeTarget \""
                  + lbl
                  + "\" */ = {\n"
                  + "      isa = XCConfigurationList;\n"
                  + "      buildConfigurations = (\n"
                  + "        " + relN + " /* Release */,\n"
                  + "        " + dbgN + " /* Debug */,\n"
                  + "      );\n"
                  + "      defaultConfigurationIsVisible = 0;\n"
                  + "      defaultConfigurationName = Release;\n"
                  + "    };\n"
               ;
            }
          );
          fs << "    /* End XCConfigurationList section */\n";
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//Ctor:{                                          |

#ifdef __APPLE__
  #pragma mark (ctor)
#endif

  Workspace::Xcode::Xcode(){
    m_tFlags->bHardenedRuntime = 1;
    m_tFlags->bEnableARC = 1;
    keyCache.clear();
    libCache.clear();
  }

//}:                                              |
//================================================+=============================
