#ifndef constraints_h
#define constraints_h
#include <iostream>
#include <vector>
#include <utility>
enum Constraint{mM, rect, follow};
using namespace std;
class Constraints
{
	private:
		vector< pair < Constraint, vector< int > > > m_constraints;
		vector<float> m_obj;
		int m_size;
	public:
		float constraint_mM(vector<int> const & ch,int min, int max)const;
		float constraint_rect(vector<int> const & ch,int h, int t)const;
		float constraint_follow(vector<int> const & ch,int note1, int note2, int d)const;
		Constraints(string fichier);
		~Constraints();
		double valuation(vector < int > const & ch)const;
		void display()const
		{
		 for(auto e: m_constraints)
		 {
		   cout << e.first;
		   for(auto v: e.second)
			 cout << " " << v;
		    cout << endl;
		 }
		 for (auto e:m_obj)
		   cout << e << endl;
		}
		int getMin()const
		{
		  return m_constraints[0].second[0];
		};
		int getMax()const
		{
		  return m_constraints[0].second[1];
		};
		vector<float> getConstraint(vector<int> const & ch)const;
};
#endif