#pragma once


#include <functional>
#include <cassert>
#include <future>


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

		unsigned char *m_data;

		mutable std::vector<size_t> m_cacheH;
		mutable std::vector<size_t> m_cacheV;

	public:

		Board() : m_width(0), m_height(0), m_data(nullptr) {}

		Board(size_t w, size_t h, unsigned char *data = nullptr);

		Board(const Board &b);

		Board &operator=(const Board &b);

		~Board();

		const std::vector<size_t> &getPossibles(Type type) const;

		bool play(Type type, size_t x, size_t y);

		bool undo(Type type, size_t x, size_t y);

		bool play(Type type, size_t pos);

		bool undo(Type type, size_t pos);

		std::pair<size_t, size_t> getCoordinate(size_t pos) const;

		size_t getPos(std::pair<size_t, size_t> p) const;

		int evaluate(Type t) const
		{
			return getPossibles(t).size() - getPossibles(inverse(t)).size();
		}

		std::string toString() const;

		static Type inverse(Type t) { return (t == Horizontal ? Vertical : Horizontal); }


	private:

		void cachePossibles() const;

		const std::vector<size_t> &getPossiblesHorizontal() const;

		const std::vector<size_t> &getPossiblesVertical() const;
};


struct Move
{
	Board::Type type;
	size_t pos;
	int value;

	Move() : value(-1) {}

	Move(Board::Type type, size_t pos, int value) : type(type), pos(pos), value(-1) {}


	bool operator<(const Move &m) const
	{
		return value < m.value;
	}
};




Move getMax(Board &b, Board::Type type);

int minimax_getMax(Board &b, Move &maxMove, Board::Type type);


int minimax(Board &b, Move &max_move, Board::Type type, size_t depth);


namespace FDAI
{
	int internal_minimax(Board &b, Move *m, Board::Type t, size_t depth, const std::vector<size_t> &v, size_t from, size_t to);

	int minimax(Board &b, Move *m, Board::Type t, size_t depth);

	int max(Board &b, Board::Type t, size_t depth);

	int min(Board &b, Board::Type t, size_t depth);
}

