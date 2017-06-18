#ifndef chromosome_h
#define chromosome_h
#include <vector>
#include <iostream>
#include "constraints.h"
using namespace std;
class Chromosome
{
  private:
	vector<int> m_notes;
	int m_gen;
	double m_value;
  public:
	Chromosome(vector< int > const & notes, double val, int g);
	~Chromosome();
	int getGen()const
	{
	  return m_gen;
	}
	// operateur de muttation
	bool mutation(int size);
	// retourne l'évaluation
	double getValue()const
	{
		return m_value;
	}
	
	bool operator<(Chromosome const & c2)const
	{
	  return m_value < c2.m_value;
	}
	vector<int> const & getNotes()const
	{
	  return m_notes;
	}
	int const & getNote(int i)const
	{
	  return m_notes[i];
	}
	void loSearch(Constraints const & co);
	string toString()const;

};

#endif