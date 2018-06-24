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

#define SPARE 0		//����ڿ���״̬��
#define WORKING 1	//����ڹ�����״̬�����������ˣ�
#define WAITING 2	//����ڹ���̬��������ڷ�����Ϣ���뵫����������,��ʱ���ٽ��ճ˿ͣ�
#define RESTING 3	//�������Ϣ״̬��������û�ˣ�Ҳ�����ճ˿ͣ�
#define NOTOPEN 4	//����ڴ�����״̬��
#define CLOSED	5	//��������°�״̬��

#define BEONDUTY 0		//�����ϰ�״̬��
#define TOOFFDUTY 1		//����׼���°࣬��ջ������������갲���������°ࣻ
#define BEOFFDUTY 2		//�����°�״̬��

#define UP 0	//�˿������ƶ���
#define DOWN 1	//�˿������ƶ���
#define LEFT 2	//�˿������ƶ���
#define RIGHT 3	//�˿������ƶ���
#define STILL 4 //�˿�ֹͣ�ƶ���

#define SCR_WIDTH	1280	//�������ڿ�ȣ�
#define SCR_HEIGHT	720		//�������ڸ߶ȣ�

#define PASSENGER_HEIGHT 40 //�˿���ͼ�߶ȣ�
#define PASSENGER_WIDTH 40 //�˿���ͼ��ȣ�
#define ENTRANCE_X 345	//��ڴ�X����ֵ��
#define ENTRANCE_Y 675	//��ڴ�Y����ֵ��
#define BUFFER_UP_Y 425 //�������ϲ�߽�Y���ꣻ
#define BUFFER_DOWN_Y 675 //�������²�߽�Y���ꣻ
#define BUFFER_LINE_WIDTH 76//���������п�ȣ�
#define CHECK_LINE_Y	165//�����Y���ꣻ
#define PI 3.141592654


typedef struct
{
	int ID;	//�˿ͱ�ţ�
	PIMAGE pic;	//�˿Ͷ�Ӧ��ͼָ��;
	int x, y;	//�˿����ꣻ
	int dir;	//�˿��˶�����
	int LastDir;	//�˿;�ֹǰ���һ���˶�����
	double CheckTime;	//���ʱ�䣻
	double LastLineX;	//��¼����һ�����������е�X���꣬��������ƶ�����
}Passenger;
typedef struct
{
	int x, y;	//��������ꣻ
	int state;	//�����״̬��
	double RestTime;	//�������Ϣʱ�䣻
	vector<Passenger> CheckQueue;	//����ڶ�Ӧ�˿Ͷ��У�
}CheckPoint;

#endif // !__DEFINITION_H__