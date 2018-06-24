#include "Definition.h"

extern int state_x[]; //��¼�����״̬��ͼx����

extern int SpShift[];	//�˿��ƶ��ٶȵ�λ
extern int ShiftNum;	//�˿����ٵ�λ��Ǳ���
extern int Speed;	//ʵ���ֶ������˿��ٶȣ�
extern double CurrentTime;	//��ȡ��ǰʱ�䣻
extern int SystemState;	//ʵ���ֶ�����ϵͳ�°ࣻ
extern bool IsAutoGenerate;	//����Ƿ��ֶ�����˿ͣ�
extern void NewCusEnqueue();	//����Controlģ���³˿���Ӻ���ʵ���ֶ������˿ͣ�

extern CheckPoint checkpoint[9];
extern FILE * out;	//����Initģ�鶨����ļ�ָ��ʵ�ֳ������й����ж��ļ��Ķ�̬д�룻

void Input()
{
	mouse_msg msg;
	while (mousemsg())
	{
		msg = getmouse();
		if (msg.is_down())
		{
			if (msg.y >= 22 && msg.y <= 55)	//�ж��Ƿ���İ����״̬
			{
				for (int i = 1; i <= 8; i++)
				{
					if (msg.x >= state_x[i] && msg.x <= state_x[i] + 80)	//�������ڰ����״̬��
					{
						switch (checkpoint[i].state)	//����������İ����״̬
						{
						case SPARE:	 fprintf(out, "��%.2f��:��%d�Ű���ڷ�����Ϣָ��,�����׼����Ϣ\n", CurrentTime, i); checkpoint[i].state = WAITING;	break;
						case WORKING: fprintf(out, "��%.2f��:��%d�Ű���ڷ�����Ϣָ��,�����׼����Ϣ\n", CurrentTime, i);	checkpoint[i].state = WAITING;	break;
						case NOTOPEN:	fprintf(out, "��%.2f��:��%d�Ű���ڷ�������ָ��\n", CurrentTime, i);	checkpoint[i].state = SPARE;	break;
						default:	break;
						}
					}
				}
			}
			else if (msg.x >= 220 && msg.x <= 300 && msg.y >= 630 && msg.y <= 715)	//����������ӳ˿Ͱ�ť
			{
				fprintf(out, "��%.2f��:������ֶ���ӳ˿Ͱ�ť\n", CurrentTime);
				NewCusEnqueue();	//�²����˿����
			}
			else if (SystemState == BEONDUTY && msg.x >= 1180 && msg.x <= 1280 && msg.y >= 630 && msg.y <= 720)	//��������ϰ�״̬�ҵ�����°ఴť
			{
				fprintf(out, "��%.2f��:������°ఴť,׼���°�\n", CurrentTime);
				SystemState = TOOFFDUTY;
			}
			else if (msg.x >= 200 && msg.x <= 240 && msg.y >= 515 && msg.y <= 580 && ShiftNum < 5)	//�������˳˿��������ӣ�
			{
				fprintf(out, "��%.2f��:����˳˿���������,��ǰ�ٶ�%d->%d\n", CurrentTime, ShiftNum, ShiftNum+1);
				Speed = SpShift[++ShiftNum];
			}
			else if (msg.x >= 0 && msg.x <= 40 && msg.y >= 515 && msg.y <= 580 && ShiftNum > 1)	//�������˳˿����ټ��٣�
			{
				fprintf(out, "��%.2f��:����˳˿����ټ���,��ǰ�ٶ�%d->%d\n", CurrentTime, ShiftNum, ShiftNum-1);
				Speed = SpShift[--ShiftNum];
			}
		}
	}
}