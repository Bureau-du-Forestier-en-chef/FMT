

#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#include "FMTversion.h"
#include <iostream>
#ifdef FMTWITHGDAL
#include <ogrsf_frmts.h> 


namespace Testing
{

	class UnitTestFMTareaparser
	{
	public:
		UnitTestFMTareaparser() : m_AreaParser()
		{

		}
		void runAllTests()
		{
		#ifdef FMTWITHGDAL
					testGetGeometryAreaWithValidPolygon();
					testGetGeometryAreaWithNullptrAndLineString();
					testGetMultiPolygonTypeCasting();
		#else
					std::cout << "UnitTestFMTareaparser bypassed: FMTWITHGDAL is not defined" << std::endl;
		#endif
		}

	#ifdef FMTWITHGDAL
	private:
		void testGetGeometryAreaWithValidPolygon()
		{
			OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(
				OGRGeometryFactory::createGeometry(wkbPolygon));

			OGRLinearRing ring;
			ring.addPoint(0.0, 0.0);
			ring.addPoint(10.0, 0.0);
			ring.addPoint(10.0, 10.0);
			ring.addPoint(0.0, 10.0);
			ring.addPoint(0.0, 0.0);
			polygon->addRing(&ring);

			const double EXPECTED_AREA = 100.0;
			// Testing private method allowed via macro access override
			const double CALCULATED_AREA = m_AreaParser._GetGeometryArea(polygon);

			OGRGeometryFactory::destroyGeometry(polygon);

			if (std::abs(CALCULATED_AREA - EXPECTED_AREA) > 0.0001)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"Calculated geometry area does not match expected square area",
					"UnitTestFMTareaparser::testGetGeometryAreaWithValidPolygon", __LINE__, __FILE__);
			}
			std::cout << "UnitTestFMTareaparser::testGetGeometryAreaWithValidPolygon passed" << std::endl;
		}

		void testGetGeometryAreaWithNullptrAndLineString()
		{
			// Testing private method allowed via macro access override
			const double NULL_AREA = m_AreaParser._GetGeometryArea(nullptr);
			if (NULL_AREA != 0.0)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"Nullptr geometry should return 0.0 area",
					"UnitTestFMTareaparser::testGetGeometryAreaWithNullptrAndLineString", __LINE__, __FILE__);
			}

			OGRLineString* line = dynamic_cast<OGRLineString*>(
				OGRGeometryFactory::createGeometry(wkbLineString));
			line->addPoint(0.0, 0.0);
			line->addPoint(10.0, 10.0);

			// Testing private method allowed via macro access override
			const double LINE_AREA = m_AreaParser._GetGeometryArea(line);
			OGRGeometryFactory::destroyGeometry(line);

			if (LINE_AREA != 0.0)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"LineString geometry should return 0.0 surface area",
					"UnitTestFMTareaparser::testGetGeometryAreaWithNullptrAndLineString", __LINE__, __FILE__);
			}
			std::cout << "UnitTestFMTareaparser::testGetGeometryAreaWithNullptrAndLineString passed" << std::endl;
		}

		void testGetMultiPolygonTypeCasting()
		{
			OGRMultiPolygon* multipoly = dynamic_cast<OGRMultiPolygon*>(
				OGRGeometryFactory::createGeometry(wkbMultiPolygon));

			// Testing private method allowed via macro access override
			const OGRMultiPolygon* castedResult = m_AreaParser._GetMultiPolygon(multipoly);

			if (castedResult == nullptr)
			{
				OGRGeometryFactory::destroyGeometry(multipoly);
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"Failed to cast a valid wkbMultiPolygon object",
					"UnitTestFMTareaparser::testGetMultiPolygonTypeCasting", __LINE__, __FILE__);
			}

			OGRPoint* point = dynamic_cast<OGRPoint*>(
				OGRGeometryFactory::createGeometry(wkbPoint));

			// Testing private method allowed via macro access override
			const OGRMultiPolygon* invalidCastResult = m_AreaParser._GetMultiPolygon(point);

			OGRGeometryFactory::destroyGeometry(multipoly);
			OGRGeometryFactory::destroyGeometry(point);

			if (invalidCastResult != nullptr)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"Point geometry cast should have returned nullptr",
					"UnitTestFMTareaparser::testGetMultiPolygonTypeCasting", __LINE__, __FILE__);
			}

			std::cout << "UnitTestFMTareaparser::testGetMultiPolygonTypeCasting passed" << std::endl;
		}
#endif

	private:
		Parser::FMTareaparser m_AreaParser;

	};

}


int main()
{
	Logging::FMTdefaultlogger().logstamp();
	Testing::UnitTestFMTareaparser test;
	test.runAllTests();
	return 0;
}

