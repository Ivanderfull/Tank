#include<SFML\Graphics.hpp>

using namespace sf;

View view;

View getPlayerCoordinateForView(float x, float y)
{
	float tempX = x; float tempY = y;

	if (x < 320) tempX = 320;
	if (y < 240) tempY = 240;
	if (x > 1710) tempX = 1710;
	if (y > 1780) tempY = 1780;

	view.setCenter(tempX + 16, tempY + 16);
	return view;
}

