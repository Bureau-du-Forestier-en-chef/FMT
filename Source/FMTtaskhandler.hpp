/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASKHANDLER_H_INCLUDED
#define FMTTASKHANDLER_H_INCLUDED
#include <memory>
#include "FMTobject.hpp"

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTtask;
	// DocString: FMTtaskhandler
	/**
	Main class for running concurrent tasks at the same time.
	It uses the virtual function split() from FMTtask to split the task in multiple task and can run it concurrently
	*/
	class FMTEXPORT FMTtaskhandler : public Core::FMTobject
	{
	std::vector<std::unique_ptr<FMTtask>>alltasks;
	public:
		// DocString: FMTtaskhandler::FMTtaskhandler(const std::unique_ptr<FMTtask>&,unsigned int)
		/**
		Construct the handler with the number of threads you want. Will use the split task virtual function to
		generate all the tasks at the same time.
		*/
		FMTtaskhandler(const std::unique_ptr<FMTtask>& maintask,
						unsigned int maxthread = 0);
		// DocString: FMTtaskhandler::FMTtaskhandler()
		/**
		Default constructor for FMTtaskhandler
		*/
		FMTtaskhandler() = default;
		// DocString: FMTtaskhandler::FMTtaskhandler(const FMTtaskhandler)
		/**
		Default copy constructor for FMTtaskhandler
		*/
		FMTtaskhandler(const FMTtaskhandler& rhs) = default;
		// DocString: FMTtaskhandler::operator=
		/**
		Default copy assignement for FMTtaskhandler
		*/
		FMTtaskhandler& operator =(const FMTtaskhandler& rhs) = default;
		// DocString: FMTtaskhandler::gettasks()
		/**
		Return reference to the task handled by the task handler.
		*/
		const std::vector<std::unique_ptr<FMTtask>>& gettasks() const;
		// DocString: FMTtaskhandler::gettasksfromdynamiccast()
		/**
		Return reference to the task already in the ptrype.
		*/
		template<class ptrtype>
		const std::vector<const ptrtype*> gettasksfromdynamiccast() const;
		// DocString: FMTtaskhandler::conccurentrun()
		/**
		Run the task vector conccurently
		*/
		void conccurentrun();



	};

}

#endif
