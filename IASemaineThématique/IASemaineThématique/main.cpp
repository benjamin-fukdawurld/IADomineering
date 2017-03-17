#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <ctime>
#include "utils.h"

using namespace std;

// Classe pour le plateau de jeu

		// Type de joueur




		// Dimmension en x et en y


		// Tableau de données


		// Constructeur par défaut


		// Constructeur avec des dimensions
		
		// Constructeur par copie


		// Copie
		
		// list des coups possibles

		


		// Joue un coup à un emplacement
		
		// Joue sur le tableua à une dimension
		

		// Récupère les coordonées à partir d'une position
		

		// Récupère la position à partir d'une coordonnée
		

		// Affiche le plateau de jeu
	
		// Donne les coup possibles pour le jouer horizontal
		
		// Donne les coup possibles pour le jouer vertical
		




	// Fonction d'évaluation
	


// Récupère la coup de coût 


int max(Board &b, vector<Move> &tree, Board::Type type, int depth, int alpha, int beta);
int min(Board &b, vector<Move> &tree, Board::Type type, int depth, int alpha, int beta);

vector<int> killer(3, -1);
vector<int> history;
clock_t beginT;

bool sortWithHistory(size_t i, size_t j) { return (history[i]>history[j]); }

int max(Board &b, vector<Move> &tree, Board::Type type, int depth, int alpha, int beta) {
	auto v = b.getPossibles(type);

	int maxValue = -100000000;

	/*if (double(clock() - beginT) / CLOCKS_PER_SEC > 3){
		return -1;
	}*/

	if (depth == 0) {
		return v.size();
	}

	if (killer[depth - 1] != -1) {
		v.insert(v.begin(), killer[depth - 1]);
	}

	if (history.size() > 0) {
		std::sort(v.begin(), v.end(), sortWithHistory);
	}

	for (int m = 0; m < v.size(); m++) {
		Move current;

		current.type = type;
		current.pos = v[m];

		b.play(type, current.pos);
		Move tmp;
		int e = min(b, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1, alpha, beta);

		b.undo(type, current.pos);

		if (e > alpha) {
			alpha = e;
			if (alpha >= beta) {
				killer[depth - 1] = v[m];
				history[v[m]] += pow(depth, 4);
				return beta;
			}
			//tree = current;
		}
		tree.push_back(current);
	}
	return alpha;
}

int min(Board &b, vector<Move> &tree, Board::Type type, int depth, int alpha, int beta) {
	auto v = b.getPossibles(type);

	int maxValue = -100000000;


	if (depth == 0) {
		return v.size();
	}

	for (int m=0; m < v.size(); m++) {
		Move current;
		current.type = type;
		current.pos = v[m];

		b.play(type, current.pos);
		Move tmp;
		int e = max(b, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1, alpha, beta);

		b.undo(type, current.pos);

		if (e < beta) {
			beta = e;
			if (alpha >= beta) {
				return alpha;
			}
			//tree = current;
		}
	}
	return beta;
}

int minimax(Board &b, vector<Move> &tree, Board::Type type, size_t depth)
{
	auto v = b.getPossibles(type);

	int maxValue = -100000000;

	for (size_t i = 0, imax = v.size(); i < imax; ++i)
	{
		Move current;
		current.type = type;
		current.pos = v[i];

		b.play(type, current.pos);
		
		if(depth > 0)
		maxValue = std::max(minimax(b, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1), maxValue);
		else
		maxValue = std::max(current.valueE(b), maxValue);
		//(int)(b.getPossibles(type).size() - b.getPossibles(Board::inverse(type)).size())
		tree.push_back(current);
	}

	return maxValue;

}



int _main(int argc, char *argv[])
{
	// Initialisation des datas
	Board b(8, 8);
	Board::Type t(Board::Horizontal);
	int inputPlayerW, inputPlayerH;
	// Tant qu'il y a un coup possible
	while(!b.getPossibles(t).empty())
	{
		cout << b.toString() << endl;

		/*if (t == Board::Horizontal) {
			cout << "Horizontal Player: Colomn ? ";
			cin >> inputPlayerW; 
			cout << "Line ? ";
			cin >> inputPlayerH;
			std::pair<int, int> p;
			p.first = inputPlayerW; p.second = inputPlayerH;
			b.play(t, b.getPos(p));
		}
		else
		{
			auto m = getMax(b, t);
			b.play(t, m.pos);
		}*/

		Move m;
		if(t == Board::Vertical)
		{
			vector<Move> tree;
			//minimax(b, tree, t, 2);
			//std::fill(killer.begin(), killer.begin() + 2, -1);
			killer.clear();
			std::fill_n(std::back_inserter(killer), 3, -1);
			beginT = clock();
			//max(b, tree, t, 2, +10000000, -1000000);
			vector<Move> treetmp;
			//history = vector<int>(b.getPossibles.size(), 0);
			history.clear();
			std::fill_n(std::back_inserter(history), 8*8, 0);
			max(b, tree, t, 3, -10000000, 1000000);
			treetmp = tree;
			// Pour l'incrément
			/*for (size_t i = 0; i < 3; i++)
			{
				if (max(b, tree, t, i, -10000000, 1000000) != -1) {
					treetmp = tree;
				}
			}*/
			m = treetmp[0];
			for (int i = 1; i < treetmp.size(); ++i)
			{
				if (m.valueE(b) < treetmp[i].valueE(b));
					m = treetmp[i];
			}
		}
		else {
				cout << "Horizontal Player: Colomn ? ";
				cin >> inputPlayerW;
				cout << "Line ? ";
				cin >> inputPlayerH;
				std::pair<int, int> p;
				p.first = inputPlayerW; p.second = inputPlayerH;
				m.pos = b.getPos(p);
			}
		//m = getMax(b, t);

		b.play(t, m.pos);
		t = (t == Board::Horizontal ? Board::Vertical : Board::Horizontal);
	}

	cout << b.toString();

	cout << endl;

	return 0;
}


int main(int argc, char *argv[])
{
	return run(argc, argv);
}