//------------------------------------------------------------------------------
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// Published under the GPL3 license; see LICENSE for more information.
//------------------------------------------------------------------------------

#pragma once

#include<cstdarg>
#include<string>
#include<regex>

/** \addtogroup engine
  * @{
  *   \addtogroup gfc
  *   @{
  *     \addtogroup strings
  *     @{
  */

//================================================+=============================
//String:{                                        |

  namespace EON{

    namespace gfc{

      /** Generic string class
        *
        * For basic string operations use the string object. It will certainly
        * grow over time. This class breaks our naming convention. All classes
        * must begin with an uppercase letter. gfc::string is obviously bad
        * and/or very naughty. If the case insensitive bit is not set in the
        * flags then the hash of the string will be computed every time cat()
        * is called.
        */

      struct E_PUBLISH String{

        //----------------------------------------+-----------------------------
        //Aliases:{                               |

          template<u32 N> using pairs = array<std::pair<String,String>,N>;

        //}:                                      |
        //Structs:{                               |

          /** \brief String builder.
            *
            * This class defines the temporary string builder interface. With
            * it you can create string objects from thousands of little pieces
            * without blocking any threads. It's completely thread safe. Note
            * that the string builder uses scratch memory.
            */

          struct Factory{

            //------------------------------------+-----------------------------
            //Operate:{                           |

              e_forceinline Factory& operator<<( const Factory& value ){
                cat( value.e_str() );
                return *this;
              }

              e_forceinline Factory& operator<<( const String& value ){
                cat( value );
                return *this;
              }

              e_forceinline Factory& operator<<( ccp pValue ){
                cat( pValue );
                return *this;
              }

            //}:                                  |
            //Methods:{                           |

              /** \brief Clear out the factory; reset size.
                *
                * This routine quickly clears out the factory buffer.
                */

              e_forceinline void clear(){
                m_nSize = 0;
              * m_pData = 0;
              }

              /** \brief Return whether the factory is empty or not.
                *
                * This routine will check to see if the factory has any text
                * added to it and if so returns true.
                *
                * \return Returns true if the factory is empty and false
                * otherwise.
                */

              e_forceinline bool empty()const{
                return!m_nSize;
              }

              /** \brief Return C string from the factory.
                *
                * This routine will return the C string version of the factory.
                *
                * \return Returns a C string.
                */

              e_forceinline ccp c_str()const{
                return reinterpret_cast<ccp>( m_pData );
              }

              /** \brief Return std::string from the factory.
                *
                * This routine will return a standard string from the factory
                * data. Remember that the data is transitory in nature, having
                * been allocated on the scratch pad.
                *
                * \return Returns a std::string.
                */

              e_forceinline std::string str()const{
                return c_str();
              }

              /** \brief Return gfc::string from the factory.
                *
                * This routine will return a fully hashed gfc string from the
                * factory data. After this call the factory can be thrown away
                * and it's data ignored. The string will be valid until the
                * end of the next frame.
                */

              e_forceinline string e_str()const{
                return c_str();
              }

              /** \brief Append a gfc string to the builder.
                *
                * This routine will append a string to the end of the string
                * builder object.
                *
                * \param s The gfc string to append.
                *
                * \return Returns *this.
                */

              e_forceinline Factory& cat( const string& s ){
                return cat( s.c_str() );
              }

              /** \brief Append a C string to the builder.
                *
                * This routine will append a C string to the end of the string
                * builder object.
                *
                * \param pString The C string to append.
                *
                * \return Returns *this.
                */

              e_noinline Factory& cat( ccp pString ){
                const auto nString = strlen( pString );
                auto* pData = reinterpret_cast<s8*>( e_salloc( m_nSize+nString+1 ));
                if( m_pData ){
                  memcpy(
                      cp( pData )
                    , ccp( m_pData )
                    , m_nSize
                  );
                }
                memcpy(
                    pData+m_nSize
                  , pString
                  , nString );
                pData[ m_nSize + nString ] = 0;
                m_nSize += u32( nString );
                m_pData = pData;
                return *this;
              }

              /** \brief Append a formatted C string to the builder.
                *
                * This routine will append a formatted C string to the end of
                * the string builder object.
                *
                * \param frm The formatted C string to append.
                *
                * \return Returns *this.
                */

              e_noinline Factory& catf( ccp frm,... ){
                va_list va;
                va_start( va, frm );
                  cp tmp = cp( e_salloc( 1024 ));
                  vsnprintf(
                      tmp
                    , 1024
                    , frm
                    , va );
                  cat( tmp );
                va_end( va );
                return *this;
              }

            //}:                                  |
            //------------------------------------+-----------------------------

          private:

            e_var_ptr( s8, Data ) = nullptr;
            e_var( u32, n, Size ) = 0;
          };

        //}:                                      |
        //Operate:{                               |

          /** \name Conversion operators.
            *
            * These operators allow you to convert the string to a number of
            * types just by casting it or assigning it to an object of that
            * type.
            *
            * @{
            */

          /** \brief Deference string like any normal C string.
            *
            * This operator will return the first character at the head of the
            * stream. This is not the same as dereferencing the return value
            * from the stream's data method either. The c_str() method returns
            * "" if the stream is empty, which of course stream will not.
            *
            * \return Returns the first character in the string or zero if the
            * string is empty.
            */

          e_forceinline char operator*()const{
            return *c_str();
          }

          /** String to C string operator.
            *
            * Converts a std type string to a C string.
            *
            * \returns A C string.
            */

          e_forceinline operator ccp()const{
            return c_str();
          }

          /** @}
            *
            * \name Virtual assignment operators.
            *
            * These operators provide means for assigning a variety of
            * different objects to the string.
            *
            * @{
            */

          /** \brief Assignment operator.
            *
            * This operator will assign a string object to this one.
            *
            * \param lvalue The string object to assign to this string.
            *
            * \return A reference to *this.
            */

          virtual String& operator=( const String& lvalue );

          /** \brief Move operator.
            *
            * This operator will assign a string object to this one. The
            * operator will do a move operation for extra speed so statements
            * like a+"b" are lighting fast and need no allocations.
            *
            * \param rvalue The string object to assign to this string.
            *
            * \return A reference to *this.
            */

          virtual String& operator=( String&& rvalue );

          /** \brief Assignment operator.
            *
            * This operator will assign a string object to this one.
            *
            * \param pValue The C string to assign to this string.
            *
            * \return A reference to *this.
            */

          virtual String& operator=( ccp pValue );

          /** @}
            *
            * \name Streaming concatenation operators.
            *
            * These operators will append various types to the end of the
            * string.
            *
            * @{
            */

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const String& value ){
            cat( value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const double value ){
            catf( "%lf", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const u64 value ){
            catf( "%llu", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const s64 value ){
            catf( "%lld", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const float value ){
            catf( "%f", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const u32 value ){
            catf( "%u", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( const s32 value ){
            catf( "%d", value );
            return *this;
          }

          /** \brief Streaming in operator.
            *
            * This operator will append the given data to the end of this
            * string.
            */

          e_forceinline String& operator<<( ccp value ){
            cat( value );
            return *this;
          }

          /** @}
            *
            * \name Concatenation (appending) operators.
            *
            * These operators will append a variety of values to the end of
            * this string object. They do this by converting the value to a
            * string and then calling cat (or catf) on it.
            *
            * @{
            */

          /** \brief Add operator.
            *
            * This operator will add a string object to this string as if you
            * had called this->cat( sValue ).
            *
            * \param value The string object to append to this string.
            *
            * \return A reference to the string buffer.
            */

          e_forceinline String& operator+=( const String& value ){
            cat( value );
            return *this;
          }

          /** \brief Add operator.
            *
            * This operator will concatenate a single character onto the end of
            * the string data in "m".
            *
            * \param chr The character to append.
            *
            * \return Returns the deferenced this pointer.
            */

          e_forceinline String& operator+=( const char chr ){
            const char as[2]{ chr, 0 };
            cat( as, 1 );
            return *this;
          }

          /** \brief Add operator.
            *
            * This routine will add a string object to this string as if you
            * had called this->cat( sValue ).
            *
            * \param pValue The C string to append to this string.
            *
            * \return A reference to the string buffer.
            */

          e_forceinline String& operator+=( ccp pValue ){
            cat( pValue );
            return *this;
          }

          /** \brief Plus operator.
            *
            * This routine will create a string from this string and s appended
            * to it. This string is not modified.
            *
            * \param s The string object to append.
            *
            * \return A new string which is the concatenation of this and s.
            */

          String operator+( const String& s )const;

          /** \brief Plus operator.
            *
            * This routine will create a string from this string and s appended
            * to it. This string is not modified.
            *
            * \param p The C string to append.
            *
            * \return A new string which is the concatenation of this and s.
            */

          String operator+( ccp p )const;

          /** @}
            *
            * \name Comparison operators.
            *
            * These operators allow us to compare two strings and see if they
            * are the same, less than or greater than one another.
            *
            * @{
            */

          /** \brief Less than operator.
            *
            * This operator compares this string against the string provided
            * using strcmp(). The case insensitivity flag is ignored by this
            * operator.
            *
            * \param s The string object to compare with.
            *
            * \return If strcmp() returns a value that's less than zero then
            * the return value is true otherwise it'll be false.
            */

          bool operator<( const String& s )const;

          /** \brief Less than operator.
            *
            * This operator compares this string against the string provided
            * using strcmp(). The case insensitivity flag is ignored by this
            * operator.
            *
            * \param p The C string to compare with.
            *
            * \return If strcmp() returns a value that's less than zero then
            * the return value is true otherwise it'll be false.
            */

          bool operator<( ccp p )const;

          /** \brief Greater than operator.
            *
            * This operator compares this string against the string provided
            * using strcmp(). The case insensitivity flag is ignored by this
            * operator.
            *
            * \param s The string object to compare with.
            *
            * \return If strcmp() returns a value that's greater than zero then
            * the return value is true otherwise it'll be false.
            */

          bool operator>( const String& s )const;

          /** \brief Greater than operator.
            *
            * This operator compares this string against the string provided
            * using strcmp(). The case insensitivity flag is ignored by this
            * operator.
            *
            * \param p The C string to compare with.
            *
            * \return If strcmp() returns a value that's greater than zero then
            * the return value is true otherwise it'll be false.
            */

          bool operator>( ccp p )const;

          /** \brief Inequality operator.
            *
            * This operator compares this string against the one provided. The
            * case sensitivity flag is used in this code.
            *
            * \param s The string object to compare against this string.
            *
            * \return Returns true if the two strings are identical if the case
            * insensitivity flag is false or if the two strings match in a case
            * insensitive way. Returns false if both of these checks fail.
            */

          bool operator!=( const String& s )const;

          /** \brief Inequality operator.
            *
            * This operator compares this string against the one provided. The
            * case sensitivity flag is used in this code.
            *
            * \param p The C string to compare against this string.
            *
            * \return Returns true if the two strings are identical if the case
            * insensitivity flag is false or if the two strings match in a case
            * insensitive way. Returns false if both of these checks fail.
            */

          bool operator!=( ccp p )const;

          /** Equality operator.
            *
            * Test the equality of this string object with the input c string.
            *
            * \param s Input string to test.
            *
            * \return Returns true if two string are equal and false otherwise.
            */

          bool operator==( const String& s )const;

          /** Equality operator.
            *
            * Test the equality of this string object with the input c string.
            *
            * \param p Input string to test.
            *
            * \return Returns true if two string are equal and false otherwise.
            */

          bool operator==( ccp p )const;

          /** Equality operator.
            *
            * Test the equality of this string object with the input c string.
            *
            * \param h Input hashed string to test.
            *
            * \return Returns true if two string are equal and false otherwise.
            */

          bool operator==( const u64 h )const{
            return( hash() == h );
          }

          /** @} */

        //}:                                      |
        //Methods:{                               |
          //streamId:{                          |

            /** \brief Generate a unique stream ID.
              *
              * This routine will scan through all streams checking for an id
              * as it builds. The previous String will be obliterated.
              */

            static String streamId();
            static String guid();

          //}:                                    |
          //scrubf:{                              |

            String  scrubf()const;
            String& scrubf();

          //}:                                    |
          //clone:{                               |

            e_forceinline String clone()const{
              return String( c_str() );
            }

          //}:                                    |
          //skip*:{                               |

            static cp skip_anynonws( ccp s );
            static cp skip_nonws( ccp s );
            static cp skip_anyws( ccp s );
            static cp skip_2eol( ccp s );
            static cp skip_eols( ccp s );
            static cp skip_ws( ccp s );
            static cp skip_number( ccp s );
            static cp skip_cc( ccp s );
            static cp skip_str( ccp s );

          //}:                                    |
          //is*:{                                 |

            static bool is_upper( const char c );
            static bool is_lower( const char c );
            static bool is_eol  ( const char c );

            /** \brief Test if input is an alpha character.
              *
              * This parsing helper routine will test if the input character is
              * an alpha character based on the ASCII table. That's any upper
              * or lower case letter in the range A-Z or a-z.
              *
              * \param c The character to test to see if it's an alphabetic
              * character.
              *
              * \return Returns true if 'c' is an alpha character or false.
              */

            static bool is_alpha( const char c );

            /** \brief Test if C comment.
              *
              * This parsing helper routine will test the given String to see
              * if it is a C11 style line or block comment. The input String
              * may have leading whitespace in it because isCC will
              * automatically skip this.
              *
              * \param s String to be tested as a C11 style comment.
              *
              * \return Returns true if the String started with a C style
              * comment and false otherwise.
              */

            static bool is_cc( ccp s );

            /** \brief Test if input String starts with an alpha character.
              *
              * This parsing helper routine will test if the input String has
              * an alpha character based on the ASCII table. That's any upper
              * or lower case letter in the range A-Z or a-z.
              *
              * \param s The String to test.
              *
              * \return Returns true if 's' has an alpha character or false.
              */

            static bool is_alpha( ccp s );

            /** \brief Test if input String is uppercase or not.
              *
              * This routine will return true if the given String is uppercase
              * or not.
              *
              * \return Returns true if the first non-whitespace character is
              * upper case.
              */

            static bool is_upper( ccp s );

            /** \brief Test if input String is lowercase or not.
              *
              * This routine will return true if the given String is lowercase
              * or not.
              *
              * \return Returns true if the first non-whitespace character is
              * lower case.
              */

            static bool is_lower( ccp s );

            /** \brief Test if character is an identifier character.
              *
              * This parsing helper routine tests to see if the given character
              * is a valid C identifier character. It assumes that it is not
              * the first character so digits are allowed.
              *
              * \param c An ASCII character to test if it's a valid identifier
              * character if not the first one.
              *
              * \param bFirst The character is the first one.
              *
              * \return Returns true if the character falls within the set _
              * a-z 0-9 A-Z. If it doesn't then we return false.
              */

            static bool is_id( const char c, const bool bFirst );

            /** \brief Test if String is valid c identifier.
              *
              * This parsing helper routine will test if the input String is a
              * valid C identifier.
              *
              * \param s A C String to test whether it's a valid C identifier.
              * A good ID is one that starts with either an underscore or an
              * alpha character and continues after with the same or a numeric
              * digit in the range 0-9.
              *
              * \return Returns true if the C String is a good C identifier or
              * false.
              */

            static bool is_id( ccp s );

            /** \brief Test if String is a numeric digit.
              *
              * This parsing helper routine will test to see if the input
              * character is a numeric digit or not.
              *
              * \param c The ASCII character to test.
              *
              * \return Returns true if the input character is a digit or false
              * otherwise.
              */

            static bool is_digit( const char c );

            /** \brief Test if String is a real number or integer.
              *
              * This parsing helper routine will test to see if the input
              * String is a valid real or integer number.
              *
              * \param s The String to test.
              *
              * \return Returns true if the String is a valid number or false.
              */

            static bool is_number( ccp s );

            /** \brief Check if String is a hex value.
              *
              * This routine checks to see if the String is hex and returns the
              * result.
              */

            bool is_hex()const;

            /** \brief Check if String is a sha1 key.
              *
              * This routine checks to see if the String is a good sha1 key
              * candidate and returns true if it is, false if it's not.
              */

            e_forceinline bool is_sha1()const{
              if( len() != 40 ){
                return false;
              }
              if( !is_hex() ){
                return false;
              }
              return true;
            }

          //}:                                    |
          //as:{                                  |

            /** \brief Convert String to given type.
              *
              * This routine will convert the input String to the type T in the
              * template. This is extremely useful for parsers, the XML
              * serializers use it for example. If the template type T is a
              * float or double then the exprtk parser will be used to convert
              * a complex expression into a numeric value which then gets
              * converted to a real value we return from the as method. If T is
              * a u32 or u64 it will first quickly parse the String to see if
              * it begins with $, 0x or 0b. If it does the code will convert
              * the hexadecimal or binary number to an integer and return the
              * value. If the String is a straight up decimal number and T is a
              * [u|s]32 or [u|s]64 then it will be converted just like you
              * called atoi but unsigned integers are handled appropriately. If
              * the same T type is provided a String with a complex expression
              * in it and doesn't start with one of the aforementioned integer
              * prefixes then the String will be sent to the expression toolkit
              * just like it was a real number. You should be able to use any
              * base engine type not including classes but everything within
              * the math<> template and things like u32 and s64 etc.
              *
              * \param pString A textual representation of an engine type.
              *
              * \return Returns a value of typename T parsed from pString.
              */

            template<typename T>static T as( ccp pString );

            /** \brief Convert String to type.
              *
              * This routine will calls the static as method to convert c_str()
              * to a type that's returned from this method.
              *
              * \return Returns a value of typename T parsed from pString.
              */

            template<typename T> e_forceinline T as()const{
              return as<T>( c_str() );
            }

          //}:                                        |
          //replace:{                             |

            /** \brief Search and replace method.
              *
              * This routine will search for the String in 'find' and replace
              * it with the String in 'with'.
              *
              * \param find The subString to look for.
              * \param with The replacement String.
              *
              * \return Returns true if one or more replacements were made and
              * false otherwise.
              */

            bool replace( const String& find, const String& with );

          //}:                                    |
          //spaces:{                              |

            static ccp spaces( u32 bytes );

          //}:                                    |
          //repeated:{                            |

            e_forceinline static String repeating( const char c, const u64 count ){
              String r;
              r.repeat( c, count );
              return r;
            }

          //}:                                    |
          //repeat:{                              |

            String& repeat( const char c, const u64 count );

          //}:                                    |
          //setv:{                                |

            /** \brief Set the String to the formatted text provided.
              *
              * This routine will blow away the current contents of the String
              * and replace it with the formatted text provied.
              *
              * \param format The formatted String to replace this with.
              *
              * \param arg Arguments a la vsprintf.
              *
              * \return Returns *this.
              */

            String& setv( ccp format, va_list arg );

          //}:                                    |
          //setf:{                                |

            /** \brief Set the String to the formatted text provided.
              *
              * This routine will blow away the current contents of the String
              * and replace it with the formatted text provied.
              *
              * \param format The formatted String to replace this with.
              *
              * \return Returns *this.
              */

            String& setf( ccp format,... );

          //}:                                    |
          //set:{                                 |

            /** \brief Set the String to the text provided.
              *
              * This routine will blow away the current contents of the String and
              * replace it with the text provied.
              *
              * \param str The String to replace this with.
              *
              * \return Returns *this.
              */

            e_forceinline String& set( ccp str ){
              setf( str );
              return *this;
            }

          //}:                                    |
          //tryv:{                                |

            /** \brief Try building formatted text.
              *
              * This routine will calculate the amount of space requird to
              * store the formatted text provided. The operation takes place in
              * a thread safe 32K buffer which all tryv, setv and catv methods
              * use. The return value can be passed directly into the stream
              * constructor if you choose, which is why the method exists.
              *
              * \param format The formatted text to try.
              *
              * \param arg The arguments.
              *
              * \returns The number of characters that will be allocated.
              */

            static u64 tryv( ccp format, va_list arg );

          //}:                                    |
          //tryf:{                                |

            /** \brief Try building formatted text.
              *
              * This routine will calculate the amount of space requird to
              * store the formatted text provided. The operation takes place in
              * a thread safe 32K buffer which all tryv, setv and catv methods
              * use. The return value can be passed directly into the stream
              * constructor if you choose, which is why the method exists.
              *
              * \param format The formatted text.
              *
              * \param ... The arguments.
              *
              * \returns The number of characters that will be allocated.
              */

            static u64 tryf( ccp format,... );

          //}:                                    |
          //getv:{                                |

            /** \brief Construct a String from formatted text.
              *
              * The constructor serves a similar purpose to the xfs class but
              * allocates in system memory rather than scratch memory.
              *
              * \param format The formatted text as per printf standard.
              * \param va Arguments to pass into formatted text.
              */

            static String getv( ccp format, va_list va );

          //}:                                    |
          //getf:{                                |

            /** \brief Construct a String from formatted text.
              *
              * The constructor serves a similar purpose to the xfs class but
              * allocates in system memory rather than scratch memory.
              *
              * \param format The formatted text as per printf standard.
              *
              * \param ... Arguments to pass into formatted text.
              */

            static String getf( ccp format,... );

          //}:                                    |
          //catv:{                                |

            /** \brief Concatenate formatted text to String.
              *
              * This routine tacks a piece of formatted text to the end of the
              * String object data. It may be empty initially.
              *
              * \param format The formatted String ala printf.
              *
              * \param arg The data arguments.
              *
              * \todo Make this a C++17 variadic template.
              *
              * \returns A '*this' reference.
              */

            String& catv( ccp format, va_list arg );

          //}:                                    |
          //catf:{                                |

            /** \brief Concatenate formatted text to String.
              *
              * This routine tacks a piece of formatted text to the end of the
              * String object data. It may be empty initially.
              \code
              void foo(){
                String s;
                s.catf( "Hello %s", "world" );
              }
              \endcode
              * \returns A '*this' reference.
              */

            String& catf( ccp format, ... );

          //}:                                    |
          //cat:{                                 |

            /** \brief Concatenate text to String.
              *
              * This routine tacks a piece of unformatted text to the end of
              * the String object data. It may be empty initially. The cat
              * functions is the workhorse of concatenation and is called by
              * catf.
              *
              * \param chars The C-String to concatenate.
              \code
                void foo(){
                  String s;
                  s.cat( "Hello world" );
                }
              \endcode
              * \return A String reference to this.
              */

            virtual String& cat( ccp chars );

          //}:                                    |
          //cat:{                                 |

          /** Concatenate text to String.
            *
            * This routine will "cat" a C-String onto the end of this one.
            *
            * \param p The C-String to concatenate.
            *
            * \param n The length of the C String in characters.
            *
            * \return Returns *this.
            */

          virtual String& cat( ccp p, const u64 n );

          /** Concatenate text to String.
            *
            * This routine will "cat" a String onto the end of this one.
            *
            * \param s The String to concatenate.
            *
            * \return Returns *this.
            */

          virtual String& cat( const String& s );

          /** Concatenate text to String.
            *
            * This routine will "cat" a String onto the end of this one.
            *
            * \param a The start of the String to concatenate.
            *
            * \param b The end of the sequence of characters.
            *
            * \return Returns *this.
            */

          virtual String& cat( ccp a, ccp b );

          //}:                                    |
          //hash:{                                |

            /** \brief Get hash of String.
              *
              * This routine will hash the String and return a 64-bit unsigned
              * integer for it. It simply returns the value of m_uHash because
              * that member variable is updated every time you call cat(),
              * which is the primary interface for allocation in the class.
              *
              * \return A 64-bit unsigned hash for the current String.
              */

            u64 hash()const;

            /** \brief Compare hashes.
              *
              * This routine will compute and compare this hash with the
              * incoming 64-bit hash value.
              */

            e_forceinline bool notHash( const u64 hash )const{
              return( m_uHash != hash );
            }

            /** \brief Compare hashes.
              *
              * This routine will compute and compare this hash with the
              * incoming 64-bit hash value.
              */

            e_forceinline bool isHash( const u64 hash )const{
              return( m_uHash == hash );
            }

          //}:                                    |
          //toupper:{                             |

            String toupper()const;

          //}:                                    |
          //upper:{                               |

            void upper();

          //}:                                    |
          //tolower:{                             |

            String tolower()const;

          //}:                                    |
          //lower:{                               |

            void lower();

          //}:                                    |
          //len:{                                 |

            /** String to length method.
              *
              * Converts a stdtype String to a String length.
              *
              * \returns Length of stdtype String.
              */

            u64 len()const;

          //}:                                    |
          //c_str:{                               |

            /** String to C String method.
              *
              * Converts a stdtype String to a C String.
              *
              * \returns A C String.
              */

            ccp c_str()const;

          //}:                                    |
          //empty:{                               |

            /** \brief Is the String empty?
              *
              * This routine checks if this is an empty String, and if it is
              * returns the appropriate boolean result.
              *
              * \return Returns true if the String is unallocated or empty, and
              * false otherwise. A false value means the String is allocated
              * and potentially shared.
              */

            e_forceinline bool empty()const{
              return( m_uHash==0 );
            }

          //}:                                    |
          //clear:{                               |

            /** \brief Clear the String to empty.
              *
              * This routine nukes the String. If the String reference count
              * bottoms out then we'll destroy the String.
              */

            void clear();

          //}:                                    |
          //chr:{                                 |

            ccp chr( const char c )const;

          //}:                                    |
          //find:{                                |

            /** \brief Find sub-String.
              *
              * This routine will search for the given String as a sub-String
              * of this.
              *
              * \param s The sub-String to search for.
              *
              * \return Returns a C String pointer to the given sub-String.
              */

            ccp find( const String& s )const;

            /** \brief Find sub-String.
              *
              * This routine will search for the given String as a sub-String
              * of this.
              *
              * \param pSub The sub-String to search for.
              *
              * \return Returns a C String pointer to the given sub-String.
              */

            ccp find( ccp pSub )const;

          //}:                                    |
          //back:{                                |

            e_forceinline char back()const{
              if( !empty() ){
                return end()[-1];
              }
              return 0;
            }

          //}:                                    |
          //end:{                                 |

            e_forceinline ccp end()const{
              return( c_str() + len() );
            }

          //}:                                    |
          //ltrim:{                               |

            e_forceinline String& e_ltrimstr( String& s, const u64 n ){
              return s.ltrim( n );
            }

            String  ltrimmed( const u64 n )const;
            String& ltrim(    const u64 n );

          //}:                                    |
          //trim:{                                |

            e_forceinline String& e_trimstr( String& s, const u64 n ){
              return s.trim( n );
            }

            String trimmed( const u64 n )const;
            String& trim( const u64 n );

          //}:                                    |
          //mixedcase:{                           |

            e_forceinline friend String e_mixedcase( const String& s ){
              return s.mixedcase();
            }

            /** \brief Convert String to mixed case String.
              *
              * This routine will scan through the String converting it to
              * mixed case. If the first non-whitespace character is a
              * lowercase character it will be converted to A-Z. All other
              * characters will be treated as lowercase unless they are already
              * uppercase or two characters are separated by whitespace or
              * underscores(s). The latter will cause the next non-whitespace
              * or underscore character to be converted from a-z to A-Z.
              *
              * \return Returns a String that's been converted to mixed case.
              * This is especially useful for converting filenames to
              * identifiers in the Swordlight editor.
              */

            String mixedcase()const;

          //}:                                        |
          //camelcase:{                           |

            e_forceinline friend String e_camelcase( const String& s ){
              return s.camelcase();
            }

            String camelcase()const;

          //}:                                    |
          //right:{                               |

            e_forceinline friend String e_rightstr( const String& s, const u64 i ){
              return s.right( i );
            }

            String right( const u64 i )const;

          //}:                                    |
          //left:{                                |

            e_forceinline friend String e_leftstr( const String& s, const u64 n ){
              return s.left( n );
            }

            String left( const u64 n )const;
            bool isleft( ccp pText )const;

          //}:                                    |
          //mid:{                                 |

            e_forceinline friend String e_midstr( const String& s, const u64 left, const u64 right ){
              return s.mid( left, right );
            }

            String mid( const u64 left, const u64 right )const;

          //}:                                    |
          //ext:{                                 |

            /** \brief Get the filename extension.
              *
              * This routine returns a String representing a dot (.) and the
              * filename extension.
              *
              * \return Returns something like ".png" or ".fbx".
              */

            String ext()const;

          //}:                                    |
          //name:{                                |

            /** \brief Get the filename (base+ext).
              *
              * This routine will scan backwards from the end of the String
              * looking for the first / (*nix) or \\ character in the String.
              * It will return a new String representing the filename.
              */

            String filename()const;

            /** \brief Get the base name.
              *
              * This routine will treat the String as a filename and strip off
              * the extension and path parts.
              */

            String base()const;

          //}:                                    |
          //os:{                                  |

            /** \brief Convert String to os path.
              *
              * This routine will treat the String as a Unix style path and
              * convert all \ or / as appropriate for the platform.
              *
              * \return Returns a new String representing the converted path.
              */

            String os()const;

          //}:                                    |
          //envs:{                                |

            /** \brief Expand environment variables.
              *
              * This routine is pretty slow. It will run through all
              * environment variables stored in IEngine::envs and expand $()
              * style sub-Strings with their matching values.
              */

            String envs()const;

            /** \brief Expand environment variables.
              *
              * This routine is pretty slow. It will run through all
              * environment variables stored in IEngine::envs and expand $()
              * style sub-Strings with their matching values.
              */

            String& envs();

          //}:                                    |
          //path:{                                |

            /** \brief Get the path.
              *
              * This routine will treat the String as a filename and strip off
              * the file part plus extension if it has one.
              *
              * \return Returns the path part of the String if it's a filename.
              */

            String path()const;

          //}:                                    |
          //splitAtCommas:{                       |

            /** \brief Separate String into comma delineated lines.
              *
              * This routine will separate the String out into lines and stores
              * them in the returned buffer provided.
              */

            vector<String> splitAtCommas()const;

          //}:                                    |
          //splitLines:{                          |

            /** \brief Separate String into lines.
              *
              * This routine will separate the String out into lines and stores
              * them in the returned buffer provided.
              */

            vector<String> splitLines()const;

          //}:                                    |
          //setHashCode:{                         |

            /** \brief Set the hash code.
              *
              * This is a pretty dangerous API actually. It changes the hash on
              * the String which will instantly point it to a different entry
              * in the String table internally. The accessor therefore only
              * exists to support the String code itself and shouldn't be used
              * by the user unless absolutely necessary and you know what
              * you're doing.
              *
              * \param hash The new hash code. Will instantly change the String
              * table entry without updating the reference count. Danger!
              */

            void setHashCode( const u64 hash ){
              m_uHash = hash;
            }

          //}:                                    |
          //del:{                                 |

            e_forceinline bool erase( ccp s ){
              return replace( s, "" );
            }

          //}:                                    |
        //}:                                      |
        //----------------------------------------+-----------------------------

        /** \brief Standard string constructor.
          *
          * This routine will construct a string from a standard string.
          */

        String( const std::string& s );

        /** \brief String's copy constructor.
          *
          * This will construct a string as a duplicate of another string. It
          * will initialize the string to zero then "cat" the string; that way
          * we get a properly allocated copy and don't need to worry about
          * memory leaks.
          */

        String( const String& s );

        /** \brief Move constructor.
          *
          * This constructor will move the old string value into this one
          * without doing any copying whatsoever. It's a great optimization
          * that's part of C++11.
          */

        String( String&& rvalue );

        /** \brief Two string constructor.
          *
          * This will construct a string from two strings: the start and end.
          *
          * \param a The start of the new string.
          * \param b The terminator.
          */

        String( ccp a, ccp b );

        /** \brief Single string constructor.
          *
          * This routine will construct a string out of a C string.
          */

        String( ccp c );

        /** \brief String's default constructor.
          *
          * This will initialize the string to empty: no data and zero size.
          */

        String() = default;

        /** \brief String destructor.
          *
          * This routine does nothing more than call string::clear.
          */

        virtual~String();

      private:

        #if e_compiling( debug )
          mutable ccp m_pData = nullptr;
        #endif
        mutable u64 m_uHash = 0;

      public:

        static string null;
      };

      /* Aliases */

      using string_pair  = std::pair<string,string>;
      using string_pairs = vector<string_pair>;
      using strings      = vector<string>;

      /* Macros */

      #define e_xfs(...)::EON::gfc::string::getf(__VA_ARGS__)
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//Rope:{                                          |

  namespace EON{

    namespace gfc{

      /** \brief Rope factory.
        *
        * This class defines a rope string factory. A rope is basically a tree
        * structure that breaks strings down into a words in nodes. This rope
        * structure is completely thread safe.
        *
        * \tparam T The type of character: usually "char" but it can be a
        * unicode u16 type too.
        *
        * \tparam K The size of the dictionary in elements. 100 is a good size
        * in testing but it really means the number of words in a sentence or
        * paragraph.
        *
        * \tparam N The max size of all the T sized elements in the rope
        * dictionary.
        */

      template<int K=1048576,int N=1048576,typename T=char> class Rope{

        //https://brianbondy.com/blog/90/introducing-the-rope-data-structure

        //----------------------------------------+-----------------------------
        //Structs:{                               |

          /** \brief Rope knot (a knot).
            *
            * This structure is a rope knot. It can contain an index and a size
            * that references a block in the outer Rope object.  When a knot is
            * deleted  from the rope the data and size members are set to zero,
            * thereby causing the getter c_str() to skip it or otherwise append
            * nothing.
            */

          class Knot final{
            e_var_dim( u32,    Branch, 2 );     //!< Child knots.
            e_var(     u32, u, Data      ) = 0; //!< Data index into Rope buffer.
            e_var(     u32, n, Size      ) = 0; //!< Size of data.
          };

        //}:                                      |
        //Aliases:{                               |

          using Dictionary = pool<Knot,K>;
          using RawData    = pool<T,N>;

        //}:                                      |
        //Operate:{                               |

        public:

          /** \brief Streaming operator.
            *
            * This operator will stream in some bytes as if by calling cat.
            *
            * \param pString Text to concatenate.
            *
            * \return Returns *this.
            */

          Rope& operator<<( ccp pString ){
            cat( pString );
            return *this;
          }

          /** \brief Streaming operator.
            *
            * This operator will remove a substring from the rope.
            *
            * \param regex Text to remove.
            *
            * \return Returns *this.
            */

          Rope& operator>>( ccp regex ){
            del( regex );
            return *this;
          }

          /** \brief Conversion operator to gfc string.
            *
            * This conversion operator will build the string stored in the rope
            * structure and return a gfc string to it.
            *
            * \return Returns a gfc string containing the data stored in the
            * rope. This will only really work if T is a char type.
            */

          operator string()const{
            return str();
          }

          /** \brief Conversion operator to C string.
            *
            * This conversion operator will build the string stored in the rope
            * and return it as a C string.
            *
            * \return Returns a C string containing the rope data unwound.
            */

          operator ccp()const{
            return c_str();
          }

        //}:                                      |
        //Methods:{                               |

          /** \name String accessors (builders).
            *
            * These routines will return the roped string as a regular C or
            * GFC string.
            *
            * @{
            */

          /** \brief Get the C string for this rope.
            *
            * This routine will return the rope as a C string if T is a char.
            *
            * \return Returns a standard C string if T is char.
            */

          e_noinline ccp c_str()const{
            T* s = (T*)e_salloc( sizeof( T )*( m_uSize+1 ));
            T* z = s + m_uSize;
             * z = 0;
            if( !m_tFlags->bInserts ){
              memcpy( s, m_cRawData.data(), sizeof( T )*m_uSize );
            }else{
              std::deque<u32> w;
              w.push_back( m_uRoot );
              T* p = s;
              for( u32 l, r, n, d; p<z && !w.empty(); ){
                const auto next = w.front();
                w.pop_front();
                l = r = n = 0;
                m_cDictionary.query( next,
                  [&]( const Knot& knot ){
                    l = knot.inBranch( 0 );
                    r = knot.inBranch( 1 );
                    n = knot.toSize();
                    d = knot.toData();
                  }
                );
                T* e = p + n;
                while( p < e && p < z ){
                  m_cRawData.query( d++,
                    [&]( const T& t ){
                      *p = t;
                    }
                  );
                  ++p;
                }
                if( l ){
                  w.push_back( l );
                }
                if( r ){
                  w.push_back( r );
                }
              }
            }
            return s;
          }

          /** \brief Get GFC string.
            *
            * This routine will return a gfc string to the caller. It does it
            * by walking the rope and concatenating in scratch memory.
            *
            * \return Returns a GFC string with the newly constructed rope in
            * it.
            */

          e_forceinline string str()const{
            return c_str();
          }

          /** @}
            *
            * \next Deleting substrings.
            *
            * These routines will delete substrings out of the rope.
            *
            * @{
            */

          /** \brief Remove a substring from the rope.
            *
            * This routine will delete a substring or element of the rope and
            * remove it from contention. All the pooled string elements will be
            * freed up and released for general consumption by future
            * concatenation calls. Because of this the order of the characters
            * in the raw data will not match the final string.
            *
            * \param regex The string to remove. Regex must match the
            * originally inserted string. So, if you originally added "   .vs"
            * plain ".vs" won't work. You'd have to say "\s+.vs" instead.
            *
            * \return Returns true if the substring was deleted and false
            * otherwise.
            */

          e_noinline bool del( const T* regex ){
            auto result = false;
            m_cDictionary.foreachs(
              [&]( Knot& knot ){
                const auto data = knot.toData();
                if( !data ){
                  return true;
                }
                const auto size = knot.toSize();
                if( !size ){
                  return true;
                }
                T* buf = reinterpret_cast<T*>( e_salloc( size+1 ));
                for( u32 n=data+size, c=0, i=data; i<n; ++i ){
                  m_cRawData.query( i,
                    [&]( const T& t ){
                      buf[ c++ ]=t;
                    }
                  );
                }
                buf[ size ]=0;
                std:: regex r( regex );
                std::string s( buf );
                std::smatch m;
                if( std::regex_match( s, m, r )){
                  knot.setData( 0 );
                  knot.setSize( 0 );
                  result = true;
                }
                return!result;
              }
            );
            return result;
          }

          /** @}
            *
            * \next Concatenation routines.
            *
            * These routine will concatenate or insert a string into the rope.
            * It is a great little string builder.
            *
            * @{
            */

          /** \brief Concatenate string.
            *
            * This routine will concatenate a string to the Rope data struct.
            * The routine is really fast at doing this as is inserting into the
            * string we're building.
            */

          e_noinline u32 catf( ccp fmt,... ){
            va_list arg;
            va_start( arg, fmt );
              auto* temp = (T*)e_salloc( sizeof( T )*N );
              vsnprintf( temp, sizeof( T )*N, fmt, arg );
              const auto re = cat( temp, sizeof( T )*N );
            va_end( arg );
            return re;
          }

          /** \brief Insert string.
            *
            * This routine will insert a string at the given element point. It
            * will make the insert very very quickly.
            *
            * \param knotIndex The knot index for the root returned by the
            * first call to cat().
            *
            * \param pString The string to insert.
            */

          e_noinline void insert( const u32 knotIndex, const T* pString ){
            // TODO: Implement this insert.
            m_tFlags->bInserts = 1;
            // RootKnot:
            // LeftChild:
            //  - LeftChild: "Hello"
            //  - RightChild: " Great"
            // RightChild: " World"
          }

          /** \brief Concatenate string.
            *
            * This routine will concatenate a string to the Rope data struct.
            * The routine is really fast at doing this as is inserting into the
            * string we're building.
            *
            * \param pString The string to concatenate.
            *
            * \param l The length of the string in rope elements.
            *
            * \return The rope element index.
            */

          e_noinline u32 cat( const T* pString, const u32 l ){

            //------------------------------------------------------------------
            // Create a new knot.
            //------------------------------------------------------------------

            Knot R;
            const auto k = u32( m_cRawData.size()+1 );
            R.setData( k );
            R.setSize( l );
            m_uSize+=( l );
            for( u32 i=0; i < l; ++i ){
              const auto ch = pString[ i ];
              m_cRawData.set( k+i, ch );
            }

            //------------------------------------------------------------------
            // Append to the root.
            //------------------------------------------------------------------

            if( m_uRoot != ~0U ){
              R.inBranch( 0/* left */) = m_uRoot; //0=left
              m_uRoot = u32( m_cDictionary.size()+1 );
              m_cDictionary.set( m_uRoot, R );    //R=right
              R.inBranch( 1 ) = m_uRoot;          //1=right
            }else{
              m_uRoot = u32( m_cDictionary.size()+1 );
              m_cDictionary.set( m_uRoot, R );    //R=root
            }
            return m_uRoot;
          }

          /** \brief Concatenate string.
            *
            * This routine will concatenate a string to the Rope data struct.
            * The routine is really fast at doing this as is inserting into the
            * string we're building.
            *
            * \param pString The string to concatenate.
            */

          e_noinline u32 cat( const T* pString ){
            return cat( pString, u32( strlen( pString )));
          }

          /** @}
            *
            * \next Clearing and testing if clear (empty).
            *
            * These routine deal with empty ropes. Creating them and testing
            * for them in the wild.
            *
            * @{
            */

          /** \brief Check if the rope is empty.
            *
            * This routine will check if the rope structure is empty of strings
            * or other data.
            *
            * \return Returns true if the rope is empty, no elements, and false
            * otherwise.
            */

          e_forceinline bool empty()const{
            return !m_uSize;
          }

          /** \brief Clear out the rope structure.
            *
            * This routine will clean the rope, deleting all strings in it.
            */

          e_noinline void clear(){
            m_cDictionary.clear();
            m_cRawData.clear();
            m_uRoot = 0;
            m_uSize = 0;
          }

          /** @} */

        //}:                                      |
        //----------------------------------------+-----------------------------

        virtual~Rope() = default;
        Rope() = default;

      private:

        e_var1( c,  Dictionary );
        e_var1( c,  RawData    );
        e_var( u32, u, Size    ) = 0;
        e_var( u32, u, Root    ) = 0;
        e_var_bits(    Flags
          , bInserts:1
        );
      };
    }
  }

//}:                                              |
//================================================+=============================
//                                                :
//                                                :
//                                                :
//================================================+=============================
//e_*:{                                           |
  //e_stricmp:{                                   |

    #if e_compiling( osx )                                                      \
     || e_compiling( ios )                                                      \
     || e_compiling( android )                                                  \
     || e_compiling( linux )                                                    \
     || e_compiling( web )
      #define e_stricmp strcasecmp
    #elif e_compiling( microsoft )
      #define e_stricmp _stricmp
    #else
      #define e_stricmp stricmp
    #endif

  //}:                                            |
//}:                                              |
//================================================+=============================

/**     @}
  *   @}
  * @}
  */
