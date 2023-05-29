#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <functional>
#include <exception>

using namespace std;

#include "ppm.h"

int convert(const string& sInput, const string& sOutput) {

	// Dal file UBJ devo estrarre le informazioni e creare il canvas

	unsigned w = 100; // TODO : modificare
	unsigned h = 100; // TODO : modificare

	image<vec3b> img(w, h);

	// Per accedere ai pixel di img posso usare img(x,y) oppure img.begin() e img.end()

	// Dal file UBJ devo estrarre le informazioni sulle immagini da incollare su img 

	// Output in formato PPM
	if (!writeP6(sOutput, img))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

	// TODO : gestire la linea di comando

	string sInput = "caso01.ubj";
	string sOutput = "caso01.ppm";

	return convert(sInput, sOutput);
}