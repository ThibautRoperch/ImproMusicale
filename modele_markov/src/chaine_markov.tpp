using namespace std;

// S'agissant d'un patron de classe (ou modèle générique), ce fichier est inclus dans chaine_markov.h

/**
 * Constructeur
 */
template<class T>
ChaineMarkov<T>::ChaineMarkov() {
	m_nombre_elements_ajoutes = 0;
}

/**
 * Destructeur
 */
template<class T>
ChaineMarkov<T>::~ChaineMarkov() {
	
}

/**
 * Affiche la chaine de Markov (la suite des éléments composant la chaine)
 */
template<class T>
void ChaineMarkov<T>::afficherChaine() const {
	for (unsigned int i = 0; i < m_elements.size(); ++i) {
		cout << *m_elements[i];
		if (i < m_elements.size() -1) cout << ", ";
	}
	cout << endl;
}

/**
 * Affiche la matrice de statistiques de la chaine de Markov
 * Matice à lire à l'horizontal puis à la vertical (l'élément e1 est suivit par l'élément e2 à x %)
 */
template<class T>
void ChaineMarkov<T>::afficherMatrice() const {
	for (unsigned int i = 0; i < m_elements_uniques.size(); ++i) {
		if (i == 0) {
			cout << "e\\suiv\t";
		}
		cout << *m_elements_uniques[i] << "\t";
	}
	cout << endl;
	for (unsigned int i = 0; i < m_statistiques.size(); ++i) {
		cout << *m_elements_uniques[i] << "\t";
		for (auto element : m_statistiques[i]) {
			cout << fixed << setprecision(2) << element << "\t";
		}
		cout << endl;
	}
	cout << endl;
}

/**
 * Réinitialise la chaine de Markov (vide la suite de ses éléments)
 * Cette méthode est déstinée à vider la chaine avant d'en lire une autre,
 * dans le cas d'un apprentissage du modèle de Markov avec plusieurs chaines,
 * pour ne pas fausser le calcul des suivants
 */
template<class T>
void ChaineMarkov<T>::reinitialiserChaine() {
	m_elements.clear();
}

/**
 * Ajoute un élément à la chaine de Markov
 * Revient à ajouter un pointeur vers cet élément à la suite des éléments, à la liste des éléments uniques,
 * ainsi qu'à la liste des éléments suivant l'élément précédemment ajouté (s'il y a eu un élément précédemment ajouté,
 * donc si la suite des éléments n'est pas vide)
 */
template<class T>
void ChaineMarkov<T>::ajouterElement(const T &element) {
	// Recherche de la première occurence de l'élément dans le vecteur de pointeurs des éléments de la chaine
	unsigned int i = 0;
	while (i < m_elements_uniques.size()) {
		if (*m_elements_uniques[i] == element) break;
		++i;
	}

	// Si l'élément a été trouvé, récupérer son pointeur présent dans le tableau
	// Sinon, récupérer son adresse mémoire
	T *ptr_element = NULL;
	if (i < m_elements_uniques.size()) {
		ptr_element = m_elements_uniques[i];
	} else {
		ptr_element = new T(element);
	}
	
	// Ajout à la lite des suivants du dernier élément de la chaîne un pointeur vers le nouvel élément à ajouter
	if (m_elements.size() > 0) {
		m_elements[m_elements.size() - 1]->ajouterAuxSuivants(ptr_element);
	}

	// Ajout d'un pointeur vers l'élément à la fin de la chaine
	m_elements.push_back(ptr_element);

	// Ajout d'un pointeur vers l'élément à l'ensemble des éléments uniques
	if (find(m_elements_uniques.begin(), m_elements_uniques.end(), ptr_element) == m_elements_uniques.end()) {
		m_elements_uniques.push_back(ptr_element);
	}

	// Incrémentation du compteur de nombre d'éléments ajoutés, car la liste des éléments peut être réinitialisée
	++m_nombre_elements_ajoutes;
}

/**
 * Construit la matrice de statistiques à partir de la chaine de Markov
 * Les statistiques calculées sont les fréquences auxquelles un élément est suivit par un autre élément
 * Un vecteur contient les lignes de la matrice
 * Autant de colonnes que de lignes, soit le nombre d'éléments uniques
 * Les statistiques sont des nombres décimaux entre 0 et 1
 */
template<class T>
void ChaineMarkov<T>::calculerStatistiques() {
	// Pour chaque élément unique de la chaine
	for (auto element : m_elements_uniques) {
		// Ajouter à la matrice les statistiques de l'élément
		// Ces stats sont le nombre de fois qu'un élément suit cet élément, divisé par le nombre d'éléments suivants
		vector<double> stats_element;
		map<T *, int> occurences = element->occurencesDesSuivants(); // nombre de fois qu'un élément suit cet élément
		// Pour chaque élément unique de la chaine
		for (auto e : m_elements_uniques) {
			// Si e est présent dans les éléments suivant l'élément, récupérer le nombre d'occurences et le diviser par le nombre d'éléments suivant l'élément
			// Et ajouter le résultat aux satistiques de l'élément, pour e
			// Sinon, ajouter 0
			if (occurences.find(e) != occurences.end()) {
				stats_element.push_back((double) occurences.find(e)->second / element->nombreDeSuivants());
			} else {
				stats_element.push_back(0);
			}
		}
		// ajouter la nouvelle ligne à la matrice m_statistiques
		m_statistiques.push_back(stats_element);
	}
}

/**
 * Génère un élément en se basant sur les statistiques de la chaine de Markov
 * Le dernier élément de la chaine génère un élément en fonction de la fréquence des éléments qui le suivent dans la chaine de Markov originale
 */
template<class T>
T * ChaineMarkov<T>::genererElement() {
	// Si la chaîne contient au moins un élément
	if (m_elements.size() > 0) {
		// Générer un nouvel élément choisi parmi les éléments suivant le dernier élément de la chaîne
		T *nouvel_element = m_elements[m_elements.size() - 1]->piocherParmiSuivants();
		// Si l'élément tiré au sort n'est pas NULL (il est NULL dans le cas où le dernier élément de la chaîne n'a pas de suivant)
		if (nouvel_element != NULL) {
			// Ajouter l'élément à la fin de la chaîne
			m_elements.push_back(nouvel_element);
			return nouvel_element;
		} else {
			cerr << "Le dernier élément de la chaîne de Markov est unique : il n'a pas d'élément suivant" << endl;
		}
	} else {
		cerr << "Impossible de générer un élément à partir d'une chaine de Markov vide" << endl;
	}
	return NULL;
}

/**
 * Retourne la position d'un élément dans la liste des éléments uniques ou -1 s'il n'y existe pas
 * La méthode find de la STL compare l'adresse des pointeurs et non la valeur des pointeurs
 * Cette fonction compare donc les éléments uniques avec l'élément donné en paramètre pour trouver sa position dans la liste
 */
template<class T>
int ChaineMarkov<T>::positionElementUnique(const T &element) const {
	int position = -1;

	for (unsigned int i = 0; i < m_elements_uniques.size(); ++i) {
		if (element == *m_elements_uniques[i]) {
			position = i;
		}
	}

	return position;
}

/**
 * Calcule la récompense d'une chaine dans la chaine de Markov avec la matrice de statistiques
 * La récompense totale d'une chaine dans le modèle est de 1 pour chacune de ses transitions réalisables dans le modèle
 * Un automate peut représenter la chaine de Markov et aider à comprendre cette notion de récompense pour une chaine
 */
template<class T>
int ChaineMarkov<T>::recompenseChaine(const vector<T> &chaine) {
	double recompense_totale = 0;

	for (unsigned int i = 0; i < chaine.size() - 1; ++i) {
		int pos_element_actuel = positionElementUnique(chaine[i]);
		int pos_element_suivant = positionElementUnique(chaine[i+1]);
		if (pos_element_actuel != -1 && pos_element_suivant != -1) {
			if (m_statistiques[pos_element_actuel][pos_element_suivant] > 0) ++recompense_totale;
		}
	}
	
	return recompense_totale;
}

/**
 * Calcule la probabilité que la chaine donnée en paramètre est réalisabe dans la chaine de Markov
 * avec la matrice de statistiques
 * Tous les éléments uniques ont une probabilité équivalante de commencer une chaine, à savoir 1 / nombre_elements_uniques
 */
template<class T>
double ChaineMarkov<T>::probabiliteChaineRealisable(const vector<T> &chaine) {
	double probabilite = 1.0 / m_elements_uniques.size();

	for (unsigned int i = 0; i < chaine.size() - 1; ++i) {
		int pos_element_actuel = positionElementUnique(chaine[i]);
		int pos_element_suivant = positionElementUnique(chaine[i+1]);
		if (pos_element_actuel != -1 && pos_element_suivant != -1) {
			probabilite *= m_statistiques[pos_element_actuel][pos_element_suivant];
		} else {
			probabilite *= 0;
		}
	}
	
	return probabilite;
}
