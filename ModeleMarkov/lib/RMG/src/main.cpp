#include <iostream>
#include <time.h>
#include<stdlib.h>
#include "../include/mgenerator.h"
#include "../include/chromosome.h"
using namespace std;
int main(int argc, char **argv) {
  string in ,out = "";
  bool ar, ls, ts;
  ls = false;
  ts = false;
  int nbnotes, nbgeneration = 0;
    srand (time(NULL));
	for(int i = 1 ; i < argc; i++)
	{
	  string tmp = argv[i];
	  // fichier de sortie
	  if(tmp.compare("-o")==0)
	  {
		if(++i < argc)
		{
		  out = argv[i];	
		}
		else
		{
		cerr << " -o fichier de sortie" << endl;
		return 1;
		}		
	  }
	  else if(tmp.compare("-c")==0)
		   {
			  if(++i < argc)
			  {
				in = argv[i];
			  }
			  else
			  {
				cerr << " -c fichier de contrainte" << endl;
				return 1;
			  }
			}
			else if(tmp.compare("-n")==0)
				  {
					if(++i < argc)
					{
					  nbnotes = stoi(argv[i]);
					}
					else
					{
					  cerr << " -n nombre de notes à générer" << endl;
					  return 1;
					}
				  }
				  else if((tmp.compare("-g")==0))
						{
						  if(++i < argc)
						  {
							nbgeneration =  stoi(argv[i]);
						  }
						  else
						  {
							cerr << " -g nombre d'itteration" << endl;
							return 1;
						  }
						}
						else if(tmp.compare("-ls")== 0)
							  {
								ls = true;
							  }
							  else if(tmp.compare("-ts")== 0)
									{
									  ts = true;
									}
	} 
	
	if(nbnotes == 0|| nbgeneration == 0 || in == "" || out == "")
	{
	  cerr << "" << endl;
	}
	cout  << nbnotes << " " << nbgeneration << " " << in << " " << out << endl;
	MGenerator m(nbnotes, in, out, ls, ts);
    m.generation(nbgeneration);
	m.out(-1);
	return 0;
}