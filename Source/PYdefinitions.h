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

/*template<typename T1, typename T2>
struct PairToPythonConverter {
    static PyObject* convert(const std::pair<T1, T2>& pair)
    {
        return boost::python::incref(boost::python::make_tuple(pair.first, pair.second).ptr());
    }
};

template<typename T1, typename T2>
struct PythonToPairConverter {
    PythonToPairConverter()
    {
    	boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<std::pair<T1, T2> >());
    }
    static void* convertible(PyObject* obj)
    {
        if (!PyTuple_CheckExact(obj)) return 0;
        if (PyTuple_Size(obj) != 2) return 0;
        return obj;
    }
    static void construct(PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
    	boost::python::tuple tuple(boost::python::borrowed(obj));
        void* storage = ((boost::python::converter::rvalue_from_python_storage<std::pair<T1, T2> >*) data)->storage.bytes;
        new (storage) std::pair<T1, T2>(boost::python::extract<T1>(tuple[0]), boost::python::extract<T2>(tuple[1]));
        data->convertible = storage;
    }
};*/

template<typename T1, typename T2>
void define_pypair() {
	boost::python::to_python_converter<std::pair<T1, T2>, PairToPythonConverter<T1, T2>>();
    PythonToPairConverter<T1, T2>();
	}


}
#endif // PYDEFINITIONS_H_INCLUDED
