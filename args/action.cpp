#include "action.h"
#include "../generators/generator.h"
#include "../pathfind/pathfind.h"

#include "../data/maze.h"

/**
 * @file action.cpp contains implementations of concrete child classes of 
 * the action class which represent a request to do something from the 
 * command line.
 **/

/**
 * @param m the maze data structure to initialise and write the generated
 * maze to.
 **/
    mazer2016sp1oua::data::maze& 
mazer2016sp1oua::args::generate_action::do_action(mazer2016sp1oua::data::maze& m)
{
	m.set_unvisited();
	std::shared_ptr<generators::generator> g;
	if (gen_type == generator_type::BTREE) {
		g.reset(new generators::binary_gen(m, _width, _height, _seed));
	}
	else if (gen_type == generator_type::WILSON) {
		g.reset(new generators::wilson_gen(m, _width, _height, _seed));
	}
	else if (gen_type == generator_type::SIDEWINDER) {
		g.reset(new generators::sidewinder_gen(m, _width, _height, _seed));
	}
	
	g->generate();
    
#ifdef DEBUG
    //output the adjacency lists for the maze
    std::cerr << m;
#endif //DEBUG
    return m;
}

/**
 * perform a save action - save a maze either as a binary or svg file
 **/
    mazer2016sp1oua::data::maze&
mazer2016sp1oua::args::save_action::do_action(mazer2016sp1oua::data::maze &m)
{
    /**
     * test if the maze has been initialized yet and if it has not 
     * throw an exception
     **/
    if(m.initialized())
    {
        //save a binary file
        if(_type == save_type::BINARY)
        {
            if(!m.save_binary(_name))
            {
                std::ostringstream oss;
                oss << "There was an error saving the binary file "
                    << _name << std::endl;
                throw action_failed(oss.str());
            }
        }
        else
        {
            //save an svg file
            if(!m.save_svg(_name))
            {
                std::ostringstream oss;
                oss << "There was an error saving the svg file " 
                    << _name << std::endl;
                throw action_failed(oss.str());
            }
        }
    }
    else
    {
        //the maze is not initialized
        std::ostringstream oss;
        oss << "Error: the maze is not yet initialized. " 
            << "I can't save a non-existent maze." << std::endl;
        throw action_failed(oss.str());
    }
    return m;
}

    mazer2016sp1oua::data::maze & 
mazer2016sp1oua::args::load_action::do_action(mazer2016sp1oua::data::maze & m)
{
    if(!m.load_binary(_name))
    {
        std::ostringstream oss;
        oss << "There was an error loading the binary file " 
            << _name << std::endl;
        throw action_failed(oss.str());
    }
    return m;
}


	mazer2016sp1oua::data::maze &
mazer2016sp1oua::args::pathfind_action::do_action(mazer2016sp1oua::data::maze & m)
{
	if (m.initialized())
	{
		std::shared_ptr<pathfinders::maze_sover> solver;
		if (_type == pathfind_type::DIJKSTRA) {
			solver.reset(new pathfinders::dijkstra_solver(m));
		}
		else if (_type == pathfind_type::BREADTH_FIRST_SEARCH) {
			solver.reset(new pathfinders::bfs_solver(m));
		}
		else if (_type == pathfind_type::A_STAR_MANHATTAN) {
			solver.reset(new pathfinders::a_star_solver(m, [](int start_x, int start_y, int end_x, int end_y) -> double {
				// Mahattan distance
				return abs(end_y - start_y)	+ abs(end_x - start_x);
			}));
		}
		else if (_type == pathfind_type::A_STAR_EUCLIDEAN) {
			solver.reset(new pathfinders::a_star_solver(m, [](int start_x, int start_y, int end_x, int end_y) -> double {
				// Euclidean distance
				return sqrt(pow(end_y - start_y, 2.0) + pow(end_x - start_x, 2.0));
			}));
		}

		solver->solve();
	}
	else
	{
		//the maze is not initialized
		std::ostringstream oss;
		oss << "Error: the maze is not yet initialized. "
			<< "I can't solve a non-existent maze." << std::endl;
		throw action_failed(oss.str());
	}
	return m;
}