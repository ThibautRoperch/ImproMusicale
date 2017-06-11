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
	int m_nombre_elements_ajoutes;

public:

	// Constructeur et destructeur

	ChaineMarkov();
	~ChaineMarkov();

	// Accesseurs

	std::vector<T *> chaine() const { return m_elements; };
	std::vector<T *> elementsUniques() const { return m_elements_uniques; };
	std::vector<std::vector<double>> matrice() const { return m_statistiques; };
	T * dernierElement() const { return m_elements.back(); };
	int nombreElementsAjoutes() const { return m_nombre_elements_ajoutes; };
	
	// Affichages

	void afficherChaine() const;
	void afficherMatrice() const;

	// Autres méthodes
	
	void reinitialiserChaine();

	void ajouterElement(const T &element);
	void calculerStatistiques();

	T * genererElement();

	int positionElementUnique(const T &element) const;
	int recompenseChaine(const std::vector<T> &chaine) const;
	double probabiliteChaineRealisable(const std::vector<T> &chaine) const;

};

#include "../src/chaine_markov.tpp"

#endif
