#include "../include/constraints.h"
#include <sstream>
#include <math.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include "../lib/rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;
Constraints::Constraints(string fichier): m_size(0), m_obj()
{
  int min, max, h, n1, n2, nbr, d, t;
  float o;
  vector<int> tmp;
  xml_document<> doc;
  xml_node<> * root_node;
  // lecture du fichier xml
  ifstream theFile (fichier);
  vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
  buffer.push_back('\0');
  // initialise le buffer à l'aide du fichier
  doc.parse<0>(&buffer[0]);
  // trouver la racine
  root_node = doc.first_node("contraintes");
  
  // min max
  xml_node<> * min_max_node = root_node->first_node("elements-min-max");
	m_obj.push_back(1);
	//debut element min
	xml_node<> * min_node = min_max_node->first_node("element-min");
	  xml_node<> * note_node = min_node->first_node("note");
		xml_node<> * value_node = note_node->first_node("valeur");
		xml_node<> * octave_node = note_node->first_node("octave");
		min = stoi(value_node->value());
		min+= stoi(octave_node->value())*12;
	// fin element_min
	
	// debut element max
	xml_node<> * max_node = min_max_node->first_node("element-max");
		note_node = max_node->first_node("note");
		  value_node = note_node->first_node("valeur");
		  octave_node = note_node->first_node("octave");
		  max = stoi(value_node->value());
		  max += stoi(octave_node->value())*12;
	//fin element max
	tmp.push_back(min);
	tmp.push_back(max);
	m_constraints.push_back(make_pair(mM,tmp));
	tmp.clear();
		  
  // rect
  xml_node<> * rectangles_node = root_node->first_node("rectangles");
	xml_node<> * rectangle_node = rectangles_node->first_node("rectangle");
	  xml_node<> * objectif_node = rectangle_node->first_node("objectif");
	  o = stof(objectif_node->value());
	
	  m_obj.push_back(o);
	  
	  xml_node<> * hauteur_node = rectangle_node->first_node("hauteur");
	  h = stoi(hauteur_node->value());
	  
	  xml_node<> * largeur_node = rectangle_node->first_node("largeur");
	  t = stoi(largeur_node->value());
	  
	tmp.push_back(h);
	tmp.push_back(t);
	tmp.push_back(nbr);
	m_constraints.push_back(make_pair(rect,tmp));
	tmp.clear();
  // couples_notes
  xml_node<> * couples_notes_node = root_node->first_node("couples-notes");
  //couple
  for (xml_node<> * couple_node = couples_notes_node->first_node("couple"); couple_node; couple_node = couple_node->next_sibling())
  {
	//note1
	note_node = couple_node->first_node("note");
	  value_node = note_node->first_node("valeur");
	  octave_node = note_node->first_node("octave");
	  n1 = stoi(value_node->value());
	  n1 += stoi(octave_node->value())*12;
	//note 2
	note_node = note_node->next_sibling();
	  value_node = note_node->first_node("valeur");
	  octave_node = note_node->first_node("octave");
	  n2 = stoi(value_node->value());
	  n2 += stoi(octave_node->value())*12;
	//distance
	xml_node<> * dist_node = couple_node->first_node("distance");
	d = stoi(dist_node->value());
	// probabilite
	xml_node<> * prob_node = couple_node->first_node("probabilite");
	o = stof(prob_node->value());
	
	m_obj.push_back(o);
	
	tmp.push_back(n1);
	tmp.push_back(n2);
	tmp.push_back(d);
	m_constraints.push_back(make_pair(follow,tmp));
	tmp.clear();
	}
  m_size = m_obj.size();
}
Constraints::~Constraints()
{

}

float Constraints::constraint_mM(vector<int> const & ch,int min, int max)const
{
	float cmpt = 0.0;
	for(auto e: ch)
	{
		if(e<=max && e>=min)
			cmpt++;
	}
	cmpt = (cmpt / ch.size()) * 100.0;
	return cmpt;
}
float Constraints::constraint_rect(vector<int> const & ch,int h, int t)const
{
	int max ,min;
	int e;
	float cmpt = 0.0;
	for(int i = 0;i < ch.size(); i++)
	{
		min = ch[i];
		max = min;
		for(int j = 1; ((j < t) && (j + i < ch.size())); j++)
		{
				e = ch[j + i];
				if(e < min)
				{
					min = e;
				}
				else if(e > max)
				{
					max = e;
				}	
		}
		if(max-min<=h)
		{
			cmpt++;
		}
	}
	
	return (cmpt/(ch.size()))*100.0;
}
float Constraints::constraint_follow(vector<int> const & ch,int note1, int note2, int d)const
{
	float cmpt = 0.0;
	int nb1 = 0;
	for(int i = 0; i < ch.size(); i++)
	{
		if(ch[i]==note1){
			nb1++;
			
			if(i+d<ch.size())
			{
			  if(ch[i+d])
				cmpt++;
			}			
		}
	}
	if(nb1!=0)
	{
	  cmpt = (cmpt / nb1)*100.0;
	}
	else
	{
	  cmpt = 0;
	}
	return cmpt;
}
double Constraints::valuation(const vector< int >& ch)const
{
 int n1 ,n2 ,d, h, t, min, max;
 long double val;
 float res, o;
 val =0.0;
  for(int i = 0;  i < m_size; i++)
  {
	switch(m_constraints[i].first)
	{
	  case mM : 
		  min = m_constraints[i].second[0];
		  max = m_constraints[i].second[1];
		  res = constraint_mM(ch, min, max);
		  
		  break;
	  
	  case rect : 
		h = m_constraints[i].second[0];
		t = m_constraints[i].second[1];
		res = constraint_rect(ch, h, t);
		break;
		
	  case follow : 
		n1 = m_constraints[i].second[0];
		n2 = m_constraints[i].second[1];
		d = m_constraints[i].second[2];
		res = constraint_follow(ch, n1, n2, d);
		break;
	}
	o = m_obj[i];
	val += o*res;
	
	
  }
  
  return val;
}
vector< float > Constraints::getConstraint(const vector< int >& ch) const
{
 int n1 ,n2 ,d, o, h, t, min, max;
 vector<float> val;
 float res;
  for(int i = 0;  i < m_size; i++)
  {
	switch(m_constraints[i].first)
	{
	  case mM : 
		  min = m_constraints[i].second[0];
		  max = m_constraints[i].second[1];
		  res = constraint_mM(ch, min, max);
		  break;
	  
	  case rect : 
		h = m_constraints[i].second[0];
		t = m_constraints[i].second[1];
		res = constraint_rect(ch, h, t);
		break;
		
	  case follow : 
		n1 = m_constraints[i].second[0];
		n2 = m_constraints[i].second[1];
		d = m_constraints[i].second[2];
		res = constraint_follow(ch, n1, n2, d);
		break;
	}
	val.push_back(o*res);

  }
  
  return val;

}
