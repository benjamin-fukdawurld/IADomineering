#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <ctime>

using namespace std;

/*bool is_over(unsigned char *pile)
{
	return (pile[0] | pile[1] | pile[2] | pile[3]) == 0;
}



unsigned char XOR(unsigned char *pile)
{
	return pile[0] ^ pile[1] ^ pile[2] ^ pile[3];
}




void AI(unsigned char *pile)
{
	unsigned char to_remove = XOR(pile);

	bool found = false;
	int i = -1;
	while(!found && i < 3)
	{
		++i;
		found = (pile[i] >= to_remove);
	}

	if (found)
	{
		pile[i] -= to_remove;
		return;
	}

	to_remove = std::log2(to_remove + 1);
	found = false;
	i = 0;
	while (!found && i < 4)
	{
		found = (pile[i] > to_remove);
		++i;
	}

	if (found)
	{
		pile[i - 1] -= to_remove;
		return;
	}

	assert(found);
}




int main(int argc, char *argv[])
{
	unsigned char pile[] = { 1, 3, 5, 7 };
	size_t nb_tas, nb_a_retirer;

	do
	{
		for (size_t i = 0; i < 4u; ++i)
			wcout << static_cast<int>(pile[i]) << ' ';

		wcout << endl;


		wcout << "entrez le numéro du tas: ";
		wcin >> nb_tas;
		wcout << "entrez le nombre d'éléments à retirer: ";
		wcin >> nb_a_retirer;

		pile[nb_tas] -= nb_a_retirer;

		assert(!is_over(pile));
		AI(pile);
	} while (!is_over(pile));

	return 0;
}*/

// Classe pour le plateau de jeu
class Board
{
	public:
		// Type de joueur
		enum Type
		{
			None,
			Vertical,
			Horizontal
		};


	private:
		// Dimmension en x et en y
		size_t m_width;
		size_t m_height;

		// Tableau de données
		Type *m_data;

		mutable std::vector<size_t> m_cacheH;
		mutable std::vector<size_t> m_cacheV;

	public:

		// Constructeur par défaut
		Board() : m_width(0), m_height(0), m_data(nullptr) {}

		// Constructeur avec des dimensions
		Board(size_t w, size_t h, Type *data = nullptr) : m_width(w), m_height(h)
		{
			m_data = new Type[m_width * m_height];

			if (data == nullptr)
			{
				for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
					m_data[i] = None;
			}
			else
			{
				for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
					m_data[i] = data[i];
			}

		}

		// Constructeur par copie
		Board(const Board &b) : Board(b.m_width, b.m_height, b.m_data) { m_cacheH = b.m_cacheH; m_cacheV = b.m_cacheV; }

		// Copie
		Board &operator=(const Board &b)
		{
			if (m_data)
				delete[] m_data;

			m_width = b.m_width;
			m_height = b.m_height;
			
			m_data = new Type[m_width * m_height];

			for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
				m_data[i] = b.m_data[i];

			m_cacheH = b.m_cacheH;
			m_cacheV = b.m_cacheV;

			return *this;
		}

		~Board()
		{
			delete[] m_data;
		}

		// list des coups possibles
		const std::vector<size_t> &getPossibles(Type type) const
		{
			if (type == Horizontal)
				return getPossiblesHorizontal();


			return getPossiblesVertical();
		}


		// Joue un coup à un emplacement
		bool play(Type type, size_t x, size_t y)
		{
			return play(type, getPos(std::make_pair(x, y)));
		}

		// Joue sur le tableua à une dimension
		bool play(Type type, size_t pos)
		{
			assert(type != None);
			size_t cell1 = pos, cell2;
			cell2 = cell1 + (type == Horizontal ? 1 : m_width);

			if (m_data[cell1] != None || m_data[cell2] != None)
				return false;

			m_cacheH.clear();
			m_cacheV.clear();

			m_data[cell1] = m_data[cell2] = type;
			return true;
		}

		// Récupère les coordonées à partir d'une position
		std::pair<size_t, size_t> getCoordinate(size_t pos) const
		{
			size_t x = pos % m_width, y = pos / m_width;

			return std::make_pair(x, y);
		}

		// Récupère la position à partir d'une coordonnée
		size_t getPos(std::pair<size_t, size_t> p) const
		{
			size_t pos = p.first + p.second * m_width;

			return pos;
		}

		// Affiche le plateau de jeu
		std::string toString() const
		{
			ostringstream oss;
			oss << "  ";
			for (size_t j = 0; j < m_width; ++j)
				oss << j << " ";
			oss << endl;
			for (size_t i = 0; i < m_height; ++i)
			{
				oss << i << " ";
				size_t currentRow = i * m_width;
				for (size_t j = 0; j < m_width; ++j)
				{
					size_t currentCell = currentRow + j;
					oss << (m_data[currentCell] == Horizontal ? 'H' : (m_data[currentCell] == Vertical ? 'V' : '_')) << ' ';
				}
				oss << endl;
			}

			return oss.str();
		}


	private:
		// Donne les coup possibles pour le jouer horizontal
		const std::vector<size_t> &getPossiblesHorizontal() const
		{
			if (!m_cacheH.empty())
				return m_cacheH;

			std::vector<size_t> ret;
			for (size_t i = 0; i < m_height; ++i)
			{
				size_t currentRow = i * m_width;
				for (size_t j = 0; j < m_width - 1; ++j)
				{
					size_t currentCell = currentRow + j;
					if (m_data[currentCell] == m_data[currentCell + 1] && m_data[currentCell] == None)
						m_cacheH.push_back(currentCell);
				}
			}

			return m_cacheH;
		}

		// Donne les coup possibles pour le jouer vertical
		const std::vector<size_t> &getPossiblesVertical() const
		{
			if (!m_cacheV.empty())
				return m_cacheV;

			for (size_t i = 0; i < m_height - 1; ++i)
			{
				size_t currentRow = i * m_width;
				for (size_t j = 0; j < m_width; ++j)
				{
					size_t currentCell = currentRow + j;
					if (m_data[currentCell] == m_data[currentCell + m_width] && m_data[currentCell] == None)
						m_cacheV.push_back(currentCell);
				}
			}

			return m_cacheV;
		}
};


struct Move
{
	Board::Type type;
	size_t pos;
	Board board;

	// Fonction d'évaluation
	int value() const
	{
		int tmp = board.getPossibles(type).size();
		tmp -= board.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();

		return tmp;
	}

	bool operator<(const Move &m) const
	{
		return value() < m.value();
	}
};


// Récupère la coup de coût 
Move getMax(const Board &b, Board::Type type)
{
	auto v = b.getPossibles(type);
	Move maxMove;
	maxMove.board = b;
	maxMove.type = type;
	maxMove.pos = v[0];
	maxMove.board.play(type, maxMove.pos);

	for (size_t i = 1, imax = v.size(); i < imax; ++i)
	{
		Move current;
		current.board = b;
		current.type = type;
		current.pos = v[i];

		current.board.play(type, current.pos);

		if (maxMove < current)
			maxMove = current;
	}

	return maxMove;
}


struct MoveNode
{
	Move move;
	int value;
	vector<MoveNode> children;

	bool operator<(const MoveNode &node) const { return value < node.value; }
};

int max(const Board &b, vector<MoveNode> &tree, Board::Type type, int depth, int alpha, int beta);
int min(const Board &b, vector<MoveNode> &tree, Board::Type type, int depth, int alpha, int beta);

vector<int> killer(3, -1);
vector<int> history;
clock_t beginT;

bool sortWithHistory(size_t i, size_t j) { return (history[i]<history[j]); }

int max(const Board &b, vector<MoveNode> &tree, Board::Type type, int depth, int alpha, int beta) {
	auto v = b.getPossibles(type);

	int maxValue = -100000000;

	/*if (double(clock() - beginT) / CLOCKS_PER_SEC > 3){
		return -1;
	}*/
	
	if (depth == 0) {
		return v.size();
	}

	if(killer[depth-1] != -1){
		v.insert(v.begin(), killer[depth-1]);
	}

	if (history.size() > 0) {
		std::sort(v.begin(), v.end(), sortWithHistory);
	}

	for (int m=0; m < v.size(); m++) {
		MoveNode current;
		current.move.board = b;
		current.move.type = type;
		current.move.pos = v[m];

		current.move.board.play(type, current.move.pos);
		int e = min(current.move.board, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1, alpha, beta);

		if (e > alpha) {
			alpha = e;
			if (alpha >= beta) {
				killer[depth-1] = v[m];
				history[v[m]] += pow(depth, 4);
				return beta;
			}
		}

		tree.push_back(current);
	}
	return alpha;
}

int min(const Board &b, vector<MoveNode> &tree, Board::Type type, int depth, int alpha, int beta) {
	auto v = b.getPossibles(type);

	int maxValue = -100000000;


	if (depth == 0) {
		return v.size();
	}

	for (int m=0; m < v.size(); m++) {
		MoveNode current;
		current.move.board = b;
		current.move.type = type;
		current.move.pos = v[m];

		current.move.board.play(type, current.move.pos);
		int e = max(current.move.board, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1, alpha, beta);

		if (e < beta) {
			beta = e;
			if (alpha >= beta) {
				return alpha;
			}
		}
	}
	return beta;
}

int minimax(const Board &b, vector<MoveNode> &tree, Board::Type type, size_t depth)
{
	auto v = b.getPossibles(type);

	int maxValue = -100000000;

	for (size_t i = 0, imax = v.size(); i < imax; ++i)
	{
		MoveNode current;
		current.move.board = b;
		current.move.type = type;
		current.move.pos = v[i];

		current.move.board.play(type, current.move.pos);
		
		if(depth > 0)
		maxValue = std::max(minimax(current.move.board, current.children, (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1), maxValue);
		else
		maxValue = std::max(current.move.value(), maxValue);

		tree.push_back(current);
	}

	return maxValue;

}



int main(int argc, char *argv[])
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
			vector<MoveNode> tree;
			//minimax(b, tree, t, 2);
			//std::fill(killer.begin(), killer.begin() + 2, -1);
			killer.clear();
			std::fill_n(std::back_inserter(killer), 2, -1);
			beginT = clock();
			//max(b, tree, t, 2, +10000000, -1000000);
			vector<MoveNode> treetmp;
			//history = vector<int>(b.getPossibles.size(), 0);
			history.clear();
			std::fill_n(std::back_inserter(history), 8*8, 0);
			for (size_t i = 0; i < 3; i++)
			{
				if (max(b, tree, t, i, -10000000, 1000000) != -1) {
					treetmp = tree;
				}
			}
			//if(tree.size()>0)
				m = treetmp[0].move;
			for (int i = 1; i < treetmp.size(); ++i)
			{
				if (m.value() < treetmp[i].move.value())
					m = treetmp[i].move;
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