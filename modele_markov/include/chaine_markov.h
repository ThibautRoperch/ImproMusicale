/**
 * Cette classe générique implémente une chaine de Markov
 * https://fr.wikipedia.org/wiki/Cha%C3%AEne_de_Markov
 *
 * La suite des éléments est enregistrée dans un vecteur, ainsi que la liste des éléments uniques
 * Une matrice de statistiques de la chaine est calculée en fonction des éléments suivant chaque élément
 * Cette matrice peut être réutilisée à des fins de génération aléatoire
 *
 * Le type T des éléments de la chaine de Markov doivent implémenter des méthodes dont voici la signature :
 *   ajouterAuxSuivants : T* -> void
 *   piocherParmiSuivants const : void -> T*
 *   occurencesDesSuivants const : void -> map <T*, int>
 *   nombreDeSuivants const : void -> int
 */

#ifndef CHAINE_MARKOV_H_
#define CHAINE_MARKOV_H_

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <map>
#include <algorithm>

template<class T>
class ChaineMarkov {

private:

	std::vector<T *> m_elements;
	std::vector<T *> m_elements_uniques;
	std::vector<std::vector<double>> m_statistiques;

public:

	ChaineMarkov();
	~ChaineMarkov();

	std::vector<T *> chaine() { return m_elements; };
	std::vector<T *> elementsUniques() { return m_elements_uniques; };
	std::vector<std::vector<double>> matrice() { return m_statistiques; };
	
	void afficherChaine() const;
	void afficherMatrice() const;

	void reinitialiserChaine();

	void ajouterElement(const T &element);
	void calculerStatistiques();

	T * genererElement();

	int positionElement(const T &element) const;
	int recompenseChaine(const std::vector<T> &chaine);
	double probabiliteChaineRealisable(const std::vector<T> &chaine);

};

#include "../src/chaine_markov.tpp"

#endif
