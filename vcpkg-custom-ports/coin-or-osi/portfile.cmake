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

if (MOSEK_FOUND)
message("TEST ${MOSEK_INCLUDE_DIR}")
	set(ENV{LDFLAGS} "-LIBPATH:${MOSEK_LIB_LOCATION}${MOSEK_WIN_LIBS}")
	#set(ENV{LDFLAGS} "-LIBPATH:C:/PROGRA~1/Mosek/10.1/tools/platform/win64x86/bin fusion64_10_1.lib mosek64_10_1.lib")
	vcpkg_configure_make(
		SOURCE_PATH "${SOURCE_PATH}"
		AUTOCONFIG
		CONFIGURE_ENVIRONMENT_VARIABLES LIBS
		OPTIONS
			--disable-option-checking
			#--with-mosek-cflags=-IC:/PROGRA~1/Mosek/10.1/tools/platform/win64x86/h
			#--with-mosek-lflags=-LIBPATH:C:/PROGRA~1/Mosek/10.1/tools/platform/win64x86/bin
			--with-mosek-cflags=-I${MOSEK_INCLUDE_DIR}
			--with-mosek-lflags=-LIBPATH:${MOSEK_LIB_LOCATION}${MOSEK_WIN_LIBS}
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
	)

endif(MOSEK_FOUND)



vcpkg_install_make()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
