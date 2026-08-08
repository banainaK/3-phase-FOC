
#include <cmath>
#include <vector>
#define MAX_DATA_POINTS 200

class MovingAverageFilter {
    public: 
        MovingAverageFilter();
        void process(float value);

    
    float values[MAX_DATA_POINTS];
    int index;
    float average;
    float sum;
    
};