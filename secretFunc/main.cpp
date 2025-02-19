#include <math.h>


float secretFunc(float x, float y) {
	x -= 0.5f;
	y += 1.f;
	if (x == 0.0f && y == 0.0f) return 0.0f;
	else return sinf(1 / (x*x / 2.0f + y*y));;
}
