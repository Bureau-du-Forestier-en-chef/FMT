#include "FMTNotLineGraph.h"

namespace Exception
	{
	FMTNotLineGraph::FMTNotLineGraph(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTnotlinegraph,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le graphe fourni n'est pas un graphe linéaire valide.",
				"The supplied graph is not a valid line graph.")
		{
		}
	}