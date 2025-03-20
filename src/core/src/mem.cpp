//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#if e_compiling( web )
  #include<memory.h>
#elif e_compiling( win64 )
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include<windows.h>
#elif e_compiling( osx )
  #include<malloc/malloc.h>
  #include<mach/mach.h>
#elif e_compiling( linux )
  #include<memory.h>
#endif
#include<stdlib.h>

#if e_compiling( win64 )
  #define __compiling_ned_malloc__ 0
  #define __compiling_rp_malloc__  0 //Cannot enable this unless core lib is a DLL.
  #define __compiling_mem_checks__ 0 //__compiling_debug__
  #define __compiling_mem_logger__ 0
#else
  #define __compiling_ned_malloc__ 0
  #define __compiling_rp_malloc__  0 //Cannot enable this unless core lib is a DLL.
  #define __compiling_mem_checks__ 0 //__compiling_debug__
  #define __compiling_mem_logger__ 0
#endif
#undef e_memory_validator

using namespace EON;
using namespace gfc;

//================================================+=============================
//Private:{                                       |
  //[variables]:{                                 |

    namespace{
      std::atomic<ccp>___scratchBuffer{};
      std::atomic<u64>___scratchOffset{};
      #if e_compiling( mem_checks )
        std::atomic<u64>___mallocs{};
      #endif
    }

  //}:                                            |
  //[nedmalloc]:{                                 |

    #if e_compiling( ned_malloc )
      #include"nedmalloc/nedmalloc.c"
    #endif

  //}:                                            |
  //[rpmalloc]:{                                  |

    #if e_compiling( rp_malloc )
      #include"rpmalloc/rpmalloc/rpmalloc.h"
    #endif

  //}:                                            |
  //e_record:{                                    |

    namespace{

      #if !e_compiling( mem_checks ) && !e_compiling( mem_logger )
        e_forceinline
      #endif
      vp e_record( vp in_pBuffer, const size_t bytes, ccp pLabel ){

        //----------------------------------------------------------------------
        // Runtime sanity checking.
        //
        // TODO: In future e_unreachable should display alerts like the hoary
        // old fashione Guru Meditation on the Amiga.
        //----------------------------------------------------------------------

        if( !in_pBuffer ){
          e_unreachable( "Allocation failed!" );
        }

        //----------------------------------------------------------------------
        // Check the memory headers and footers to make sure thi block wasn't
        // overwritten by some rogue thread.
        //----------------------------------------------------------------------

        cp pResult = cp( in_pBuffer );
        #if e_compiling( mem_checks )
          u64& headerSize = *reinterpret_cast<u64*>( pResult );
          u32& headerMgc1 = *reinterpret_cast<u32*>( pResult + sizeof( u64 )     );
          u32& headerTIDs = *reinterpret_cast<u32*>( pResult + sizeof( u64 ) + 4 );
          u32& headerMgc2 = *reinterpret_cast<u32*>( pResult + bytes - 4 );
          headerTIDs = Thread::tid();
          headerSize = bytes;
          headerMgc1 = 0x14753817;
          headerMgc2 = 0x14753817;
          pResult += 16;
        #endif

        //----------------------------------------------------------------------
        // Log the allocation
        //----------------------------------------------------------------------

        #if e_compiling( mem_logger )
          e_logf( "[%4u]\t%p = %s( %p, %llu )", Thread::tid(), pResult, pLabel, in_pBuffer, bytes );
        #endif

        //----------------------------------------------------------------------
        // Increment the allocation count and return input pointer.
        //----------------------------------------------------------------------

        #if e_compiling( mem_checks )
          ++___mallocs;
        #endif
        return pResult;
      }
    }

  //}:                                            |
  //e_erase:{                                     |

    namespace{

      #if !e_compiling( mem_checks ) && !e_compiling( mem_logger )
        e_forceinline
      #endif
      vp e_erase( vp in_pBuffer, ccp pLabel ){

        //----------------------------------------------------------------------
        // Bail condtions.
        //----------------------------------------------------------------------

        if( !in_pBuffer ){
          return nullptr;
        }

        //----------------------------------------------------------------------
        // Check the memory headers and footers to make sure thi block wasn't
        // overwritten by some rogue thread.
        //----------------------------------------------------------------------

        #if e_compiling( mem_checks )
          cp   pResult    = cp( in_pBuffer )-16;
          u64& headerSize = *reinterpret_cast<u64*>( pResult );
          u32& headerMgc1 = *reinterpret_cast<u32*>( pResult + sizeof( u64 ));
          u32& headerMgc2 = *reinterpret_cast<u32*>( pResult + headerSize-4 );
          if( headerMgc1 != 0x14753817 ){
            e_unreachable( "Bad magic number" );
          }
          if( headerMgc2 != 0x14753817 ){
            e_unreachable( "Bad magic number" );
          }
        #else
          cp pResult = cp( in_pBuffer );
        #endif

        //----------------------------------------------------------------------
        // Log the allocation
        //----------------------------------------------------------------------

        #if e_compiling( mem_logger )
          #if e_compiling( mem_checks )
            e_logf( "[%4u]\t%s( %p->%p[ %llu ])", Thread::tid(), pLabel, in_pBuffer, pResult, headerSize );
          #else
            e_logf( "[%4u]\t%s( %p )", Thread::tid(), pLabel, in_pBuffer );
          #endif
        #endif
        #if e_compiling( mem_checks )
          memset( pResult, 0xDDDDDDDD, headerSize );
        #endif

        //----------------------------------------------------------------------
        // Increment the allocation count and return input pointer.
        //----------------------------------------------------------------------

        #if e_compiling( mem_checks )
          --___mallocs;
        #endif
        return pResult;
      }
    }

  //}:                                            |
//}:                                              |
//Operate:{                                       |

#if !e_compiling( win64 )
  vp operator new( std::size_t bytes ){
    #if e_compiling( mem_checks )
      bytes += 20;
    #endif
    #if e_compiling( ned_malloc )
      return e_record( nedalloc::nedmemalign( 16, bytes ), bytes, "new" );
    #elif e_compiling( rp_malloc )
      return e_record( rpmalloc(bytes ), bytes, "new" );
    #else
      return e_record( malloc( bytes ), bytes, "new" );
    #endif
  }

  vp operator new[]( std::size_t bytes ){
    #if e_compiling( mem_checks )
      bytes += 20;
    #endif
    #if e_compiling( ned_malloc )
      return e_record( nedalloc::nedmemalign( 16, bytes ), bytes, "new[]" );
    #elif e_compiling( rp_malloc )
      return e_record( rpmalloc(bytes ), bytes, "new" );
    #else
      return e_record( malloc( bytes ), bytes, "new[]" );
    #endif
  }

  vp operator new( std::size_t bytes, const std::nothrow_t& )noexcept{
    #if e_compiling( mem_checks )
      bytes += 20;
    #endif
    #if e_compiling( ned_malloc )
      return e_record( nedalloc::nedmemalign( 16, bytes ), bytes, "new" );
    #elif e_compiling( rp_malloc )
      return e_record( rpmalloc( bytes ), bytes, "new" );
    #else
      return e_record( malloc( bytes ), bytes, "new" );
    #endif
  }

  vp operator new[]( std::size_t bytes, const std::nothrow_t& )noexcept{
    #if e_compiling( mem_checks )
      bytes += 20;
    #endif
    #if e_compiling( ned_malloc )
      return e_record( nedalloc::nedmemalign( 16, bytes ), bytes, "new[]" );
    #elif e_compiling( rp_malloc )
      return e_record( rpmalloc( bytes ), bytes, "new" );
    #else
      return e_record( malloc( bytes ), bytes, "new[]" );
    #endif
  }

  void operator delete( vp ptr )noexcept{
    #if e_compiling( ned_malloc )
      nedalloc::nedfree( e_erase( ptr, "delete" ));
    #elif e_compiling( rp_malloc )
      rpfree( e_erase( ptr, "delete" ));
    #else
      free( e_erase( ptr, "delete" ));
    #endif
  }

  void operator delete[]( vp ptr )noexcept{
    #if e_compiling( ned_malloc )
      nedalloc::nedfree( e_erase( ptr, "delete[]" ));
    #elif e_compiling( rp_malloc )
      rpfree( e_erase( ptr, "delete" ));
    #else
      free( e_erase( ptr, "delete[]" ));
    #endif
  }

  void operator delete( vp ptr, const std::nothrow_t& )noexcept{
    #if e_compiling( ned_malloc )
      nedalloc::nedfree( e_erase( ptr, "delete" ));
    #elif e_compiling( rp_malloc )
      rpfree( e_erase( ptr, "delete" ));
    #else
      free( e_erase( ptr, "delete" ));
    #endif
  }

  void operator delete[]( vp ptr, const std::nothrow_t& )noexcept{
    #if e_compiling( ned_malloc )
      nedalloc::nedfree( e_erase( ptr, "delete[]" ));
    #elif e_compiling( rp_malloc )
      rpfree( e_erase( ptr, "delete" ));
    #else
      free( e_erase( ptr, "delete[]" ));
    #endif
  }

#endif

//}:                                              |
//Globals:{                                       |
  //[scratch]:{                                   |
    //e_scratched:{                               |

      bool e_scratched( ccp ptr ){
        if( ptr ){
          return( ptr >= ___scratchBuffer )&&( ptr < ___scratchBuffer+MAX_SCRATCH_MEMORY );
        }
        return false;
      }

    //}:                                          |
    //e_scratch:{                                 |

      ccp e_scratch(){
        return ___scratchBuffer;
      }

    //}:                                          |
    //e_salloc:{                                  |

      cp e_salloc( const u64 bytes ){
        if( !___scratchBuffer.load() ){
          #if e_compiling( ned_malloc )
            ___scratchBuffer.store( ccp( e_record( nedalloc::nedmemalign( 16, MAX_SCRATCH_MEMORY ), MAX_SCRATCH_MEMORY, "e_salloc" )));
          #else
            ___scratchBuffer.store( ccp( e_record( malloc( MAX_SCRATCH_MEMORY ), MAX_SCRATCH_MEMORY, "e_salloc" )));
          #endif
        }
        u64 size = ___scratchOffset.fetch_add( bytes );
        if( size + bytes > MAX_SCRATCH_MEMORY ){
          ___scratchOffset.store( 0 );
          size = 0;
        }
        ccp pEndBuffer = ___scratchBuffer + size + bytes;
        ccp pBegBuffer = ___scratchBuffer + size;
        if( pEndBuffer > ___scratchBuffer + MAX_SCRATCH_MEMORY ){
          e_unreachable( "Too big!" );
        }
        return cp( pBegBuffer );
      }

    //}:                                          |
    //e_sreset:{                                  |

      void e_sreset(){
        ___scratchOffset.store( 0 );
      }

    //}:                                          |
  //}:                                            |
  //[aligned]:{                                   |
    //e_pagesize:{                                |

      u32 e_pagesize(){
        #if e_compiling( win64 )
          SYSTEM_INFO sysInfo;
          GetSystemInfo( &sysInfo );
          return u32( sysInfo.dwPageSize );
        #else
          return u32( sysconf(_SC_PAGE_SIZE ));
        #endif
      }

    //}:                                          |
    //e_amalloc:{                                 |

      vp e_amalloc( const size_t size, const size_t align ){
        #if e_compiling( web )
          return aligned_alloc( size, align );
        #elif e_compiling( rp_malloc )
          return rpaligned_alloc( align, size );
        #else
          #if e_compiling( win64 )
            return _aligned_malloc( size, align );
          #elif e_compiling( osx )||e_compiling( ios )||e_compiling( linux )
            cp pResult = nullptr;
            switch( posix_memalign( (vp*)&pResult, align, size )){
              case EINVAL:
                e_unreachable( "EINVAL!" );
              case ENOMEM:
                e_unreachable( "ENOMEM!" );
              default:
                return pResult;
            }
          #else
            return aligned_alloc( size, align );
          #endif
        #endif
      }

    //}:                                          |
    //e_afree:{                                   |

      void e_afree( cvp ptr ){
        #if e_compiling( web )
          free( vp( ptr ));
        #elif e_compiling( rp_malloc )
          return rpfree( vp( ptr ));
        #elif e_compiling( win64 )
          _aligned_free( vp( ptr ));
        #elif e_compiling( osx )||e_compiling( ios )||e_compiling( linux )
          free( vp( ptr ));
        #else
          aligned_free( vp( ptr ));
        #endif
      }

    //}:                                          |
  //}:                                            |
  //[general]:{                                   |
    //e_realloc:{                                 |

      cp e_realloc( cvp in_pBuffer, const size_t in_resize ){
        #if e_compiling( mem_checks )
          const size_t bytes = in_resize + 20;
          #if e_compiling( ned_malloc )
            return cp( e_record( nedalloc::nedrealloc2( in_pBuffer ? cp( in_pBuffer )-16 : nullptr, bytes, 16, 0 ), bytes, "e_realloc" ));
          #else
            return cp( e_record( realloc( in_pBuffer ? cp( in_pBuffer )-16 : nullptr, bytes ), bytes, "e_realloc" ));
          #endif
        #else
          const size_t bytes = in_resize;
          #if e_compiling( ned_malloc )
            return cp( e_record( nedalloc::nedrealloc( vp( in_pBuffer ), bytes ), bytes, "e_realloc" ));
          #elif e_compiling( rp_malloc )
            return cp( e_record( rprealloc( vp( in_pBuffer ), bytes ), bytes, "e_realloc" ));
          #else
            return cp( e_record( realloc( vp( in_pBuffer ), bytes ), bytes, "e_realloc" ));
          #endif
        #endif
      }

    //}:                                          |
    //e_malloc:{                                  |

      cp e_malloc( size_t in_size ){
        #if e_compiling( mem_checks )
          const size_t bytes = in_size + 20;
        #else
          const size_t bytes = in_size;
        #endif
        #if e_compiling( ned_malloc )
          cvp pBuffer=nedalloc::nedmemalign( 16, bytes );
        #elif e_compiling( rp_malloc )
          cvp pBuffer=rpmalloc( bytes );
        #else
          cvp pBuffer=::malloc( bytes );
        #endif
        if( pBuffer ){
          return cp( e_record( cp( pBuffer ), bytes, "e_malloc" ));
        }
        e_unreachable( "Couldn't allocate bytes!" );
      }

    //}:                                          |
    //e_free:{                                    |

      void e_free( cvp pBuffer ){
        if( pBuffer ){
          #if e_compiling( ned_malloc )
            nedalloc::nedfree( e_erase( vp( pBuffer ), "e_free" ));
          #elif e_compiling( rp_malloc )
            rpfree( vp( pBuffer ));
          #else
            free( e_erase( vp( pBuffer ), "e_free" ));
          #endif
        }
      }

    //}:                                          |
  //}:                                            |
  //[sanity]:{                                    |
    //e_memory_validator:{                        |

      void e_memory_validator(){
        /*#if e_compiling( mem_checks )
          e_guardr(___alloclock );
          if(___allocated ){
            ___allocated.load()->foreach( []( ccp pBlock ){
              cp pHeader = cp( pBlock ) - 16;
              // Grab metrics from header.
              size_t& headerSize = *reinterpret_cast<size_t*>( pHeader );
              u32&    headerMgk1 = *reinterpret_cast<u32   *>( pHeader + sizeof( size_t ));
              u32&    headerMgk2 = *reinterpret_cast<u32   *>( pHeader + headerSize - 4 );
              // Sanity check magic id.
              e_assert( headerMgk1 == 0x14753817 );
              e_assert( headerMgk2 == 0x14753817 );
            });
          }
        #endif*/
      }

    //}:                                          |
    //e_isbadrptr:{                               |

      bool e_isbad( cvp ptr ){
        if( !ptr ){
          return false;
        }
        if( e_scratched( ccp( ptr ))){
          return false;
        }
        #if e_compiling( sanity )
          #if e_compiling( win64 )
            return( TRUE == IsBadReadPtr( ptr, 1 ));
          #elif e_compiling( osx )
            vm_size_t size;
            struct vm_region_basic_info info;
            mach_msg_type_number_t count;
            mach_port_t objectName = MACH_PORT_NULL;
            vm_address_t address = vm_address_t( ptr );
            count = VM_REGION_BASIC_INFO_COUNT;
            kern_return_t result = vm_region_64( mach_task_self()
                , &address
                , &size
                , VM_REGION_BASIC_INFO
                , vm_region_info_t( &info )
                , &count
                , &objectName );
            if( objectName != MACH_PORT_NULL ){
              mach_port_deallocate( mach_task_self(), objectName );
            }
            if(( result != KERN_SUCCESS )||( address > vm_address_t( ptr ))||( info.protection == VM_PROT_NONE )){
              return false;
            }
          #endif
        #endif
        return false;
      }

    //}:                                          |
    //e_memrand:{                                 |

      void e_memrand( cp ptr, const size_t size ){
        for( size_t i=0; i<size; ++i ){
          *ptr++ = s8( e_rand<u32>( 0, 255 ) & 0xFF );
        }
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================+=============================
