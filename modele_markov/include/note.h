#ifndef NOTE_H_
#define NOTE_H_

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>

class Note {

private:

	int m_valeur;
	int m_octave;
	// int m_duree;
	std::vector<Note *> m_notes_suivantes;


public:

	Note(int valeur, int octave);
	Note(const Note &note);
	~Note();

	int valeurNote() const { return m_valeur; };
	int octaveNote() const { return m_octave; };
	int hauteurNote() const { return m_valeur + m_octave * 12; };
	
	void afficherNote(std::ostream &flux) const;

	// Méthodes propres au modèle de Markov appellées dans le template chaine_markov.tpp
	void ajouterAuxSuivants(Note *note);
	Note * piocherParmiSuivants() const;
	std::map<Note *, int> occurencesDesSuivants() const;
	int nombreDeSuivants() const;
};

#endif
