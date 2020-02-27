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

#ifndef FMTareaparser_H_INCLUDED
#define FMTareaparser_H_INCLUDED

#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTparser.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include "FMTGCBMtransition.h"
#include <iterator>
#include "FMToperatingarea.h"
#include <map>
#include <string>
#include <vector>
#include "FMTutility.h"
#include <regex>
#include "FMTconstants.h"
#include "FMTtheme.h"

namespace Parser
{

class FMTareaparser : public FMTparser
    {
    private:
        std::regex rxcleanarea;
		std::string getdisturbancepath(const std::string& location, const int& period) const;
		std::vector<Core::FMTGCBMtransition> getGCBMtransitions(const Spatial::FMTlayer<std::string>& stacked_actions,
													const Spatial::FMTlayer<int>& ages,
													const Spatial::FMTforest& newfor,
													const std::vector<Core::FMTtheme>& themes) const;
		#ifdef FMTWITHGDAL
			Core::FMTactualdevelopment getfeaturetodevelopment(const OGRFeature* feature,
															const std::vector<Core::FMTtheme>& themes,
															const std::map<int, int>& themes_fields,
															const int& age_field, 
															const int& lock_field,
															const int& area_field,
															const double& agefactor,
															const double& areafactor,
															const double& minimalarea) const;
			void validate_raster(const std::vector<std::string>&data_rasters) const;
			GDALDataset* openvectorfile(std::map<int, int>&themes_fields, int& age_field, int& lock_field, int& area_field,
				const std::string& data_vectors, const std::string& agefield, const std::string& areafield, const std::string& lockfield,
				const std::vector<Core::FMTtheme>& themes) const;
			#ifdef FMTWITHOSI
			std::vector<OGRMultiPolygon>getmultipolygons(const std::vector<Heuristics::FMToperatingarea>& operatingareas,
											  const std::vector<Core::FMTtheme>& themes, const std::string& data_vectors,
											  const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
											  double areafactor = 1, std::string lockfield = "",
											  double minimal_area = 0.0) const;
			std::vector<Heuristics::FMToperatingarea> getneighborsfrompolygons(const std::vector<OGRMultiPolygon>& multipolygons,
																			std::vector<Heuristics::FMToperatingarea> operatingareas,
																	const double& buffersize) const;
			#endif
			OGRLayer* subsetlayer(OGRLayer*layer, const std::vector<Core::FMTtheme>& themes,
								const std::string& agefield, const std::string& areafield) const;
		
    public:
        Spatial::FMTforest readrasters(const std::vector<Core::FMTtheme>& themes,const std::vector<std::string>&data_rasters,
                             const std::string& age,double agefactor = 1.0,double areafactor = 0.0001, std::string lock = "") const;
        template<typename T>
        bool writelayer(const Spatial::FMTlayer<T>& layer,std::string location,const std::map<T,std::string>& mapping) const;
        bool writeforest(const Spatial::FMTforest& for_layer,
                         const std::vector<Core::FMTtheme>& themes,
                         const std::vector<std::string>&data_rasters,
                         const std::string& age,
                         const std::string& lock,
						std::vector<std::map<std::string, std::string>> mapping = std::vector<std::map<std::string, std::string>>()) const;
		std::vector<Core::FMTGCBMtransition> writedisturbances(const std::string& location,
								const Spatial::FMTdisturbancestack& disturbances,
								const Spatial::FMTforest& for_layer,
								const Spatial::FMTforest& out_layer,
								const std::vector<Core::FMTtheme>& themes,
								std::map<std::string, std::string> mapping = std::map<std::string, std::string>()) const;
		#ifdef FMTWITHOSI
		std::vector<Heuristics::FMToperatingarea> getneighbors(std::vector<Heuristics::FMToperatingarea> operatingareaparameters,
						const std::vector<Core::FMTtheme>& themes,const std::string& data_vectors,
						const std::string& agefield, const std::string& areafield, double agefactor = 1.0,
						double areafactor = 1, std::string lockfield = "",
						double minimal_area = 0.0,double buffersize= 100) const;
		#endif
		std::vector<Core::FMTactualdevelopment>readvectors(const std::vector<Core::FMTtheme>& themes,const std::string& data_vectors,
                                   const std::string& agefield,const std::string& areafield,double agefactor = 1.0,
                                   double areafactor = 1, std::string lockfield = "",
								   double minimal_area = 0.0) const;
		#endif
	public:
		FMTareaparser();
		FMTareaparser(const FMTareaparser& rhs);
		FMTareaparser& operator = (const FMTareaparser& rhs);
		std::vector<Core::FMTactualdevelopment>read(const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants, std::string location);
        bool write(const std::vector<Core::FMTactualdevelopment>& areas, std::string location);
		
    };
}
#endif // FMTareaparser_H_INCLUDED
