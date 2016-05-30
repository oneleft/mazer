#include <random>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "../data/maze.h"

/**
 * containing namespace for all the application
 **/
namespace mazer2016sp1oua
{
    /* namespace for all generators - we will have a few different types 
     * of generators in the second assignment
     */
    namespace generators
    {
        /**
         * we can only go down or right for the binary generator
         **/
		enum class generator_dir
		{
			DOWN, RIGHT, UP, LEFT
		};

		enum class move_right_down
		{
			RIGHT, DOWN
		};

		class generator {
		protected:
			//a reference to the one maze in the system that we will manipulate
			data::maze & mymaze;
			//our random number generator
			std::mt19937 rnd_gen;
		public:
			generator(data::maze& m, int _width, int _height, int _seed)
				: mymaze(m), rnd_gen(_seed)
			{
				mymaze.height(_height);
				mymaze.width(_width);
				//clear all connections
				mymaze.clear();
				std::cout << "seed for this generator is: " <<
					_seed << std::endl;
			}

			virtual void generate() = 0;
		};


		/**
		* implements the binary tree generator for this application
		**/
		class binary_gen : public generator
		{
		public:
			binary_gen(data::maze& m, int _width, int _height, int _seed)
				: generator(m, _width, _height, _seed) {}
			/**
			* function that implements the generation of the maze
			**/
			void generate(void);
		};

		/**
		* implements the Wilson's algorithm for this application
		**/
		class wilson_gen : public generator
		{
		public:
			wilson_gen(data::maze& m, int _width, int _height, int _seed)
				: generator(m, _width, _height, _seed) {}
			/**
			* function that implements the generation of the maze
			**/
			void generate(void);
		};
		/**
		* implements the Sidewinder generator algorithm for this application
		**/
		class sidewinder_gen : public generator
		{
		public:
			sidewinder_gen(data::maze& m, int _width, int _height, int _seed)
				: generator(m, _width, _height, _seed) {}
			/**
			* function that implements the generation of the maze
			**/
			void generate(void);
		};

    }
}
