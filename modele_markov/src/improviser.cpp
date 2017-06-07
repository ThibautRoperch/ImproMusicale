#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../lib/rapidxml-1.13/rapidxml.hpp"
#include "../include/chaine_markov.h"
#include "../include/note.h"


using namespace std;
using namespace rapidxml;


int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 3) {
		cerr << "Donner en argument le ou les fichier(s) XML contenant une mélodie et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation de la chaine de Markov et du vecteur de mélodies */

	ChaineMarkov<Note> chaine_markov;

	/* Lecture des fichiers contenant une mélodie */

	for (int i = 1; i < argc - 1; ++i) {
		cout << "Analyse de la mélodie du fichier " << argv[i] << endl;

		xml_document<> doc;
		xml_node<> * noeud_racine;

		// Initialisation du vecteur contenant les noeuds du fichier
		ifstream theFile(argv[i]);
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		doc.parse<0>(&buffer[0]);

		noeud_racine = doc.first_node("notes");

		// Itération sur les parties (noeuds "note" du noeud "notes")
		for (xml_node<> * noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
			// cout << "+ Note" << endl;

			int valeur_note = stoi(noeud_note->first_node("valeur")->value());
			int octave_note = stoi(noeud_note->first_node("octave")->value());
			Note n(valeur_note, octave_note);

			// cout << "    " << valeur_note << "\n" << "    " << octave_note << endl;

			chaine_markov.ajouterElement(n);
		} // Fin de la lecture des notes de la mélodie de ce fichier

		// Affichage de la chaîne de Markov
		chaine_markov.afficherChaine();

		// Ecrasement de la mélodie précédente en vue d'une nouvelle mélodie
		chaine_markov.reinitialiserChaine();
	}

	/* Calcul de la matrice des statistiques du modèle de Markov des mélodies sources */

	chaine_markov.calculerStatistiques();

	/* Initialisation du srand */

	srand(time(NULL));

	/* Génération de nouvelles notes */

	string res = "<notes>\n";

	int nombre_notes = 20;

	Note *nouvelle_note = chaine_markov.genererElement();

	while (nouvelle_note != NULL && improvisation.size() < nombre_notes) {
		res += "  <note>\n";
		res += "    <valeur>" + to_string(nouvelle_note->valeurNote()) + "</valeur>\n";
		res += "    <octave>" + to_string(nouvelle_note->octaveNote()) + "</octave>\n";
		res += "  </note>\n";
		nouvelle_note = chaine_markov.genererElement();
	}

	res += "</notes>";

	/* Enregistrement de la mélodie générée dans le fichier de sortie */
	 
	string nom_fichier_sortie = argv[argc - 1];
	ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
	
	if(fichier_sortie) {
		fichier_sortie << res;
		fichier_sortie.close();
	} else {
		cerr << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
	}

	if (argc == 3) {
		cout << "\nLe fichier " << nom_fichier_sortie << " contient la mélodie générée à partir de la mélodie du fichier " << argv[1] << endl;
	} else {
		cout << "\nLe fichier " << nom_fichier_sortie << " contient la mélodie générée à partir des mélodies des fichiers suivants :" << endl;
		for (int i = 1; i < argc - 1; ++i) {
			cout << "- " << argv[i] << endl;
		}
	}

	cout << endl;

	return EXIT_SUCCESS;
}
