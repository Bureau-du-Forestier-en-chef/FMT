





vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO coin-or/Osi
    REF 79167ab9a4487b5a1f88ec4fdfd4ed529a1c31ff 
    SHA512 405206d1a8e1f0adff8223ad4843251532dc954a6400f8566d826f93dd55741423185001f4d5a97b4d02ed39a9fe32ef063978d101c0a3eaa341a7c0dbce9765
    PATCHES glpk.patch
)


set(CMAKE_MODULE_PATH "$ENV{MODULE_PATH};${CMAKE_MODULE_PATH}")

find_package(Mosek)

file(COPY "${CURRENT_INSTALLED_DIR}/share/coin-or-buildtools/" DESTINATION "${SOURCE_PATH}")

set(ENV{ACLOCAL} "aclocal -I \"${SOURCE_PATH}/BuildTools\"")

set(GLPK_LFLAGS_DEBUG "${CURRENT_INSTALLED_DIR}/debug/lib glpk.lib")
set(GLPK_LFLAGS_RELEASE "${CURRENT_INSTALLED_DIR}/lib glpk.lib")
set(GLPK_INCLUDE "-I${CURRENT_INSTALLED_DIR}/include")

if (Mosek_FOUND)
	set(ENV{LDFLAGS} "-LIBPATH:${Mosek_LIB_LOCATION}${Mosek_WIN_LIBS}")
	vcpkg_configure_make(
		SOURCE_PATH "${SOURCE_PATH}"
		AUTOCONFIG
		CONFIGURE_ENVIRONMENT_VARIABLES LIBS
		OPTIONS
			--disable-option-checking
			--with-mosek-cflags=-I${Mosek_INCLUDE_DIR}
			--with-mosek-lflags=-LIBPATH:${Mosek_LIB_LOCATION}${Mosek_WIN_LIBS}
			--with-lapack
			--with-coinutils
			--without-netlib
			--without-sample
			--without-gurobi
			--without-xpress
			--without-cplex
			--without-soplex
			--enable-relocatable
			--disable-readline
		OPTIONS_DEBUG
			"CXXFLAGS=-Xcompiler -std:c++14 -Xcompiler -Zc:__cplusplus -Xcompiler -EHsc -Xcompiler -MDd"
			--with-glpk-cflags=${GLPK_INCLUDE}
			--with-glpk-lflags=-LIBPATH:${GLPK_LFLAGS_DEBUG}
		OPTIONS_RELEASE
			"CXXFLAGS=-Xcompiler -std:c++14 -Xcompiler -Zc:__cplusplus -Xcompiler -EHsc -Xcompiler -MD"
			--with-glpk-cflags=${GLPK_INCLUDE}
			--with-glpk-lflags=-LIBPATH:${GLPK_LFLAGS_RELEASE}
	)
else()
	vcpkg_configure_make(
		SOURCE_PATH "${SOURCE_PATH}"
		AUTOCONFIG
		CONFIGURE_ENVIRONMENT_VARIABLES LIBS
		OPTIONS
			--with-glpk
			--with-lapack
			--with-coinutils
			--without-netlib
			--without-sample
			--without-gurobi
			--without-xpress
			--without-cplex
			--without-soplex
			--enable-relocatable
			--disable-readline
		OPTIONS_DEBUG
			"CXXFLAGS=-Xcompiler -std:c++14 -Xcompiler -Zc:__cplusplus -Xcompiler -EHsc -Xcompiler -MDd"
			--with-glpk-cflags=${GLPK_INCLUDE}
			--with-glpk-lflags=-LIBPATH:${GLPK_LFLAGS_DEBUG}
		OPTIONS_RELEASE
			"CXXFLAGS=-Xcompiler -std:c++14 -Xcompiler -Zc:__cplusplus -Xcompiler -EHsc -Xcompiler -MD"
			--with-glpk-cflags=${GLPK_INCLUDE}
			--with-glpk-lflags=-LIBPATH:${GLPK_LFLAGS_RELEASE}
	)

endif(Mosek_FOUND)

vcpkg_install_make()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)