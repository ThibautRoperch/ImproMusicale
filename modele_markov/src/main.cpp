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


// TODO
/*
	Faire un programme qui converti mon format vers MusicXML ? => librairie pour faire un MusicXML from scratch
	Convertir du MIDI
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
		cerr << "Donner en argument le ou les fichier(s) XML contenant une mélodie et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation de la chaine de Markov */

	ChaineMarkov<Note> chaine_markov;

	/* Lecture des fichiers contenant la mélodie */

	for (int i = 1; i < argc - 1; ++i) {
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
		if (note->hauteurNote() < note_min->hauteurNote()) note_min = note;
		if (note->hauteurNote() > note_max->hauteurNote()) note_max = note;
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
	unsigned int largeur_rectangle = 1;
	
	bool valide = true;
	while (valide && largeur_rectangle < melodie.size()) {
		// Augmentation de la largeur du rectangle
		++largeur_rectangle;

		for (unsigned int i = 0; i < melodie.size() - largeur_rectangle + 1; ++i) {
			// Initialisation de l'intervalle des notes et actualisation de la hauteur du rectangle
			int indice_min = i;
			int indice_max = i + largeur_rectangle - 1;
			Note *premiere_note = melodie[indice_min];
			Note *derniere_note = melodie[indice_max];
			if (hauteur_rectangle < abs(premiere_note->hauteurNote() - derniere_note->hauteurNote())) {
				hauteur_rectangle = abs(premiere_note->hauteurNote() - derniere_note->hauteurNote());
			}
			++indice_min; // ++indice_min au début de la boucle pour ommettre la première note
			// Vérification auprès des notes qu'elles sont entre la première et la dernière note
			while(valide && indice_min < indice_max) { // indice_min < indice_max au lieu de <= pour ommetre la dernière note
				if (premiere_note < derniere_note) {
					if (melodie[indice_min]->hauteurNote() < premiere_note->hauteurNote() || melodie[indice_min]->hauteurNote() > derniere_note->hauteurNote()) {
						valide = false;
					}
				} else if (premiere_note->hauteurNote() > derniere_note->hauteurNote()) {
					if (melodie[indice_min]->hauteurNote() > premiere_note->hauteurNote() || melodie[indice_min]->hauteurNote() < derniere_note->hauteurNote()) {
						valide = false;
					}
				} else if (premiere_note->hauteurNote() == derniere_note->hauteurNote()) {
					if (melodie[indice_min]->hauteurNote() != premiere_note->hauteurNote()) {
						valide = false;
					}
				}
				++indice_min;
			}
		}
	}

	if (!valide) {
		--largeur_rectangle;
	}

	res += "  <rectangles>\n";
	res += "    <objectif>1</objectif>\n";
	res += "    <rectangle>\n";
	res += "      <hauteur>" + to_string(hauteur_rectangle) + "</hauteur>\n";
	res += "      <largeur>" + to_string(largeur_rectangle) + "</largeur>\n";
	res += "    </rectangle>\n";
	res += "    <nombre-rectangles>" + to_string(melodie.size() - largeur_rectangle + 1) + "</nombre-rectangles>\n";
	res += "  </rectangles>\n";

	/* C3 : Lecture des statistiques pour chaque couple notes successives */
	
	res += "  <couples-notes>\n";

	for (unsigned int i = 0; i < statistiques.size(); ++i) {
		for (unsigned int j = 0; j < statistiques[i].size(); ++j) {
			if (statistiques[i][j] != 0) {
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
	}

	res += "  </couples-notes>\n";

	/* C4 : Détection des répétitions de groupes de notes (patterns) */

	map<vector<Note *>, vector<int>> patterns; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

	// Pour chaque note i de la mélodie
	unsigned int indice_note_i = 0;
	while (indice_note_i < melodie.size()) {
		cout << "Recherche d'un pattern commencant à " << indice_note_i << " par " << *melodie[indice_note_i] << endl;
		// Si la note i est retrouvée plus loin dans la mélodie, déterminer si c'est un pattern :
		// Avancer en même temps i et j
		// Avancer i dans la boucle j permet en plus d'éviter de détecter un pattern qui est en fait un sous-pattern
		// Résultat : seuls les sur-patterns sont détectés (un pattern dans un pattern ne sera pas trouvé)
		
		unsigned int plus_petit_pattern_trouve = 0; // 0 : aucun pattern trouvé

		// Plusieurs patterns différents peuvent être trouvée en partant de la note i
		// La boucle for suivante va tous les repérer
		// Lorsque tous les patterns partant de i seront trouvés, il faudra incrémenter i de la taille du plus petit pattern
		// pour éviter de repartir sur la deuxième note d'un pattern déjà trouvé (et donc de trouver un nouveau pattern qui est en fait u nsous-pattern)
		// et pour ne pas passer à côté d'un pattern si celui commence après la fin du plus petit pattern partant de i trouvé

		// Pour chaque note j de la mélodie, à partir de la note suivant la note i
		unsigned int indice_note_j = indice_note_i + 1;
		while (indice_note_j < melodie.size()) {
			vector<Note *> suite;
			
			// Tant que la note de la suite i est égale à la note de la suite j, ajouter la note à la suite et incrémenter la note j
			while (indice_note_j< melodie.size() && *melodie[indice_note_i + suite.size()] == *melodie[indice_note_j]) {
				cout << "  "  << indice_note_j << " : " << *melodie[indice_note_i + suite.size()] << " = " << *melodie[indice_note_j] << endl;
				suite.push_back(melodie[indice_note_j]);
				++indice_note_j;
			}

			// Si aucun de début de suite formant un potentiel pattern n'a été trouvé, incrémenter la note j et recommencer la recherche
			// Sinon, étudier la suite obtenue afin de savoir si elle peut être considérée comme un pattern, auquel cas il faut l'enregistrer avec les autres patterns
			if (suite.size() == 0) {
				++indice_note_j;
			} else {
				// Si la suite de notes a une probabilité d'exister < 0.05, alors on peut la considérer comme un pattern
				if (suite.size() > 3) { // tochange
					// Ajouter ce pattern s'il n'existe pas déjà dans la liste, ainsi que le position du premier pattern (partant de la note i)
					if (patterns.find(suite) == patterns.end()) {
						vector<int> positions;
						positions.push_back(indice_note_i);
						patterns[suite] = positions;
					}
					// Ajouter la position du début de ce pattern (partant de la position à laquelle était la note j avant de parcourir la suite)
					patterns[suite].push_back(indice_note_j - suite.size());
					cout << " => Pattern ajouté" << endl;
					// Enregistrement de la taille du pattern trouvé si c'est le premier pattern partant de la note i trouvé ou s'il est plus petit que celui (ceux) déjà trouvé(s)
					if (plus_petit_pattern_trouve == 0 || suite.size() < plus_petit_pattern_trouve) {
						plus_petit_pattern_trouve = suite.size();
					}
				}
			}
		} // Fin de la recherche d'une suite de notes partant d'une note égale à la note i

		// Si aucun pattern partant de la note i n'a été trouvé, incrémenter la note i de 1
		// Sinon, incrémenter la note i de la taille de la taille du plus petti pattern pour éviter de retrouver un pattern en partant de i+1,
		// qui serait un sous-pattern d'un pattern déjà trouvé en partant de la note i
		if (plus_petit_pattern_trouve == 0) {
			++indice_note_i;
		} else {
			indice_note_i += plus_petit_pattern_trouve - 1; // -1 car la note i est déjà comptée
		}
	}

	res += "  <patterns>\n";
	int i = 0;
	for (auto pattern : patterns) {
		cout << "PATTERN " << i << " : "; for (auto note : pattern.first) cout << *note << " "; cout << endl;
		res += "    <pattern id=\"" + to_string(i++) + "\">\n";
		res += "      <taille>" + to_string(pattern.first.size()) + "</taille>\n";
		res += "      <positions>\n";
		for (auto position : pattern.second) {
			res += "        <indice>" + to_string(position) + "</indice>\n";
		}
		res += "      </positions>\n";
		res += "    </pattern>\n";
	}
	res += "  </patterns>\n";

	res += "</contraintes>";

	/* Enregistrement des propriétés de la mélodie dans fichier de sortie ou affichage à défaut de fichier donné en argument */
	 
	string nom_fichier_sortie = argv[argc - 1];
	ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
	
	if(fichier_sortie) {
		fichier_sortie << res;
		fichier_sortie.close();
	} else {
		cout << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
	}

	cout << "\nLe fichier " << nom_fichier_sortie << " contient les propriétés de la mélodie du fichier " << argv[1] << endl;

	cout << endl;

	return EXIT_SUCCESS;
}
