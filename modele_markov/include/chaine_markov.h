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
