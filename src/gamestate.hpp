#pragma once

#include <string>
#include <queue>

class Gamestate{

    public:
        typedef struct{
            int player_x;
            int player_y;
        } State;
        typedef enum Input{
            NOTHING,
            UP,
            RIGHT,
            DOWN,
            LEFT
        } Input;

        Gamestate();
        Gamestate(State initial_state);

        void input_enqueue(Input event);

        void update(float delta);
        void process_turn(Input input);

        std::string log[8];
        int log_head = 0;
        void log_message(std::string message);

        int interpolate_value(int current_value, int next_value);
        State magnify(State state);
        State get_state();

    private:
        static const int SHIFT_DURATION = 5; // Measured in deltas
        float shift_timer = 0;

        State current_state;
        State next_state;

        std::queue<Input> input_queue;
};
