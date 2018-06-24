#include "Definition.h"

extern int MaxLine;	//蛇形缓冲区最多有多少行；
extern int MaxSeqLen;	//最大允许等待长度；
extern int MaxCustCheck;	//安检口队伍最大长度；
extern int MinCheckTime;	//一次安检最短时长；     
extern int MaxCheckTime;  //一次安检最大时长；
extern int MinRestTimeLen;	 //安检口一次休息最短时长；
extern int MaxRestTimeLen;	//安检口一次休息最长时长；
extern int OffdutyTime;	//下班时间 1 ~ Max；
extern bool IsAutoGenerate; //是否自动产生乘客；
extern int Speed;	//乘客移动速度；

extern CheckPoint checkpoint[9];

int SpShift[6] = { 0,2,4,5,8,10 };	//乘客移动速度档位
int ShiftNum = 1;	//乘客移动速度档位标记变量
FILE * out;	//定义文件指针 用于打开Log日志文件

MUSIC mus;		//定义音乐对象；

PIMAGE cus;	//添加乘客按钮
PIMAGE on;		//安检口工作状态贴图
PIMAGE off;	//安检口未开放状态贴图
PIMAGE waiting;	//安检口准备休息状态贴图
PIMAGE resting;	//安检口休息状态贴图
PIMAGE closed;	//安检口关闭状态贴图
PIMAGE fence;	//缓冲区围栏贴图
PIMAGE bk1;	//背景图片
PIMAGE bk2;
PIMAGE bk3;
PIMAGE bk4;

void SetData()	//初始化安检口坐标，贴图等数据
{
	cus = newimage();	//添加乘客按钮
	on = newimage();		//安检口工作状态贴图
	off = newimage();	//安检口未开放状态贴图
	waiting = newimage();	//安检口准备休息状态贴图
	resting = newimage();	//安检口休息状态贴图
	closed = newimage();	//安检口已下班状态贴图
	fence = newimage();		//缓冲区围栏贴图
	bk1 = newimage();	//背景图片
	bk2 = newimage();
	bk3 = newimage();
	bk4 = newimage();
	getimage(on, "ON.png");		//初始化贴图指针，在Output模块中调用；
	getimage(off, "OFF.png");
	getimage(waiting, "Waiting.png");
	getimage(resting, "Resting.png");
	getimage(closed, "Closed.png");
	getimage(cus, "PassengerButton.png");
	getimage(fence, "fence.png");
	getimage(bk1, "background1.png");
	getimage(bk2, "background2.png");
	getimage(bk3, "background3.png");
	getimage(bk4, "background4.png");

	for (int i = 0; i <= 8; i++)	//初始化安检口对应坐标及状态参数
		checkpoint[i].y = 185;
	checkpoint[0].x = 1140;
	checkpoint[0].state = SPARE;
	for (int i = 1; i <= 4; i++)
	{
		checkpoint[i].x = 80 + (i - 1) * 125;
		checkpoint[i].state = SPARE;
	}
	for (int i = 5; i <= 8; i++)
	{
		checkpoint[i].x = 580 + (i - 5) * 125;
		checkpoint[i].state = NOTOPEN;
	}

	PIMAGE pic_fail = newimage();	//打开文件失败框架；*
	getimage(pic_fail, "fail.png");

	errno_t err;
	err = fopen_s(&out, "log.txt", "w");	//打开log日志文件
	if (err != 0)
	{
		putimage(0, 0, pic_fail);	//打开文件失败输出；
		fprintf(out, "打开文件失败！\n");
		delay_ms(1000);
		exit(EXIT_FAILURE);
	}

	time_t Time;	//用于获取当前自然时间；
	struct tm ITime;
	time(&Time);
	localtime_s(&ITime, &Time);

	//向log文件里面输入；
	fprintf(out, "%d年%d月%d日%d时%d分%02d秒程序开始运行，配置参数如下：\n", ITime.tm_year + 1900, ITime.tm_mon + 1, ITime.tm_mday, ITime.tm_hour, ITime.tm_min, ITime.tm_sec);

	fprintf(out, "缓冲区最多直队数量：%d\n", MaxLine);

	fprintf(out, "最大允许等待长度：%d\n", MaxSeqLen);

	fprintf(out, "安检口队伍最大等待长度：%d\n", MaxCustCheck);

	fprintf(out, "一次安检最长时长(单位为秒)：%d\n", MaxCheckTime);

	fprintf(out, "一次安检最短时长(单位为秒)：%d\n", MinCheckTime);

	fprintf(out, "一次暂停休息最长时长(单位为秒)：%d\n", MaxRestTimeLen);

	fprintf(out, "一次暂停休息最短时长(单位为秒)：%d\n", MinRestTimeLen);

	fprintf(out, "下班时间(单位为秒)：%d\n", OffdutyTime);

	fprintf(out, "是否自动产生乘客（1代表是 0代表否）: %d\n", IsAutoGenerate);

	fprintf(out, "乘客移动速度：%d\n\n", ShiftNum);

	delimage(pic_fail);	//删除对应贴图指针；
}
void InputConfig()
{
	PIMAGE p1 = newimage();	//选项界面图（文件还是手动输入）；*
	PIMAGE p2 = newimage();	//手动输入界面；
	PIMAGE p3 = newimage();	//配置文件显示输出；
	PIMAGE p4 = newimage();	//您已选择手动输入图画；*
	PIMAGE p5 = newimage();	//白板（用于去除已经写上的数字）；
	getimage(p1, "interface1.png");
	getimage(p2, "interface2.png");
	getimage(p3, "interface3.png");
	getimage(p4, "maninput.png");
	getimage(p5, "blank.png");
	setfont(25, 0, "宋体");
	setbkcolor(EGERGB(0xFF, 0xFF, 0xFF));
	setcolor(BLACK);
	int flag = 1;
	while (flag)
	{
		putimage(0, 0, p1);
		mouse_msg msg = { 0 };
		while (!msg.is_down())
			msg = getmouse();
		if (msg.x <= 1080 && msg.x >= 750 && msg.y <= 460 && msg.y >= 382)	//判断点击是否在手动输入框内；(坐标未确定)
		{
			cleardevice();
			putimage(0, 0, p4);	//选择了手动；(坐标未确定)
			delay_ms(500);
			cleardevice();
			putimage(0, 0, p2);
			FILE * in;
			errno_t er;
			er = fopen_s(&in, "default_config.txt", "r");

			int variable[10] = { 7,4,5,5,2,10,5,200,1,3 };	//储存10个变量的初始值；

			for (int i = 0; i <= 9; i++)	//输出最开始10个变量的初始值；
			{
				if (i != 7 && i != 8 && i != 9)
				{
					char d[5] = { 0 };
					sprintf_s(d, "%d", variable[i]);
					outtextxy(425, 28 + i * 80, d);//循环一次加一个Δy，直到10个变量初始值全部输完；
				}
				else if (i == 7)
				{
					char d[5] = { 0 };
					sprintf_s(d, "%d", variable[i]);
					outtextxy(415, 28 + i * 80, d);
				}
				else if (i == 8)
				{
					if (variable[8])
						outtextxy(425, 28 + i * 80, "是");
					else
						outtextxy(425, 28 + i * 80, "否");
				}
				else if (i == 9)
				{
					char d[5] = { 0 };
					sprintf_s(d, "%d", variable[i]);
					outtextxy(1080, 665, d);
				}
			}


			MaxLine = variable[0];
			MaxSeqLen = variable[1];
			MaxCustCheck = variable[2];
			MaxCheckTime = variable[3];
			MinCheckTime = variable[4];
			MaxRestTimeLen = variable[5];
			MinRestTimeLen = variable[6];
			OffdutyTime = variable[7];
			IsAutoGenerate = variable[8];
			ShiftNum = variable[9];
			Speed = SpShift[ShiftNum];

			int flag1 = 1;
			while (flag1)
			{
				mouse_msg msg = { 0 };
				while (!msg.is_down())
					msg = getmouse();
				if (msg.x <= 1240 && msg.x >= 930 && msg.y <= 560 && msg.y >= 370)	//如果点击了确定按钮；
				{
					flag1 = 0;	//关闭循环，输出结束；
					cleardevice();
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 60 && msg.y >= 20)	//如果点击了MaxLine +
				{
					if (MaxLine < 7)
					{
						MaxLine += 2;
						putimage_transparent(NULL, p5, 396, 28, 0X0);//贴掉已经有的文字；
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxLine);
						outtextxy(425, 28, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 60 && msg.y >= 20)	//如果点击了MaxLine -
				{
					if (MaxLine > 1)
					{
						MaxLine -= 2;
						putimage_transparent(NULL, p5, 396, 28, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxLine);
						outtextxy(425, 28, s);
					}
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 140 && msg.y >= 100 && MaxSeqLen < MaxCustCheck)	//如果点击了MaxSeqLen +
				{
					if (MaxSeqLen < 5)
					{
						MaxSeqLen++;
						putimage_transparent(NULL, p5, 396, 28 + 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxSeqLen);
						outtextxy(425, 28 + 80, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 140 && msg.y >= 100)	//如果点击了MaxSeqLen -
				{
					if (MaxSeqLen > 1)
					{
						MaxSeqLen--;
						putimage_transparent(NULL, p5, 396, 28 + 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxSeqLen);
						outtextxy(425, 28 + 80, s);
					}
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 220 && msg.y >= 180)	//如果点击了MaxCustCheck +
				{
					if (MaxCustCheck < 5)
					{
						MaxCustCheck++;
						putimage_transparent(NULL, p5, 396, 28 + 2 * 80, 0X0);//贴掉已经有的文字；
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCustCheck);
						outtextxy(425, 28 + 2 * 80, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 220 && msg.y >= 180 && MaxCustCheck > MaxSeqLen)	//如果点击了MaxCustCheck -
				{
					if (MaxCustCheck > 1)
					{
						MaxCustCheck--;
						putimage_transparent(NULL, p5, 396, 28 + 2 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCustCheck);
						outtextxy(425, 28 + 2 * 80, s);
					}
				}

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 300 && msg.y >= 260)	//如果点击了MaxCheckTime +
				{
					MaxCheckTime++;
					putimage_transparent(NULL, p5, 396, 28 + 3 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", MaxCheckTime);
					outtextxy(425, 28 + 3 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 300 && msg.y >= 260)	//如果点击了MaxCheckTime -
				{
					if (MaxCheckTime == MinCheckTime&& MaxCheckTime > 1)
					{
						MaxCheckTime--;
						putimage_transparent(NULL, p5, 396, 28 + 3 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCheckTime);
						outtextxy(425, 28 + 3 * 80, s);
						MinCheckTime--;
						putimage_transparent(NULL, p5, 396, 28 + 4 * 80, 0X0);
						char t[5] = { 0 };
						sprintf_s(t, "%d", MinCheckTime);
						outtextxy(425, 28 + 4 * 80, t);
					}
					else if (MaxCheckTime > MinCheckTime && MaxCheckTime > 1)
					{
						MaxCheckTime--;
						putimage_transparent(NULL, p5, 396, 28 + 3 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCheckTime);
						outtextxy(425, 28 + 3 * 80, s);
					}
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 380 && msg.y >= 340)	//如果点击了MinCheckTime +
				{
					if (MinCheckTime == MaxCheckTime)
					{
						MaxCheckTime++;
						putimage_transparent(NULL, p5, 396, 28 + 3 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCheckTime);
						outtextxy(425, 28 + 3 * 80, s);
						MinCheckTime++;
						putimage_transparent(NULL, p5, 396, 28 + 4 * 80, 0X0);
						char t[5] = { 0 };
						sprintf_s(t, "%d", MinCheckTime);
						outtextxy(425, 28 + 4 * 80, t);
					}
					else
					{
						MinCheckTime++;
						putimage_transparent(NULL, p5, 396, 28 + 4 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MinCheckTime);
						outtextxy(425, 28 + 4 * 80, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 380 && msg.y >= 340)	//如果点击了MinCheckTime -
				{
					if (MinCheckTime > 1)
					{
						MinCheckTime--;
						putimage_transparent(NULL, p5, 396, 28 + 4 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MinCheckTime);
						outtextxy(425, 28 + 4 * 80, s);
					}
				}

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 460 && msg.y >= 420)	//如果点击了MaxRestTimeLen +
				{
					MaxRestTimeLen++;
					putimage_transparent(NULL, p5, 396, 28 + 5 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", MaxRestTimeLen);
					outtextxy(425, 28 + 5 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 460 && msg.y >= 420)	//如果点击了MaxRestTimeLen -
				{
					if (MaxRestTimeLen == MinRestTimeLen &&MaxRestTimeLen > 1)
					{
						MaxRestTimeLen--;
						putimage_transparent(NULL, p5, 396, 28 + 5 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxRestTimeLen);
						outtextxy(425, 28 + 5 * 80, s);
						MinRestTimeLen--;
						putimage_transparent(NULL, p5, 396, 28 + 6 * 80, 0X0);
						char t[5] = { 0 };
						sprintf_s(t, "%d", MinRestTimeLen);
						outtextxy(425, 28 + 6 * 80, t);
					}
					else if (MaxRestTimeLen > MinRestTimeLen && MaxRestTimeLen > 1)
					{
						MaxRestTimeLen--;
						putimage_transparent(NULL, p5, 396, 28 + 5 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxRestTimeLen);
						outtextxy(425, 28 + 5 * 80, s);
					}
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 540 && msg.y >= 500)	//如果点击了MinRestTimeLen +
				{
					if (MinRestTimeLen == MaxRestTimeLen)
					{
						MaxRestTimeLen++;
						putimage_transparent(NULL, p5, 396, 28 + 5 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxRestTimeLen);
						outtextxy(425, 28 + 5 * 80, s);
						MinRestTimeLen++;
						putimage_transparent(NULL, p5, 396, 28 + 6 * 80, 0X0);
						char t[5] = { 0 };
						sprintf_s(t, "%d", MinRestTimeLen);
						outtextxy(425, 28 + 6 * 80, t);
					}
					else
					{
						MinRestTimeLen++;
						putimage_transparent(NULL, p5, 396, 28 + 6 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MinRestTimeLen);
						outtextxy(425, 28 + 6 * 80, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 540 && msg.y >= 500)	//如果点击了MinRestTimeLen -
				{
					if (MinRestTimeLen > 1)
					{
						MinRestTimeLen--;
						putimage_transparent(NULL, p5, 396, 28 + 6 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", MinRestTimeLen);
						outtextxy(425, 28 + 6 * 80, s);
					}
				}

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 620 && msg.y >= 580)	//如果点击了OffdutyTime +
				{
					OffdutyTime += 10;
					putimage_transparent(NULL, p5, 396, 28 + 7 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", OffdutyTime);
					outtextxy(415, 28 + 7 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 620 && msg.y >= 580)	//如果点击了OffdutyTime -
				{
					if (OffdutyTime > 10)
					{
						OffdutyTime -= 10;
						putimage_transparent(NULL, p5, 396, 28 + 7 * 80, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", OffdutyTime);
						outtextxy(415, 28 + 7 * 80, s);
					}
				}

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 700 && msg.y >= 660)	//如果点击了IsAutoGenerate +
				{
					IsAutoGenerate = !IsAutoGenerate;
					putimage_transparent(NULL, p5, 396, 28 + 8 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", IsAutoGenerate);
					if (!IsAutoGenerate)
						outtextxy(425, 28 + 8 * 80, "否");
					else
						outtextxy(425, 28 + 8 * 80, "是");
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 700 && msg.y >= 660)	//如果点击了IsAutoGenerate -
				{
					IsAutoGenerate = !IsAutoGenerate;
					putimage_transparent(NULL, p5, 396, 28 + 8 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", IsAutoGenerate);
					if (!IsAutoGenerate)
						outtextxy(425, 28 + 8 * 80, "否");
					else
						outtextxy(425, 28 + 8 * 80, "是");
				}
				else if (msg.x <= 1200 && msg.x >= 1150 && msg.y <= 700 && msg.y >= 660)	//如果点击了Speed +
				{
					if (ShiftNum < 5)
					{
						ShiftNum++;
						Speed = SpShift[ShiftNum];
						putimage_transparent(NULL, p5, 1050, 665, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", ShiftNum);
						outtextxy(1080, 665, s);
					}
				}
				else if (msg.x <= 1250 && msg.x >= 975 && msg.y <= 700 && msg.y >= 660)	//如果点击了Speed -
				{
					if (ShiftNum > 1)
					{
						ShiftNum--;
						Speed = SpShift[ShiftNum];
						putimage_transparent(NULL, p5, 1050, 668, 0X0);
						char s[5] = { 0 };
						sprintf_s(s, "%d", ShiftNum);
						outtextxy(1080, 665, s);
					}
				}
			}
			flag = 0;	//初始化结束，不用继续循环；
		}
		else if (msg.x <= 1084 && msg.x >= 750 && msg.y <= 580 && msg.y >= 514)	//判断点击是否在文件配置框内；(坐标未确定)
		{
			FILE * in;
			errno_t er;
			er = fopen_s(&in, "default_config.txt", "r");

			fscanf_s(in, " %d  %d  %d  %d  %d  %d  %d  %d  %d  %d", &MaxLine, &MaxSeqLen, &MaxCustCheck, &MaxCheckTime,
				&MinCheckTime, &MaxRestTimeLen, &MinRestTimeLen, &OffdutyTime, &IsAutoGenerate, &ShiftNum);	//配置输入变量；
			Speed = SpShift[ShiftNum];
			putimage(0, 0, p3);	//已选择文件配置,显示配置参数；
			delay_ms(5000);
			flag = 0;	//初始化结束，不用继续循环；
		}

	}

	delimage(p1);	//删除对应贴图指针；
	delimage(p2);
	delimage(p3);
	delimage(p4);
	delimage(p5);
}
void Init()
{
	initgraph(SCR_WIDTH, SCR_HEIGHT); //初始化图形界面；
	randomize();	//初始化随机数种子；
	InputConfig();	//输入程序配置；
	mus.OpenFile("BGM.mp3"); //打开音乐文件；
	mus.SetVolume(0.8f); //设置音量；
	SetData();	//设置程序运行中所需参数；
}