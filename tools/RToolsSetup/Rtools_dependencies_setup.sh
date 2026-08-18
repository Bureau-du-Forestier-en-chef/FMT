#Install R
#Install Rtools
#Change to do in /etc/profile in Rtools:
MSYS2_PATH="/usr/local/bin:/usr/bin:/x86_64-w64-mingw32.static.posix/bin"
MANPATH="/usr/local/man:/usr/share/man:/usr/man:/share/man:/x86_64-w64-mingw32.static.posix/man"
INFOPATH="/usr/local/info:/usr/share/info:/usr/info:/share/info:/x86_64-w64-mingw32.static.posix/share/info"

unset MINGW_MOUNT_POINT
. '/etc/msystem'
case "${MSYSTEM}" in
MINGW*|CLANG*|UCRT*)
  #MINGW_MOUNT_POINT="${MINGW_PREFIX}"
  MINGW_MOUNT_POINT="x86_64-w64-mingw32.static.posix"
  PATH="${MINGW_MOUNT_POINT}/bin:${MSYS2_PATH}${ORIGINAL_PATH:+:${ORIGINAL_PATH}}"
  PKG_CONFIG_PATH="${MINGW_MOUNT_POINT}/lib/pkgconfig:${MINGW_MOUNT_POINT}/share/pkgconfig:D:/Rtools43/x86_64-w64-mingw32.static.posix/lib/pkgconfig"
  PKG_CONFIG_SYSTEM_INCLUDE_PATH="${MINGW_MOUNT_POINT}/include"
  PKG_CONFIG_SYSTEM_LIBRARY_PATH="${MINGW_MOUNT_POINT}/lib"
  ACLOCAL_PATH="${MINGW_MOUNT_POINT}/share/aclocal:/usr/share/aclocal"
  MANPATH="${MINGW_MOUNT_POINT}/local/man:${MINGW_MOUNT_POINT}/share/man:${MANPATH}"
  INFOPATH="${MINGW_MOUNT_POINT}/local/info:${MINGW_MOUNT_POINT}/share/info:${INFOPATH}"
  ;;
*)
  PATH="${MSYS2_PATH}:/opt/bin${ORIGINAL_PATH:+:${ORIGINAL_PATH}}"
  PKG_CONFIG_PATH="/usr/lib/pkgconfig:/usr/share/pkgconfig:/lib/pkgconfig"
esac

PKG_CONFIG_PATH="/x86_64-w64-mingw32.static.posix/lib/pkgconfig"
PKG_CONFIG="/x86_64-w64-mingw32.static.posix/bin/pkgconfig"
#Install Rcpp on R
