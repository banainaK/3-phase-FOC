#include "utilities.h"
#include <cstdint>
#include <math.h>


uint16_t verify_park_transform(){
    struct Vec2 input;
    input.arr[0] = 1;
    input.arr[1] = 1;

    struct Vec2 expected;
    expected.arr[0] = 1;
    expected.arr[1] = -1;
    
    struct Vec2 output = park_transform(input, 90.0);
    if (round(output.arr[0]) != expected.arr[0] && round(output.arr[1]) != expected.arr[1]) {
        return 0;
    } else {
        return 1;
    }
}



