/**
 * Programme de conversion d'un ensemble de fichiers au format XML épuré vers un fichier au format MIDI
 * 
 * https://github.com/craigsapp/midifile/blob/master/src-programs/createmidifile2.cpp
 */

#include <iostream>
#include <iomanip>

#include "../lib/rapidxml-1.13/rapidxml.hpp"
#include "../lib/craigsapp-midifile-5a7dc97/include/MidiFile.h"


using namespace rapidxml;
using namespace std;

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 3) {
		cerr << "Donner en argument le ou les fichier(s) contenant une mélodie et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation du fichier MIDI et des propriétés par défaut des notes MIDI qui composent la mélodie */

	MidiFile fichier_midi;
	fichier_midi.addTracks(argc - 2);

	long duree_noire = 120; // nombre de ticks par noire
 	fichier_midi.setTicksPerQuarterNote(duree_noire);

	int velocite = 64;
	int canal = 0;

	/* Lecture des fichiers contenant une mélodie */

	int piste = 0; // la piste 0 est pour des messages

	for (int i = 1; i < argc - 1; ++i) {
		piste++;
		int tick_action = 0;

		cout << "Lecture de la mélodie contenue dans le fichier " << argv[i] << " au format XML" << endl;

		xml_document<> doc;
		xml_node<> *noeud_racine;

		// Initialisation du vecteur contenant les noeuds du fichier

		ifstream theFile(argv[i]);
		vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		doc.parse<0>(&buffer[0]);

		noeud_racine = doc.first_node("notes");

		// Création et ajout des notes MIDI de la mélodie du fichier à la piste MIDI

		cout << "\nCréation et ajout des notes MIDI de la mélodie du fichier " << argv[i] << " à la piste MIDI" << endl;

		// Itération sur les parties (noeuds "note" du noeud "notes")
		for (xml_node<> *noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
			int valeur_note = stoi(noeud_note->first_node("valeur")->value());
			int octave_note = stoi(noeud_note->first_node("octave")->value());
			
			int hauteur_note = valeur_note + octave_note * 12;

			hauteur_note += 21 - 9; // la première note commence à 21, -9 pour faire correspondre avec la version MusicXML des partitions

			fichier_midi.addNoteOn(piste, tick_action, canal, hauteur_note, velocite);
			fichier_midi.addNoteOff(piste, tick_action += duree_noire, canal, hauteur_note, velocite);
		}

		cout << "Création de la mélodie MIDI terminée\n" << endl;
	}

	// Fin de la lecture des fichier contenant une mélodie

	cout << "Lecture des fichiers terminée" << endl;

	/* Enregistrement des pistes MIDI dans le fichier de sortie */

	string nom_fichier_sortie = argv[argc - 1];

	fichier_midi.sortTracks(); // s'assurer que les temps des ticks sont dans le bon ordre
	fichier_midi.write(nom_fichier_sortie);

	if (argc == 3) {
		cerr << "\nLe fichier " << nom_fichier_sortie << " contient la mélodie du fichier " << argv[1] << " au format MIDI" << endl;
	} else {
		cerr << "\nLe fichier " << nom_fichier_sortie << " contient les mélodie des fichiers suivants au format MIDI :" << endl;
		for (int i = 1; i < argc - 1; ++i) {
			cerr << "- " << argv[i] << endl;
		}
	}

	cout << endl;

	return EXIT_SUCCESS;
}
