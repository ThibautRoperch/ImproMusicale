#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "../lib/rapidxml-1.13/rapidxml.hpp"
#include "../include/chaine_markov.h"
#include "../include/note.h"

using namespace std;
using namespace rapidxml;


// TODO
/*
	Un modèle de Markov représente un groupe de mélodies du même genre/type/auteur. Il est construit avec un échantillon de mélodies
	jugées semblables.
	Une mélodie est dite "faite à la manière de" si elle est réalisable dans le cadre de ce modèle de Markov, c'est à dire
	si elle est rélisable dans le modèle, soit si la suite de notes est probable (calculer la proba, au passage ?)
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

	if (argc < 3) {
		cerr << "Donner en argument le ou les fichier(s) XML contenant une mélodie et la mélodie générée à vérifier\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation des chaines de Markov */

	ChaineMarkov<Note> chaine_markov;
	vector<Note> melodie_generee;

	/* Lecture des fichiers contenant les mélodies sources et la mélodie générée */

	for (int i = 1; i < argc; ++i) {
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

			// cout << "    " << valeur_note << "\n" << "    " << octave_note << endl;

			if (i == argc - 1) {
				melodie_generee.push_back(Note(valeur_note, octave_note));
			} else {
				chaine_markov.ajouterElement(Note(valeur_note, octave_note));
			}
		}
		chaine_markov.reinitialiserChaine();
	}

	/* Calcul de la matrice des statistiques du modèle de Markov des mélodies sources */

	chaine_markov.calculerStatistiques();

	/* Affichage de la matrice des statistiques du modèle de Markov des mélodies sources */

	cout << "\nMatrice des statistiques :" << endl;
	chaine_markov.afficherMatrice();
	
	/* Affichage de la mélodie générée */

	cout << "\nMélodie générée :" << endl;
	for (auto note : melodie_generee) {
		cout << note << " ";
	}
	cout << "\n" << endl;

	/* Vérification que la mélodie générée et possiblement réalisable dans le modèle de Markov des mélodies sources */

	cout << "Ressemblance de la mélodie avec les sources\t" << chaine_markov.recompenseChaine(melodie_generee) / (melodie_generee.size() - 1) * 100 << " %" << endl;
	cout << "Probabilité que cette mélodie soit produite\t" << setprecision(30) << chaine_markov.probabiliteChaineRealisable(melodie_generee) * 100 << " %" << endl;

	return EXIT_SUCCESS;
}
