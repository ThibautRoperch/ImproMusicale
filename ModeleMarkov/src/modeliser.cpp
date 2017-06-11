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

	/* Initialisation de la chaine de Markov et du vecteur de mélodies */

	ChaineMarkov<Note> chaine_markov;
	vector<vector<Note *>> melodies;

	/* Lecture des fichiers contenant une mélodie */

	Note *note_min = NULL;
	Note *note_max = NULL;

	// Un rectangle est calculé pour chaque mélodie, une moyenne est faite à la fin pour calculer le rectangle final
	vector<double> hauteur_rectangle(argc - 2, 0); // vecteur de x 0, x étant le nombre de mélodies 
	vector<int> largeur_rectangle(argc - 2, 0); // vecteur de x 0, x étant le nombre de mélodies 
	vector<double> difference_moyenne(argc - 2, -1); // vecteur de x -1, x étant le nombre de mélodies 

	vector<int> repartition_notes(12, 0); // à un indice correspond la valeur d'une note, la valeur de l'indice correspond au nombre de notes qui ont cette valeur

	for (int i = 1; i < argc - 1; ++i) {
		cout << "Analyse de la mélodie du fichier " << argv[i] << endl;

		int id_fichier = i - 1;

		xml_document<> doc;
		xml_node<> *noeud_racine;

		// Initialisation du vecteur contenant les noeuds du fichier
		ifstream theFile(argv[i]);
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		doc.parse<0>(&buffer[0]);
		noeud_racine = doc.first_node("notes");

		Note *note_precedente = NULL;
		int monotonie = -1;

		// Itération sur les parties (noeuds "note" du noeud "notes")
		for (xml_node<> *noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
			// cout << "+ Note" << endl;

			int valeur_note = stoi(noeud_note->first_node("valeur")->value());
			int octave_note = stoi(noeud_note->first_node("octave")->value());
			Note n(valeur_note, octave_note);

			// cout << "    " << valeur_note << "\n" << "    " << octave_note << endl;

			chaine_markov.ajouterElement(n);

			/* C1 : Calcul de la note min et de la note max */

			if (note_min == NULL || n < *note_min) note_min = chaine_markov.dernierElement();
			if (note_max == NULL || n > *note_max) note_max = chaine_markov.dernierElement();

			/* C2 : Calcul de la hauteur moyenne entre deux notes sur une plage de notes donnée (rectangle) */
			// Le rectangle adapte sa taille en fonction des notes consécutives : il faut donc une note précédente et une note actuelle
			// Les mélodies ne se suivent pas forcément, la note précédente est donc NULL à chaque nouvelle mélodie lue
			// S'il n'y a pas de note précédemment lue pour cette mélodie, fixer la monotonie à 1 car la première note de cette mélodie est en train d'être lue
			if (note_precedente != NULL) {
				// Changement de la largeur du rectangle en fonction de la monotonie de la mélodie sur la largeur du rectangle
				// La suite de notes est dite monotone si leur différence de hauteur est inférieure ou égale à la moyenne des différences de hauteur entre deux notes
				// à ce stade de la lecture de la mélodie
				// Si la mélodie est monotone dans le rectangle et l'est toujours une note après le rectangle, celui-ci gagne 1 en largeur

				// Changement de la hauteur du rectangle en fonction de la moyenne des différences de hauteur des notes consécutives de la mélodie à ce stade de la lecture
				// La différence moyenne en comptant cette note est soustraite à la différence moyenne sans compter cette note,
				// le résultat est ajouté à la hauteur du rectangle

				int difference_hauteur = abs(n.hauteurNote() - note_precedente->hauteurNote());

				// Actualisation de la monotonie avec la différence de hauteur entre cette note et la pécédente
				// Si la différence de hauteur est inférieure à la moyenne (ou s'il n'y a pas encore de moyenne pour cette mélodie), la monotonie gagne un point
				// Sinon, la monotonie est remise à 1
				if (difference_hauteur <= difference_moyenne[id_fichier] || difference_moyenne[id_fichier] == -1) {
					++monotonie;
				} else {
					monotonie = 1;
				}
				// Lorsque la monotonie vaut la largeur du rectangle + 1, celui-ci gagne 1 en largeur
				// La monotonie n'est pas remise à 0, elle peut continuer d'augmenter à la prochaine note
				if (monotonie == largeur_rectangle[id_fichier] + 1) {
					++largeur_rectangle[id_fichier];
				}

				// Dans le cas où la note actuellement lue est la deuxième, toutes mélodies confondues, la différence moyenne vaut la différence actuelle
				// Sinon, dans le cas normal, la nouvelle moyenne de différences de hauteur est calculée, et la hauteur du rectangle est mise à jour
				if (difference_moyenne[id_fichier] == -1) {
					difference_moyenne[id_fichier] = difference_hauteur;
					hauteur_rectangle[id_fichier] = difference_moyenne[id_fichier];
				} else {
					// Calcule de la différence entre la différence moyenne en ajoutant la différence de hauteur avec cette note et la différence moyenne sans compter cette note
					// Ajout du résultat à la hauteur du rectangle
					// Enregistrement de la différence moyenne
					double nouvelle_difference_moyenne = (difference_moyenne[id_fichier] * (chaine_markov.nombreElementsAjoutes() - 1 - 1) + difference_hauteur) / (chaine_markov.nombreElementsAjoutes() - 1); // - 1 pour omettre la dernière note et - 1 car le nombre de différences = nombre de notes - 1
					hauteur_rectangle[id_fichier] += nouvelle_difference_moyenne - difference_moyenne[id_fichier];
					difference_moyenne[id_fichier] = nouvelle_difference_moyenne;
				}
			} else {
				hauteur_rectangle[id_fichier] = 1;
				largeur_rectangle[id_fichier] = 1;
				monotonie = 1;
			}

			note_precedente = chaine_markov.dernierElement();

			/* C5 : Calcul de la répartition des notes de la mélodie */

			++repartition_notes[valeur_note];
		} // Fin de la lecture des notes de la mélodie de ce fichier
		
		// Affichage de la chaîne de Markov
		chaine_markov.afficherChaine();
		
		// Sauvegarde de la mélodie
		melodies.push_back(chaine_markov.chaine());

		// Ecrasement de la mélodie précédente en vue d'une nouvelle lecture de mélodie
		chaine_markov.reinitialiserChaine();
	}

	/* Calcul de la matrice des statistiques du modèle de Markov des mélodies sources */

	chaine_markov.calculerStatistiques();

	/* Affichage de la matrice des statistiques du modèle de Markov des mélodies sources */

	cout << "\nMatrice des statistiques :" << endl;
	chaine_markov.afficherMatrice();

	/* Récupération de la matrice des statistiques de la chaine de Markov */

	string res = "<contraintes>\n";
	vector<Note *> melodie = chaine_markov.chaine();
	vector<Note *> notes = chaine_markov.elementsUniques();
	vector<vector<double>> statistiques = chaine_markov.matrice();

	/* C1 : Calcul de la note min et de la note max */

	if (note_min == NULL) note_min = new Note(0, 0);
	if (note_max == NULL) note_max = new Note(0, 0);

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

	/* C2 : Calcul de la hauteur moyenne entre deux notes sur une plage de notes donnée (rectangle) */

	double hauteur_totale_rectangle = 0;
	double largeur_totale_rectangle = 0;

	for (auto dimension : hauteur_rectangle) {
		hauteur_totale_rectangle += dimension;
	}
	for (auto dimension : largeur_rectangle) {
		largeur_totale_rectangle += dimension;
	}

	int hauteur_moyenne_rectangle = nearbyint(hauteur_totale_rectangle / hauteur_rectangle.size()); // arrondi à l'entier le plus proche
	int largeur_moyenne_rectangle = nearbyint(largeur_totale_rectangle / largeur_rectangle.size()); // arrondi à l'entier le plus proche
	int nombre_notes_couvertes = 0;
	int nombre_rectangles = 0;

	// Pour chaque mélodie
	for (auto melodie : melodies) {
		// Si la mélodie possède au moins suffisamment de notes pour remplir un rectangle, et que le rectangle fait au moins 1
		if ((int) melodie.size() >= largeur_moyenne_rectangle && largeur_moyenne_rectangle > 0) {
			nombre_rectangles += melodie.size() - largeur_moyenne_rectangle + 1;
			// Pour chaque rectangle de la mélodie
			for (unsigned int note = 0; note < melodie.size() - largeur_moyenne_rectangle + 1; ++note) {
				int position = melodie[note]->hauteurNote() + hauteur_moyenne_rectangle; // position haute maximale (repère du rectangle : angle haut gauche)
				int max_notes_couvertes = 0;
				// Pour chaque position du rectangle possible (du haut vers le bas) et tant que le nombre de notes couvertes est strictement inférieur au nombre de notes couvrables,
				// compter le nombre de notes couvertes et l'enregistrer s'il ce nombre est supérieur au max trouvé jusqu'ici
				while (position >= melodie[note]->hauteurNote() && max_notes_couvertes < largeur_moyenne_rectangle) { // position basse maximale (repère du rectangle : angle haut gauche)
					int notes_couvertes = 1; // la premiere note est toujours dans le rectangle
					// Pour chaque note se trouvant à portée horizontalement, la compter si elle est dans le rectangle
					for (unsigned int note_suivante = note + 1; note_suivante < note + largeur_moyenne_rectangle; ++note_suivante) {
						if (melodie[note_suivante]->hauteurNote() <= position && melodie[note_suivante]->hauteurNote() >= position - hauteur_moyenne_rectangle) {
							++notes_couvertes;
						}
					}
					if (notes_couvertes > max_notes_couvertes) {
						max_notes_couvertes = notes_couvertes;
					}
					--position;
				}
				nombre_notes_couvertes += max_notes_couvertes;
			} // Fin du "Pour chaque rectangle de la mélodie"
		} // Fin de la conditionnelle qui vérifie que la mélodie est suffisamment grande pour accueillir au moins un rectangle
	} // Fin du "Pour chaque mélodie"
	
	double precision_moyenne = (largeur_moyenne_rectangle * nombre_rectangles == 0) ? 0 : (double) nombre_notes_couvertes / (largeur_moyenne_rectangle * nombre_rectangles);

	res += "  <rectangles>\n";
	res += "    <rectangle>\n";
	res += "      <objectif>" + to_string(precision_moyenne) + "</objectif>\n";
	res += "      <hauteur>" + to_string(hauteur_moyenne_rectangle) + "</hauteur>\n";
	res += "      <largeur>" + to_string(largeur_moyenne_rectangle) + "</largeur>\n";
	res += "    </rectangle>\n";
	res += "  </rectangles>\n";

	/* C3 : Lecture des statistiques pour chaque couple de notes successives */
	
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

	/* C4 : Détection des répétitions de groupes de notes (patterns, style musical) */

	map<vector<Note>, vector<int>> patterns; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

	// Pour chaque mélodie
	for (auto melodie : melodies) {
		map<vector<Note>, vector<int>> patterns_melodie; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

		// Pour chaque note i de la mélodie
		unsigned int indice_note_i = 0;
		while (indice_note_i < melodie.size()) {
			// Si la note i est retrouvée plus loin dans la mélodie, déterminer si c'est un pattern :
			// Avancer en même temps i et j
			// Avancer i dans la boucle j permet en plus d'éviter de détecter un pattern qui est en fait un sous-pattern
			// Résultat : seuls les sur-patterns sont détectés (un pattern dans un pattern ne sera pas identifié)
			
			map<vector<Note>, vector<int>> patterns_partants_de_i; // en clef la suite de notes, en valeur les positions de ces paternes (en nombre de notes)

			// Plusieurs patterns différents peuvent être trouvée dans la mélodie en partant de la note i
			// La boucle for suivante va tous les repérer, et compter le nombre de d'occurences de chaque pattern en ajoutant l'indice de départ de chacun d'eux
			// Le pattern le plus grand sera retenu, impossible de trouver deux patterns différents de même taille dans une même mélodie
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
			// Ajouter au passage à la liste des patterns de la mélodie celui qui est le plus souvent trouvé en partant de la note i
			if (patterns_partants_de_i.size() != 0) {
				// Ajouter définitivement le pattern le plus grand partant de i, un seul choix possible, pas d'égalité de patterns possible
				vector<Note> plus_grand_pattern;
				for (auto pattern : patterns_partants_de_i) {
					if (pattern.first.size() > plus_grand_pattern.size()) {
						plus_grand_pattern = pattern.first;
					}
				}
				patterns_melodie[plus_grand_pattern] = patterns_partants_de_i[plus_grand_pattern];
			}

			++indice_note_i;
		} // Fin du "Pour chaque note de la mélodie"

		// Trier les patterns de la mélodie : certaines suites peuvent être des sous-patterns, ou certains patterns peuvent se chevaucher
		// Dans tous les cas, ne garder que les patterns de la mélodie les plus grands
		// Pour chaque pattern de la liste des patterns de la mélodie
		for (auto pattern : patterns_melodie) {
			bool garder = true;

			// Pour chaque pat de la liste des patterns de la mélodie
			for (auto pat : patterns_melodie) {
				// S'il y a un conflit entre les deux patterns de la mélodie, ne pas garder pattern s'il est strictement plus petit que le pat
				// Conflit : les patterns se chevauchent
				for (unsigned int position : pattern.second) {
					for (unsigned int pos : pat.second) {
						if ((position >= pos && position <= pos + pat.first.size()) // pattern commence dans pat
						|| (position + pattern.first.size() >= pos && position + pattern.first.size() <= pos + pat.first.size())) // pattern finit dans pat
						{
							// S'il y a un conflit, ne garder que le pattern qui occupe un plus grand pourcentage dans la mélodie
							// Si les deux patterns de la mélodie ont la même proportion, soit ce sont les mêmes, soit ils sont différents
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
			} // Fin du "Pour chaque pat de la liste des patterns de la mélodie"
			
			if (garder) {
				patterns[pattern.first] = pattern.second;
			}
		} // Fin du "Pour chaque pattern de la liste des patterns de la mélodie"
	} // Fin du "Pour chaque mélodie"

	res += "  <patterns>\n";
	int i = 0;
	for (auto pattern : patterns) {
		res += "    <pattern id=\"" + to_string(i++) + "\">\n";
		res += "      <taille>" + to_string(pattern.first.size()) + "</taille>\n";
		res += "      <nombre>" + to_string(pattern.second.size()) + "</nombre>\n";
		Note min = pattern.first[0];
		Note max = pattern.first[0];
		for (auto note : pattern.first) {
			if (note < min) min = note;
			if (note > max) max = note;
		}
		res += "      <amplitude>" + to_string(max.hauteurNote() - min.hauteurNote()) + "</amplitude>\n";
		res += "      <positions>\n";
		for (auto position : pattern.second) {
			res += "        <indice>" + to_string(position) + "</indice>\n";
		}
		res += "      </positions>";
		res += "    </pattern>\n";
		// cout << "Pattern " << i << " : "; for (auto note : pattern.first) cout << note << " "; cout << endl;
		// cout << "Positions : "; for (auto position : pattern.second) cout << position << " "; cout << endl;
		// cout << endl;
	}
	res += "  </patterns>\n";

	/* C5 : Calcul de la répartition des notes de la mélodie (tonalité) */

	int nombre_notes = 0;
	for (int nb_note : repartition_notes) {
		nombre_notes += nb_note;
	}

	res += "  <repartition-notes>\n";

	for (unsigned int i = 0; i < repartition_notes.size(); ++i) {
		double repartition = (nombre_notes == 0) ? 0 : (double) repartition_notes[i] / nombre_notes;
		res += "    <note-unique valeur=\"" + to_string(i) + "\">" + to_string(repartition) + "</note-unique>\n";
	}

	res += "  </repartition-notes>\n";
	

	res += "</contraintes>";

	/* Enregistrement des propriétés de la mélodie dans le fichier de sortie */
	 
	string nom_fichier_sortie = argv[argc - 1];
	ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
	
	if(fichier_sortie) {
		fichier_sortie << res;
		fichier_sortie.close();
	} else {
		cerr << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
	}

	if (argc == 3) {
		cerr << "\nLe fichier " << nom_fichier_sortie << " contient les propriétés de la mélodie du fichier " << argv[1] << endl;
	} else {
		cerr << "\nLe fichier " << nom_fichier_sortie << " contient les propriétés des mélodies des fichiers suivants :" << endl;
		for (int i = 1; i < argc - 1; ++i) {
			cerr << "- " << argv[i] << endl;
		}
	}

	cout << endl;

	return EXIT_SUCCESS;
}
