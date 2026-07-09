/*
Copyright (c) 2019 Gouvernement du Qubec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef RDEFINITIONS_H_INCLUDED
#define RDEFINITIONS_H_INCLUDED

#if defined FMTWITHR

#include <Rcpp.h>
#include <vector>
#include <map>
#include "FMTlist.hpp"
#include "FMTlayer.hpp"



#define RCPP_DEFINEVECTOR(CLASS) namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj){\
const int RTYPE = traits::r_sexptype_traits<CLASS>::rtype;\
Vector<RTYPE> rcppVec(obj);\
std::vector<CLASS>newvec;\
newvec.reserve(rcppVec.size());\
for(int index = 0; index < rcppVec.size();++index)\
{\
newvec.push_back(rcppVec[index]);\
}\
return newvec;}\
template <> SEXP wrap(const std::vector<CLASS>& obj){\
const int RTYPE=traits::r_sexptype_traits<CLASS>::rtype;\
return Vector<RTYPE>(obj.begin(), obj.end());}}

#define RCPP_DEFINELIST(CLASS) namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj){\
List listObj(obj);\
std::vector<CLASS>newvec;\
newvec.reserve(listObj.size());\
for(int index = 0; index < listObj.size();++index)\
{\
newvec.push_back(as<CLASS>(listObj[index]));\
}\
return newvec;}\
template <> SEXP wrap(const std::vector<CLASS>& obj){\
List listObj(obj.size());\
for(int index = 0; index < listObj.size();++index)\
{\
listObj[index]=wrap<CLASS>(obj.at(index));\
}\
return listObj;}}

#define RCPP_DEFINEPAIR(FIRST,SECOND)namespace Rcpp{\
template <> std::pair<FIRST,SECOND> as(SEXP obj){\
Rcpp::List rcppList(obj);\
return std::pair<FIRST,SECOND>(as<FIRST>(rcppList["first"]), as<SECOND>(rcppList["second"]));}\
template <> SEXP wrap(const std::pair<FIRST,SECOND>& obj){\
return List::create(Named("first") = wrap<FIRST>(obj.first),Named("second") = wrap<SECOND>(obj.second));}}

#define RCPP_DEFINEMAP(KEY,OBJECT)namespace Rcpp{\
template <> std::map<KEY,OBJECT> as(SEXP obj){\
List rcppList(obj);\
std::map<KEY,OBJECT>newmap;\
for(int index = 0; index < rcppList.size();++index)\
{\
newmap[as<KEY>(as<List>(rcppList[index])["first"])]=as<OBJECT>(as<List>(rcppList[index])["second"]);\
}\
return newmap;}\
template <> SEXP wrap(const std::map<KEY,OBJECT>& obj){\
List rcppList(obj.size());\
size_t index = 0;\
for(const auto& mapobject: obj)\
{\
rcppList[index]=List::create(Named("first")=wrap<KEY>(mapobject.first),Named("second")=wrap<OBJECT>(mapobject.second));\
++index;\
}\
return rcppList;}}

namespace R
{
	template <class T>
	void define_FMTlist(const char* name)
		{
		Rcpp::class_< Core::FMTlist<T> >(name, "@DocString(FMTlist)")
			.constructor("@DocString(FMTlist())")
			.method("update", &Core::FMTlist<T>::update,
				"@DocString(FMTlist::update)")
			.method("push_back",
				Core::FMTlist<T>::APIpush_back,
				"@DocString(FMTyieldhandler::APIpush_back)");

		}

	template <class T>
	void define_FMTlayer(const char* name)
		{
		Rcpp::class_<Spatial::FMTlayer<T>>(name, "@DocString(FMTlayer)")
				.constructor("@DocString(FMTlayer())")
				.method("getXSize", &Spatial::FMTlayer<T>::GetXSize,
					"@DocString(FMTlayer::GetXSize)")
				.method("getYSize", &Spatial::FMTlayer<T>::GetYSize,
					"@DocString(FMTlayer::GetYSize)")
				.method("getgeotransform", &Spatial::FMTlayer<T>::getgeotransform,
					"@DocString(FMTlayer::getgeotransform)")
				.method("getprojection", &Spatial::FMTlayer<T>::getprojection,
					"@DocString(FMTlayer::getprojection)")
				.method("getmapping", &Spatial::FMTlayer<T>::getMapping,
					"@DocString(FMTlayer::getmapping)")
				.method("area", &Spatial::FMTlayer<T>::area,
					"@DocString(FMTlayer::area)")
				.method("getcellsize", &Spatial::FMTlayer<T>::getcellsize,
					"@DocString(FMTlayer::getcellsize)")
				.method("size", &Spatial::FMTlayer<T>::size,
					"@DocString(FMTlayer::size)");
		}
}



#endif



#endif // RDEFINITIONS_H_INCLUDED
