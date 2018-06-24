#ifndef	DEFINITION_H
#define	DEFINITION_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>
#include <string>
#include <vector>
#include <queue>
#include "graphics.h"
using namespace std;

#define SPARE 0		//安检口空闲状态；
#define WORKING 1	//安检口工作中状态，即窗口有人；
#define WAITING 2	//安检口过渡态，即安检口发出休息申请但窗口仍有人,此时不再接收乘客；
#define RESTING 3	//安检口休息状态，即窗口没人，也不接收乘客；
#define NOTOPEN 4	//安检口待开放状态；
#define CLOSED	5	//安检口已下班状态；

#define BEONDUTY 0		//机场上班状态；
#define TOOFFDUTY 1		//机场准备下班，清空缓存区，处理完安检区进入下班；
#define BEOFFDUTY 2		//机场下班状态；

#define UP 0	//乘客向上移动；
#define DOWN 1	//乘客向下移动；
#define LEFT 2	//乘客向左移动；
#define RIGHT 3	//乘客向右移动；
#define STILL 4 //乘客停止移动；

#define SCR_WIDTH	1280	//动画窗口宽度；
#define SCR_HEIGHT	720		//动画窗口高度；

#define PASSENGER_HEIGHT 40 //乘客贴图高度；
#define PASSENGER_WIDTH 40 //乘客贴图宽度；
#define ENTRANCE_X 345	//入口处X坐标值；
#define ENTRANCE_Y 675	//入口处Y坐标值；
#define BUFFER_UP_Y 425 //缓冲区上侧边界Y坐标；
#define BUFFER_DOWN_Y 675 //缓冲区下侧边界Y坐标；
#define BUFFER_LINE_WIDTH 76//缓冲区队列宽度；
#define CHECK_LINE_Y	165//安检口Y坐标；
#define PI 3.141592654


typedef struct
{
	int ID;	//乘客编号；
	PIMAGE pic;	//乘客对应贴图指针;
	int x, y;	//乘客坐标；
	int dir;	//乘客运动方向；
	int LastDir;	//乘客静止前最后一次运动方向；
	double CheckTime;	//检查时间；
	double LastLineX;	//记录在上一个缓冲区队列的X坐标，方便横向移动处理；
}Passenger;
typedef struct
{
	int x, y;	//安检口坐标；
	int state;	//安检口状态；
	double RestTime;	//安检口休息时间；
	vector<Passenger> CheckQueue;	//安检口对应乘客队列；
}CheckPoint;

#endif // !__DEFINITION_H__