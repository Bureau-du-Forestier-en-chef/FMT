#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The unit tests of the spatial classes of FMTlib (namespace Spatial), labelled unit;spatial, in the GoogleTest
# executable UnitSpatial. Each source holds the suite of one class, named after that class; every .cpp file of
# this folder is built at the next configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createspatialunittests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(UnitSpatial Unit spatial ${FMTTESTSsources})
endfunction(createspatialunittests)

createspatialunittests()
