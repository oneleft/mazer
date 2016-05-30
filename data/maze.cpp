#include "maze.h"

void mazer2016sp1oua::data::maze::init(void)
{
    _initialized = true;
}

std::string mazer2016sp1oua::data::dir_to_str(mazer2016sp1oua::data::direction d) {
	switch (d)
	{
	case mazer2016sp1oua::data::direction::NORTH:
		return "NORTH";
		break;
	case mazer2016sp1oua::data::direction::SOUTH:
		return "SOUTH";
		break;
	case mazer2016sp1oua::data::direction::EAST:
		return "EAST";
		break;
	case mazer2016sp1oua::data::direction::WEST:
		return "WEST";
		break;
	default:
		return "INVALID";
		break;
	}
}

/**
 * @param dir the direction we wish to reverse
 * @return the reverse direction to the one passed in
 **/
const mazer2016sp1oua::data::direction mazer2016sp1oua::data::operator!(
        const mazer2016sp1oua::data::direction  dir)
{
    //shortens our code quite a bit here
    using namespace mazer2016sp1oua::data;
    //simply return a direction that is the reverse of the one 
    //passed in
    switch(dir)
    {
        case direction::NORTH:
            return direction::SOUTH;
            break;
        case direction::SOUTH:
            return direction::NORTH;
            break;
        case direction::EAST:
            return direction::WEST;
            break;
        case direction::WEST:
            return direction::EAST;
            break;
        default:
            return direction::INVALID;
            break;
    }
}

/**
 * @param name the file name to save the binary data to
 * return true when we successfully save the maze and false if we 
 * have any i/o problems along the way.
 **/
bool mazer2016sp1oua::data::maze::save_binary(const std::string& name)
{
    int num_edges;
    int x_count, y_count, e_count;

    //open the file and check that the open was successful
    std::ofstream out(name, std::ios::binary);
    if(!out)
    {
        std::cerr << "Failed to open file " << name << std::endl;
        return false;
    }
    //enable exception handling on the stream
    out.exceptions(std::ios::badbit | std::ios::failbit);
    //clear the visited flags so we are starting our edge navigation
    //from scratch
    set_unvisited();
    num_edges = edge_count();
    int edge_counter=0;
    //reset the visited flags as counting the number of edges changes
    //them
    set_unvisited();
    try
    {
        //write out the width, height and number of edges
        out.write((char*)&_width, sizeof(int));
        out.write((char*)&_height, sizeof(int));
        out.write((char*)&num_edges, sizeof(int));

        //iterate over the cells vector, writing each edge to the 
        //svg file.
        for(y_count = 0; y_count < _height; ++y_count)
        {
            for(x_count = 0; x_count < _width; ++x_count)
            {
                cell & c = _cells[y_count][x_count];
                for(e_count = 0; e_count < num_dirs; ++e_count)
                {
                    edge & e = c.adjacents[e_count];
                    //if this is a valid edge and has not been visited, 
                    //we write it out
                    if(valid_edge(e) && !e.visited)
                    {
                        out.write((char*)&e.in_x, sizeof(int));
                        out.write((char*)&e.in_y, sizeof(int));
                        out.write((char*)&e.out_x, sizeof(int));
                        out.write((char*)&e.out_y, sizeof(int));
                        e.visited = true;
                        if(e.out_y < 0 || e.out_y > int(_height)) 
                            continue;
                        if(e.out_x < 0 || e.out_x > int(_width))
                            continue;
                        ++edge_counter;
                        //grab the reverse edge and also set it 
                        //to being visited
                        cell & other = _cells[e.out_y][e.out_x];
                        edge & e = 
                            other.adjacents[int(!direction(e_count))];
                        e.visited = true;
                    }
                }
            }
        }
    }
    catch(const std::ios_base::failure& f)
    {
        //exception occured so output it and return false
        std::cerr<< f.what() << std::endl;
        return false;
    }
    //have we written the number of edges we expected to? If we 
    //haven't, display an error message and return false
    if(num_edges != edge_counter)
    {
        std::cerr << "Error: incorrect number of edges written." <<
            std::endl;
        std::cerr << "Expected: " << num_edges << " Actual: " 
            << edge_counter << std::endl;
        return false;
    }
    //this is a successful run so return true
    return true;
}

/**
 * @param name the file name to save binary file to
 * @return true if the save was successful and false otherwise
 **/
bool mazer2016sp1oua::data::maze::load_binary(const std::string& name)
{
    int num_edges, cur_edges=0, y_count;

    //open the file and check that the open is successful
    std::ifstream in(name);
    if(!in)
    {
        std::cerr << "oh no - there was an error opening binary file" 
            << " for reading." << std::endl;
        return false;
    }
    //enable exception handling on the stream
    in.exceptions(std::ios::badbit | std::ios::failbit);
    //set all the edges in the maze to not visited as we are starting
    //from scratch
    set_unvisited();
    try
    {
        //read in the width and height and count of edges
        in.read((char*)&_width, sizeof(int));
        in.read((char*)&_height, sizeof(int));
        //get the number of edges from the file
        in.read((char*)&num_edges, sizeof(int));
        if(!valid_dim(_width) || !valid_dim(_height))
        {
            throw args::action_failed("invalid dimensions specified for "
                "the maze in the binary file");
        }
        //resize the width and height according to the numbers read in
        _cells.resize(_height);
        for(y_count=0; y_count < _height; ++y_count)
        {
            _cells[y_count].resize(_width);
        }

        //set the maze to be initialized so it can be written out if 
        //requested
        init();
        //read in edges from the file. We could read them in as a set of 4 
        //using sizeof(edge) as well. There would be no difference in the
        //runtime as c++ uses buffered i/o by default.
        while(in)
        {
            edge e;
            in.read((char*)&e.in_x, sizeof(int));
            in.read((char*)&e.in_y, sizeof(int));
            in.read((char*)&e.out_x, sizeof(int));
            in.read((char*)&e.out_y, sizeof(int));
            if(!valid_edge(e))
            {
                std::cerr << "Error: an invalid edge was read." << std::endl;
                return false;
            }
            //get the direction of this edge (north, south, east or west)
            //as well as its reverse direction then store this edge as well
            int in_x = e.in_x, in_y = e.in_y, out_x = e.out_x, 
                out_y = e.out_y;
            direction dir = get_dir(in_x,in_y,out_x,out_y);
            if(dir == direction::INVALID)
            {
                std::cerr << "Error: there was an edge in an invalid "
                    << "direction" << std::endl;
                return false;
            }
            direction rev = !dir;
            e.visited=false;
            _cells[in_y][in_x].adjacents[int(dir)] = e;
            edge other = {out_x,out_y,in_x,in_y};
            other.visited=false;
            _cells[out_y][out_x].adjacents[int(rev)] = other;
            //we increment the count of edges for validation
            ++cur_edges;
        }
    }
    catch(const std::ios_base::failure & f)
    {
        //we have reached the end of the file
        if(in.rdstate() & std::ifstream::eofbit)
        {
            //validate that there were exactly the number of edges specified
            if(cur_edges != num_edges)
            {
                std::ostringstream oss;
                oss << "Error: the number of edges don't match what was "
                    << "specified in the file. " << std::endl;
                throw args::action_failed(oss.str());
            }
            return true;
        }
        //there was an io error so display the exception message and return
        //false
        std::cerr << f.what() << std::endl;
        return false;
    }

    //everything has worked so return true
    return true;
}

/**
 * @param e the edge to validate
 **/
bool mazer2016sp1oua::data::maze::valid_edge(const edge& e) const
{
    //check that each component of the edge is valid
    //within the current maze
    if(e.in_x >= int(_width) || e.in_x < 0)
        return false;
    if(e.in_y >= int(_height) || e.in_y < 0)
        return false;
    if(e.out_x >= int(_width) || e.out_x < 0)
        return false;
    if(e.out_y >= int(_height) || e.out_y < 0)
        return false;
    if(e.in_x == e.out_x && e.in_y == e.out_y)
        return false;
    return true;
}

/**
 * @param name the name of the svg file to save
 * @return true when the file is successfully saved and false otherwise
 **/
bool mazer2016sp1oua::data::maze::save_svg(const std::string& name) 
{
    //magnification changes with the size of the maze so we roughly
    //get an svg file that displays at the same size
	double magnification = MAXRES /
		(double)std::max(_width-1,_height-1) ;
    int x,y;
    //open the file and validate it was successful
    std::ofstream out(name);
    if(!out)
        return false;
    //enable exception handling on the stream
    out.exceptions(std::ios::badbit | std::ios::failbit);
    //reset the visited status of each edge so we only write each 
    //bidirectional edge once
    set_unvisited();

    try
    {
        //write the prologue for the svg file and the background
        save_svg_prologue(out, magnification);
        save_svg_background(out, magnification);
        //iterate over each cell of the maze. If it is a valid edge, 
        //save it and set its visited status as well as setting the 
        //visited status of the opposite edge
        for(y=0; y<_height; ++y)
        {
            for(x=0; x<_width; ++x)
            {
                cell c = _cells[y][x];
                for(int e_count = 0; e_count < num_dirs; ++e_count)
                {
                    edge e = c.adjacents[e_count];
                    if(!valid_edge(e))
                        continue;
					save_svg_edge(out, e, magnification);
					set_visited(e, e_count);           
                }
            }
        }

		out << "</g>" << std::endl;
		// Draw path
		if (path.size()) {
			out << "<polyline fill=\"none\" stroke=\"#ff0000\" stroke-width=\"2\" stroke-linecap=\"square\" stroke-linejoin=\"round\" points=\"";
			edge e;
			for (size_t p = 0; p < path.size(); ++p)
			{
				e = path[p];
				out << e.out_x * magnification + constants::OFFSET_X + 4 * magnification / 8
					<< ","
					<< e.out_y * magnification + constants::OFFSET_Y + 4 * magnification / 8
					<< " ";
			}
			e.out_x = 0;
			e.out_y = 0;
			out << e.out_x * magnification + constants::OFFSET_X + 4 * magnification / 8
				<< ","
				<< e.out_y * magnification + constants::OFFSET_Y + 4 * magnification / 8
				<< " ";
			out << "\" />" << std::endl;
		}

        //write the closing tag for the svg file
        save_svg_epilogue(out);
    }
    catch(std::ios_base::failure& f)
    {
        //there was an io error so display an error message and return 
        //false
        std::cerr << f.what() << std::endl;
        return false;
    }
    return out.good();
}

void mazer2016sp1oua::data::maze::set_unvisited(void)
{
    //iterate over each cell
    for(int y_count = 0; y_count < _height; ++y_count)
    {
        for(int x_count = 0; x_count < _width; ++x_count)
        {
            //set all of its edges to not been visited
            for(int e_count = 0; e_count < num_dirs; ++e_count)
            {
                _cells[y_count][x_count].adjacents[e_count].visited = false;
            }
        }
    }
}


int mazer2016sp1oua::data::maze::edge_count(void)
{
    int edge_counter = 0;
    //iterate over the cells vector
    for(int y_count = 0; y_count < _height; ++y_count)
    {
        for(int x_count = 0; x_count < _width; ++x_count)
        {
            //for each edge going out from this cell, set it to
            //visited and increment the count of edges
            for(int e_count = 0; e_count < num_dirs; ++e_count)
            {
                cell & c = _cells[y_count][x_count];
                edge & e = c.adjacents[e_count];
                if(valid_edge(e) && !e.visited)
                {
                    set_visited(e, e_count);
                    ++edge_counter;
                }
            }
        }
    }
    return edge_counter;
}

/**
 * @param in_x the x coordinate of the current cell
 * @param in_y the y coordinate of the current cell
 * @param out_x the destination x coordinate
 * @param out_y the destination y coordinate
 * @return the direction from the incoming cell to the outgoing 
 * cell.
 **/
mazer2016sp1oua::data::direction mazer2016sp1oua::data::get_dir(const int in_x, 
        const int in_y, const int out_x, const int out_y)
{
    using namespace mazer2016sp1oua::data;
    int diff_x = in_x - out_x;
    int diff_y = in_y - out_y;
    //the cells should be just one cell away from each other in either a
    //horizontal or vertical direction
    if(diff_x != 0 && diff_y != 0)
    {
        std::cerr << "error: invalid coordinates given" << std::endl;
        return direction::INVALID;
    }
    else
    {
        //check that the coordinates are one space apart
        if(std::abs(diff_x) != 1 && std::abs(diff_y) != 1)
        {
            std::cerr << "error: invalid coordinates given" << std::endl;
        }
        //calculate direction based on difference between the coordinates
        //passed in
        if(diff_x == 0)
        {
            if(diff_y == -1)
                return direction::NORTH;
            else
                return direction::SOUTH;
        }
        else
        {
            if(diff_x == -1)
                return direction::WEST;
            else
                return direction::EAST;
        }
    }
    return direction::INVALID;
}

/**
 * @param out the stream to write the svg prologue to
 * @param magnification how big is the svg maze to be displayed
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2016sp1oua::data::maze::save_svg_prologue(std::ofstream& out, 
        double magnification)
{
    out << "<svg width=\"";
    out << std::round((_width) * magnification) + constants::OFFSET_X << "\" height=\"" <<
        std::round((_height) * magnification) + constants::OFFSET_Y << "\" xmlns=\"";
    out << "http://www.w3.org/2000/svg\" >" << std::endl;
}

/**
 * @param out the stream to write the svg data to
 * return true when there has been no i/o error and false otherwise
 **/
void mazer2016sp1oua::data::maze::save_svg_epilogue(std::ofstream& out)
{
	//points="154,2 154,10 138,10 138,42 170,42 170,58 154,58 154,74 122,74 122,58 106,58 106,26 90,26 90,42 74,42 74,26 10,26 10,42 26,42 26,74 10,74 10,106 26,106 26,122 10,122 10,154 26,154 26,186 10,186 10,202 42,202 42,234 26,234 26,218 10,218 10,266 26,266 26,250 42,250 42,282 26,282 26,314 74,314 74,234 90,234 90,218 106,218 106,234 138,234 138,250 154,250 154,266 90,266 90,298 138,298 138,314 170,314 170,322" />

	out << "</svg>" << std::endl;
}


/**
 * @param out the output stream we are writing to
 * @param e the edge data to write
 * @param magnification how big do we want to draw each edge
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2016sp1oua::data::maze::save_svg_edge(std::ofstream & out, 
        const mazer2016sp1oua::data::edge& e, double magnification)
{
	
	//if (!e.path_part) {
		out << "<line shape-rendering=\"crispEdges\"  stroke=\"white\" x1=\"";
	/*}
	else {
		out << "<line shape-rendering=\"crispEdges\" stroke=\"red\" x1=\"";
		std::cout << "path part: ";
	}*/
	//'shape-rendering', 'crispEdges')
	
	out
		<< e.in_x * magnification + constants::OFFSET_X + 4 * magnification / 8
		<< "\" x2=\"" << e.out_x * magnification + constants::OFFSET_X + 4 * magnification / 8
		<< "\" y1=\"" << e.in_y * magnification  + constants::OFFSET_Y + 4 * magnification / 8
		<< "\" y2=\"" << e.out_y * magnification + constants::OFFSET_Y + 4 * magnification / 8
		<< "\" />"
        //<< "\" stroke-width=\"" << magnification / 2.0 << "\" />"
        << std::endl;
}

/**
 * @param out the stream to write svg data to
 * @param magnification how big do we want to draw each stroke
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2016sp1oua::data::maze::save_svg_background(std::ofstream & out, 
        double magnification)
{
	out << "<rect width=\"" << _width * magnification <<
		"\" height=\"" << _height * magnification <<
		"\" style=\"fill:black\" x=\"" << constants::OFFSET_X << "\" y=\"" << constants::OFFSET_Y << "\" />"
		<< std::endl;
	out << "<g fill=\"none\" stroke=\"#000000\" stroke-width=\""
		<< magnification / 2 << "\" stroke-linecap=\"square\">" << std::endl;
    
}

/**
 * @param e the edge we wish to set as visited
 * @param e_index the index into the cell's edges that represents
 * the direction of the edge
 **/
void mazer2016sp1oua::data::maze::set_visited(edge& e, int e_index)
{
    //if this is a valid edge that we haven't visited, mark it so
    if(valid_edge(e) && !e.visited)
    {
        //grab the opposing edge
        cell & other_c = _cells[e.out_y][e.out_x];
        edge & other_e 
            = other_c.adjacents[int(!direction(e_index))];
        //set them both to visited
        other_e.visited = e.visited =  true;
    }
}

/**
 * @param dim the dimension to test
 **/
bool mazer2016sp1oua::data::maze::valid_dim(const int dim)
{
    if(dim < constants::MIN_DIM || dim > constants::MAX_DIM)
    {
        return false;
    }
    return true;
}

/**
 * @param e the edge we wish to reverse
 * @return a reversed edge where the in coordinates are now the out 
 * coordinates and vice versa
 **/
    mazer2016sp1oua::data::edge 
mazer2016sp1oua::data::operator!(const mazer2016sp1oua::data::edge& e)
{
    edge new_e;
    new_e.out_x = e.in_x;
    new_e.in_x = e.out_x;
    new_e.in_y = e.out_y;
    new_e.out_y = e.in_y;
    return new_e;
}

/**
 * @param orient the orientation we wish to reverse
 * @return the reverse orientation to the one passed in so if we got 
 * horizontal we should return vertical and vice-versa
 **/
mazer2016sp1oua::data::orientation 
mazer2016sp1oua::data::operator !( 
        const mazer2016sp1oua::data::orientation orient)
{
    int intorient = int(orient);
    intorient++;
    return orientation(intorient%NUM_ORIENTATIONS);
}

std::ostream &
mazer2016sp1oua::data::operator<<(
				  std::ostream& out,
				  const mazer2016sp1oua::data::edge& the_edge)
{
  return out << "(" << the_edge.in_x << ", " << the_edge.in_y << ") -> "
      << "(" << the_edge.out_x << ", " << the_edge.out_y << ")";
}

void mazer2016sp1oua::data::maze::clear(void)
{
    int w_count, h_count;
    
    for(h_count = 0; h_count < _height; ++h_count)
    {
	for(w_count = 0; w_count < _width; ++w_count)
	{
	    auto & cell = _cells[h_count][w_count];
	    cell.x = w_count;
	    cell.y = h_count;
	}
    }
}

std::ostream& mazer2016sp1oua::data::operator<<(std::ostream& out,
				  mazer2016sp1oua::data::maze const& m)
{
    auto & cells = m._cells;
    int x_count,y_count,e_count;
    for(y_count = 0; y_count < m._height; ++y_count)
    {
	for(x_count = 0; x_count < m._width; ++x_count)
	{
	    const cell & cur_cell = cells[y_count][x_count];
	    out << "(" <<cur_cell.x << ", " << cur_cell.y << ") : " ;
	    
	    for(e_count = 0; e_count < num_dirs; ++e_count)
	    {
		const edge & e = cur_cell.adjacents[e_count];
		if(m.valid_edge(e))
		{
		    out << "(" << e.out_x << ", " << e.out_y << "), ";
		}
	    }
	    out << std::endl;
	}
    }
    return out;
}

