#include "arg_processor.h"
//array of valid argument strings that can be passed in from the 
//command line
const std::string mazer2016sp1oua::args::arg_processor::arg_strings
[mazer2016sp1oua::args::arg_processor::NUM_OPTIONS]
= {
	"--gb", "--gw", "--gs", 
	"--pd", "--pb", "--pm", "--pe",
	"--sv", "--sb", "--lb"};

/**
 * constructor - simply copies the arguments passed in from the command line
 * into a vector for later processing
 **/
mazer2016sp1oua::args::arg_processor::arg_processor( int argc, char*argv[])
{
    int count;
    for(count = 1; count < argc; ++count)
    {
        arguments.push_back(argv[count]);
    }
}

/**
 * processes all command line arguments that were passed into the 
 * constructor
 **/
    std::vector <std::unique_ptr < mazer2016sp1oua::args::action > > 
mazer2016sp1oua::args::arg_processor::process(void)
{
    //count of the arguments that have been processed
    int arg_count, 
        //count of the option values we have iterated over 
        opt_count;
    int size = arguments.size();
	std::vector < std::unique_ptr < args::action > > last_actions;
    //iterate over each argument in the arguments vector
    for(arg_count = 0; arg_count < size; ++arg_count)
    {
        bool processed;
        processed=false;
        //iterate over each option in the arg_strings vector
        for(opt_count = 0; opt_count < NUM_OPTIONS; ++opt_count)
        {
            //if they are the same
            if(arguments[arg_count] == arg_strings[opt_count])
            {
                processed=true;
                //increment the count of arguments as we have just 
                //processed one.
                ++arg_count;
                //which option type are we processing?
                option_type type = option_type(opt_count);
                //if we are not generating, there should be one argument
                //for the option
                if(type != option_type::GENBTREE 
                    && type != option_type::GENWILSON
					&& type != option_type::GENSIDEWINDER
					&& type != option_type::SOLVE_DIJKSTRA
					&& type != option_type::SOLVE_BFS
					&& type != option_type::SOLVE_A_MANHATTAN
					&& type != option_type::SOLVE_A_EUCLIDE)
                {
                    int distance = find_next_option(arguments, arg_count);
                    if(distance != ONE_ARGUMENT)
                    {
                        std::ostringstream oss;
                        oss << "Error: You did not specify correct "
                            << "arguments for the " 
                            << arguments[arg_count-1] << " action."
                            << std::endl;
                        throw action_failed(oss.str());
                    }
                }
                std::unique_ptr<action> newact;
                switch (type)
                {
                    case option_type::GENBTREE:
                        {
                            //a generate action was requested - we 
                            //implement this processing in a different
                            //function
                            newact=process_generate_argument(arg_count,
                                    false, args::generator_type::BTREE);
                            //need to move the unique_ptr onto the vector
                            //as it is a move only type
                            actions.push_back(std::move(newact));
                        }
                        break;
                    case option_type::GENWILSON:
                        {
                            //a generate action using the stack method
                            //has been requested
                            newact = process_generate_argument(arg_count,
                                    true, args::generator_type::WILSON);
                            actions.push_back(std::move(newact));
                            break;
                        }
					case option_type::GENSIDEWINDER:
						{
							newact = process_generate_argument(arg_count,
								true, args::generator_type::SIDEWINDER);
							actions.push_back(std::move(newact));
							break;
						}
					case option_type::SOLVE_DIJKSTRA:
						{
							std::unique_ptr<action> newact
								= std::make_unique<pathfind_action>(pathfind_type::DIJKSTRA);
							actions.push_back(std::move(newact));
							--arg_count;
							break;
						}
					case option_type::SOLVE_BFS:
					{
						std::unique_ptr<action> newact
							= std::make_unique<pathfind_action>(pathfind_type::BREADTH_FIRST_SEARCH);
						actions.push_back(std::move(newact));
						--arg_count;
						break;
					}
					case option_type::SOLVE_A_MANHATTAN:
					{
						std::unique_ptr<action> newact
							= std::make_unique<pathfind_action>(pathfind_type::A_STAR_MANHATTAN);
						actions.push_back(std::move(newact));
						--arg_count;
						break;
					}
					case option_type::SOLVE_A_EUCLIDE:
					{
						std::unique_ptr<action> newact
							= std::make_unique<pathfind_action>(pathfind_type::A_STAR_EUCLIDEAN);
						actions.push_back(std::move(newact));
						--arg_count;
						break;
					}
                    case option_type::SAVE_VECTOR:
                        {
                            std::string name = arguments[arg_count];
                            if(name.size() < EXTLEN 
                                    || (name.compare(name.size()-EXTLEN,
                                            EXTLEN, ".svg") 
                                        && name.compare(name.size()-EXTLEN,
                                            EXTLEN,".SVG")))
                            {
                                throw action_failed("svg files must have "
                                        "a .svg extension");
                            }
                            //create a save action that will store
                            //this request
                            std::unique_ptr<action> newact 
                                = std::make_unique<save_action>
                                (save_type::SVG, arguments[arg_count]);

							last_actions.push_back(std::move(newact));
                            //actions.push_back(std::move(newact));
                        }
                        break;
                    case option_type::SAVE_BINARY:
                        {
                            //create a save action that will store this
                            //request
                            std::unique_ptr<action> newact
                                = std::make_unique<save_action>
                                (save_type::BINARY, arguments[arg_count]);

							last_actions.push_back(std::move(newact));
                            //actions.push_back(std::move(newact));
                        }
                        break;
                    case option_type::LOAD_BINARY:
                        {
                            //create a load action that will store
                            //this request
                            std::unique_ptr<action> newact
                                = std::make_unique<load_action>
                                (arguments[arg_count]);
                            actions.push_back(std::move(newact));
                        }
                        break;
                }
                break;
            }
            //test whether we have processed the current argument
            //if we have not then it is an invalid argument
            if(arg_count >= size-1 && processed)
                break;
        }
        if(!processed)
            throw action_failed("invalid argument passed in from the"
                    " command line");
    }

	for (auto act = 0; act < last_actions.size(); ++act) {
		actions.push_back(std::move(last_actions[act]));
	}

    return std::move(actions);
}

    std::string mazer2016sp1oua::args::option_string
(mazer2016sp1oua::args::option_type type)
{
    switch(type)
    {
        case option_type::GENBTREE:
            return "generate using the binary tree method";
            break;
        case option_type::GENWILSON:
            return "generate using Wilsons�s algorithm";
            break;
		case option_type::GENSIDEWINDER:
			return "generate using the sidewinder algorithm";
			break;

		case option_type::SOLVE_DIJKSTRA:
			return "solve using Dijkstra�s algorithm";
			break;
		case option_type::SOLVE_BFS:
			return "solve using Breadth First Search";
			break;
		case option_type::SOLVE_A_MANHATTAN:
			return "solve using a* search with the manhattan distance heuristic";
			break;
		case option_type::SOLVE_A_EUCLIDE:
			return "solve using a* search with the euclidean distance heuristic";
			break;

        case option_type::SAVE_VECTOR:
            return "save vector";
            break;
        case option_type::SAVE_BINARY:
            return "save binary";
            break;
        case option_type::LOAD_BINARY:
            return "load binary";
            break;
    }
    return "Invalid";
}

/**
 * finds the next option argument (one with --) for processing in order
 * to evaluate variable arguments for an option
 **/
int mazer2016sp1oua::args::arg_processor::find_next_option(
        const std::vector<std::string> args, const int cur_arg)
{
    //store the index of the current argument
    int count = cur_arg;
    //constant for telling string comparison to compare the first two
    //characters
    const int FIRSTTWO=2;
    //iterate over the args vector starting at cur_arg until we find
    //an option argument
    for(auto it = args.begin()+cur_arg; it != args.end(); ++it, ++count)
    {
        //is this an option argument?
        if(it->compare(0,FIRSTTWO,"--") == 0)
            return count - cur_arg;
    }
    //if we have reached the end of the vector, just return a count of 
    //how far we have gotten
    return count - cur_arg;
}


bool mazer2016sp1oua::args::arg_processor::valid_dim(int val)
{
    if(val < MINDIM || val > MAXDIM)
        return false;
    return true;
}


/**
 * handles the processing of a generate argument. Moved into a separate
 * function as it is a fairly complex process that was better moved to a 
 * separate function
 **/
    std::unique_ptr<mazer2016sp1oua::args::action> 
mazer2016sp1oua::args::arg_processor::process_generate_argument(int& arg_count,
        bool use_stack,
		args::generator_type gen_type)
{
    //this function returns a std::unique_ptr so that the memory will be
    //freed when it goes out of scope
    std::unique_ptr<action>newact;

    //as generate actions require a variable number
    //of arguments, we need to see how many 
    //arguments there are and this will be 
    //calculated in the find_next_option() function
    int distance = 
        find_next_option(arguments,arg_count);
    int width, height;
    int seed;
    //different distances indicate different types of generate requests
    generate_type type=generate_type(distance);
    switch(type)
    {
        case generate_type::DEFAULT:
            {
                //set all values to default values as none were specified
                seed=constants::ERROR;
                width=constants::DEFAULT_WIDTH;
                height=constants::DEFAULT_HEIGHT;
                break;
            }
        case generate_type::NODIMS:
            {
                width=constants::DEFAULT_WIDTH;
                height=constants::DEFAULT_HEIGHT;
                try
                {
                    seed = stoi(arguments[arg_count]);
                }
                catch(std::invalid_argument& inval)
                {
                    std::ostringstream oss;
                    oss << "You specified an invalid seed value" 
                        << std::endl;
                    throw action_failed(oss.str());
                }

                break;
            }
        case generate_type::NOSEED:
            {
                //no seed was provided so we will 
                //have to generate one later
                seed = constants::ERROR;
                try
                {
                    //two arguments left to process
                    //let's do that.
                    width = 
                        stoi(arguments[arg_count++]);
                    height = 
                        stoi(arguments[arg_count]);
                }
                catch(std::invalid_argument & inval)
                {
                    //validate that actual integers were passed in
                    std::ostringstream oss;
                    oss << "You did not pass in valid integers for " 
                        << "generation of a maze" << std::endl;
                    throw(action_failed(oss.str()));
                }
                //we need to validate that these are 
                //values within the range of valid widths 
                // / heights
                if(!valid_dim(width) || !valid_dim(height))
                {
                    std::string message = 
                        "the dimension provided for a maze"
                        " are out of range";
                    throw action_failed(message);
                }
                break;
            }
        case generate_type::ALLSPEC:
            {

                //in this case, the seed has been provided
                //so we will capture it along with the 
                //width and height
                try
                {

                    seed = stoi(arguments[arg_count++]);
                    width = stoi(arguments[arg_count++]);
                    height = stoi(arguments[arg_count]);
                }
                catch(std::invalid_argument & inval)
                {
                    //oops we got some invalid input so let's deal 
                    //with that
                    std::ostringstream oss;
                    oss << "You did not pass in valid integers into "
                        << "a generation request." << std::endl;
                    throw action_failed(oss.str());
                }
                break;
            }
        default:
            {
                //an invalid action was requested - due to how this
                //function has been written, this should never be run -
                //it is mainly here to remove a compiler warning
                throw action_failed("invalid generation request");
            }
    }
    newact=std::make_unique<generate_action>(seed, width, height, 
		gen_type,
		use_stack);
    return std::move(newact);
}
