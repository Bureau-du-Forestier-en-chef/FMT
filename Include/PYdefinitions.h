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
void definePyList()
{
	boost::python::to_python_converter<std::vector<T, std::allocator<T>>, VecToList<T>>();
	iterable_converter().from_python<std::vector<T>>();
}

template<class T>
void definePySet()
{
    boost::python::to_python_converter<std::set<T>, SetToList<T>>();
    iterable_converter().from_python<std::set<T>>();
}

template <class T>
void define_FMTlist()
    {
	 py_pair<Core::FMTMask, T>();
	 py_pair<Core::FMTMask const, T>();
	 boost::python::class_<Core::FMTList<T>>("FMTlist", "@DocString(FMTList)")
		 .def("__iter__", boost::python::iterator<Core::FMTList<T>>())
         .def("update", &Core::FMTList<T>::update,
             "@DocString(FMTList::update)")
         .def("push_back",
             &Core::FMTList<T>::apiPushBack,
             "@DocString(FMTYieldHandler::APIpush_back)");
	definePyList<T>();
    }

template <class T>
void define_FMTlayer()
    {

	boost::python::class_<Spatial::FMTLayer<T>>("FMTlayer", "@DocString(FMTLayer)")
        .def("getXSize",&Spatial::FMTLayer<T>::getXSize,
			"@DocString(FMTLayer::GetXSize)")
        .def("getYSize",&Spatial::FMTLayer<T>::getYSize,
			"@DocString(FMTLayer::GetYSize)")
        .def("getgeotransform",&Spatial::FMTLayer<T>::getGeoTransform,
			"@DocString(FMTLayer::getgeotransform)")
        .def("getprojection",&Spatial::FMTLayer<T>::getProjection,
			"@DocString(FMTLayer::getprojection)")
        .def("getmapping",&Spatial::FMTLayer<T>::getMapping,
			"@DocString(FMTLayer::getmapping)")
        .def("area",&Spatial::FMTLayer<T>::area,
			"@DocString(FMTLayer::area)")
        .def("getcellsize",&Spatial::FMTLayer<T>::getCellSize,
			"@DocString(FMTLayer::getcellsize)")
        .def("__len__",&Spatial::FMTLayer<T>::size,
			"@DocString(FMTLayer::size)")
        //const because begin and end return const iterator
        .def("itercell", boost::python::iterator<Spatial::FMTLayer<T>>())
        .def("__getitem__",&Spatial::FMTLayer<T>::at,boost::python::return_internal_reference<>(),"@DocString(FMTLayer::at)");
    boost::python::to_python_converter<std::map<Spatial::FMTCoordinate,T>,MapToDict<Spatial::FMTCoordinate,T>>();
    }

template<class k,class v>
void definePyDict()
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
void definePyPair() {
	boost::python::to_python_converter<std::pair<T1, T2>, PairToPythonConverter<T1, T2>>();
    PythonToPairConverter<T1, T2>();
	}


}
#endif // PYDEFINITIONS_H_INCLUDED
