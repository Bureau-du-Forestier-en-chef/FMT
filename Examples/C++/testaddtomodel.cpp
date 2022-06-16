#include <vector>
#include <string>
#include "FMTmodelparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTmodel.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include "FMTageyieldhandler.hpp"
#include "FMTmask.hpp"
#include "FMTdevelopment.hpp"


int main()
	{
        Logging::FMTlogger().logstamp();
        const std::string folder = "../../../../Examples/Models/TWD_land/";
        const std::string primarylocation = folder+"TWD_land.pri";
        const std::string outdir = "tests/testaddmodel/";
        Parser::FMTmodelparser modelparser;
        const std::vector<std::string>scenarios(1, "LP");
        const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
        Models::FMTmodel modifmodel = models.at(0);
        const std::vector<Core::FMTtheme> themes = modifmodel.getthemes();
        std::vector<Core::FMTageyieldhandler> newhandlers;
        const std::vector<int> bases = {0,5,10,15,20,25,30};
        std::vector<Core::FMTdevelopment> totest;
        for (const std::string& forestattributes : themes.at(1).getattributes("?"))
        {
            if (forestattributes.find("PEUPLEMENT")==0)
            {
                const std::string maskstr = "? "+forestattributes+" ?";
                Core::FMTmask newmask = Core::FMTmask(maskstr,themes);
                Core::FMTdevelopment newdev = Core::FMTdevelopment();
                newdev.setlock(0);  
                newdev.setmask(newmask);
                std::vector<double> newvalues;
                for (const int& age : bases)
                {  
                    newdev.setage(age+5);
                    Core::FMTyieldrequest yldrequest = newdev.getyieldrequest();
                    newvalues.push_back(modifmodel.getyields().get(yldrequest,"VOLUMETOTAL"));
                }
                Core::FMTageyieldhandler nhandler(newmask);
                nhandler.setyieldvalues("VOLUMETOTALNEXTAGE",bases,newvalues);  
                newhandlers.push_back(nhandler);
            }
        }
        modifmodel.addyieldhandlers(newhandlers);
        Core::FMTyields modifyields = modifmodel.getyields();
        if(modifmodel.getyields().isyld("VOLUMETOTALNEXTAGE"))
        {
            for(auto& dev:totest)
            {
                Core::FMTdevelopment devcopy = dev;
                for (const int& age : bases)
                {
                    dev.setage(age);
                    Core::FMTyieldrequest actualyldrequest = dev.getyieldrequest();
                    devcopy.setage(age+5);
                    Core::FMTyieldrequest futuryldrequest = devcopy.getyieldrequest();
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

