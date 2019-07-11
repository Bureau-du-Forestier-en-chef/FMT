#ifndef PYDEFINITIONS_H_INCLUDED
#define PYDEFINITIONS_H_INCLUDED


template<class T>
void define_pylist()
{
	boost::python::to_python_converter<vector<T, allocator<T>>, VecToList<T>>();
	iterable_converter().from_python<vector<T>>();
}

template <class T>
void define_FMTlist()
    {
     const char* py_FMTlist =
        " ``FMTlist`` class.\n"
        "\n"
        "This class is used for mapping masks\n"
        "\n";
	 class_<Core::FMTlist<T>>("FMTlist", py_FMTlist)
        .def("getmasklist",&Core::FMTlist<T>::getmasklist)
		.def("getdatalist", &Core::FMTlist<T>::getdatalist);
	define_pylist<T>();
    }


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getarea_overloads,getarea, 0, 1)

template <class T>
void define_FMTlayer()
    {
    const char* py_FMTlayer =
        " ``FMTlayer`` class.\n"
        "\n"
        "This class is used for spatial mapping\n"
        "It uses FMTcoordinate as key to spatialise object\n"
        "\n";
    class_<Spatial::FMTlayer<T>>("FMTlayer",py_FMTlayer)
        .def("getXSize",&Spatial::FMTlayer<T>::GetXSize)
        .def("getYSize",&Spatial::FMTlayer<T>::GetYSize)
        .def("getgeotransform",&Spatial::FMTlayer<T>::getgeotransform)
        .def("getprojection",&Spatial::FMTlayer<T>::getprojection)
        .def("getmapping",&Spatial::FMTlayer<T>::getmapping)
        .def("area",&Spatial::FMTlayer<T>::area)
        .def("getcellsize",&Spatial::FMTlayer<T>::getcellsize)
        .def("__len__",&Spatial::FMTlayer<T>::size);
    boost::python::to_python_converter<map<Spatial::FMTcoordinate,T>,MapToDict<Spatial::FMTcoordinate,T>>();
    }

template<class k,class v>
void define_pydict()
    {
    boost::python::to_python_converter<map<k,v>,MapToDict<k,v>>();
	MapFrDict<k,v>();
    }

#endif // PYDEFINITIONS_H_INCLUDED
