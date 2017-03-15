#include "utils.h"
#include <ostream>
#include <sstream>

Board::Board(size_t w, size_t h, unsigned char *data) : m_width(w), m_height(h)
{
	m_data = new unsigned char[m_width * m_height];

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

Board::Board(const Board &b) : Board(b.m_width, b.m_height, b.m_data) { m_cacheH = b.m_cacheH; m_cacheV = b.m_cacheV; }


Board &Board::operator=(const Board &b)
{
	if (m_data)
		delete[] m_data;

	m_width = b.m_width;
	m_height = b.m_height;

	m_data = new unsigned char[m_width * m_height];

	for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
		m_data[i] = b.m_data[i];

	m_cacheH = b.m_cacheH;
	m_cacheV = b.m_cacheV;

	return *this;
}

Board::~Board()
{
	delete[] m_data;
}

const std::vector<size_t> &Board::getPossibles(Type type) const
{
	if (type == Horizontal)
		return getPossiblesHorizontal();


	return getPossiblesVertical();
}



bool Board::play(Type type, size_t x, size_t y)
{
	return play(type, getPos(std::make_pair(x, y)));
}

bool Board::undo(Type type, size_t x, size_t y)
{
	return undo(type, getPos(std::make_pair(x, y)));
}

bool Board::play(Type type, size_t pos)
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

bool Board::undo(Type type, size_t pos)
{
	assert(type != None);
	size_t cell1 = pos, cell2;
	cell2 = cell1 + (type == Horizontal ? 1 : m_width);

	if (m_data[cell1] != type || m_data[cell2] != type)
		return false;

	m_cacheH.clear();
	m_cacheV.clear();

	m_data[cell1] = m_data[cell2] = None;
	return true;
}


std::pair<size_t, size_t> Board::getCoordinate(size_t pos) const
{
	size_t x = pos % m_width, y = pos / m_width;

	return std::make_pair(x, y);
}



size_t Board::getPos(std::pair<size_t, size_t> p) const
{
	size_t pos = p.first + p.second * m_width;

	return pos;
}


std::string Board::toString() const
{
	std::ostringstream oss;
	for (size_t i = 0; i < m_height; ++i)
	{
		size_t currentRow = i * m_width;
		for (size_t j = 0; j < m_width; ++j)
		{
			size_t currentCell = currentRow + j;
			oss << (m_data[currentCell] == Horizontal ? 'H' : (m_data[currentCell] == Vertical ? 'V' : '_')) << ' ';
		}
		oss << std::endl;
	}

	return oss.str();
}

void Board::cachePossibles() const
{
	for (size_t i = 0; i < m_height - 1; ++i)
	{
		size_t currentRow = i * m_width;
		for (size_t j = 0; j < m_width - 1; ++j)
		{
			size_t currentCell = currentRow + j;
			if (m_data[currentCell] == m_data[currentCell + 1] && m_data[currentCell] == None)
				m_cacheH.push_back(currentCell);
			if (m_data[currentCell] == m_data[currentCell + m_width] && m_data[currentCell] == None)
				m_cacheV.push_back(currentCell);
		}
	}

	size_t currentRow = (m_height - 1) * m_width;
	for (size_t j = 0; j < m_width - 1; ++j)
	{
		size_t currentCell = currentRow + j;
		if (m_data[currentCell] == m_data[currentCell + 1] && m_data[currentCell] == None)
			m_cacheH.push_back(currentCell);
	}

	for (size_t i = 0; i < m_height - 1; ++i)
	{
		size_t j = m_width - 1;
		size_t currentCell = i * m_width + j;
		if (m_data[currentCell] == m_data[currentCell + m_width] && m_data[currentCell] == None)
			m_cacheV.push_back(currentCell);
	}
}

const std::vector<size_t> &Board::getPossiblesHorizontal() const
{
	if (!m_cacheH.empty())
		return m_cacheH;

	cachePossibles();

	return m_cacheH;
}

const std::vector<size_t> &Board::getPossiblesVertical() const
{
	if (!m_cacheV.empty())
		return m_cacheV;

	cachePossibles();

	return m_cacheV;
}

Move getMax(Board &b, Board::Type type)
{
	auto v = b.getPossibles(type);
	Move maxMove;
	maxMove.type = type;
	maxMove.pos = v[0];
	b.play(type, maxMove.pos);
	maxMove.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();
	b.undo(type, maxMove.pos);

	for (size_t i = 1, imax = v.size(); i < imax; ++i)
	{
		Move current;
		current.type = type;
		current.pos = v[i];

		b.play(type, current.pos);
		current.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();
		b.undo(type, current.pos);
		if (maxMove < current)
			maxMove = current;
	}

	return maxMove;
}


int minimax_getMax(Board &b, Move &maxMove, Board::Type type)
{
	auto v = b.getPossibles(type);
	maxMove.type = type;
	maxMove.pos = v[0];
	b.play(type, maxMove.pos);
	maxMove.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();
	b.undo(type, maxMove.pos);

	for (size_t i = 1, imax = v.size(); i < imax; ++i)
	{
		Move current;
		current.type = type;
		current.pos = v[i];

		b.play(type, current.pos);
		current.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();
		b.undo(type, current.pos);
		if (maxMove < current)
			maxMove = current;
	}

	return maxMove.value;
}


int minimax(Board &b, Move &max_move, Board::Type type, size_t depth)
{
	auto v = b.getPossibles(type);
	int maxValue = -100000000;

	Move max_move1, max_move2, max_move3, max_move4;
	Move max_move5, max_move6, max_move7, max_move8;
	auto lambda = [&v](Move &max_move, Board &b, Board::Type type, size_t depth, size_t from, size_t to)
	{
		int maxValue_lambda = -100000000;
		std::function<int(Board &, Board::Type, size_t)> lambda_inner = [&lambda_inner](Board &b, Board::Type type, size_t depth)
		{
			auto v_inner = b.getPossibles(type);
			int maxValue_inner = -100000000;
			Move max_move_inner;
			for (size_t i = 0, imax = v_inner.size(); i < imax; ++i)
			{
				Move current;
				current.type = type;
				current.pos = v_inner[i];
				b.play(type, current.pos);
				current.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();

				if (depth > 0)
				{
					Move m;
					int tmp = lambda_inner(std::ref(b), (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1);
					if (maxValue_inner < tmp)
					{
						maxValue_inner = tmp;
						max_move_inner = current;
					}
				}
				else if (maxValue_inner < current.value)
				{
					maxValue_inner = current.value;
					max_move_inner = current;
				}

				b.undo(type, current.pos);
			}

			return maxValue_inner;
		};

		for (size_t i = from, imax = to; i < imax; ++i)
		{
			Move current;
			current.type = type;
			current.pos = v[i];
			b.play(type, current.pos);
			current.value = b.getPossibles(type).size() - b.getPossibles(type == Board::Horizontal ? Board::Vertical : Board::Horizontal).size();

			if (depth > 0)
			{
				int tmp = lambda_inner(std::ref(b), (type == Board::Horizontal ? Board::Vertical : Board::Horizontal), depth - 1);
				if (maxValue_lambda < tmp)
				{
					maxValue_lambda = tmp;
					max_move = current;
				}
			}
			else if (maxValue_lambda < current.value)
			{
				maxValue_lambda = current.value;
				max_move = current;
			}

			b.undo(type, current.pos);
		}

		return maxValue_lambda;
	};

	if (v.size() > 8)
	{
		size_t nb = v.size() / 8;

		std::future<int> future1 = std::async(std::launch::async, lambda, std::ref(max_move1), b, type, depth, 0, nb);
		std::future<int> future2 = std::async(std::launch::async, lambda, std::ref(max_move2), b, type, depth, nb, 2 * nb);
		std::future<int> future3 = std::async(std::launch::async, lambda, std::ref(max_move3), b, type, depth, 2 * nb, 3 * nb);
		std::future<int> future4 = std::async(std::launch::async, lambda, std::ref(max_move4), b, type, depth, 3 * nb, 4 * nb);
		std::future<int> future5 = std::async(std::launch::async, lambda, std::ref(max_move5), b, type, depth, 4 * nb, 5 * nb);
		std::future<int> future6 = std::async(std::launch::async, lambda, std::ref(max_move6), b, type, depth, 5 * nb, 6 * nb);
		std::future<int> future7 = std::async(std::launch::async, lambda, std::ref(max_move7), b, type, depth, 6 * nb, 7 * nb);
		std::future<int> future8 = std::async(std::launch::async, lambda, std::ref(max_move8), b, type, depth, 7 * nb, v.size());


		future1.wait();
		future2.wait();
		future3.wait();
		future4.wait();

		future5.wait();
		future6.wait();
		future7.wait();
		future8.wait();


		int ret = 8;
		int ret1 = future1.get(), ret2 = future2.get(), ret3 = future3.get(), ret4 = future4.get();
		int ret5 = future5.get(), ret6 = future6.get(), ret7 = future7.get(), ret8 = future8.get();

		if (ret1 > ret2 && ret1 > ret3 && ret1 > ret4 && ret1 > ret5 && ret1 > ret6 && ret1 > ret7 && ret1 > ret8)
			ret = 1;
		else if (ret2 > ret3 && ret2 > ret4 && ret2 > ret5 && ret2 > ret6 && ret2 > ret7 && ret2 > ret8)
			ret = 2;
		else if (ret3 > ret4 && ret3 > ret5 && ret3 > ret6 && ret3 > ret7 && ret3 > ret8)
			ret = 3;
		else if (ret4 > ret5 && ret4 > ret6 && ret4 > ret7 && ret4 > ret8)
			ret = 4;
		else if (ret5 > ret6 && ret5 > ret7 && ret5 > ret8)
			ret = 5;
		else if (ret6 > ret7 && ret6 > ret8)
			ret = 6;
		else if (ret7 > ret8)
			ret = 5;

		switch (ret)
		{
		case 1:
			maxValue = ret1;
			max_move = max_move1;
			break;
		case 2:
			maxValue = ret2;
			max_move = max_move2;
			break;
		case 3:
			maxValue = ret3;
			max_move = max_move3;
			break;
		case 4:
			maxValue = ret4;
			max_move = max_move4;
			break;
		case 5:
			maxValue = ret5;
			max_move = max_move5;
			break;
		case 6:
			maxValue = ret6;
			max_move = max_move6;
			break;
		case 7:
			maxValue = ret7;
			max_move = max_move7;
			break;
		default:
			maxValue = ret8;
			max_move = max_move8;
			break;
		}
	}
	else
		maxValue = lambda(std::ref(max_move), b, type, depth, 0, v.size());
	return maxValue;

}