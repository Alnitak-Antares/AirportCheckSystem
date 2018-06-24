#include "Definition.h"

extern int state_x[]; //记录安检口状态贴图x坐标

extern int SpShift[];	//乘客移动速度档位
extern int ShiftNum;	//乘客移速档位标记变量
extern int Speed;	//实现手动调整乘客速度；
extern double CurrentTime;	//获取当前时间；
extern int SystemState;	//实现手动控制系统下班；
extern bool IsAutoGenerate;	//检查是否手动输入乘客；
extern void NewCusEnqueue();	//利用Control模块新乘客入队函数实现手动产生乘客；

extern CheckPoint checkpoint[9];
extern FILE * out;	//利用Init模块定义的文件指针实现程序运行过程中对文件的动态写入；

void Input()
{
	mouse_msg msg;
	while (mousemsg())
	{
		msg = getmouse();
		if (msg.is_down())
		{
			if (msg.y >= 22 && msg.y <= 55)	//判断是否更改安检口状态
			{
				for (int i = 1; i <= 8; i++)
				{
					if (msg.x >= state_x[i] && msg.x <= state_x[i] + 80)	//如果点击在安检口状态栏
					{
						switch (checkpoint[i].state)	//根据情况更改安检口状态
						{
						case SPARE:	 fprintf(out, "第%.2f秒:向%d号安检口发出休息指令,安检口准备休息\n", CurrentTime, i); checkpoint[i].state = WAITING;	break;
						case WORKING: fprintf(out, "第%.2f秒:向%d号安检口发出休息指令,安检口准备休息\n", CurrentTime, i);	checkpoint[i].state = WAITING;	break;
						case NOTOPEN:	fprintf(out, "第%.2f秒:向%d号安检口发出开启指令\n", CurrentTime, i);	checkpoint[i].state = SPARE;	break;
						default:	break;
						}
					}
				}
			}
			else if (msg.x >= 220 && msg.x <= 300 && msg.y >= 630 && msg.y <= 715)	//如果点击在添加乘客按钮
			{
				fprintf(out, "第%.2f秒:点击了手动添加乘客按钮\n", CurrentTime);
				NewCusEnqueue();	//新产生乘客入队
			}
			else if (SystemState == BEONDUTY && msg.x >= 1180 && msg.x <= 1280 && msg.y >= 630 && msg.y <= 720)	//如果正在上班状态且点击了下班按钮
			{
				fprintf(out, "第%.2f秒:点击了下班按钮,准备下班\n", CurrentTime);
				SystemState = TOOFFDUTY;
			}
			else if (msg.x >= 200 && msg.x <= 240 && msg.y >= 515 && msg.y <= 580 && ShiftNum < 5)	//如果点击了乘客移速增加；
			{
				fprintf(out, "第%.2f秒:点击了乘客移速增加,当前速度%d->%d\n", CurrentTime, ShiftNum, ShiftNum+1);
				Speed = SpShift[++ShiftNum];
			}
			else if (msg.x >= 0 && msg.x <= 40 && msg.y >= 515 && msg.y <= 580 && ShiftNum > 1)	//如果点击了乘客移速减少；
			{
				fprintf(out, "第%.2f秒:点击了乘客移速减少,当前速度%d->%d\n", CurrentTime, ShiftNum, ShiftNum-1);
				Speed = SpShift[--ShiftNum];
			}
		}
	}
}