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
        Logging::FMTdefaultlogger().logstamp();
        const std::string folder = "../../../../Examples/Models/TWD_land/";
        const std::string primarylocation = folder+"TWD_land.pri";
        const std::string outdir = "../../tests/testaddmodel/";
        Parser::FMTmodelparser modelparser;
        const std::vector<std::string>scenarios(1, "LP");
        const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
        Models::FMTmodel modifmodel = models.at(0);
        const std::vector<Core::FMTtheme> themes = modifmodel.getthemes();
        std::vector<Core::FMTageyieldhandler> newhandlers;
        const std::vector<int> bases = {0,5,10,15,20,25,30};
        std::vector<Core::FMTdevelopment> totest;
        for (const std::string& forestattributes : themes.at(1).getAttributes("?"))
        {
            if (forestattributes.find("PEUPLEMENT")==0)
            {
                const std::string maskstr = "? "+forestattributes+" ?";
                Core::FMTmask newmask = Core::FMTmask(maskstr,themes);
                Core::FMTdevelopment newDev = Core::FMTdevelopment();
                newDev.setLock(0);  
                newDev.setMask(newmask);
                std::vector<double> newvalues;
                for (const int& age : bases)
                {  
                    newDev.setAge(age+5);
                    Core::FMTyieldrequest yldrequest = newDev.getYieldRequest();
                    newvalues.push_back(modifmodel.getyields().get(yldrequest,"VOLUMETOTAL"));
                }
                Core::FMTageyieldhandler nhandler(newmask);
                nhandler.setYieldValues("VOLUMETOTALNEXTAGE",bases,newvalues);  
                newhandlers.push_back(nhandler);
            }
        }
        modifmodel.addYieldHandlers(newhandlers);
        Core::FMTyields modifyields = modifmodel.getyields();
        if(modifmodel.getyields().isYld("VOLUMETOTALNEXTAGE"))
        {
            for(auto& dev:totest)
            {
                Core::FMTdevelopment devcopy = dev;
                for (const int& age : bases)
                {
                    dev.setAge(age);
                    Core::FMTyieldrequest actualyldrequest = dev.getYieldRequest();
                    devcopy.setAge(age+5);
                    Core::FMTyieldrequest futuryldrequest = devcopy.getYieldRequest();
                    if(modifyields.get(actualyldrequest,"VOLUMETOTALNEXTAGE")!=modifyields.get(futuryldrequest,"VOLUMETOTAL"))
                    {
                        Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "The yields were not correctly added",
			            "testaddtomodel", __LINE__, primarylocation); 
                    }
                }
            }
        }else{
            Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Did not add the yield to model",
			"testaddtomodel", __LINE__, primarylocation);
        }
        return 0;
	}

