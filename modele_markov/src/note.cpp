#include "../include/note.h"

using namespace std;

Note::Note(int valeur, int octave) : m_valeur(valeur), m_octave(octave) {

}

Note::Note(const Note &note) : m_valeur(note.valeurNote()), m_octave(note.octaveNote()) {

}

Note::~Note() {
	for (auto note : m_notes_suivantes) {
		free(note);
	}
}

void Note::afficherNote(ostream &flux) const {
	flux << m_valeur << "(" << m_octave << ")";
	/*flux << " [ ";
	for (auto note : m_notes_suivantes) {
		flux << note->valeurNote() << " ";
	}
	flux << "]";*/
}

void Note::ajouterAuxSuivants(Note *n) {
	m_notes_suivantes.push_back(n);
}

Note * Note::piocherParmiSuivants() const {
	if (m_notes_suivantes.size() > 0) {
		int tirage_aleatoire = rand() % m_notes_suivantes.size(); // entre 0 et la taille du vecteur de notes suivantes - 1

		/*unsigned int i = 0;
		double proba_cumulee = 0.0;
		// Pour chaque note N parmi le tableau de notes suivantes
		while (i < m_notes_suivantes.size()) {
			double proba_min_note = proba_cumulee;
			double proba_max_note = proba_cumulee + probabiliteNoteSuivante(m_notes_suivantes[i]);
			// Retourne la note N si l'entier tiré aléatoirement est compris dans la fourchette de la fréquence d'apparition de la note N
			if (proba_min_note <= tirage_aleatoire && tirage_aleatoire < proba_max_note) {
				return m_notes_suivantes.at(i);
			}
			proba_cumulee += probabiliteNoteSuivante(m_notes_suivantes[i]);
			++i;
		}*/

		return m_notes_suivantes[tirage_aleatoire];
	}
	return NULL;
}

map<Note *, int> Note::occurencesDesSuivants() const {
	map<Note *, int> res;

	for (auto n : m_notes_suivantes) {
		if (res.find(n) != res.end()) {
			res[n] = res[n] + 1;
		} else {
			res.insert(pair<Note *, int>(n, 1));
		}
	}

	return res;
}

int Note::nombreDeSuivants() const {
	return m_notes_suivantes.size();
}
