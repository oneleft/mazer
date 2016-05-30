#include <iostream>
#include <exception>
#include <sstream>
#include "../constants/constants.h"
#include <chrono>
#pragma once
namespace mazer2016sp1oua
{
    namespace data
    {
        /** 
         * forwards declaration required to avoid a recursive include
         **/
        class maze;
    }
    namespace args
    {
		enum generator_type
		{
			BTREE,
			WILSON,
			SIDEWINDER
		};
        /**
         * abstract class that represents all types of actions that can
         * be requested from the command line.
         **/
        class action
        {
            public:
                /**
                 * pure virtual function that must be implemented in each
                 * child class to describe how to perform the action 
                 * specified on a maze.
                 * @return a reference to the maze passed in.
                 **/
                virtual data::maze & 
                    do_action(data::maze &) = 0;
        };

        /**
         * exception that may be thrown from any do_action() function to
         * indicate that that action failed.
         **/
		class action_failed : public std::exception
		{
			/**
			 * message to be returned from this exception
			 **/
			std::string _message;

		public:
			/**
			 * constructor - simply assigns the message
			 **/
			action_failed(const std::string message)
				: _message(message) {}

			/**
			 * return the message to be displayed. After this,
			 * the message is no longer valid in this class as it is
			 * returned by move.
			 **/
			const char * what(void)
			{
				return std::move(_message.c_str());
			}

		};

        /**
         * represents a request to generate a maze
         **/
        class generate_action : public action
        {
			generator_type gen_type;
            /**
             *  the seed used to start the random number generator or 
             *  @ref constants::ERROR if it has not been
             *  initialized yet.
             **/
            int _seed;

            /**
             * the width of the maze to generate
             **/
            unsigned _width;

            /**
             * the height of the maze to generate
             **/
            unsigned _height;

            public: 

            /**
             * default constructor - required to insert actions into an
             * stl container.
             **/
            generate_action(void){}

            /**
             * creates a generate_action based on a width and height
             * being specified
             **/
            generate_action(unsigned width,unsigned height, generator_type gen_type)
                : gen_type(gen_type),
				_seed(std::chrono::system_clock::now()
                .time_since_epoch() / 
                std::chrono::milliseconds(1)*-1), 
                _width(width), _height(height)
            {
            }

            /**
             * creates a generate_action based on the seed, width and 
             * height being specified
             **/
            generate_action(int seed, unsigned width, unsigned height, 
				generator_type gen_type,
                    bool use_stack)
                : gen_type(gen_type),
				_seed(
                std::chrono::system_clock::now()
                .time_since_epoch() / 
                std::chrono::milliseconds(1)*-1), 
                _width(width), _height(height) 
            {
            }

            /**
             * creates a generate_action for when only the seed is 
             * specified
             **/
            generate_action(int seed) :
				gen_type(generator_type::BTREE),
				_seed(seed),
            _width(constants::DEFAULT_WIDTH), 
            _height(constants::DEFAULT_HEIGHT)
            {
            }

            virtual data::maze & do_action(data::maze &);
        };

		enum class pathfind_type {
			DIJKSTRA,
			BREADTH_FIRST_SEARCH,
			A_STAR_MANHATTAN,
			A_STAR_EUCLIDEAN
		};

		/**
		* models a solve maze request passed in from the command line
		**/
		class pathfind_action : public action
		{
			/**
			* the type of pathfind algorithm to be done
			**/
			pathfind_type _type;
		public:
			/**
			* constructor - simply assigns the type of save and the
			* file to save to
			**/
			pathfind_action(pathfind_type type)
				: _type(type)
			{
			}
			virtual data::maze & do_action(data::maze &);
		};

        /**
         * defines a load action specified from the command line
         **/
        class load_action : public action
        {
            /**
             * the file name of the file to load from disk
             **/
            std::string _name;
            public:
            /**
             * constructor - just assigns the file name; the rest of the 
             * work will be done later.
             **/
            load_action(const std::string & name) : _name(name)
            {
            }

            virtual data::maze& do_action(data::maze &);

        };

        /**
         * how do we want to save a file? Binary or Svg? 
         **/
        enum class save_type
        {
            ///save the file as binary
            BINARY, 
            ///save the file as svg
            SVG
        };

        /**
         * models a save request passed in from the command line
         **/
        class save_action : public action
        {
            /**
             * the type of saving to be done
             **/
            save_type _type;

            /**
             * the name of the file to save to
             **/
            std::string _name;

            public:
            /**
             * constructor - simply assigns the type of save and the 
             * file to save to
             **/
            save_action(save_type type, const std::string & name) 
                : _type(type), _name(name)
            {
            }
            virtual data::maze & do_action(data::maze &);
        };
    }
}

