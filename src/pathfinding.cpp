#include "pathfinding.hpp"

#include <iostream>
#include <vector>

int direction_vector[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

int manhatten_distance(int ax, int ay, int bx, int by){

    return abs(bx - ax) + abs(by - ay);
}

int pathfind(int start_x, int start_y, int goal_x, int goal_y, bool* map, int map_width, int map_height){

    typedef struct{

        int direction;
        int path_length;
        int x;
        int y;
        int score;
    } Node;


    std::vector<Node> frontier;
    std::vector<Node> explored;

    frontier.push_back((Node){
        .direction = -1,
        .path_length = 0,
        .x = start_x,
        .y = start_y,
        .score = manhatten_distance(start_x, start_y, goal_x, goal_y)
    });

    while(true){

        // Check that the frontier isn't empty
        if(frontier.size() == 0){

            std::cout << "Pathfinding failed!" << std::endl;
            return -1;
        }

        // Find the smallest node in the frontier
        int smallest_index = 0;
        for(unsigned int i = 1; i < frontier.size(); i++){

            if(frontier.at(i).score < frontier.at(smallest_index).score){

                smallest_index = i;
            }
        }

        // Remove it from the frontier
        Node smallest = frontier.at(smallest_index);
        frontier.erase(frontier.begin() + smallest_index);

        // Check if it's the solution
        if(smallest.x == goal_x && smallest.y == goal_y){

            // If it is, return the first move in that path direction
            return smallest.direction;
        }

        // Add it to explored
        explored.push_back(smallest);

        // Expand out all possible paths based on the one we've chosen
        for(int direction = 0; direction < 4; direction++){

            int child_x = smallest.x + direction_vector[direction][0];
            int child_y = smallest.y + direction_vector[direction][1];

            // If out of bounds or a collision, ignore child
            if(child_x < 0 || child_x >= map_width || child_y < 0 || child_y >= map_height || map[child_x + (child_y * map_height)]){

                continue;
            }

            // Create the child
            int first_direction;
            if(smallest.direction == -1){

                first_direction = direction;

            }else{

                first_direction = smallest.direction;
            }
            int path_length = smallest.path_length + 1;
            Node child = (Node){
                .direction = first_direction,
                .path_length = path_length,
                .x = child_x,
                .y = child_y,
                .score = path_length + manhatten_distance(child_x, child_y, goal_x, goal_y)
            };

            // Ignore this child if it's in explored
            bool child_in_explored = false;
            for(unsigned int i = 0; i < explored.size(); i++){

                if(child.x == explored.at(i).x && child.y == explored.at(i).y){

                    child_in_explored = true;
                    break;
                }
            }
            if(child_in_explored){

                continue;
            }

            // Ignore this child if it's in frontier
            int frontier_index = -1;
            for(unsigned int i = 0; i < frontier.size(); i++){

                if(child.x == frontier.at(i).x && child.y == frontier.at(i).y){

                    frontier_index = i;
                    break;
                }
            }
            if(frontier_index != -1){

                if(child.score < frontier.at(frontier_index).score){

                    frontier.at(frontier_index) = child;
                }
                continue;
            }

            // Finally if a child is neither in frontier nor explored, add it to the frontier
            frontier.push_back(child);
        }
    }
}

void quicksort(int** array, int low, int high){

    if(low < high){

        int pi = partition(array, low, high);

        quicksort(array, low, pi - 1);
        quicksort(array, pi + 1, high);
    }
}

int partition(int** array, int low, int high){

    int pivot = array[high][1];
    int i = low - 1;

    for(int j = low; j <= high - 1; j++){

        if(array[j][1] < pivot){

            i++;
            int temp_key = array[i][0];
            int temp_val = array[i][1];
            array[i][0] = array[j][0];
            array[i][1] = array[j][1];
            array[j][0] = temp_key;
            array[j][1] = temp_val;
        }
    }

    int temp_key = array[i + 1][0];
    int temp_val = array[i + 1][1];
    array[i + 1][0] = array[high][0];
    array[i + 1][1] = array[high][1];
    array[high][0] = temp_key;
    array[high][1] = temp_val;

    return i + 1;
}
