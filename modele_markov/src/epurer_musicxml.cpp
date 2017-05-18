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
#include <map>
#include <cstdlib>

#include "../lib/rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

int noteAnglaiseVersEntier(char valeur_note_en) {
	int valeur_note = 0;
	switch (valeur_note_en) {
		case 'A':
			valeur_note = 9;
			break;
		case 'B':
			valeur_note = 11;
			break;
		case 'C':
			valeur_note = 0;
			break;
		case 'D':
			valeur_note = 2;
			break;
		case 'E':
			valeur_note = 4;
			break;
		case 'F':
			valeur_note = 5;
			break;
		case 'G':
			valeur_note = 7;
			break;
	}
	return valeur_note;
}

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 2) {
		cerr << "Donner en argument le fichier contenant la partition au format MusicXML et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}
	
	/* Lecture du fichier contenant la partition au format MusicXML */

	cout << "Lecture de la partition contenue dans le fichier " << argv[1] << " au format MusicXML" << endl;

	string res = "";
	xml_document<> doc;
	xml_node<> * noeud_racine;
	string structure_partition;

	// Initialisation du vecteur contenant les noeuds du fichier

	ifstream theFile(argv[1]);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	// Détection du type de la structure du fichier MusicXML (2 DTD/phylosophies différentes)

	// Les parties musicales (une partie par instrument) sont primaires, et les mesures sont contenues dans chaque partie (duplication des 'number' des 'measure')
	if (doc.first_node("score-partwise")) {
		noeud_racine = doc.first_node("score-partwise");
		structure_partition = "partwise";
	}
	// Les mesures sont primaires, et les parties musicales sont contenues dans chaque mesure (duplication des 'id' des 'part')
	else if (doc.first_node("score-timewise")) {
		noeud_racine = doc.first_node("score-timewise");
		structure_partition = "timewise";
	}

    res += "<notes>\n";

	// Analyse des parties musicales présentes la partition

	map<string, string> parties_musicales;

	xml_node<> * noeud_liste_parties = noeud_racine->first_node("part-list");

	// Itération sur les parties (noeuds "score-part" du noeud "part-list")
	for (xml_node<> * noeud_partie = noeud_liste_parties->first_node("score-part"); noeud_partie; noeud_partie = noeud_partie->next_sibling()) {
		string nom_noeud = noeud_partie->name();
		if (nom_noeud.compare("score-part") == 0) {
			parties_musicales[noeud_partie->first_attribute("id")->value()] = noeud_partie->first_node("part-name")->value();
		}
	}

	string partie_musicale_selectionnee = parties_musicales.begin()->first;
	
	if (parties_musicales.size() > 1) {
		cout << "\nPlusieurs parties musicales sont présentes dans la partition :" << endl;
		int i = 0;
		for (auto partie : parties_musicales) {
			cout << "  [" << i++ << "] " << partie.second << endl;
		}
		string indice_partie;
		cout << "\nDonner l'indice de la partie dont il faut extraire la mélodie :" << endl;
		cin >> indice_partie;
		i = 0;
		for (auto partie : parties_musicales) {
			if (i == stoi(indice_partie)) {
				partie_musicale_selectionnee = partie.first;
			}
			++i;
		}
	}

	// Extraction de la mélodie de la partie selectionnée, en fonction de la structure de la partition

	cout << "\nExtraction des notes composant la mélodie de la partie " << parties_musicales.find(partie_musicale_selectionnee)->second << " (id=" << partie_musicale_selectionnee << " dans le fichier)" << endl;

	// Extraction des notes de la partie musicale sélectionnée, mesure après mesure
	if (structure_partition.compare("partwise") == 0) {
		// Itération sur les parties (noeuds "part" du noeud "score-partwise")
		for (xml_node<> * noeud_partie = noeud_racine->first_node("part"); noeud_partie; noeud_partie = noeud_partie->next_sibling()) {
			// Si cette partie est la partie qui a été sélectionnée
			if (noeud_partie->first_attribute("id")->value() == partie_musicale_selectionnee) {
				// Itération sur les mesures (noeuds "measure" des noeuds "part")
				for(xml_node<> * noeud_mesure = noeud_partie->first_node("measure"); noeud_mesure; noeud_mesure = noeud_mesure->next_sibling()) {
					// Itération sur les notes (noeuds "note" des noeuds "measure")
					for(xml_node<> * noeud_note = noeud_mesure->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
						xml_node<> * noeud_accord = noeud_note->first_node("chord");

						if (noeud_accord == NULL) {
							xml_node<> * noeud_pitch = noeud_note->first_node("pitch");

							if (noeud_pitch != NULL) {
								res += "  <note>\n";

								char valeur_note_en = noeud_pitch->first_node("step")->value()[0];
								int valeur_note = noteAnglaiseVersEntier(valeur_note_en);

								if (noeud_pitch->first_node("alter") != NULL) {
									valeur_note += stoi(noeud_pitch->first_node("alter")->value());
								}

								int octave_note = stoi(noeud_pitch->first_node("octave")->value());

								// La valeur d'une note peut valoir -1 dans le cas d'un 0 bémol
								// Dans ce cas, la valeur est équivalente à celle de la note la plus haute de la gamme inférieure
								if (valeur_note == -1) {
									valeur_note = 11;
									octave_note -= 1;
								}

								res += "    <valeur>" + to_string(valeur_note) + "</valeur>\n";
								res += "    <octave>" + to_string(octave_note) + "</octave>\n";

								res += "  </note>\n";
							}
						} // fin si noeud_accord == NULL
					}
				}
			} // fin si noeud_partie.id = partie_musicale_selectionnee
		}
	}
	// Extraction des notes mesure après mesure, seulement de la partie musicale selectionnée
	else if (structure_partition.compare("timewise") == 0) {
		// Itération sur les mesures (noeuds "measure" du noeud "score-timewise")
		for(xml_node<> * noeud_mesure = noeud_racine->first_node("measure"); noeud_mesure; noeud_mesure = noeud_mesure->next_sibling()) {
			// Itération sur les parties (noeuds "part" des noeuds "measure")
			for (xml_node<> * noeud_partie = noeud_mesure->first_node("part"); noeud_partie; noeud_partie = noeud_partie->next_sibling()) {
				// Si cette partie est la partie qui a été sélectionnée
				if (noeud_partie->first_attribute("id")->value() == partie_musicale_selectionnee) {
					// Itération sur les notes (noeuds "note" des noeuds "part")
					for(xml_node<> * noeud_note = noeud_partie->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
						xml_node<> * noeud_accord = noeud_note->first_node("chord");

						if (noeud_accord == NULL) {
							xml_node<> * noeud_pitch = noeud_note->first_node("pitch");

							if (noeud_pitch != NULL) {
								res += "  <note>\n";

								char valeur_note_en = noeud_pitch->first_node("step")->value()[0];
								int valeur_note = noteAnglaiseVersEntier(valeur_note_en);

								if (noeud_pitch->first_node("alter") != NULL) {
									valeur_note += stoi(noeud_pitch->first_node("alter")->value());
								}

								int octave_note = stoi(noeud_pitch->first_node("octave")->value());

								// La valeur d'une note peut valoir -1 dans le cas d'un 0 bémol
								// Dans ce cas, la valeur est équivalente à celle de la note la plus haute de la gamme inférieure
								if (valeur_note == -1) {
									valeur_note = 11;
									octave_note -= 1;
								}

								res += "    <valeur>" + to_string(valeur_note) + "</valeur>\n";
								res += "    <octave>" + to_string(octave_note) + "</octave>\n";

								res += "  </note>\n";
							}
						} // fin si noeud_accord == NULL
					}
				}
			} // fin si noeud_partie.id = partie_musicale_selectionnee
		}
	}

    cout << "\nExtraction de la mélodie terminée" << endl;

	// Fin de la lecture du fichier contenant la partition

    res += "</notes>";

    cout << "\nLecture terminée" << endl;

	/* Enregistrement de la mélodie épurée dans fichier de sortie ou affichage à défaut de fichier donné en argument */
	 
	if (argc == 3) {
		string nom_fichier_sortie = argv[2];
		ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
		
		if(fichier_sortie) {
			fichier_sortie << res;
			fichier_sortie.close();
		}
		else {
			cout << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
		}

		cout << "\nLe fichier " << nom_fichier_sortie << " contient la mélodie de la partie " << partie_musicale_selectionnee << " de la partition du fichier " << argv[1] << endl;
	} else {
		cout << "\nAucun fichier de sortie n'est donné en argument du programme" << endl;
		cout << "\n" << res << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}
