#include "Definition.h"
#include "Output.h"
#include "SystemControl.h"
#include "Init.h"
#include "Input.h"
using namespace std;


int MaxCustSingleLine = 7;	//缓冲区单队列最大乘客数；
int MaxLine;	//蛇形缓冲区最多有多少行 1 ~ 8；
int MaxSeqLen;	//最大允许等待长度 1 ~ 6；
int MaxCustCheck;	//安检口队伍最大长度 1 ~ 6；
int MinCheckTime;	//一次安检最短时长 1 ~ Max；     
int MaxCheckTime;  //一次安检最大时长 MinCheckTime ~ Max；
int MinRestTimeLen;	 //安检口一次休息最短时长 1 ~ Max；
int MaxRestTimeLen;	//安检口一次休息最长时长 MinRestTimeLen ~ Max；
int OffdutyTime;	//下班时间 1 ~ Max；
bool IsAutoGenerate; //是否自动产生乘客 true or false；
int Speed;	//乘客移动速度 1~5；
int SystemState = BEONDUTY;	//记录当前系统运行状态；
double CurrentTime = 0;	//记录当前时间；


vector<Passenger> buffer;	//利用vector动态数组模拟缓冲区队列；
CheckPoint checkpoint[9];	//安检口数组，共九个，0号为紧急安检口；

int main()
{
	Init();	//程序初始化
	for (; SystemState != BEOFFDUTY; delay_fps(60))
	{
		Input();
		SystemControl();
		Output();
	}
	delay_ms(1000);
	closegraph();
	return 0;
}