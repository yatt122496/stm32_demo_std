#include "resetdisplay.h"
#include "lcd.h"

void Resetdisplay()
{
	LCD_Fill(120, lcddev.height - 1, 239, lcddev.height, WHITE);
	LCD_Fill(0, lcddev.height - 16, 119, lcddev.height, CYAN);
	LCD_Fill(120, lcddev.height - 14, 139, lcddev.height, BLUE);
  LCD_Fill(140, lcddev.height - 14, 159, lcddev.height, RED);
  LCD_Fill(160, lcddev.height - 14, 179, lcddev.height, MAGENTA);
  LCD_Fill(180, lcddev.height - 14, 199, lcddev.height, GREEN);
  LCD_Fill(200, lcddev.height - 14, 219, lcddev.height, CYAN);
  LCD_Fill(220, lcddev.height - 14, 239, lcddev.height, YELLOW);
	LCD_Fill(119, lcddev.height - 16, 119, lcddev.height, WHITE);
	BACK_COLOR=CYAN;
	LCD_Fill(0, 0, lcddev.width,lcddev.height-16, BACK_COLOR);
	FRONT_COLOR=RED;
	BACK_COLOR=YELLOW;
  LCD_ShowString(lcddev.width-8*4,0,lcddev.width,lcddev.height,16,"RST");
	BACK_COLOR=CYAN;
	LCD_ShowFontHZ(50,20,"袁明正");
	LCD_ShowString(55,49,lcddev.width,lcddev.height,24,date);
	LCD_ShowString(65,73,lcddev.width,lcddev.height,24,ti);
	LCD_ShowFontHZ(0,97,"手动");
	FRONT_COLOR=BRRED;
	LCD_ShowFontHZ(10,126,"轴一");
	LCD_ShowFontHZ(10,154,"轴二");
	FRONT_COLOR=BLACK;
	BACK_COLOR=GREEN;
	LCD_ShowString(82,130,lcddev.width,lcddev.height,16,"RUN");
	LCD_ShowString(114,130,lcddev.width,lcddev.height,16,"Straight");
	LCD_ShowString(186,130,lcddev.width,lcddev.height,16,"Back");
	LCD_ShowString(82,158,lcddev.width,lcddev.height,16,"RUN");
	LCD_ShowString(114,158,lcddev.width,lcddev.height,16,"Straight");
	LCD_ShowString(186,158,lcddev.width,lcddev.height,16,"Back");
	FRONT_COLOR=RED;
	BACK_COLOR=CYAN;
	LCD_ShowFontHZ(0,183,"自动");
	FRONT_COLOR=BLUE;
	BACK_COLOR=GREEN;
	LCD_ShowFontHZ(22,222,"方");
	LCD_ShowFontHZ(80,222,"三角");
	LCD_ShowFontHZ(170,222,"圆");
	FRONT_COLOR=RED;
	LCD_ShowString(80,290,lcddev.width,lcddev.height,24,"STOP");
	FRONT_COLOR=BLUE;
	LCD_ShowFontHZ(50,255,"速度变化");
	FRONT_COLOR=RED;
	BACK_COLOR=CYAN;
}
