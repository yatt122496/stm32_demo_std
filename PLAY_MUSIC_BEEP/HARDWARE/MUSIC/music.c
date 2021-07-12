#include "music.h"
#include "timer.h"

#define MUSIC_LENGTH	10
u16 tone[] = {
//  低7 1   2   3   4   5   6   7   高1 高2 高3  高4 高5 不发音
	247, 262,294,330,349,392,440,494,523,587,659,698,784,1000
};//音频数据表
u16 tone_pwm_arr[] = {
	1000,3816,3401,3030,2865,2551,2272,2024,1912,1703,1517,1432,1275,1000
};//音频数据表

static unsigned char bMusic_time[] = {		// 单位：四分之一拍 129ms
	2,2, 2,2, 4, 2,2,
//	告   别了 家  乡
	4, 2,2, 2, 6,
//  亲 切的 明 月，
	4, 2,2, 2,4,2, 16,
//  亲 切的  明     月，
	2,2,2,2, 4, 2,2,
//  迎来了   校  园
	4, 2,2, 4, 4,
//  灿 烂的 朝 阳，
	2, 4, 2, 4,2,2, 16,
//  灿 烂 的  朝     阳。
	4, 2,2, 8,
//	八 角   塘
	4,2,2, 8,
//  映     日
	4,2,2, 4,2,2,
//	荷花   别样
	4,4,8,
//  红，
	4,2,2, 4,4,
//  赣江   水
	2,2,2,2, 8,
//  源       头
	4, 4, 2,4, 2,16,
//  万 涓 泛波  浪。
	8, 6, 2,
//  走 进 这
	6,2, 8,
//  梦   想
	4, 2,2, 4,4, 16,
//  梦 想的 殿    堂，
	4, 2,2, 6, 2,
//  年 轻的 心  啊
	2,2, 2,2, 8,
//  澎湃 激   昂，
	4, 4, 4,4, 2,16,
//  澎 湃 激    昂，
	8, 6, 2,
//  我 们 从
	6,2, 8,
//  这   里
	4, 4, 6,2, 16,
//  这 里 启   航，
	4, 2,2, 6,2,
//  去 实   现
	6,2, 8,
//  心   中
	4, 4, 4, 6, 16,
//  心 中 的 梦  想。

	2,2,2,2, 4, 2,2,
//	风儿     传 颂着
	4, 2,2, 2, 6,
//  冶 金的 故 事，
	4, 2,2, 2,4,2, 16,
//  冶 金的  故     事，
	2,2,2,2, 4, 2,2,
//  心儿     放 飞着
	4, 2,2, 4, 4,
//  青 春的 梦 想，
	2, 4, 2, 4,2,2, 16,
//  青 春 的 梦      想。
	4, 2,2, 8,
//	让 我   们
	4,2,2, 8,
//  造     就
	4,2,2, 4,2,2,
//	金属的 风
	4,4,8,
//  骨，
	4,2,2, 4,4,
//  让我   们
	2,2,2,2, 8,
//  成       为
	4, 4, 2,4, 2,16,
//  祖 国 的栋  梁。
	8, 6,2,
//  为 了
	6,2, 8,
//  民   族
	4, 2,2, 4,4, 16,
//  民 族的 复    兴，
	4, 2,2, 6, 2,
//  不 辱   使 命
	2,2, 2,2, 8,
//  谱写 新   章，
	4, 4, 4,4, 2,16,
//  谱 写 新   章，
	8, 6, 2,
//  我 们 从
	6,2, 8,
//  这   里
	4, 4, 6,2, 16,
//  这 里 启   航，
	4, 2,2, 6,2,
//  去 拥   抱
	6,2, 8,
//  明   天
	4, 4, 4, 6, 20,
//  明 天 的 太  阳。
	4, 4, 4,
//  明 天 的
	8,8,16,
//  太
	16,16,16,
//  阳。
/****************************************
 * 	总 1128 / 4 = 282 拍
 * 一拍时间是 60 / 116 = 0.517 s
 * 四分之一拍的时间是	0.129 s
 * 此曲最小的时间是二分之一拍即 258 ms
*/
};
static unsigned char bMusic_tone[] = {
	0,1, 1,2, 3, 5,5,
//	告   别了 家  乡
	6, 5,3, 2, 1,
//  亲 切的 明 月，
	2, 2,3, 2,1,6, 1,
//  亲 切的  明     月，
	0,1,1,2, 3, 5,5,
//  迎来了   校  园
	6, 5,6, 8, 3,
//  灿 烂的 朝 阳，
	5, 5, 6, 5,3,2, 2,
//  灿 烂 的  朝     阳。
	3, 2,3, 5,
//	八 角   塘
	6,3,2, 1,
//  映     日
	3,5,6, 7,6,5,
//	荷花   别样
	5,6,6,
//  红，
	5,5,6, 8,3,
//  赣江   水
	2,1,2,3, 5,
//  源       头
	6, 5, 2,3, 1,1,
//  万 涓 泛波  浪。
	8, 8, 6,
//  走 进 这
	5,6, 8,
//  梦   想
	9, 9,8, 6,3, 5,
//  梦 想的 殿    堂，
	6, 5,6, 8, 6,
//  年 轻的 心  啊
	5,5, 5,3, 2,
//  澎湃 激   昂，
	6, 1, 2,3, 3,3,
//  澎 湃 激    昂，
	8, 8, 6,
//  我 们 从
	5,6, 8,
//  这   里
	9, 8, 6,5, 6,
//  这 里 启   航，
	5, 5,6, 8,3,
//  去 实   现
	2,3, 5,
//  心   中
	6, 8, 9, 9, 8,
//  心 中 的 梦  想。

	0,1,1,2, 3, 5,5,
//	风儿     传 颂着
	6, 5,3, 2, 1,
//  冶 金的 故 事，
	2, 2,3, 2,1,6, 1,
//  冶 金的  故     事，
	0,1,1,2, 3, 5,5,
//  心儿     放 飞着
	6, 5,6, 8, 3,
//  青 春的 梦 想，
	5, 5, 6, 5,3,2, 2,
//  青 春 的 梦      想。
	3, 2,3, 5,
//	让 我   们
	6,3,2, 1,
//  造     就
	3,5,6, 7,6,5,
//	金属的 风
	5,6,6,
//  骨，
	5,5,6, 8,3,
//  让我   们
	2,1,2,3, 5,
//  成       为
	6, 5, 2,3, 1,1,
//  祖 国 的栋  梁。
	8, 8,6,
//  为 了
	5,6, 8,
//  民   族
	9, 9,8, 6,3, 5,
//  民 族的 复    兴，
	6, 5,6, 8, 6,
//  不 辱   使 命
	5,5, 5,3, 2,
//  谱写 新   章，
	6, 1, 2,3, 3,3,
//  谱 写 新   章，
	8, 8, 6,
//  我 们 从
	5,6, 8,
//  这   里
	9, 8, 6,5, 6,
//  这 里 启   航，
	5, 5,6, 8,3,
//  去 拥   抱
	2,3, 5,
//  明   天
	6, 8, 9, 9, 8,
//  明 天 的 太  阳。
	5, 6, 8,
//  明 天 的
	9,10,3,
//  太
	8,1,0,
//  阳。
};

static void music_pwm_output(u16 wFrequency)
{
	TIM3->ARR = wFrequency;
	if (wFrequency == 1000)
		TIM3->CCR2 = 999;
	else
		TIM3->CCR2 = wFrequency / 2;
	TIM3->EGR |= 1;
}

u8 play_music_beep(u8 control)
{
	u8 res = 0;
	static u8 music_play_state = 0;
	static u16 music_play_step = 0;
	static u32 music_play_delay = 0, music_play_time = 0;

	if (control == 1) {
		music_play_step = 0;
		music_play_state = 0;
	} else if (control == 2) {
		music_play_step = MUSIC_LENGTH;
		music_pwm_output(tone_pwm_arr[0]);
	}
	if (music_play_step < MUSIC_LENGTH) {
		if (!music_play_state) {
			music_play_state++;
			music_play_time = Sys_time;
			music_play_delay = bMusic_time[music_play_step] * 129 - 1;
			music_pwm_output(tone_pwm_arr[bMusic_tone[music_play_step]]);
		} else if (Sys_time - music_play_time > music_play_delay) {
			music_play_step++;
			if (music_play_step == MUSIC_LENGTH)
				music_pwm_output(tone_pwm_arr[0]);
			music_play_state = 0;
		}
		res = 1;
	} else {
		music_play_state = 0;
	}

	return res;
}
