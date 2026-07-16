#include <vector>
#include <string>
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTmodel.h"
#include "FMTfreeexceptionhandler.h"
#include "FMTageyieldhandler.h"
#include "FMTmask.h"
#include "FMTdevelopment.h"


int main()
	{
        Logging::FMTDefaultLogger().logStamp();
        const std::string folder = "../../../../Examples/Models/TWD_land/";
        const std::string primarylocation = folder+"TWD_land.pri";
        const std::string outdir = "../../tests/testaddmodel/";
        Parser::FMTModelParser modelparser;
        const std::vector<std::string>scenarios(1, "LP");
        const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
        Models::FMTModel modifmodel = models.at(0);
        const std::vector<Core::FMTTheme> themes = modifmodel.getThemes();
        std::vector<Core::FMTAgeYieldHandler> newhandlers;
        const std::vector<int> bases = {0,5,10,15,20,25,30};
        std::vector<Core::FMTDevelopment> totest;
        for (const std::string& forestattributes : themes.at(1).getAttributes("?"))
        {
            if (forestattributes.find("PEUPLEMENT")==0)
            {
                const std::string maskstr = "? "+forestattributes+" ?";
                Core::FMTMask newmask = Core::FMTMask(maskstr,themes);
                Core::FMTDevelopment newDev = Core::FMTDevelopment();
                newDev.setLock(0);  
                newDev.setMask(newmask);
                std::vector<double> newvalues;
                for (const int& age : bases)
                {  
                    newDev.setAge(age+5);
                    Core::FMTYieldRequest yldrequest = newDev.getYieldRequest();
                    newvalues.push_back(modifmodel.getYields().get(yldrequest,"VOLUMETOTAL"));
                }
                Core::FMTAgeYieldHandler nhandler(newmask);
                nhandler.setYieldValues("VOLUMETOTALNEXTAGE",bases,newvalues);  
                newhandlers.push_back(nhandler);
            }
        }
        modifmodel.addYieldHandlers(newhandlers);
        Core::FMTYields modifyields = modifmodel.getYields();
        if(modifmodel.getYields().isYld("VOLUMETOTALNEXTAGE"))
        {
            for(auto& dev:totest)
            {
                Core::FMTDevelopment devcopy = dev;
                for (const int& age : bases)
                {
                    dev.setAge(age);
                    Core::FMTYieldRequest actualyldrequest = dev.getYieldRequest();
                    devcopy.setAge(age+5);
                    Core::FMTYieldRequest futuryldrequest = devcopy.getYieldRequest();
                    if(modifyields.get(actualyldrequest,"VOLUMETOTALNEXTAGE")!=modifyields.get(futuryldrequest,"VOLUMETOTAL"))
                    {
                        Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "The yields were not correctly added",
			            "testaddtomodel", __LINE__, primarylocation); 
                    }
                }
            }
        }else{
            Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Did not add the yield to model",
			"testaddtomodel", __LINE__, primarylocation);
        }
        return 0;
	}

