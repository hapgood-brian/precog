//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#include<sys/stat.h>
#include<stdarg.h>
#include<stdio.h>
#include<mutex>

#if e_compiling( win64 )
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef WIN32_EXTRA_LEAN
  #define WIN32_EXTRA_LEAN
  #endif
  #ifndef VC_EXTRA_LEAN
  #define VC_EXTRA_LEAN
  #endif
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif
  #include<windows.h>
  #include<combaseapi.h>
#endif

using namespace EON;
using namespace gfc;

//================================================+=============================
//Private:{                                       |

  e_noinline atomic::ShareLockRecursive& lock(){
      static atomic::ShareLockRecursive _lock;
    return _lock;
  }
  namespace{
    constexpr u32 CATV_BUFFER_SIZE = 1048576*10;
    cp catvAlloc(){
      static __thread char __catvBuffer[ CATV_BUFFER_SIZE ];
      return __catvBuffer;
    }
  }
  namespace db{
    using unordered_map = std::unordered_map<u64,std::shared_ptr<stream>>;
    e_noinline unordered_map& unsafe(){
      static auto& m = *( new unordered_map );
      return m;
    }
    e_noinline void read( const std::function<void( const unordered_map& )>& lambda ){
      e_guardr( lock() );
      lambda( unsafe() );
    }
    e_noinline void write( const std::function<void( unordered_map& )>& lambda ){
      e_guardw( lock() );
      lambda( unsafe() );
    }
    e_noinline unordered_map& lock(){
      return unsafe();
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Public:{                                        |
  //e_strsplit:{                                  |

#ifdef __APPLE__
  #pragma mark - Functions -
#endif

    bool e_strsplit( cp& inout_L, const char at, cp& out_R ){
      e_sanity_check( !e_isbad( inout_L ));
      e_sanity_check( !e_isbad( out_R ));
      if( inout_L && at ){
        cp M = strchr( inout_L, at );
        while( M[-1] == ' ' ){
          --M;
        }
        u32 N = u32( M - inout_L );
        ccp L = inout_L;
        inout_L = new char[ N+1 ];
        memcpy( inout_L, L, N );
        inout_L[ N ] = 0;
        out_R = M + 1;
        return true;
      }
      return false;
    }

  //}:                                            |
  //e_pathof:{                                    |

    string e_pathof( const u64 clsid, ccp subattr ){
      Object::handle hObject = e_newt( clsid );
      Object& object = hObject.cast();
      string path = "src/data/";
      if( !subattr ){
        return path + object.pathof();
      }
      path += object.pathof() + subattr;
      if( *path.right( 1 ) != '/' ){
        path += '/';
      }
      return path;
    }

  //}:                                            |
  //e_attrof:{                                    |

    string e_attrof( const u64 clsid, ccp subattr ){
      string attr = e_pathof( clsid, subattr );
      attr.replace( "/", " " );
      attr.ltrim( 9 );
      attr.trim( 1 );
      attr = ( "id " + attr ).camelcase();
      return attr;
    }

  //}:                                            |
  //e_strto:{                                     |

    void e_strto( const string& value, const u64 clsid, vp pData ){

      e_sanity_check( !e_isbad( pData ));

      //------------------------------------------------------------------------
      // Real metric numbers.
      //------------------------------------------------------------------------

      if( clsid == e_classid<ssi_mass<f64::Value>>() ){
        *reinterpret_cast<f64*>( pData ) = value.as<f64>();
      }else if( clsid == e_classid<ssi_mass<f64>>() ){
        *reinterpret_cast<f64*>( pData ) = value.as<f64>();
      }else if( clsid == e_classid<ssi_mass<f32::Value>>() ){
        *reinterpret_cast<f32*>( pData ) = value.as<f32>();
      }else if( clsid == e_classid<ssi_mass<f32>>() ){
        *reinterpret_cast<f32*>( pData ) = value.as<f32>();
      }else if( clsid == e_classid<ssi<f64>>() ){
        *reinterpret_cast<f64*>( pData ) = value.as<f64>();
      }else if( clsid == e_classid<ssi<f32>>() ){
        *reinterpret_cast<f32*>( pData ) = value.as<f32>();

      //------------------------------------------------------------------------
      // Real numbers.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<double>() ){
        *reinterpret_cast<double*>( pData ) = value.as<double>();
      }else if( clsid == e_classid<f64::Value>() ){
        reinterpret_cast<f64::Value*>( pData )->setValue( value.as<f64>() );
      }else if( clsid == e_classid<f64>() ){
        *reinterpret_cast<f64*>( pData ) = value.as<f64>();
      }else if( clsid == e_classid<float>() ){
        *reinterpret_cast<float*>( pData ) = value.as<float>();
      }else if( clsid == e_classid<f32::Value>() ){
        reinterpret_cast<f32::Value*>( pData )->setValue( value.as<f32>() );
      }else if( clsid == e_classid<f32>() ){
        *reinterpret_cast<f32*>( pData ) = value.as<f32>();
      }else if( clsid == e_classid<f16>() ){
        *reinterpret_cast<f16*>( pData ) = value.as<f32>();

      //------------------------------------------------------------------------
      // Boolean.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<bool>() ){
        *reinterpret_cast<bool*>( pData ) = value.as<bool>();

      //------------------------------------------------------------------------
      // Integers.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<u64>() ){
        *reinterpret_cast<u64*>( pData ) = value.as<u64>();
      }else if( clsid == e_classid<s64>() ){
        *reinterpret_cast<s64*>( pData ) = value.as<s64>();
      }else if( clsid == e_classid<u32>() ){
        *reinterpret_cast<u32*>( pData ) = value.as<u32>();
      }else if( clsid == e_classid<s32>() ){
        *reinterpret_cast<s32*>( pData ) = value.as<s32>();
      }else if( clsid == e_classid<u16>() ){
        *reinterpret_cast<u16*>( pData ) = value.as<u16>();
      }else if( clsid == e_classid<s16>() ){
        *reinterpret_cast<s16*>( pData ) = value.as<s16>();
      }else if( clsid == e_classid<u8>() ){
        *reinterpret_cast<u8*>( pData ) = value.as<u8>();
      }else if( clsid == e_classid<s8>() ){
        *reinterpret_cast<s8*>( pData ) = value.as<s8>();

      //------------------------------------------------------------------------
      // Strings.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<string>() ){
        *reinterpret_cast<gfc::string*>( pData ) = value;

      //------------------------------------------------------------------------
      // Transforms.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<vec4x4>() ){
      }else if( clsid == e_classid<qst3>() ){

      //------------------------------------------------------------------------
      // Vectors and points.
      //------------------------------------------------------------------------

      }else if(( clsid == e_classid<vec4>() )||( clsid == e_classid<ssi<vec4>>() )||( clsid == e_classid<ssi_mass<vec4>>() )){
        *reinterpret_cast<vec4*>( pData ) = value.as<vec4>();
      }else if(( clsid == e_classid<vec3>() )||( clsid == e_classid<ssi<vec3>>() )||( clsid == e_classid<ssi_mass<vec3>>() )){
        *reinterpret_cast<vec3*>( pData ) = value.as<vec3>();
      }else if(( clsid == e_classid<vec2>() )||( clsid == e_classid<ssi<vec2>>() )||( clsid == e_classid<ssi_mass<vec2>>() )){
        *reinterpret_cast<vec2*>( pData ) = value.as<vec2>();
      }else if(( clsid == e_classid<pt3>() )||( clsid == e_classid<ssi<pt3>>() )||( clsid == e_classid<ssi_mass<pt3>>() )){
        *reinterpret_cast<pt3*>( pData ) = value.as<pt3>();
      }else if(( clsid == e_classid<pt2>() )||( clsid == e_classid<ssi<pt2>>() )||( clsid == e_classid<ssi_mass<pt2>>() )){
        *reinterpret_cast<pt2*>( pData ) = value.as<pt2>();
      }else if( clsid == e_classid<vec4i>() ){
        *reinterpret_cast<vec4i*>( pData ) = value.as<vec4i>();
      }else if( clsid == e_classid<vec3i>() ){
        *reinterpret_cast<vec3i*>( pData ) = value.as<vec3i>();
      }else if( clsid == e_classid<vec2i>() ){
        *reinterpret_cast<vec2i*>( pData ) = value.as<vec2i>();

      //------------------------------------------------------------------------
      // Bounding volumes.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<aabb3>() ){
        *reinterpret_cast<aabb3*>( pData ) = value.as<aabb3>();
      }else if( clsid == e_classid<aabb2>() ){
        *reinterpret_cast<aabb2*>( pData ) = value.as<aabb2>();

      //------------------------------------------------------------------------
      // Rotations.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<quat>() ){
        *reinterpret_cast<quat*>( pData ) = value.as<quat>();

      //------------------------------------------------------------------------
      // Colors.
      //------------------------------------------------------------------------

      }else if( clsid == e_classid<rgba>() ){
        *reinterpret_cast<rgba*>( pData ) = value.as<rgba>();
      }else{
        DEBUG_BREAK
      }
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//string:{                                        |
  //Statics:{                                     |

    string string::null;

  //}:                                            |
  //Private:{                                     |
    //kmp:{                                       |

      namespace{
        s32* prekmp( ccp pattern, const s32 n_pattern ){
          s32 k = -1;
          s32 i = 1;
          s32* pi = (s32*)catvAlloc();
          *pi = k;
          for( i=1; i<n_pattern; ++i ){
            while(( k > -1 )&&( pattern[k+1] != pattern[i] )){
              k = pi[k];
            }
            if( pattern[i] == pattern[k+1] ){
              k++;
            }
            pi[i] = k;
          }
          return pi;
        }
        std::vector<s32> kmp( ccp target, const s32 n_target, ccp pattern, const s32 n_pattern ){
          if( !target || !n_target ){
            return{};
          }
          if( !pattern || !n_pattern ){
            return{};
          }
          std::vector<s32> matches;
          s32 i;
          s32 *pi = prekmp( pattern, n_pattern );
          s32 k = -1;
          for( i=0; i<n_target; ++i ){
            while(( k > -1 )&&( pattern[ k+1 ] != target[ i ])){
              k = pi[k];
            }
            if( target[i] == pattern[ k+1 ]){
              k++;
            }
            if( k == n_pattern-1 ){
              matches.push_back( i-k );
              k = -1;
            }
          }
          return matches;
        }
      }

    //}:                                          |
  //}:                                            |
  //Operate:{                                     |
    //operator!=:{                                |

#ifdef __APPLE__
  #pragma mark - Operators -
#endif

      bool String::operator!=( const string& s )const{
        const bool iSempty = s.empty();
        const bool isEmpty = empty();
        if( isEmpty && !iSempty ){
          return true;
        }
        if( !isEmpty && iSempty ){
          return true;
        }
        if( isEmpty && iSempty ){
          return false;
        }
        return( m_uHash != s.m_uHash );
      }

      bool String::operator!=( ccp pSrc )const{
        const bool isEmpty = empty();
        if( !isEmpty && !pSrc ){
          return true;
        }
        if( isEmpty && pSrc ){
          return true;
        }
        if( isEmpty && !pSrc ){
          return false;
        }
        return( m_uHash != e_hashstr64( pSrc ));
      }

    //}:                                          |
    //operator==:{                                |

      bool String::operator==( const string& s )const{
        const bool iSempty = s.empty();
        const bool isEmpty = empty();
        if( isEmpty && !iSempty ){
          return false;
        }
        if( !isEmpty && iSempty ){
          return false;
        }
        if( isEmpty && iSempty ){
          return true;
        }
        return( m_uHash == s.m_uHash );
      }

      bool String::operator==( ccp pSrc )const{
        const bool isEmpty = empty();
        if( !isEmpty && !pSrc ){
          return false;
        }
        if( isEmpty && pSrc ){
          return false;
        }
        if( isEmpty && !pSrc ){
          return true;
        }
        return( m_uHash == e_hashstr64( pSrc ));
      }

    //}:                                          |
    //operator=:{                                 |

      string& String::operator=( const string& lvalue ){
        if(( this != &lvalue )&&( m_uHash != lvalue.m_uHash )){
          clear();
          cat( lvalue );
        }
        return *this;
      }

      string& String::operator=( string&& rvalue ){
        if(( this != &rvalue )&&( m_uHash != rvalue.m_uHash )){
          clear();
          if( rvalue.m_uHash ){
            m_uHash = rvalue.m_uHash;
            rvalue.m_uHash = 0;
            #if e_compiling( debug )
              m_pData = std::move( rvalue.m_pData );
            #endif
          }
        }
        return *this;
      }

      string& String::operator=( ccp pValue ){
        clear();
        if( pValue ){
          cat( pValue );
        }
        return *this;
      }

    //}:                                          |
    //operator+:{                                 |

      string String::operator+( const string& s )const{
        string r;
        r.cat( *this );
        r.cat( s );
        return r;
      }

      string String::operator+( ccp p )const{
        string r;
        r.cat( *this );
        if( p )
          r.cat( p );
        return r;
      }

    //}:                                          |
    //operator<:{                                 |

      bool String::operator<( const string& s )const{
        if( this != &s ){
          return( strcmp( c_str(), s.c_str() ) < 0 );
        }
        return false;
      }

      bool String::operator<( ccp pCmp )const{
        return( strcmp( c_str(), pCmp ) < 0 );
      }

    //}:                                          |
    //operator>:{                                 |

      bool String::operator>( const string& s )const{
        if( this != &s ){
          return( strcmp( c_str(), s.c_str() ) > 0 );
        }
        return false;
      }

      bool String::operator>( ccp pCmp )const{
        return( strcmp( c_str(), pCmp ) > 0 );
      }

    //}:                                          |
  //}:                                            |
  //Structs:{                                     |

#ifdef __APPLE__
  #pragma mark - Classes -
#endif

    enum class Status:s8{
      kNaN=-1,
      kFalse,
      kTrue,
    };

  //}:                                            |
  //Private:{                                     |
    //asDecimal:{                                 |

#ifdef __APPLE__
  #pragma mark - Private -
#endif

      namespace{
        template<typename T>Status asDecimal( ccp pString, T& value ){
          e_sanity_check( !e_isbad( pString ));
          cp s = String::skip_anyws( cp( pString ));
          if( !s ){
            return Status::kNaN;
          }
          cp E = s+strlen( s );

          //--------------------------------------------------------------------
          // Hexidecimal numbers
          //--------------------------------------------------------------------

          bool bIsHexidecimal=false;
          if( !strncmp( s, "0x", 2 )){
            bIsHexidecimal = true;
            s += 2;
          }else if( *s == '$' ){
            bIsHexidecimal = true;
            s++;
          }
          if( bIsHexidecimal ){
            cp e = s;
            while(( isdigit( *e )||(( *e >= 'A' )&&( *e <= 'F' ))||(( *e >= 'a' )&&( *e <= 'f' )))&&( e < E )){
              ++e;
            }
            if( e == s ){
              return Status::kNaN;
            }
            if( String::is_alpha( *e )){
              //failed hexadecimal test but still alpha so NaN.
              return Status::kNaN;
            }
            T shift = 0;
            value = 0;
            while( --e >= s ){
              if(( *e >= '0' )&&( *e <= '9' )){
                value |= (*e-'0')<<shift;
              }else if(( *e >= 'A' )&&( *e <= 'F' )){
                value |= (*e-'A'+10)<<shift;
              }else if(( *e >= 'a' )&&( *e <= 'f' )){
                value |= (*e-'a'+10)<<shift;
              }else{
                return Status::kNaN;
              }
              shift+=4;
            }
            return Status::kTrue;
          }

          //--------------------------------------------------------------------
          // Binary numbers: 0101_0001_1111_1011011
          //--------------------------------------------------------------------

          bool bIsBinary=false;
          if( !strncmp( s, "0b", 2 )){
            bIsBinary = true;
            s += 2;
          }
          if( bIsBinary ){
            cp e = s;
            while((( *e == '0' )||( *e == '1' )||( *e == '_' ))&&( e < E )){
              ++e;
            }
            if( e == s ){
              return Status::kNaN;
            }
            T shift = 0;
            value = 0;
            while( --e >= s ){
              if(( *e == '0' )||( *e == '1' )){
                value |= (*e-'0')<<shift;
              }else if( *e=='_' ){
                continue;
              }else{
                return Status::kNaN;
              }
              shift++;
            }
            return Status::kTrue;
          }

          //--------------------------------------------------------------------
          // Decimal numbers
          //--------------------------------------------------------------------

          cp e = s;
          while( String::is_digit( *e )&&( e < E )){
            ++e;
          }
          if( e == s ){
            const float fvalue = atof( s );
            value = T( fvalue );
          }else{
            if( String::is_alpha( *e )){
              e_assert( !"invalid number in string" );
              return Status::kNaN;
            }
            T shift = 1;
            while( --e >= s ){
              value += (*e-'0')*shift;
              shift *= 10;
            }
          }
          return Status::kTrue;
        }
      }

    //}:                                          |
  //}:                                            |
  //Methods:{                                     |
    //[parsing]:{                                 |
      //skip_anyws:{                              |

#ifdef __APPLE__
  #pragma mark - Skip methods -
#endif

        cp String::skip_anyws( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = cp( in );
          while(( *s > 0 )&&( *s <= ' ' )){
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skip_anynonws:{                           |

        cp String::skip_anynonws( ccp in ){
          e_sanity_check( !e_isbad( in ));
          const u8* s = (u8*)in;
          if( *s <= 32 ){
            return cp( s );
          }
          if( is_alpha( *s )||( *s == '_' )){
            while( *s &&(( *s == '_' )||is_alpha( *s )||is_digit( *s ))){
              ++s;
            }
            return cp( s );
          }
          while( *s ){
            if( *s <= 32 ){
              break;
            }
            ++s;
          }
          return cp( s );
        }

      //}:                                        |
      //skip_nonws:{                              |

        cp String::skip_nonws( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = cp( in );
          while(( *s > 0x0 )&&(
              (( *s >= 'a' )&&( *s <= 'z' ))
            ||(( *s >= 'A' )&&( *s <= 'Z' ))
            ||(( *s >= '0' )&&( *s <= '9' )))){
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skip_2eol:{                               |

        cp String::skip_2eol( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = cp( in );
          while( !is_eol( *s )){
            if( !*s ){
              return nullptr;
            }
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skipeols:{                                |

        cp String::skip_eols( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = cp( in );
          while(( *s > 0 )&& is_eol( *s )){
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skipws:{                                  |

        cp String::skip_ws( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = cp( in );
          while(( *s <= ' ' )&&( *s != 13 )&&( *s != 10 )){
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skipnumber:{                              |

        cp String::skip_number( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = skip_anyws( in );
          if(( *s == '-' )||( *s == '+' )){
            ++s;
          }
          bool dot=false;
          bool exp=false;
          while((( *s>='0' )&&( *s<='9' ))||( *s=='.' )){
            if( *s == '.' ){
              if( !dot ){
                dot = true;
              }else{
                break;
              }
            }
            if( *s == 'e' ){
              if( !exp ){
                if(( s[1] == '-' )||( s[1] == '+' )){
                  s++;
                }
                exp = true;
              }else{
                break;
              }
            }
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skipcc:{                                  |

        //skip c comment.
        cp String::skip_cc( ccp in ){
          e_sanity_check( !e_isbad( in ));
          u32 d = 0;
          cp s = skip_anyws( in );
          while( *s > 0 ){
            if(( s[0] == '/' )&&( s[1] == '/' )&& !d ){
              s = skip_2eol( s );
              s = skip_eols( s );
              break;
            }
            if(( s[0] == '/' )&&( s[1] == '*' )){
              ++s;
              ++d;
            }
            if(( s[0] == '*' )&&( s[1] == '/' )){
              ++s;
              --d;
              if( !d ){
                break;
              }
            }
            ++s;
          }
          return s;
        }

      //}:                                        |
      //skipstr:{                                 |

        cp String::skip_str( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = skip_anyws( cp( in ));
          if( *s == '"' ){
            s=strchr( s+1, '"' );
          }else if( *s == '\'' ){
            s=strchr( s+1, '\'' );
          }
          return s;
        }

      //}:                                        |
      //is_eol:{                                  |

#ifdef __APPLE__
  #pragma mark - Is methods -
#endif

        bool String::is_eol( const char c ){
          return( c == 10 )||( c == 13 );
        }

      //}:                                        |
      //is_hex:{                                  |

        bool String::is_hex()const{
          ccp p = c_str();
          ccp e = end();
          while( p < e ){
            if(!( is_digit( *p )||((( *p >= 'a' )&&( *p <= 'f' ))||(( *p >= 'A' )&&( *p <= 'F' ))))){
              return false;
            }
            ++p;
          }
          return true;
        }

      //}:                                        |
      //iscc:{                                    |

        bool String::is_cc( ccp s ){
          e_sanity_check( !e_isbad( s ));
          if( skip_cc( s ) > s ){
            return true;
          }
          return false;
        }

      //}:                                        |
      //isalpha:{                                 |

        bool String::is_alpha( const char ascii_char ){
          if(( ascii_char >= 'a' )&&( ascii_char <= 'z' )){
            return true;
          }
          if(( ascii_char >= 'A' )&&( ascii_char <= 'Z' )){
            return true;
          }
          return false;
        }

        bool String::is_alpha( ccp in ){
          e_sanity_check( !e_isbad( in ));
          ccp s = skip_anyws( in );
          return is_alpha( *s );
        }

      //}:                                        |
      //isupper:{                                 |

        bool String::is_upper( const char c ){
          return( c >= 'A' )&&( c <= 'Z' );
        }

        bool String::is_upper( ccp s ){
          e_sanity_check( !e_isbad( s ));
          while( *s ){
            if( !is_upper( *s )){
              return false;
            }
            ++s;
          }
          return true;
        }

      //}:                                        |
      //islower:{                                 |

        bool String::is_lower( const char c ){
          return( c >= 'a' )&&( c <= 'z' );
        }

        bool String::is_lower( ccp s ){
          e_sanity_check( !e_isbad( s ));
          while( *s ){
            if( !is_lower( *s )){
              return false;
            }
            ++s;
          }
          return true;
        }

      //}:                                        |
      //isid:{                                    |

        bool String::is_id( const char ascii_char, const bool bFirst ){
          if( ascii_char == '_' ){
            return true;
          }
          if( is_alpha( ascii_char )){
            return true;
          }
          if( !bFirst && is_digit( ascii_char )){
            return true;
          }
          return false;
        }

        bool String::is_id( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = skip_anyws( in );
          if( !*s ){
            return false;
          }
          if( !is_id( *s, true )){
            return false;
          }
          while( *++s > 0 ){
            if( !is_id( *s, false )){
              return false;
            }
          }
          return true;
        }

      //}:                                        |
      //isnm:{                                    |

        bool String::is_digit( const char c ){
          if( c >= '0' && c <= '9' ){
            return true;
          }
          return false;
        }

        bool String::is_number( ccp in ){
          e_sanity_check( !e_isbad( in ));
          cp s = skip_anyws( in );
          if( skip_number( s ) > s ){
            return true;
          }
          return false;
        }

      //}:                                        |
    //}:                                          |
    //[as]:{                                      |
      //as<strings>:{                             |

#ifdef __APPLE__
  #pragma mark - As methods -
#endif

        namespace EON{
          namespace gfc{
            template<> strings String::as<strings>( ccp pString ){
              return string( pString ).splitLines();
            }
          }
        }

      //}:                                        |
      //as<double>:{                              |

        namespace EON{
          namespace gfc{
            template<> double String::as<double>( ccp pString ){
              return double( atof( pString ));
            }
          }
        }

      //}:                                        |
      //as<float>:{                               |

        namespace EON{
          namespace gfc{
            template<> float String::as<float>( ccp pString ){
              return float( atof( pString ));
            }
          }
        }

      //}:                                        |
      //as<vec4x4>:{                              |

        namespace EON{
          namespace gfc{
            template<> vec4x4 String::as<vec4x4>( ccp pString ){
              float f[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return vec4x4(
                  f[ 0], f[ 1], f[ 2], f[ 3],
                  f[ 4], f[ 5], f[ 6], f[ 7],
                  f[ 8], f[ 9], f[10], f[11],
                  f[12], f[13], f[14], f[15] );
            }
          }
        }

      //}:                                        |
      //as<aabb3>:{                               |

        namespace EON{
          namespace gfc{
            template<> aabb3 String::as<aabb3>( ccp pString ){
              float f[6]={0,0,0,0,0,0};
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return aabb3( f[0], f[1], f[2], f[3], f[4], f[5] );
            }
          }
        }

      //}:                                        |
      //as<aabb2>:{                               |

        namespace EON{
          namespace gfc{
            template<> aabb2 String::as<aabb2>( ccp pString ){
              float f[4]={0,0,0,0};
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return aabb2( f[0], f[1], f[2], f[3] );
            }
          }
        }

      //}:                                        |
      //as<quat>:{                                |

        namespace EON{
          namespace gfc{
            template<> quat String::as<quat>( ccp pString ){
              float f[3]={ 0,0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              quat q;
              q.setEuler( vec3( f[0], f[1], f[2] ));
              return q;
            }
          }
        }

      //}:                                        |
      //as<rgba>:{                                |

        namespace EON{
          namespace gfc{
            template<> rgba String::as<rgba>( ccp pString ){
              rgba c = rgba::kBlack;
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<4; ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  c.tuple[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return c;
            }
          }
        }

      //}:                                        |
      //as<vec4i>:{                               |

        namespace EON{
          namespace gfc{
            template<> vec4i String::as<vec4i>( ccp pString ){
              s32 f[4]={ 0,0,0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<s32>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return *(vec4i*)f;
            }
          }
        }

      //}:                                        |
      //as<vec4>:{                                |

        namespace EON{
          namespace gfc{
            template<> vec4 String::as<vec4>( ccp pString ){
              float f[4]={ 0,0,0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return vec4( f[0], f[1], f[2], f[3] );
            }
          }
        }

      //}:                                        |
      //as<pt3>:{                                 |

        namespace EON{
          namespace gfc{
            template<> pt3 String::as<pt3>( ccp pString ){
              float f[3]={ 0,0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return pt3( f[0], f[1], f[2] );
            }
          }
        }

      //}:                                        |
      //as<vec3i>:{                               |

        namespace EON{
          namespace gfc{
            template<> vec3i String::as<vec3i>( ccp pString ){
              s32 f[3]={ 0,0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<s32>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return *(vec3i*)f;
            }
          }
        }

      //}:                                        |
      //as<vec3>:{                                |

        namespace EON{
          namespace gfc{
            template<> vec3 String::as<vec3>( ccp pString ){
              pt3 p = as<pt3>( pString );
              return vec3( p.x, p.y, p.z );
            }
          }
        }

      //}:                                        |
      //as<pt2>:{                                 |

        namespace EON{
          namespace gfc{
            template<> pt2 String::as<pt2>( ccp pString ){
              float f[2]={ 0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return pt2( f[0], f[1] );
            }
          }
        }

      //}:                                        |
      //as<vec2i>:{                               |

        namespace EON{
          namespace gfc{
            template<> vec2i String::as<vec2i>( ccp pString ){
              s32 f[2]={ 0,0 };
              if( pString ){
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<s32>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
              }
              return *(vec2i*)f;
            }
          }
        }

      //}:                                        |
      //as<vec2>:{                                |

        namespace EON{
          namespace gfc{
            template<> vec2 String::as<vec2>( ccp pString ){
              pt2 p = as<pt2>( pString );
              return vec2( p.x, p.y );
            }
          }
        }

      //}:                                        |
      //as<qst3>:{                                |

        namespace EON{
          namespace gfc{
            template<> qst3 String::as<qst3>( ccp pString ){
              qst3 L2W;
              if( pString ){
                float f[7]={ 0,0,0,0,0,0,0 };
                cp s = cp( pString );
                cp E = s+strlen( s );
                for( u32 i=0; i<e_dimof( f ); ++i ){
                  s = String::skip_anyws( s );
                  cp e = strchr( s, ',' );
                  if( !e ){
                    e = strchr( s, ' ' );
                  }
                  string expr( s, e ? e : E );
                  f[i] = expr.as<float>();
                  if( !e ){
                    break;
                  }
                  s = e+1;
                }
                L2W.toPosition() = pt3( f[0], f[1], f[2] );
                L2W.toRotation().setEuler( vec3( f[3], f[4], f[5] ));
                L2W.setScale( f[6] );
              }
              return L2W;
            }
          }
        }

      //}:                                        |
      //as<?64>:{                                 |

        namespace EON{
          namespace gfc{
            template<> u64 String::as<u64>( ccp pString ){
              u64 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
            template<> s64 String::as<s64>( ccp pString ){
              s64 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
          }
        }

      //}:                                        |
      //as<?32>:{                                 |

        namespace EON{
          namespace gfc{
            template<> u32 String::as<u32>( ccp pString ){
              u32 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
            template<> s32 String::as<s32>( ccp pString ){
              s32 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
          }
        }

      //}:                                        |
      //as<u16>:{                                 |

        namespace EON{
          namespace gfc{
            template<> u16 String::as<u16>( ccp pString ){
              u16 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
            template<> s16 String::as<s16>( ccp pString ){
              s16 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
          }
        }

      //}:                                        |
      //as<u8>:{                                  |

        namespace EON{
          namespace gfc{
            template<> u8 String::as<u8>( ccp pString ){
              u8 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
            template<> s8 String::as<s8>( ccp pString ){
              s8 value = 0;
              if( pString ){
                asDecimal( pString, value );
              }
              return value;
            }
          }
        }

      //}:                                        |
      //as<f64>:{                                 |

        namespace EON{
          namespace gfc{
            template<> f64 String::as<f64>( ccp pString ){
              return as<double>( pString );
            }
          }
        }

      //}:                                        |
      //as<f32>:{                                 |

        namespace EON{
          namespace gfc{
            template<> f32 String::as<f32>( ccp pString ){
              return as<float>( pString );
            }
          }
        }

      //}:                                        |
      //as<bool>:{                                |

        namespace EON{
          namespace gfc{
            template<> bool String::as<bool>( ccp pString ){
              switch( e_hashstr64( pString )){
                case"FALSE"_64:
                case"False"_64:
                case"false"_64:
                case   "NO"_64:
                case   "No"_64:
                case   "no"_64:
                case    "0"_64:
                  return false;
                case "TRUE"_64:
                case "True"_64:
                case "true"_64:
                case  "YES"_64:
                case  "Yes"_64:
                case  "yes"_64:
                case    "1"_64:
                  return true;
                default:
                  return false;
              }
            }
          }
        }

      //}:                                        |
    //}:                                          |
    //find:{                                      |

#ifdef __APPLE__
  #pragma mark - Search methods -
#endif

      ccp String::find( const string& s )const{
        e_sanity_check( !e_isbad( s.get().query() ));
        e_guardr( lock() );
        const auto itA = db::lock().find( s.m_uHash );
        if( itA == db::lock().end() ){
          return nullptr;
        }
        const auto itB = db::lock().find( m_uHash );
        if( itB == db::lock().end() ){
          return nullptr;
        }
        ccp pDst = nullptr;
        itA->second->query(
          [&]( ccp pData ){
            pDst = pData;
          }
        );
        if( !pDst ){
          return nullptr;
        }
        ccp pSrc = nullptr;
        itB->second->query(
          [&]( ccp pData ){
            pSrc = pData;
          }
        );
        if( !pSrc ){
          return nullptr;
        }
        return::strstr( pDst, pSrc );
      }

      ccp String::find( ccp pSrc )const{
        e_sanity_check( !e_isbad( s.get().query() ));
        e_guardr( lock() );
        const auto it = db::lock().find( m_uHash );
        if( it == db::lock().end() ){
          return nullptr;
        }
        ccp pDst = nullptr;
        it->second->query(
          [&]( ccp pData ){
            pDst = pData;
          }
        );
        if( !pDst ){
          return nullptr;
        }
        return::strstr( pDst, pSrc );
      }

    //}:                                          |
    //chr:{                                       |

      ccp String::chr( const char c )const{
        e_sanity_check( !e_isbad( s.get().query() ));
        e_guardr( lock() );
        const auto it = db::lock().find( m_uHash );
        if( it == db::lock().end() ){
          return nullptr;
        }
        ccp pDst = nullptr;
        it->second->query(
          [&]( ccp pData ){
            pDst = pData;
          }
        );
        if( !pDst ){
          return nullptr;
        }
        return::strchr( pDst, c );
      }

    //}:                                          |
    //camelcase:{                                 |

#ifdef __APPLE__
  #pragma mark - Case methods -
#endif

      string String::camelcase()const{
        if( empty() ){
          return nullptr;
        }
        stream st;
        ccp r = c_str();
        ccp e = end();
        if( *r >= 'A' && *r <= 'Z' ){
          *st.realloc( 1 )=( *r+++32 );
        }
        while( r < e ){
          cp s = skip_anyws( r );
          while( *s == '_' ){
            ++s;
          }
          const bool b=( s > r );
          if( b &&( *s >= 'a' )&&( *s <= 'z' )){
            *st.realloc( 1 )=( *s-32 );
          }else if( s < e ){
            *st.realloc( 1 )=*s;
          }
          r = s+1;
        }
        string s;
        st.query(
          [&]( ccp pBuffer ){
            s.cat( pBuffer );
          }
        );
        return s;
      }

    //}:                                          |
    //mixedcase:{                                 |

      string String::mixedcase()const{
        if( empty() ){
          return nullptr;
        }
        stream st;
        ccp r = c_str();
        ccp e = end();
        if( *r >= 'a' && *r <= 'z' ){
          *st.realloc( 1 )=( *r++-32 );
        }
        while( r < e ){
          cp s = skip_anyws( r );
          while( *s == '_' ){
            ++s;
          }
          const bool b=( s > r );
          if( b &&( *s >= 'a' )&&( *s <= 'z' )){
            *st.realloc( 1 )=( *s-32 );
          }else if( s < e ){
            *st.realloc( 1 )=*s;
          }
          r = s+1;
        }
        string s;
        st.query(
          [&]( ccp pBuffer ){
            s.cat( pBuffer );
          }
        );
        return s;
      }

    //}:                                          |
    //lower:{                                     |

      void String::lower(){
        if( empty() ){
          return;
        }
        stream st;
        ccp r = c_str();
        ccp e = end();
        while( r < e ){
          cp s = cp( r );
          if(( *s >= 'A' )&&( *s <= 'Z' )){
            *st.realloc( 1 )=( *s+32 );
          }else{
            *st.realloc( 1 )=*s;
          }
          ++r;
        }
        u64 hash = 0;
        st.query(
          [&]( ccp pBuffer ){
            hash = e_hashstr64( pBuffer );
          }
        );
        clear();
        db::write(
          [&]( db::unordered_map& m ){
            const auto& it = m.find( hash );
            if( it == m.end() ){
              m.insert_or_assign( hash, std::make_unique<stream>( std::move( st )));
            }else{
              it->second->addRef();
            }
          }
        );
        m_uHash = hash;
        #if e_compiling( debug )
          m_pData = c_str();
        #endif
      }

      string String::tolower()const{
        if( empty() ){
          return *this;
        }
        string lwr( *this );
        lwr.lower();
        return lwr;
      }

    //}:                                          |
    //upper:{                                     |

      void String::upper(){
        if( empty() ){
          return;
        }
        stream st;
        ccp r = c_str();
        ccp e = end();
        while( r < e ){
          cp s = cp( r );
          if(( *s >= 'a' )&&( *s <= 'z' )){
            *st.realloc( 1 )=( *s-32 );
          }else{
            *st.realloc( 1 )=*s;
          }
          ++r;
        }
        u64 hash = 0;
        st.query(
          [&]( ccp pBuffer ){
            hash = e_hashstr64( pBuffer );
          }
        );
        clear();
        db::write(
          [&]( db::unordered_map& m ){
            const auto& it = m.find( hash );
            if( it == m.end() ){
              m.insert_or_assign( hash, std::make_unique<stream>( std::move( st )));
            }else{
              it->second->addRef();
            }
          }
        );
        m_uHash = hash;
        #if e_compiling( debug )
          m_pData = c_str();
        #endif
      }

      string String::toupper()const{
        if( empty() ){
          return *this;
        }
        string upr( *this );
        upr.upper();
        return upr;
      }

    //}:                                          |
    //streamId:{                                |

#ifdef __APPLE__
  #pragma mark - Allocation methods -
#endif

      string String::streamId(){
        char text[25]{};
        text[ 0 ] = '8';
      retry:
        for( u32 i=1; i<24; ++i ){
          const u8 select = e_rand<u8>() % 2;
          switch( select ){
            case 0:
              text[ i ] = s8( e_rand<u32>( 'A', 'F' ) & 0xFF );
              break;
            case 1:
              text[ i ] = s8( e_rand<u32>( '0', '9' ) & 0xFF );
              break;
          }
        }
        { e_guardr( lock() );
          auto it = db::lock().find( e_hashstr64( text ));
          if( it != db::lock().end() ){
            goto retry;
          }
        }
        string r;
        r.setf( text );
        return r;
      }

    //}:                                          |
    //guid:{                                      |

      //{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
      string String::guid(){
        char text[39]{};
        #if e_compiling( win64 )
          GUID guid;
          const HRESULT hr = CoCreateGuid( &guid );
          if( !hr ){
            const u8* p = reinterpret_cast<u8*>( &guid );
            static const s8 t[] = "0123456789ABCDEF";
            for( u32 i=0; i<36; ){
              switch( i ){
                case  8:
                case 13:
                case 18:
                case 23:
                  text[ i++ ]='-';
                  continue;
              }
              const u8 h = ( *p & 0xF0 ) >> 4;
              const u8 l = ( *p & 0x0F ) >> 0;
              text[ i++ ]=t[ l ];
              text[ i++ ]=t[ h ];
              ++p;
            }
            string r;
            r.setf( "{%s}", text );
            return r;
          }
        #endif
      retry:
        for( u32 i=0; i<38; ++i ){
          switch( i ){
            case  8:
            case 13:
            case 18:
            case 23:
              text[ i ]='-';
              continue;
          }
          const u8 select = e_rand<u8>() % 2;
          switch( select ){
            case 0:
              text[ i ] = s8( e_rand<u32>( 'A', 'F' ) & 0xFF );
              break;
            case 1:
              text[ i ] = s8( e_rand<u32>( '0', '9' ) & 0xFF );
              break;
          }
        }
        { e_guardr( lock() );
          auto it = db::lock().find( e_hashstr64( text ));
          if( it != db::lock().end() ){
            goto retry;
          }
        }
        string r;
        r.setf( text );
        return r;
      }

    //}:                                          |
    //setv:{                                      |

#ifdef __APPLE__
  #pragma mark - Allocation methods -
#endif

      string& String::setv( ccp format, va_list va ){
        clear();
        catv( format, va );
        return *this;
      }

    //}:                                          |
    //setf:{                                      |

      string& String::setf( ccp format,... ){
        e_sanity_check( !e_isbad( format ));
        if( format ){
          va_list va;
          va_start( va, format );
            setv( format, va );
          va_end( va );
        }
        return *this;
      }

    //}:                                          |
    //tryv:{                                      |

      u64 String::tryv( ccp format, va_list va ){
        e_sanity_check( !e_isbad( format ));
        u64 bytes = 0;
        if( format ){
          #if e_compiling( win64 )
            bytes = u64(
              vsprintf_s(
                  catvAlloc()
                , CATV_BUFFER_SIZE
                , format
                , va
              )
            );
          #else
            bytes = u64(
              vsnprintf(
                  catvAlloc()
                , CATV_BUFFER_SIZE
                , format
                , va
              )
            );
            if( bytes >= CATV_BUFFER_SIZE ){
              e_break( "Holy crap!" );
            }
          #endif
        }
        return bytes;
      }

    //}:                                          |
    //tryf:{                                      |

      u64 String::tryf( ccp format,... ){
        u64 bytes = 0;
        va_list va;
        va_start( va, format );
          bytes = tryv( format, va );
        va_end( va );
        return bytes;
      }

    //}:                                          |
    //getv:{                                      |

      string String::getv( ccp format, va_list va ){
        e_sanity_check( !e_isbad( format ));
        string r;
        r.catv( format, va );
        return r;
      }

    //}:                                          |
    //getf:{                                      |

      string String::getf( ccp format,... ){
        va_list va;
        va_start( va, format );
          e_sanity_check( !e_isbad( format ));
          const string& r = getv( format, va );
        va_end( va );
        return r;
      }

    //}:                                          |
    //catv:{                                      |

      string& String::catv( ccp format, va_list va ){
        e_sanity_check( !e_isbad( format ));
        cp pCatvBuffer = catvAlloc();
        if( format ){
          #if e_compiling( win64 )
            const u64 bytes = u64(
              vsprintf_s(
                  pCatvBuffer
                , CATV_BUFFER_SIZE
                , format
                , va
              )
            );
          #else
            const u64 bytes = u64(
              vsnprintf(
                  pCatvBuffer
                , CATV_BUFFER_SIZE
                , format
                , va
              )
            );
          #endif
          if( bytes > CATV_BUFFER_SIZE ){
            e_break( "Holy crap!" );
          }
          cat( pCatvBuffer, bytes );
        }
        return *this;
      }

    //}:                                          |
    //catf:{                                      |

      string& String::catf( ccp format,... ){
        va_list va;
        va_start( va, format );
          catv( format, va );
        va_end( va );
        return *this;
      }

    //}:                                          |
    //cat:{                                       |

      string& String::cat( ccp pArg, const u64 nArg ){

        //----------------------------------------------------------------------
        // Bail conditions.
        //----------------------------------------------------------------------

        e_sanity_check( !e_isbad( pArg ));
        if( !pArg || !*pArg ){
          return *this;
        }
        if( !nArg ){
          return *this;
        }

        //----------------------------------------------------------------------
        // Create the concatenated string on the thread local scratch buffer.
        //----------------------------------------------------------------------

        std::shared_ptr<stream> ou;
        if( !m_uHash ){
          ou = std::make_shared<stream>();
        }else{
          db::write(
            [&]( db::unordered_map& m ){
              auto it = m.find( m_uHash );
              if( it != m.end() ){
                if( it->second->subRef() <= 0 ){
                  ou = std::move( it->second );
                  m.erase( m_uHash );
                }else{
                  ou = std::make_shared<stream>( *it->second );
                }
              }else{
                ou = std::make_shared<stream>();
              }
            }
          );
        }

        //----------------------------------------------------------------------
        // Concatenate the passed in string.
        //----------------------------------------------------------------------

        auto* ptr = ou->realloc( nArg );
        memcpy( ptr, pArg, nArg );
        ptr[ nArg ] = 0;

        // Compute the hash for the whole stream.
        const auto hash = e_hashstr64( ou->data() );

        // Write back at hash.
        db::write(
          [&]( db::unordered_map& m ){
            auto it = m.find( hash );
            if( it == m.end() ){
              ou->setRefs( 1 );
              m[ hash ] = std::move( ou );
            }else{
              it->second->addRef();
            }
          }
        );
        m_uHash = hash;

        //----------------------------------------------------------------------
        // Store off the data and stream.
        //----------------------------------------------------------------------

        #if e_compiling( debug )
          m_pData = c_str();
        #endif
        return *this;
      }

      string& String::cat( ccp s, ccp e ){

        //----------------------------------------------------------------------
        // Validation and sanity checks for the good of ya soul.
        //----------------------------------------------------------------------

        if( !s || !*s ){
          return *this;
        }
        e_sanity_check( !e_isbad( s ));
        e_sanity_check( !e_isbad( e ));
        e_assert( s <= e, "Bad ptr!" );

        //----------------------------------------------------------------------
        // Apend input string between s and e inclusive.
        //----------------------------------------------------------------------

        return cat( s, e-s );
      }

      string& String::cat( const string& s ){
        if( !s.empty() ){
          cat( s.c_str(), s.len() );
        }
        return *this;
      }

      string& String::cat( ccp pChars ){
        if( pChars && *pChars ){
          cat( pChars, strlen( pChars ));
        }
        return *this;
      }

    //}:                                          |
    //len:{                                       |

      u64 String::len()const{
        u64 ln = 0ULL;
        if( m_uHash ){
          db::read(
            [&]( const db::unordered_map& m ){
              const auto& it = m.find( m_uHash );
              if( it != m.end() ){
                ln = it->second->size();
              }
            }
          );
        }
        return ln;
      }

    //}:                                          |
    //right:{                                     |

#ifdef __APPLE__
  #pragma mark - Substring methods -
#endif

      string String::right( const u64 i )const{
        if( empty() ){
          return nullptr;
        }
        const u64 n = len();
        if( n > i ){
          return string( c_str()+n-i, end() );
        }
        return nullptr;
      }

    //}:                                          |
    //left:{                                      |

      bool String::isleft( ccp pText )const{
        if( !empty() ){
          return !strncmp( c_str(), pText, e_min<size_t>( len(), strlen( pText )));
        }
        return false;
      }

      string String::left( const u64 n )const{
        if( empty() ){
          return nullptr;
        }
        const u64 l = len();
        if( n > l ){
          return nullptr;
        }
        if( n < l ){
          ccp pBuffer = c_str();
          return string( pBuffer, pBuffer+n );
        }
        return *this;
      }

    //}:                                          |
    //mid:{                                       |

      string String::mid( const u64 left, const u64 right )const{
        e_assert( right-left <= len() );
        e_assert( right > left );
        ccp pData = c_str();
        if( pData ){
          return string( pData+left, pData+right );
        }
        return nullptr;
      }

    //}:                                          |
    //ltrim:{                                     |

      string String::ltrimmed( const u64 i )const{
        string r( *this );
        r.ltrim( i );
        return r;
      }

      string& String::ltrim( const u64 i ){

        //----------------------------------------------------------------------
        // Bail conditions.
        //----------------------------------------------------------------------

        if( empty() || !i ){
          return *this;
        }

        //----------------------------------------------------------------------
        // If trim greater than length just clear and return.
        //----------------------------------------------------------------------

        auto n = len();
        if( i >= n ){
          clear();
          return *this;
        }
        n -= i;

        //----------------------------------------------------------------------
        // Set the left trim amount and debug clear to '.'s.
        //----------------------------------------------------------------------

        stream st;
        cp ptr = st.realloc( n );
        memcpy(
            ptr
          , c_str()+i
          , n );
        clear();
        cat( ptr );
        return *this;
      }

    //}:                                          |
    //trim:{                                      |

      string String::trimmed( const u64 i )const{
        string r( *this );
        r.trim( i );
        return r;
      }

      string& String::trim( const u64 i ){
        if( empty() || !i ){
          return *this;
        }
        const u64 l = len();
        if( i >= l ){
          clear();
        }else{
          const u64 n=( l - i );
          stream st;
          cp ptr = st.alloc( n );
          memcpy( ptr, c_str(), n );
          ptr[n] = 0;
          clear();
          cat( ptr );
        }
        return *this;
      }

    //}:                                          |
    //clear:{                                     |

#ifdef __APPLE__
  #pragma mark - Utility methods -
#endif

      void String::clear(){
        // Decrement the reference count and destroy.
        db::write(
          [this]( db::unordered_map& m ){
            const auto it = m.find( m_uHash );
            if( it != m.end() ){
              const s32 refs = it->second->subRef();
              if( refs < 1 ){
                m.erase( it );
              }
            }
          }
        );
        // Nuke the hash key.
        m_uHash = 0;
        // Clear the debug only std::string copy.
        #if e_compiling( debug )
          m_pData = nullptr;
        #endif
      }

    //}:                                          |
    //repeat:{                                    |

      string& String::repeat( const char c, const u64 count ){
        // Bail conditions.
        if( !count || !c )
          return *this;
        // Create a stream object to replicate 'c' into.
        stream st;
        const u64 n = count * st.stride();
        cp p = st.realloc( count );
        memset( p, c, n );
        p[ n ] = 0;
        // Append stream data.
        cat( p );
        // Return *this.
        return *this;
      }

    //}:                                          |
    //hash:{                                      |

      u64 String::hash()const{
        return m_uHash;
      }

    //}:                                          |
    //c_str:{                                     |

      ccp String::c_str()const{
        ccp pResult = "";
        db::read(
          [&]( const db::unordered_map& m ){
            const auto it = m.find( m_uHash );
            if( it == m.end() ){
              return;
            }
            it->second->query(
              [&]( ccp p ){
                pResult = p;
              }
            );
          }
        );
        return pResult;
      }

    //}:                                          |
    //replace:{                                   |

      bool String::replace( const string& find, const string& with ){

        //----------------------------------------------------------------------
        // Bail conditions.
        //----------------------------------------------------------------------

        // Not using e_guardw due to std::move later on that would obliterate
        // the stream reference.
        if( empty() ){
          return false;
        }

        //----------------------------------------------------------------------
        // Find matches.
        //----------------------------------------------------------------------

        const auto L = find.len();
        std::vector<s32> matches;
        matches = kmp( c_str(), s32( len() ), find.c_str(), s32( L ));
        if( matches.empty() ){
          return false;
        }

        //----------------------------------------------------------------------
        // Slow string reconstruction.
        //----------------------------------------------------------------------

        stream st;
        const s32* eMatches = matches.data() + matches.size();
        const s32* pMatches = matches.data();
        ccp r = c_str();
        u64 l = 0;
        s64 n;
        cp  w;
        while( pMatches < eMatches ){
          n = *pMatches - l;
          e_assert( n >= 0 );
          if( n ){
            w = st.realloc( n );
            if( w != r ){
              memcpy( w, r, n );
              w[ n ] = 0;
            }
            r += n;
          }
          l = *pMatches + L;
          r += L;
          n = with.len();
          if( n ){
            w = st.realloc( n );
            if( n > 1 ){
              memcpy( w, with, n );
              w[ n ] = 0;
            }else{
              *w = *with;
            }
          }
          ++pMatches;
        }
        n = strlen( r );
        if( n ){
          w = st.realloc( n );
          memcpy( w, r, n );
          w[ n ] = 0;
        }

        //----------------------------------------------------------------------
        // Assign new stream.
        //----------------------------------------------------------------------

        st.query(
          [&]( ccp pBuffer ){
            this->clear( );
            cat( pBuffer, pBuffer + st.size() );
          }
        );
        return true;
      }

    //}:                                          |
    //spaces:{                                    |

      ccp String::spaces( const u32 bytes ){
        if( bytes ){
          cp spaces = e_salloc( bytes+1 );
          memset( spaces, 0x20202020, bytes );
          spaces[ bytes ] = 0;
          return spaces;
        }
        return"";
      }

    //}:                                          |
    //ext:{                                       |

      string String::ext()const{
        string r;
        ccp s = c_str();
        ccp z = end();
        ccp e = z;
        while( e > s ){
          if( *e == '.' ){
            r = string( e );
            break;
          }
          if( *e == '/' ){
            break;
          }
          --e;
        }
        return r;
      }

    //}:                                          |
    //envs:{                                      |

      string& String::envs(){
        IEngine::envs.foreach(
          [&]( const string_pair& sp ){
            replace( e_xfs( "$(%s)", sp.first.c_str() ), sp.second );
          }
        );
        return *this;
      }

      string String::envs()const{
        return string( *this ).envs();
      }

    //}:                                          |
    //os:{                                        |

      string String::os()const{
        #if e_compiling( win64 )
          if( !empty() ){
            string os;
            switch( *(u16*)c_str() ){
              case 0x2F7E:// ~/
                os = e_getenv( "USER" ) + "/" + ltrimmed( 2 );
                break;
              case 0x2F2E:// ./
                os = e_getenv( "PWD"  ) + "/" + ltrimmed( 2 );
                break;
              default:
                os = *this;
                break;
            }
            while( os.replace(    "/", "\\" ));
            while( os.replace( "\\\\", "\\" ));
            return os;
          }
        #else
          if( !empty() ){
            string os;
            switch( *(u16*)c_str() ){
              case 0x2F7E:// ~/
                os = e_getenv( "HOME" ) + "/" + ltrimmed( 2 );
                break;
              case 0x2F2E:// ./
                os = e_getenv( "PWD"  ) + "/" + ltrimmed( 2 );
                break;
              default:
                os = *this;
                break;
            }
            while( os.replace( "//", "/" ));
            return os;
          }
        #endif
        return *this;
      }

    //}:                                          |
    //path:{                                      |

      string String::path()const{
        if( !empty() ){
          ccp s = c_str();
          ccp e = end();
          while( e > s ){
            if( *e == '/' ){
              return string( s, e+1 );
            }
            --e;
          }
        }
        return nullptr;
      }

    //}:                                          |
    //filename:{                                  |

      string String::filename()const{
        if( empty() ){
          return nullptr;
        }
        string r;
        ccp s = c_str();
        ccp z = end();
        ccp e = z;
        while( e > s ){
          if( *e == '/' ){
            r = e+1;
            break;
          }
          --e;
        }
        if( r.empty() ){
          r = s;
        }
        return r;
      }

    //}:                                          |
    //base:{                                      |

      string String::base()const{
        if( empty() )
          return nullptr;
        ccp e = end();
        ccp start = c_str();
        while( e-- > start ){
          if( *e == '.' ){
            break;
          }
        }
        ccp s = e;
        if( s <= start ){
          s = e = end();
          while( s > start ){
            if( *s == '/' ){
              ++s;
              break;
            }
            --s;
          }
        }else{
          while( s > start ){
            if( *s == '/' ){
              ++s;
              break;
            }
            --s;
          }
        }
        return string( s, e );
      }

    //}:                                          |
    //splitLines:{                                |

      strings String::splitLines()const{
        strings vLines;
        if( !empty() ){
          ccp p = c_str();
          ccp e = p+len();
          ccp s = p;
          while( s < e ){
            ccp eol = skip_2eol( s );
            if( !eol ){
              vLines.push( s );
              break;
            }
            if( !is_eol( *s )){
              const string res = string( s, eol );
              vLines.push( res );
            }
            s = skip_eols( eol );
            u64 i = u64( eol > p );
            const u64 n = s-eol;
            while( i++ < n ){
              vLines.push( nullptr );
            }
          }
        }
        return vLines;
      }

    //}:                                          |
    //splitAtCommas:{                             |

      strings String::splitAtCommas()const{
        strings v;
        if( !empty() ){
          string tmp( c_str() );
          if( tmp.replace( ",", "\n" )){
            v = tmp.splitLines();
            auto it = v.getIterator();
            while( it ){
              string& s = *it;
              if( s.empty() ){
                it.erase();
                continue;
              }
              auto* b = s.c_str();
              auto* e = b;
              while( *e && ( *e <= ' ' )){
                ++e;
              }
              s.ltrim( e-b );
              ++it;
            }
          }else{
            v += tmp;
          }
        }
        return v;
      }

    //}:                                          |
    //scrubf:{                                    |

      string& String::scrubf(){
        #if 0
          if( empty() ){
            return *this;
          }
          s32 refs = 0;
          db::read(
            [&]( const db::unordered_map& m ){
              const auto it = m.find( m_uHash );
              if( it == m.end() ){
                return;
              }
              refs = it->second->subRef();
              e_assert( refs > -1 );
            }
          );
          if( !refs ){
            return *this;
          }




          stream ou;
          ccp    s = c_str();//because of left trimming.
          ccp    e = end();
          ccp    p = s;
          while( p < e ){
            if(( *p == '$' )&&( p[1] == '(' )){
              while( ++p < e ){
                if( *p == ')' ){
                  ++p;
                  break;
                }
              }
            }else if( !strncmp( p, "\033[", 2 )){
              while( ++p < e ){
                if( *p == 'm' ){
                  ++p;
                  break;
                }
              }
            }else{
              *ou.realloc( 1 ) = *p++;
            }
          }
          if( !refs ){
            db().erase( it );
          }
          ou.addRef();
          ou.query(
            [&]( ccp pBuffer ){
              m_uHash = e_hashstr64( pBuffer );
            }
          );
          db().insert_or_assign( m_uHash, std::make_unique<stream>( std::move( ou )));
          #if e_compiling( debug )
            m_pData = c_str();
          #endif
        #endif
        return *this;
      }

      string String::scrubf()const{
        string out( *this );
        return out.scrubf();
      }

    //}:                                          |
  //}:                                            |
  //Ctor:{                                        |

#ifdef __APPLE__
  #pragma mark - Ctors -
#endif

    String::String( const std::string& lvalue ){
      cat( lvalue.c_str() );
    }

    String::String( string&& rvalue ){
      if(( this != &rvalue ) && rvalue.m_uHash ){
        m_uHash = rvalue.m_uHash;
        rvalue.m_uHash = 0ULL;
        #if e_compiling( debug )
          m_pData = rvalue.m_pData;
          rvalue.m_pData = nullptr;
        #endif
      }
    }

    String::String( const string& lvalue ){
      if(( this != &lvalue ) && lvalue.m_uHash ){
        cat( lvalue );
      }
    }

    String::String( ccp a, ccp b ){
      e_sanity_check( !e_isbad( a ));
      e_sanity_check( !e_isbad( b ));
      e_assert( a <= b );
      if( !a || !*a || !b ){
        return;
      }
      cat( a, b );
    }

    String::String( ccp a ){
      e_sanity_check( !e_isbad( a ));
      if( !a || !*a ){
        return;
      }
      cat( a );
    }

  //}:                                            |
  //Dtor:{                                        |

    String::~String(){
      clear();
    }

  //}:                                            |
//}:                                              |
//================================================+=============================
