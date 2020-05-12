#include<iostream>
#include<bitset>

template <int H, int W> // size of environment determined on construction
class GameOfLife
{
	private: int height; // height of environment
	private: int width; // width of environment

	private: std::bitset<H*W> environment;
	
	public: GameOfLife()
	{
		height = H;
		width = W;
	};

	public: void printState()
	{
		for(int r = 0; r < height; r++){
			for(int c = 0; c < width; c++)
				std::cout << environment[r*width+c];
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

};


int main()
{
	GameOfLife<25,25> game = GameOfLife<25,25>();
	std::cout << "Initialized game successfully!" << std::endl;
	std::cout << "Height: " << game.getHeight() << std::endl;
	std::cout << "Width: " << game.getWidth() << std::endl;
	std::cout << "Initial State: " << std::endl;
	game.printState();

}
