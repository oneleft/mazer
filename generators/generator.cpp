#include "generator.h"

/**
* iterates over the maze and at each point flips a coin. We will either draw
* an edge going downwards or right.
**/
void mazer2016sp1oua::generators::binary_gen::generate(void)
{
	// there are only two values we are interested in 0 or 1 to represent the
	// coin flip.
	std::uniform_int_distribution<int> w_dist(0, 1);
	int w_count, h_count;
	//iterate over the maze
	for (h_count = 0; h_count < mymaze.height(); ++h_count)
	{
		for (w_count = 0; w_count < mymaze.width(); ++w_count)
		{
			// flip the coin and store that value as a direction
			move_right_down dir = static_cast<move_right_down>(w_dist(rnd_gen));
			auto & cells = mymaze.get_cells();
			auto & cur_cell = cells[h_count][w_count];
			data::cell * other_cell = nullptr;
			//while we don't have a successful coin flip
			while (!other_cell)
			{
				data::edge out_edge, in_edge;

				//if the flip results in going down
				if (dir == move_right_down::DOWN)
				{
					//check for out of bounds
					if (h_count < mymaze.height() - 1)
					{
						//create the edges that go both out from here and
						//in from here
						out_edge.in_x = in_edge.out_x = w_count;
						out_edge.in_y = in_edge.out_y = h_count;
						out_edge.out_x = in_edge.in_x = w_count;
						out_edge.out_y = in_edge.in_y = h_count + 1;
						//grab the cell at the other end of this edge
						other_cell = &cells[out_edge.out_y][out_edge.out_x];
						//set the out_edge and the in_edge in our maze
						cur_cell.adjacents[int(data::direction::SOUTH)]
							= out_edge;
						other_cell->adjacents[int(data::direction::NORTH)]
							= in_edge;
					}
				}
				else
				{
					//horizontal edge
					if (w_count < mymaze.width() - 1)
					{
						//construct the outgoing and incoming edges
						out_edge.in_x = in_edge.out_x = w_count;
						out_edge.in_y = in_edge.out_y = h_count;
						out_edge.out_x = in_edge.in_x = w_count + 1;
						out_edge.out_y = in_edge.in_y = h_count;
						//set the edges in our maze
						other_cell = &cells[out_edge.out_y][out_edge.out_x];
						cur_cell.adjacents[int(data::direction::EAST)]
							= out_edge;
						other_cell->adjacents[int(data::direction::WEST)]
							= in_edge;
					}
				}
				dir = static_cast<move_right_down>(w_dist(rnd_gen));
				//no need to draw an edge from the bottom right of the maze
				//in fact it would be out of bounds so we stop here.
				if (w_count == mymaze.width() - 1
					&& h_count == mymaze.height() - 1)
				{
					//we have finished generating the maze and so set it to
					//the initialised state
					mymaze.init();
					break;
				}
			}
		}
	}
}

/**
* Generate maze using Wilson's algorithm
**/

void mazer2016sp1oua::generators::wilson_gen::generate(void)
{
	auto & cells = mymaze.get_cells();
	/* Unvisited points */
	std::vector<data::point> unvisited;
	/* Visited points (already part of maze) */
	std::vector<data::point> visited;
	/* Current path */
	std::vector<data::point> path;
	/* Moves while generate path (with directions. each cell can be used 4 times) */
	std::vector<data::point> moves;
	/* Generate unvisited points */
	for (auto x = 0; x < mymaze.width(); ++x)
		for (auto y = 0; y < mymaze.height(); ++y)		
			unvisited.push_back(data::point(x, y));
	
	// Pick last right bottom as init cell
	int pos_first = unvisited.size() - 1;
	/* Remove from unvisited */
	data::point first_cell = unvisited[pos_first];
	unvisited.erase(unvisited.begin() + pos_first);
	/* Add initial cell to visited list */	
	visited.push_back(first_cell);
	/* Until we proceed all points */
	while (unvisited.size() > 0) {
		std::uniform_int_distribution<int> cell_dist(0, unvisited.size() - 1);
		int random_pos = static_cast<int>(cell_dist(rnd_gen));

		data::point random_cell = unvisited[random_pos];
		path.push_back(random_cell);
		moves.push_back(random_cell);
		data::point last_pos = random_cell;

		do {
			// Check directions that we can move		
			std::vector<data::point> can_move;
			// We can move left if we NOT moved in current path and in main moves we have at least 1 direction for move
			if (find_if(path.begin(), path.end(), data::point_predicate_by_coords(data::point(last_pos.x + 1, last_pos.y))) == path.end()
				&& find_if(moves.begin(), moves.end(), data::point_predicate_full(data::point(last_pos.x + 1, last_pos.y, data::direction::WEST))) == moves.end()
				&& last_pos.x + 1 < mymaze.width()) {
				can_move.push_back(data::point(last_pos.x + 1, last_pos.y, data::direction::WEST));
			}
			if (find_if(path.begin(), path.end(), data::point_predicate_by_coords(data::point(last_pos.x - 1, last_pos.y))) == path.end()
				&& find_if(moves.begin(), moves.end(), data::point_predicate_full(data::point(last_pos.x - 1, last_pos.y, data::direction::EAST))) == moves.end()
				&& last_pos.x - 1 >= 0) {
				can_move.push_back(data::point(last_pos.x - 1, last_pos.y, data::direction::EAST));
			}
			if (find_if(path.begin(), path.end(), data::point_predicate_by_coords(data::point(last_pos.x, last_pos.y + 1))) == path.end()
				&& find_if(moves.begin(), moves.end(), data::point_predicate_full(data::point(last_pos.x, last_pos.y + 1, data::direction::NORTH))) == moves.end()
				&& last_pos.y + 1 < mymaze.height()) {
				can_move.push_back(data::point(last_pos.x, last_pos.y + 1, data::direction::NORTH));
			}
			if (find_if(path.begin(), path.end(), data::point_predicate_by_coords(data::point(last_pos.x, last_pos.y - 1))) == path.end()
				&& find_if(moves.begin(), moves.end(), data::point_predicate_full(data::point(last_pos.x, last_pos.y - 1, data::direction::SOUTH))) == moves.end()
				&& last_pos.y - 1 >= 0) {
				can_move.push_back(data::point(last_pos.x, last_pos.y - 1, data::direction::SOUTH));
			}
			// We cant move anymore from this point
			if (can_move.size() == 0) {
				if (path.size() < 2) throw std::runtime_error("path size < 2");
				// Pop back
				last_pos = path[path.size() - 2	];
				path.pop_back();
			}
			else {
				// Pick random direction
				std::uniform_int_distribution<int> dir_dist(0, can_move.size() - 1);
				int dir_pos = static_cast<int>(dir_dist(rnd_gen));

				data::point new_pos = can_move.at(dir_pos);
				// Save to the path
				path.push_back(new_pos);
				// Save to the total moves wile construct current path (from one cell we can move 4 times and no more)
				moves.push_back(new_pos);
				// Save new point as last visited
				last_pos = new_pos;
			}
			// Until we reach point from visited list
		} while (find(visited.begin(), visited.end(), last_pos) == visited.end());


		for (std::vector<data::point>::iterator it = path.begin(); it != path.end(); ++it) {
			// Mark current point from path as visited
			visited.push_back(*it);
			auto p = find_if(unvisited.begin(), unvisited.end(), data::point_predicate_by_coords(*it));
			if (p != unvisited.end()) {
				unvisited.erase(p);
			}

			switch (it->dir)
			{
				// We come from east
			case data::direction::EAST:
			{
				cells[it->y][it->x].adjacents[int(data::direction::EAST)]
					= data::edge(it->x, it->y, it->x + 1, it->y);

				cells[it->y][it->x + 1].adjacents[int(data::direction::WEST)]
					= data::edge(it->x + 1, it->y, it->x, it->y);
			}
			break;
			// We come from west
			case data::direction::WEST:
			{
				cells[it->y][it->x].adjacents[int(data::direction::WEST)]
					= data::edge(it->x, it->y, it->x - 1, it->y);

				cells[it->y][it->x - 1].adjacents[int(data::direction::EAST)]
					= data::edge(it->x - 1, it->y, it->x, it->y);				
			}
			break;
			// We come from south
			case data::direction::SOUTH:
			{
				cells[it->y][it->x].adjacents[int(data::direction::SOUTH)]
					= data::edge(it->x, it->y, it->x, it->y + 1);

				cells[it->y + 1][it->x].adjacents[int(data::direction::NORTH)]
					= data::edge(it->x, it->y + 1, it->x, it->y);
			}
			break;
			// We come from north
			case data::direction::NORTH:
			{
				cells[it->y][it->x].adjacents[int(data::direction::NORTH)]
					= data::edge(it->x, it->y, it->x, it->y - 1);

				cells[it->y - 1][it->x].adjacents[int(data::direction::SOUTH)]
					= data::edge(it->x, it->y - 1, it->x, it->y);
			}
			break;
			default:
				break;
			}
		}

		moves.clear();
		path.clear();
	}
	mymaze.init();
}


/**
* Generate maze using Sidewinder algorithm
**/
void mazer2016sp1oua::generators::sidewinder_gen::generate(void)
{
	auto & cells = mymaze.get_cells();
	std::uniform_int_distribution<int> carve_east_dist(0, /*3*/1);

	// Iterate through maze rows
	for (auto row = 0; row < mymaze.height(); ++row) {
		// First row always must be single passage
		if (row == 0) {
			for (auto col = 0; col < mymaze.width() - 1; ++col) {
				//set the edges in our maze
				cells[row][col].adjacents[int(data::direction::EAST)]
					= data::edge(col, row, col + 1, row);
				cells[row][col + 1].adjacents[int(data::direction::WEST)]
					= data::edge(col + 1, row, col, row);
			}
		}
		else {
			std::vector<data::cell> runset;
			for (auto col = 0; col < mymaze.width(); ++col) {
				
				runset.push_back(cells[row][col]);
				// Decide to carve east
				int carve_east = static_cast<int>(carve_east_dist(rnd_gen));
				if (col + 1 == mymaze.width() || carve_east /*% 2 == 0*/) {

					std::uniform_int_distribution<int> carve_north_dist(0, runset.size() - 1);
					int carve_north = static_cast<int>(carve_north_dist(rnd_gen));

					int x = runset.at(carve_north).x;
					int y = runset.at(carve_north).y;
					
					//set the edges in our maze					
					cells[y][x].adjacents[int(data::direction::NORTH)]
						= data::edge(x, y, x, y - 1);
					cells[y - 1][x].adjacents[int(data::direction::SOUTH)]
						= data::edge(x, y - 1, x, y);

					runset.clear();
					
				} else {
					
					cells[row][col].adjacents[int(data::direction::EAST)]
						= data::edge(col, row, col + 1, row);
					
					cells[row][col + 1].adjacents[int(data::direction::WEST)]
						= data::edge(col + 1, row, col, row);
				}
			}
		}
	}

	mymaze.init();
}
