/*                    
 *  This file is part of Christian's OpenMP parallel Sudoku Solver
 *  
 *  Copyright (C) 2013 by Christian Terboven <christian@terboven.com>
 *                                                                       
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "SudokuBoard.h"

#include <cstring>

CSudokuBoard::CSudokuBoard(int fsize, int bsize)
	: field_size(fsize), block_size(bsize), solutions(-1)
{
	field = new int[field_size*field_size];
}


CSudokuBoard::CSudokuBoard(const CSudokuBoard& other)
	: field_size(other.getFieldSize()), block_size(other.getBlockSize()), solutions(other.getNumSolutions())
{
	field = new int[field_size*field_size];
	std::memcpy(field, other.field, sizeof(int) * field_size*field_size);
}


CSudokuBoard::~CSudokuBoard(void)
{
	delete[] field;
}


bool CSudokuBoard::loadFromFile(char *filename)
{
  std::ifstream ifile(filename);
  
  if (!ifile) {
    std::cout << "There was an error opening the input file " << filename << std::endl;
    std::cout << std::endl;
    return false;
  }

  for (int i = 0; i < this->field_size; i++) {
    for (int j = 0; j < this->field_size; j++) {
	  ifile >> this->field[ACCESS(i,j)];
    }
  }

  return true;
}


void CSudokuBoard::printBoard()
{
	for(int i = 0; i < field_size; i++) {
		for(int j = 0; j < field_size; j++) {
			std::cout << std::setw(3) << 
				this->field[ACCESS(i,j)] 
				<< " ";
		}
		std::cout << std::endl;
	}
}

// Function to check if it's safe to place a number in the current cell (i, j)
bool CSudokuBoard::isValidSeq(int x, int y, int value) {
	// check row
	for (int i = 0; i < field_size; i++) {
		if (field[ACCESS(x, i)] == value) {
			return false;
		}
	}
	
	// check column
	for (int i = 0; i < field_size; i++) {
		if (field[ACCESS(i, y)] == value) {
			return false;
		}
	}
	
	// check block
	int block_x = x / block_size;
	int block_y = y / block_size;
	
	for (int i = block_x * block_size; i < block_x * block_size + block_size; i++) {
		for (int j = block_y * block_size; j < block_y * block_size + block_size; j++) {
			if (field[ACCESS(i, j)] == value) {
				return false;
			}
		}
	}
	
	return true;
}

// Non recursive function to solve the game
void solve(bool parallel) {
	// find the first empty cell
	int x = -1;
	int y = -1;

	bool (*isValid)() = parallel ? isValidPar : isValidSeq;

	// can be paralelized
	for (int i = 0; i < field_size; i++) {
		for (int j = 0; j < field_size; j++) {
			if (field[ACCESS(i, j)] == 0) {
				x = i;
				y = j;
				break;
			}
		}
		if (x != -1) {
			break;
		}
	}
	
	// if there is no empty cell, we have a solution
	if (x == -1) {
		incrementSolutionCounter();
		return;
	}
	
	// try all possible values for the empty cell
	for (int i = 1; i <= field_size; i++) {
		if (isValid(x, y, i)) {
			field[ACCESS(x, y)] = i;
			solve(parallel);
			field[ACCESS(x, y)] = 0;
		}
	}
}
