#include "../include/mgenerator.h"
#include<algorithm>
#include <fstream>
#include<thread>
#include<string>
void thread_local_search(pair<Chromosome*, Constraints> p)
{
  p.first->loSearch(p.second);
}
MGenerator::MGenerator(int size, string const & inputFile, string const & outputFile, bool ls):m_nbNotes(size), m_population(), 
m_objectiv(inputFile), m_sortie(outputFile), m_res(), w_localSearch(ls), m_genCour(0)
{
  
  thread losth[POP];
  //o + m = note max, m = note min
  int o,m; 
  m = m_objectiv.getMin();
  o = m_objectiv.getMax() - m;
  for( int  i = 0; i < POP; i++)
  {
	vector<int> tmp;
	for(int  j = 0; j < size; j++)
	  tmp.push_back((rand() % o)+m);
	double v =m_objectiv.valuation(tmp);
	//m_objectiv.display();
	addChromosome(tmp, v);
  }
  if(ls)
  {
	for(int i = 0; i < POP; i++)
	{
		losth[i] = thread(thread_local_search, make_pair(&m_population[i], m_objectiv));
	}
  
	for(int i = 0; i < POP; i++)
	{
		losth[i].join();
	}
  }
  
}
MGenerator::~MGenerator()
{

}

void MGenerator::addChromosome(const vector< int >& ch, double val)
{
m_population.push_back(Chromosome(ch, val, m_genCour));
}

vector<int> MGenerator::tournamentSelection()
{
  vector<int>  res;
  double val1, val2;
  double rd;
  random_shuffle ( m_population.begin(), m_population.end() );
  for(int i = 0; i < m_population.size()-1; i+=2)
  {
	val1=m_population[i].getValue()*(m_population[i].getGen()+1);
	val2= m_population[i+1].getValue()*(m_population[i+1].getGen()+1);
	rd = rand() % (int)(val1+val2);
	if(rd <= val1)
	{
	  res.push_back(i);
	}
	else
	{
	  res.push_back(i+1);
	}
  }
  return res;
}

double MGenerator::valuation() const
{
  double val = 0.0;
  for(auto e: m_population)
	val+=e.getValue();
  return val;
}
void MGenerator::generation(int g)
{
  bool mutate;
  int  i, sizepop;
  double value;
  value = valuation();
  sizepop = m_population.size();
  for(int nbgen = 1; nbgen < g; nbgen++)
  {
	m_genCour++;
	i = 0;
	mutate = false;
	vector<int> selected = tournamentSelection();
	vector<Chromosome> childs = crossOver(selected);
	while(!mutate && i < childs.size())
	{
	  mutate = childs[i].mutation(childs.size());
	  i++;
	}
	thread losth[childs.size()];
	if(w_localSearch)
	{
	  for(int  i = 0; i < childs.size(); i++)
	  {
		losth[i] = thread(thread_local_search,make_pair(&childs[i],m_objectiv));
		m_population.push_back(childs[i]);
	  }
	  for(int  i = 0; i < childs.size(); i++)
	  {
		losth[i].join();
	  }
	}
	sort (m_population.begin(), m_population.end());
	for(int j = 0; j < childs.size(); j++)
	{
	  m_population.erase(m_population.begin());
	}
	if(nbgen%10 == 0)
	{
	  out(nbgen);
	}
  }
  sort(m_population.begin(), m_population.end());
  out(-1);
}
vector< Chromosome > MGenerator::crossOver(vector< int > selected)
{
  vector <Chromosome> res;
  /*for(auto e: selected)
  {
	cout << "individu :" << e <<endl;
	
	for(auto f : m_population[e].getNotes())
	{
	  cout << f << " ";
	}
	cout << endl;
  }
  cout << "___________________________________________________________" << endl;*/
  random_shuffle ( selected.begin(), selected.end() );
  double val;
  for(int  i = 0; i < selected.size()-1; i++){
	int rnd;
	vector<int> child1, child2;
	for(int j = 0; j < m_nbNotes; j++)
	{
	  rnd = rand() % 2;
	  if(rnd == 0)
	  {
		child1.push_back(m_population[selected[i]].getNote(j));
		child2.push_back(m_population[selected[i+1]].getNote(j));
	  }
	  else
	  {
		child1.push_back(m_population[selected[i+1]].getNote(j));
		child2.push_back(m_population[selected[i]].getNote(j));
	  }
	}
	val = m_objectiv.valuation(child1);
	res.push_back(Chromosome(child1, val, m_genCour));
	val = m_objectiv.valuation(child2);
	res.push_back(Chromosome(child2, val, m_genCour));
	i++;
  }
  /*cout << "child :" << endl;
  for(auto e: res)
  {
	for(auto f: e.getNotes())
	{
	  cout << f << " ";
	}
	cout << endl;
  }
  cout << "__________________________________________________________________________" << endl;*/
  return res;
}
void MGenerator::out(int i)
{

  string file;
  file.append(m_sortie);
  if (i != -1)
  {
    file.append(to_string(i));
    file.append(".xml");
  }
  cout << file << endl;
  filebuf fb;
  fb.open(file, ios::out);
  ostream os(&fb);
  os << "<notes>" <<endl;
  int j = POP;
  
  cout << " gen  :" << m_population[j-1].getGen() << endl;
  cout << "valeur :" << m_population[j-1].getValue()<<endl;
  vector<int> res = m_population[j-1].getNotes();
  m_res.insert(make_pair(i,m_objectiv.getConstraint(res)));
  for(auto e: res)
  {
	os << "<note>" << endl;
	os << "<valeur>" << e % 12 << "</valeur>" << endl;
	os << "<octave>" << e / 12 << "</octave>" << endl;
	os << "</note>" << endl;
  }
  os << "</notes>" << endl;
  fb.close();
  
}
void MGenerator::getEv() const
{

}
