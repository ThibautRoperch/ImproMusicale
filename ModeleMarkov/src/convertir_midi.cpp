/**
 *
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
		cerr << "Donner en argument le fichier contenant la partition au format MIDI et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}

	/* Initialisation du fichier MIDI et des propriétés par défaut des notes MIDI qui composent la mélodie */

	MidiFile fichier_midi;
	fichier_midi.addTracks(1);

	long duree_noire = 120; // nombre de ticks par noire
 	fichier_midi.setTicksPerQuarterNote(duree_noire);

	int tick_action = 0;
	int piste = 1; // la piste 0 est pour des messages

	int vitesse = 64;
	int canal = 0;

	/* Lecture du fichier contenant la partition au format XML */

	cout << "Lecture de la mélodie contenue dans le fichier " << argv[1] << " au format XML" << endl;

	xml_document<> doc;
	xml_node<> *noeud_racine;

	// Initialisation du vecteur contenant les noeuds du fichier

	ifstream theFile(argv[1]);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	noeud_racine = doc.first_node("notes");

	// Création et ajout des notes MIDI de la mélodie du fichier à la piste MIDI

	cout << "\nCréation et ajout des notes MIDI de la mélodie du fichier " << argv[1] << " à la piste MIDI" << endl;

	// Itération sur les parties (noeuds "note" du noeud "notes")
	for (xml_node<> *noeud_note = noeud_racine->first_node("note"); noeud_note; noeud_note = noeud_note->next_sibling()) {
		int valeur_note = stoi(noeud_note->first_node("valeur")->value());
		int octave_note = stoi(noeud_note->first_node("octave")->value());
		
		int hauteur_note = valeur_note + octave_note * 12;

		hauteur_note += 21 - 9; // la première note commence à 21, -9 pour faire correspondre avec la version MusicXML des partitions

		fichier_midi.addNoteOn(piste, tick_action, canal, hauteur_note, vitesse);
		fichier_midi.addNoteOff(piste, tick_action += duree_noire, canal, hauteur_note, vitesse);
	}

	cout << "\nCréation de la mélodie MIDI terminée" << endl;

	// Fin de la lecture du fichier contenant la mélodie

	cout << "\nLecture terminée" << endl;

	/* Enregistrement de la mélodie MIDI dans le fichier de sortie */

	string nom_fichier_sortie = argv[2];

	fichier_midi.sortTracks(); // s'assurer que les temps de ticks sont dans le bon ordre
	fichier_midi.write(nom_fichier_sortie);

	cerr << "\nLe fichier " << nom_fichier_sortie << " contient les propriétés de la mélodie du fichier " << argv[1] << endl;

	cout << endl;

	return EXIT_SUCCESS;
}
