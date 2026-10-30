#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The integration tests of the planning models of FMTlib with their real dependency, a solver, labelled
# integration;models, in the GoogleTest executable IntegrationModels: the graph and the matrix that buildPeriod builds,
# and the solutions that setSolution, setSolutionByLp and boundSolution set. They read Examples/Models/TWD_land, hence
# the ExamplesModels fixture, and write nothing. The models and their solver interface are part of FMTlib, which
# registergoogletests links: no other library is needed. Each source holds the suite of one class, named after it
# (Documentation/CodingStandards.md, Test names); every .cpp file of this folder is built at the next configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createmodelsintegrationtests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(IntegrationModels Integration models EXAMPLES_MODELS ${FMTTESTSsources})
endfunction(createmodelsintegrationtests)

createmodelsintegrationtests()
