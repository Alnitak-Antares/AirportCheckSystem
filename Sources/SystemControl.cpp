#include "Definition.h"
using namespace std;

extern int MaxCustSingleLine;	//缓冲区单队列最大乘客数；
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
extern double CurrentTime;	//记录当前时间；

extern FILE * out;	//利用Init模块定义的文件指针实现程序运行过程中对文件的动态写入；

extern vector<Passenger> buffer;
extern CheckPoint checkpoint[9];

extern int SystemState;	//记录当前系统运行状态；
int BufferLineNum = 1; //记录当前蛇形队列行数；
int EXIT_X[] = { 345,505,670,830 };	//缓冲区出口X坐标；
int PassengerNum = 1;	//乘客编号；

double gaussrand()	//采用Box-Muller算法，提供符合标准正态分布的随机数；
{
	static double U, V;
	static int phase = 0;
	double z;
	if (phase == 0)
	{
		U = randomf();
		V = randomf();
		z = sqrt(-2.0 * log(U))* sin(2.0 * PI * V);
	}
	else
	{
		z = sqrt(-2.0 * log(U)) * cos(2.0 * PI * V);
	}

	phase = 1 - phase;
	return z;
}
int CheckPointNum()	//返回当前开放的安检口数量；
{
	int cnt = 0;
	for (int i = 1; i <= 8; i++)
		if (checkpoint[i].state == SPARE || checkpoint[i].state == WORKING)
			cnt++;
	return cnt;
}
int ProperPointID()	//返回当前正在工作的排队人数最少的安检口的编号；
{
	int MinID;
	for (int i = 1; i <= 8; i++)	//预处理
	{
		if (checkpoint[i].state == SPARE || checkpoint[i].state == WORKING)
		{
			MinID = i;
			break;	//函数的调用逻辑保证这个函数必定会运行到这一步；
		}
	}
	for (int i = 1; i <= 8; i++)
	{
		if (checkpoint[i].state != SPARE && checkpoint[i].state != WORKING)
			continue;
		if (checkpoint[i].CheckQueue.size() < checkpoint[MinID].CheckQueue.size())
			MinID = i;
	}
	return MinID;
}

bool BufferFull()	//判断缓冲区是否已满；
{
	return buffer.size() >= MaxCustSingleLine * BufferLineNum + BufferLineNum - 1;
}
bool CheckPointFull()	//判断安检口是否已满；
{
	for (int i = 1; i <= 8; i++)
	{
		if (checkpoint[i].state == SPARE || (checkpoint[i].state == WORKING && checkpoint[i].CheckQueue.size() < MaxCustCheck))
			return  false;
	}
	return true;
}
bool CheckPointEmpty()	//判断所有安检口是否为空；
{
	for (int i = 0; i < 9; i++)
	{
		if (!checkpoint[i].CheckQueue.empty())
			return false;
	}
	return true;
}
bool IsInExit(Passenger * cus)	//判断乘客是否到达缓冲区出口；
{
	return abs(cus->x - EXIT_X[BufferLineNum / 2]) <= 10 && abs(cus->y - BUFFER_UP_Y) <= 10;
}

void CheckPointEnqueue(Passenger * cus, int num)	//乘客入队至相应编号的安检口；
{
	if (num == 0)	//紧急安检口特殊处理
	{
		if (!checkpoint[0].CheckQueue.empty() && checkpoint[0].CheckQueue.back().x > 1240)
			cus->x = checkpoint[0].CheckQueue.back().x + PASSENGER_WIDTH;
		else
		cus->x = 1280;
		cus->y = 380;
	}
	checkpoint[num].CheckQueue.push_back(*cus);
}

void NewCusEnqueue()	//产生新乘客入队
{
	if (SystemState != BEONDUTY)	//如果此时非工作状态，则不予响应；
		return;
	Passenger tmp;
	tmp.x = ENTRANCE_X;			//初始化乘客坐标；
	if (buffer.empty())
		tmp.y = ENTRANCE_Y;
	else
	{
		Passenger tail = buffer.back();
		if (abs(tail.x - ENTRANCE_X) < 1e-6 && tail.y > ENTRANCE_Y - PASSENGER_HEIGHT)
			tmp.y = tail.y + PASSENGER_HEIGHT;
		else
			tmp.y = ENTRANCE_Y;
	}
	tmp.pic = newimage(); //初始化乘客贴图指针；
	tmp.ID = PassengerNum++;	//初始化乘客ID;
	tmp.dir = UP;	//乘客初始进入队列，移动方向向上；
	tmp.LastDir = UP;
	tmp.LastLineX = ENTRANCE_X;
	int type = random(5);	//采用ege库提供的随机函数，产生0~4之间的随机数，分配乘客类型；
	switch (type)
	{
	case 0:	getimage(tmp.pic, "official.png");	break;
	case 1:	getimage(tmp.pic, "youngman.png");	break;
	case 2:	getimage(tmp.pic, "middle-aged.png");	break;
	case 3:	getimage(tmp.pic, "lady1.png");		break;
	case 4:	getimage(tmp.pic, "lady2.png");	break;
	default: break;
	}

	tmp.CheckTime = (MinCheckTime + MaxCheckTime) / 2 + gaussrand();	//利用正态分布随机化模拟每位乘客的检查时间；
	if (tmp.CheckTime < MinCheckTime)	tmp.CheckTime = MinCheckTime;	//范围处理；
	if (tmp.CheckTime > MaxCheckTime)	tmp.CheckTime = MaxCheckTime;

	bool IsEmergent = (randomf() > 0.95);	//利用randomf()产生0.0~1.0的随机数，模拟每位乘客为急客的概率为5%；
	if (IsEmergent && checkpoint[0].CheckQueue.size() < MaxCustCheck)	//如果为急客且紧急通道未满，直接进入紧急安检口；
	{
		fprintf(out, "第%.2f秒:%d号乘客到达机场,急客进入紧急安检口\n",CurrentTime, tmp.ID);
		CheckPointEnqueue(&tmp, 0);
	}
	else if (!BufferFull())	//否则如果缓冲区未满，则进入缓冲区
	{
		fprintf(out, "第%.2f秒:%d号乘客到达机场,进入缓冲区\n",CurrentTime, tmp.ID);
		buffer.push_back(tmp);	//新产生乘客入队；
	}
	else	//如果入队失败，则将乘客数量还原
	PassengerNum--;
}

void BufferDequeue()	//缓冲区队首元素出队；
{
	if (!buffer.empty() && IsInExit(&buffer[0]))	//如果队首乘客到达出口位置
	{
		if (CheckPointFull())	//如果此时安检口已满，使乘客等待；
		{
			if (buffer[0].dir != STILL)
			{
				buffer[0].LastDir = buffer[0].dir;
				buffer[0].dir = STILL;
			}
		}
		else	//否则乘客出缓冲区，进入对应的安检口队列；
		{
			int num = ProperPointID();
			CheckPointEnqueue(&buffer[0], num);
			fprintf(out, "第%.2f秒:%d号乘客出缓冲区,选择%d号安检口\n", CurrentTime, buffer[0].ID,num);
			buffer.erase(buffer.begin());
		}
	}
}

void MoveInDir(Passenger * cus)	//根据当前运动方向和速度改变乘客的坐标
{
	switch (cus->dir)
	{
	case UP:	cus->y -= Speed; break;
	case DOWN:	cus->y += Speed; break;
	case LEFT:	cus->x -= Speed; break;
	case RIGHT:	cus->x += Speed; break;
	default:	break;
	}
}

void TransPosInBuffer()	//改变乘客在缓冲区内的位置;
{
	for (int i = 0; i < buffer.size(); i++)
	{
		switch (buffer[i].dir)	//边界转向处理；
		{
		case UP:
			if (buffer[i].y - 5 < BUFFER_UP_Y)
				buffer[i].dir = RIGHT;
			break;
		case DOWN:
			if (buffer[i].y + 5 > BUFFER_DOWN_Y)
				buffer[i].dir = RIGHT;
			break;
		case RIGHT:
			if (buffer[i].x - buffer[i].LastLineX > BUFFER_LINE_WIDTH)
			{
				buffer[i].LastLineX = buffer[i].x;
				if (buffer[i].y - 5 < BUFFER_UP_Y)
					buffer[i].dir = DOWN;
				else
					buffer[i].dir = UP;
			}
		default:	break;
		}
		if (i > 0 && buffer[i - 1].dir == STILL)	//移动转向静止判断处理；
		{
			if (buffer[i].dir == UP && abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y - Speed - PASSENGER_HEIGHT < buffer[i - 1].y)
			{
				buffer[i].dir = STILL;	buffer[i].LastDir = UP;
				continue;
			}
			if (buffer[i].dir == DOWN && abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y + Speed + PASSENGER_HEIGHT > buffer[i - 1].y)
			{
				buffer[i].dir = STILL;	buffer[i].LastDir = DOWN;
				continue;
			}
			if (buffer[i].dir == RIGHT && abs(buffer[i].y - buffer[i - 1].y) < PASSENGER_HEIGHT && buffer[i].x + Speed + PASSENGER_WIDTH > buffer[i - 1].x)
			{
				buffer[i].dir = STILL;	buffer[i].LastDir = RIGHT;
				continue;
			}
		}
		if (buffer[i].dir == STILL)	//静止转向移动判断处理；
		{
			if (i == 0 && !IsInExit(&buffer[i]))	//队首元素特殊处理
				buffer[i].dir = buffer[i].LastDir;
			if (i > 0 && buffer[i].LastDir == UP && !(abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y - 10 - PASSENGER_HEIGHT < buffer[i - 1].y))
				buffer[i].dir = UP;
			if (i > 0 && buffer[i].LastDir == DOWN && !(abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y + 10 + PASSENGER_HEIGHT > buffer[i - 1].y))
				buffer[i].dir = DOWN;
			if (i > 0 && buffer[i].LastDir == RIGHT && !(abs(buffer[i].y - buffer[i - 1].y) < PASSENGER_HEIGHT && buffer[i].x + 10 + PASSENGER_WIDTH > buffer[i - 1].x))
				buffer[i].dir = RIGHT;
		}
		MoveInDir(&buffer[i]);	//根据已改变的运动方向改变乘客的坐标；
	}
}

void TransPosInCheckPoint()	//改变乘客在安检口队列里的位置；
{
	for (int i = 0; i < checkpoint[0].CheckQueue.size(); i++)	//控制紧急安检口乘客移动（紧急安检口乘客的移速为普通乘客的两倍）
	{
		if (checkpoint[0].CheckQueue[i].x - Speed * 2 >= checkpoint[0].x)
		{
			checkpoint[0].CheckQueue[i].x -= Speed * 2;
			continue;
		}
		if (i == 0 && checkpoint[0].CheckQueue[i].y - Speed * 2 >= CHECK_LINE_Y)
			checkpoint[0].CheckQueue[i].y -= Speed * 2;
		else if (i > 0 && checkpoint[0].CheckQueue[i].y - Speed * 2 - PASSENGER_HEIGHT >= checkpoint[0].CheckQueue[i - 1].y)
			checkpoint[0].CheckQueue[i].y -= Speed * 2;
		if (abs(checkpoint[0].CheckQueue[i].x - checkpoint[0].x) < 2 * Speed)	//如果误差小于两个Speed单位,则直接对齐
			checkpoint[0].CheckQueue[i].x = checkpoint[0].x;
		if (abs(checkpoint[0].CheckQueue[i].y - CHECK_LINE_Y) < 2 * Speed)
			checkpoint[0].CheckQueue[i].y = CHECK_LINE_Y;
	}
	for (int i = 1; i <= 8; i++)	//控制普通安检口乘客移动
	{
		for (int j = 0; j < checkpoint[i].CheckQueue.size(); j++)
		{
			if (checkpoint[i].CheckQueue[j].y > 370)		//未到转向区则继续向上移动
				checkpoint[i].CheckQueue[j].y -= Speed;
			else if (abs(checkpoint[i].CheckQueue[j].x - checkpoint[i].x) >= Speed)	//未到安检口对应横坐标则继续横向移动
				checkpoint[i].CheckQueue[j].x += (checkpoint[i].CheckQueue[j].x < checkpoint[i].x ? 1 : -1) * Speed;	//根据乘客及安检口x坐标的相对位置使乘客横向移动
			else if (j == 0 && checkpoint[i].CheckQueue[j].y > CHECK_LINE_Y)	//如果为队首乘客且纵向位置未到达安检口,则向上移动
				checkpoint[i].CheckQueue[j].y -= Speed;
			else if(j > 0 && checkpoint[i].CheckQueue[j].y - Speed - PASSENGER_HEIGHT >= checkpoint[i].CheckQueue[j-1].y)	//如果非队首乘客且向上不会发生碰撞,则向上移动
				checkpoint[i].CheckQueue[j].y -= Speed;

			if (abs(checkpoint[i].CheckQueue[j].x - checkpoint[i].x) < Speed)	//如果误差小于一个Speed单位,则直接对齐
				checkpoint[i].CheckQueue[j].x = checkpoint[i].x;
			if (abs(checkpoint[i].CheckQueue[j].y - CHECK_LINE_Y) < Speed)
				checkpoint[i].CheckQueue[j].y = CHECK_LINE_Y;
			if (abs(checkpoint[i].CheckQueue[j].y - 370) < Speed)
				checkpoint[i].CheckQueue[j].y = 370;
			if (j > 0 && checkpoint[i].CheckQueue[j].x == checkpoint[i].x && checkpoint[i].CheckQueue[j].y - PASSENGER_HEIGHT - checkpoint[i].CheckQueue[j - 1].y < Speed)
				checkpoint[i].CheckQueue[j].y = PASSENGER_HEIGHT + checkpoint[i].CheckQueue[j - 1].y;

		}
	}
}

void TransCheckPointState()	//根据情况改变安检口的状态
{
	if (SystemState == BEONDUTY && (CheckPointNum() == 0 || buffer.size() / CheckPointNum() >= MaxSeqLen))	//满足条件则动态增开一个安检口；
	{
		for (int i = 1; i <= 8; i++)
		{
			if (checkpoint[i].state == NOTOPEN)
			{
				fprintf(out, "第%.2f秒:缓冲区人数过多,增开%d号安检口\n", CurrentTime, i);
				checkpoint[i].state = SPARE;
				break;
			}
		}
	}
	for (int i = 1; i <= 8; i++)
	{
		if (checkpoint[i].state == WORKING && checkpoint[i].CheckQueue.empty())
			checkpoint[i].state = SPARE;
		if (checkpoint[i].state == SPARE && !checkpoint[i].CheckQueue.empty())
			checkpoint[i].state = WORKING;
		if (checkpoint[i].state == RESTING)
		{
			checkpoint[i].RestTime -= 1.0 / 60;		//休息时间减少一帧所对应时长；
			if (checkpoint[i].RestTime <= 0)		//如果休息完毕，转为待开放状态
			{
				checkpoint[i].state = NOTOPEN;
				fprintf(out, "第%.2f秒:%d号安检口休息完毕,转为待开放状态", CurrentTime, i);
			}
		}
		if (checkpoint[i].state == WAITING && checkpoint[i].CheckQueue.empty())
		{
			checkpoint[i].state = RESTING;
			checkpoint[i].RestTime = (MinRestTimeLen + MaxRestTimeLen) / 2 + gaussrand() * 2;	//利用正态分布随机化模拟每个安检口的休息时间；
			if (checkpoint[i].RestTime < MinRestTimeLen)	checkpoint[i].RestTime = MinRestTimeLen;	//范围处理；
			if (checkpoint[i].RestTime > MaxRestTimeLen)	checkpoint[i].RestTime = MaxRestTimeLen;
			if(SystemState == BEONDUTY)
				fprintf(out, "第%.2f秒:%d号安检口转为休息状态,休息时间为%.2f秒\n",CurrentTime, i, checkpoint[i].RestTime);
		}
	}
	if (SystemState != BEONDUTY && buffer.empty())	//如果非工作状态且缓冲区已清空
	{
		for (int i = 1; i <= 8; i++)
		{
			if (checkpoint[i].state == SPARE || checkpoint[i].state == RESTING || checkpoint[i].state == NOTOPEN)
			{
				checkpoint[i].state = CLOSED;
				fprintf(out, "第%.2f秒:%d号安检口转为已下班状态\n",CurrentTime, i, checkpoint[i].RestTime);
			}
		}
	}
}

void CheckPointDequeue()
{
	for (int i = 0; i <= 8; i++)
	{
		if (!checkpoint[i].CheckQueue.empty()) //如果安检口队列非空；
		{
			if (abs(checkpoint[i].CheckQueue[0].y == CHECK_LINE_Y))	//如果队首乘客已到达安检口
				checkpoint[i].CheckQueue[0].CheckTime -= 1.0 / 60;	//安检口安检时间减少一帧所对应的时间
			if (checkpoint[i].CheckQueue[0].CheckTime <= 0)	//安检时间到则出队
			{
				if(i == 0)
					fprintf(out, "第%.2f秒:紧急安检口队首%d号乘客安检完毕\n", CurrentTime, checkpoint[i].CheckQueue[0].ID);
				else
					fprintf(out, "第%.2f秒:%d号安检口队首%d号乘客安检完毕\n", CurrentTime, i, checkpoint[i].CheckQueue[0].ID);
				delimage(checkpoint[i].CheckQueue[0].pic);
				checkpoint[i].CheckQueue.erase(checkpoint[i].CheckQueue.begin());
			}
		}

	}
}

void NewOpenAdjust()	//实现新增开安检口时临近安检口乘客队列的动态调整
{
	if (checkpoint[1].state == SPARE)	//左侧边界安检口新开时，只需调整右侧安检口队列
	{
		for (int i = 2; i < checkpoint[2].CheckQueue.size(); i++)
		{
			fprintf(out, "第%.2f秒:2号安检口%d号乘客动态调整至1号安检口\n", CurrentTime, checkpoint[2].CheckQueue[i].ID);
			checkpoint[1].CheckQueue.push_back(checkpoint[2].CheckQueue[i]);
		}
		for (int i = checkpoint[2].CheckQueue.size() - 1; i >= 2; i--)
			checkpoint[2].CheckQueue.erase(checkpoint[2].CheckQueue.begin() + i);
	}
	if (checkpoint[8].state == SPARE)	//右侧边界安检口新开时，只需调整左侧安检口队列
	{
		for (int i = 2; i < checkpoint[7].CheckQueue.size(); i++)
		{
			fprintf(out, "第%.2f秒:7号安检口%d号乘客动态调整至8号安检口\n", CurrentTime, checkpoint[7].CheckQueue[i].ID);
			checkpoint[8].CheckQueue.push_back(checkpoint[7].CheckQueue[i]);
		}
		for (int i = checkpoint[7].CheckQueue.size() - 1; i >= 2; i--)
			checkpoint[7].CheckQueue.erase(checkpoint[7].CheckQueue.begin() + i);
	}
	for (int i = 2; i <= 7; i++)	//调整左侧及右侧安检口队列
	{
		if (checkpoint[i].state == SPARE)
		{
			int todel1[5] = { 0 }, todel2[5] = { 0 };	//用于记录将要删除的邻近安检口的乘客
			int idx1 = 0, idx2 = 0;
			for (int j =  2; j < max(checkpoint[i - 1].CheckQueue.size(), checkpoint[i + 1].CheckQueue.size()); j++)	//两侧安检口对应乘客进入新开安检口
			{
				int ID1 = j < checkpoint[i - 1].CheckQueue.size() ? checkpoint[i - 1].CheckQueue[j].ID : INT_MAX;
				int ID2 = j < checkpoint[i + 1].CheckQueue.size() ? checkpoint[i + 1].CheckQueue[j].ID : INT_MAX;
				if (ID1 < ID2)
				{
					fprintf(out, "第%.2f秒:%d号安检口%d号乘客动态调整至%d号安检口\n", CurrentTime,i-1, checkpoint[i-1].CheckQueue[j].ID,i);
					checkpoint[i].CheckQueue.push_back(checkpoint[i - 1].CheckQueue[j]);
					todel1[idx1++] = j;
				}
				else
				{
					fprintf(out, "第%.2f秒:%d号安检口%d号乘客动态调整至%d号安检口\n", CurrentTime, i+1, checkpoint[i+1].CheckQueue[j].ID, i);
					checkpoint[i].CheckQueue.push_back(checkpoint[i + 1].CheckQueue[j]);
					todel2[idx2++] = j;
				}
			}
			for (int j = idx1 - 1; j >= 0; j--)		//删除两侧安检口队列对应乘客
				checkpoint[i - 1].CheckQueue.erase(checkpoint[i - 1].CheckQueue.begin() + todel1[j]);
			for (int j = idx2 - 1; j >= 0; j--)
				checkpoint[i + 1].CheckQueue.erase(checkpoint[i + 1].CheckQueue.begin() + todel2[j]);
		}
	}

}

void BufferControl()	//实现对缓冲区的总体控制
{
	if (BufferFull() && BufferLineNum < MaxLine)
	{
		fprintf(out, "第%.2f秒:缓冲区已满,蛇形队列动态增开\n", CurrentTime);
		BufferLineNum += 2;
	}
	BufferDequeue();	//判断处理缓冲区队首元素出队操作；
	if (IsAutoGenerate && SystemState == BEONDUTY)	//自动模式且工作状态下下产生新乘客进入缓冲区；
	{
		if (randomf() < 0.05 + gaussrand() / 100)	//利用正态分布模拟每一帧乘客出现的概率，平均每秒出现三个乘客；
			NewCusEnqueue();
	}
	TransPosInBuffer();	//改变乘客坐标，实现动画效果；
}

void CheckPointControl()	//实现对安检口的总体控制
{
	CheckPointDequeue();
	TransCheckPointState();
	NewOpenAdjust();
	TransPosInCheckPoint();
}

void SystemControl()	//对系统的总体控制，并实现周期性文件输出等;
{
	CurrentTime = fclock();	//更新一帧所对应的时间；
	CheckPointControl();
	BufferControl();
	if ((int)(CurrentTime + 1.0 / 60) > (int)CurrentTime)	//每次在新的一秒，输出程序相关关键参数；
	{
		fprintf(out, "\n程序运行第%d秒:\n", (int)(CurrentTime + 1.0 / 60) );
		fprintf(out, "缓冲区当前人数: %d\n", buffer.size());
		if (BufferFull() && BufferLineNum == MaxLine)
			fprintf(out, "缓冲区已满\n");
		if (checkpoint[0].CheckQueue.size() > 0)
			fprintf(out, "紧急安检口工作中,当前窗口人数: %d,队首%d号乘客剩余安检时间%.2fs\n", checkpoint[0].CheckQueue.size(), checkpoint[0].CheckQueue[0].ID, checkpoint[0].CheckQueue[0].CheckTime);
		else
			fprintf(out, "紧急安检口空闲中\n");
		for (int i = 1; i <= 8; i++)
		{
			fprintf(out, "%d号安检口当前状态: ", i);
			switch (checkpoint[i].state)
			{
			case SPARE: fprintf(out, "空闲中\n");	break;
			case WORKING: fprintf(out, "工作中,当前窗口人数: %d,队首%d号乘客剩余安检时间%.2fs\n", checkpoint[i].CheckQueue.size(), checkpoint[i].CheckQueue[0].ID, checkpoint[i].CheckQueue[0].CheckTime);	break;
			case WAITING: fprintf(out, "准备休息,当前窗口人数: %d,队首%d号乘客剩余安检时间%.2fs\n", checkpoint[i].CheckQueue.size(), checkpoint[i].CheckQueue[0].ID, checkpoint[i].CheckQueue[0].CheckTime);	break;
			case RESTING: fprintf(out, "休息中,剩余休息时间%.2fs\n", checkpoint[i].RestTime);	break;
			case NOTOPEN: fprintf(out, "未开放\n");	break;
			case CLOSED: fprintf(out, "已下班\n");	break;
			}
		}
	}
	if (CurrentTime >= OffdutyTime && SystemState == BEONDUTY)	//根据情况改变系统状态：工作 待下班 已下班；
	{
		SystemState = TOOFFDUTY;
		fprintf(out, "第%.2f秒:下班时间已到,准备下班\n",CurrentTime);
	}
	if (SystemState == TOOFFDUTY && buffer.empty() && CheckPointEmpty())
	{
		SystemState = BEOFFDUTY;
		fprintf(out, "第%.2f秒:乘客处理完毕,机场已下班\n",CurrentTime);

	}
}