#ifndef PARALLELWINGO_PATTERN_H_
#define PARALLELWINGO_PATTERN_H_

namespace CJCU
{

class Pattern
{
public:
	int GetPattern();
	int get_open_moves(int c, int *board, int *moves);
	int get_joseki_moves(int c, int *board, int *moves);
	std::string hashcode_board(int *b);
	std::string hashcode_board(int *b, int x1, int y1, int x2, int y2);
};

} //namespace CJCU

#endif /* PARALLELWINGO_PATTERN_H_ */
