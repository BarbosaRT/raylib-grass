#include "Utils.hpp"
float normalize(float current, float rate, float target) {
    float output = current;
    if (current > target + rate) output -= rate;
    else if (current < target - rate) output += rate;
    return output;
}