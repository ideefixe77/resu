Program: Sudoku solver
Author: Wojciech Martusewicz
Start of development: 2013-03-13
Significant improvements: 2020-10-29

Basic run:

	resu <file=sudoku>

By default, the puzzle is saved in the "sudoku" file.
If another puzzle is to be solved, the filename must be given as a parameter.

Comments can be used in the puzzle file (the line must start with the '#' sign).
On the line, characters other than numbers are omitted.

Advanced run:

	resu <file=sudoku> <recursion_level=0> <full_find=0>

For difficult sudoku, you can use recursive search by specifying the recursion depth level (eg 1,2,3 ..).
We can also set up a full search. In the case of ambiguous sudoku that have more than one solution, we can try to find them.

Example "sudoku" file:
0 0 0  2 6 5  0 0 0
0 8 0  0 0 0  0 9 0
0 6 0  0 0 0  0 7 0

7 0 0  0 4 0  0 0 3
8 0 0  7 0 3  0 0 4
9 0 0  0 8 0  0 0 7

0 4 0  0 0 0  0 6 0
0 7 0  0 0 0  0 5 0
0 0 0  1 9 7  0 0 0

By running this command (execution time about 20 minutes):
	resu sudoku 3 1
We can find 41 different, correct solutions (the generated "results" file needs to be sorted and get rid of duplicate solutions).

