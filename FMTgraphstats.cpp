#include "FMTgraphstats.h"


namespace Graph {

	FMTgraphstats::FMTgraphstats():
		cols(0),
		rows(0),
		vertices(0),
		edges(0),
		transfer_rows(0),
		output_rows(0),
		output_cols(0)
		{

		}

	FMTgraphstats::FMTgraphstats(const unique_ptr<OsiSolverInterface>& solverinterface,
		const FMTadjacency_list& graph, int ltransfer_rows, int loutput_rows, int loutput_cols):
		cols(solverinterface->getNumCols()),
		rows(solverinterface->getNumRows()),
		vertices(num_vertices(graph)),
		edges(num_edges(graph)),
		transfer_rows(ltransfer_rows),
		output_rows(loutput_rows),
		output_cols(loutput_cols)
		{

		}

	FMTgraphstats::FMTgraphstats(const FMTgraphstats& rhs):
		cols(rhs.cols),
		rows(rhs.rows),
		vertices(rhs.vertices),
		edges(rhs.edges),
		transfer_rows(rhs.transfer_rows),
		output_rows(rhs.output_rows),
		output_cols(rhs.output_cols)
		{

		}

	FMTgraphstats& FMTgraphstats::operator = (const FMTgraphstats& rhs)
		{
		if (this!=&rhs)
			{
			cols = rhs.cols;
			rows = rhs.rows;
			vertices = rhs.vertices;
			edges = rhs.edges;
			transfer_rows = rhs.transfer_rows;
			output_rows = rhs.output_rows;
			output_cols = rhs.output_cols;
			}
		return *this;
		}

	FMTgraphstats& FMTgraphstats::operator += (const FMTgraphstats& rhs)
		{
		cols += rhs.cols;
		rows += rhs.rows;
		vertices += rhs.vertices;
		edges +=rhs.edges;
		transfer_rows += rhs.transfer_rows;
		output_rows += rhs.output_rows;
		output_cols += rhs.output_cols;
		return *this;
		}
	FMTgraphstats& FMTgraphstats::operator -= (const FMTgraphstats& rhs)
	{
		cols -= rhs.cols;
		rows -= rhs.rows;
		vertices -= rhs.vertices;
		edges -= rhs.edges;
		transfer_rows -= rhs.transfer_rows;
		output_rows -= rhs.output_rows;
		output_cols -= rhs.output_cols;
		return *this;
	}
	FMTgraphstats FMTgraphstats::operator + (const FMTgraphstats& rhs)
		{
		*this += rhs;
		return *this;
		}

	FMTgraphstats FMTgraphstats::operator - (const FMTgraphstats& rhs)
		{
		*this -= rhs;
		return *this;
		}

	FMTgraphstats::operator string() const
		{
		string values = "";
		values += "Columns: " + to_string(cols);
		values += " Rows: " + to_string(rows);
		values += " Verticies: " + to_string(vertices);
		values += " Edges: " + to_string(edges);
		values += " Transfer Rows: " + to_string(transfer_rows);
		values += " Output Rows: " + to_string(output_rows);
		values += " Output Columns: " + to_string(output_cols);
		return values;
		}

	bool FMTgraphstats::operator == (const FMTgraphstats& rhs) const
		{
		return (cols == rhs.cols && rows == rhs.rows &&
			vertices == rhs.vertices && edges == rhs.edges && 
			transfer_rows == rhs.transfer_rows && output_rows == rhs.output_rows &&
			output_cols == rhs.output_cols);
		}
	bool FMTgraphstats::operator != (const FMTgraphstats& rhs) const
		{
		return (!(*this == rhs));
		}

}

