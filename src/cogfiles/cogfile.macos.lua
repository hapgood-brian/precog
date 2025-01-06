--------------------------------------------------------------------------------
-- Create the 'wsp' object and name it; this is the final program name on disk.
--------------------------------------------------------------------------------

wsp = workspace:new'precog'

--------------------------------------------------------------------------------
-- Build options.
--------------------------------------------------------------------------------

local BOOST_DIRECTORY = 'usr/share/boost/1.84.0'
local EON_DIRECTORY   = 'src/core/include'
local UNIVERSAL_BIN   = 'yes'
local APPLE_SILICON   = 'no'

--------------------------------------------------------------------------------
-- Create a new wsp under workspace to compile startup code.
--------------------------------------------------------------------------------

wsp:new'startup'
  : defines{'_DEBUG=1,DEBUG=1','NDEBUG=1'}
  : set_include_paths( BOOST_DIRECTORY )
  : prefix'src/core/include/eon/eon.h'
  : find_sources'src/bootseq/start'
  : universal'yes'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Setup the build settings for lz4.
--------------------------------------------------------------------------------

wsp:new'lz4'
  : defines('_DEBUG=1,DEBUG=1','NDEBUG=1')
  : set_include_paths'src/lz4/include'
  : find_sources'src/lz4/src'
  : universal'yes'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Setup the build settings for lua.
--------------------------------------------------------------------------------

wsp:new'lua'
  : defines( '_DEBUG=1,DEBUG=1', 'NDEBUG=1' )
  : set_include_paths'src/lua/5.4.7/lua'
  : find_sources'src/lua/5.4.7/src'
  : ignore'lua.c,luac.c'
  : universal'yes'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Setup the build settings for gfc.
--------------------------------------------------------------------------------

wsp:new'gfc'
  : defines( '_DEBUG=1,DEBUG=1', 'NDEBUG=1' )
  : set_include_paths( BOOST_DIRECTORY..[[,
    src/lz4/include,]]
  ..EON_DIRECTORY )
  : find_sources'src/core/src,src/core/include'
  : prefix'src/core/include/eon/eon.h'
  : skip_unity'f32.cpp'
  : universal'yes'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Create a new wsp under workspace to compile startup code.
--
-- The PLATFORM variable is one of android, ios, linux, osx, web and win. If you
-- name your platform specific directories like so then one line pulls in the
-- code for a specific platform.
--------------------------------------------------------------------------------

wsp:new'pal'
  : defines( '_DEBUG=1, DEBUG=1','NDEBUG=1' )
  : set_include_paths(
    BOOST_DIRECTORY..','
  ..EON_DIRECTORY )
  : find_sources'src/pal/src/osx,src/pal/include'
  : prefix'src/core/include/eon/eon.h'
  : universal'yes'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Generate precog executable wsp.
--------------------------------------------------------------------------------

wsp:new'precog'
  : defines( '_DEBUG=1, DEBUG=1','NDEBUG=1' )
  : organization'Brian Hapgood'
  : identifier'com.noenvel-games.precog'
  : team'HE96RQ5ZY9'
  : set_include_paths( BOOST_DIRECTORY..[[,
      src/console/include,
      src/lua/5.4.7,]]
  ..EON_DIRECTORY )
  : find_sources'src/console/precog/src,src/console/precog/include'
  -- Specify frameworks with no decoration and static libraries from other precog
  -- projects with full filename (pathing is allowed too).
  : link_with[[
      CoreFoundation,
      Foundation,
      libstartup.a,
      libgfc.a,
      liblua.a,
      libpal.a,
      liblz4.a]]
  : prefix'src/core/include/eon/eon.h'
  : universal'true'
  : target'console'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Generate and save all project files.
--------------------------------------------------------------------------------

local uuid=generate(wsp)
save( uuid )
