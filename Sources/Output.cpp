#include "Definition.h"

extern PIMAGE cus;	//添加乘客按钮
extern PIMAGE on;		//安检口工作状态贴图
extern PIMAGE off;	//安检口未开放状态贴图
extern PIMAGE resting;	//安检口休息状态贴图
extern PIMAGE waiting;	//安检口准备休息状态贴图
extern PIMAGE closed;	//安检口已下班状态贴图
extern PIMAGE fence;	//缓冲区围栏贴图
extern PIMAGE bk1;	//程序背景贴图
extern PIMAGE bk2;
extern PIMAGE bk3;
extern PIMAGE bk4;

extern MUSIC mus;		//定义音乐对象；


extern int BufferLineNum;	//当前蛇形队列行数
extern int SystemState;	//获取系统运行状态变量，以便输出相应参数
extern int ShiftNum;	//乘客移速档位标记变量
extern double CurrentTime;
extern vector<Passenger> buffer;
extern bool IsAutoGenerate;
extern CheckPoint checkpoint[9];
int state_x[9] = {0,25,150,275,400,527,654,779,904 }; //记录安检口状态贴图x坐标
char MyClock[20] = { 0 };

void Output()
{
	cleardevice();
	switch (BufferLineNum)	//根据蛇形队列数动态更换背景贴图；
	{
	case 1:	putimage(0, 0, bk1); break;
	case 3:	putimage(0, 0, bk2); break;
	case 5:	putimage(0, 0, bk3); break;
	case 7:	putimage(0, 0, bk4); break;
	default: break;
	}

	putimage_transparent(NULL, cus,200,615, 0x0);	//输出添加乘客的按钮贴图
	if(SystemState != BEONDUTY)
		putimage_transparent(NULL, fence, 323, 675, 0x0);	//如果非工作状态，则增加一个围栏贴图表示缓冲区已关闭
	for (int i = 1; i <= 8; i++)	//输出安检口状态贴图；
	{
		switch (checkpoint[i].state)
		{
		case SPARE:		putimage_transparent(NULL, on, state_x[i], 22, 0x0);			break;
		case WORKING:	putimage_transparent(NULL, on, state_x[i], 22, 0x0);			break;
		case WAITING:	putimage_transparent(NULL, waiting, state_x[i], 22, 0x0);		break;
		case RESTING:	putimage_transparent(NULL, resting, state_x[i], 22, 0x0);			break;
		case NOTOPEN:	putimage_transparent(NULL, off, state_x[i], 22, 0x0);			break;
		case CLOSED:	putimage_transparent(NULL, closed, state_x[i], 22, 0x0);		break;
		}
	}
	for (int i = 0; i < buffer.size(); i++)	//输出蛇形队列的乘客；
	{
		putimage_transparent(NULL, buffer[i].pic, buffer[i].x, buffer[i].y, 0X0);
	}
	setfontbkcolor(EGERGB(216,213,208));	//设置乘客ID字体样式
	setfont(18, 0, "宋体");
	for (int i = 0; i <= 8; i++)	//输出安检口的乘客；
	{
		for (int j = 0; j < checkpoint[i].CheckQueue.size(); j++)
		{
			putimage_transparent(NULL, checkpoint[i].CheckQueue[j].pic, checkpoint[i].CheckQueue[j].x, checkpoint[i].CheckQueue[j].y, 0X0);
			if (checkpoint[i].CheckQueue[j].x == checkpoint[i].x)	//如果乘客已排在安检口队列,输出其编号
			{
				char s[5] = { 0 };
				sprintf_s(s, "%d", checkpoint[i].CheckQueue[j].ID);
				outtextxy(checkpoint[i].CheckQueue[j].x + 40, checkpoint[i].CheckQueue[j].y + 23, s);
			}
		}
	}

	char s[10] = { 0 };
	setfontbkcolor(WHITE);	//输出当前速度参数；
	setfont(22, 0, "宋体");
	sprintf_s(s, "%d", ShiftNum);
	outtextxy(115, 548, s);

	setfontbkcolor(EGERGB(139, 217, 236));	//输出安检口剩余安检时间或休息剩余时间
	if (!checkpoint[0].CheckQueue.empty() && checkpoint[0].CheckQueue[0].y - 10 <= CHECK_LINE_Y)	//紧急安检口特殊处理
	{
		setfont(40, 0, "宋体");
		char s[10] = { 0 };
		sprintf_s(s, "%.1f", checkpoint[0].CheckQueue[0].CheckTime);
		outtextxy(1030, 185, s);
	}
	setfont(20, 0, "宋体");
	for (int i = 1; i <= 8; i++)
	{
		char s[10] = { 0 };
		if (!checkpoint[i].CheckQueue.empty() && checkpoint[i].CheckQueue[0].y == CHECK_LINE_Y)	//如果安检口非空且队首乘客已到达安检口，输出安检时间
		{
			sprintf_s(s, "%.1f", checkpoint[i].CheckQueue[0].CheckTime);
			outtextxy(25 + (i - 1) * 125, 190, s);
		}
		if(checkpoint[i].state == RESTING)
		{
			sprintf_s(s, "%.1f", checkpoint[i].RestTime);
			outtextxy(25 + (i - 1) * 125, 190, s);
		}
	}

	setfontbkcolor(EGERGB(255, 199, 100));	//设置输出运行时间字体样式
	setfont(30, 0, "新宋体");
	if (SystemState == BEONDUTY)	//根据程序运行状态输出相应参数
	{
		int min = (int)CurrentTime / 60, sec = (int)CurrentTime % 60, ms = (int)(CurrentTime * 100) % 100;	//格式化当前时间，精确到毫秒
		sprintf_s(MyClock, "%02d:%02d:%02d", min, sec, ms);
		outtextxy(45, 660, MyClock);
	}
	if(SystemState == TOOFFDUTY)
		outtextxy(45, 660, "准备下班");
	if(SystemState == BEOFFDUTY)
		outtextxy(55, 660, "已下班");

	if (mus.GetPlayStatus() == MUSIC_MODE_STOP) // 发现背景音乐停止了就重新播放
		mus.Play(0);
}