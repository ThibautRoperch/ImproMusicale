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
	return n1.hauteurNote() == n2.hauteurNote();
}

bool operator!=(Note const &n1, Note const &n2) {
	return n1.hauteurNote() != n2.hauteurNote();
}

bool operator<(Note const &n1, Note const &n2) {
	return n1.hauteurNote() < n2.hauteurNote();
}

bool operator>(Note const &n1, Note const &n2) {
	return n1.hauteurNote() > n2.hauteurNote();
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
		if (*note < *note_min) note_min = note;
		if (*note > *note_max) note_max = note;
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
			// Augmentation de la hauteur du rectangle si la différence de hauteur des deux notes est supérieure à la hauteur du rectangle
			if (hauteur_rectangle < abs(premiere_note->hauteurNote() - derniere_note->hauteurNote())) {
				hauteur_rectangle = abs(premiere_note->hauteurNote() - derniere_note->hauteurNote());
			}
			++indice_min; // ++indice_min au début de la boucle pour ommettre la première note
			// Vérification auprès des notes qu'elles sont entre la première et la dernière note
			while(valide && indice_min < indice_max) { // indice_min < indice_max au lieu de <= pour ommetre la dernière note
				if (*premiere_note < *derniere_note) {
					if (*melodie[indice_min] < *premiere_note || *melodie[indice_min] > *derniere_note) {
						valide = false;
					}
				} else if (*premiere_note > *derniere_note) {
					if (*melodie[indice_min] > *premiere_note || *melodie[indice_min] < *derniere_note) {
						valide = false;
					}
				} else if (*premiere_note == *derniere_note) {
					if (*melodie[indice_min] != *premiere_note) {
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
	res += "    <rectangle>\n";
	res += "      <objectif>1</objectif>\n";
	res += "      <hauteur>" + to_string(hauteur_rectangle) + "</hauteur>\n";
	res += "      <largeur>" + to_string(largeur_rectangle) + "</largeur>\n";
	res += "      <nombre>" + to_string(melodie.size() - largeur_rectangle + 1) + "</nombre>\n";
	res += "    </rectangle>\n";
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

	map<vector<Note>, vector<int>> patterns; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

	// Pour chaque note i de la mélodie
	unsigned int indice_note_i = 0;
	while (indice_note_i < melodie.size()) {
		// Si la note i est retrouvée plus loin dans la mélodie, déterminer si c'est un pattern :
		// Avancer en même temps i et j
		// Avancer i dans la boucle j permet en plus d'éviter de détecter un pattern qui est en fait un sous-pattern
		// Résultat : seuls les sur-patterns sont détectés (un pattern dans un pattern ne sera pas identifié)
		
		map<vector<Note>, vector<int>> patterns_partants_de_i; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

		// Plusieurs patterns différents peuvent être trouvée en partant de la note i
		// La boucle for suivante va tous les repérer, et compter le nombre de d'occurences de chaque pattern en ajoutant l'indice de départ de chacun d'eux
		// Le pattern le plus grand sera retenu, impossible de trouver deux patterns différents de même taille
		// Lorsque tous les patterns partant de i seront trouvés, il faudra incrémenter i de la taille du pattern retenu
		// pour éviter de repartir sur la deuxième note d'un pattern déjà trouvé (et donc de trouver un nouveau pattern qui est en fait un sous-pattern)

		// Pour chaque note j de la mélodie, à partir de la note suivant la note i
		unsigned int indice_note_j = indice_note_i + 1;
		while (indice_note_j < melodie.size()) {
			vector<Note> suite;
			
			// Tant que la note i+taille_suite de la suite commancant par i est égale à la note de la suite commancant par j-taille_suite
			// et que la note i+taille_suite n'est pas une note de la suite commancant par j-taille_suite,
			// ajouter la note à la suite et incrémenter la note j
			while (indice_note_j < melodie.size() && *melodie[indice_note_i + suite.size()] == *melodie[indice_note_j]
				&& indice_note_i + suite.size() < indice_note_j - suite.size()) {
				suite.push_back(*melodie[indice_note_j]);
				++indice_note_j;
			}

			// Si aucun de début de suite formant un potentiel pattern n'a été trouvé, incrémenter la note j et recommencer la recherche
			// Sinon, étudier la suite obtenue afin de savoir si elle peut être considérée comme un pattern, auquel cas il faut l'enregistrer avec les autres patterns
			if (suite.size() == 0) {
				++indice_note_j;
			} else {
				if (suite.size() >= 3) {
					// Ajouter ce pattern s'il n'existe pas déjà dans la liste des patterns partants de i, ainsi que la position du premier pattern (partant de la note i)
					if (patterns_partants_de_i.find(suite) == patterns_partants_de_i.end()) {
						vector<int> positions;
						positions.push_back(indice_note_i);
						patterns_partants_de_i[suite] = positions;
					}
					// Ajouter la position du début de ce pattern (partant de la position à laquelle était la note j avant de parcourir la suite)
					patterns_partants_de_i[suite].push_back(indice_note_j - suite.size());
				}
			}
		} // Fin de la recherche d'une suite de notes partant d'une note égale à la note i

		// Si aucun pattern partant de la note i n'a été trouvé, incrémenter la note i de 1
		// Sinon, incrémenter la note i de la taille pattern le plus présent pour éviter de retrouver un pattern en partant de i+1,
		// qui serait un sous-pattern d'un pattern déjà trouvé en partant de la note i
		// Ajouter au passage à la liste des patterns celui qui est le plus souvent trouvé en partant de la note i
		if (patterns_partants_de_i.size() != 0) {
			// Ajouter définitivement le pattern le plus grand partant de i, un seul choix possible, pas d'égalité de patterns possible
			vector<Note> plus_grand_pattern;
			for (auto pattern : patterns_partants_de_i) {
				if (pattern.first.size() > plus_grand_pattern.size()) {
					plus_grand_pattern = pattern.first;
				}
			}
			patterns[plus_grand_pattern] = patterns_partants_de_i[plus_grand_pattern];
		}

		++indice_note_i;
	}

	// Trier les patterns : certaines suites peuvent être des sous-patterns, ou certains patterns peuvent se chevaucher
	// Dans tous les cas, ne garder que les patterns les plus grands
	map<vector<Note>, vector<int>> patterns_tmp = patterns;
	patterns.clear();
	for (auto pattern : patterns_tmp) {
		bool garder = true;

		for (auto pat : patterns_tmp) {
			// S'il y a un conflit entre les deux patterns, ne pas garder pattern s'il est strictement plus petit que le pat
			// Conflit : les patterns se chevauchent
			for (unsigned int position : pattern.second) {
				for (unsigned int pos : pat.second) {
					if ((position >= pos && position <= pos + pat.first.size()) // pattern commence dans pat
					 || (position + pattern.first.size() >= pos && position + pattern.first.size() <= pos + pat.first.size())) // pattern finit dans pat
					{
						// S'il y a un conflit, ne garder que le pattern qui occupe un plus grand pourcentage dans la mélodie
						// Si les deux patterns ont la même proportion, soit ce sont les mêmes, soit ils sont différents
						// S'ils sont différents, garder le pattern qui est le plus grand, si là aussi il a une égalité, garder le pattern qui est le plus dispersé
						if (pattern.first.size() * pattern.second.size() < pat.first.size() * pat.second.size()) {
							garder = false;
						} else if(pattern.first.size() * pattern.second.size() == pat.first.size() * pat.second.size()) {
							// pattern et pat ont la même proportion dans la mélodie
							// Ne pas garder pattern s'il est plus petit que pat
							// S'ils sont de taille égale, calculer leur dispersion
							if (pattern.first.size() < pat.first.size()) {
								garder = false;
							} else if (pattern.first.size() == pat.first.size()) {
								// Calculer la somme de la différence entre chaque position du pattern
								// Si pattern a une somme inférieure à celle de pat, ne pas le garder
								// Si les patterns ont la même dispersion, ils seront tous les deux gardés
								if (pattern.first != pat.first) {
									int dispersion_pattern = 0;
									for (unsigned int i = 0; i < pattern.second.size() - 1; ++i) {
										dispersion_pattern += pattern.second[i + 1] - pattern.second[i];
									}
									int dispersion_pat = 0;
									for (unsigned int i = 0; i < pat.second.size() - 1; ++i) {
										dispersion_pat += pat.second[i + 1] - pat.second[i];
									}
									if (dispersion_pattern < dispersion_pat) { // < : les 2 patterns gardés si égalité de leur dispersion		<= : aucun pattern gardé si égalité de sleur dispersion
										garder = false;
									}
								}
							}
						} // Fin de la comparaison de la proportion des deux patterns
					} // Fin de la recherche de conflits entre les deux patterns
					// Sinon, alors pattern et pat sont assez loin (non conflictuels), ou alors pat est un sous-pattern de pattern, il sera donc traité plus tard et non retenu
				}
			}
		}
		
		if (garder) {
			patterns[pattern.first] = pattern.second;
		}
	}

	res += "  <patterns>\n";
	int i = 0;
	for (auto pattern : patterns) {
		double proportion_pattern = (double) pattern.first.size() * pattern.second.size() / melodie.size(); // taille du pattern * nombre de fois qu'il apparaît / taille de la mélodie
		// if (proportion_pattern >= 0.20) { // Etude réalisée sur tous les patterns trouvés afin de déterminer la valeur de la proportion minimale d'une suite pour qu'elle soit acceptée en tant que pattern
			res += "    <pattern id=\"" + to_string(i++) + "\">\n";
			res += "      <taille>" + to_string(pattern.first.size()) + "</taille>\n";
			res += "      <positions>\n";
			for (auto position : pattern.second) {
				res += "        <indice>" + to_string(position) + "</indice>\n";
			}
			res += "      </positions>\n";
			res += "    </pattern>\n";
			// Débuggage
			cout << "Pattern " << i << " : "; for (auto note : pattern.first) cout << note << " "; cout << endl;
			cout << "Positions : "; for (auto position : pattern.second) cout << position << " "; cout << endl;
			cout << "Proportion : " << setprecision(15) << proportion_pattern << endl;
			cout << endl;
		// }
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