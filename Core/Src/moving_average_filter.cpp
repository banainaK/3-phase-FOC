#include "moving_average_filter.hpp"

MovingAverageFilter::MovingAverageFilter(): index(0) {
    index = 0;
    // initialize with 0
    for (int i = 0; i < MAX_DATA_POINTS; i++) {
        values[i] = 0;
    }
}
void MovingAverageFilter::process(float value) {
    sum = 0.0;
    values[index] = value;
    index = (index + 1) % MAX_DATA_POINTS;
    // overwrite initial values once filled up

    for (int i = 0; i < MAX_DATA_POINTS; ++i) {
        sum += values[i];
    }
    average = sum / MAX_DATA_POINTS;
}