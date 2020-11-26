#include "gamestate.hpp"
#include "pathfinding.hpp"

// INIT

Gamestate::Gamestate(){

    for(int i = 0; i < 8; i++){

        log[i] = "";
    }

    sprite_player_base = Sprite::GOBLIN_BASE;
    sprite_player_larm = Sprite::GOBLIN_L_UNARMED;
    sprite_player_rarm = Sprite::GOBLIN_R_UNARMED;

    player_x = 2;
    player_y = 2;

    player_health = 20;
    player_max_health = 100;

    player_low_attack = 5;
    player_high_attack = 5;
    player_defense = 5;
    player_speed = 5;

    enemy_spawn(4, 2);

    map_width = 26;
    map_height = 16;
    map = new bool*[map_width];
    for(int i = 0; i < map_width; i++){

        map[i] = new bool[map_height];
        for(int j = 0; j < map_height; j++){

            map[i][j] = false;
        }
    }
}

Gamestate::~Gamestate(){

    for(int i = 0; i < map_width; i++){

        delete map[i];
    }

    delete map;
}

// UPDATE

void Gamestate::input_enqueue(Input event){

    if(input_queue.size() < 5){

        input_queue.push(event);
    }
}

void Gamestate::update(float delta){

    if(!input_queue.empty()){

        Input input = input_queue.front();
        input_queue.pop();
        process_turn(input);
    }
}

void Gamestate::process_turn(Input input){

    // Determine player desired move
    bool move_player = false;
    int player_attacking_index = -1;
    int player_move_x = player_x;
    int player_move_y = player_y;
    if(input == Input::UP){

        player_move_y--;
        move_player = true;

    }else if(input == Input::RIGHT){

        player_move_x++;
        move_player = true;

    }else if(input == Input::DOWN){

        player_move_y++;
        move_player = true;

    }else if(input == Input::LEFT){

        player_move_x--;
        move_player = true;
    }

    if(move_player){

        for(unsigned int i = 0; i < enemy.size(); i++){

            if(enemy.at(i).x == player_move_x && enemy.at(i).y == player_move_y){

                player_attacking_index = i;
                break;
            }
        }
    }

    // Determine enemy desired move
    typedef struct{
        int x;
        int y;
        bool attacks_player;
    } EnemyTurn;

    EnemyTurn* enemy_turns = nullptr;
    if(enemy.size() != 0){

        enemy_turns = new EnemyTurn[enemy.size()];
        for(unsigned int i = 0; i < enemy.size(); i++){

            int direction = pathfind(enemy.at(i).x, enemy.at(i).y, player_x, player_y, map, map_width, map_height);
            int enemy_move_x = enemy.at(i).x + direction_vector[direction][0];
            int enemy_move_y = enemy.at(i).y + direction_vector[direction][1];
            enemy_turns[i] = (EnemyTurn){
                .x = enemy_move_x,
                .y = enemy_move_y,
                .attacks_player = enemy_move_x == player_x && enemy_move_y == player_y
            };
        }
    }

    // Sort turns by speed
    int num_turntakers = 1 + enemy.size();
    int** turns = new int*[num_turntakers];
    for(int i = 0; i < num_turntakers; i++){

        turns[i] = new int[2];
        if(i == 0){

            turns[i][0] = -1;
            turns[i][1] = player_speed;

        }else{

            turns[i][0] = i - 1;
            turns[i][1] = enemy.at(i - 1).speed;
        }
    }
    quicksort(turns, 0, num_turntakers - 1);
    int current_first = 0;
    for(int i = 1; i < num_turntakers; i++){

        if(turns[i][1] != turns[current_first][1]){

            current_first = i;
        }
        // If player is not the first of their speed group, make them the first of their speed group
        if(turns[i][0] == -1 && current_first != i){

            int temp_index = turns[i][0];
            int temp_value = turns[i][1];
            turns[i][0] = turns[current_first][0];
            turns[i][1] = turns[current_first][1];
            turns[current_first][0] = temp_index;
            turns[current_first][1] = temp_value;
            break;
        }
    }

    // Take each turn, one speed group at a time
    unsigned int i = 0;
    int current_speed_group = turns[i][1];
    int current_speed_group_start_index = i;
    bool handling_attackers = true;
    while(i < enemy.size() + 1){

        // If i refers to player
        if(turns[i][0] == -1){

            // If handling attackers and player attacking
            if(handling_attackers && player_attacking_index != -1){

                // Attempt player attack
                if(enemy.at(player_attacking_index).x == player_move_x && enemy.at(player_attacking_index).y == player_move_y){

                    log_message("You attacked the enemy");
                    enemy.at(player_attacking_index).health -= 4;

                }else{

                    log_message("You whiffed the enemy");
                }

            // If handling movers and player moving
            }else if(!handling_attackers && player_attacking_index == -1){

                // If target square unoccupied, move there
                if(enemy_occupies(player_move_x, player_move_y) == -1){

                    player_x = player_move_x;
                    player_y = player_move_y;

                // Else stop; path is blocked
                }else{

                    log_message("An enemy has blocked your path!");
                }
            }

        // Else i will refer to an enemy index
        }else{

            if(enemy_turns == nullptr){

                std::cout << "Error! Tried to handle enemy turn when enemy turns in null!" << std::endl;
                return;
            }

            // If handling attackers and enemy attacking
            int enemy_index = turns[i][0];
            if(handling_attackers && enemy_turns[enemy_index].attacks_player){

                // Attempt enemy attack
                if(player_x == enemy_turns[enemy_index].x && player_y == enemy_turns[enemy_index].y){

                    player_health -= 2;
                    if(player_health < 0){

                        player_health = 0;
                    }
                    log_message("An enemy attacked you!");

                }else{

                    log_message("An enemy whiffed you!");
                }

            // If handling movers and enemy moving
            }else if(!handling_attackers && !enemy_turns[enemy_index].attacks_player){

                // If target square unoccupied by enemy or player, move there
                if(enemy_occupies(enemy_turns[enemy_index].x, enemy_turns[enemy_index].y) == -1 && !(enemy_turns[enemy_index].x == player_x && enemy_turns[enemy_index].y == player_y)){

                    enemy.at(enemy_index).x = enemy_turns[enemy_index].x;
                    enemy.at(enemy_index).y = enemy_turns[enemy_index].y;

                }else{

                    log_message("An enemy had its path blocked!");
                }
            }
        }

        i++;
        // If we have reached the end of the current speed group
        if(i == enemy.size() + 1 || turns[i][1] != current_speed_group){

            if(handling_attackers){

                // Set to handle movers, and turn the index back to the start of the current speed group
                handling_attackers = false;
                i = current_speed_group_start_index;

            }else{

                // Set to handle attackers as we move into the next speed group
                handling_attackers = true;
            }
        }
    }

    if(player_attacking_index != -1){

        if(enemy.at(player_attacking_index).health < 0){

            log_message("You killed the enemy");
            enemy.erase(enemy.begin() + player_attacking_index);
        }
    }

    if(enemy_turns != nullptr){

        delete[] enemy_turns;
    }
}

// ENEMIES

void Gamestate::enemy_spawn(int x, int y){

    enemy.push_back((Enemy){
            .sprite = Sprite::MONSTER_SPIDER,
            .x = x,
            .y = y,
            .health = 20,
            .max_health = 20,
            .low_attack = 5,
            .high_attack = 5,
            .defense = 5,
            .speed = 5
    });
}

int Gamestate::enemy_occupies(int x, int y){

    for(unsigned int i = 0; i < enemy.size(); i++){

        if(enemy.at(i).x == x && enemy.at(i).y == y){

            return i;
        }
    }

    return -1;
}

// LOGS

void Gamestate::log_message(std::string message){

    if(log_head == 0){

        log_head = 7;

    }else{

        log_head--;
    }

    log[log_head] = message;
}
