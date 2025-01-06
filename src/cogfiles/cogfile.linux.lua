--------------------------------------------------------------------------------
-- Create the 'wsp' object and name it; this is the final program name on disk.
--------------------------------------------------------------------------------

wsp = workspace:new'precog'

--------------------------------------------------------------------------------
-- Build options.
--------------------------------------------------------------------------------

local BOOST_DIRECTORY = 'usr/share/boost/1.84.0'
local EON_DIRECTORY   = 'src/core/include'

--------------------------------------------------------------------------------
-- Create a new wsp under workspace to compile startup code.
--------------------------------------------------------------------------------

wsp:new'startup'
  : defines{'_DEBUG=1,DEBUG=1','NDEBUG=1'}
  : set_include_paths( BOOST_DIRECTORY )
  : prefix'src/core/include/eon/eon.h'
  : find_sources'src/bootseq/start'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Setup the build settings for lz4.
--------------------------------------------------------------------------------

wsp:new'lz4'
  : defines('_DEBUG=1,DEBUG=1','NDEBUG=1')
  : set_include_paths'src/lz4/include'
  : find_sources'src/lz4/src'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Setup the build settings for lua.
--------------------------------------------------------------------------------

wsp:new'lua'
  : defines( '_DEBUG=1,DEBUG=1', 'NDEBUG=1' )
  : set_include_paths'src/lua/5.4.4/lua'
  : find_sources'src/lua/5.4.4/src'
  : ignore'lua.c,luac.c'
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
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Create new project for the PAL or (P)latform (A)bstration (L)ayer).
--------------------------------------------------------------------------------

wsp:new'pal'
  : defines( '_DEBUG=1, DEBUG=1','NDEBUG=1' )
  : set_include_paths(BOOST_DIRECTORY..','..EON_DIRECTORY )
  : find_sources'src/pal/src/linux,src/pal/include'
  : prefix'src/core/include/eon/eon.h'
  : target'static'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Generate precog executable wsp.
--------------------------------------------------------------------------------

wsp:new'precog'
  : defines( '_DEBUG=1, DEBUG=1','NDEBUG=1' )
  : set_include_paths( BOOST_DIRECTORY..[[,
    src/console/precog/include,
    src/core/include,
    src/lua/5.4.4]]
  ..EON_DIRECTORY )
  : find_sources'src/console/precog/src,src/console/precog/include'
  -- Specify frameworks with no decoration and static libraries from other
  -- precoging projects with full filename (pathing is allowed too).
  : link_with[[
      libstartup.a,
      liblua.a,
      libgfc.a,
      libpal.a,
      liblz4.a]]
  : prefix'src/core/include/eon/eon.h'
  : target'console'
  : lang'c++20'

--------------------------------------------------------------------------------
-- Generate and save all project files.
--------------------------------------------------------------------------------

local uuid=generate(wsp)
save( uuid )
