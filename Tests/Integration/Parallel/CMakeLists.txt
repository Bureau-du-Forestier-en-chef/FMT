#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The integration tests of the parallel tasks of FMTlib with their real dependencies, a solver and the CSV driver of
# GDAL, labelled integration;parallel, in the GoogleTest executable IntegrationParallel: the planning and the
# replanning that FMTTaskHandler runs on several threads, as the interface runs them, must give the results of one
# thread, and the threads of a solver the optimum of one. They read Examples/Models/TWD_land, hence the ExamplesModels
# fixture, and write under tests/IntegrationParallel in the build folder. Each source holds the suite of one class, named
# after it (Documentation/CodingStandards.md, Test names); every .cpp file of this folder is built at the next
# configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createparallelintegrationtests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(IntegrationParallel Integration parallel EXAMPLES_MODELS ${FMTTESTSsources})
endfunction(createparallelintegrationtests)

createparallelintegrationtests()
