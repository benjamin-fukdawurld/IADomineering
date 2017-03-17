#include "utils.h"
#include <ostream>
#include <sstream>
#include <random>
#include <cassert>
#include <iostream>
#include <ctime>
#include <iterator>

using namespace std;

vector<int> killer(3, -1);
vector<int> history;
clock_t beginT;

Board::Board(size_t w, size_t h, unsigned char *data, size_t *hashmap, const size_t &hash) : m_width(w), m_height(h), m_hash(hash)
{
	// Seed with a real random value, if available
	std::random_device r;

	// Choose a random mean between 1 and 6
	std::default_random_engine e1(r());
	std::uniform_int_distribution<size_t> uniform_dist(1, static_cast<size_t>(pow(2, 8 * sizeof(size_t)) - 1));
	
	
	m_data = new unsigned char[m_width * m_height];
	m_hashmap = new size_t[m_width * m_height];

	if (data == nullptr)
	{
		for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
		{
			m_data[i] = None;
			m_hashmap[i] = uniform_dist(e1);
		}
	}
	else
	{
		for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
		{
			m_data[i] = data[i];
			m_hashmap[i] = hashmap[i];
		}
	}

}

Board::Board(const Board &b) : Board(b.m_width, b.m_height, b.m_data, b.m_hashmap, b.m_hash) { m_cacheH = b.m_cacheH; m_cacheV = b.m_cacheV; }


Board &Board::operator=(const Board &b)
{
	if (m_data)
		delete[] m_data;

	m_width = b.m_width;
	m_height = b.m_height;

	m_data = new unsigned char[m_width * m_height];

	for (size_t i = 0, imax = m_width * m_height; i < imax; ++i)
	{
		m_data[i] = b.m_data[i];
		m_hashmap[i] = b.m_hashmap[i];
	}

	m_hash = b.m_hash;
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

	m_hash ^= m_hashmap[cell1];
	m_hash ^= m_hashmap[cell2];

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

	m_hash ^= m_hashmap[cell1];
	m_hash ^= m_hashmap[cell2];

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


namespace FDAI
{

	int internal_minimax(Board &b, Move *m, Board::Type t, size_t depth, const std::vector<size_t> &v, size_t from, size_t to)
	{
		int eval = -1000000000;
		for (size_t i = from, imax = to; i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = FDAI::min(b, Board::inverse(t), depth - 1);
			b.undo(t, v[i]);
			if (min_eval > eval)
			{
				eval = min_eval;
				if (m)
					*m = Move(t, v[i], eval);
			}
		}

		return eval;
	}

	int minimax(Board &b, Move *m, Board::Type t, size_t depth)
	{
		if (depth == 0)
		{
			Move tmp;
			int ret = minimax_getMax(b, tmp, t);
			if (m)
				*m = tmp;
			return ret;
		}

		std::vector<size_t> v = b.getPossibles(t);

		if (v.size() < 8)
			return internal_minimax(b, m, t, depth, v, 0, v.size());

		int eval = -1000000000;
		std::vector<Move> v_m(8);
		std::vector<int> v_eval(8, -1000000000);

		{
			std::vector<std::future<int>> fut(8);
			size_t nb = v.size() / 8;
			std::vector<Board> v_b(8, b);
			for (size_t i = 0; i < 8; ++i)
			{
				fut[i] = std::async(std::launch::async, internal_minimax, std::ref(v_b[i]), &(v_m[i]), t, depth, std::ref(v), i * nb, std::min(i + 1 * nb, v.size()));
			}

			for (size_t i = 0; i < 8; ++i)
			{
				fut[i].wait();
				v_eval[i] = fut[i].get();
			}
		}

		size_t pos = 0;
		eval = v_eval[0];
		for (size_t i = 1; i < 8; ++i)
		{
			if (v_eval[i] > eval)
			{
				eval = v_eval[i];
				pos = i;
			}
		}

		if (m)
			*m = v_m[pos];

		return eval;
	}

	int max(Board &b, Board::Type t, size_t depth)
	{
		if (depth == 0)
		{
			return b.evaluate(t);
		}

		int eval = -1000000000;
		std::vector<size_t> v = b.getPossibles(t);
		for (size_t i = 0, imax = v.size(); i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = FDAI::min(b, Board::inverse(t), depth - 1);
			b.undo(t, v[i]);
			if (min_eval > eval)
				eval = min_eval;
		}

		return eval;
	}

	int min(Board &b, Board::Type t, size_t depth)
	{
		if (depth == 0)
		{
			return b.evaluate(Board::inverse(t));
		}

		int eval = 1000000000;
		std::vector<size_t> v = b.getPossibles(t);
		for (size_t i = 0, imax = v.size(); i < imax; ++i)
		{
			b.play(t, v[i]);
			int max_eval = FDAI::max(b, Board::inverse(t), depth - 1);
			b.undo(t, v[i]);
			if (max_eval < eval)
				eval = max_eval;
		}

		return eval;
	}

	int internal_negamax2(Board &b, Board::Type t, size_t depth)
	{
		int eval = -1000000000;
		std::vector<size_t> v = b.getPossibles(t);
		for (size_t i = 0, imax = v.size(); i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = -internal_negamax2(b, Board::inverse(t), depth - 1);
			b.undo(t, v[i]);
			if (min_eval > eval)
			eval = min_eval;
		}

		return eval;
	}

	int internal_negamax(Board &b, Move *m, Board::Type t, size_t depth, const std::vector<size_t> &v, size_t from, size_t to)
	{
		int eval = -1000000000;
		for (size_t i = from, imax = to; i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = -internal_negamax2(b, Board::inverse(t), depth - 1);
			b.undo(t, v[i]);
			if (min_eval > eval)
			{
				eval = min_eval;
				if (m)
					*m = Move(t, v[i], eval);
			}
		}

		return eval;
	}

	int negamax(Board &b, Move *m, Board::Type t, size_t depth)
	{
		if (depth == 0)
		{
			Move tmp;
			int ret = minimax_getMax(b, tmp, t);
			if (m)
				*m = tmp;
			return ret;
		}

		std::vector<size_t> v = b.getPossibles(t);

		if (v.size() < 8)
			return internal_negamax(b, m, t, depth, v, 0, v.size());

		int eval = -1000000000;
		std::vector<Move> v_m(8);
		std::vector<int> v_eval(8, -1000000000);

		{
			std::vector<std::future<int>> fut(8);
			size_t nb = v.size() / 8;
			std::vector<Board> v_b(8, b);
			for (size_t i = 0; i < 8; ++i)
			{
				fut[i] = std::async(std::launch::async, internal_negamax, std::ref(v_b[i]), &(v_m[i]), t, depth, std::ref(v), i * nb, std::min(i + 1 * nb, v.size()));
			}

			for (size_t i = 0; i < 8; ++i)
			{
				fut[i].wait();
				v_eval[i] = fut[i].get();
			}
		}

		size_t pos = 0;
		eval = v_eval[0];
		for (size_t i = 1; i < 8; ++i)
		{
			if (v_eval[i] > eval)
			{
				eval = v_eval[i];
				pos = i;
			}
		}

		if (m)
			*m = v_m[pos];

		return eval;
	}

	bool sortWithHistory(size_t i, size_t j) { return (history[i]>history[j]); }

	int alphabeta_max(Board &b, Board::Type t, size_t depth, int alpha, int beta)
	{
		if (depth == 0)
		{
			return b.evaluate(t);
		}

		std::vector<size_t> v = b.getPossibles(t);

		if (double(clock() - beginT) / CLOCKS_PER_SEC > 3){
			return -1;
		}

		if (killer[depth - 1] != -1) {
			v.insert(v.begin(), killer[depth - 1]);
		}

		if (history.size() > 0) {
			std::sort(v.begin(), v.end(), sortWithHistory);
		}

		for (size_t i = 0, imax = v.size(); i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = alphabeta_min(b, Board::inverse(t), depth - 1, alpha, beta);
			b.undo(t, v[i]);
			if (min_eval > alpha)
			{
				alpha = min_eval;
				if (alpha >= beta)
					killer[depth - 1] = v[i];
					history[v[i]] += pow(4, depth);
					return beta;
			}
		}

		return alpha;
	}


	int alphabeta_min(Board &b, Board::Type t, size_t depth, int alpha, int beta)
	{
		if (depth == 0)
		{
			return b.evaluate(Board::inverse(t));
		}

		std::vector<size_t> v = b.getPossibles(t);
		for (size_t i = 0, imax = v.size(); i < imax; ++i)
		{
			b.play(t, v[i]);
			int max_eval = alphabeta_max(b, Board::inverse(t), depth - 1, alpha, beta);
			b.undo(t, v[i]);
			if (max_eval < beta)
			{
				beta = max_eval;
				if (alpha >= beta)
					return alpha;
			}
		}

		return beta;
	}


	int internal_alphabeta(Board &b, Move *m, Board::Type t, size_t depth, const std::vector<size_t> &v, size_t from, size_t to, int alpha, int beta)
	{
		for (size_t i = from, imax = to; i < imax; ++i)
		{
			b.play(t, v[i]);
			int min_eval = alphabeta_min(b, Board::inverse(t), depth - 1, alpha, beta);
			b.undo(t, v[i]);
			if (min_eval > alpha)
			{
				alpha = min_eval;
				if (m)
					*m = Move(t, v[i], alpha);

				if (alpha >= beta)
					return beta;
			}
		}

		return alpha;
	}


	int alphabeta(Board &b, Move *m, Board::Type t, size_t depth, int alpha, int beta)
	{
		if (depth == 0)
		{
			Move tmp;
			int ret = minimax_getMax(b, tmp, t);
			if (m)
				*m = tmp;
			return ret;
		}

		std::vector<size_t> v = b.getPossibles(t);

		if (v.size() < 8)
			return internal_alphabeta(b, m, t, depth, v, 0, v.size(), alpha, beta);

		int eval = -1000000000;
		std::vector<Move> v_m(8);
		std::vector<int> v_eval(8, -1000000000);
		std::vector<int> v_alpha(8, -1000000000);
		std::vector<int> v_beta(8, -100000000);

		{
			std::vector<std::future<int>> fut(8);
			size_t nb = v.size() / 8;
			std::vector<Board> v_b(8, b);
			for (size_t i = 0; i < 8; ++i)
			{
				fut[i] = std::async(std::launch::async, internal_alphabeta, std::ref(v_b[i]), &(v_m[i]), t, depth, std::ref(v), i * nb, std::min(i + 1 * nb, v.size()), std::ref(v_alpha[i]), std::ref(v_beta[i]));
			}

			for (size_t i = 0; i < 8; ++i)
			{
				fut[i].wait();
				v_eval[i] = fut[i].get();
			}
		}

		size_t pos = 0;
		eval = v_eval[0];
		for (size_t i = 1; i < 8; ++i)
		{
			if (v_eval[i] > eval)
			{
				eval = v_eval[i];
				pos = i;
			}
		}

		if (m)
			*m = v_m[pos];

		return eval;
	}
}

int run(int argc, char *argv[])
{
	// Initialisation des datas
	Board b(8, 8);
	Board::Type t(Board::Horizontal);
	int inputPlayerW, inputPlayerH;

	cout << b.toString() << endl;

	// Tant qu'il y a un coup possible
	while (!b.getPossibles(t).empty())
	{
		Move m;
		Move tmpM;
		if (t == Board::Vertical)
		{
			killer.clear();
			std::fill_n(back_inserter(killer), 3, -1);
			beginT = clock();
			history.clear();
			std::fill_n(std::back_inserter(history), 8 * 8, 0);

			int alpha = -1000000000, beta = 1000000000;
			for (size_t i = 0; i < 3; i++)
			{
				if (FDAI::alphabeta(b, &m, t, i, alpha, beta) != -1) {
					tmpM = m;
				}
			}
		}
		else {
			cout << "Horizontal Player: Colomn ? ";
			cin >> inputPlayerW;
			cout << "Line ? ";
			cin >> inputPlayerH;
			std::pair<int, int> p;
			p.first = inputPlayerW; p.second = inputPlayerH;
			tmpM.pos = b.getPos(p);
		}
			//m = getMax(b, t);

		b.play(t, tmpM.pos);
		cout << (t == Board::Horizontal ? "Horizontal" : "Vertical") << endl;

		cout << b.toString() << endl;
		t = (t == Board::Horizontal ? Board::Vertical : Board::Horizontal);
	}

	cout << b.toString();

	cout << endl;

	return 0;
}