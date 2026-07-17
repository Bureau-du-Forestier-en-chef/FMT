/*
Copyright (c) 2019 Gouvernement du Québec

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
		Rcpp::class_< Core::FMTList<T> >(name, "@DocString(FMTList)")
			.constructor("@DocString(FMTList())")
			.method("update", &Core::FMTList<T>::update,
				"@DocString(FMTList::update)")
			.method("push_back",
				Core::FMTList<T>::apiPushBack,
				"@DocString(FMTYieldHandler::APIpush_back)");

		}

	template <class T>
	void define_FMTlayer(const char* name)
		{
		Rcpp::class_<Spatial::FMTLayer<T>>(name, "@DocString(FMTLayer)")
				.constructor("@DocString(FMTLayer())")
				.method("getXSize", &Spatial::FMTLayer<T>::getXSize,
					"@DocString(FMTLayer::GetXSize)")
				.method("getYSize", &Spatial::FMTLayer<T>::getYSize,
					"@DocString(FMTLayer::GetYSize)")
				.method("getgeotransform", &Spatial::FMTLayer<T>::getGeoTransform,
					"@DocString(FMTLayer::getgeotransform)")
				.method("getprojection", &Spatial::FMTLayer<T>::getProjection,
					"@DocString(FMTLayer::getprojection)")
				.method("getmapping", &Spatial::FMTLayer<T>::getMapping,
					"@DocString(FMTLayer::getmapping)")
				.method("area", &Spatial::FMTLayer<T>::area,
					"@DocString(FMTLayer::area)")
				.method("getcellsize", &Spatial::FMTLayer<T>::getCellSize,
					"@DocString(FMTLayer::getcellsize)")
				.method("size", &Spatial::FMTLayer<T>::size,
					"@DocString(FMTLayer::size)");
		}
}



#endif



#endif // RDEFINITIONS_H_INCLUDED
