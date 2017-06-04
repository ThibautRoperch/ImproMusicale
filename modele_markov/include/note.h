/**
 * Cette classe représente une note composant une mélodie
 *
 * Une note est définie par sa valeur (de 0 à 11), son octave (de 0 à -pas de limite-), et une liste de notes
 * La corresepondance entre valeur numérique et notation anglaise est définie lors de la lecture d'une partition
 * La liste d'une note contient les notes qui la suivent (ces éléments ne sont pas forcément uniques), et est destinée
 * à des fins de génération aléatoire d'une note parmi les notes qui la suivent
 * La hauteur d'une note est abolue par rapport à la note la plus basse, qui a une valeur 0 et une octave 0
 *
 * Une note est déstinée à appartenir à une chaine de Markov, c'est le type de la classe générique ChaineMarkov
 */

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
