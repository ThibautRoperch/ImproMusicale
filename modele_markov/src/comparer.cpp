#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

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
	int difference = abs(source - cible);
	return 100 - ((difference * 100) / source);
}

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 4) {
		cerr << "Donner en argument les deux fichiers modélisant une mélodie à comparer et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation de la chaine de caractères contenant le détail de la comparaison et les variables des valuations */

	string res = "";
	int somme_valuations = 0;
	int nombre_valuations = 0;

	/* Lecture des fichiers contenant la modélisation de la mélodie source et la modélisation de la mélodie cible */

	xml_document<> doc_source;
	xml_document<> doc_cible;
	xml_node<> *noeud_racine_source;
	xml_node<> *noeud_racine_cible;

	for (int i = 1; i <= 2; ++i) {
		// Initialisation du vecteur contenant les noeuds du fichier source
		ifstream theFile(argv[i]);
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');

		if (i == 1) {
			doc_source.parse<0>(&buffer[0]);
			noeud_racine_source = doc_source.first_node("contraintes");
		} else if (i == 2) {
			doc_cible.parse<0>(&buffer[0]);
			noeud_racine_cible = doc_cible.first_node("contraintes");
		}
	}

	/* C1 : Comparaison de la note min et de la note max */

	res += "1. Comparaison de l'intervalle de notes de la mélodie générée avec la (les) mélodie(s) originale(s)\n\n";

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

	res += "Valuation de l'intervalle des notes de la mélodie générée par rapport aux notes originales : ";
	++nombre_valuations;

	if (intervalle_commun == 0) {
		res += "0 %";
		somme_valuations += 0;
	} else {
		res += to_string((intervalle_commun * 100) / intervalle_total) + " %";
		somme_valuations += (intervalle_commun * 100) / intervalle_total;
	}

	res += "\n\n";

	/* C2 : Comparaison de la hauteur moyenne entre deux notes sur une plage de notes donnée (rectangle) */

	res += "2. Comparaison du rectangle de notes de la mélodie générée avec la (les) mélodie(s) originale(s)\n\n";

	xml_node<> *noeud_rectangle_source = noeud_racine_source->first_node("rectangles")->first_node("rectangle");
	xml_node<> *noeud_rectangle_cible = noeud_racine_cible->first_node("rectangles")->first_node("rectangle");

	res += "Valuation du rectangle de la mélodie générée par rapport au rectangle original : ";
	++nombre_valuations;

	// Si les rectangles sont différents, comparer leur aire
	// Sinon, comparer leur objectif
	if (noeud_rectangle_source->first_node("hauteur")->value() != noeud_rectangle_cible->first_node("hauteur")->value()
	 && noeud_rectangle_source->first_node("largeur")->value() != noeud_rectangle_cible->first_node("largeur")->value()) {
		int aire_rectangle_source = stoi(noeud_rectangle_source->first_node("hauteur")->value()) * stoi(noeud_rectangle_source->first_node("largeur")->value());
		int aire_rectangle_cible = stoi(noeud_rectangle_cible->first_node("hauteur")->value()) * stoi(noeud_rectangle_cible->first_node("largeur")->value());
		res += to_string(ressemblance(aire_rectangle_source, aire_rectangle_cible)) + " %";
		somme_valuations += ressemblance(aire_rectangle_source, aire_rectangle_cible);
	 } else {
		int objectif_rectangle_source = stoi(noeud_rectangle_source->first_node("objectif")->value());
		int objectif_rectangle_cible = stoi(noeud_rectangle_cible->first_node("objectif")->value());
		res += to_string(ressemblance(objectif_rectangle_source, objectif_rectangle_cible)) + " %";
		somme_valuations += ressemblance(objectif_rectangle_source, objectif_rectangle_cible);
	 }

	res += "\n\n";

	/* C4 : Comparaison des répétitions de groupes de notes (patterns, style musical) */

	res += "3. Comparaison des patterns de la mélodie générée avec la (les) mélodie(s) originale(s)\n\n";

	xml_node<> *noeud_patterns_source = noeud_racine_source->first_node("patterns");
	int valeur_totale_patterns_source = 0;
	int nombre_patterns_source = 0;
	int valeur_pattern_max_source = -1;
	int valeur_pattern_min_source = -1;

	for (xml_node<> *noeud_pattern = noeud_patterns_source->first_node("pattern"); noeud_pattern; noeud_pattern = noeud_pattern->next_sibling()) {
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
		int valeur_pattern = atoi(noeud_pattern->first_node("taille")->value()) * atoi(noeud_pattern->first_node("amplitude")->value());
		int quantite_pattern = atoi(noeud_pattern->first_node("nombre")->value());

		valeur_totale_patterns_cible += valeur_pattern * quantite_pattern;

		nombre_patterns_cible += quantite_pattern;
		
		if (valeur_pattern_max_cible == -1 || valeur_pattern > valeur_pattern_max_cible) {
			valeur_pattern_max_cible = valeur_pattern;
		}
		if (valeur_pattern_max_cible == -1 || valeur_pattern < valeur_pattern_min_cible) {
			valeur_pattern_max_cible = valeur_pattern;
		}
	}

	int valeur_moyenne_patterns_source = (valeur_totale_patterns_source * 100) / nombre_patterns_source;
	int valeur_moyenne_patterns_cible = (valeur_totale_patterns_cible * 100) / nombre_patterns_cible;

	res += "Valuation de l'amplitude moyenne des patterns générés par rapport aux patterns originaux : ";
	++nombre_valuations;
	res += to_string(ressemblance(valeur_moyenne_patterns_source, valeur_moyenne_patterns_cible)) + " %";
	somme_valuations += ressemblance(valeur_moyenne_patterns_source, valeur_moyenne_patterns_cible);
	res += "\n\n";

	res += "Valuation de l'amplitude maximum des patterns générés par rapport aux patterns originaux : ";
	++nombre_valuations;
	res += to_string(ressemblance(valeur_pattern_max_source, valeur_pattern_max_cible)) + " %";
	somme_valuations += ressemblance(valeur_pattern_max_source, valeur_pattern_max_cible);
	res += "\n\n";

	res += "Valuation de l'amplitude minimum des patterns générés par rapport aux patterns originaux : ";
	++nombre_valuations;
	res += to_string(ressemblance(valeur_pattern_min_source, valeur_pattern_min_cible)) + " %";
	somme_valuations += ressemblance(valeur_pattern_min_source, valeur_pattern_min_cible);
	res += "\n\n";

	/* C5 : Comparaison de la répartition des notes de la mélodie (tonalité) */


	cout << res;

	cout << nombre_valuations << " valuations" << endl;
	cout << "indice de ressemblance : " << somme_valuations / nombre_valuations << " %" << endl;

	cout << endl;

	return EXIT_SUCCESS;
}
