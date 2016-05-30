
#include "pathfind.h"
#include "../constants/constants.h"



void mazer2016sp1oua::pathfinders::dijkstra_solver::solve(void) {
	auto & cells = mymaze.get_cells();
	int target_x = mymaze.width() - 1, target_y = mymaze.height() - 1;
	
	std::set<data::cell*> Q;
	data::cell * u, *v;
	data::cell * path = nullptr;

	for (int y = 0; y < mymaze.height(); ++y) {
		for (int x = 0; x < mymaze.width(); ++x) {
			// Initial distance == infinity
			cells[y][x].g_cost = constants::INF;
			// Initial prev node == null
			cells[y][x].parent = nullptr;
			Q.insert(&cells[y][x]);
		}
	}

	while (!Q.empty()) {
		std::set<data::cell *>::iterator it = std::min_element(Q.begin(), Q.end(), [](data::cell *c1, data::cell *c2) { return c1->g_cost < c2->g_cost;});
		u = *it;
		Q.erase(u);
		if (u->x == target_x && u->y == target_y) {
			path = u;
			break;
		}
		// For each neighbor
		for (int e_count = 0; e_count < data::num_dirs; ++e_count) {
			int adj_x = cells[u->y][u->x].adjacents[e_count].out_x;
			int adj_y = cells[u->y][u->x].adjacents[e_count].out_y;
			if (adj_x == constants::ERROR || adj_y == constants::ERROR) continue;

			v = &cells[adj_y][adj_x];
			if (u->g_cost == constants::INF) u->g_cost = constants::PATH_COST;

			long alt = u->g_cost + constants::PATH_COST;
			if (alt < v->g_cost) {
				v->g_cost = alt;
				v->parent = u;
			}
		} // End for neighbors
	} // End wile Q

	if (path == nullptr) {
		std::cout << "Path not found!" << std::endl;
	}
	else {
		mymaze.path.clear();
		int prev_x = path->x, prev_y = path->y;
		path = path->parent;
		while (path != nullptr) {
			mymaze.path.push_back(data::edge(path->x, path->y, prev_x, prev_y));
			prev_x = path->x;
			prev_y = path->y;
			path = path->parent;
		}
	}

}

void mazer2016sp1oua::pathfinders::bfs_solver::solve(void) {
	int w_count = 0, h_count = 0;
	auto & cells = mymaze.get_cells();
	std::list<data::edge *> path_queue;
	std::list<data::edge *> moves;
	// Add adj edges to start point
	for (int e_count = 0; e_count < data::num_dirs; ++e_count)
	{
		if (cells[h_count][w_count].adjacents[e_count].out_x != constants::ERROR
			&& cells[h_count][w_count].adjacents[e_count].out_y != constants::ERROR) {
			path_queue.push_back(&cells[h_count][w_count].adjacents[e_count]);
		}
	}
	// Until we reach target cell or stuck(no more moves)
	while (!path_queue.empty()) {
		data::edge *cur_edge = path_queue.front();
		path_queue.pop_front();

		data::direction d = data::get_dir(cur_edge->out_x, cur_edge->out_y, cur_edge->in_x, cur_edge->in_y);
		mymaze.set_visited(*cur_edge, int(d));
		moves.push_back(cur_edge);

		// Check if we reach "exit"
		if (cur_edge->out_x == mymaze.width() - 1 && cur_edge->out_y == mymaze.height() - 1) {
			// Gather path reverse moves
			while (!moves.empty()) {
				mymaze.set_unvisited();
				data::edge e = *moves.back();
				data::direction edg_dir = data::get_dir(e.out_x, e.out_y, e.in_x, e.in_y);
				mymaze.path.push_back(e);
				moves.pop_back();
				// find previus
				while (!moves.empty() && !(moves.back()->out_x == e.in_x && moves.back()->out_y == e.in_y)) {
					moves.pop_back();
				}				
			}			
			break;
		}
		// Gather adjacents edges
		for (int e_count = 0; e_count < data::num_dirs; ++e_count)
		{
			// Append unvisited edge to queue
			if (cells[cur_edge->out_y][cur_edge->out_x].adjacents[e_count].out_x != constants::ERROR
				&& cells[cur_edge->out_y][cur_edge->out_x].adjacents[e_count].out_y != constants::ERROR
				&& !cells[cur_edge->out_y][cur_edge->out_x].adjacents[e_count].visited) {

				data::edge *adj = &cells[cur_edge->out_y][cur_edge->out_x].adjacents[e_count];
				data::direction adj_d = data::get_dir(adj->out_x, adj->out_y, adj->in_x, adj->in_y);
        

				path_queue.push_back(&cells[cur_edge->out_y][cur_edge->out_x].adjacents[e_count]);
			}
		}
	}
	data::edge first = mymaze.path.back();
	data::edge last = mymaze.path.front();	
	// If we not reach (0, 0) as last point or first point is not (width - 1, height - 1) -> path not found
	if ( first.in_x != 0 || first.in_y != 0
		|| last.out_x != mymaze.width() - 1 || last.out_y != mymaze.height() - 1 ) {
		std::cout << "Path not found !" << std::endl;
		mymaze.path.clear();
	}	
}

void mazer2016sp1oua::pathfinders::a_star_solver::solve(void) {
	std::set<data::cell *> open;
	data::cell *path = nullptr;
	data::cell *cur_cell, *adj_cell;
	auto & cells = mymaze.get_cells();
	int start_x = 0, start_y = 0, end_x = mymaze.width() - 1, end_y = mymaze.height() - 1;	
	cur_cell = &cells[start_y][start_x];
	open.insert(cur_cell);
	cur_cell->closed = true;	

	while (!open.empty() && path == nullptr) {
		// Find lowest f_cost cell
		std::set<data::cell *>::iterator it = std::min_element(open.begin(), open.end(), [](data::cell *c1, data::cell *c2) {
			return c1->f_cost < c2->f_cost;
		});

		cur_cell = *it;
		open.erase(*it);
		cur_cell->closed = true;
		// Gather adjacents edges
		for (int e_count = 0; e_count < data::num_dirs; ++e_count)
		{
			int adj_x = cells[cur_cell->y][cur_cell->x].adjacents[e_count].out_x;
			int adj_y = cells[cur_cell->y][cur_cell->x].adjacents[e_count].out_y;
			if (adj_x == constants::ERROR || adj_y == constants::ERROR) continue;

			adj_cell = &cells[adj_y][adj_x];

			if (adj_x == end_x && adj_y == end_y) {
				adj_cell->parent = cur_cell;
				path = adj_cell;
				break;
			}
			// Append unclosed cell to set
			if (!cells[adj_y][adj_x].closed) {
				// Cell already in open set
				if (open.find(adj_cell) != open.end()) {
					if (cur_cell->g_cost + constants::PATH_COST < adj_cell->g_cost) {
						adj_cell->parent = cur_cell;
						adj_cell->g_cost = cur_cell->g_cost + constants::PATH_COST;
						adj_cell->h_cost = heuristic_func(cur_cell->x, cur_cell->y, adj_x, adj_y);
						adj_cell->f_cost = adj_cell->g_cost + adj_cell->h_cost;
					}
				}
				// Otherwise update g, h, f values, set parent and add to open set
				else {
					adj_cell->parent = cur_cell;
					adj_cell->g_cost = cur_cell->g_cost + constants::PATH_COST;
					adj_cell->h_cost = heuristic_func(cur_cell->x, cur_cell->y, adj_x, adj_y);
					adj_cell->f_cost = adj_cell->g_cost + adj_cell->h_cost;

					open.insert(adj_cell);
				}
			}
		}// end for
	
	}// end wile
	if (path == nullptr) {
		std::cout << "Path not found !" << std::endl;
	}
	else {
		mymaze.path.clear();
		int prev_x = path->x, prev_y = path->y;
		path = path->parent;
		while (path != nullptr) {
			mymaze.path.push_back(data::edge(path->x, path->y, prev_x, prev_y));
			prev_x = path->x;
			prev_y = path->y;
			path = path->parent;
		}
	}
}