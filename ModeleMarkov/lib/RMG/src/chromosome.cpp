#include "../include/chromosome.h"
#include <stdlib.h> 
#include <sstream>
#include <math.h>
#include<iomanip>
Chromosome::Chromosome(const vector< int >& notes, double val, int g): m_value(val), m_notes(), m_gen(g)
{
  for(auto e: notes)
	m_notes.push_back(e);
}
Chromosome::~Chromosome()
{

}
bool Chromosome::mutation(int size)
{
  int b, v;
  int i = rand() % size;
  // la mutation a 1 chance sur size d'etre effectue
  if(i == 0)
  {
	i == rand() % m_notes.size();
	v = m_notes[i];
	v = v % 12;
	m_notes[i]-= v;
	vector<bool> tmp;
	for(int j = 3; j >=0 ; j--)
	{
	  b = v/pow(2,j);
	  if(b!=0)
	  {
		v-=pow(2,j);
		tmp.push_back(true);
	  }
	  else
	  {
		tmp.push_back(false);
	  }
	}
	v = rand() % 4;
	tmp[v] = !tmp[v];
	b = 0;
	for(int j = 3; j >=0 ; j--)
	{
	  if(tmp[j])
		b+=pow(2,j);
	}
	if(b>11)
	  b-=12;
	m_notes[i] += b;
	return true;
  }
  return false;
}

void Chromosome::loSearch(Constraints const & co)
{
  double val, valtmp;
  bool change = true;
  while(change)
  {
	change = false;
	for(int i = 0; i < m_notes.size(); i++)
	{
	  for(int j = 0; j < m_notes.size(); j++)
	  {
		 if(m_notes[i] != m_notes[j])
		 {
		  vector<int> tmp = m_notes;
		  tmp[i] = tmp[j];
		  tmp[j] = m_notes[i];
		  val = getValue();
		  valtmp = co.valuation(tmp);
		  if(val < valtmp)
		  {
			m_notes = tmp;
			m_value=valtmp;
			change = true;
		  }
		}
	  }
	}
  }
}
string Chromosome::toString() const
{
  ostringstream out;
  for(auto e: m_notes)
	out <<setw(4)<< e;
  return out.str();
}
