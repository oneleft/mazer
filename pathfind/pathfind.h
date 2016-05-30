#pragma once
#include <random>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <stdexcept>
#include "../constants/constants.h"
#include "../data/maze.h"

/**
* containing namespace for all the application
**/
namespace mazer2016sp1oua
{
	/* namespace for all generators - we will have a few different types
	* of pathfind algorithms in the second assignment
	*/
	namespace pathfinders
	{

		class maze_sover {
		protected:
			//a reference to the one maze in the system that we will manipulate
			data::maze & mymaze;
		public:
			maze_sover(data::maze& m) : mymaze(m) {}
			virtual void solve() = 0;
		};


		/**
		* implements the Dijkstra pathfind algorithm for this application
		**/
		class dijkstra_solver : public maze_sover
		{
		public:
			dijkstra_solver(data::maze& m)
				: maze_sover(m) {}
			/**
			* function that implements the pathfind
			**/
			void solve(void);
		};

		/**
		* implements the Breadth First Search pathfind algorithm for this application
		**/
		class bfs_solver : public maze_sover
		{
		public:
			bfs_solver(data::maze& m)
				: maze_sover(m) {}
			/**
			* function that implements the pathfind
			**/
			void solve(void);
		};


		/**
		* implements the A start pathfind algorithm with manhattan/euclidean/etc heuristic for this application
		**/
		// Pointer to heuristic function
		typedef double (*pHeuristic)(int start_x, int start_y, int end_x, int end_y);

		class a_star_solver : public maze_sover
		{
			pHeuristic heuristic_func;
		public:
			/* set maze object and heuristic function */
			a_star_solver(data::maze& m, pHeuristic heuristic_func)
				: maze_sover(m), heuristic_func(heuristic_func) {}
			/**
			* function that implements the pathfind
			**/
			void solve(void);
		};
	}
}
