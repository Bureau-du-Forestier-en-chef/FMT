#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#endif

int main()
{
#ifdef FMTWITHOSI

	// INPUTS
	const std::string INPUT1 = "*SOURCE ~FMT0A_3 FORP TOUTES GSR0005 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAPAR _AREA * 1.000000 + ~FMT0A_11 FORP 2 + GSR0014 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAREG _AREA * 2.000000 + ~FMT0A_11 FORP 2 + GSR0013 ? NAT ? INC ? ? NA NA NA ? ";
	std::cout << "Inputs 1 : " << INPUT1 << std::endl;
	const std::string INPUT2 = "*SOURCE ~FMT0A_3 FORP TOUTES GSR0005 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAPAR _AREA fghdfgh 1.000000 dfghdfghdfgh ~FMT0A_11 FORP 2 dfghdfghdfghdfgh GSR0014 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAREG _AREA * 2.000000 dfghdfghdfgh ~FMT0A_11 FORP 2 dfghdfghdfgh GSR0013 ? NAT ? INC ? ? NA NA NA ? ";
	std::cout << "Inputs 2 : " << INPUT2 << std::endl;

	// Résultats attendu
	const std::string EXPECTEDOUT1 = "*SOURCE ~FMT0A_3 FORP TOUTES GSR0005 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAPAR _AREA * 1.000000 +\n~FMT0A_11 FORP 2 +\nGSR0014 ? NAT ? INC ? ? NA NA NA ? ? NA NA ? ? NA REGAREG _AREA * 2.000000 +\n~FMT0A_11 FORP 2 +\nGSR0013 ? NAT ? INC ? ? NA NA NA ? ";
	std::cout << "Exepected results 1 : " << EXPECTEDOUT1 << std::endl;
	bool errorCatched = false;
	bool sameResult = true;
	std::cout << "Exepected result 2 : raiseError" << std::endl;


	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);

	// Pour test la fonction au dessus
	std::vector<Core::FMToutputsource> sources;
	std::vector<Core::FMToperator> operators;

	Core::FMToutput newoutput("naturalgrowth", "", "", sources, operators);
	std::string result1 = newoutput.splitSource(INPUT1);
	try
	{
		newoutput.splitSource(INPUT2);
	}
	catch (...)
	{
		errorCatched = true;
	}

	if(EXPECTEDOUT1.size() == result1.size())
	{
		for (int i = 0; i < std::int32_t(EXPECTEDOUT1.size()); ++i)
		{
			if (EXPECTEDOUT1[i] != result1[i])
			{
				std::cout << "Difference entre le caratere a la position " + std::to_string(i) + " " + EXPECTEDOUT1[i] + " : " + result1[i] << std::endl;
				sameResult = false;
			}
		}
	}
	else
	{
		sameResult = false;
	}
	if (sameResult)
	{
		std::cout << "Fonctionnement normal dans le cas d'une ligne valide" << std::endl;
	}
	else
	{
		std::cout << "Differencence entre le resultat attendu et la sortie de la fonction\nResultat attendu : " << EXPECTEDOUT1 << "\nResultat reçus : " << result1 << std::endl;
	}
	if(errorCatched)
	{
		std::cout << "L'erreur a bien ete lancé lors d'une chaine trop longue" << std::endl;
	}
	else
	{
		std::cout << "L'erreur n'a pas ete lancé lors d'une chaine trop longue" << std::endl;
	}


#endif
	return 0;
}