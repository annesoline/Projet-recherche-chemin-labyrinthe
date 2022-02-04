/** \author Anne-Soline GUILBERT--LY
 *  \date	dec 2019
 */

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <algorithm>

#define MAX_ERROR 0.01
#define MIN_DELTA_P 0.000001

using namespace std;


constexpr int indice_premiere_ligne(0);
constexpr double minimum(0.);
constexpr double maximum(1.);

typedef vector<vector<bool>> Grille;

struct Graphe {
	vector<double> p;
	vector<double> pprime;	
};


void initialisation_libre(Grille& libre, const int& n);

void construire_passage(Grille& passage,const Grille& libre,
						const int& n);

void chercher_chemin(Grille& passage, const Grille& libre, const int& n,
					 int i, int j);

void affiche_terrain(const Grille& terrain, const int& n, 
					 const string& format);

void init_libre_hasard(Grille& libre, const int& n,
					   default_random_engine& e,
					   bernoulli_distribution& b);

void construire_un_seul_passage(Grille& passage, const Grille& libre,
								const int& n);

bool terrain_traversable(const Grille& terrain, const int& n);

double calcul_pprime(Grille& libre, const int& n, Grille& passage, 
					 default_random_engine& e, const double& p, 
					 const double& nbterrains);

void init_passage_a_false(Grille& passage, const int& n);

void dichotomie(double& min,double& max,double& error,double& maxprime,
				double& minprime, double& p, double& pprime, 
				const int& n,Grille& libre, Grille& passage,
				default_random_engine& e, const double& nbterrains,
				Graphe& courbe);

void afficher_p_et_pprime(const Graphe& courbe);

void graphe_p(Grille& libre,const int& n,Grille& passage, 
			  default_random_engine& e, double& p,
			  const double& nbterrains);



int main(){
	
	default_random_engine e(time(0));
	
	char niveau; cin >>niveau;
	int n; cin >> n;
	
	Grille libre(n, vector<bool>(n));
	Grille passage(n, vector<bool>(n));

	switch (niveau){
		case 'a':{
			string format("P1");
			initialisation_libre(libre, n);
			construire_passage(passage, libre, n);
			affiche_terrain(passage, n, format);
			return 0;
		}
		case 'b' : {
			double p; cin >> p;
			int nbt; cin >>nbt;
			double nbterrains(nbt);
			
			cout <<setprecision(6)<<fixed;
			cout<<calcul_pprime(libre, n, passage, e, p, nbterrains);
			return 0;
		}
		case 'c' : {
			int nbt; cin >>nbt;
			double nbterrains(nbt), min(minimum), max(maximum), 
			maxprime(maximum), minprime(minimum), pprime, error, p;
			
			Graphe courbe={{minimum, maximum}, {minimum, maximum}};
			
			dichotomie(min, max, error, maxprime, minprime, p, pprime, 
			n, libre, passage, e, nbterrains, courbe);
			return 0;
		}
	}
}


void initialisation_libre(Grille& libre, const int& n){
	
	int valeur;
	
	for(int i(0); i<n; ++i){
		for(int j(0); j<n; ++j){
			cin >> valeur;
			if(valeur == 0){
				libre[i][j] = true;
			}
		}
	}
}

void construire_passage(Grille& passage,const Grille& libre,
const int& n){
	
	int i;
	
	for(int j(0); j<n; ++j){
		i=indice_premiere_ligne;
		if(libre[i][j]){
			passage[i][j]=true;
			chercher_chemin(passage, libre, n, i, j);
		}
	}
}

void chercher_chemin(Grille& passage, const Grille& libre, const int& n, 
int i, int j){
	
	if((i+1)<=(n-1) and libre[i+1][j] and !passage[i+1][j]){
		passage[i+1][j]=true;
		chercher_chemin(passage, libre, n, (i+1), j);				
	}
	
	if((i-1)>=0 and libre[i-1][j] and !passage[i-1][j]){
		passage[i-1][j]=true;
		chercher_chemin(passage, libre, n, (i-1), j);				
	}
	
	if((j-1)>=0 and libre[i][j-1] and !passage[i][j-1]){
		passage[i][j-1]=true;
		chercher_chemin(passage, libre, n, i, (j-1));				
	}
	
	if((j+1)<=(n-1) and libre[i][j+1] and !passage[i][j+1]){
		passage[i][j+1]=true;
		chercher_chemin(passage, libre, n, i, (j+1));				
	}	
}

void affiche_terrain(const Grille& terrain, const int& n, 
const string& format){
	
	cout << format <<endl;
	cout << n <<" "<<n<<endl;
	
	for(int i(0); i<n; ++i){
		for(int j(0); j<n; ++j){
			if(terrain[i][j]){
				cout <<"0 ";
			}else{
				cout <<"1 ";
			}
			if(j==34){
				cout<<endl;
			}
		}
		cout<<endl;
	}
}

void init_libre_hasard(Grille& libre,const int& n,
default_random_engine& e, bernoulli_distribution& b){
	
	for(int i(0); i<n; ++i){
		for(int j(0); j<n; ++j){
			if(b(e) == 1){
				libre[i][j] = true;
			}else {
				libre[i][j] = false;
			}
		}
	}
}

void construire_un_seul_passage(Grille& passage, const Grille& libre,
const int& n){
	
	int i;
	
	for(int j(0); j<n; ++j){
		i=indice_premiere_ligne;
		if(libre[i][j]){
			passage[i][j]=true;
			chercher_chemin(passage, libre, n, i, j);
		}
		if(terrain_traversable(passage, n)){
			break;
		}
	}
}

bool terrain_traversable(const Grille& terrain, const int& n){
	
	for(int j(0); j<n; ++j){
		if(terrain[n-1][j]){
			return true;
		}
	}
	return false;
}

double calcul_pprime(Grille& libre, const int& n, Grille& passage, 
default_random_engine& e, const double& p, const double& nbterrains){
	
	bernoulli_distribution b(p);
	int compteur(0);
	
	for (double k(1); k<=nbterrains; ++k){	
		init_libre_hasard(libre, n, e, b);
		construire_un_seul_passage(passage, libre, n);
		if(terrain_traversable(passage, n)){
			++compteur; 
		}
		init_passage_a_false(passage, n);
	}
	
	return (compteur/nbterrains);
}

void init_passage_a_false(Grille& terrain, const int& n){
	Grille tmp(n, vector<bool>(n));
	terrain = tmp;
}

void dichotomie(double& min, double& max,double& error,double& maxprime,
double& minprime, double& p, double& pprime, const int& n,Grille& libre,
Grille& passage, default_random_engine& e, const double& nbterrains,
Graphe& courbe){
	
	do{
		p = (max+min)/2.;
		pprime = calcul_pprime(libre, n, passage, e, p, nbterrains);
		error = pprime - (maxprime + minprime)/2.;
		
		courbe.p.push_back(p);
		courbe.pprime.push_back(pprime);
		
		if(error>0.){
			max = p;
			maxprime = calcul_pprime(libre,n,passage,e,max,nbterrains);
		}
		if(error<0.){
			min = p;
			minprime = calcul_pprime(libre,n,passage,e,min,nbterrains);
		}
		
	}while((max-min)>MIN_DELTA_P and abs(error)>MAX_ERROR);
	
	sort(courbe.p.begin(), courbe.p.end());
	sort(courbe.pprime.begin(), courbe.pprime.end());
	afficher_p_et_pprime(courbe);
}

void afficher_p_et_pprime(const Graphe& courbe){
	
	for(unsigned int i(0); i<courbe.p.size(); ++i){
		cout <<setprecision(6)<<fixed;
		cout<<courbe.p[i]<<" "<<courbe.pprime[i]<<endl;
	}
}

