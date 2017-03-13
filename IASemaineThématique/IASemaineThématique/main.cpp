#include <iostream>
#include <vector>
#include <sstream>

#include <cassert>

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




class Board
{
	public:
		enum Type
		{
			None,
			Vertical,
			Horizontal
		};


	private:
		size_t m_width;
		size_t m_height;

		Type *m_data;

		mutable std::vector<size_t> m_cacheH;
		mutable std::vector<size_t> m_cacheV;

	public:

		Board() : m_width(0), m_height(0), m_data(nullptr) {}

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

		Board(const Board &b) : Board(b.m_width, b.m_height, b.m_data) {}


		Board &operator=(const Board &b)
		{
			if (m_data)
				delete[] m_data;

			m_width = b.m_width;
			m_height = b.m_height;
			
			m_data = new Type[m_width * m_height];

			for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
				m_data[i] = b.m_data[i];

			return *this;
		}

		~Board()
		{
			delete[] m_data;
		}

		const std::vector<size_t> &getPossibles(Type type) const
		{
			if (type == Horizontal)
				return getPossiblesHorizontal();

			if (type == Vertical)
				return getPossiblesVertical();

			return std::vector<size_t>();
		}



		bool play(Type type, size_t x, size_t y)
		{
			return play(type, getPos(std::make_pair(x, y)));
		}

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


		std::pair<size_t, size_t> getCoordinate(size_t pos) const
		{
			size_t x = pos % m_width, y = pos / m_width;

			return std::make_pair(x, y);
		}



		size_t getPos(std::pair<size_t, size_t> p) const
		{
			size_t pos = p.first + p.second * m_width;

			return pos;
		}


		std::string toString() const
		{
			ostringstream oss;
			for (size_t i = 0; i < m_height; ++i)
			{
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


std::vector<Move> minimax()
{
	
}



int main(int argc, char *argv[])
{
	Board b(8, 8);
	Board::Type t(Board::Horizontal);
	while(!b.getPossibles(t).empty())
	{
		cout << b.toString() << endl;
		auto m = getMax(b, t);
		b.play(t, m.pos);
		t = (t == Board::Horizontal ? Board::Vertical : Board::Horizontal);
	}

	cout << b.toString();

	return 0;
}