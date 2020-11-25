#include "gamestate.hpp"

Gamestate::Gamestate(){

    for(int i = 0; i < 8; i++){

        log[i] = "";
    }

    current_state.player_x = 2;
    current_state.player_y = 2;
}

void Gamestate::input_enqueue(Input event){

    if(input_queue.size() < 5){

        input_queue.push(event);
    }
}

void Gamestate::update(float delta){

    if(shift_timer != 0){

        shift_timer -= delta * (1 + input_queue.size());
        if(shift_timer <= 0){

            shift_timer = 0;
            current_state = next_state;

        }else{

            return;
        }
    }

    if(!input_queue.empty()){

        Input input = input_queue.front();
        input_queue.pop();
        process_turn(input);
    }
}

void Gamestate::process_turn(Input input){

    next_state = current_state;

    if(input == Input::UP){

        next_state.player_y--;
        log_message("You moved up.");

    }else if(input == Input::RIGHT){

        next_state.player_x++;
        log_message("You moved right.");

    }else if(input == Input::DOWN){

        next_state.player_y++;
        log_message("You moved down.");

    }else if(input == Input::LEFT){

        next_state.player_x--;
        log_message("You moved left.");
    }

    shift_timer = SHIFT_DURATION;
}

void Gamestate::log_message(std::string message){

    if(log_head == 0){

        log_head = 7;

    }else{

        log_head--;
    }

    log[log_head] = message;
}

int Gamestate::interpolate_value(int current_value, int next_value){

    int diff = next_value - current_value;
    float shift_percent = 1 - (shift_timer / SHIFT_DURATION);
    return (int)(current_value + (diff * shift_percent ));
}

Gamestate::State Gamestate::magnify(State state){

    State return_state = state;

    return_state.player_x *= 36;
    return_state.player_y *= 36;

    return return_state;
}

Gamestate::State Gamestate::get_state(){

    State current = magnify(current_state);
    State next = magnify(next_state);

    if(shift_timer != 0){

        current.player_x = interpolate_value(current.player_x, next.player_x);
        current.player_y = interpolate_value(current.player_y, next.player_y);
    }

    return current;
}
