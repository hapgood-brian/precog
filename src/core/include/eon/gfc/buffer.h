//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

/** \addtogroup engine
  * @{
  *   \addtogroup gfc
  *   @{
  *     \addtogroup memory
  *     @{
  */

//================================================+=============================
//lockable:{                                      |

  namespace EON{

    namespace gfc{

      /** Generic lockable object.
        *
        * A non-POD structure containing a data pointer and a size.
        */

      struct E_PUBLISH lockable{

        //----------------------------------------+-----------------------------
        //Operate:{                               |

          template<typename T> e_noinline lockable& operator<<( const T& in ){
            // We cannot assign 'in' to realloc value because of alignment issues.
            auto pRealloc = realloc( sizeof( T ));
            if( !pRealloc ){
              return *this;
            }
            memcpy( pRealloc, &in, sizeof( T ));
            return *this;
          }

          template<typename T> e_noinline lockable& operator>>( T& out ){
            if(( m_uTail + sizeof( T )) > ( m_uMaximum * m_uStride ))
              return *this;
            if( !m_pData )
              return *this;
            cp pBlk = m_pData + m_uTail;
            m_uTail += sizeof( T );
            // We cannot assign 'out' to realloc value because of alignment issues.
            memcpy( &out, pBlk, sizeof( T ));
            return *this;
          }

          lockable& operator=( const lockable& lvalue );
          lockable& operator=( lockable&& rvalue );

        //}:                                      |
        //Methods:{                               |

          /** \name Locking mechanism.
            *
            * These methods allow the user to lock the buffer and assign a
            * pointer to the m_pData member.
            *
            * @{
            */

          /** \brief Pure lock interface.
            *
            * This routine must lock the buffer and set the size to zero. After
            * the lock m_pData will be assigned a value.
            *
            * \param writeOnly True if you want the buffer to be writable
            * only.  This hint is not supported by all objects.
            */

          e_noinline bool lock( const bool writeOnly=true ){
            if( !m_pData ){
              m_pData = lock_impl( writeOnly );
            }
            if( m_pData ){
              bLocked = 1;
              return true;
            }
            return false;
          }

          /** \brief Pure unlock interface.
            *
            * This routine will unlock the buffer. Afterwards the data member
            * will be set to nullptr and the size to the number of elements
            * that were actually written.
            */

          e_noinline void unlock(){
            if( m_pData ){
              bLocked = 0;
              unlk_impl();
            }
          }

          /** @}
            *
            * \name Freezing mechanism.
            *
            * These routines freezes the current allocations, making the buffer
            * return that figure instead of the total capacity. This is used in
            * the renderer for batched partial geometries like sprite lists and
            * lines.
            *
            * @{
            */

          /** \brief Freeze the buffer.
            *
            * This routine will freeze the buffer where it is and unlock the
            * data.
            */

          e_forceinline void freeze(){
            m_uMaximum = m_uCount;
            m_uCount = pending();
          }

          /** \brief Thaw the lockable.
            *
            * This routine will thaw or unfreeze the capacity, returning the
            * lockable buffer to a state where you can allocate again in it.
            */

          e_forceinline void thaw(){
            m_uCount = m_uMaximum;
          }

          /** \brief Get the capacity.
            *
            * This routine will return the allocated capacity.
            */

          e_forceinline u64 size()const{
            return m_uCount;
          }

          /** \brief Get number of pending elements.
            *
            * This routine returns the number of pending elements in the
            * lockable buffer.
            */

          e_forceinline u64 pending()const{
            if( !m_uStride ){
              return 0;
            }
            const auto sanity=( m_uTail % m_uStride );
            if( !sanity ){
              return( m_uTail / m_uStride );
            }
            return 0;
          }

          /** \brief Get the frozen capacity.
            *
            * This routine will return the allocated capacity.
            */

          e_forceinline u64 capacity()const{
            return m_uMaximum;
          }

          /** \brief Get the stride for this lockable.
            *
            * This routine returns the size in bytes of each element in the
            * lockable.  By default the stride is 1. In this case size() is the
            * actual number of bytes allocated by the lockable.
            *
            * \return Returns the number of bytes per element in the lockable.
            */

          e_forceinline u64 stride()const{
            return m_uStride;
          }

          /** \brief Set the capacity.
            *
            * This routine will set the overall capacity of the lockable buffer.
            */

          e_forceinline void setCapacity( const u64 capacity ){
            m_uCount = m_uMaximum = capacity;
          }

          /** \brief Set the tail index.
            *
            * This routine will set the current tail by multiplying index by
            * the lockable stride.
            *
            * \param i The element to set the tail to.
            */

          e_forceinline void setTail( const u64 i ){
            m_uTail = i * m_uStride;
          }

          /** @}
            *
            * \name Accessors.
            *
            * These member functions return various metrics about the lockable.
            *
            * @{
            */

          /** \brief Check for potential overrun.
            *
            * This routine checks to see if the present tail plus the stride
            * would overrun the capacity.
            *
            * \param n The number of elements to allocate, the total number of
            * bytes being n * stride().
            *
            * \return Returns true if the current number of allocations plus n
            * is greater than the buffer size. False if it's safe to add
            * another n elements in other words.
            */

          e_forceinline bool overrun( const u64 n )const{
            return( pending() + n > m_uCount );
          }

          /** \brief Get the number of free elements.
            *
            * This routine will return the number of free elements, which is
            * the capacity - the size.
            *
            * \return Returns the available bytes.
            */

          e_forceinline u64 avail()const{
            return( m_uCount - pending() );
          }

          /** \brief Get the number of allocated bytes.
            *
            * This routine will return the true number of bytes allocated by
            * the buffer object.
            */

          e_forceinline u64 bytes()const{
            return m_uCount * m_uStride;
          }

          /** \brief Get the start of the lockable.
            *
            * This routine will return a pointer to the start of the allocated
            * lockable. If the lockable is unallocated it will assert.
            *
            * \return Returns m_pData if the assertion passes.
            */

          e_forceinline ccp data()const{
            return m_pData;
          }

          /** \brief Get the start of the lockable.
            *
            * This routine will return a pointer to the start of the allocated
            * lockable. If the lockable is unallocated it will assert.
            *
            * \return Returns m_pData if the assertion passes.
            */

          e_forceinline cp data(){
            return m_pData;
          }

          /** \brief Get the end of the lockable.
            *
            * This routine will return a pointer to the physical end of the
            * lockable.
            *
            * \return A pointer to the end of the lockable.
            */

          e_forceinline ccp end()const{
            return m_pData + bytes();
          }

          /** \brief Get the end of the lockable.
            *
            * This routine will return a pointer to the physical end of the
            * lockable.
            *
            * \return A pointer to the end of the lockable.
            */

          e_forceinline cp end(){
            return m_pData + bytes();
          }

          /** @{
            *
            * \name Allocation methods.
            *
            * These methods control and report on the lockable buffer's
            * allocations and such.
            *
            * @{
            */

          /** \brief Checks if the lockable is empty.
            *
            * There are two cases when the lockable is said to be empty. If the
            * data pointer is null or the size is zero. Either case will cause
            * this routine to return true.
            *
            * \return Returns true if the data pointer is null or the size is
            * zero, and false otherwise.
            */

          e_forceinline bool empty()const{
            return( !m_uCount );
          }

          /** \brief Reallocate the lockable.
            *
            * This routine will reallocate the lockable and give it the new
            * size.
            *
            * \param deltaBytes The number of elements to allocate. Each
            * element will be stride bytes. It will add to the existing size
            * not replace it.
            *
            * \return Returns a pointer to the start of the newly allocated
            * ram. If the lockable contained 200 bytes of memory and you called
            * realloc(10) then the pointer returned would be &m_pData[200].
            */

          cp realloc( const u64 deltaBytes );

          /** \brief Allocate the lockable with size.
            *
            * This routine will allocate the lockable with size bytes.
            *
            * \param bytes The number of bytes to allocate for the new lockable
            * saved in m_pData pointer.
            *
            * \return Returns a pointer to the start of the newly allocated
            * ram.
            */

          cp alloc( const u64 bytes );

          /** \brief Reset the read head.
            *
            * This routine will reset the read head.
            */

          e_forceinline void reset(){
            m_uTail = 0;
          }

          /** \brief Clear the buffer.
            *
            * This routine will clear the buffer.
            */

          void clear(){
            e_free( m_pData );
            m_pData = nullptr;
            reset();
          }

          /** \brief Check if in GPU memory.
            *
            * This routine will return true if the lock allocated from GPU mem.
            */

          bool isInGpuMem()const{
            return( 1 == bGpuMem );
          }

          /** \brief Check if locked.
            *
            * This routine returns true if the lockable is locked.
            */

          bool isLocked()const{
            return( 1 == bLocked );
          }

          /** \brief Check if write only.
            *
            * This routine will return true if the write only bit is set, and
            * false otherwise.
            */

          bool isWriteOnly()const{
            return( 1 == bWriteOnly );
          }

          /** \brief Set write only bit.
            *
            * This routine will set the write only bit.
            */

          void setWriteOnly(){
            bWriteOnly = 1;
          }

          /** \brief Clear the write only bit.
            *
            * This routine will clear out the write only (or write combined)
            * bit.
            */

          void clearWriteOnly(){
            bWriteOnly = 0;
          }

          /** @} */

        //}:                                      |
        //----------------------------------------+-----------------------------

        lockable( const u64 stride, const u64 size );
        lockable( const lockable& lvalue );
        lockable( lockable&& rvalue );
        virtual~lockable();
        lockable() = default;

      protected:

        union{
          u32 all = 0;
          struct{
            u32 bWriteOnly:1;
            u32 bGpuMem:1;
            u32 bLocked:1;
            u32 bBound:1;
          };
        };

      private:

        virtual cp   lock_impl( const bool bWriteOnly )=0;
        virtual void unlk_impl()=0;

        e_var( u64, u, Maximum ) = 0;
        e_var( u64, u, Stride  ) = 1;
        e_var( u64, u, Count   ) = 0;
        e_var(  cp, p, Data    ) = nullptr;

        u64 m_uTail = 0;
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//buffer:{                                        |

  namespace EON{

    namespace gfc{

      /** Generic buffer object.
        *
        * A non-POD structure containing a data pointer and a size.
        */

      struct E_PUBLISH buffer final{

        //----------------------------------------+-----------------------------
        //Operate:{                               |

          buffer& operator=( const buffer& lvalue );
          buffer& operator=( buffer&& rvalue );

        //}:                                      |
        //Methods:{                               |

          /** \brief Get the number of allocated bytes.
            *
            * This routine will return the true number of bytes allocated by
            * the buffer object.
            */

          e_forceinline u64 bytes()const{
            return u64( m_uSize ) * u64( m_uStride );
          }

          /** \brief Get the start of the buffer.
            *
            * This routine will return a pointer to the start of the allocated
            * buffer. If the buffer is unallocated it will assert.
            *
            * \return Returns m_pData if the assertion passes.
            */

          e_forceinline ccp begin()const{
            return m_pData;
          }

          /** \brief Get the start of the buffer.
            *
            * This routine will return a pointer to the start of the allocated
            * buffer. If the buffer is unallocated it will assert.
            *
            * \return Returns m_pData if the assertion passes.
            */

          e_forceinline cp begin(){
            return m_pData;
          }

          /** \brief Get the end of the buffer.
            *
            * This routine will return a pointer to the physical end of the
            * buffer.
            *
            * \return A pointer to the end of the buffer.
            */

          e_forceinline ccp end()const{
            return m_pData + bytes();
          }

          /** \brief Get the end of the buffer.
            *
            * This routine will return a pointer to the physical end of the
            * buffer.
            *
            * \return A pointer to the end of the buffer.
            */

          e_forceinline cp end(){
            return m_pData + bytes();
          }

          /** \brief Clears the data members only.
            *
            * This routine will clear m_pData by deleting the memory pointed to
            * by it and setting the pointer to nullptr.
            */

          void clearDataOnly();

          /** \brief Clears the data and sets size to zero.
            *
            * This routine will clear m_pData by deleting the memory pointed to
            * by it and sets both the pointer and size to zero.
            */

          void clear();

          /** \brief Checks if the buffer is empty.
            *
            * There are two cases when the buffer is said to be empty. If the
            * data pointer is null or the size is zero. Either case will cause
            * this routine to return true.
            *
            * \return Returns true if the data pointer is null or the size is
            * zero, and false otherwise.
            */

          e_forceinline bool empty()const{
            return( nullptr == m_pData )||!m_uSize;
          }

          /** \brief Reallocate the buffer.
            *
            * This routine will reallocate the buffer and give it the new size.
            *
            * \param size The number of elements to allocate. Each element will
            * be stride bytes.
            *
            * \return Returns true if the buffer was reallocated or false. In
            * the latter case it's usually because the data was donated.
            */

          bool realloc( const u32 size );

          /** \brief Allocate the buffer with size.
            *
            * This routine will allocate the buffer with size bytes.
            *
            * \param size The number of bytes to allocate for the new buffer
            * saved in m_pData pointer.
            */

          void alloc( const u32 size );

          /** \brief Get the size of the buffer in stride sized blocks.
            *
            * This routine will return the number of items of stride size in
            * this buffer.
            *
            * \return Returns the number of elements in the buffer. Each
            * element is exactly stride bytes in length.
            */

          e_forceinline u32 size()const{
            return m_uSize;
          }

          /** \brief Get the stride for this buffer.
            *
            * This routine returns the size in bytes of each element in the
            * buffer.  By default the stride is 1. In this case size() is the
            * actual number of bytes allocated by the buffer.
            *
            * \return Returns the number of bytes per element in the buffer.
            */

          e_forceinline u32 stride()const{
            return m_uStride;
          }

          /** \brief Set the buffer stride.
            *
            * This routine will set the buffer stride in bytes. If the buffer
            * has already been allocated this routine will assert out. At
            * runtime we return true if this is not the case or false.
            *
            * \param stride The number of bytes per element pointed to by
            * m_pData.
            */

          e_forceinline void setStride( const u32 stride ){
            m_uStride = stride;
            if( m_pData ){
              e_unreachable( "You cannot change the stride of an allocated buffer!" );
            }
          }

          /** \brief Get a pointer to the allocated data.
            *
            * This routine returns a pointer to the allocated (or not) data.
            *
            * \return Returns a character pointer to the data so you can do
            * useful pointer arithmetic with the result. This is especially
            * useful when using vertex buffer objects (Submesh::VBO).
            */

          e_forceinline ccp data()const{
            return m_pData;
          }

          /** \brief Get a pointer to the allocated data.
            *
            * This routine returns a pointer to the allocated (or not) data.
            *
            * \return Returns a character pointer to the data so you can do
            * useful pointer arithmetic with the result. This is especially
            * useful when using vertex buffer objects (Submesh::VBO).
            */

          e_forceinline cp data(){
            return m_pData;
          }

          /** \brief Set the data and size in one call.
            *
            * This routine will let you change the allocated data pointer by
            * hand and the size member in one call. This is for cases when you
            * want a buffer to point to an array on the stack, render it, and
            * then set it back to nullptr again after the call.
            *
            * \param pData A temporary pointer to assign to the buffer.
            *
            * \param nData The number of elements of stride() bytes. This is
            * what a call to size() will return afterwards.
            */

          e_forceinline void set( cp pData, const u32 nData ){
            m_pData = pData;
            m_uSize = nData;
            m_bDonated = 1;
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        buffer( const u32 stride, const u32 size );
        explicit buffer( const u32 size );
        buffer( const buffer& lvalue );
        buffer( buffer&& rvalue );
        buffer() = default;
        virtual~buffer();

      private:

        cp  m_pData   = nullptr;
        u32 m_uSize   = 0;
        u32 m_uStride = 1;
        union{
          u32 all = 0;
          struct{
            u32 m_bDonated:1;
          };
        };
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//stream:{                                        |

  namespace EON{

    namespace gfc{

      /** Generic stream object.
        *
        * A non-POD structure containing a data pointer and a size.
        */

      struct E_PUBLISH stream{

        constexpr static u64 PARAGRAPH = 4;

        //----------------------------------------+-----------------------------
        //Operate:{                               |

          stream& operator=( const stream& lvalue );
          stream& operator=( stream&& rvalue );

          stream& operator<<( const string& );
          stream& operator<<( ccp );

          /** \brief Equality operator.
            *
            * This operator compares certain members of this and the input
            * streams then the data itself.
            *
            * \param st The stream to compare this against.
            *
            * \return Returns true if the two streams are identical save their
            * capacities, false if they're not.
            */

          e_noinline bool operator==( const stream& st )const{
            e_guardr( m_tLock );
            if( this == &st ){
              return true;
            }
            if( empty() || st.empty() ){
              return false;
            }
            if( m_uStride != st.m_uStride ){
              return false;
            }
            if( m_uSize != st.m_uSize ){
              return false;
            }
            return !memcmp( m_pData, st.m_pData, size_t( bytes() ));
          }

          /** \brief Inequality operator.
            *
            * This routine will compare two streams for inequality.
            *
            * \param st The stream to comprae this against.
            *
            * \return Returns true if the two streams are different and false
            * otherwise.
            */

          e_forceinline bool operator!=( const stream& st )const{
            e_guardr( m_tLock );
            return !( *this == st );
          }

        //}:                                      |
        //Methods:{                               |

          /** \brief Return a const pointer to the data.
            *
            * This routine returns a const pointer to the data of type ccp.
            *
            * \return Returns a ccp to the data. It's a string.
            */

          e_forceinline ccp data()const{
            return m_pData;
          }

          /** \brief Check if donated.
            *
            * This routine will return true if the data was donated or false.
            */

          e_forceinline bool donated()const{
            e_guardr( m_tLock );
            return( 1 == bDonated );
          }

          /** \brief Get the number of free elements.
            *
            * This routine will return the number of free elements, which is
            * the capacity - the size.
            *
            * \return Returns a 64-bit value representing available space in
            * the stream.
            */

          e_forceinline u64 avail()const{
            e_guardr( m_tLock );
            return m_uCapacity - 1 - m_uSize;
          }

          /** \brief Get the number of allocated bytes.
            *
            * This routine will return the true number of bytes allocated by
            * the buffer object.
            *
            * \return Returns the number of bytes allocated in this stream.
            */

          e_forceinline u64 bytes()const{
            e_guardr( m_tLock );
            return m_uSize * m_uStride;
          }

          /** \brief Clears the data members only.
            *
            * This routine will clear m_pData by deleting the memory pointed to
            * by it and setting the pointer to nullptr.
            */

          void clearDataOnly();

          /** \brief Clears the data and sets size to zero.
            *
            * This routine will clear m_pData by deleting the memory pointed to
            * by it and sets both the pointer and size to zero.
            */

          void clear();

          /** \brief Checks if the stream is empty.
            *
            * There are two cases when the stream is said to be empty. If the
            * data pointer is null or the size is zero. Either case will cause
            * this routine to return true.
            *
            * \return Returns true if the data pointer is null or the size is
            * zero, and false otherwise.
            */

          e_forceinline bool empty()const{
            e_guardr( m_tLock );
            return( nullptr == m_pData )|| !m_uSize;
          }

          /** \brief Reallocate the stream.
            *
            * This routine will reallocate the stream and give it the new size.
            *
            * \param deltaSize The number of elements to allocate. Each element
            * will be stride bytes. It will add to the existing size not
            * replace it.
            *
            * \return Returns a pointer to the start of the newly allocated
            * ram. If the stream contained 200 bytes of memory and you called
            * realloc(10) then the pointer returned would be &m_pData[200].
            */

          cp realloc( const u64 deltaSize );

          /** \brief Allocate the stream with size.
            *
            * This routine will allocate the stream with size bytes.
            *
            * \param size The number of bytes to allocate for the new stream
            * saved in m_pData pointer.
            *
            * \return Returns a pointer to the start of the newly allocated ram.
            */

          cp alloc( const u64 size );

          /** \brief Skip so many items.
            *
            * This routine will skip a number of items in the stream.
            *
            * \param size The number of items to skip.
            *
            * \return Returns true if we didn't skip off the end of the stream.
            */

          bool skip( const u64 size );

          /** \brief Reset the read head.
            *
            * This routine will reset the read head.
            */

          e_forceinline bool seek( const u64 to ){
            e_guardw( m_tLock );
            if( to <= m_uCapacity ){
              m_uSize = to;
              return true;
            }
            return false;
          }

          /** \brief Reset the read head.
            *
            * This routine will reset the read head.
            */

          e_forceinline void reset(){
            e_guardw( m_tLock );
            m_uSize = 0;
          }

          /** \brief Get the capacity of this stream.
            *
            * This routine will return the total allocated capacity of the
            * stream.
            *
            * \return Returns the number of bytes reserved and allocated.
            */

          e_forceinline u64 capacity()const{
            return m_uCapacity;
          }

          /** \brief Get the size of the stream in stride sized blocks.
            *
            * This routine will return the number of items of stride size in
            * this stream.
            *
            * \return Returns the number of elements in the stream. Each
            * element is exactly stride bytes in length.
            */

          e_forceinline u64 size()const{
            e_guardr( m_tLock );
            return m_uSize;
          }

          /** \brief Get the stride for this stream.
            *
            * This routine returns the size in bytes of each element in the
            * stream.  By default the stride is 1. In this case size() is the
            * actual number of bytes allocated by the stream.
            *
            * \return Returns the number of bytes per element in the stream.
            */

          e_forceinline u64 stride()const{
            e_guardr( m_tLock );
            return m_uStride;
          }

          /** \brief Set the stream stride.
            *
            * This routine will set the stream stride in bytes. If the stream
            * has already been allocated this routine will assert out. At
            * runtime we return true if this is not the case or false.
            *
            * \param stride The number of bytes per element pointed to by m_pData.
            */

          e_forceinline void setStride( const u64 stride ){
            e_guardw( m_tLock );
            m_uStride = stride;
            if( m_pData ){
              e_unreachable( "You cannot change the stride of an allocated stream!" );
            }
          }

          /** \brief Get the end of the stream.
            *
            * This routine will return a pointer to the physical end of the
            * stream.
            *
            * \param lambda A callback function passed with the end of the
            * stream as a const char pointer.
            */

          void end( const std::function<void( ccp )>& lambda )const;

          /** \brief Get a pointer to the allocated data.
            *
            * This routine returns a character at the indexed location.
            *
            * \param i The index into the data we want to query.
            *
            * \param lambda The callback function to do the querying. It will
            * be passed a const char* to the offsetted data.
            *
            * \return Returns true if 'i' was valid, the lambda was non-null
            * and was called successfully.
            */

          bool query( const u64 i, const std::function<void( ccp )>& lambda )const;

          /** \brief Get a pointer to the allocated data.
            *
            * This routine returns a pointer to the allocated (or not) data.
            *
            * \param lambda The callback function to do the querying. It will
            * be passed a const char* to the offsetted data.
            *
            * \return Returns true if the lambda was non-null and was called
            * successfully.
            */

          bool query( const std::function<void( ccp )>& lambda )const;

          /** \brief Get a pointer to the allocated data.
            *
            * This routine returns a pointer to the allocated (or not) data.
            *
            * \param lambda The callback function to do the altering. It will
            * be passed a const char* to the offsetted data.
            *
            * \return Returns true if the lambda was non-null and was called
            * successfully.
            */

          bool alter( const std::function<void( cp )>& lambda );

          /** \brief Set the data and size in one call.
            *
            * This routine will let you change the allocated data pointer by
            * hand and the size member in one call. This is for cases when you
            * want a stream to point to an array on the stack, render it, and
            * then set it back to nullptr again after the call.
            *
            * \param p A temporary pointer to assign to the stream.
            *
            * \param z The number of elements of stride() bytes. This is what a
            * call to size() will return afterwards.
            */

          e_forceinline void set( cp p, const u64 z ){
            e_guardw( m_tLock );
            m_pData = p;
            m_uSize = z;
          }

          /** \brief Cast current position to T reference.
            *
            * This routine will cast the current read position to a reference
            * of type T. It is a templatized class.
            */

          template<typename T> T& cast(){
            return( *reinterpret_cast<T*>( realloc( sizeof( T ))));
          }

          /** \brief Return seek position.
            *
            * This routine will return the read/write head.
            */

          e_forceinline u64 tell()const{
            e_guardr( m_tLock );
            return m_uSize;
          }

          /** \brief Set the is partial flag.
            *
            * This routine will return the is partial flags flag. This is used
            * in the object history class to tell whether a stream is partial
            * or not or is delta and rleZ compressed.
            */

          e_forceinline void setIsPartial( const bool isPartial ){
            e_guardw( m_tLock );
            bPartial = isPartial;
          }

          /** \brief Return is partial flag.
            *
            * This routine returns the is partial flag.
            */

          e_forceinline bool isPartial()const{
            e_guardr( m_tLock );
            return( 1 == bPartial );
          }

          /** \brief Get the dual lock.
            *
            * This routine will return the stream's read-write (dual) lock.
            * With this you can make outer operations like appending a string
            * with cat completely atomic.
            */

          e_forceinline atomic::ShareLockRecursive& toLock()const{
            return m_tLock;
          }

          /** \brief Add ref.
            *
            * This routine will increment the reference count of this stream.
            */

          s32 addRef(){
            return ++m_aRefs;
          }

          /** \brief Sub ref.
            *
            * This routine will decrement the reference count of this stream.
            */

          s32 subRef(){
            return --m_aRefs;
          }

          s32 refs()const{
            return m_aRefs.load();
          }

          void setRefs( const s32 refs ){
            m_aRefs.store( refs );
          }

        //}:                                      |
        //----------------------------------------+-----------------------------

        stream( cp pBuffer, const u64 stride, const u64 capacity );
        stream( const stream& lvalue );
        stream( stream&& rvalue );
        virtual~stream();
        stream() = default;

      private:

        friend string;
        union{
          struct{
            u32 bDonated:1;
            u32 bPartial:1;
          };
          u32 all = 0;
        };
        u64                                m_uCapacity = 0;
        u64                                m_uStride   = 1;
        u64                                m_uSize     = 0;
        cp                                 m_pData     = nullptr;
        mutable atomic::ShareLockRecursive m_tLock;
        mutable std::atomic<s32>           m_aRefs{1};
      };
    }
  }

//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
