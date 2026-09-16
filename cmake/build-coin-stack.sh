#!/usr/bin/env bash
set -Eeuo pipefail

trap 'status=$?; echo >&2; echo "ERROR at line $LINENO, status $status" >&2; echo "Command: $BASH_COMMAND" >&2; exit $status' ERR

log() { printf '\n============================================================\n==> %s\n============================================================\n' "$*"; }
fail() { printf '\nERROR: %s\n' "$*" >&2; exit 1; }

ROOT="${ROOT:-$PWD/coin-stack-build}"
PREFIX="${PREFIX:-$ROOT/install}"
SOURCES="${SOURCES:-$ROOT/sources}"
JOBS="${JOBS:-8}"
RTOOLS_ROOT="${RTOOLS_ROOT:-/x86_64-w64-mingw32.static.posix}"

# MOSEK is opt-in. FindOSI.cmake passes MOSEK_ROOT only when ENV{Mosek_DIR}
# exists and is non-empty.
MOSEK_ROOT="${MOSEK_ROOT:-}"
WITH_MOSEK=0
MOSEK_LIBRARY=""
MOSEK_LIBRARY_NAME=""
MOSEK_DLL=""

COINUTILS_COMMIT="aae9b0b807a920c41d7782d7bf2775afb17a12c6"
OSI_COMMIT="79167ab9a4487b5a1f88ec4fdfd4ed529a1c31ff"
CLP_COMMIT="5315ef2e93f5f532a600e16ab604ac439a416e59"

COINUTILS_SOURCE="$SOURCES/CoinUtils"
OSI_SOURCE="$SOURCES/Osi"
CLP_SOURCE="$SOURCES/Clp"

for cmd in basename find git grep make pkg-config sed sort tail; do
    command -v "$cmd" >/dev/null 2>&1 || fail "Required command not found: $cmd"
done

CC_PATH="$RTOOLS_ROOT/bin/gcc"
CXX_PATH="$RTOOLS_ROOT/bin/g++"
FC_PATH="$RTOOLS_ROOT/bin/gfortran"
AR_PATH="$RTOOLS_ROOT/bin/ar"
RANLIB_PATH="$RTOOLS_ROOT/bin/ranlib"

test -x "$CC_PATH" || fail "Missing compiler: $CC_PATH"
test -x "$CXX_PATH" || fail "Missing compiler: $CXX_PATH"
test -x "$AR_PATH" || fail "Missing archiver: $AR_PATH"
test -x "$RANLIB_PATH" || fail "Missing ranlib: $RANLIB_PATH"

export CC="$CC_PATH" CXX="$CXX_PATH" AR="$AR_PATH" RANLIB="$RANLIB_PATH"
if test -x "$FC_PATH"; then export FC="$FC_PATH" F77="$FC_PATH"; fi

log "Build configuration"
echo "ROOT:        $ROOT"
echo "PREFIX:      $PREFIX"
echo "SOURCES:     $SOURCES"
echo "JOBS:        $JOBS"
echo "RTOOLS_ROOT: $RTOOLS_ROOT"
echo "Target:      $($CXX_PATH -dumpmachine)"

export PKG_CONFIG_PATH="$RTOOLS_ROOT/lib/pkgconfig"
pkg-config --exists glpk || fail "RTools pkg-config package not found: glpk"
pkg-config --exists blas || fail "RTools pkg-config package not found: blas"
GLPK_CFLAGS="$(pkg-config --cflags glpk)"
GLPK_LIBS="$(pkg-config --libs glpk)"

test -f "$RTOOLS_ROOT/include/glpk.h" || fail "Missing GLPK header"
test -f "$RTOOLS_ROOT/lib/libglpk.a" || fail "Missing GLPK library"

if test -n "$MOSEK_ROOT"; then
    log "MOSEK support enabled by MOSEK_ROOT"

    MOSEK_ROOT="${MOSEK_ROOT%/}"

    # Accept either the MOSEK version root:
    #   /c/PROGRA~1/Mosek/10.1
    # or the platform root:
    #   /c/PROGRA~1/Mosek/10.1/tools/platform/win64x86
    if test -f "$MOSEK_ROOT/h/mosek.h"; then
        MOSEK_PLATFORM_ROOT="$MOSEK_ROOT"
    elif test -f "$MOSEK_ROOT/tools/platform/win64x86/h/mosek.h"; then
        MOSEK_PLATFORM_ROOT="$MOSEK_ROOT/tools/platform/win64x86"
    else
        fail "Could not locate MOSEK. Checked $MOSEK_ROOT/h/mosek.h and $MOSEK_ROOT/tools/platform/win64x86/h/mosek.h"
    fi

    MOSEK_ROOT="$MOSEK_PLATFORM_ROOT"
    MOSEK_HEADER="$MOSEK_ROOT/h/mosek.h"

    MOSEK_LIBRARY="$(
        find "$MOSEK_ROOT/bin" \
            -maxdepth 1 \
            -type f \
            -name 'libmosek64_*.a' \
            -print \
        | sort -V \
        | tail -n 1
    )"

    test -n "$MOSEK_LIBRARY" || \
        fail "No libmosek64_*.a found in $MOSEK_ROOT/bin"

    MOSEK_LIBRARY_NAME="$(basename "$MOSEK_LIBRARY")"
    MOSEK_LIBRARY_NAME="${MOSEK_LIBRARY_NAME#lib}"
    MOSEK_LIBRARY_NAME="${MOSEK_LIBRARY_NAME%.a}"
    MOSEK_DLL="$MOSEK_ROOT/bin/${MOSEK_LIBRARY_NAME}.dll"

    test -f "$MOSEK_HEADER" || fail "Missing MOSEK header: $MOSEK_HEADER"
    test -f "$MOSEK_LIBRARY" || fail "Missing MOSEK library: $MOSEK_LIBRARY"
    test -f "$MOSEK_DLL" || fail "Missing MOSEK DLL: $MOSEK_DLL"

    WITH_MOSEK=1

    echo "MOSEK platform root: $MOSEK_ROOT"
    echo "MOSEK header:        $MOSEK_HEADER"
    echo "MOSEK library name:  $MOSEK_LIBRARY_NAME"
    echo "MOSEK library:       $MOSEK_LIBRARY"
    echo "MOSEK DLL:           $MOSEK_DLL"
else
    log "MOSEK support disabled because MOSEK_ROOT is not set"
fi

# Always replace the prefix so changing the optional MOSEK feature cannot leave
# stale OsiMsk artifacts behind.
rm -rf "$PREFIX" "$SOURCES"
mkdir -p "$PREFIX" "$SOURCES"
export PATH="$PREFIX/bin${MOSEK_ROOT:+:$MOSEK_ROOT/bin}:$PATH"

clone_exact() {
    local repo="$1" dir="$2" commit="$3"
    git clone "$repo" "$dir"
    git -C "$dir" checkout --detach "$commit"
    test "$(git -C "$dir" rev-parse HEAD)" = "$commit" || fail "Unexpected commit in $dir"
}

patch_dependency_names() {
    local dir="$1" file
    while IFS= read -r file; do
        grep -q coinglpk "$file" && sed -i 's/coinglpk/glpk/g' "$file" || true
        grep -q coinmumps "$file" && sed -i 's/coinmumps/mumps/g' "$file" || true
    done < <(find "$dir" -type f \( -name configure -o -name configure.ac \))
}

log "Build CoinUtils"
clone_exact https://github.com/coin-or/CoinUtils.git "$COINUTILS_SOURCE" "$COINUTILS_COMMIT"
patch_dependency_names "$COINUTILS_SOURCE"
cd "$COINUTILS_SOURCE"
export CPPFLAGS="$GLPK_CFLAGS" CFLAGS="-O2 -DNDEBUG" CXXFLAGS="-O2 -DNDEBUG -std=gnu++14" LDFLAGS="-L$RTOOLS_ROOT/lib"
./configure --prefix="$PREFIX" --enable-static --disable-shared --disable-option-checking --disable-dependency-tracking --disable-maintainer-mode --disable-readline --with-glpk-cflags="$GLPK_CFLAGS" --with-glpk-lflags="-L$RTOOLS_ROOT/lib -lglpk"
make -j"$JOBS"
make install

PRIVATE_COIN_HEADER="$(find "$PREFIX/include" -type f -name CoinTypes.h -print -quit)"
PRIVATE_CONFLICT_HEADER="$(find "$PREFIX/include" -type f -name CoinStaticConflictGraph.hpp -print -quit)"
test -n "$PRIVATE_COIN_HEADER" || fail "Private CoinUtils did not install CoinTypes.h"
test -n "$PRIVATE_CONFLICT_HEADER" || fail "Private CoinUtils did not install CoinStaticConflictGraph.hpp"
PRIVATE_COIN_INCLUDE="$(dirname "$PRIVATE_COIN_HEADER")"
COINUTILS_CFLAGS="-I$PRIVATE_COIN_INCLUDE"
COINUTILS_LIBS="-L$PREFIX/lib -lCoinUtils -lbz2 -lz -lblas -lm"

log "Build Osi adapters"
clone_exact https://github.com/coin-or/Osi.git "$OSI_SOURCE" "$OSI_COMMIT"
patch_dependency_names "$OSI_SOURCE"
cd "$OSI_SOURCE"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:$RTOOLS_ROOT/lib/pkgconfig"
export CPPFLAGS="$COINUTILS_CFLAGS $GLPK_CFLAGS" CFLAGS="-O2 -DNDEBUG" CXXFLAGS="-O2 -DNDEBUG -std=gnu++14" LDFLAGS="-L$PREFIX/lib -L$RTOOLS_ROOT/lib"

OSI_OPTIONS=(
    --prefix="$PREFIX" --enable-static --disable-shared
    --disable-option-checking --disable-dependency-tracking
    --disable-maintainer-mode --disable-readline
    --with-coinutils-cflags="$COINUTILS_CFLAGS"
    --with-coinutils-lflags="$COINUTILS_LIBS"
    --with-glpk-cflags="$GLPK_CFLAGS"
    --with-glpk-lflags="-L$RTOOLS_ROOT/lib -lglpk"
    --without-netlib --without-sample --without-gurobi
    --without-xpress --without-cplex --without-soplex
)
if test "$WITH_MOSEK" = 1; then
    OSI_OPTIONS+=(
        --with-mosek-cflags="-I$MOSEK_ROOT/h"
        --with-mosek-lflags="-L$MOSEK_ROOT/bin -l$MOSEK_LIBRARY_NAME -lpthread"
    )
else
    OSI_OPTIONS+=(--without-mosek)
fi
./configure "${OSI_OPTIONS[@]}"
make -j"$JOBS"
make install

test -f "$PREFIX/lib/libOsi.a" || fail "Osi was not installed"
test -f "$PREFIX/lib/libOsiGlpk.a" || fail "OsiGlpk was not installed"
if test "$WITH_MOSEK" = 1; then
    test -f "$PREFIX/lib/libOsiMsk.a" || fail "OsiMsk was requested but not installed"
else
    test ! -f "$PREFIX/lib/libOsiMsk.a" || fail "OsiMsk exists although MOSEK is disabled"
fi

PRIVATE_OSI_HEADER="$(find "$PREFIX/include" -type f -name OsiSolverInterface.hpp -print -quit)"
test -n "$PRIVATE_OSI_HEADER" || fail "Private Osi headers were not installed"
PRIVATE_OSI_INCLUDE="$(dirname "$PRIVATE_OSI_HEADER")"
OSI_CFLAGS="-I$PRIVATE_OSI_INCLUDE -I$PRIVATE_COIN_INCLUDE"
OSI_LIBS="-L$PREFIX/lib -lOsi $COINUTILS_LIBS"

log "Build Clp and OsiClp"
clone_exact https://github.com/coin-or/Clp.git "$CLP_SOURCE" "$CLP_COMMIT"
patch_dependency_names "$CLP_SOURCE"
cd "$CLP_SOURCE"
export CPPFLAGS="$COINUTILS_CFLAGS $OSI_CFLAGS $GLPK_CFLAGS" CFLAGS="-O2 -DNDEBUG" CXXFLAGS="-O2 -DNDEBUG -std=gnu++14" LDFLAGS="-L$PREFIX/lib -L$RTOOLS_ROOT/lib"
./configure --prefix="$PREFIX" --enable-static --disable-shared --disable-option-checking --disable-dependency-tracking --disable-maintainer-mode --disable-readline --with-coinutils-cflags="$COINUTILS_CFLAGS" --with-coinutils-lflags="$COINUTILS_LIBS" --with-osi-cflags="$OSI_CFLAGS" --with-osi-lflags="$OSI_LIBS" --with-glpk-cflags="$GLPK_CFLAGS" --with-glpk-lflags="-L$RTOOLS_ROOT/lib -lglpk" --without-netlib --without-sample
make -j"$JOBS"
make install

for lib in libCoinUtils.a libOsi.a libOsiClp.a libOsiGlpk.a libClp.a; do
    test -f "$PREFIX/lib/$lib" || fail "Final library missing: $PREFIX/lib/$lib"
done

{
    echo "CoinUtilsCommit=$COINUTILS_COMMIT"
    echo "OsiCommit=$OSI_COMMIT"
    echo "ClpCommit=$CLP_COMMIT"
    echo "WithMosek=$WITH_MOSEK"
    echo "MosekRoot=$MOSEK_ROOT"
    echo "MosekLibraryName=$MOSEK_LIBRARY_NAME"
    echo "CompilerTarget=$($CXX_PATH -dumpmachine)"
    echo "CompilerVersion=$($CXX_PATH -dumpfullversion -dumpversion)"
} > "$PREFIX/.coin-stack-info"

log "Complete matched COIN-OR stack built successfully"
echo "Prefix:    $PREFIX"
echo "WithMosek: $WITH_MOSEK"
find "$PREFIX/lib" -maxdepth 1 -type f -name 'lib*.a' -print
