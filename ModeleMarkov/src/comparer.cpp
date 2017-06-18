#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cmath>

#include "../lib/rapidxml-1.13/rapidxml.hpp"
#include "../include/note.h"

using namespace std;
using namespace rapidxml;


bool operator<(Note const &n1, Note const &n2) {
	return n1.hauteurNote() < n2.hauteurNote();
}

bool operator>(Note const &n1, Note const &n2) {
	return n1.hauteurNote() > n2.hauteurNote();
}

bool operator<=(Note const &n1, Note const &n2) {
	return n1.hauteurNote() <= n2.hauteurNote();
}

bool operator>=(Note const &n1, Note const &n2) {
	return n1.hauteurNote() >= n2.hauteurNote();
}

ostream& operator<<(ostream &flux, const Note &note) {
	note.afficherNote(flux);
	return flux;
}

int ressemblance(int source, int cible) {
	source = abs(source);
	cible = abs(cible);

	// Si la source est équivalante à la cible, la ressemblance est de 100 (%)

	if (source == cible) return 100;

	// Si la source est supérieure à la cible, la source est l'échelle de référence
	// L'intervalle de référence est défini de 0 à source

	// Si la cible est supérieure à la source, la cible est l'échelle de référence
	// L'intervalle de référence est défini de 0 à cible

	int possibilites = 1, difference = 1;

	if (source > cible) {
		possibilites = source + 1;
		difference = possibilites - cible;
	}

	if (cible > source) {
		possibilites = cible + 1;
		difference = possibilites - source;
	}

	double taux_erreur = (double) difference / possibilites;
	return 100 * (1 - taux_erreur);

	/*
	Exemple :		le rectangle source a 4 de largeur		les largeurs possibles sont {0, 1, 2, 3, 4} (5 poss)
					le rectangle cible a 3 de largeur		il est à 1 unité de la source, taux_erreur = 1 / 5
					la ressemblance du rectangle cible par rapport au rectangle source est de 1 - taux_erreur
					le résultat est compris entre 0 et 1, il faut donc le multipler par 100
	*/
}

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 4) {
		cerr << "Donner en argument les deux fichiers modélisant une mélodie à comparer, la mélodie générée aléatoirement et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation de la chaine de caractères contenant le détail de la comparaison et les variables des valuations */

	string res = "";
	string res_html = "";
	string tmp;
	int somme_valuations = 0;
	int nombre_valuations = 0;

	/* Lecture des fichiers contenant la modélisation de la mélodie source et la modélisation et la mélodie de la cible */

	xml_document<> doc_source;
	xml_document<> doc_cible;
	xml_node<> *noeud_racine_source;
	xml_node<> *noeud_racine_cible;

	ifstream theFile1(argv[1]);
	vector<char> buffer1((istreambuf_iterator<char>(theFile1)), istreambuf_iterator<char>());
	buffer1.push_back('\0');
	doc_source.parse<0>(&buffer1[0]);
	noeud_racine_source = doc_source.first_node("contraintes");

	ifstream theFile2(argv[2]);
	vector<char> buffer2((istreambuf_iterator<char>(theFile2)), istreambuf_iterator<char>());
	buffer2.push_back('\0');
	doc_cible.parse<0>(&buffer2[0]);
	noeud_racine_cible = doc_cible.first_node("contraintes");

	/* C1 : Comparaison de la note min et de la note max */

	tmp = "1. Comparaison de l'intervalle de notes de la mélodie générée avec la (les) mélodie(s) originale(s)";
	res += tmp + "\n\n";
	res_html += "<section>\n\n<article>\n\n<h2>" + tmp + "</h2>\n\n<p>";

	xml_node<> *noeud_note_min_source = noeud_racine_source->first_node("elements-min-max")->first_node("element-min")->first_node("note");
	xml_node<> *noeud_note_max_source = noeud_racine_source->first_node("elements-min-max")->first_node("element-max")->first_node("note");

	xml_node<> *noeud_note_min_cible = noeud_racine_cible->first_node("elements-min-max")->first_node("element-min")->first_node("note");
	xml_node<> *noeud_note_max_cible = noeud_racine_cible->first_node("elements-min-max")->first_node("element-max")->first_node("note");

	Note note_min_source(stoi(noeud_note_min_source->first_node("valeur")->value()), stoi(noeud_note_min_source->first_node("octave")->value()));
	Note note_max_source(stoi(noeud_note_max_source->first_node("valeur")->value()), stoi(noeud_note_max_source->first_node("octave")->value()));

	Note note_min_cible(stoi(noeud_note_min_cible->first_node("valeur")->value()), stoi(noeud_note_min_cible->first_node("octave")->value()));
	Note note_max_cible(stoi(noeud_note_max_cible->first_node("valeur")->value()), stoi(noeud_note_max_cible->first_node("octave")->value()));
	
	int intervalle_source = note_max_source.hauteurNote() - note_min_source.hauteurNote();

	int intervalle_total = 0; // l'intervalle total est l'intervalle source + ce que couvre l'intervalle cible, s'il est plus grand que l'intervalle source
	int intervalle_commun = 0; // l'intervalle commun est l'intervalle couvert à la fois par l'intervalle source et l'intervalle cible

	// Si les intervalles ont une intersection, calculer l'intervalle total et l'intervalle commun
	// Sinon, ne rien faire, laisser 0
	if (note_min_cible <= note_max_source || note_max_cible >= note_min_source) {
		intervalle_total = intervalle_source;
		
		if (note_max_cible > note_max_source) {
			intervalle_total += note_max_cible.hauteurNote() - note_max_source.hauteurNote();
			intervalle_commun -= note_max_cible.hauteurNote() - note_max_source.hauteurNote();
		}
		if (note_min_cible < note_min_source) {
			intervalle_total += note_min_source.hauteurNote() - note_min_cible.hauteurNote();
			intervalle_commun -= note_min_source.hauteurNote() - note_min_cible.hauteurNote();
		}

		if (note_max_cible < note_max_source) {
			intervalle_commun -= note_max_source.hauteurNote() - note_max_cible.hauteurNote();
		}
		if (note_min_cible > note_min_source) {
			intervalle_commun -= note_min_cible.hauteurNote() - note_min_source.hauteurNote();
		}

		intervalle_commun += intervalle_total;
	}

	tmp = "Valuation de l'intervalle des notes de la mélodie générée par rapport aux notes originales : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;

	if (intervalle_commun <= 0) {
		tmp = "0 %";
		somme_valuations += 0;
	} else {
		tmp = to_string((intervalle_commun * 100) / intervalle_total) + " %";
		somme_valuations += (intervalle_commun * 100) / intervalle_total;
	}

	res += tmp + "\n\n";
	res_html += tmp + "</p>\n\n";

	/* C2 : Comparaison de la hauteur moyenne entre deux notes sur une plage de notes donnée (rectangle) */

	tmp = "2. Comparaison du rectangle de notes de la mélodie générée avec la (les) mélodie(s) originale(s)";
	res += tmp + "\n\n";
	res_html += "<h2>" + tmp + "</h2>\n\n<p>";

	xml_node<> *noeud_rectangle_source = noeud_racine_source->first_node("rectangles")->first_node("rectangle");
	xml_node<> *noeud_rectangle_cible = noeud_racine_cible->first_node("rectangles")->first_node("rectangle");

	tmp = "Valuation du rectangle de la mélodie générée par rapport au rectangle original : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;

	// Si les rectangles sont différents, comparer leur aire
	// Sinon, comparer leur objectif
	if (noeud_rectangle_source->first_node("hauteur")->value() != noeud_rectangle_cible->first_node("hauteur")->value()
	 && noeud_rectangle_source->first_node("largeur")->value() != noeud_rectangle_cible->first_node("largeur")->value()) {
		int aire_rectangle_source = stoi(noeud_rectangle_source->first_node("hauteur")->value()) * stoi(noeud_rectangle_source->first_node("largeur")->value());
		int aire_rectangle_cible = stoi(noeud_rectangle_cible->first_node("hauteur")->value()) * stoi(noeud_rectangle_cible->first_node("largeur")->value());
		tmp = to_string(ressemblance(aire_rectangle_source, aire_rectangle_cible)) + " %";
		somme_valuations += ressemblance(aire_rectangle_source, aire_rectangle_cible);
	 } else {
		int objectif_rectangle_source = stoi(noeud_rectangle_source->first_node("objectif")->value());
		int objectif_rectangle_cible = stoi(noeud_rectangle_cible->first_node("objectif")->value());
		tmp = to_string(ressemblance(objectif_rectangle_source, objectif_rectangle_cible)) + " %";
		somme_valuations += ressemblance(objectif_rectangle_source, objectif_rectangle_cible);
	 }

	res += tmp + "\n\n";
	res_html += tmp + "</p>\n\n";

	/* C3 : Comparaison des valeurs de la matrice de Markov */

	tmp = "3. Comparaison des matrices des transitions de notes générées par le modèle de Markov";
	res += tmp + "\n\n";
	res_html += "<h2>" + tmp + "</h2>\n\n<p>";

	map<Note, map<Note, double>> couples_notes; // vecteur de type : (note) [ (note) : %, (note) : % ] | (note) [ (note) : % ]

	// Fichier de la modélisation originale : ajout des % au vecteur (créer une nouvelle pair si (note)1 existe pas)
	// Fichier de la modélisation improvisée : soustraction des % au vecteur si (note)1 et (note)2 existent, sinon ajouter (note)2 à (note)1 (ajouter (note)1 s'il existe pas)

	xml_node<> *noeud_couples_source = noeud_racine_source->first_node("couples-notes");

	for (xml_node<> *noeud_couple = noeud_couples_source->first_node("couple"); noeud_couple; noeud_couple = noeud_couple->next_sibling()) {
		// Récupération de la note (premier noeud "note")
		xml_node<> *noeud_note = noeud_couple->first_node("note");
		Note note(atoi(noeud_note->first_node("valeur")->value()), atoi(noeud_note->first_node("octave")->value()));

		// Récupération de la note suivant la note (second noeud "note")
		noeud_note = noeud_note->next_sibling();
		Note note_suivante(atoi(noeud_note->first_node("valeur")->value()), atoi(noeud_note->first_node("octave")->value()));
		
		// Récupération de la fréquence à laquelle cette seconde note suit la première
		double probabilite = atof(noeud_couple->first_node("probabilite")->value());

		// Ajout de la note1 si elle n'existe pas déjà dans le vecteur
		// Ajout du couple (note2, probabilite) à la note1
		couples_notes[note][note_suivante] = probabilite;
	}
	
	xml_node<> *noeud_couples_cible = noeud_racine_cible->first_node("couples-notes");

	for (xml_node<> *noeud_couple = noeud_couples_cible->first_node("couple"); noeud_couple; noeud_couple = noeud_couple->next_sibling()) {
		// Récupération de la note (premier noeud "note")
		xml_node<> *noeud_note = noeud_couple->first_node("note");
		Note note(atoi(noeud_note->first_node("valeur")->value()), atoi(noeud_note->first_node("octave")->value()));

		// Récupération de la note suivant la note (second noeud "note")
		noeud_note = noeud_note->next_sibling();
		Note note_suivante(atoi(noeud_note->first_node("valeur")->value()), atoi(noeud_note->first_node("octave")->value()));
		
		// Récupération de la fréquence à laquelle cette seconde note suit la première
		double probabilite = atof(noeud_couple->first_node("probabilite")->value());

		// Ajout de la note1 si elle n'existe pas déjà dans le vecteur
			// Sinon, ajout du couple (note2, probabilite) à la note1 s'il n'existe pas déjà dans la note1
			// Sinon, soustraire la probabilité de la note2 à la note1 (valeur absolue)
		if (couples_notes.find(note) == couples_notes.end() || couples_notes[note].find(note_suivante) == couples_notes[note].end()) {
			couples_notes[note][note_suivante] = probabilite;
		} else {
			if (couples_notes[note].find(note_suivante) != couples_notes[note].end()) {
				couples_notes[note][note_suivante] = fabs(couples_notes[note].find(note_suivante)->second - probabilite);
			}
		}
	}

	tmp = "Valuation de la matrice de la mélodie générée par rapport à la matrice originale : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;

	double somme_difference = 0;

	for (auto note : couples_notes) {
		for (auto note_suivante : note.second) {
			somme_difference += note_suivante.second;
		}
	}

	int taux_difference = (couples_notes.size() == 0) ? 100 : abs(100 - (int)(somme_difference * 100 / couples_notes.size()));

	tmp = to_string(taux_difference) + " %";
	somme_valuations += taux_difference;

	res += tmp + "\n\n";
	res_html += tmp + "\n\n<br><br>\n\n";

	tmp = "Valuation de la récompense de la mélodie générée dans la matrice originale : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;
	
	xml_document<> doc;
	xml_node<> *noeud_racine;

	ifstream theFile(argv[3]);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);
	noeud_racine = doc.first_node("notes");

	int recompense_totale = 0;
	int nombre_notes = 0;
	Note *note_precedente = NULL;

	for (xml_node<> *noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
		int valeur_note = stoi(noeud_note->first_node("valeur")->value());
		int octave_note = stoi(noeud_note->first_node("octave")->value());
		Note note(valeur_note, octave_note);

		if (note_precedente != NULL) {
			if (couples_notes.find(*note_precedente) != couples_notes.end() && couples_notes[*note_precedente].find(note) != couples_notes[*note_precedente].end()) {
				++recompense_totale;
			}
		}
		
		note_precedente = new Note(valeur_note, octave_note);
		++nombre_notes;
	}

	int recompense = (nombre_notes - 1 == 0) ? 100 : recompense_totale * 100 / (nombre_notes - 1);

	tmp = to_string(recompense) + " %";
	somme_valuations += recompense;
	res += tmp + "\n\n";
	res_html += tmp + "</p>\n\n";

	/* C4 : Comparaison des répétitions de groupes de notes (patterns, style musical) */

	tmp = "4. Comparaison des patterns de la mélodie générée avec la (les) mélodie(s) originale(s)";
	res += tmp + "\n\n";
	res_html += "<h2>" + tmp + "</h2>\n\n<p>";

	xml_node<> *noeud_patterns_source = noeud_racine_source->first_node("patterns");
	int valeur_totale_patterns_source = 0;
	int nombre_patterns_source = 0;
	int valeur_pattern_max_source = -1;
	int valeur_pattern_min_source = -1;

	for (xml_node<> *noeud_pattern = noeud_patterns_source->first_node("pattern"); noeud_pattern; noeud_pattern = noeud_pattern->next_sibling()) {
		++nombre_patterns_source;

		int valeur_pattern = atoi(noeud_pattern->first_node("taille")->value()) * atoi(noeud_pattern->first_node("amplitude")->value());
		int quantite_pattern = atoi(noeud_pattern->first_node("nombre")->value());

		valeur_totale_patterns_source += valeur_pattern * quantite_pattern;

		nombre_patterns_source += quantite_pattern;
		
		if (valeur_pattern_max_source == -1 || valeur_pattern > valeur_pattern_max_source) {
			valeur_pattern_max_source = valeur_pattern;
		}
		if (valeur_pattern_min_source == -1 || valeur_pattern < valeur_pattern_min_source) {
			valeur_pattern_min_source = valeur_pattern;
		}
	}

	xml_node<> *noeud_patterns_cible = noeud_racine_cible->first_node("patterns");
	int valeur_totale_patterns_cible = 0;
	int nombre_patterns_cible = 0;
	int valeur_pattern_max_cible = -1;
	int valeur_pattern_min_cible = -1;

	for (xml_node<> *noeud_pattern = noeud_patterns_cible->first_node("pattern"); noeud_pattern; noeud_pattern = noeud_pattern->next_sibling()) {
		++nombre_patterns_cible;

		int valeur_pattern = atoi(noeud_pattern->first_node("taille")->value()) * atoi(noeud_pattern->first_node("amplitude")->value());
		int quantite_pattern = atoi(noeud_pattern->first_node("nombre")->value());

		valeur_totale_patterns_cible += valeur_pattern * quantite_pattern;

		nombre_patterns_cible += quantite_pattern;
		
		if (valeur_pattern_max_cible == -1 || valeur_pattern > valeur_pattern_max_cible) {
			valeur_pattern_max_cible = valeur_pattern;
		}
		if (valeur_pattern_min_cible == -1 || valeur_pattern < valeur_pattern_min_cible) {
			valeur_pattern_min_cible = valeur_pattern;
		}
	}

	int valeur_moyenne_patterns_source = 0;
	int valeur_moyenne_patterns_cible = 0;

	if (nombre_patterns_source > 0) valeur_moyenne_patterns_source = (valeur_totale_patterns_source * 100) / nombre_patterns_source;
	if (nombre_patterns_cible > 0) valeur_moyenne_patterns_cible = (valeur_totale_patterns_cible * 100) / nombre_patterns_cible;

	tmp = "Valuation de l'amplitude moyenne des patterns générés par rapport aux patterns originaux : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;
	tmp = to_string(ressemblance(valeur_moyenne_patterns_source, valeur_moyenne_patterns_cible)) + " %";
	somme_valuations += ressemblance(valeur_moyenne_patterns_source, valeur_moyenne_patterns_cible);
	res += tmp + "\n\n";
	res_html += tmp + "\n\n<br><br>\n\n";

	tmp = "Valuation de l'amplitude maximum des patterns générés par rapport aux patterns originaux : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;
	tmp = to_string(ressemblance(valeur_pattern_max_source, valeur_pattern_max_cible)) + " %";
	somme_valuations += ressemblance(valeur_pattern_max_source, valeur_pattern_max_cible);
	res += tmp + "\n\n";
	res_html += tmp + "\n\n<br><br>\n\n";

	tmp = "Valuation de l'amplitude minimum des patterns générés par rapport aux patterns originaux : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;
	tmp = to_string(ressemblance(valeur_pattern_min_source, valeur_pattern_min_cible)) + " %";
	somme_valuations += ressemblance(valeur_pattern_min_source, valeur_pattern_min_cible);
	res += tmp + "\n\n";
	res_html += tmp + "</p>\n\n</article>\n\n";
	
	/* C5 : Comparaison de la répartition des notes de la mélodie (tonalité) */

	tmp = "5. Comparaison de la répartition des notes de la mélodie générée avec la (les) mélodie(s) originale(s)";
	res += tmp + "\n\n";
	res_html += "<article>\n\n<h2>" + tmp + "</h2>\n\n<p>";

	xml_node<> *noeud_repartition_source = noeud_racine_source->first_node("repartition-notes");
	vector<int> repartition_notes_source(12, 0);

	for (xml_node<> *noeud_note_unique = noeud_repartition_source->first_node("note-unique"); noeud_note_unique; noeud_note_unique = noeud_note_unique->next_sibling()) {
		int valeur_note = atoi(noeud_note_unique->first_attribute("valeur")->value());
		int proportion_note = atof(noeud_note_unique->value()) * 100;

		repartition_notes_source[valeur_note] = proportion_note;
	}

	xml_node<> *noeud_repartition_cible = noeud_racine_cible->first_node("repartition-notes");
	vector<int> repartition_notes_cible(12, 0);

	for (xml_node<> *noeud_note_unique = noeud_repartition_cible->first_node("note-unique"); noeud_note_unique; noeud_note_unique = noeud_note_unique->next_sibling()) {
		int valeur_note = atoi(noeud_note_unique->first_attribute("valeur")->value());
		int proportion_note = atof(noeud_note_unique->value()) * 100;

		repartition_notes_cible[valeur_note] = proportion_note;
	}
	
	tmp = "Valuation de la proportion de chaque note contenue dans la mélodie générée par rapport aux proportions originales : ";
	res += tmp;
	res_html += tmp;
	
	res += "\n\nNote\tOriginal\tGénéré\t\tValuation";
	res_html += "</p>\n\n<table>\n<tr><th>Note</th><th>Original</th><th>Généré</th><th>Valuation</th></tr>\n";
	for (unsigned int i = 0; i < repartition_notes_cible.size(); ++i) {
		++nombre_valuations;
		int difference = abs(repartition_notes_source[i] - repartition_notes_cible[i]);
		int ressemblance = 100 - difference;
		res += "\n" + to_string(i) + "\t" + to_string(repartition_notes_source[i]) + " %\t\t" + to_string(repartition_notes_cible[i]) + " %\t\t" + to_string(ressemblance) + " %";
		res_html += "\n<tr><td>" + to_string(i) + "</td><td>" + to_string(repartition_notes_source[i]) + " %</td><td>" + to_string(repartition_notes_cible[i]) + " %</td><td>" + to_string(ressemblance) + " %</td></tr>";
		somme_valuations += ressemblance;
	}
	res += "\n\n";
	res_html += "\n</table>\n\n<p>";

	tmp = "Valuation de la tonalité de la mélodie générée par rapport à la tonalité originale : ";
	res += tmp;
	res_html += tmp;
	++nombre_valuations;

	// Met à -1 la proportion des 4 notes qui ont la plus faible précense dans la mélodie, pour ne garder que les 8 plus présentes
	for (unsigned int iteration = 0; iteration < 4; ++iteration) {
		int note_faible;

		// Répartition des notes de la source
		note_faible = -1;
		for (unsigned note = 0; note < repartition_notes_source.size(); ++note) {
			int proportion = repartition_notes_source[note];
			if ((note_faible == -1 || proportion < note_faible) && proportion > -1) {
				note_faible = note;
			}
		}
		repartition_notes_source[note_faible] = -1;

		// Répartition des notes de la cible
		note_faible = -1;
		for (unsigned note = 0; note < repartition_notes_cible.size(); ++note) {
			int proportion = repartition_notes_cible[note];
			if ((note_faible == -1 || proportion < note_faible) && proportion > -1) {
				note_faible = note;
			}
		}
		repartition_notes_cible[note_faible] = -1;
	}

	// Compare les 8 notes les plus présentes des les mélodies
	unsigned int note_unique = 0;
	bool continuer = true;
	while (note_unique < repartition_notes_source.size() && continuer) {
		continuer = false;
		if ((repartition_notes_source[note_unique] > -1 && repartition_notes_cible[note_unique] > -1)
		 || (repartition_notes_source[note_unique] == -1 && repartition_notes_cible[note_unique] == -1)) {
			continuer = true;
		}
		++note_unique;
	}

	tmp = to_string(continuer * 100) + " %";
	somme_valuations += continuer * 100;
	res += tmp + "\n\n";
	res_html += tmp + "</p>\n\n</article>\n\n</section>\n\n";

	/* Indice de ressemblance de la mélodie générée par rapport aux mélodies originales */

	tmp = "Indice de ressemblance de la mélodie générée par rapport à (aux) mélodie(s) originale(s) : ";
	tmp += to_string(somme_valuations / nombre_valuations) + " %";
	res += tmp + "\n\n";
	res_html += "<pre>" + tmp + "</pre>";

	/* Affichage de la comparaison */

	cout << res;

	/* Enregistrement de la comparaison dans le fichier de sortie */
	
	if (argc >= 5) {
		string nom_fichier_sortie = argv[4];
		ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
		
		if(fichier_sortie) {
			fichier_sortie << "<!DOCTYPE html>\n\n<html>\n\n\t<head>\n\n\t\t<meta charset=\"utf-8\">\n\t\t<title>Comparaison entre deux mélodies modélisées</title>\n";
			fichier_sortie << "\n\t\t<link rel=\"stylesheet\" href=\"../../../InterfaceGraphique/styles/main.css\" type=\"text/css\">";
			fichier_sortie << "\n\t\t<link rel=\"stylesheet\" href=\"../../../InterfaceGraphique/styles/comparator.css\" type=\"text/css\">\n\n\t</head>\n\n\t<body>";
			fichier_sortie << "\n\n" << res_html << "\n\n\t</body>\n\n</html>";
			fichier_sortie.close();
		} else {
			cerr << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
		}

		cerr << "\nLa comparaison entre la modélisation du fichier " << argv[1] << " et du fichier " << argv[2] << " est enregistrée dans le fichier " << nom_fichier_sortie << endl;
	} else {
		cout << "\nAucun fichier de sortie n'est donné en argument du programme" << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}
