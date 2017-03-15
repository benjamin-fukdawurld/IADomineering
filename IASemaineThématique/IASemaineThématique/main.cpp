#include <iostream>
#include <vector>
#include <sstream>

#include <cassert>
#include <algorithm>

#include <future>

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




#include "utils.h"


int main(int argc, char *argv[])
{
	Board b(8, 8);
	Board::Type t(Board::Horizontal);
	cout << b.toString() << endl;
	while(!b.getPossibles(t).empty())
	{
		cout << ((t == Board::Horizontal) ? "Horizontal" : "Vertical") << endl;
		Move m;
		if(t == Board::Vertical)
		{
			minimax(b, m, t, 2);
		}
		else
		m = getMax(b, t);

		b.play(t, m.pos);
		cout << b.toString() << endl;
		t = (t == Board::Horizontal ? Board::Vertical : Board::Horizontal);
	}

	cout << b.toString() << endl;

	cout << endl;

	return 0;
}
