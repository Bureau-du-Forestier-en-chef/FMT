#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Formatting shared by CompareResults.cmake and ScalingReport.cmake, included by both.

# Converts a decimal written without exponent, such as "612.345" or "-0.5", into thousandths: math()
# only computes with integers. Anything else, null included, gives an empty result.
function(_toThousandths p_value p_result)
	if (NOT p_value MATCHES "^(-?)([0-9]+)(\\.([0-9]*))?$")
		set(${p_result} "" PARENT_SCOPE)
		return()
	endif()
	set(sign "${CMAKE_MATCH_1}")
	set(integerPart "${CMAKE_MATCH_2}")
	string(SUBSTRING "${CMAKE_MATCH_4}000" 0 3 fraction)
	# Leading zeros are removed, so that no number can be read as octal.
	string(REGEX REPLACE "^0+([0-9])" "\\1" integerPart "${integerPart}")
	string(REGEX REPLACE "^0+([0-9])" "\\1" fraction "${fraction}")
	math(EXPR thousandths "${integerPart} * 1000 + ${fraction}")
	if (sign)
		math(EXPR thousandths "0 - ${thousandths}")
	endif()
	set(${p_result} "${thousandths}" PARENT_SCOPE)
endfunction()

# Writes p_thousandths, a whole number of thousandths, as a decimal with three decimals, such as
# "612.345" or "-0.500": the reverse of _toThousandths.
function(_fromThousandths p_thousandths p_result)
	set(sign "")
	set(value "${p_thousandths}")
	if (value LESS 0)
		set(sign "-")
		math(EXPR value "0 - ${value}")
	endif()
	math(EXPR units "${value} / 1000")
	math(EXPR fraction "${value} % 1000")
	if (fraction LESS 10)
		set(fraction "00${fraction}")
	elseif (fraction LESS 100)
		set(fraction "0${fraction}")
	endif()
	set(${p_result} "${sign}${units}.${fraction}" PARENT_SCOPE)
endfunction()

# Writes the change from p_before to p_after in percent, with one decimal, such as "-12.4%".
function(_percentChange p_before p_after p_result)
	_toThousandths("${p_before}" before)
	_toThousandths("${p_after}" after)
	if (before STREQUAL "" OR after STREQUAL "" OR before EQUAL 0)
		set(${p_result} "n/a" PARENT_SCOPE)
		return()
	endif()
	math(EXPR tenths "(${after} - ${before}) * 1000 / ${before}")
	set(sign "+")
	if (tenths LESS 0)
		set(sign "-")
		math(EXPR tenths "0 - ${tenths}")
	endif()
	math(EXPR units "${tenths} / 10")
	math(EXPR decimal "${tenths} % 10")
	set(${p_result} "${sign}${units}.${decimal}%" PARENT_SCOPE)
endfunction()

# Writes a duration read from the results, in nanoseconds, with its unit: nanoseconds with one
# decimal below a millisecond, then milliseconds or seconds with three. string(JSON) returns it with
# every digit of its binary value, such as 378.11599999999999.
function(_formatDuration p_value p_result)
	_toThousandths("${p_value}" thousandths)
	if (thousandths STREQUAL "" OR thousandths LESS 0)
		set(${p_result} "${p_value} ns" PARENT_SCOPE)
		return()
	endif()
	if (thousandths LESS 1000000000)
		math(EXPR units "${thousandths} / 1000")
		math(EXPR tenth "(${thousandths} % 1000) / 100")
		set(${p_result} "${units}.${tenth} ns" PARENT_SCOPE)
		return()
	endif()
	set(unit "ms")
	set(scale 1000000000)
	if (NOT thousandths LESS 1000000000000)
		set(unit "s")
		set(scale 1000000000000)
	endif()
	math(EXPR units "${thousandths} / ${scale}")
	math(EXPR fraction "(${thousandths} % ${scale}) * 1000 / ${scale}")
	if (fraction LESS 10)
		set(fraction "00${fraction}")
	elseif (fraction LESS 100)
		set(fraction "0${fraction}")
	endif()
	set(${p_result} "${units}.${fraction} ${unit}" PARENT_SCOPE)
endfunction()
