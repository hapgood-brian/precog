OS=`uname`
[ "$OS" == 'Darwin' ] && {
  cmake .. -G Xcode -Wno-dev
}
[ "$OS" == 'Linux' ] && {
  cmake .. -G "CodeLite - Ninja" -Wno-dev
}
[ "$OS" == "Windows" ] && {
  cmake .. -G "Visual Studio 16 2019" -Wno-dev
}
