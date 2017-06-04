#include "../include/note.h"

using namespace std;

/**
 * Constructeur
 */
Note::Note(int valeur, int octave) : m_valeur(valeur), m_octave(octave) {

}

/**
 * Constructeur par recopie
 */
Note::Note(const Note &note) : m_valeur(note.valeurNote()), m_octave(note.octaveNote()) {

}

/**
 * Destructeur
 */
Note::~Note() {
	for (auto note : m_notes_suivantes) {
		free(note);
	}
}

/**
 * Affiche la note
 */
void Note::afficherNote(ostream &flux) const {
	flux << m_valeur << "(" << m_octave << ")";
	/*flux << " [ ";
	for (auto note : m_notes_suivantes) {
		flux << note->valeurNote() << " ";
	}
	flux << "]";*/
}

/**
 * Ajoute à la liste des notes suivantes la note donnée en paramètre
 */
void Note::ajouterAuxSuivants(Note *n) {
	m_notes_suivantes.push_back(n);
}

/**
 * Retourne une note prise aléatoirement dans la liste des notes suivantes
 * Si une note x est présente dan la liste plus de fois qu'une note y, elle aura plus de chance d'être tirée au hasard
 */
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

/**
 * Retourne un tableau associatif contenant en clef les notes qui suivent la note,
 * et en valeur le nombre de fois qu'ellesla suivent
 */
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
