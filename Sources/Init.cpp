#include "Definition.h"

extern int MaxLine;	//���λ���������ж����У�
extern int MaxSeqLen;	//�������ȴ����ȣ�
extern int MaxCustCheck;	//����ڶ�����󳤶ȣ�
extern int MinCheckTime;	//һ�ΰ������ʱ����     
extern int MaxCheckTime;  //һ�ΰ������ʱ����
extern int MinRestTimeLen;	 //�����һ����Ϣ���ʱ����
extern int MaxRestTimeLen;	//�����һ����Ϣ�ʱ����
extern int OffdutyTime;	//�°�ʱ�� 1 ~ Max��
extern bool IsAutoGenerate; //�Ƿ��Զ������˿ͣ�
extern int Speed;	//�˿��ƶ��ٶȣ�

extern CheckPoint checkpoint[9];

int SpShift[6] = { 0,2,4,5,8,10 };	//�˿��ƶ��ٶȵ�λ
int ShiftNum = 1;	//�˿��ƶ��ٶȵ�λ��Ǳ���
FILE * out;	//�����ļ�ָ�� ���ڴ�Log��־�ļ�

MUSIC mus;		//�������ֶ���

PIMAGE cus;	//��ӳ˿Ͱ�ť
PIMAGE on;		//����ڹ���״̬��ͼ
PIMAGE off;	//�����δ����״̬��ͼ
PIMAGE waiting;	//�����׼����Ϣ״̬��ͼ
PIMAGE resting;	//�������Ϣ״̬��ͼ
PIMAGE closed;	//����ڹر�״̬��ͼ
PIMAGE fence;	//������Χ����ͼ
PIMAGE bk1;	//����ͼƬ
PIMAGE bk2;
PIMAGE bk3;
PIMAGE bk4;

void SetData()	//��ʼ����������꣬��ͼ������
{
	cus = newimage();	//��ӳ˿Ͱ�ť
	on = newimage();		//����ڹ���״̬��ͼ
	off = newimage();	//�����δ����״̬��ͼ
	waiting = newimage();	//�����׼����Ϣ״̬��ͼ
	resting = newimage();	//�������Ϣ״̬��ͼ
	closed = newimage();	//��������°�״̬��ͼ
	fence = newimage();		//������Χ����ͼ
	bk1 = newimage();	//����ͼƬ
	bk2 = newimage();
	bk3 = newimage();
	bk4 = newimage();
	getimage(on, "ON.png");		//��ʼ����ͼָ�룬��Outputģ���е��ã�
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

	for (int i = 0; i <= 8; i++)	//��ʼ������ڶ�Ӧ���꼰״̬����
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

	PIMAGE pic_fail = newimage();	//���ļ�ʧ�ܿ�ܣ�*
	getimage(pic_fail, "fail.png");

	errno_t err;
	err = fopen_s(&out, "log.txt", "w");	//��log��־�ļ�
	if (err != 0)
	{
		putimage(0, 0, pic_fail);	//���ļ�ʧ�������
		fprintf(out, "���ļ�ʧ�ܣ�\n");
		delay_ms(1000);
		exit(EXIT_FAILURE);
	}

	time_t Time;	//���ڻ�ȡ��ǰ��Ȼʱ�䣻
	struct tm ITime;
	time(&Time);
	localtime_s(&ITime, &Time);

	//��log�ļ��������룻
	fprintf(out, "%d��%d��%d��%dʱ%d��%02d�����ʼ���У����ò������£�\n", ITime.tm_year + 1900, ITime.tm_mon + 1, ITime.tm_mday, ITime.tm_hour, ITime.tm_min, ITime.tm_sec);

	fprintf(out, "���������ֱ��������%d\n", MaxLine);

	fprintf(out, "�������ȴ����ȣ�%d\n", MaxSeqLen);

	fprintf(out, "����ڶ������ȴ����ȣ�%d\n", MaxCustCheck);

	fprintf(out, "һ�ΰ����ʱ��(��λΪ��)��%d\n", MaxCheckTime);

	fprintf(out, "һ�ΰ������ʱ��(��λΪ��)��%d\n", MinCheckTime);

	fprintf(out, "һ����ͣ��Ϣ�ʱ��(��λΪ��)��%d\n", MaxRestTimeLen);

	fprintf(out, "һ����ͣ��Ϣ���ʱ��(��λΪ��)��%d\n", MinRestTimeLen);

	fprintf(out, "�°�ʱ��(��λΪ��)��%d\n", OffdutyTime);

	fprintf(out, "�Ƿ��Զ������˿ͣ�1������ 0�����: %d\n", IsAutoGenerate);

	fprintf(out, "�˿��ƶ��ٶȣ�%d\n\n", ShiftNum);

	delimage(pic_fail);	//ɾ����Ӧ��ͼָ�룻
}
void InputConfig()
{
	PIMAGE p1 = newimage();	//ѡ�����ͼ���ļ������ֶ����룩��*
	PIMAGE p2 = newimage();	//�ֶ�������棻
	PIMAGE p3 = newimage();	//�����ļ���ʾ�����
	PIMAGE p4 = newimage();	//����ѡ���ֶ�����ͼ����*
	PIMAGE p5 = newimage();	//�װ壨����ȥ���Ѿ�д�ϵ����֣���
	getimage(p1, "interface1.png");
	getimage(p2, "interface2.png");
	getimage(p3, "interface3.png");
	getimage(p4, "maninput.png");
	getimage(p5, "blank.png");
	setfont(25, 0, "����");
	setbkcolor(EGERGB(0xFF, 0xFF, 0xFF));
	setcolor(BLACK);
	int flag = 1;
	while (flag)
	{
		putimage(0, 0, p1);
		mouse_msg msg = { 0 };
		while (!msg.is_down())
			msg = getmouse();
		if (msg.x <= 1080 && msg.x >= 750 && msg.y <= 460 && msg.y >= 382)	//�жϵ���Ƿ����ֶ�������ڣ�(����δȷ��)
		{
			cleardevice();
			putimage(0, 0, p4);	//ѡ�����ֶ���(����δȷ��)
			delay_ms(500);
			cleardevice();
			putimage(0, 0, p2);
			FILE * in;
			errno_t er;
			er = fopen_s(&in, "default_config.txt", "r");

			int variable[10] = { 7,4,5,5,2,10,5,200,1,3 };	//����10�������ĳ�ʼֵ��

			for (int i = 0; i <= 9; i++)	//����ʼ10�������ĳ�ʼֵ��
			{
				if (i != 7 && i != 8 && i != 9)
				{
					char d[5] = { 0 };
					sprintf_s(d, "%d", variable[i]);
					outtextxy(425, 28 + i * 80, d);//ѭ��һ�μ�һ����y��ֱ��10��������ʼֵȫ�����ꣻ
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
						outtextxy(425, 28 + i * 80, "��");
					else
						outtextxy(425, 28 + i * 80, "��");
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
				if (msg.x <= 1240 && msg.x >= 930 && msg.y <= 560 && msg.y >= 370)	//��������ȷ����ť��
				{
					flag1 = 0;	//�ر�ѭ�������������
					cleardevice();
				}
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 60 && msg.y >= 20)	//��������MaxLine +
				{
					if (MaxLine < 7)
					{
						MaxLine += 2;
						putimage_transparent(NULL, p5, 396, 28, 0X0);//�����Ѿ��е����֣�
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxLine);
						outtextxy(425, 28, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 60 && msg.y >= 20)	//��������MaxLine -
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
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 140 && msg.y >= 100 && MaxSeqLen < MaxCustCheck)	//��������MaxSeqLen +
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
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 140 && msg.y >= 100)	//��������MaxSeqLen -
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
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 220 && msg.y >= 180)	//��������MaxCustCheck +
				{
					if (MaxCustCheck < 5)
					{
						MaxCustCheck++;
						putimage_transparent(NULL, p5, 396, 28 + 2 * 80, 0X0);//�����Ѿ��е����֣�
						char s[5] = { 0 };
						sprintf_s(s, "%d", MaxCustCheck);
						outtextxy(425, 28 + 2 * 80, s);
					}
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 220 && msg.y >= 180 && MaxCustCheck > MaxSeqLen)	//��������MaxCustCheck -
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

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 300 && msg.y >= 260)	//��������MaxCheckTime +
				{
					MaxCheckTime++;
					putimage_transparent(NULL, p5, 396, 28 + 3 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", MaxCheckTime);
					outtextxy(425, 28 + 3 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 300 && msg.y >= 260)	//��������MaxCheckTime -
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
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 380 && msg.y >= 340)	//��������MinCheckTime +
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
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 380 && msg.y >= 340)	//��������MinCheckTime -
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

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 460 && msg.y >= 420)	//��������MaxRestTimeLen +
				{
					MaxRestTimeLen++;
					putimage_transparent(NULL, p5, 396, 28 + 5 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", MaxRestTimeLen);
					outtextxy(425, 28 + 5 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 460 && msg.y >= 420)	//��������MaxRestTimeLen -
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
				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 540 && msg.y >= 500)	//��������MinRestTimeLen +
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
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 540 && msg.y >= 500)	//��������MinRestTimeLen -
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

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 620 && msg.y >= 580)	//��������OffdutyTime +
				{
					OffdutyTime += 10;
					putimage_transparent(NULL, p5, 396, 28 + 7 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", OffdutyTime);
					outtextxy(415, 28 + 7 * 80, s);
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 620 && msg.y >= 580)	//��������OffdutyTime -
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

				else if (msg.x <= 550 && msg.x >= 500 && msg.y <= 700 && msg.y >= 660)	//��������IsAutoGenerate +
				{
					IsAutoGenerate = !IsAutoGenerate;
					putimage_transparent(NULL, p5, 396, 28 + 8 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", IsAutoGenerate);
					if (!IsAutoGenerate)
						outtextxy(425, 28 + 8 * 80, "��");
					else
						outtextxy(425, 28 + 8 * 80, "��");
				}
				else if (msg.x <= 370 && msg.x >= 320 && msg.y <= 700 && msg.y >= 660)	//��������IsAutoGenerate -
				{
					IsAutoGenerate = !IsAutoGenerate;
					putimage_transparent(NULL, p5, 396, 28 + 8 * 80, 0X0);
					char s[5] = { 0 };
					sprintf_s(s, "%d", IsAutoGenerate);
					if (!IsAutoGenerate)
						outtextxy(425, 28 + 8 * 80, "��");
					else
						outtextxy(425, 28 + 8 * 80, "��");
				}
				else if (msg.x <= 1200 && msg.x >= 1150 && msg.y <= 700 && msg.y >= 660)	//��������Speed +
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
				else if (msg.x <= 1250 && msg.x >= 975 && msg.y <= 700 && msg.y >= 660)	//��������Speed -
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
			flag = 0;	//��ʼ�����������ü���ѭ����
		}
		else if (msg.x <= 1084 && msg.x >= 750 && msg.y <= 580 && msg.y >= 514)	//�жϵ���Ƿ����ļ����ÿ��ڣ�(����δȷ��)
		{
			FILE * in;
			errno_t er;
			er = fopen_s(&in, "default_config.txt", "r");

			fscanf_s(in, " %d  %d  %d  %d  %d  %d  %d  %d  %d  %d", &MaxLine, &MaxSeqLen, &MaxCustCheck, &MaxCheckTime,
				&MinCheckTime, &MaxRestTimeLen, &MinRestTimeLen, &OffdutyTime, &IsAutoGenerate, &ShiftNum);	//�������������
			Speed = SpShift[ShiftNum];
			putimage(0, 0, p3);	//��ѡ���ļ�����,��ʾ���ò�����
			delay_ms(5000);
			flag = 0;	//��ʼ�����������ü���ѭ����
		}

	}

	delimage(p1);	//ɾ����Ӧ��ͼָ�룻
	delimage(p2);
	delimage(p3);
	delimage(p4);
	delimage(p5);
}
void Init()
{
	initgraph(SCR_WIDTH, SCR_HEIGHT); //��ʼ��ͼ�ν��棻
	randomize();	//��ʼ����������ӣ�
	InputConfig();	//����������ã�
	mus.OpenFile("BGM.mp3"); //�������ļ���
	mus.SetVolume(0.8f); //����������
	SetData();	//���ó������������������
}