#pragma once

extern int direction_vector[4][2];
int pathfind(int start_x, int start_y, int goal_x, int goal_y, bool** map, int map_width, int map_height);
