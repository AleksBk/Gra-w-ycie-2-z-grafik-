#pragma once
#include <iostream>
using namespace std;

class GameLife {

	int width;
	int height;
	int** previous_cells_state;

	
public:
	int** cells_state;
	GameLife(int w, int n){
		cells_state = new int*[width];
		width = w;
		height = n;

		for (int i = 0; i < width; i++)
		{
			cells_state[i] = new int[height];
		}

		previous_cells_state = new int*[width];

		for (int i = 0; i < width; i++)
		{
			previous_cells_state[i] = new int[height];
		}

	}

	~GameLife(){

		for (int i = 0; i < width; i++)
		{
			delete[] cells_state[i];
		}
		delete[] cells_state;

		for (int i = 0; i < width; i++)
		{
			delete[] previous_cells_state[i];
		}
		delete[] previous_cells_state;
	
	}

	int get_width() {
		return width;
	}
	
	int get_height() {
		return height;
	}

	int count_neighbours(int a, int b) {
		int sasiad = 0;

		if (b + 1 < height)
		{
			if (cells_state[a][b + 1] == 1) sasiad++;
		}

		if (b - 1 >= 0)
		{
			if (cells_state[a][b - 1] == 1) sasiad++;
		}

		if ((a - 1 >= 0) && (b - 1 >= 0))
		{
			if (cells_state[a - 1][b - 1] == 1) sasiad++;
		}
		if ((a - 1 >= 0) && (b + 1 < height))
		{
			if (cells_state[a - 1][b + 1] == 1) sasiad++;
		}
		if (a - 1 >= 0)
		{
			if (cells_state[a - 1][b] == 1) sasiad++;
		}
		if ((a + 1 < width) && (b + 1 < height))
		{
			if (cells_state[a + 1][b + 1] == 1) sasiad++;
		}
		if ((a + 1 < width) && (b - 1 >= 0))
		{
			if (cells_state[a + 1][b - 1] == 1) sasiad++;
		}
		if (a + 1 < width)
		{
			if (cells_state[a + 1][b] == 1) sasiad++;
		}

		return sasiad;
	}

	void calc_neighbors(int a, int b){
		int sasiad = count_neighbours(a, b);

		if (cells_state[a][b] == 1 && (sasiad < 2 || sasiad >= 4))
		{
			previous_cells_state[a][b] = 0;
		}
		else if (cells_state[a][b] == 1 && (sasiad >= 2 && sasiad < 4))
		{
			previous_cells_state[a][b] = 1;
		}
		else if (cells_state[a][b] == 0 && sasiad == 3)
		{
			previous_cells_state[a][b] = 1;
		}
		else {
			previous_cells_state[a][b] = 0;
		}
	}

	void read_state(FILE* plik1)
	{
		char c;
		for (int i = 0; i < 25; i++)
		{
			for (int j = 0; j < 40; j++)
			{

				fscanf(plik1, "%1c", &c);
				cells_state[i][j] = atoi(&c);
			}
			fscanf(plik1, "%1c", &c);
		}
	}
	
	void next_step() {
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				calc_neighbors(i, j);
			}
		}

		int** tmp = cells_state;
		cells_state = previous_cells_state;
		previous_cells_state = tmp;
	}

	int is_alive(int x, int y) {
		return cells_state[x][y];
	}

	int was_alive(int x, int y) {
		return previous_cells_state[x][y];
	}
};