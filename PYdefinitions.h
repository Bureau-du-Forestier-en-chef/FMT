/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
