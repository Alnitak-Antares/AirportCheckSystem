#include "Definition.h"

extern PIMAGE cus;	//��ӳ˿Ͱ�ť
extern PIMAGE on;		//����ڹ���״̬��ͼ
extern PIMAGE off;	//�����δ����״̬��ͼ
extern PIMAGE resting;	//�������Ϣ״̬��ͼ
extern PIMAGE waiting;	//�����׼����Ϣ״̬��ͼ
extern PIMAGE closed;	//��������°�״̬��ͼ
extern PIMAGE fence;	//������Χ����ͼ
extern PIMAGE bk1;	//���򱳾���ͼ
extern PIMAGE bk2;
extern PIMAGE bk3;
extern PIMAGE bk4;

extern MUSIC mus;		//�������ֶ���


extern int BufferLineNum;	//��ǰ���ζ�������
extern int SystemState;	//��ȡϵͳ����״̬�������Ա������Ӧ����
extern int ShiftNum;	//�˿����ٵ�λ��Ǳ���
extern double CurrentTime;
extern vector<Passenger> buffer;
extern bool IsAutoGenerate;
extern CheckPoint checkpoint[9];
int state_x[9] = {0,25,150,275,400,527,654,779,904 }; //��¼�����״̬��ͼx����
char MyClock[20] = { 0 };

void Output()
{
	cleardevice();
	switch (BufferLineNum)	//�������ζ�������̬����������ͼ��
	{
	case 1:	putimage(0, 0, bk1); break;
	case 3:	putimage(0, 0, bk2); break;
	case 5:	putimage(0, 0, bk3); break;
	case 7:	putimage(0, 0, bk4); break;
	default: break;
	}

	putimage_transparent(NULL, cus,200,615, 0x0);	//�����ӳ˿͵İ�ť��ͼ
	if(SystemState != BEONDUTY)
		putimage_transparent(NULL, fence, 323, 675, 0x0);	//����ǹ���״̬��������һ��Χ����ͼ��ʾ�������ѹر�
	for (int i = 1; i <= 8; i++)	//��������״̬��ͼ��
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
	for (int i = 0; i < buffer.size(); i++)	//������ζ��еĳ˿ͣ�
	{
		putimage_transparent(NULL, buffer[i].pic, buffer[i].x, buffer[i].y, 0X0);
	}
	setfontbkcolor(EGERGB(216,213,208));	//���ó˿�ID������ʽ
	setfont(18, 0, "����");
	for (int i = 0; i <= 8; i++)	//�������ڵĳ˿ͣ�
	{
		for (int j = 0; j < checkpoint[i].CheckQueue.size(); j++)
		{
			putimage_transparent(NULL, checkpoint[i].CheckQueue[j].pic, checkpoint[i].CheckQueue[j].x, checkpoint[i].CheckQueue[j].y, 0X0);
			if (checkpoint[i].CheckQueue[j].x == checkpoint[i].x)	//����˿������ڰ���ڶ���,�������
			{
				char s[5] = { 0 };
				sprintf_s(s, "%d", checkpoint[i].CheckQueue[j].ID);
				outtextxy(checkpoint[i].CheckQueue[j].x + 40, checkpoint[i].CheckQueue[j].y + 23, s);
			}
		}
	}

	char s[10] = { 0 };
	setfontbkcolor(WHITE);	//�����ǰ�ٶȲ�����
	setfont(22, 0, "����");
	sprintf_s(s, "%d", ShiftNum);
	outtextxy(115, 548, s);

	setfontbkcolor(EGERGB(139, 217, 236));	//��������ʣ�ల��ʱ�����Ϣʣ��ʱ��
	if (!checkpoint[0].CheckQueue.empty() && checkpoint[0].CheckQueue[0].y - 10 <= CHECK_LINE_Y)	//������������⴦��
	{
		setfont(40, 0, "����");
		char s[10] = { 0 };
		sprintf_s(s, "%.1f", checkpoint[0].CheckQueue[0].CheckTime);
		outtextxy(1030, 185, s);
	}
	setfont(20, 0, "����");
	for (int i = 1; i <= 8; i++)
	{
		char s[10] = { 0 };
		if (!checkpoint[i].CheckQueue.empty() && checkpoint[i].CheckQueue[0].y == CHECK_LINE_Y)	//�������ڷǿ��Ҷ��׳˿��ѵ��ﰲ��ڣ��������ʱ��
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

	setfontbkcolor(EGERGB(255, 199, 100));	//�����������ʱ��������ʽ
	setfont(30, 0, "������");
	if (SystemState == BEONDUTY)	//���ݳ�������״̬�����Ӧ����
	{
		int min = (int)CurrentTime / 60, sec = (int)CurrentTime % 60, ms = (int)(CurrentTime * 100) % 100;	//��ʽ����ǰʱ�䣬��ȷ������
		sprintf_s(MyClock, "%02d:%02d:%02d", min, sec, ms);
		outtextxy(45, 660, MyClock);
	}
	if(SystemState == TOOFFDUTY)
		outtextxy(45, 660, "׼���°�");
	if(SystemState == BEOFFDUTY)
		outtextxy(55, 660, "���°�");

	if (mus.GetPlayStatus() == MUSIC_MODE_STOP) // ���ֱ�������ֹͣ�˾����²���
		mus.Play(0);
}