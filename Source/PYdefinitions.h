/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYDEFINITIONS_H_INCLUDED
#define PYDEFINITIONS_H_INCLUDED

#include "boost/python.hpp"

namespace Python
{ 

template<class T>
void define_pylist()
{
	boost::python::to_python_converter<std::vector<T, std::allocator<T>>, VecToList<T>>();
	iterable_converter().from_python<std::vector<T>>();
}

template <class T>
void define_FMTlist()
    {
	 py_pair<Core::FMTmask, T>();
	 py_pair<Core::FMTmask const, T>();
	 boost::python::class_<Core::FMTlist<T>>("FMTlist", "@DocString(FMTlist)")
		 .def("__iter__", boost::python::iterator<Core::FMTlist<T>>());
	define_pylist<T>();
    }


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getarea_overloads,getarea, 0, 1)

template <class T>
void define_FMTlayer()
    {

	boost::python::class_<Spatial::FMTlayer<T>>("FMTlayer", "@DocString(FMTlayer)")
        .def("getXSize",&Spatial::FMTlayer<T>::GetXSize,
			"@DocString(FMTlayer::GetXSize)")
        .def("getYSize",&Spatial::FMTlayer<T>::GetYSize,
			"@DocString(FMTlayer::GetYSize)")
        .def("getgeotransform",&Spatial::FMTlayer<T>::getgeotransform,
			"@DocString(FMTlayer::getgeotransform)")
        .def("getprojection",&Spatial::FMTlayer<T>::getprojection,
			"@DocString(FMTlayer::getprojection)")
        .def("getmapping",&Spatial::FMTlayer<T>::getmapping,
			"@DocString(FMTlayer::getmapping)")
        .def("area",&Spatial::FMTlayer<T>::area,
			"@DocString(FMTlayer::area)")
        .def("getcellsize",&Spatial::FMTlayer<T>::getcellsize,
			"@DocString(FMTlayer::getcellsize)")
        .def("__len__",&Spatial::FMTlayer<T>::size,
			"@DocString(FMTlayer::size)");
    boost::python::to_python_converter<std::map<Spatial::FMTcoordinate,T>,MapToDict<Spatial::FMTcoordinate,T>>();
    }

template<class k,class v>
void define_pydict()
    {
    boost::python::to_python_converter<std::map<k,v>,MapToDict<k,v>>();
	MapFrDict<k,v>();
    }


}
#endif // PYDEFINITIONS_H_INCLUDED
