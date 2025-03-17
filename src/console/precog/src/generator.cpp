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
//Structs:{                                       |
  //Generator:{                                   |

    template<typename T> struct Generator final:Object{

      e_reflect_no_properties( Generator, Object );

      //------------------------------------------+-----------------------------
      //Aliases:{                                 |

        using Type = typename T::Type;

      //}:                                        |
      //Methods:{                                 |

        e_noinline bool addFiles(){
          if( !m_pProject )
            return false;
          const string paths[]{
            m_pProject->toResPath(),
            m_pProject->toSrcPath(),
          };
          for( u32 i=0; i<e_dimof( paths ); ++i ){
            if( !paths[ i ].empty() ){
              const auto& innerPaths = paths[ i ].splitAtCommas();
              innerPaths.foreach(
                [&]( const string& innerPath ){
                  if( IEngine::dexists( innerPath )){
                    IEngine::dir( innerPath,
                      [this]( const string& d
                            , const string& f
                            , const bool isDirectory ){
                        if( f.hash()==".DS_Store"_64 )
                          return true;
                        if( isDirectory ){
                          const auto& d_ext = f.ext().tolower();
                          if( !d_ext.empty() ){
                            switch( d_ext.hash() ){
                              case".xcassets"_64:
                                break;
                              default:/**/{
                                // NB: If the extension is non-empty then we
                                // must return. Otherwise we'll recurse thru
                                // Lelu-XD packages and other Mac related
                                // bundles picking up way too many images.
                                return true;
                              }
                            }
                          }
                        }
                        m_pProject->sortingHat( d + f );
                        return true;
                      }
                    );
                  }else{
                    m_pProject->sortingHat( innerPath );
                  }
                  return true;
                }
              );
            }
          }
          return true;
        }

      //}:                                        |
      //------------------------------------------+-----------------------------

      Generator( T* pProject )
        : m_pProject( pProject )
      {}

      Generator() = default;
    ~ Generator() = default;

    private:

      T* m_pProject = nullptr;
    };

  //}:                                            |
//}:                                              |
//Extends:{                                       |

#ifdef __APPLE__
  #pragma mark - Extensions -
#endif

  e_specialized_extends( Generator<Workspace::Xcode> );
  e_specialized_extends( Generator<Workspace::Ninja> );
  e_specialized_extends( Generator<Workspace::MSVC>  );

//}:                                              |
//Methods:{                                       |
  //lua_gather:{                                  |

#ifdef __APPLE__
  #pragma mark - Functions -
#endif

    namespace{

      //------------------------------------------------------------------------
      // Get string function.
      //------------------------------------------------------------------------

      string lua_getCleansedID( lua_State* L, const s32 ix ){
        if( !lua_isstring( L, ix ))
          return nullptr;
        string input = lua_tostring( L, ix );
        if( input.empty() )
          return nullptr;
        input.erase( "\n" );
        string result;
        cp  r = cp( input.c_str() );
        ccp e = input.end();
        while( r &&( r < e )){
          if((( r[0]=='-' )&&( r[1]=='-' ))||( *r == '#' )){
            r = string::skip_2eol( r );
          }else if(( *r == '"' )||( *r == '\'' )){
            r = strchr( r, *r );
          }else if( ' ' == *r ){
            ++r;
          }else{
            result.catf( "%c", *r++ );
          }
        }
        if( result.back() == ',' )
          result.trim( 1 );
        return result;
      }

      //------------------------------------------------------------------------
      // XCODE gathering function.
      //------------------------------------------------------------------------

      void lua_gather( lua_State* L, Workspace::Xcode& p ){
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string& key = lua_tostring( L, -2 );
          switch( key.hash() ){

            //------------------------------------+-----------------------------
            //Hardening:{                         |

              case"m_hardenedRuntime"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bNoEmbedAndSign = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bNoEmbedAndSign = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //Symbolics:{                         |

              case"m_loadAllSymbols"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bLoadAllSymbols = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bLoadAllSymbols = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //Universal:{                         |

              case"m_enableUniversal"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bUniversalBinary = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bUniversalBinary = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //EnableJIT:{                         |

              case"m_enableJIT"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bEnableJIT = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bEnableJIT = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //DisablePageProtection:{             |

              case"m_disablePageProtection"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bDisablePageProtection = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bDisablePageProtection = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //DisableLibValidation:{              |

              case"m_disableLibValidation"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bDisableLibValidation = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bDisableLibValidation = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //AppleSilicon:{                      |

              case"m_enableAppleSilicon"_64:/**/{
                const auto& boolean = lua_getCleansedID( L, -1 );
                if( boolean.empty() )
                  break;
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.toFlags()->bAppleSilicon = 0;
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.toFlags()->bAppleSilicon = 1;
                    break;
                }
                break;
              }

            //}:                                  |
            //Libs:{                              |

              case"m_libs"_64:/**/{
                const auto& gathered = lua_getCleansedID( L, -1 );
                if( !gathered.empty() )
                  p.setLibraryPaths( gathered );
                break;
              }

            //}:                                  |
            //EmbedSign:{                         |

              case"m_filesToEmbedAndSign"_64:/**/{
                const auto& s = lua_getCleansedID( L, -1 );
                if( s.empty() )
                  break;
                const auto& embeddables = s.splitAtCommas();
                auto it = embeddables.getIterator();
                while( it ){
                  Workspace::File f( *it );
                  f.setEmbed( true );
                  f.setSign( true );
                  p.toEmbedFiles()
                    . push( f );
                  ++it;
                }
                p.setEmbedAndSign( s );
                break;
              }

              case"m_noEmbedAndSign"_64:/**/{
                const string opt( lua_tostring( L, -1 ));
                if( opt.empty() ){
                  p.toFlags()->bNoEmbedAndSign = 1;
                  break;
                }
                switch( opt.hash() ){
                  case"false"_64:
                    [[fallthrough]];
                  case"no"_64:
                    p.toFlags()->bNoEmbedAndSign = 0;
                    break;
                  case"true"_64:
                    [[fallthrough]];
                  case"yes"_64:
                    p.toFlags()->bNoEmbedAndSign = 1;
                    break;
                }
                break;
              }

              case"m_teamName"_64:/**/{
                const string& teamName = lua_tostring( L, -1 );
                if( teamName.empty() )
                  break;
                p.setTeamName( teamName );
                break;
              }

            //}:                                  |
            //Installer:{                         |

              case"m_installScript"_64:
                p.setInstallScript( lua_getCleansedID( L, -1 ));
                break;

            //}:                                  |
            //LinkWith:{                          |

              case"m_linkWith"_64:/**/{

                //--------------------------------------------------------------
                // Name the libraries to link with via a single string.
                //--------------------------------------------------------------

                if( lua_isstring( L, -1 )){
                  p.toLinkWith() << lua_getCleansedID( L, -1 );
                  p.toLinkWith() << ",";
                  break;
                }

                //--------------------------------------------------------------
                // Name the libraries to link with via a single table. We will
                // pull the table apart, and build the m_sLinkWith string from
                // it.
                //--------------------------------------------------------------

                if( lua_istable( L, -1 )){
                  lua_pushnil( L );//+1
                  while( lua_next( L, -2 )){//+1
                    if( lua_istable( L, -1 )){
                      lua_getfield( L, -1, "label" );
                        p.toLinkWith() << lua_getCleansedID( L, -1 );
                        p.toLinkWith() << ",";
                      lua_pop( L, 1 );
                    }else if( lua_isstring( L, -1 )){
                      p.toLinkWith() << lua_getCleansedID( L, -1 );
                      p.toLinkWith() << ",";
                    }
                    lua_pop( L, 1 );
                  }
                  p.toLinkWith().replace( ",,", "," );
                  p.toLinkWith().erase( " " );
                  break;
                }
              }

            //}:                                  |
            //Language:{                          |

              case"m_clanguage"_64:
                p.setLanguageC( lua_tostring( L, -1 ));
                break;

              case"m_language"_64:
                p.setLanguage( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Include:{                           |

            case"m_includePaths"_64:
              p.setIncludePaths( lua_getCleansedID( L, -1 ));
              break;

            //}:                                  |
            //Defines:{                           |

              case"m_definesDbg"_64:
                if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                  p.setDefinesDbg( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
                }else{
                  p.setDefinesDbg( lua_tostring( L, -1 ));
                }
                break;

              case"m_definesRel"_64:
                if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                  p.setDefinesRel( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
                }else{
                  p.setDefinesRel( lua_tostring( L, -1 ));
                }
                break;

            //}:                                  |
            //Ignore:{                            |

              case"m_ignore"_64:
                p.setIgnoreParts( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Bundle:{                            |

              case"m_bundleId"_64:
                p.setProductBundleId( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Prefix:{                            |

              case"m_prefixHeader"_64:
                p.setPrefixHeader( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Export:{                            |

              case"m_exportHeaders"_64:/**/{
                const auto& strlist = lua_getCleansedID( L, -1 );
                const auto& headers = strlist.splitAtCommas();
                headers.foreach(
                  [&]( const auto& header ){
                    if( header.empty() )
                      return;
                    Workspace::File f( header );
                    f.setWhere( header.path() );
                    f.setPublic( true );
                    p.toPublicHeaders()
                      . push( f
                    );
                  }
                );
                break;
              }

              case"m_exportRefs"_64:/**/{
                const auto& str = lua_getCleansedID( L, -1 );
                const auto& ref = str.splitAtCommas();
                ref.foreach(
                  [&]( const string& r ){
                    if( r.empty() ){
                      return;
                    }
                    Workspace::File f( r );
                    p.toPublicRefs()
                      . push( f )
                    ;
                  }
                );
                break;
              }

            //}:                                  |
            //Deploy:{                            |

              case"m_deployTo"_64:
                p.setDeployment( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Build:{                             |

              case"m_build"_64:
                p.setBuild( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Unity:{                             |

              case"m_skipUnity"_64:
                p.setSkipUnity( lua_getCleansedID( L, -1 ));
                break;

              case"m_bUnity"_64:/**/{
                const string& boolean = lua_tostring( L, -1 );
                switch( boolean.tolower().hash() ){
                  case"false"_64:
                  case"no"_64:
                    p.setUnityBuild( false );
                    break;
                  case"true"_64:
                  case"yes"_64:
                    p.setUnityBuild( true );
                    break;
                }
                break;
              }

            //}:                                  |
            //Paths:{                             |

              case"m_resPaths"_64:
                p.setResPath( lua_getCleansedID( L, -1 ));
                break;

              case"m_srcPaths"_64:
                p.setSrcPath( lua_getCleansedID( L, -1 ));
                break;

              case"m_frameworkPaths"_64:
                p.setFrameworkPaths( lua_getCleansedID( L, -1 ));
                break;

              case"m_pluginPaths"_64:/**/{ Workspace::Files files;
                string plugins( lua_getCleansedID( L, -1 ));
                strings paths( plugins.splitAtCommas() );
                paths.foreach(
                  [&]( const auto& plugin ){
                    e_msgf( "Found plugin: %s", ccp( plugin ));
                    Workspace::File f( plugin );
                    f.setPluginRef( plugin );
                    f.toFlags()->bPlugin = 1;
                    f.toFlags()->bSign   = 1;
                    p.toPluginFiles().push( f );
                  }
                );
                break;
              }

              case"m_libraryPaths"_64:
                p.setFindLibsPaths( lua_getCleansedID( L, -1 ));
                break;

            //}:                                  |
            //ARC:{                               |
              //disable:{                         |

                case"m_disableOpts"_64:/**/{
                  string s = lua_tostring( L, -1 );
                  s.erase( "\n" );
                  const auto& arc = s.tolower();
                  if( arc.hash() == "arc"_64 ){
                    p.setDisableOptions( arc );
                    p.toFlags()->bEnableARC=0;
                  }else{
                    p.setDisableOptions( s );
                  }
                  break;
                }

              //}:                                |
              //enable:{                          |

                case"m_arcEnabled"_64:/**/{
                  const string& boolean = lua_tostring( L, -1 );
                  switch( boolean.tolower().hash() ){
                    case"false"_64:
                    case"no"_64:
                    case"0"_64:
                      p.toFlags()->bEnableARC = 0;
                      break;
                    case"true"_64:
                    case"yes"_64:
                    case"1"_64:
                      p.toFlags()->bEnableARC = 1;
                      break;
                  }
                  break;
                }

              //}:                                |
            //}:                                  |
            //Org:{                               |

              case"m_orgName"_64:
                p.setOrgName( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //Os:{                                |

              case"m_osTarget"_64:
                p.setTargetOS( lua_tostring( L, -1 ));
                break;

            //}:                                  |
            //------------------------------------+-----------------------------
          }
          lua_pop( L, 1 );
        }
      }

      //------------------------------------------------------------------------
      // NDK gathering function.
      //------------------------------------------------------------------------

      void lua_gather( lua_State* L, Workspace::NDK& p ){
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string& key = lua_tostring( L, -2 );
          switch( key.hash() ){
            case"m_build"_64:
              p.setBuild( lua_tostring( L, -1 ));
              break;
            case"m_linkWith"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setLinkWith( s );
              break;
            }
            case"m_prefixHeader"_64:
              p.setPrefixHeader( lua_tostring( L, -1 ));
              break;
            case"m_ignore"_64:
              p.setIgnoreParts( lua_tostring( L, -1 ));
              break;
            case"m_clanguage"_64:
              p.setLanguageC( lua_tostring( L, -1 ));
              break;
            case"m_language"_64:
              p.setLanguage( lua_tostring( L, -1 ));
              break;
            case"m_disableOpts"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              break;
            }
            case"m_skipUnity"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setSkipUnity( s );
              break;
            }
            case"m_exportHeaders"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              const auto& headers = s.splitAtCommas();
              headers.foreach(
                [&]( const string& header ){
                  if( header.empty() ){
                    return;
                  }
                  Workspace::File f( header );
                  f.setPublic( true );
                  p.toPublicHeaders().push( f );
                }
              );
              break;
            }
            case"m_includePaths"_64:/**/{
              string s( lua_tostring( L, -1 ));
              s.erase( "\n" );
              p.setIncludePaths(
                s );
              break;
            }
            case"m_definesDbg"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesDbg( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesDbg( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "DBG_DEFINES: %s", ccp( p.toDefinesDbg() ));
              #endif
              break;
            case"m_definesRel"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesRel( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesRel( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "REL_DEFINES: %s", ccp( p.toDefinesRel() ));
              #endif
              break;
            case"m_srcPaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setSrcPath( s );
              break;
            }
            case"m_libraryPaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setFindLibsPaths( s );
              break;
            }
          }
          lua_pop( L, 1 );
        }
      }

      //------------------------------------------------------------------------
      // NINJA gathering function.
      //------------------------------------------------------------------------

      void lua_gather( lua_State* L, Workspace::Ninja& p ){
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string& key = lua_tostring( L, -2 );
          switch( key.hash() ){
            case"m_build"_64:
              p.setBuild( lua_tostring( L, -1 ));
              break;
            case"m_installScript"_64:
              p.setInstallScript( lua_tostring( L, -1 ));
              break;
            case"m_linkWith"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setLinkWith( s );
              break;
            }
            case"m_prefixHeader"_64:
              p.setPrefixHeader( lua_tostring( L, -1 ));
              break;
            case"m_ignore"_64:
              p.setIgnoreParts( lua_tostring( L, -1 ));
              break;
            case"m_clanguage"_64:
              p.setLanguageC( lua_tostring( L, -1 ));
              break;
            case"m_language"_64:
              p.setLanguage( lua_tostring( L, -1 ));
              break;
            case"m_disableOpts"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              break;
            }
            case"m_skipUnity"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setSkipUnity( s );
              break;
            }
            case"m_exportHeaders"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              const auto& headers = s.splitAtCommas();
              headers.foreach(
                [&]( const string& header ){
                  if( header.empty() ){
                    return;
                  }
                  Workspace::File f( header );
                  f.setPublic( true );
                  p.toPublicHeaders().push( f );
                }
              );
              break;
            }
            case"m_includePaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setIncludePaths( s );
              break;
            }
            case"m_definesDbg"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesDbg( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesDbg( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "DBG_DEFINES: %s", ccp( p.toDefinesDbg() ));
              #endif
              break;
            case"m_definesRel"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesRel( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesRel( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "REL_DEFINES: %s", ccp( p.toDefinesRel() ));
              #endif
              break;
            case"m_srcPaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setSrcPath( s );
              break;
            }
            case"m_libraryPaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setFindLibsPaths( s );
              break;
            }
          }
          lua_pop( L, 1 );
        }
      }

      //------------------------------------------------------------------------
      // MSVC gathering function.
      //------------------------------------------------------------------------

      void lua_gather( lua_State* L, Workspace::MSVC& p ){
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string& key = lua_tostring( L, -2 );
          switch( key.hash() ){
            case"m_ignore"_64:
              p.setIgnoreParts( lua_tostring( L, -1 ));
              break;
            case"m_clanguage"_64:
              p.setLanguageC( lua_tostring( L, -1 ));
              break;

            case"m_language"_64:
              p.setLanguage( lua_tostring( L, -1 ));
              break;
            case"m_prefixHeader"_64:
              p.setPrefixHeader( lua_tostring( L, -1 ));
              break;
            case"m_skipUnity"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setSkipUnity( s );
              break;
            }
            case"m_includePaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setIncludePaths( s );
              break;
            }
            case"m_libraryPaths"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setFindLibsPaths( s );
              break;
            }
            case"m_definesDbg"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesDbg( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesDbg( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "DBG_DEFINES: %s", ccp( p.toDefinesDbg() ));
              #endif
              break;
            case"m_definesRel"_64:
              if( p.isUnityBuild() && Workspace::bmp->bUnity ){
                p.setDefinesRel( "__compiling_unity__=1," + string( lua_tostring( L, -1 )));
              }else{
                p.setDefinesRel( lua_tostring( L, -1 ));
              }
              #if e_compiling( debug )
                e_msgf( "REL_DEFINES: %s", ccp( p.toDefinesRel() ));
              #endif
              break;
            case"m_def"_64:
              p.setDefinition( lua_tostring( L, -1 ));
              break;
            case"m_toolchain"_64:
              p.setPlatformTools( lua_tostring( L, -1 ));
              break;
            case"m_dependencies"_64:
              p.setDependencies( lua_tostring( L, -1 ));
              break;
            case"m_winsdk"_64:
              p.setWindowsSDK( lua_tostring( L, -1 ));
              break;
            case"m_build"_64:
              p.setBuild( lua_tostring( L, -1 ));
              break;
            case"m_resPaths"_64:
              p.setResPath( lua_tostring( L, -1 ));
              break;
            case"m_srcPaths"_64:
              p.setSrcPath( lua_tostring( L, -1 ));
              break;
            case"m_linkWith"_64:/**/{
              string s = lua_tostring( L, -1 );
              s.erase( "\n" );
              p.setLinkWith( s );
              break;
            }
          }
          lua_pop( L, 1 );
        }
      }

      //------------------------------------------------------------------------
      // Gathering & add files function.
      //------------------------------------------------------------------------

      template<typename T> void lua_gatherAddFiles( lua_State* L
          , Workspace::Targets& v
          , typename Generator<T>::handle& hGenerator
          , typename T::handle& hProject ){
        const string& key = lua_tostring( L, -2 );
        auto& p = hProject.cast();
        p.setLabel( key );
        lua_gather( L, p );
        v.push( hProject.template as<Workspace::Target>() );
        p.setGenerator( hGenerator.template as<Object>() );
        hGenerator->addFiles();
        p.setGenerator( 0 );
      }

      //------------------------------------------------------------------------
      // Gathering function.
      //------------------------------------------------------------------------

      void lua_gather( lua_State* L, Workspace::Targets& targets ){
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string next( lua_tostring( L, -2 ));
          if( !next.empty() ){

            //------------------------------------------------------------------
            // Gradle workspaces; sets up the targets for building build files.
            //------------------------------------------------------------------

            if( Workspace::bmp->bNDK ){
              auto hNDK = e_new<Workspace::NDK>();
              auto hGen = e_new<Generator<Workspace::NDK>>(
                reinterpret_cast<Workspace::NDK*>( hNDK.pcast() ));
              lua_gatherAddFiles<Workspace::NDK>( L
                , targets
                , hGen
                , hNDK
              );
            }

            //------------------------------------------------------------------
            // Ninja workspaces; connectitive tissue to making targets.
            //------------------------------------------------------------------

            if( Workspace::bmp->bNinja ){
              auto hNinja = e_new<Workspace::Ninja>();
              auto hGenerator = e_new<Generator<Workspace::Ninja>>(
                reinterpret_cast<Workspace::Ninja*>( hNinja.pcast() ));
              lua_gatherAddFiles<Workspace::Ninja>( L
                , targets
                , hGenerator
                , hNinja
              );
            }

            //------------------------------------------------------------------
            // Xcode workspaces; connectitive tissue to making targets.
            //------------------------------------------------------------------

            if( Workspace::bmp->bXcode11 ||
                Workspace::bmp->bXcode12 ||
                Workspace::bmp->bXcode15 ||
                Workspace::bmp->bXcode16 ){
              auto hXcode = e_new<Workspace::Xcode>();
              auto hGenerator = e_new<Generator<Workspace::Xcode>>(
                reinterpret_cast<Workspace::Xcode*>( hXcode.pcast() ));
              lua_gatherAddFiles<Workspace::Xcode>( L
                , targets
                , hGenerator
                , hXcode
              );
            }

            //------------------------------------------------------------------
            // Visual Studio solutions; connectitive tissue to making targets.
            //------------------------------------------------------------------

            if( Workspace::bmp->bVS2022 ){
              auto hMSVC = e_new<Workspace::MSVC>();
              if( Workspace::bmp->bVSTools143 ){
                hMSVC->setPlatformTools( "v143" );
              }
              auto hGenerator = e_new<Generator<Workspace::MSVC>>(
                reinterpret_cast<Workspace::MSVC*>( hMSVC.pcast() ));
              lua_gatherAddFiles<Workspace::MSVC>( L
                , targets
                , hGenerator
                , hMSVC
              );
            }else if( Workspace::bmp->bVS2019 ){
              auto hMSVC = e_new<Workspace::MSVC>();
              auto hGenerator = e_new<Generator<Workspace::MSVC>>(
                reinterpret_cast<Workspace::MSVC*>( hMSVC.pcast() ));
              lua_gatherAddFiles<Workspace::MSVC>( L
                , targets
                , hGenerator
                , hMSVC
              );
            }
          }
          lua_pop( L, 1 );
        }
      }

      void lua_gather( lua_State* L, Workspace& w ){
        // https://pgl.yoyo.org/luai/i/lua_next
        lua_pushnil( L );
        while( lua_next( L, -2 )){
          const string key( lua_tostring( L, -2 ));
          switch( key.hash() ){
            case"m_tProjects"_64:
              lua_pushvalue( L, -1 );
              lua_gather( L
                , w.toTargets() );
              lua_pop( L, 1 );
              break;
            case"m_sName"_64:
              w.setName( lua_tostring( L, -1 ));
              break;
          }
          // Removes 'value'; keeps 'key' for next iteration.
          lua_pop( L, 1 );
        }
      }
    }

#ifdef __APPLE__
  #pragma mark - Action -
#endif

    s32 onGenerate( lua_State* L ){
      auto hWorkspace = e_new<Workspace>();
      auto& workspace = hWorkspace.cast();
      lua_pushvalue( L, -1 );//+1
        if( !lua_istable( L, -1 ))
          e_break( "[issue] Must pass in a table" );
        lua_gather( L, workspace );
      lua_pop( L, 1 );//-1
      lua_getfield( L, -1, "__class" );//+1
        if( !lua_isstring( L, -1 )){
          lua_pop( L, 1 );//-1
          lua_pushnil( L );//+1
          return 1;
        }
        const string& classid = lua_tostring( L, -1 );
      lua_pop( L, 1 );//-1
      if( classid.hash() != "workspace"_64 ){
        lua_pushnil( L );//+1
        return 1;
      }
      lua_pushinteger( L, workspace.UUID );//+1
      workspace.addref();
      return 1;
    }

  //}:                                            |
//}:                                              |
//Actions:{                                       |
  //onSave:{                                      |

    s32 onSave( lua_State* L ){
      auto path = Workspace::out;
      if( path.empty() )
          path = "tmp";
      auto bResult = false;

      //------------------------------------------------------------------------
      // Bail conditions.
      //------------------------------------------------------------------------

      const s64 UUID = lua_tointeger( L, -1 );
      if( !Class::Factory::valid( UUID )){
        lua_pushboolean( L, false );
        return 1;
      }
      Object::handle hObject = UUID;
      if( !hObject.isa<Workspace>() ){
        lua_pushboolean( L, false );
        return 1;
      }
      auto& wsp = hObject.as<Workspace>().cast();
      if( wsp.toName().empty() ){
        e_msg( "No name in workspace object" );
        lua_pushboolean( L, false );
        return 1;
      }

      //------------------------------------------------------------------------
      // Generate the workspace bundle for Xcode11.
      //------------------------------------------------------------------------

      if( Workspace::bmp->bXcode11 ||
          Workspace::bmp->bXcode12 ||
          Workspace::bmp->bXcode15 ||
          Workspace::bmp->bXcode16 ){
        const auto& xcworkspace = path
          + "/"
          + wsp.toName()
          + ".xcworkspace";
        e_rm( xcworkspace );
        e_md( xcworkspace );
        Writer fs( xcworkspace
          + "/contents.xcworkspacedata"
          , kTEXT );
        wsp.serialize( fs );
        bResult = true;
        fs.save();
      }

      //------------------------------------------------------------------------
      // Generate the solution XML for Visual Studio 2019.
      //------------------------------------------------------------------------

      if( Workspace::bmp->bVS2019 ||
          Workspace::bmp->bVS2022 ){
        const auto& sln = path
          + "/"
          + wsp.toName()
          + ".sln";
        e_msgf( "Generating %s"
          , ccp( sln ));
        Writer fs( sln, kTEXT );
        wsp.serialize( fs );
        bResult = true;
        fs.save();
      }

      //------------------------------------------------------------------------
      // Generate the build.gradle for Android NDK.
      //------------------------------------------------------------------------

      if( Workspace::bmp->bGradle &&
          Workspace::bmp->bNDK ){
        const auto& build = path
          + "/settings.gradle";
        e_msgf( "Generating %s"
              , ccp( build ));
        Writer fs( build, kTEXT );
        wsp.serialize( fs );
        bResult = true;
        fs.save();
      }

      //------------------------------------------------------------------------
      // Generate the Ninja.build for Linux primarily.
      //------------------------------------------------------------------------

      if( Workspace::bmp->bNinja ){
        const auto build = path + "/build.ninja";
        e_msgf( "Generating %s"
          , ccp( build ));
        Writer fs( build, kTEXT );
        wsp.serialize( fs );
        bResult = true;
        fs.save();
      }

      //------------------------------------------------------------------------
      // Return result boolean to Lua.
      //------------------------------------------------------------------------

      lua_pushboolean( L, bResult );
      wsp.cleanup();
      return 1;
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
