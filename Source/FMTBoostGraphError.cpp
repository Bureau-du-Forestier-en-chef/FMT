#include "FMTBoostGraphError.h"

namespace Exception
	{
	FMTBoostGraphError::FMTBoostGraphError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTboostgrapherror,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La bibliothèque Boost Graph a signalé une erreur.",
				"The Boost Graph library reported an error.")
		{

		}

	FMTBoostGraphError::FMTBoostGraphError(const boost::bad_graph& p_GraphError):
		FMTBoostGraphError(Core::FMTsection::Empty, p_GraphError.what(),
			"FMTBoostGraphError::FMTBoostGraphError",__FILE__,__LINE__)
	{

	}


	}