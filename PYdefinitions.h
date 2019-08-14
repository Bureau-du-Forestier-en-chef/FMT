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
