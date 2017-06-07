/**
 * https://github.com/craigsapp/midifile#midi-file-reading-examples
 * https://usermanuals.finalemusic.com/Finale2012Mac/Content/Finale/MIDI_Note_to_Pitch_Table.htm
 */

#include <iostream>
#include <iomanip>

#include "../lib/craigsapp-midifile-5a7dc97/include/MidiFile.h"
#include "../lib/craigsapp-midifile-5a7dc97/include/Options.h"

using namespace std;

int main(int argc, char* argv[]) {

	cout << endl;

	/* Vérification du nombre d'arguments */

	if (argc < 2) {
		cerr << "Donner en argument le fichier contenant la partition au format MIDI et le fichier de sortie\n" << endl;
		return EXIT_FAILURE;
	}
	
	/* Lecture du fichier contenant la partition au format MIDI */

	cout << "Lecture de la partition contenue dans le fichier " << argv[1] << " au format MIDI" << endl;

	MidiFile midifile(argv[1]);
	
	// Analyse des parties musicales présentes la partition

	int nb_parties_musicales = midifile.getTrackCount();
	int partie_musicale_selectionnee = 0;

	if (nb_parties_musicales > 1) {
		cerr << "\nPlusieurs parties musicales sont présentes dans la partition :" << endl;
		int i;
		for (i = 0; i < nb_parties_musicales; i++) {
			cerr << "  [" << i << "] Partie " << i << endl;
		}
		int indice_partie;
		cerr << "\nDonner l'indice de la partie dont il faut extraire la mélodie :" << endl;
		cin >> indice_partie;

		if (indice_partie < nb_parties_musicales) {
			partie_musicale_selectionnee = (int) indice_partie;
		}
	}
	
	// Extraction de la mélodie de la partie selectionnée, en fonction de la structure de la partition

	cout << "\nExtraction des notes composant la mélodie de la partie " << partie_musicale_selectionnee << endl;
	unsigned indice_note = 0;
	string res = "<notes>\n";

	// Lecture de la valeur des notes de la partie musicale sélectionnée
	for (int note = 0; note < midifile[partie_musicale_selectionnee].size(); note++) {
		// Les notes qui sont des messages MIDI ont leur marque qui commence par 0
		if (to_string(midifile[partie_musicale_selectionnee][note].tick)[0] != '0') {
			int hauteur_note = (int)midifile[partie_musicale_selectionnee][note][1];
			hauteur_note -= 21 - 9; // la première note commence à 21, +9 pour faire correspondre avec la version MusicXML des partitions
			
			// Si la note a une intensité strictement supérieure à 0 et une hauteur recalculée supérieure ou égale à 0, calculer sa valeur et son octave
			if ((int)midifile[partie_musicale_selectionnee][note][2] > 0 && hauteur_note >= 0) {
				res += "  <note id=\"" + to_string(indice_note++) + "\">\n";

				int octave_note = hauteur_note / 12;
				int valeur_note = hauteur_note % 12;

				res += "    <valeur>" + to_string(valeur_note) + "</valeur>\n";
				res += "    <octave>" + to_string(octave_note) + "</octave>\n";

				res += "  </note>\n";
			}
		}
	}

	cout << "\nExtraction de la mélodie terminée" << endl;

	// Fin de la lecture du fichier contenant la partition

	res += "</notes>";

	cout << "\nLecture terminée" << endl;

	/* Enregistrement de la mélodie extraite dans le fichier de sortie ou affichage à défaut de fichier donné en argument */
	 
	if (argc >= 3) {
		string nom_fichier_sortie = argv[2];
		ofstream fichier_sortie(nom_fichier_sortie, ios::out | ios::trunc);
		
		if(fichier_sortie) {
			fichier_sortie << res;
			fichier_sortie.close();
		} else {
			cout << "\nImpossible de créer le fichier " << nom_fichier_sortie << endl;
		}

		cout << "\nLa mélodie de la partie " << partie_musicale_selectionnee << " de la partition du fichier " << argv[1] << " est enregistrée dans le fichier " << nom_fichier_sortie << endl;
	} else {
		cout << "\nAucun fichier de sortie n'est donné en argument du programme" << endl;
		cout << "\n" << res << endl;
	}

	cout << endl;
	
	return EXIT_SUCCESS;
}
