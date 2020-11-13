// Sudoku solver
// Author: Wojciech Martusewicz
// First version: 2013-03-13
// Significant improvements: 2020-10-29

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct element_t
{
	int firm;
	int posible[9];
};

//////////////////////////////////////////////////////////////
// I/O section
//////////////////////////////////////////////////////////////

int load_field(struct element_t field[9 * 9], char *name)
{
	int i, j, l = 0;
	char line[32];
	FILE *fp = NULL;
	fp = fopen(name, "r");
	if (fp == NULL)
		return -1;

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (line[0] == '#' || line[0] == 0 || line[0] == 10)
			continue;

		for (i = 0, j = 0; i < 9; j++)
		{
			if (line[j] - 48 >= 0 && line[j] - 48 <= 9)
			{
				field[l * 9 + i].firm = line[j] - 48;
				i++;
			}
		}

		if (++l > 9)
			break;
	}

	fclose(fp);
	return 0;
}

void clear_file()
{
	FILE *fp = NULL;
	fp = fopen("results", "w");
	fclose(fp);
}

void fprint_field_all(struct element_t field[9 * 9])
{
	FILE *fp = NULL;
	fp = fopen("results", "a+");
	if (fp == NULL)
		return;

	for (int i = 0; i < 81; i++)
		fprintf(fp, "%d", field[i].firm);

	fprintf(fp, "\n");
	fclose(fp);
}

void fprint_field_nice(struct element_t field[9 * 9])
{
	FILE *fp = NULL;
	fp = fopen("result", "w");
	if (fp == NULL)
		return;

	for (int i = 0; i < 81; i++)
	{
		fprintf(fp, "%d%s", field[i].firm, ((i + 1) % 3)?" ":"");
		if ((i + 1) % 3 == 0 && (i + 1) % 9 != 0)
			fprintf(fp, "  ");
		if ((i + 1) % 9 == 0 && i != 80)
			fprintf(fp, "\n");
		if ((i + 1) % 27 == 0 && i != 80)
			fprintf(fp, "\n");
	}

	fclose(fp);
}

void display_field(struct element_t field[9 * 9])
{
	char txt[16], tmp[16], firm = 0;

	for (int i = 0; i < 81; i++)
	{
		printf("%d%s", field[i].firm, ((i + 1) % 3)?" ":"");
		if ((i + 1) % 3 == 0)
			printf("  ");
		if ((i + 1) % 9 == 0)
			printf("\n");
		if ((i + 1) % 27 == 0)
			printf("\n");
		
		if (field[i].firm)
			firm++;
	}

	if (firm == 81)
		return;

	for (int i = 0; i < 81; i++)
	{
		txt[0] = 0;
		for (int j = 0; j < 9; j++)
			if (field[i].posible[j])
			{
				sprintf(tmp, "%d", j + 1);
				strcat(txt, tmp);
			}
		printf("%7s", txt);
		if ((i + 1) % 3 == 0)
			printf("  ");
		if ((i + 1) % 9 == 0)
			printf("\n");
		if ((i + 1) % 27 == 0)
			printf("\n");
	}
}

//////////////////////////////////////////////////////////////
// Sudoku resolving section
//////////////////////////////////////////////////////////////

void init_field(struct element_t field[9 * 9])
{
	for (int i = 0; i < 9 * 9; i++)
	{
		for (int j = 0; j < 9; j++)
			if (field[i].firm)
				field[i].posible[j] = 0;
			else
				field[i].posible[j] = 1;
	}
}

int check_posible_in_row(struct element_t field[9 * 9], int row)
{
	int rp = 0;

	// For any element in the row
	for (int I = row * 9; I < row * 9 + 9; I++)
	{
		// Check with any others emenents in the row
		for (int i = row * 9; i < row * 9 + 9; i++)
		{
			if (i == I) continue;
			if (field[i].firm > 0 && field[I].posible[field[i].firm - 1] > 0)
			{
				field[I].posible[field[i].firm - 1] = 0;
				rp++;
			}
		}
	}

	return rp;
}

int check_posible_in_col(struct element_t field[9 * 9], int col)
{
	int rp = 0;

	// For any element in the collumn
	for (int I = col; I <= 9 * 8 + col; I += 9)
	{
		for (int i = col; i <= 9 * 8 + col; i += 9)
		{
			if (i == I) continue;
			if (field[i].firm > 0 && field[I].posible[field[i].firm - 1] > 0)
			{
				field[I].posible[field[i].firm - 1] = 0;
				rp++;
			}
		}
	}

	return rp;
}

int check_posible_in_sqr(struct element_t field[9 * 9], int X, int Y)
{
	int rp = 0;

	for (int I = Y * 27 + X * 3; I < Y * 27 + 18 + X * 3 + 3; ((I + 1) % 3) ? (I++) : (I += 7))
	{
		for (int i = Y * 27 + X * 3; i < Y * 27 + 18 + X * 3 + 3; ((i + 1) % 3) ? (i++) : (i += 7))
		{
			if (i == I) continue;
			if (field[i].firm > 0 && field[I].posible[field[i].firm - 1] > 0)
			{
				field[I].posible[field[i].firm - 1] = 0;
				rp++;
			}
		}
	}

	return rp;
}

int check_all(struct element_t field[9 * 9])
{
	int rp = 0; // removed posibilities

	for (int i = 0; i < 9; i++)
		rp += check_posible_in_row(field, i);
	for (int i = 0; i < 9; i++)
		rp += check_posible_in_col(field, i);

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			rp += check_posible_in_sqr(field, x, y);

	return rp;
}

void remove_pos_in_firm(struct element_t field[9 * 9], int i)
{
	for (int j = 0; j < 9; j++)
		field[i].posible[j] = 0;
}

int find_firm(struct element_t field[9 * 9])
{
	int new_firms = 0, p = -1; // p it's first posibility

	for (int i = 0; i < 9 * 9; i++)
	{
		if (field[i].firm > 0)
			continue;

		p = -1;
		for (int j = 0; j < 9; j++)
		{
			if (p < 0 && field[i].posible[j] == 1)
			{
				p = j;
				continue;
			}

			// It's not only posibility, move on
			if (field[i].posible[j] == 1)
			{
				p = -1;
				break;
			}
		}
		if (p >= 0)
		{
			field[i].firm = p + 1;
			remove_pos_in_firm(field, i);
			new_firms++;
		}
	}

	return new_firms;
}

int find_only_one_in_row(struct element_t field[9 * 9], int row)
{
	int rp = 0, found = -1;

	// For each number from 1 to 9
	for (int num = 1; num <= 9; num++)
	{
		// Check if not already found
		for (int i = row * 9; i < row * 9 + 9; i++)
		{
			if (num == field[i].firm)
				goto next_num;
		}

		found = -1;
		// Check with emenents in the row
		for (int i = row * 9; i < row * 9 + 9; i++)
		{
			// Ommit founded
			if (field[i].firm)
				continue;

			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[i].posible[j] == 0)
					continue;

				if (j + 1 == num)
				{
					// Check if posibility more than one
					if (found >= 0)
						goto next_num;

					found = i;
					continue;
				}
			}
		}
		if (found >= 0)
		{
			field[found].firm = num;
			remove_pos_in_firm(field, found);
			rp++;
		}
	next_num:;
	}

	return rp;
}

int find_only_one_in_col(struct element_t field[9 * 9], int col)
{
	int rp = 0, found = -1;

	// For each number from 1 to 9
	for (int num = 1; num <= 9; num++)
	{
		// Check if not already found
		for (int i = col; i <= 9 * 8 + col; i += 9)
		{
			if (num == field[i].firm)
				goto next_num;
		}

		found = -1;
		// Check with emenents in the row
		for (int i = col; i <= 9 * 8 + col; i += 9)
		{
			// Ommit founded
			if (field[i].firm)
				continue;

			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[i].posible[j] == 0)
					continue;

				if (j + 1 == num)
				{
					// Check if posibiliti more than one
					if (found >= 0)
						goto next_num;

					found = i;
					continue;
				}
			}
		}
		if (found >= 0)
		{
			field[found].firm = num;
			remove_pos_in_firm(field, found);
			rp++;
		}
	next_num:;
	}

	return rp;
}

int find_only_one_in_sqr(struct element_t field[9 * 9], int X, int Y)
{
	int rp = 0, found = -1;

	// For each number from 1 to 9
	for (int num = 1; num <= 9; num++)
	{
		// Check if not already found
		for (int i = Y * 27 + X * 3; i < Y * 27 + 18 + X * 3 + 3; ((i + 1) % 3) ? (i++) : (i += 7))
		{
			if (num == field[i].firm)
				goto next_num;
		}

		found = -1;
		// Check with emenents in the row
		for (int i = Y * 27 + X * 3; i < Y * 27 + 18 + X * 3 + 3; ((i + 1) % 3) ? (i++) : (i += 7))
		{
			// Ommit founded
			if (field[i].firm)
				continue;

			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[i].posible[j] == 0)
					continue;

				if (j + 1 == num)
				{
					// Check if posibiliti more than one
					if (found >= 0)
						goto next_num;

					found = i;
					continue;
				}
			}
		}
		if (found >= 0)
		{
			field[found].firm = num;
			remove_pos_in_firm(field, found);
			rp++;
		}
	next_num:;
	}

	return rp;
}

int find_only_ones(struct element_t field[9 * 9])
{
	int rp = 0;

	for (int i = 0; i < 9; i++)
	{
		rp += find_only_one_in_row(field, i);
	}

	for (int i = 0; i < 9; i++)
	{
		rp += find_only_one_in_col(field, i);
	}

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
		{
			rp += find_only_one_in_sqr(field, x, y);
		}

	return rp;
}

int find_double_in_row(struct element_t field[9 * 9], int row)
{
	int rp = 0;
	int pn1 = -1, pn2 = -1; // possible number 1 and 2
	int pf1 = -1, pf2 = -1; // possible field 1 and 2
	int starter;

start:
	starter = 0;
	if (pf1 != -1)
	{
		starter = pf1 + 1;
		pf1 = -1;
	}

	// Find first pair (possible field 1)
	for (int i = row * 9 + starter; i < row * 9 + 9; i++)
	{
		int pos_num = 0;

		// Check if not already found
		if (field[i].firm)
			continue;

		for (int j = 0; j < 9; j++) // p it's first posibility
		{
			if (field[i].posible[j] == 1)
			{
				if (pos_num == 0)
					pn1 = j;
				else
					pn2 = j;
				pos_num++;
			}
		}

		if (pos_num != 2)
		{
			pos_num = 0;
			continue;
		}

		if (pf1 == -1)
		{
			pf1 = i;
			continue;
		}
		else
		{
			pf2 = i;

			// Check if pair's posibility are the same
			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[pf1].posible[j] != field[pf2].posible[j])
				{
					pf2 = -1;
					break;
				}
			}
			// Pair are not the same
			if (pf2 == -1)
				continue;
			break;
		}
	}

	if (pf1 == -1)
		return rp;
	if (pf2 == -1)
		goto start;

	// Reduce posibilities in row
	for (int i = row * 9; i < row * 9 + 9; i++)
	{
		// Ommit founded or pair
		if (field[i].firm || i == pf1 || i == pf2)
			continue;

		if (field[i].posible[pn1])
		{
			field[i].posible[pn1] = 0;
			rp++;
		}
		if (field[i].posible[pn2])
		{
			field[i].posible[pn2] = 0;
			rp++;
		}
	}

	return rp;
}

int find_double_in_col(struct element_t field[9 * 9], int col)
{
	int rp = 0;
	int pn1 = -1, pn2 = -1; // possible number 1 and 2
	int pf1 = -1, pf2 = -1; // possible field 1 and 2
	int starter;

start:
	starter = 0;
	if (pf1 != -1)
	{
		starter = 9 * (pf1 + 1);
		pf1 = -1;
	}

	// Find first pair (possible field 1)
	for (int i = col + starter; i <= 9 * 8 + col; i += 9)
	{
		int pos_num = 0;

		// Check if not already found
		if (field[i].firm)
			continue;

		for (int j = 0; j < 9; j++) // p it's first posibility
		{
			if (field[i].posible[j] == 1)
			{
				if (pos_num == 0)
					pn1 = j;
				else
					pn2 = j;
				pos_num++;
			}
		}

		if (pos_num != 2)
		{
			pos_num = 0;
			continue;
		}

		if (pf1 == -1)
		{
			pf1 = i;
			continue;
		}
		else
		{
			pf2 = i;

			// Check if pair's posibility are the same
			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[pf1].posible[j] != field[pf2].posible[j])
				{
					pf2 = -1;
					break;
				}
			}
			// Pair are not the same
			if (pf2 == -1)
				continue;
			break;
		}
	}

	if (pf1 == -1)
		return rp;
	if (pf2 == -1)
		goto start;

	// Reduce posibilities in col
	for (int i = col; i <= 9 * 8 + col; i += 9)
	{
		// Ommit founded or pair
		if (field[i].firm || i == pf1 || i == pf2)
			continue;

		if (field[i].posible[pn1])
		{
			field[i].posible[pn1] = 0;
			rp++;
		}
		if (field[i].posible[pn2])
		{
			field[i].posible[pn2] = 0;
			rp++;
		}
	}

	return rp;
}

int find_double_in_sqr(struct element_t field[9 * 9], int X, int Y)
{
	int rp = 0;
	int pn1 = -1, pn2 = -1; // possible number 1 and 2
	int pf1 = -1, pf2 = -1; // possible field 1 and 2
	int starter;

start:
	starter = 0;
	if (pf1 != -1)
	{
		if ((pf1 + 1) % 3)
			starter = pf1 + 1 - (Y * 27 + X * 3);
		else
			starter = pf1 + 7 - (Y * 27 + X * 3);
		pf1 = -1;
	}

	// Find first pair (possible field 1)
	for (int i = Y * 27 + X * 3 + starter; i < Y * 27 + 18 + X * 3 + 3; ((i + 1) % 3) ? (i++) : (i += 7))
	{
		int pos_num = 0;

		// Check if not already found
		if (field[i].firm)
			continue;

		for (int j = 0; j < 9; j++) // p it's first posibility
		{
			if (field[i].posible[j] == 1)
			{
				if (pos_num == 0)
					pn1 = j;
				else
					pn2 = j;
				pos_num++;
			}
		}

		if (pos_num != 2)
		{
			pos_num = 0;
			continue;
		}

		if (pf1 == -1)
		{
			pf1 = i;
			continue;
		}
		else
		{
			pf2 = i;

			// Check if pair's posibility are the same
			for (int j = 0; j < 9; j++) // p it's first posibility
			{
				if (field[pf1].posible[j] != field[pf2].posible[j])
				{
					pf2 = -1;
					break;
				}
			}
			// Pair are not the same
			if (pf2 == -1)
				continue;
			break;
		}
	}

	if (pf1 == -1)
		return rp;
	if (pf2 == -1)
		goto start;

	// Reduce posibilities in row
	for (int i = Y * 27 + X * 3; i < Y * 27 + 18 + X * 3 + 3; ((i + 1) % 3) ? (i++) : (i += 7))
	{
		// Ommit founded or pair
		if (field[i].firm || i == pf1 || i == pf2)
			continue;

		if (field[i].posible[pn1])
		{
			field[i].posible[pn1] = 0;
			rp++;
		}
		if (field[i].posible[pn2])
		{
			field[i].posible[pn2] = 0;
			rp++;
		}
	}

	return rp;
}

int find_double_ones(struct element_t field[9 * 9])
{
	int rp = 0;

	// Find two doubles in row
	for (int i = 0; i < 9; i++)
	{
		rp += find_double_in_row(field, i);
	}

	// Find two doubles in column
	for (int i = 0; i < 9; i++)
	{
		rp += find_double_in_col(field, i);
	}

	// Find two doubles in squer
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
		{
			rp += find_double_in_sqr(field, x, y);
		}

	return rp;
}

int find_two_in_row_squere(struct element_t field[9 * 9], int row)
{
	int rp = 0;
	int pn = -1;
	int f1 = -1, f2 = -1; // field 1 and 2

	// Find two possibilities in row
	for (int i, j = 0; j < 9; j++)
	{
		int pos_num = 0;

		for (i = row * 9; i < row * 9 + 9; i++)
		{
			// Check if not already found
			if (field[i].firm)
				continue;

			if (field[i].posible[j] == 1)
			{
				if (pos_num == 0)
				{
					pn = j; // this posibility
					f1 = i;
				}
				else
					f2 = i;
				pos_num++;
			}

			// Must be only two (in the same squere)
			if (pos_num > 2)
			{
				pn = f1 = f2 = -1;
				break;
			}
		}

		if (pos_num > 2)
			continue; // not this posibility
		if (pn == -1 || f1 == -1 || f2 == -1)
			continue;

		// Check if in the same squere
		if (f1 < 3 + row * 9 && f2 > 2 + row * 9)
			continue;
		if (f1 > 2 + row * 9 && f1 < 6 + row * 9 && ((f2 > 5 + row * 9) || (f2 < 3 + row * 9)))
			continue;
		if (f1 > 5 + row * 9 && f2 < 6 + row * 9)
			continue;

		// Check if no others in squere
		int r = 0, last_r;
		if (row > 2 && row < 6)
			r = 3;
		else
			if (row > 5)
				r = 6;
		last_r = r + 3;

		int c = 0, last_c;
		if (f1 > 9 * row + 2 && f1 < 9 * row + 6)
			c = 3;
		else
			if (f1 > 9 * row + 5)
				c = 6;
		last_c = c + 3;

		// Reduce all others posibility in squere
		for (int i = r * 9 + c; i < (last_r - 1) * 9 + last_c; i += 9)
			for (int n = 0; n < 3; n++)
			{
				if (field[i + n].firm || i + n == f1 || i + n == f2)
					continue;
				if (field[i + n].posible[pn])
				{
					field[i + n].posible[pn] = 0;
					rp++;
				}
			}
	}

	return rp;
}

int find_two_in_col_squere(struct element_t field[9 * 9], int col)
{
	int rp = 0;
	int pn = -1;
	int f1 = -1, f2 = -1; // field 1 and 2

	// Find two possibilities in coll
	for (int i, j = 0; j < 9; j++)
	{
		int pos_num = 0;

		for (i = col; i <= 9 * 8 + col; i += 9)
		{
			// Check if not already found
			if (field[i].firm)
				continue;

			if (field[i].posible[j] == 1)
			{
				if (pos_num == 0)
				{
					pn = j; // this posibility
					f1 = i;
				}
				else
					f2 = i;
				pos_num++;
			}

			// Must be only two (in the same squere)
			if (pos_num > 2)
			{
				pn = f1 = f2 = -1;
				break;
			}
		}

		if (pos_num > 2)
			continue; // not this posibility
		if (pn == -1 || f1 == -1 || f2 == -1)
			continue;

		// Check if in the same squere
		if (f1 < 9 * 3 + col && f2 > 9 * 2 + col)
			continue;
		if (f1 > 9 * 2 + col && f1 < 9 * 6 + col && ((f2 > 9 * 5 + col) || (f2 < 5 * 3 + col)))
			continue;
		if (f1 > 9 * 5 + col && f2 < 9 * 6 + col)
			continue;

		// Check if no others in squere
		int r = 0, last_r;
		if (f1 > 9 * 2 + col && f1 < 9 * 6 + col)
			r = 3;
		else
			if (f1 > 9 * 5 + col)
				r = 6;
		last_r = r + 3;

		int c = 0, last_c;
		if (col > 2 && col < 6)
			c = 3;
		else
			if (col > 5)
				c = 6;
		last_c = c + 3;

		// Reduce all others posibility in squere
		for (int i = r * 9 + c; i < (last_r - 1) * 9 + last_c; i += 9)
			for (int n = 0; n < 3; n++)
			{
				if (field[i + n].firm || i + n == f1 || i + n == f2)
					continue;
				if (field[i + n].posible[pn])
				{
					field[i + n].posible[pn] = 0;
					rp++;
				}
			}
	}

	return rp;
}

// Two possible numbers in row (or collumn), both in the same squere eliminate others in this squere
int find_two_in_squere(struct element_t field[9 * 9])
{
	int rp = 0;

	// Find two doubles in row
	for (int i = 0; i < 9; i++)
	{
		rp += find_two_in_row_squere(field, i);
	}

	// Find two doubles in column
	for (int i = 0; i < 9; i++)
	{
		rp += find_two_in_col_squere(field, i);
	}

	return rp;
}

int check_correct(struct element_t field[9 * 9])
{
	int sum = 0;
	
	// Find two doubles in row
	for (int row = 0; row < 9; row++)
	{
		for (int i = row * 9; i <= 9 * row + 8; i++)
			sum += field[i].firm;
		if (sum != 45)
			return 0;
		sum = 0;
	}

	// Find two doubles in column
	for (int col = 0; col < 9; col++)
	{
		for (int i = col; i <= 9 * 8 + col; i += 9)
			sum += field[i].firm;
		if (sum != 45)
			return 0;
		sum = 0;
	}

	return 1;
}

int count_solved(struct element_t field[9 * 9])
{
	int s = 0;

	for (int i = 0; i < 9 * 9; i++)
	{
		if (field[i].firm)
			s++;
	}
	return s;
}

int count_possibilities(struct element_t field[9 * 9])
{
	int p = 0;

	for (int i = 0; i < 9 * 9; i++)
	{
		for (int j = 0; j < 9; j++)
			if (field[i].posible[j])
				p++;
	}
	return p;
}

int solve(struct element_t field[9 * 9], int find_all)
{
	int rp, ok;

	do {
		do {
			rp = 0;
			rp += check_all(field);
			rp += find_firm(field);
			rp += find_only_ones(field);
		} while (rp);

		rp += find_double_ones(field);
		rp += find_two_in_squere(field); // if two posibilities in col, and both in the same squere, reduce posibility in remain column
	} while (rp);

	ok = check_correct(field);
	if (ok)
	{
		if (find_all)
			fprint_field_all(field); // log to file in compact format (to eliminate duplicates in some edit tool)
		else
		{
			printf("Solved: %s\n\n", (ok) ? "yes" : "no");
			display_field(field);
			fprint_field_nice(field);
			exit(0);
		}
	}

	return ok;
}

int try_possibilities(struct element_t field[9 * 9], int find_all, int level)
{
	int ok = 0, found = 0;
	struct element_t Newfield[9 * 9];

	for (int i = 0; i < 9 * 9; i++)
	{
		for (int j = 0; j < 9; j++)
			if (field[i].posible[j])
			{
				memcpy(Newfield, field, sizeof(struct element_t) * 9 * 9);
				Newfield[i].firm = j + 1;
				remove_pos_in_firm(Newfield, i);
				found = solve(Newfield, find_all);
				if (found == 0 && level > 0)
					ok += try_possibilities(Newfield, find_all, level - 1); // Recursion... :)
				else
					ok += found;
			}
	}

	return ok;
}

// Arguments: resu <puzzle file> <find_level> <find_all>
int main(int argc, char **argv)
{
	int ok, find_all = 0, level = 1;
	char sudoku[32] = "sudoku";
	struct element_t field[9 * 9];

	if (argc > 1)
		strncpy(sudoku, argv[1], sizeof(sudoku) - 1);
	if (argc > 2)
		level = atoi(argv[2]);
	if (argc > 3)
	{
		find_all = atoi(argv[3]);
		if (find_all)
			clear_file();
	}

	if (load_field(field, sudoku) < 0)
		exit(1);
	init_field(field);

	ok = solve(field, find_all);
	if (!ok)
	{
		printf("Basic solving failed\n");
		printf("Found: %d, possibilities left: %d\n\n", count_solved(field), count_possibilities(field));
		display_field(field);
		printf("Try deep analyze... ;)\n"); // Hehe, sounds seriously ;)
		ok = try_possibilities(field, find_all, 0);
	}

	if (!ok)
	{
		printf("Dig deeper!\n");
		ok = try_possibilities(field, find_all, level);
	}

	if (find_all)
		printf("Solved:%s, details in \"results\" file\n", (ok) ? "yes" : "no");
	else
	{
		printf("Solved: %s\n\n", (ok) ? "yes" : "no");
		display_field(field);
	}

	return ok;
}
