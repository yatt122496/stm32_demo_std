#include"selectcolor.h"
#include "lcd.h"
#include "touch.h"

void SelectColor(void)
{
	if(tp_dev.x[0]>220)
	{
		penColor = YELLOW;
	}
	else if(tp_dev.x[0]>200)
	{
		penColor = CYAN;
	}       
	else if(tp_dev.x[0]>180)
	{
		penColor = GREEN;
	}
	else if(tp_dev.x[0]>160)
	{
		penColor = MAGENTA;
	}
	else if(tp_dev.x[0]>140)
	{
		penColor = RED;
	}
	else if(tp_dev.x[0]>120)
	{
		penColor = BLUE;
	}
}
