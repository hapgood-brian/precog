//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================+=============================
//Prefab:{                                        |
  //Extends:{                                     |

    e_extends( Prefab );

  //}:                                            |
  //Methods:{                                     |
    //serialize:{                                 |

      void Prefab::serialize( Writer& fs )const{
        const auto useTracing = e_getCvar( bool, "USE_PREFAB_TRACING" );
        super::serialize( fs );
        fs.version( PREFAB_VERSION );
        fs.write<u32>( m_vFiles.size() );
        m_vFiles.foreach(
          [&]( const File::handle& F ){
            if( useTracing ){
              e_msgf(
                "  to fablet: \"%s\" at base:size %llu:%llu"
                , ccp( F->toName() )
                , F->toBase()
                , F->toSize()
              );
            }
            fs.write( F->toBase()  );
            fs.write( F->toSize()  );
            fs.write( F->toFlags() );
            fs.pack(  F->toName()  );
          }
        );
        const_cast<self*>( this )->m_uBase = fs.toStream().tell();
        fs.write<u64>( m_uBase );
      }

      s64 Prefab::serialize( Reader& fs ){
        const auto useTracing = e_getCvar( bool, "USE_PREFAB_TRACING" );
        super::serialize( fs );
        fs.version( PREFAB_VERSION );
        const auto n = fs.read<u32>();
        if( useTracing ){
          e_msgf( "PrefabFileCount: %u", n );
        }
        if( n ){
          File F;
          for( u32 i=0; i<n; ++i ){
            fs.read( F.toBase() );
            fs.read( F.toSize() );
            fs.read( F.toFlags() );
            fs.unpack( F.toName() );
            if( useTracing ){
              e_msgf(
                "  from fablet: \"%s\" at base:size %llu:%llu"
                , ccp( F.toName() )
                , F.toBase()
                , F.toSize()
              );
            }
            m_vFiles.push( e_new<File>( F ));
          }
          fs.read( m_uBase );
        }
        return UUID;
      }

    //}:                                          |
    //get:{                                       |

      vector<Prefab::handle> Prefab::get( const string& path ){
        vector<Prefab::handle> fablets;
        IEngine::dir( path,
          [&](  const string& dir
              , const string& name
                , const bool isDirectory ){
            if( name.ext().tolower().hash() != ".fablet"_64 ){
              return true;
            }
            if( name.ext().tolower().hash() != ".index"_64 ){
              return true;
            }
            const auto& path = dir + name;
            Prefab::handle hPrefab = e_load<Prefab>( path );
            if( !hPrefab ){
              if( !e_fexists( path )){
                e_break( e_xfs( "Prefab does not exist: \"%s\""
                  , ccp( path )
                ));
              }else{
                e_break( e_xfs( "Couldn't load \"%s\""
                  , ccp( path )
                ));
              }
              return false;
            }
            auto& fablet = hPrefab.cast();
            fablets.push( hPrefab );
            fablet.setPath( path );
            fablet.toFiles().foreach(
              [&]( File::handle& F ){
                F->toBase() += fablet.toBase();
              }
            );
            return true;
          }
        );
        return fablets;
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Private:{                                       |
  //isTextualStream:{                             |

    namespace{
      bool isTextualStream( const stream& st ){
        ccp e = st.data() + st.bytes();
        ccp p = st.data();
        while( p < e ){
          switch( *p ){
            case 0:
            case'\t':
            case'\r':
            case'\n':
              break;
            default:
              if( *p < ' ' ){
                return false;
              }
              break;
          }
          ++p;
        }
        return true;
      }
    }

  //}:                                            |
  //makeStream:{                                  |

    namespace{
      stream makeStream( const string& filespec ){
        string tmp_spec( filespec );
        tmp_spec.replace( "/", "_" );
        tmp_spec = tmp_spec.camelcase();
        const auto& path = ".intermediate/" + tmp_spec + ".eon";
        const auto& st = e_fload( filespec );
        if( st.empty() ){
          return{};
        }
        u32 flags = 0;
        if( !isTextualStream( st )){
          flags |= kCOMPRESS | kSHA1;
        }else{
          flags |= kTEXT;
        }
        { auto fs = std::make_unique<Writer>( path, flags );
          fs->write( st );
          if( flags & kTEXT ){
            fs->save( nullptr );
          }else{
            fs->save( "File" );
          }
        }
        return e_fload( path );
      }
    }

  //}:                                            |
//}:                                              |
//Structs:{                                       |
  //Data:{                                        |

    struct Data final:Prefab::File{
      e_reflect_no_properties( Data, Prefab::File )
      e_var( stream, st, Stream );
    };

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//API:{                                           |
  //e_loadFromPrefab:{                            |

    std::shared_ptr<stream> e_loadFromPrefab( const Prefab& fablet, Reader& fs, const string& name ){
      std::shared_ptr<stream>st = std::make_shared<stream>();
      fablet.toFiles().foreachs(
        [&]( const Prefab::File::handle& F ){
          if( F->isName( name )){
            fs.toStream().seek( fablet.toBase() + F->toBase() );
            cp pBuffer = st->alloc( F->toSize() );
            fs.read( pBuffer, F->toSize() );
            st->reset();
            return false;
          }
          return true;
        }
      );
      return st;
    }

  //}:                                            |
  //e_unpackage:{                                 |

    bool e_unpackage( const string& in_cPath ){

      //------------------------------------------------------------------------
      // We're going to go pretty low-level now; ignoring e_load<> etc. and just
      // get a stream to use.
      //------------------------------------------------------------------------

      auto r = std::make_unique<Reader>( in_cPath );
      auto& fs = r->load( "Prefab" );
      if( fs.isError() ){
        return false;
      }
      const auto clsid = fs.as<u64>();
      if( !Class::Factory::describe( clsid )){
        return false;
      }
      auto hPrefab = e_new<Prefab>( in_cPath );
      auto& fablet = hPrefab.cast();
      fablet.setName( in_cPath.base() );
      fablet.setPath( in_cPath );
      fablet.preSerialize(  *r );
      fablet.serialize(     *r );
      fablet.postSerialize( *r );
      fablet.setBase( r->toStream().tell() );

      //------------------------------------------------------------------------
      // Run through all fablet filenames and rebuild directory structure.
      //------------------------------------------------------------------------

      fablet.toFiles().foreach(
        [&]( Prefab::File::handle& F ){
          const auto& file = F.cast();
          const auto& spec = file.toName();
          const auto& path = spec.path();
          e_msgf( "Unpacking %s", ccp( spec ));
          if( !path.empty() ){
            e_mkdir( path );
          }
          auto st = e_loadFromPrefab( fablet, fs, spec );
          if( !st->capacity() ){
            return;
          }
          FILE* f = e_fopen( spec, "w" );
          if( !f ){
            return;
          }
          fwrite( st->data(), 1, file.toSize(), f );
          fclose( f );
        }
      );
      return true;
    }

  //}:                                            |
  //e_package:{                                   |

    bool e_package( const strings& filesAndDirs, const string& pkgName ){

      //------------------------------------------------------------------------
      // Define the Prefab file with stream included.
      //------------------------------------------------------------------------

      auto hPrefab = e_new<Prefab>();
      auto& fablet = hPrefab.cast();

      //------------------------------------------------------------------------
      // Collect all the streams.
      //------------------------------------------------------------------------

      auto startingAt = 0;
      filesAndDirs.foreach(
        [&]( const string& path ){
          IEngine::dir( path,
            [&]( const auto& d
               , const auto& f
               , const auto isDirectory ){
              if( isDirectory )
                return true;
              const auto& spec = d + f;
              auto st = makeStream( spec );
              if( st.empty() )
                return true;
              auto hData = e_new<Data>();
              auto& data = hData.cast();
              data.setSize( st.bytes() );//must come before std::move().
              data.toStream() = std::move( st );
              data.setName( spec );
              fablet
                . toFiles()
                . push( hData.as<Prefab::File>() );
              return true;
            }
          );
          fablet.toFiles().foreach(
            [&]( Prefab::File::handle& hFile ){
              auto& f = hFile.cast( );
              f.setBase( startingAt );
              startingAt +=
                f.toSize()
              ;
            }
          );
        }
      );

      //------------------------------------------------------------------------
      // Now we have a list of files, their directories and streams so we can
      // now just write the fablet header and all the files.
      //------------------------------------------------------------------------

      const u32 flags = pkgName.empty()
        ? kSHA1
        | kCOMPRESS
        : kCOMPRESS;
      auto sfn = ( pkgName.empty() ? filesAndDirs[ 0 ].base() : pkgName ) + ".fablet";
      sfn.replace(
          ".fablet.fablet"
        , ".fablet" );
      auto pFs = std::make_shared<Writer>( sfn, flags );
      fablet.setBase( startingAt );
      fablet.preSerialize(  *pFs );
      fablet.serialize(     *pFs );
      fablet.postSerialize( *pFs );
      fablet.toFiles().foreach(
        [&]( Prefab::File::handle& hFile ){
          auto hData = hFile.as<Data>();
          auto& data = hData.cast();
          const auto& st = data.toStream();
          pFs->write( st.data()
            , data.toSize()
          );
        }
      );
      if( !pFs->save( "Prefab" )){
        return false;
      }
      e_rd({ ".intermediate" });
      return true;
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
