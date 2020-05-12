#include<iostream>
#include<bitset>
#include<cmath>
#include<fstream>
#include<unistd.h>
#include<curses.h>

template <int H, int W> // size of environment determined on construction
class GameOfLife
{
	private: int height; // height of environment
	private: int width; // width of environment

	private: std::bitset<H*W> environment;
	private: std::bitset<H*W> environment_buffer; // we only need one old state of the environment, save it as a field to avoid allocating space to copy state every time: https://codereview.stackexchange.com/questions/47167/conways-game-of-life-in-c/47179#47179


	public: GameOfLife(std::string statefile)
	{
		height = H;
		width = W;

		// read starting state from file
		std::ifstream file;
		file.open(statefile);
		if(file.is_open())
		{
			// parse file
			std::string line;
			std::getline(file, line);
			int filerows = std::stoi(line);
			std::getline(file, line);
			int filecols = std::stoi(line);

			if(filerows != height || filecols != width)
				{
					std::cout << "Invalid Dimensions" << std::endl;
					throw std::exception();
				}
			else
			{
				std::string bitstring = "";
				for(int r = 0; r < height; r++)
				{
					std::getline(file, line);
					bitstring += line;
				};
				environment = std::bitset<H*W>(bitstring);
			};
			file.close();
		}
		else
		{
			std::cout << "Could not retrieve starting state... quitting" << std::endl;
			throw std::exception();
		};
	};

	public: GameOfLife()
	{
		height = H;
		width = W;
	};


	public: void printState()
	{
		for(int r = 0; r < height; r++){
			for(int c = 0; c < width; c++)
			{
				int state = environment[r*width+c];
				if(state==1)
					std::cout << "G" << " ";
				else
					std::cout << " ." << "";
			};
			std::cout << std::endl;
		};
	};

	public: int getHeight()
	{
		return height;
	};

	public: int getWidth()
	{
		return width;
	};

	public: std::bitset<H*W> getState()
	{
		return environment;
	};

	private: int countNeighbours(int row, int col)
	// counts neighbours of cell
	{
		int neighbours = 0;
		//North Row
		neighbours += countRow(row-1, col-1, 3);
		// Cell Row
		neighbours += countRow(row, col-1, 3); // subtract starting cell
		//South Row
		neighbours += countRow(row+1, col-1, 3);
		if(environment[width*row+col] == 1)
			neighbours -= 1;
		return neighbours;
	};

	private: int countRow(int row, int col, int range)
	{
		int counter = 0;
		// check starting row
		if(row > height-1 || row < 0)
			return 0;
		// check start column
		if(col < 0)
		{
			range = range - abs(col);
			col = 0;
		};
		// check end column
		int endcol = col + range - 1;
		if(endcol >= width){
			int overshoot = endcol - (width - 1);
			range = range - overshoot;
		};
		// calculate 1D start index
		int start = row * width + col;
		// count live cells
		for(int i = start; i < start + range; i++)
			counter += environment[i];
		return counter;
	};

	public: void step()
	{
		// loop over cells
		for(int i = 0; i < height*width; i++)
		{
				// get row and col index for cell
				int row = i / width;
				int col = i % width;
				// count neighbours
				int n_neigh = countNeighbours(row, col);

				// get state of cell
				int cell = environment[i];
				if(cell == 0 && n_neigh == 3) // 4th rule: dead cells with exactly three neighbours come alive
					environment_buffer[i] = 1;
				else if(cell == 1 && (n_neigh == 2 || n_neigh == 3)) // 2nd rule: live cells with 2 or 3 neighbours stay alive
					environment_buffer[i] = 1;
				else	// rules 1 + 3: cells with too few or too many neighbours die (or stay dead)
					environment_buffer[i] = 0;
		};
		// swap buffer for environment
		std::bitset<H*W> swap;
		swap = environment;
		environment = environment_buffer;
		environment_buffer = swap;
	};

};

template<int H, int W> // statesize needs to be known for bitset, defined as H - height and W - width
class TUI
{
	int height;
	int width;

public: TUI()
{
	initscr(); // init terminal
	noecho(); // do not echo input
	cbreak(); // do not buffer input
	clear(); // clear terminal
	height = H;
	width = W;
};

	void printState(std::bitset<H*W> gamestate)
	{
		clear(); // clear screen
		std::string printstring;
		for(int r = 0; r < height; r++){
			printstring = "";
			for(int c = 0; c < width; c++)
			{
				int state = gamestate[r*width+c];
				if(state==1)
					printstring += "o ";
				else
					printstring += "  ";
			};
			mvaddstr(r,0,printstring.c_str()); // display one line of the state
		};
	refresh(); // draw screen
	};
};

int main()
{
	try
	{
	const int width = 40;
	const int height = 20;

	// read config from stdin
	std::cout << "Game size: " << height << " x " << width << std::endl;
	std::cout << "Location of Layout File: " << std::endl;
	std::string userinput;
	std::getline(std::cin, userinput);

	GameOfLife<height,width> game = GameOfLife<height,width>(userinput);
	std::cout << "Initialized game successfully!" << std::endl;

	// init curses
	TUI<height,width> interface = TUI<height,width>();
	std::bitset<width*height> gamestate;
	gamestate = game.getState();
	interface.printState(gamestate);

	for(int i = 0; i < 1000; i++)
	{
		usleep(50000);
		game.step();
		gamestate = game.getState();
		interface.printState(gamestate);

	};
	std::cout << "Game ended." << std::endl;
	}
	catch(const std::exception&){
		return 1;
	};
}
