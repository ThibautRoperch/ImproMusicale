/**
 * Programme de conversion entre un fichier au format MusicXML et un fichier au format épuré
 * 
 * https://fr.wikipedia.org/wiki/MusicXML
 * http://www.musicxml.com/tutorial/the-midi-compatible-part/pitch/
 * score-partwise => part => measure => note => pitch (1 seul ou 0) => step & octave & alter
 * https://gist.github.com/JSchaenzle/2726944
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "../lib/rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

char* noteEntiereVersAnglais(int valeur_note_entiere) {
	static char valeur_note[2];
	if (valeur_note_entiere == 0 || valeur_note_entiere == 1) {
		valeur_note[0] = 'C';
	} else if (valeur_note_entiere == 2 || valeur_note_entiere == 3) {
		valeur_note[0] = 'D';
	} else if (valeur_note_entiere == 4) {
		valeur_note[0] = 'E';
	} else if (valeur_note_entiere == 5 || valeur_note_entiere == 6) {
		valeur_note[0] = 'F';
	} else if (valeur_note_entiere == 7 || valeur_note_entiere == 8) {
		valeur_note[0] = 'G';
	} else if (valeur_note_entiere == 9 || valeur_note_entiere == 10) {
		valeur_note[0] = 'A';
	} else if (valeur_note_entiere == 11) {
		valeur_note[0] = 'B';
	}
	if (valeur_note_entiere == 1 || valeur_note_entiere == 3 || valeur_note_entiere == 6 || valeur_note_entiere == 8 || valeur_note_entiere == 10) {
		valeur_note[1] = '1';
	} else {
		valeur_note[1] = '0';
	}
	return valeur_note;
}

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 2) {
		cerr << "Donner en argument le fichier contenant la mélodie au format XML et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}
	
	/* Lecture du fichier contenant la partition au format XML */

	cout << "Lecture de la mélodie contenue dans le fichier " << argv[1] << " au format XML" << endl;

	xml_document<> doc;
	xml_node<> * noeud_racine;

	// Initialisation du vecteur contenant les noeuds du fichier

	ifstream theFile(argv[1]);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	// Lecture de la mélodie, note par note

	noeud_racine = doc.first_node("notes");
	string res = "<partwise>\n";

	// Itération sur les notes (noeuds "note" du noeud "notes")
	for (xml_node<> * noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
		res += "  <pitch>";

		char* valeur_note = noteEntiereVersAnglais(stoi(noeud_note->first_node("valeur")->value()));
		int octave_note = atoi(noeud_note->first_node("octave")->value());

		if (valeur_note[1] != 0) {
			res += "    <alter>" + to_string(valeur_note[1]) + "</alter>\n";
		}

		res += "    <octave>" + to_string(octave_note) + "</octave>\n";

		res += "  </pitch>\n";
	}
	
	cout << "\nLecture de la mélodie terminée" << endl;

	// Fin de la lecture du fichier contenant la partition

	res += "</partwise>";

	cout << "\nLecture terminée" << endl;

	/* Enregistrement de la mélodie extraite dans fichier de sortie ou affichage à défaut de fichier donné en argument */
	 
	if (argc == 3) {
		string nom_fichier_sortie = argv[2];
		ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
		
		if(fichier_sortie) {
			fichier_sortie << res;
			fichier_sortie.close();
		} else {
			cout << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
		}

		cout << "\nLa mélodie du fichier " << argv[1] << " est enregistrée comme partition dans le fichier " << nom_fichier_sortie << " au format MusicXML" << endl;
	} else {
		cout << "\nAucun fichier de sortie n'est donné en argument du programme" << endl;
		cout << "\n" << res << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}
