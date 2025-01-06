//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#include"generators.h"
#include"luacore.h"
#include<signal.h>
#include<csetjmp>
#include"std.h"

using namespace EON;
using namespace gfc;

extern s32 onGenerate( lua_State* L );
extern s32 onSave( lua_State* L );

//================================================+=============================
//Lua:{                                           |
  //Defines:{                                     |

    #define var auto

  //}:                                            |
  //Extends:{                                     |

#ifdef __APPLE__
  #pragma mark - Lua -
#endif

    e_extends( Lua );

  //}:                                            |
  //Statics:{                                     |

    hashmap<u64,string> Lua::D;

  //}:                                            |
  //Methods:{                                     |
    //Memory management:{                         |

#ifdef __APPLE__
  #pragma mark - Memory management -
#endif

      vp Lua::allocate( vp/* ud */, vp ptr, size_t/* osize */, size_t nsize ){
        if( nsize ){
          return e_realloc( ptr, u32( nsize ));
        }
        e_free( ptr );
        return 0;
      }

    //}:                                          |
    //Standard:{                                  |
      //collectgarbage:{                          |

#ifdef __APPLE__
  #pragma mark - Standard library -
#endif

        int Lua::collectgarbage( lua_State* L ){
          static const char *const opts[]={
            "stop",
            "restart",
            "collect",
            "count",
            "step",
            "setpause",
            "setstepmul",
            "isrunning",
            NULL
          };
          static const int optsnum[]={
            LUA_GCSTOP,
            LUA_GCRESTART,
            LUA_GCCOLLECT,
            LUA_GCCOUNT,
            LUA_GCSTEP,
            LUA_GCSETPAUSE,
            LUA_GCSETSTEPMUL,
            LUA_GCISRUNNING
          };
          const int o = optsnum[ luaL_checkoption( L, 1, "collect", opts )];
          const int ex = int( luaL_optinteger( L, 2, 0 ));
          const int res = lua_gc( L, o, ex );
          switch( o ){
            case LUA_GCCOUNT:/**/{
              int b = lua_gc( L, LUA_GCCOUNTB, 0 );
              lua_pushnumber( L, (lua_Number)res + ((lua_Number)b/1024) );
              return 1;
            }
            case LUA_GCSTEP:
            case LUA_GCISRUNNING:/**/{
              lua_pushboolean( L, res );
              return 1;
            }
            default:/**/{
              lua_pushinteger( L, res );
              return 1;
            }
          }
        }

      //}:                                        |
      //setmetatable:{                            |

        int Lua::setmetatable( lua_State* L ){
          int t = lua_type( L, 2 );
          luaL_checktype( L, 1, LUA_TTABLE );
          luaL_argcheck( L, t == LUA_TNIL || t == LUA_TTABLE, 2, "nil or table expected" );
          if (luaL_getmetafield( L, 1, "__metatable") != LUA_TNIL )
            return luaL_error( L, "cannot change a protected metatable" );
          lua_settop( L, 2 );
          lua_setmetatable( L, 1 );
          return 1;
        }

      //}:                                        |
      //getmetatable:{                            |

        int Lua::getmetatable( lua_State* L ){
          luaL_checkany(L, 1);
          if( !lua_getmetatable( L, 1 )){
            lua_pushnil( L );
            return 1;
          }
          luaL_getmetafield( L, 1, "__metatable" );
          return 1;
        }

      //}:                                        |
      //loadstring:{                              |

        int Lua::loadstring( lua_State* L ){
          ccp spec = lua_tostring( L, lua_gettop( L ));
          if( spec ){
            sandbox( L, spec );
          }
          return 0;
        }

      //}:                                        |
      //tostring:{                                |

        int Lua::tostring( lua_State* L ){
          switch( lua_type( L, -1 )){
            case LUA_TSTRING:
              lua_pushstring( L, lua_tostring( L, -1 ));
              return 1;
            case LUA_TNIL:
              lua_pushstring( L, "nil" );
              return 1;
            case LUA_TBOOLEAN:
              lua_pushstring( L, e_xfs( "%s", lua_toboolean( L, -1 ) ? "true" : "false" ));
              return 1;
            case LUA_TLIGHTUSERDATA:
              lua_pushstring( L, "<lightuserdata>" );
              return 1;
            case LUA_TNUMBER:
              lua_pushstring( L, e_xfs( "%f", lua_tonumber( L, -1 )));
              return 1;
            case LUA_TTABLE:
              lua_pushstring( L, "<table>" );
              return 1;
            case LUA_TFUNCTION:
              lua_pushstring( L, "<function>" );
              return 1;
            case LUA_TUSERDATA:
              lua_pushstring( L, "<userdata>" );
              return 1;
            case LUA_TTHREAD:
              lua_pushstring( L, "<thread>" );
              return 1;
          }
          return 0;
        }

      //}:                                        |
      //require:{                                 |

        void Lua::require( ccp filename ){
          if( !e_fexists( filename )){
            e_break( e_xfs( "file not found: %s", filename ));
            return;
          }
          e_msgf( "Requiring \"%s\"", ccp( filename ));
          lua_pushstring( L, filename );
             require( L );
          lua_pop( L, 1 );
        }

        int Lua::require( lua_State* L ){
          ccp spec = lua_tostring( L, -1 );
          if( spec ){
            string filename;
            filename << spec;
            if( filename.ext().tolower().hash() != ".lua"_64 )
                filename << ".lua";
            #if 0 // This is just for reference on how to do it.
              switch( luaL_loadfile( L, filename )){
                case LUA_ERRSYNTAX:
                  e_logf( "  Syntax error in %s", filename.c_str() );
                  break;
                case LUA_ERRFILE:
                  e_logf( "  Couldn't find file in %s", filename.c_str() );
                  break;
                case LUA_ERRMEM:
                  e_logf( "  OOM error in %s", filename.c_str() );
                  break;
                case LUA_OK:
                  lua_getglobal( L, "__sandbox" );
                  lua_setupvalue( L, -2, 1 );
                  lua_call( L, 0, 0 );
                  break;
              }
            #else // This is better.
              const auto& f = e_fload( filename );
              if( !f.empty() ){
                f.query(
                  [=]( ccp pFile ){
                    sandbox( L, pFile );
                  }
                );
              }
            #endif
          }
          return 0;
        }

      //}:                                        |
      //select:{                                  |

        int Lua::select( lua_State* L ){
          int n = lua_gettop( L );
          if( lua_type( L, 1 ) == LUA_TSTRING && *lua_tostring( L, 1 ) == '#' ){
            lua_pushinteger( L, n-1 );
            return 1;
          }
          lua_Integer i = luaL_checkinteger( L, 1 );
          if( i < 0 ){
            i = n + i;
          }else if (i > n){
            i = n;
          }
          luaL_argcheck( L, 1 <= i, 1, "index out of range" );
          return n - (int)i;
        }

      //}:                                        |
      //pairs:{                                   |

        namespace{
          int pairsmeta( lua_State* L, const char* method, int iszero, lua_CFunction iter ){
            if( luaL_getmetafield( L, 1, method ) == LUA_TNIL ){
              luaL_checktype( L, 1, LUA_TTABLE );
              lua_pushcfunction( L, iter );
              lua_pushvalue( L, 1 );
              if( iszero ){
                lua_pushinteger( L, 0 );
              }else{
                lua_pushnil( L );
              }
            }else{
              lua_pushvalue( L, 1 );
              lua_call( L, 1, 3 );
            }
            return 3;
          }
        }

        int Lua::pairs( lua_State* L ){
          return pairsmeta( L, "__pairs", 0, next );
        }

      //}:                                        |
      //error:{                                   |

        int Lua::error( lua_State* L ){
          e_break( lua_tostring( L, lua_gettop( L )));
          return 0;
        }

      //}:                                        |
      //print:{                                   |

        int Lua::print( lua_State* L ){
          if( !lua_istable( L, -1 )){
            #if e_compiling( android )
              e_log( lua_tostring( L, -1 ));
            #else
              e_msgf( "%s", lua_tostring( L, -1 ));
            #endif
          }else{
            //todo: dump table to log
          }
          return 0;
        }

      //}:                                        |
      //type:{                                    |

        int Lua::type( lua_State* L ){
          ccp name = luaL_typename( L, -1 );
          lua_pushstring( L, name );
          return 1;
        }

      //}:                                        |
      //next:{                                    |

        int Lua::next( lua_State* L ){
          luaL_checktype( L, 1, LUA_TTABLE );
          lua_settop( L, 2 );  /* create a 2nd argument if there isn't one */
          if( lua_next( L, 1 )){
            return 2;
          }
          lua_pushnil( L );
          return 1;
        }

      //}:                                        |
    //}:                                          |
    //Registry:{                                  |
      //set*:{                                    |

        void Lua::set( ccp key, const string& value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushstring( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, ccp value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushstring( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, s64 value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushinteger( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, f32 value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushnumber( L, f32::cast( value ));
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

      //}:                                        |
      //to*:{                                     |

        s64 Lua::toInteger( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          s64 value = lua_tointeger( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        string Lua::toString( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          string value = lua_tostring( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        ccp Lua::toCstring( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          ccp value = lua_tostring( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        f32 Lua::toFloat( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          f32 value = lua_tonumber( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

      //}:                                        |
    //}:                                          |
    //Session:{                                   |
      //initialise:{                              |

#ifdef __APPLE__
  #pragma mark - Session -
#endif
        void Lua::initialise(){

          //--------------------------------------------------------------------
          // Create new Lua state.
          //--------------------------------------------------------------------

          if( L )
            e_break( "Don't call initialise twice." );
          L = lua_newstate( allocate, 0 );
          if( !L )
            e_break( "Out of memory!" );

          //--------------------------------------------------------------------
          // Create new Lua state.
          //--------------------------------------------------------------------

          luaL_Reg regSandbox[]={
            /* standard lua keywords */
            {"collectgarbage", collectgarbage},
            {"setmetatable", setmetatable},
            {"getmetatable", getmetatable},
            {"loadstring", loadstring},
            {"tostring", tostring},
            {"require", require},
            {"select", select},
            {"error", error},
            {"print", print},
            {"type", type},
            {"next", next},
            {"pairs", pairs},
            /* engine extensions */
            {"generate", onGenerate},
            {"save", onSave},
            /* terminate */
            {0,0}
          };
          lua_createtable( L, e_dimof( regSandbox ), 0 );
          luaL_newlib( L, regSandbox );
          lua_pushvalue( L, -1 );
          lua_setglobal( L, "__sandbox" );
          //cannot store UUID because it hasn't been set yet.
          lua_pushlightuserdata( L, this );
          lua_setfield( L, -2, "this" );
          lua_pop( L, 1 );
          static constexpr ccp startupSequence=/*
            class'name'{
               name=function(self)
               end,
            }*/
            // class'name'{}
            "class=function(T)\n"
            "  if type(T)=='string'then\n"
            "    local mt={\n"
            "      __call=function(self,t,...)\n"
            "        local tt=type(t)\n"
            "        if tt=='table'then\n"
            "          for k,v in pairs(t)do\n"
            "            self[k]=v\n"
            "          end\n"
            "        elseif tt=='string'then\n"
            "          return new(self.__class,t,...)\n"
            "        end\n"
            "        return self\n"
            "      end,\n"
            "    }\n"
            "    _ENV[T]=setmetatable({},mt)\n"
            "    _ENV[T].__class=T\n"
            "    return _ENV[T]\n"
            "  end\n"
            "end\n"
            // obj=new(T,...)
            "new=function(T,...)\n"
            "  local copyMethods=function(obj,t)\n"
            "    for k,v in pairs(t)do\n"
            "      if type(v)=='function'then\n"
            "        obj[k]=v\n"
            "      end\n"
            "    end\n"
            "    return obj\n"
            "  end\n"
            "  local obj={}\n"
            "  if type(T)=='string'then\n"
            "    copyMethods(obj,_ENV[T])\n"
            "    if type(obj[T])=='function'then\n"
            "      obj[T](obj,...)\n"
            "      obj[T]=nil\n"
            "    end\n"
            "  elseif type(T)=='table'then\n"
            "    copyMethods(obj,T)\n"
            "    if type(obj[T.__class])=='function'then\n"
            "      obj[T.__class](obj,...)\n"
            "      obj[T.__class]=nil\n"
            "    end\n"
            "  end\n"
            "  return obj\n"
            "end\n"
            // Unit tests.
            #if 0 // 1: Perform class tests!
              "print'Creating test class'\n"
              "class'test1'{\n"
              "  test1=function(self,name)\n"
              "    print('hello '..name)\n"
              "  end\n"
              "}\n"
              "class'test2'\n"
              "function test2:test2(name)\n"
              "  print('bye '..name)\n"
              "end\n"
              "class'test3'.test3=function(self,name)\n"
              "  print('silly '..name)\n"
              "end\n"
              "obj = new('test1','bob')\n"
              "obj = new(test2,'dad')\n"
              "obj = test3'billy'\n"
              "test1 = nil\n"
              "test2 = nil\n"
              "test3 = nil\n"
              "obj = nil\n"
            #endif
          ; sandbox( startupSequence );
          classify( "gfc"
            , standard
          );
        }

      //}:                                        |
      //destroy:{                                 |

        void Lua::destroy(){
          if( L ){
            lua_close( L );
            L = 0;
          }
        }

      //}:                                        |
      //uuid:{                                    |

        s64 Lua::uuid( lua_State* L ){
          lua_getglobal( L, "UUID" );
          s64 UUID = lua_tointeger( L, -1 );
          lua_pop( L, 1 );
          return UUID;
        }

      //}:                                        |
      //exit:{                                    |

        void Lua::exit(){
          int exit = findInSandbox( "exit" );
          if( exit ){
            call( exit );
          }
        }

      //}:                                        |
      //main:{                                    |

        void Lua::main(){
          int main = findInSandbox( "main" );
          if( main ){
            call( main );
          }
        }

      //}:                                        |
    //}:                                          |
    //API:{                                       |
      //getFieldAlways:{                          |

#ifdef __APPLE__
  #pragma mark - API -
#endif

        bool Lua::getFieldAlways( lua_State* L, ccp pField ){
          if( !lua_istable( L, -1 )){
            return false;
          }
          lua_getfield( L, -1, pField );
          if( lua_isnil( L, -1 )){
            lua_pop( L, 1 );
            lua_createtable( L, 0, 0 );
            lua_pushvalue( L, -1 );
            lua_setfield( L, -3, pField );
          }
          return true;
        }

      //}:                                        |
      //classify:{                                |

        void Lua::classify( ccp clsid, const luaL_Reg* methods ){

          //produce a new class, e.g., class'entity'
          int cls = findInSandbox( "class" );
          lua_rawgeti( L, LUA_REGISTRYINDEX, cls );
          lua_pushstring( L, clsid );
          lua_call( L, 1, 0 );
          unref( cls );

          //pass {methods} to return value set in _ENV[T] or
          //__sandbox.entity{methods} using __call.
          cls = findInSandbox( clsid );
          lua_rawgeti( L, LUA_REGISTRYINDEX, cls );
          lua_createtable( L, 0, 0 );
          luaL_setfuncs( L, methods, 0 );
          lua_call( L, 1, 0 );
          unref( cls );
        }

      //}:                                        |
      //sandbox:{                                 |

        bool Lua::sandbox( lua_State* L, ccp pScript ){
          static string script;
          script = pScript;

          //--------------------------------------------------------------------
          // Handle the "if/elif/endif" preprocessor statement.
          //--------------------------------------------------------------------

          static const auto& onConditional=[](
                ccp p
              , ccp s
              , ccp e )->ccp{
            while( true ){
              ccp keyw = string::skip_anynonws( s );
              ccp labl = string::skip_anyws( keyw );
              if( labl ){
                ccp end = string::skip_anynonws( labl );
                if( end ){
                  const auto& label = string( labl, end );
                  if( !label.empty() ){
                    auto expr = string(
                        string::skip_ws( end )
                      , e );
                    if( expr.empty() )
                      e_break( "Missing closing > or \"" );
                    const auto& token = string( s, keyw );
                    switch( token.hash() ){
                      case"include"_64:
                        e_break( "syntax error in #include<>" );
                      case"endif"_64:
                        e_break( "Unexpected #endif" );
                      case"elif"_64:
                        e_break( "Unexpected #elif" );
                      case"else"_64:
                        e_break( "Unexpected #else" );
                      case"if"_64:/**/{
                        const auto key( label );
                        s = string
                          :: skip_anyws( e+1 );
                        auto* E = string
                          :: skip_2eol( s );
                        auto isDef = false;
                        D.find( key
                          . hash() );
                        if( !isDef ){
                          s = string::skip_anyws( E );
                          e = string::skip_2eol( s );
                      sk: const string nextKey( s
                            , e );
                          switch( nextKey.hash() ){
                            case"#"_64:
                              return nullptr;;
                            case"#endif"_64:/**/{
                              const auto t( script );
                              script = string(
                                  script
                                , p );
                              const auto* eof = t.end( );
                              script << string( s, eof );
                              const auto o( p-ccp( t ));
                              p = ccp( script )+o;
                              s = string::skip_2eol( p );
                              s = string::skip_anyws(
                                s );
                              return s;
                            }
                            case"#elif"_64:
                              e_break( "#elif not yet supported" );
                            default:
                              s = string::skip_2eol( s );
                              s = E = string::skip_anyws( s );
                              e = string::skip_2eol( s );
                              goto sk;
                          }
                          break;
                        }
                      }
                    }
                  }
                }
              }
            }
            return s;
          };

          //--------------------------------------------------------------------
          // Define an equate in the vein of C# not C/C++. Use the #macro
          // directive instead for proper macros; like old school MASM.
          //--------------------------------------------------------------------

          static const auto& onDefine=[](
                ccp p
              , ccp s
              , ccp e )->ccp{
            ccp keyw = string::skip_anynonws( s );
            ccp labl = string::skip_anyws( keyw );
            if( labl ){
              ccp end = string::skip_anynonws( labl );
              if( end ){
                const auto& label = string( labl, end );
                if( !label.empty() ){
                  auto value = string(
                      string::skip_ws( end )
                    , e );
                  const auto& token = string( s, keyw );
                  if( token.hash()=="define"_64 ){
                    value.erase( "\"" );
                    value.erase( " " );
                    const auto& p0 = string( script.c_str(), p );
                    auto p1 = string( e );
                    p1.replace(
                        "${" + label + "}"
                      , value );
                    script = p0 + p1;
                    return string::skip_anyws( e );
                  }
                }
              }
            }
            return nullptr;
          };

          //--------------------------------------------------------------------
          // Include other Lua scripts doesn't sandbox but is a textual replace
          // instead.
          //--------------------------------------------------------------------

          static const auto& onInclude=[](
                ccp& p
              , ccp s
              , ccp e )->ccp{
            auto esub = cp( strchr( p, '>' ));
            if( !esub )
              return nullptr;
            string subs;
            ccp psub = nullptr;
            ccp keyw = nullptr;
            subs = string( p, esub+1 );
            keyw = string::skip_anynonws( s );
            psub = string::skip_anyws( keyw );
            string path;
            if( psub ){
              path = string( psub+1, esub );
            }
            const auto& token = string( s, keyw );
            switch( token.hash() ){
              case"include"_64:/**/{
                const auto& st = e_fload( path );
                if( !st.empty() ){
                  const auto& ld = string( st.data() );
                  script.replace( subs, ld );
                }else{
                  e_msgf( "couldn't load %s", ccp( path ));
                  script.erase( p );
                }
                return string::skip_anyws( e );
              }
            }
            return nullptr;
          };

          //--------------------------------------------------------------------
          // Process the incoming script.
          //--------------------------------------------------------------------

          if( !e_getCvar( bool, "DISABLE_EXTS" )){
            string script( pScript );
            if( !script.empty() ){
              ccp s = ccp( script );
              ccp e = script.end( );
              ccp z = e;
              while( s < e ){
                if(( s[ 0 ]=='-' )&&( s[ 1 ]=='-' )){
                  const auto eol = string::skip_2eol( s );
                  if( !eol )
                    break;
                  s = string::skip_anyws( eol );
                }else if( *s != '#' ){
                  ++s;
                }else{
                  ccp _e = string::skip_2eol( s );
                  if( !_e )
                       _e = z;
                  ccp _s = onInclude( s
                    , string::skip_ws( s+1 )
                    , _e );
                  if( _s ){ s = _s; continue; }
                  _s = onDefine( s
                    , string::skip_ws( s+1 )
                    , _e );
                  if( _s ){ s = _s; continue; }
                  s = onConditional( s
                    , string::skip_ws( s+1 )
                    , _e );
                  z = script.end();
                  e = z;
                }
              }
            }
          }

          //--------------------------------------------------------------------
          // Dump the script out to log with line numbers.
          //--------------------------------------------------------------------

          static const auto& dumpScript=[]( const auto& script ){
            const strings lines = script.splitLines();
            auto it = lines.getIterator();
            auto ln = 1u;
            while( it ){
              e_msgf(
                "%5u %s"
                , ln
                , ccp( *it ));
              ++it;
              ++ln;
            }
          };

          //--------------------------------------------------------------------
          // Thi implements the "call" or doCall local function from lua.c
          //--------------------------------------------------------------------

          // Store in global spaces!
          static lua_State* globalL;

          // The SIGABRT handler.
          static const auto& msgHandler=[]( lua_State* L ){
            auto msg = lua_tostring( L, 1 );
            if( !msg ){// is error object not a string?
              if( luaL_callmeta( L, 1, "__tostring" )&&// does it have a metamethod?
                  lua_type( L, -1 ) == LUA_TSTRING )  // that produces a string?
                return 1;// that is the message.
              msg = lua_pushfstring( L
                , "(error object is a %s value)"
                , luaL_typename( L, 1 )
              );
            }
            dumpScript( script );
            e_msg( msg );
            luaL_traceback( L, L, msg, 1 );
            return 1;// Return traceback.
          };

          // Call function on the stack.
          static const auto& call=[]( lua_State* L, int narg, int nres ){

            //------------------------------------------------------------------
            // The setsignal local function.
            //------------------------------------------------------------------

            static const auto& setsignal=[]( int sig, void( *handler )( int )){
              struct sigaction sa;
              sa.sa_handler = handler;
              sa.sa_flags = 0;
              sigemptyset( &sa.sa_mask );  /* do not mask any signal */
              sigaction( sig, &sa, nullptr );
            };

            //------------------------------------------------------------------
            // The lstop local function.
            //------------------------------------------------------------------

            static const auto& lstop=[]( lua_State* L, lua_Debug* ar ){
              lua_sethook( L, nullptr, 0, 0 );  /* reset hook */
              luaL_error( L, "interrupted!" );
              (void)ar;  /* unused arg. */
            };

            //------------------------------------------------------------------
            // The laction local function.
            //------------------------------------------------------------------

            static const auto& laction=[]( int i ){
              const int flag = 0
                | LUA_MASKCALL
                | LUA_MASKRET
                | LUA_MASKLINE
                | LUA_MASKCOUNT;
              setsignal( i, SIG_DFL );// if another SIGINT happens, terminate process.
              lua_sethook( globalL, lstop, flag, 1 );
            };

            //------------------------------------------------------------------
            // Run Lua functions.
            //------------------------------------------------------------------

            const auto base = lua_gettop( L ) - narg;  // function index.
            lua_pushcfunction( L, msgHandler );       // push message handler.
            lua_insert( L, base );                   // put it under function and args.
            globalL = L;                            // to be available to 'laction'.
            setsignal( SIGINT, laction );          // set C-signal handler.
            auto status = lua_pcall( L, narg, nres, base );
            setsignal( SIGINT, SIG_DFL );        // reset C-signal handler.
            lua_remove( L, base );              // remove message handler from the stack.
            return status;
          };

          //--------------------------------------------------------------------
          // Compile up the script we just processed by passing to Lua.
          //--------------------------------------------------------------------

          if( !script.empty() ){
            script.replace( ",,", "," );
            // narg: 1
            var status = luaL_loadstring( L, script/* <-= Lua function */);
            if( status ){
              dumpScript( script );
              e_msgf( "%s\nCouldn't load string!"
                , ccp( script )
              );
            }
            // narg: 2
            lua_getglobal( L, "__sandbox" );//+1=3
            lua_setupvalue( L, -2, 1 );//-1=2
            // narg: 2 (nowt changed).
            lua_pushvalue( L, -2 );
            int err = call( L, 1, 0 );
            lua_pop( L, 0 );
            if( err == LUA_OK ){
              return true;
            }
          }
          e_break( "Failed to sandbox" );
          return false;
        }

        bool Lua::sandbox( ccp s ){
          return sandbox( L, s );
        }

      //}:                                        |
      //unref:{                                   |

        void Lua::unref( int ref )const{
          if( ref ){
            luaL_unref( L, LUA_REGISTRYINDEX, ref );
          }
        }

      //}:                                        |
      //find:{                                    |

        int Lua::find( int ref, ccp name )const{
          int run = 0;
          lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
          if( lua_istable( L, -1 )){
            lua_pushnil( L );
            while( lua_next( L, -2 )){
              lua_pushvalue( L, -2 );
              if( lua_isstring( L, -1 )){
                ccp key=lua_tostring( L, -1 );
                if( !strcmp( key, name )){
                  lua_pushvalue( L, -2 );
                  run = luaL_ref( L, LUA_REGISTRYINDEX );
                  lua_pop( L, 2 );
                  break;
                }
              }
              lua_pop( L, 2 );
            }
          }
          lua_pop( L, 1 );
          return run;
        }

        int Lua::findInTable( int ref, ccp pName )const{
          int run = 0;
          lua_rawgeti( L, LUA_REGISTRYINDEX, ref );//+1
          if( lua_istable( L, -1 )){
            lua_getfield( L, -1, pName );//+1
            if( !lua_isnil( L, -1 )){
              run = luaL_ref( L, LUA_REGISTRYINDEX );//-1
            }else{
              lua_pop( L, 1 );//-1
            }
          }
          lua_pop( L, 1 );//-1
          return run;
        }

        int Lua::findInGlobal( ccp pGlobalTableName, ccp pName )const{
          lua_getglobal( L, pGlobalTableName );//+1
          if( lua_istable( L, -1 )){
            const int table = luaL_ref( L, LUA_REGISTRYINDEX );//-1
            const int value = findInTable( table, pName );
            unref( table );
            return value;
          }
          lua_pop( L, 1 );//-1
          return 0;
        }

        int Lua::findInSandbox( ccp pName )const{
          return findInGlobal( "__sandbox", pName );
        }

      //}:                                        |
      //call:{                                    |

        void Lua::call( const int ref, const int run, const f32& value ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #1
            lua_pushnumber( L, f32::cast( value ));
            lua_call( L, 2, 0 );
          }
        }

        void Lua::call( const int ref, const int run, const s64 value ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #1
            lua_pushinteger( L, value );
            lua_call( L, 2, 0 );
          }
        }

        void Lua::call( const int ref, const int run ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #0
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run, const s64 value ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_pushinteger( L, value );
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run, const f32& value ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_pushnumber( L, f32::cast( value ));
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_call( L, 0, 0 );
          }
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
