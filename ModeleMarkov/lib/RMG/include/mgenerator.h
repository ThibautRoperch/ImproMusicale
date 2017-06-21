#ifndef music_generator_h
#define music_generator_h
#define POP 100
#include<vector>
#include<map>
#include <string>
#include "chromosome.h"
#include "constraints.h"
using namespace std;
class MGenerator
{
  private:
	map<int, vector<float>> m_res;
	int m_genCour;
	string m_sortie;
	 // nombre de  note dans une solution
	int m_nbNotes; 
	//nombre d'octave
	int m_nbOct;
	// ensemble de solution
	vector< Chromosome > m_population;	 
	//objet permetant de vérifier les contraintes et d'évaluer une solution
	Constraints m_objectiv;
	bool w_localSearch;
	bool w_ts;
  public:
	MGenerator(int size, string const & inputFile, string const & outputFile, bool ls, bool ts);
	~MGenerator();
	 //déroulement de l'algorithme
	void generation(int g);
	 //renvoi les enfants créés par les solutions dont l'indice est renseigné par le vecteur en parammètre
	vector<Chromosome> crossOver(vector<int> selected);
	//ajoute une solution 
	void addChromosome(vector<int> const & ch, double val);
	// évalue l'ensemble de solution
	double valuation()const;
	// retourne la selection des participants 
	vector<int> tournamentSelection();
	void out(int i);
};
#endif