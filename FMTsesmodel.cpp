#include "FMTsesmodel.h"

namespace Models
    {
    FMTsesmodel::FMTsesmodel(): FMTmodel(),mapping(), operatedschedule(),disturbances(),spactions()
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTsesmodel& rhs):
        FMTmodel(rhs),
        mapping(rhs.mapping),
		operatedschedule(rhs.operatedschedule),
        disturbances(rhs.disturbances),
        spactions(rhs.spactions)
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        mapping(), operatedschedule(),disturbances(),spactions()
        {

        }
    FMTsesmodel& FMTsesmodel::operator = (const FMTsesmodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            mapping = rhs.mapping;
			operatedschedule = rhs.operatedschedule;
            disturbances = rhs.disturbances;
            spactions = rhs.spactions;
            }
        return *this;
        }
    FMTforest FMTsesmodel::getmapping() const
        {
        return mapping;
        }
    bool FMTsesmodel::setspactions(const vector<FMTspatialaction>& lspactions)
        {
		vector<FMTtransition>newtransitions;
		vector<FMTspatialaction>newspatials;
		vector<FMTaction>newbaseactions;
		for (const FMTspatialaction& spaction : lspactions)
			{
			vector<FMTtransition>::const_iterator trn_iterator = find_if(transitions.begin(), transitions.end(), FMTtransitioncomparator(spaction.name));
			if (trn_iterator!= transitions.end())
				{
				newtransitions.push_back(*trn_iterator);
				newspatials.push_back(spaction);
				newbaseactions.push_back(spaction);
			}else{
				_exhandler->raise(FMTexc::WSinvalid_transition,
					FMTwssect::Transition, "Missing transition case for action : " + spaction.name,__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
        }
    bool FMTsesmodel::setinitialmapping(const FMTforest& forest)
        {
        disturbances = FMTdisturbancestack();
        mapping = forest;
		mapping.setperiod(1);
		return true;
        }

    FMTdisturbancestack FMTsesmodel::getdisturbances() const
        {
        return disturbances;
        }


    map<string,double> FMTsesmodel::simulate(const FMTschedule& schedule,
                             bool schedule_only,
                             unsigned int seed)
        {
		FMTschedule newschedule(static_cast<int>(disturbances.data.size()+1),map<FMTaction, map<FMTdevelopment, vector<double>>>());
        default_random_engine generator(seed);
        const double total_area = schedule.area();
        map<string,double>targets;
        for(map<FMTaction,map<FMTdevelopment,vector<double>>>::const_iterator ait = schedule.elements.begin(); ait!=schedule.elements.end(); ait++)
            {
            targets[ait->first.name] = schedule.actionarea(ait->first);
            }
        disturbances.push(map<string,vector<FMTevent<FMTdevelopment>>>());
		double allocated_area = 0;
		if (!schedule.elements.empty() || !schedule_only)
			{
			double pass_allocated_area = 0;
			bool schedulepass = true;
			int pass = 0;
			int action_pass = 0;
			boost::unordered_map<FMTdevelopment, vector<bool>>cached_operability;
			vector<boost::unordered_map<FMTdevelopment,FMTdevelopment>>cached_operated(spactions.size(), boost::unordered_map<FMTdevelopment, FMTdevelopment>());
			//unordered_map<FMTdevelopment, map<FMTtransition, bool>>cached_operated;
			do {
				pass_allocated_area = 0;
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " GETTING " << "\n";
				const clock_t begin_time = clock();
				map<FMTaction,FMTforest> forests  = mapping.getschedule(schedule, cached_operability,yields,schedulepass);//pass in cached operability!!!
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "GETTING TOOK "<< float(clock() - begin_time) / CLOCKS_PER_SEC << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "ENTERING SIMULATION" << "\n";
				for (const FMTspatialaction& spatial_action : spactions) //the order is realing important here!
					{
					vector<FMTspatialaction>::iterator acit = std::find_if(spactions.begin(), spactions.end(), FMTactioncomparator(spatial_action.name));
					if (acit != spactions.end())
						{
						const double action_area = targets[acit->name];
						FMTforest* allowable_forest = &forests[spatial_action];
						if (allowable_forest->area() > 0 && action_area > 0)
							{
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Before allow " << spatial_action.name << " " << allowable_forest->area() << "\n";
							size_t location = std::distance(spactions.begin(), acit);
							const FMTforest spatialy_allowable = allowable_forest->getallowable(*acit, disturbances);
							if (spatialy_allowable.area()>0)
								{
								//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "spallow " << spatial_action.name << " " << spatialy_allowable.area() << "\n";
								vector<FMTevent<FMTdevelopment>>events = spatialy_allowable.buildharvest(action_area, *acit, generator,action_pass);
								if (events.size() > 0)
									{ 
									//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "GOT EVENs " << events.size() << "\n";
									FMTforest newoperated = spatialy_allowable.operate(events, *acit, transitions[location], yields, themes,cached_operated[location], newschedule); //can also use caching here...
									//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OPERATED " << spatial_action.name << " " << newoperated.area() << "\n";
									if (!newoperated.mapping.empty())
										{
										disturbances.add(acit->name, events);
										//Fill the schedule here base on operated forest and acit!


										mapping.replace(newoperated.mapping.begin(), newoperated.mapping.end());
										targets[acit->name] -= (newoperated.area());
										pass_allocated_area += (newoperated.area());
										}
									}
								}
							}
						}
					++action_pass;
					}
				allocated_area += pass_allocated_area;
				++pass;
				if(!schedule_only && pass_allocated_area == 0)
					{
					if(schedulepass)
						{
						schedulepass = false;
						}else{
						schedule_only = true;
						}
					}
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " ALLOCATED " << allocated_area << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " TOTAL "<< total_area << "\n";
				}while(allocated_area<total_area&&(pass_allocated_area!=0||(!schedule_only)));
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " GROWING " << "\n";
        mapping = mapping.grow();
		map<string, double>results;
		results["Total"] = allocated_area / total_area;
		for (map<FMTaction, map<FMTdevelopment, vector<double>>>::const_iterator ait = schedule.elements.begin(); ait != schedule.elements.end(); ait++)
			{
			double total_action_area = schedule.actionarea(ait->first);
			results[ait->first.name] = ((total_action_area - targets[ait->first.name]) / total_action_area);
			}
		operatedschedule.push_back(newschedule);
        return results;
        }


	vector<FMTschedule> FMTsesmodel::getschedule() const
		{
		return operatedschedule;
		//return disturbances.getlastschedule(mapping.getcellsize(),actions,mapping);
		}

	string FMTsesmodel::getdisturbancestats() const
		{
		return disturbances.getpatchstats();
		}
    }
