#include "counter.h"

int8_t count = 0;

#if defined(USE_TWO_ROI)
#define OUT         0
#define FRONT       1
#define DOOR        2
#define BACK        3
#define FRONT_ROI   0
#define BACK_ROI    1

static int8_t state = OUT;
static int8_t movement[4] = {OUT, OUT, OUT, OUT};
static int8_t movement_index = 0;

void log_movement(int16_t* distances) {
    if (movement[0] == 0 && movement[1] == 0 && movement[2] == 0 && movement[3] == 0) return;

    printf("index: %d -", movement_index);
    for (int8_t i = 0; i < 4; i++)
    {
        printf(" %d", movement[i]);
    }
    printf(" - distances: %d %d\n", distances[0], distances[1]);
}

void check_count(int16_t* distances) {
    if (distances[BACK_ROI] >= DISTANCE_THRESHOLD && distances[FRONT_ROI] < DISTANCE_THRESHOLD)
    {
        state = FRONT;
        if ((movement_index == 0 && movement[movement_index] == OUT)
        || (movement_index == 2 && movement[movement_index] == DOOR)) 
        {
            movement_index++;
            movement[movement_index] = state;
        }
    }
    else if (distances[FRONT_ROI] <= DISTANCE_THRESHOLD && distances[BACK_ROI] <= DISTANCE_THRESHOLD)
    {
        state = DOOR;
        if ((movement_index == 1 && movement[movement_index] == FRONT) ||
            (movement_index == 1 && movement[movement_index] == BACK)) 
        {
            movement_index++;
            movement[movement_index] = state;
        }
    }
    else if (distances[FRONT_ROI] >= DISTANCE_THRESHOLD && distances[BACK_ROI] < DISTANCE_THRESHOLD)
    {
        state = BACK;
        if ((movement_index == 0 && movement[movement_index] == OUT) ||
            (movement_index == 2 && movement[movement_index] == DOOR)) 
        {
            movement_index++;
            movement[movement_index] = state;
        }
    }
    else if (distances[FRONT_ROI] >= DISTANCE_THRESHOLD && distances[BACK_ROI] >= DISTANCE_THRESHOLD)
    {
        state = OUT;
        if ((movement_index == 3 && movement[movement_index] == FRONT) ||
            (movement_index == 3 && movement[movement_index] == BACK))
        {
            movement_index++;
            movement[movement_index] = state;
        }
        else 
        {
            movement_index = 0;
            for (int8_t i = 0; i < 4; i++)
            {
                movement[i] = 0;
            }
        }
    }
    log_movement(distances);
    if (movement_index == 4) 
    {
        if (movement[0] == OUT && movement[1] == FRONT && movement[2] == DOOR  && movement[3] == BACK)
        {
            count++;
        }
        else if (movement[0] == OUT && movement[1] == BACK && movement[2] == DOOR && movement[3] == FRONT)
        {
            count--;
        }
        for (int8_t i = 0; i < 4; i++)
        {
            movement[i] = 0;
        }
        movement_index = 0;
    }
}

int8_t get_region() {
    return state;
}

int8_t* get_movement() {
    return movement;
}
#endif

int8_t get_count() {
    return count;
}