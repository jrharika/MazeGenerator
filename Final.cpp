#include <iostream> 
#include <string> 
#include <ctime> 
#include <vector>
#include <windows.h> ////////////////////////////////////////////////////////////////

using namespace std;

//if you change the maze size, you must change the output formatting
const int xDim = 10;
const int yDim = 10;

//maxBound keeps it from being too easy, minBound keeps it from being too hard
const int maxBound = 60;
const int minBound = 45;

const int HALL = 0;
const int WALL = 1;
const char PLAYER = 'O';
const char RESET_PORTAL = 'R';
const char PORTAL = 'P';

const int RPORTAL_NUM = 101;
const int PORTAL_NUM = 100;

///there are three functions that check whether it is possible to use a certain spot
void play(int** grid, int const finalBound, int const gameType);
bool setup(int** grid, int &finalBound);
int game_type();
void insert_portal(int** grid, int const finalBound, int currentCell[2]);
//checks if the location is empty to put a portal or portal drop in
bool available_cell(int** grid, int potentialX, int potentialY, int finalBound, int currentCell[2]);
void instructions();
bool want_to_restart();
char player_choice();
//checks if the inputted move takes you to a viable location (used during gameplay)
bool portal_check_player_move(int** grid, int bound, int choice, int &potentialX, int &potentialY, int currentCell[2]);
bool basic_check_player_move(int** grid, int bound, int choice, int &potentialX, int &potentialY, int currentCell[2]);
void basic_make_player_move(int potentialX, int potentialY, int currentCell[2]);
void portal_make_player_move(int** grid, int potentialX, int potentialY, int finalBound, int currentCell[2]);
void setup_grid(int** grid);
void print_grid(int** grid);
//checks if the location can be moved to and ins't in the list of past moves (setting up the maze)
bool can_move_to(int** grid, int potentialX, int potentialY, int bound, vector< vector<int> > &pastMoves);
char choose_move(int** grid, int currentX, int currentY, int bound, vector< vector<int>> &pastMoves);
void backtrack(int** grid, int& currentX, int& currentY, int bound, int &backtrackCount, vector < vector<int>> &pastMoves);
void make_move(int** grid, char direction, int& currentX, int& currentY, int bound, int &backtrackCount, vector < vector<int>> &pastMoves);
bool checking_bound(int** grid, int bound);
int finding_final_bound(int** grid, int &finalBound);
void print_maze(int** grid, int finalBound, int currentCell[2]);


void main() {
	bool restart = true;
	srand(time(0));
	int gameType = 0;

	//make int grid[xDim][yDim] using dynamic arrays;
	int** grid = new int*[xDim];
	for (int i = 0; i < xDim; i++)
		grid[i] = new int[yDim];

	//let's the player decide if they want to see the intro
	//(felt necessary because of how long the intro take)
	char seeIntro = 'o';
	while (seeIntro != 'y' && seeIntro != 'Y' && seeIntro != 'n' && seeIntro != 'N') {
		cout << endl << "Would you like to see the introduction (Y/N) ?" << endl << endl;
		cin >> seeIntro;
	}
	if (seeIntro == 'y' || seeIntro == 'Y') {
		instructions();
	}

	gameType = game_type();

	do {
		int finalBound = 0;
		bool setupDone = false;
		bool continuePlay = true;
		//create the maze using the steps in setup()
		while (setupDone == false)
		{
			setupDone = setup(grid, finalBound);
		}
		//continuePlay until terminating or reach end
		play(grid, finalBound, gameType);

		//check if player wants to play again
		restart = want_to_restart();
	} while (restart);

	//delete the excess formed when making the dynamic array
	for (int i = 0; i < xDim; i++)
		delete[] grid[i];
	delete[] grid;
	grid = nullptr;
}

void play(int** grid, int const bound, int const gameType) {
	bool notEnd = true;
	int currentCell[2];
	currentCell[0] = 1;
	currentCell[1] = 0;
	print_maze(grid, bound, currentCell);

	switch (gameType)
	{
	case 1:
		//repeats process of choosing, checking, and moving until at the end
		while (notEnd) {
			char choice = 'l';
			int potentialX = currentCell[0];
			int potentialY = currentCell[1];
			bool checkPlayerMove = false;

			//repeat until you get a location that you can move to
			while (checkPlayerMove == false)
			{
				while (choice != 'a' && choice != 'w' && choice != 's' && choice != 'd')
				{
					choice = player_choice();
				}
				checkPlayerMove = basic_check_player_move(grid, bound, choice, potentialX, potentialY, currentCell);
			}
			basic_make_player_move(potentialX, potentialY, currentCell);

			//print new location
			print_maze(grid, bound, currentCell);

			//check if you are at the ending
			if (currentCell[0] == xDim - 2 || currentCell[1] == yDim - 1) {
				notEnd = false;
			}
		}
		break;
	case 2:
		//repeats process of choosing, checking, and moving until at the end
		int numMoves = 0;
		cout << "Please exit the game and restart if the maze becomes impassable." << endl;
		while (notEnd) {
			char choice = 'l';
			int potentialX = currentCell[0];
			int potentialY = currentCell[1];
			bool checkPlayerMove = false;

			//repeat until you get a location that you can move to
			while (checkPlayerMove == false)
			{
				while (choice != 'a' && choice != 'w' && choice != 's' && choice != 'd')
				{
					choice = player_choice();
				}
				checkPlayerMove = portal_check_player_move(grid, bound, choice, potentialX, potentialY, currentCell);
			}
			portal_make_player_move(grid, potentialX, potentialY, bound, currentCell);

			//print new location
			print_maze(grid, bound, currentCell);

			//check if you are at the ending
			if (currentCell[0] == xDim - 2 || currentCell[1] == yDim - 1) {
				notEnd = false;
			}
			numMoves++;
			if (numMoves % 3 == 0) {
				insert_portal(grid, bound, currentCell);
			}
		}
		break;
	}

}

bool setup(int** grid, int &finalBound) {
	setup_grid(grid);
	print_grid(grid);

	finalBound = finding_final_bound(grid, finalBound);

	if (finalBound == 0)
	{
		return false;
	}
	else
	{
		vector <int> startingCell;
		startingCell.push_back(1);
		startingCell.push_back(0);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
}

int game_type() {
	int choice = 0;

	while (choice == 0)
	{
		cout << endl << "Which game type would you like to play?" << endl
			<< "	1. Basic" << endl
			<< "	2. Portal" << endl
			<< "  Enter '0' for descriptions of each type." << endl;
		cin >> choice;
		if (choice == 0)
		{
			cout << endl << "Basic mode allows you to play with no dangers other than the deadly walls." << endl
				<< "Portal mode makes random spots become portals represented by an 'R' or 'P'." << endl
				<< '\t' << "A portal marked by an 'R' resets you back to the starting place." << endl
				<< '\t' << "A portal with a 'P' lands you in a random location." << endl;
		}
		//if the input is invalid it justs repeats
		if (choice != 1 && choice != 2)
		{
			choice = 0;
		}
	}
	return choice;
}

void insert_portal(int** grid, int const finalBound, int currentCell[2]) {
	int i, j;
	int random;
	bool cellAvailable = false;
	while (cellAvailable == false) {
		i = rand() % xDim;
		j = rand() % yDim;
		cellAvailable = available_cell(grid, i, j, finalBound, currentCell);
	}

	//the portal will randomly be a reset portal or otherwise (twice as likely to not be reset type)
	random = rand() % 3;
	if (random == 0 || random == 1) {
		grid[i][j] = PORTAL_NUM;
	}
	else if (random == 2) {
		grid[i][j] = RPORTAL_NUM;
	}

}

///checks if the random location choosen is off the grid, a wall, the player's current cell, or the ending two cells
bool available_cell(int** grid, int potentialX, int potentialY, int finalBound, int currentCell[2]) {
	//starting
	if (potentialX >= xDim || potentialX < 0) {
		return false;
	}
	//out of bounds
	else if (potentialY >= yDim || potentialY < 0) {
		return false;
	}
	//out of bounds
	else if (grid[potentialX][potentialY] >= finalBound) {
		return false;
	}
	//current spot
	else if (potentialX == currentCell[0] && potentialY == currentCell[1]) {
		return false;
	}
	//second to ending spot
	else if (potentialX == xDim - 2 || potentialX == yDim - 2) {
		return false;
	}
	//ending spot
	else if (potentialX == xDim - 2 || potentialX == yDim - 1) {
		return false;
	}
	else {
		return true;
	}
}

void instructions() {
	cout << endl << '\t' << "Welcome to THE MAZE!" << endl;
	cout << endl;
	Sleep(3000);
	cout << '\t' << "When you reach the maze" << endl;
	Sleep(1000);
	cout << '\t' << "input 'w' / 'a' / 's' / 'd' " << endl;
	Sleep(1000);
	cout << '\t' << "and hit enter to move." << endl;
	Sleep(3000);
	cout << endl;
	cout << '\t' << "You will be represented" << endl;
	Sleep(1000);
	cout << '\t' << "by the 'O' in the maze. " << endl;
	Sleep(3000);
	cout << endl;
	cout << '\t' << "But be careful!" << endl;
	Sleep(1000);
	cout << '\t' << "If you try to move" << endl;
	Sleep(1000);
	cout << '\t' << "into a wall..." << endl;
	Sleep(3000);
	cout << '\t' << "..." << endl;
	Sleep(3000);
	cout << '\t' << "..." << endl;
	Sleep(3000);
	cout << '\t' << "The Vines on the Walls " << endl;
	Sleep(1000);
	cout << '\t' << "will catch you, and you" << endl;
	Sleep(1000);
	cout << '\t' << "won't be able to move!" << endl;
	Sleep(3000);
	cout << endl;
	cout << '\t' << "Happy Maze Running!" << endl;
	Sleep(1000);
	cout << '\t' << "And may the odds be EVER in your favor." << endl;
	cout << endl;
	Sleep(3000);
	cout << '\t' << "Please stand by as a maze is formed..." << endl;
	Sleep(5000);

}

bool want_to_restart() {
	char ans;
	cout << endl;
	cout << '\t' << "You survived!" << endl;
	cout << endl;
	Sleep(2000);
	cout << '\t' << "Congratulations!" << endl;
	Sleep(5000);
	cout << endl;
	cout << "Would you like to play agin? (Y/N)" << endl;
	cout << endl;
	cin >> ans;
	if (ans == 'Y' || ans == 'y') {
		return true;
	}
	else {
		return false;
	}
}

char player_choice()
{
	char choice;
	cin >> choice;
	return choice;
}

bool portal_check_player_move(int** grid, int bound, int choice, int &potentialX, int &potentialY, int currentCell[2]) {

	//indicate the potential new location based on payer's choice
	if (choice == 'a') {
		potentialX = currentCell[0] - 1;
	}
	else if (choice == 'w') {
		potentialY = currentCell[1] - 1;
	}
	else if (choice == 'd') {
		potentialX = currentCell[0] + 1;
	}
	else if (choice == 's') {
		potentialY = currentCell[1] + 1;
	}


	//check if the new position is a valid location
	if (grid[potentialX][potentialY] == RPORTAL_NUM) {
		return true;
	}
	else if (grid[potentialX][potentialY] == PORTAL_NUM) {
		return true;
	}
	else if (potentialX >= xDim || potentialX < 0) {
		return false;
	}
	else if (potentialY >= yDim || potentialY < 0) {
		return false;
	}
	else if (grid[potentialX][potentialY] >= bound) {
		return false;
	}
	else {
		return true;
	}
}

bool basic_check_player_move(int** grid, int bound, int choice, int &potentialX, int &potentialY, int currentCell[2]) {

	//indicate the potential new location based on payer's choice
	if (choice == 'a') {
		potentialX = currentCell[0] - 1;
	}
	else if (choice == 'w') {
		potentialY = currentCell[1] - 1;
	}
	else if (choice == 'd') {
		potentialX = currentCell[0] + 1;
	}
	else if (choice == 's') {
		potentialY = currentCell[1] + 1;
	}


	//check if the new position is a valid location
	if (potentialX >= xDim || potentialX < 0) {
		return false;
	}
	else if (potentialY >= yDim || potentialY < 0) {
		return false;
	}
	else if (grid[potentialX][potentialY] >= bound) {
		return false;
	}
	else {
		return true;
	}
}

//execute the decision made in check_player_move
void basic_make_player_move(int potentialX, int potentialY, int currentCell[2]) {
	currentCell[0] = potentialX;
	currentCell[1] = potentialY;
}

void portal_make_player_move(int** grid, int potentialX, int potentialY, int finalBound, int currentCell[2]) {
	if (grid[potentialX][potentialY] == RPORTAL_NUM)
	{
		currentCell[0] = 1;
		currentCell[1] = 0;
	}
	else if (grid[potentialX][potentialY] == PORTAL_NUM)
	{
		int i = 1000, j = 1000;
		bool cellAvailable = false;
		while (cellAvailable == false) {
			i = (rand() % xDim);
			j = (rand() % yDim);
			cellAvailable = available_cell(grid, i, j, finalBound, currentCell);
		}
		currentCell[0] = i;
		currentCell[1] = j;
	}
	else {
		currentCell[0] = potentialX;
		currentCell[1] = potentialY;
	}
}


void setup_grid(int** grid) {
	int x = 0;
	int y = 0;

	//assign the walls the value 9
	//left wall
	x = 0;
	for (y = 0; y < yDim; y++) {
		grid[x][y] = 99;
	}
	//right wall 
	x = xDim - 1;
	for (y = 0; y < yDim; y++) {
		grid[x][y] = 99;
	}
	//top wall
	y = 0;
	for (x = 0; x < xDim; x++) {
		grid[x][y] = 99;
	}
	//bottom wall
	y = yDim - 1;
	for (x = 0; x < xDim; x++) {
		grid[x][y] = 99;
	}

	//make enterence (one right from the top left corner)
	grid[1][0] = 0;

	//make exit (one left from the bottom right corner)
	grid[xDim - 2][yDim - 1] = 0;

	//assign the inside a random number 1-8
	for (x = 1; x < xDim - 1; x++)
	{
		for (y = 1; y < yDim - 1; y++)
		{
			grid[x][y] = rand() % 98 + 1;
		}
	}
}

void print_grid(int** grid) {
	cout << endl << endl << endl << endl << endl;
	for (int y = 0; y < yDim; y++)
	{
		for (int x = 0; x < xDim; x++)
		{
			if (grid[x][y] < 10)
			{
				cout << grid[x][y] << "  ";
			}
			else
			{
				cout << grid[x][y] << " ";
			}
		}
		cout << endl;
	}
}

//check if this spot is available (maze generating)
bool can_move_to(int** grid, int potentialX, int potentialY, int bound, vector< vector<int> > &pastMoves) {

	if (potentialX >= xDim || potentialX < 0) {
		return false;
	}
	else if (potentialY >= yDim || potentialY < 0) {
		return false;
	}
	else if (grid[potentialX][potentialY] >= bound) {
		return false;
	}

	//make vector for potentialCell with the x, y coor 
	vector <int> potentialCell;
	potentialCell.push_back(potentialX);
	potentialCell.push_back(potentialY);

	//if the potential cell is in the list of previous moves,
	//the function returns false b/c it's a nonviable path
	for (unsigned int i = 0; i < pastMoves.size() - 1; i++)
	{
		if (potentialCell[0] == pastMoves[i][0] && potentialCell[1] == pastMoves[i][1])
		{
			return false;
		}
	}

	//pastMoves.push_back(potentialCell);
	return true;
}

//check where you can move to and then return it (check in order D, R, U, L)
char choose_move(int** grid, int currentX, int currentY, int bound, vector< vector<int> > &pastMoves) {

	//if at the last spot, move down into the ending place
	if (currentX == xDim - 2 && currentY == yDim - 2) {
		return 'D';
	}

	//trying in the order Down, Right, Up, Left
	//down
	if (can_move_to(grid, currentX, currentY + 1, bound, pastMoves)) {
		return 'D';
	}
	//right
	else if (can_move_to(grid, currentX + 1, currentY, bound, pastMoves)) {
		return 'R';
	}
	//up
	else if (can_move_to(grid, currentX, currentY - 1, bound, pastMoves)) {
		return 'U';
	}
	//left
	else if (can_move_to(grid, currentX - 1, currentY, bound, pastMoves)) {
		return 'L';
	}
	else {
		//return 'b' for backtrack
		return 'B';
	}

}

void backtrack(int** grid, int& currentX, int& currentY, int bound, int &backtrackCount, vector < vector<int>> &pastMoves)
{
	bool mustBacktrack = true;
	bool notStartingPoint = true;
	int i = pastMoves.size() - 2; //index pos of the last move
	char direction = 'B';

	///////////////for debugging purposes/////////////////////////////////
	//int len = pastMoves.size();
	//for (int i = 0; i < len; i++) {
	//	cout << "(" << pastMoves[i][0] << "," << pastMoves[i][1] << ") ";
	//}////////////////////////////////////////////
	//cout <<"above is i = " << i<< endl;

	while (mustBacktrack && notStartingPoint && i >= 0)
	{
		currentX = pastMoves[i][0];
		currentY = pastMoves[i][1];
		direction = choose_move(grid, currentX, currentY, bound, pastMoves);
		if (direction != 'B')
		{
			mustBacktrack = false;
		}
		else if (currentX == 0 && currentY == 1)
		{
			notStartingPoint = false;
		}
		i--;
	}
}

void make_move(int** grid, char direction, int& currentX, int& currentY, int bound, int &backtrackCount, vector < vector<int>> &pastMoves) {
	if (direction == 'R' || direction == 'L' || direction == 'U' || direction == 'D')
	{
		if (direction == 'R')
		{
			currentX += 1;
		}
		else if (direction == 'L')
		{
			currentX -= 1;
		}
		else if (direction == 'U')
		{
			currentY -= 1;
		}
		else if (direction == 'D')
		{
			currentY += 1;
		}
		vector<int> currentCell;
		currentCell.push_back(currentX);
		currentCell.push_back(currentY);
		pastMoves.push_back(currentCell);
	}
	else if (direction == 'B')
	{
		backtrackCount++;
		backtrack(grid, currentX, currentY, bound, backtrackCount, pastMoves);
	}
	else {
		cout << "ERROR. Direction for function make_move() not possible";
	}
}

bool checking_bound(int** grid, int bound) {
	int currentX = 0;
	int currentY = 1;
	bool doneChecking = false;
	bool mazePossible = true;
	char direction;
	int backtrackCount = 0;


	//creating a vector of vectors
	//the first [] is how many-th move it is
	//the second []: 0 is x and 1 is y
	vector< vector<int> > pastMoves(0, vector<int>(1));
	vector <int> startingCell;
	startingCell.push_back(1);
	startingCell.push_back(0);
	pastMoves.push_back(startingCell);

	while (!doneChecking && mazePossible) {
		direction = choose_move(grid, currentX, currentY, bound, pastMoves);
		make_move(grid, direction, currentX, currentY, bound, backtrackCount, pastMoves);
		if (currentX == xDim - 2 && currentY == yDim - 1) {
			doneChecking = true;
		}
		else if (currentX == 0 && currentY == 1) {
			mazePossible = false;
		}
		else if (backtrackCount>50) {
			mazePossible = false;
		}
	}
	if (mazePossible = false) {
		return false;
	}
	else if (doneChecking == true)
	{
		//for (auto &i : pastMoves) {/////////////////////////////////////////////
		//	cout << " (" << i[0] << "," << i[1] << ");";//////////////////////////
		//}
		//cout << endl;///////////////////////////////for debugging purposes/////
		//print_maze(grid, bound);
		//cout << endl << "above is the grid for " << bound << endl;
		return true;
	}
	return false;
}

int finding_final_bound(int** grid, int &finalBound)
{
	int bound = 100;
	int smallestPossibleBound = minBound;
	finalBound = 0;
	bool mazePossible = true;
	bool boundUsable = true;

	while (mazePossible)
	{
		bound -= 1;
		mazePossible = checking_bound(grid, bound);
	}

	smallestPossibleBound = bound + 1;

	//if smallest bound too small, use minBound
	if (smallestPossibleBound < minBound) {
		finalBound = minBound;
	}
	else if (smallestPossibleBound > maxBound) {
		boundUsable = false;
	}
	else {
		finalBound = smallestPossibleBound;
	}
	if (boundUsable) {
		return finalBound;
	}
	else {
		return 0;
	}
}


///print the maze with walls and halls instead of numbers
void print_maze(int** grid, int finalBound, int playerCurrentCell[2])
{
	char wall = '#';
	char hall = ' ';
	char printable[xDim][yDim];
	cout << endl << endl << endl << endl << endl << endl << endl;
	cout << endl << endl << endl << endl << endl << endl << endl;
	for (int y = 0; y < yDim; y++)
	{
		for (int x = 0; x < xDim; x++)
		{
			if (x == playerCurrentCell[0] && y == playerCurrentCell[1])
			{
				printable[x][y] = PLAYER;
				cout << PLAYER;
			}
			else if (grid[x][y] == RPORTAL_NUM) {
				cout << RESET_PORTAL;
			}
			else if (grid[x][y] == PORTAL_NUM) {
				cout << PORTAL;
			}
			else if (grid[x][y] >= finalBound)
			{
				printable[x][y] = WALL;
				cout << wall;
			}
			else
			{
				printable[x][y] = HALL;
				cout << " ";
			}
		}
		cout << endl;
	}
}
