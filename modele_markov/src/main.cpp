#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../include/rapidxml-1.13/rapidxml.hpp"
#include "../include/chaine_markov.h"
#include "../include/note.h"

using namespace std;
using namespace rapidxml;


// TODO
/*
	Faire un programme qui converti mon format vers MusicXML ? => librairie pour faire un MusicXML from scratch
	Convertir du MIDI
	Extraire d'autres contraintes d'un XML épuré
*/

bool operator==(Note const &n1, Note const &n2) {
	return n1.valeurNote() == n2.valeurNote() && n1.octaveNote() == n2.octaveNote();
}

ostream& operator<<(ostream &flux, const Note &note) {
	note.afficherNote(flux);
	return flux;
}

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 2) {
		cerr << "Donner en argument le ou les fichier(s) XML contenant une mélodie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation de la chaine de Markov */

	ChaineMarkov<Note> chaine_markov;

	/* Lecture des fichiers contenant la mélodie */

	for (int i = 1; i < argc; ++i) {
		cout << "Analyse de la mélodie du fichier " << argv[i] << endl;

		xml_document<> doc;
		xml_node<> * noeud_racine;

		// Initialisation du vecteur contenant les noeuds du fichier
		ifstream theFile(argv[1]);
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		doc.parse<0>(&buffer[0]);

		noeud_racine = doc.first_node("notes");

		// Itération sur les parties (noeuds "note" du noeud "notes")
		for (xml_node<> * noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
			// cout << "+ Note" << endl;

			int valeur_note = stoi(noeud_note->first_node("valeur")->value());
			int octave_note = stoi(noeud_note->first_node("octave")->value());

			// cout << "    " << valeur_note << "\n" << "    " << octave_note << endl;

			chaine_markov.ajouterElement(Note(valeur_note, octave_note));
		}
	}

	/* Affichage de la chaîne de Markov */

	cout << "\nChaîne de Markov :" << endl;
	chaine_markov.afficherChaine();

	/* Calcul de la matrice des statistiques */

	chaine_markov.calculerStatistiques();

	/* Affichage de la matrice des statistiques */

	cout << "\nMatrice des statistiques :" << endl;
	chaine_markov.afficherMatrice();
	
	/* Initialisation du srand */

	// srand(time(NULL));

	/* Génération de nouvelles notes */

	// int nb_new_notes = 200;
	// Note *new_note = chaine_markov.genererElement();
	// for (int i = 1; i <= nb_new_notes && new_note != NULL; ++i) {
	// 	cout << "<note><valeur>" << new_note->valeurNote() << "</valeur><octave>" << new_note->octaveNote() << "</octave></note>";
	// 	if (i == nb_new_notes || i % 4 == 0) cout << endl;
	// 	new_note = chaine_markov.genererElement();
	// }

	/* Récupération de la mélodie pour calculer d'autres propriétés de cette dernière, ainsi que sa matrice statistiques */

	string res = "<contraintes>\n";
	vector<Note *> melodie = chaine_markov.chaine();
	vector<Note *> notes = chaine_markov.elementsUniques();
	vector<vector<double>> statistiques = chaine_markov.matrice();

	/* C1 : Calcul de la note min et de la note max */

	Note *note_min = melodie[0];
	Note *note_max = melodie[0];
	for (auto note : melodie) {
		if (note < note_min) note_min = note;
		if (note > note_max) note_max = note;
	}

	res += "  <elements-min-max>\n";
	res += "    <objectif>1</objectif>\n";
	res += "    <element-min>\n";
	res += "      <note>\n";
	res += "        <valeur>" + to_string(note_min->valeurNote()) + "</valeur>\n";
	res += "        <octave>" + to_string(note_min->octaveNote()) + "</octave>\n";
	res += "      </note>\n";
	res += "    </element-min>\n";
	res += "    <element-max>\n";
	res += "      <note>\n";
	res += "        <valeur>" + to_string(note_max->valeurNote()) + "</valeur>\n";
	res += "        <octave>" + to_string(note_max->octaveNote()) + "</octave>\n";
	res += "      </note>\n";
	res += "    </element-max>\n";
	res += "  </elements-min-max>\n";

	/* C2 : Calcul de la plus grande hauteur entre deux notes sur un plage de notes donnée */

	int hauteur_rectangle = 0;
	int largeur_rectangle = 1;
	
	bool valide = true;
	for (unsigned int i = 0; i < melodie.size() - largeur_rectangle && valide; ++i) {
		// Augmentation de la largeur du rectangle
		++largeur_rectangle;
		// Initialisation de l'intervalle des notes et actualisation de la hauteur du rectangle
		int indice_min = i;
		int indice_max = i + largeur_rectangle - 1;
		Note *premiere_note = melodie[indice_min];
		Note *derniere_note = melodie[indice_max];
		if (hauteur_rectangle < abs(premiere_note->hauteurNote() - derniere_note->hauteurNote())) {
			hauteur_rectangle = abs(premiere_note->hauteurNote() - derniere_note->hauteurNote());
		}
		// Vérification auprès des notes qu'elles sont entre la première et la dernière note
		while(valide && indice_min < indice_max) { // indice_min < indice_max au lieu de <= pour ommetre la dernière note
			++indice_min; // ++indice_min au début de la boucle pour ommettre la première note
			if (premiere_note < derniere_note) {
				if (melodie[indice_min] < premiere_note || melodie[indice_min] > derniere_note) {
					valide = false;
				}
			} else if (premiere_note > derniere_note) {
				if (melodie[indice_min] > premiere_note || melodie[indice_min] < derniere_note) {
					valide = false;
				}
			}
		}
	}

	if (!valide) {
		--largeur_rectangle;
	}

	// cout << "\nLa mélodie comporte " << melodie.size() << " rectangles.\n"
	// cin <<

	res += "  <rectangles>\n";
	res += "    <objectif>1</objectif>\n";
	res += "    <hauteur>" + to_string(hauteur_rectangle) + "</hauteur>\n";
	res += "    <largeur>" + to_string(largeur_rectangle) + "</largeur>\n";
	res += "  </rectangles>\n";

	/* C3 :  */
	
	res += "  <couples-notes>\n";
	res += "    <objectif>1</objectif>\n";

	for (unsigned int i = 0; i < statistiques.size(); ++i) {
		for (unsigned int j = 0; j < statistiques[i].size(); ++j) {
			res += "      <couple>\n";
			res += "        <note>\n";
			res += "          <valeur>" + to_string(notes[i]->valeurNote()) + "</valeur>\n";
			res += "          <octave>" + to_string(notes[i]->octaveNote()) + "</octave>\n";
			res += "        </note>\n";
			res += "        <note>\n";
			res += "          <valeur>" + to_string(notes[j]->valeurNote()) + "</valeur>\n";
			res += "          <octave>" + to_string(notes[j]->octaveNote()) + "</octave>\n";
			res += "        </note>\n";
			res += "        <distance>1</distance>\n";
			res += "        <probabilite>" + to_string(statistiques[i][j]) + "</probabilite>\n";
			res += "      </couple>\n";
		}
	}

	res += "  </couples-notes>\n";

	res += "</contraintes>";

	/* Enregistrement des propriétés de la mélodie dans fichier de sortie ou affichage à défaut de fichier donné en argument */
	 
	if (argc == 3) {
		string nom_fichier_sortie = argv[2];
		ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
		
		if(fichier_sortie) {
			fichier_sortie << res;
			fichier_sortie.close();
		}
		else {
			cout << "Impossible de créer le fichier " << nom_fichier_sortie << endl;
		}

		cout << "\nLe fichier " << nom_fichier_sortie << " contient les propriétés de la mélodie du fichier " << argv[1] << endl;
	} else {
		cout << "\nAucun fichier de sortie n'est donné en argument du programme" << endl;
		cout << "\n" << res << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}
