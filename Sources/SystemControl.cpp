#include "Definition.h"
using namespace std;

extern int MaxCustSingleLine;	//���������������˿�����
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
extern double CurrentTime;	//��¼��ǰʱ�䣻

extern FILE * out;	//����Initģ�鶨����ļ�ָ��ʵ�ֳ������й����ж��ļ��Ķ�̬д�룻

extern vector<Passenger> buffer;
extern CheckPoint checkpoint[9];

extern int SystemState;	//��¼��ǰϵͳ����״̬��
int BufferLineNum = 1; //��¼��ǰ���ζ���������
int EXIT_X[] = { 345,505,670,830 };	//����������X���ꣻ
int PassengerNum = 1;	//�˿ͱ�ţ�

double gaussrand()	//����Box-Muller�㷨���ṩ���ϱ�׼��̬�ֲ����������
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
int CheckPointNum()	//���ص�ǰ���ŵİ����������
{
	int cnt = 0;
	for (int i = 1; i <= 8; i++)
		if (checkpoint[i].state == SPARE || checkpoint[i].state == WORKING)
			cnt++;
	return cnt;
}
int ProperPointID()	//���ص�ǰ���ڹ������Ŷ��������ٵİ���ڵı�ţ�
{
	int MinID;
	for (int i = 1; i <= 8; i++)	//Ԥ����
	{
		if (checkpoint[i].state == SPARE || checkpoint[i].state == WORKING)
		{
			MinID = i;
			break;	//�����ĵ����߼���֤��������ض������е���һ����
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

bool BufferFull()	//�жϻ������Ƿ�������
{
	return buffer.size() >= MaxCustSingleLine * BufferLineNum + BufferLineNum - 1;
}
bool CheckPointFull()	//�жϰ�����Ƿ�������
{
	for (int i = 1; i <= 8; i++)
	{
		if (checkpoint[i].state == SPARE || (checkpoint[i].state == WORKING && checkpoint[i].CheckQueue.size() < MaxCustCheck))
			return  false;
	}
	return true;
}
bool CheckPointEmpty()	//�ж����а�����Ƿ�Ϊ�գ�
{
	for (int i = 0; i < 9; i++)
	{
		if (!checkpoint[i].CheckQueue.empty())
			return false;
	}
	return true;
}
bool IsInExit(Passenger * cus)	//�жϳ˿��Ƿ񵽴ﻺ�������ڣ�
{
	return abs(cus->x - EXIT_X[BufferLineNum / 2]) <= 10 && abs(cus->y - BUFFER_UP_Y) <= 10;
}

void CheckPointEnqueue(Passenger * cus, int num)	//�˿��������Ӧ��ŵİ���ڣ�
{
	if (num == 0)	//������������⴦��
	{
		if (!checkpoint[0].CheckQueue.empty() && checkpoint[0].CheckQueue.back().x > 1240)
			cus->x = checkpoint[0].CheckQueue.back().x + PASSENGER_WIDTH;
		else
		cus->x = 1280;
		cus->y = 380;
	}
	checkpoint[num].CheckQueue.push_back(*cus);
}

void NewCusEnqueue()	//�����³˿����
{
	if (SystemState != BEONDUTY)	//�����ʱ�ǹ���״̬��������Ӧ��
		return;
	Passenger tmp;
	tmp.x = ENTRANCE_X;			//��ʼ���˿����ꣻ
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
	tmp.pic = newimage(); //��ʼ���˿���ͼָ�룻
	tmp.ID = PassengerNum++;	//��ʼ���˿�ID;
	tmp.dir = UP;	//�˿ͳ�ʼ������У��ƶ��������ϣ�
	tmp.LastDir = UP;
	tmp.LastLineX = ENTRANCE_X;
	int type = random(5);	//����ege���ṩ���������������0~4֮��������������˿����ͣ�
	switch (type)
	{
	case 0:	getimage(tmp.pic, "official.png");	break;
	case 1:	getimage(tmp.pic, "youngman.png");	break;
	case 2:	getimage(tmp.pic, "middle-aged.png");	break;
	case 3:	getimage(tmp.pic, "lady1.png");		break;
	case 4:	getimage(tmp.pic, "lady2.png");	break;
	default: break;
	}

	tmp.CheckTime = (MinCheckTime + MaxCheckTime) / 2 + gaussrand();	//������̬�ֲ������ģ��ÿλ�˿͵ļ��ʱ�䣻
	if (tmp.CheckTime < MinCheckTime)	tmp.CheckTime = MinCheckTime;	//��Χ����
	if (tmp.CheckTime > MaxCheckTime)	tmp.CheckTime = MaxCheckTime;

	bool IsEmergent = (randomf() > 0.95);	//����randomf()����0.0~1.0���������ģ��ÿλ�˿�Ϊ���͵ĸ���Ϊ5%��
	if (IsEmergent && checkpoint[0].CheckQueue.size() < MaxCustCheck)	//���Ϊ�����ҽ���ͨ��δ����ֱ�ӽ����������ڣ�
	{
		fprintf(out, "��%.2f��:%d�ų˿͵������,���ͽ�����������\n",CurrentTime, tmp.ID);
		CheckPointEnqueue(&tmp, 0);
	}
	else if (!BufferFull())	//�������������δ��������뻺����
	{
		fprintf(out, "��%.2f��:%d�ų˿͵������,���뻺����\n",CurrentTime, tmp.ID);
		buffer.push_back(tmp);	//�²����˿���ӣ�
	}
	else	//������ʧ�ܣ��򽫳˿�������ԭ
	PassengerNum--;
}

void BufferDequeue()	//����������Ԫ�س��ӣ�
{
	if (!buffer.empty() && IsInExit(&buffer[0]))	//������׳˿͵������λ��
	{
		if (CheckPointFull())	//�����ʱ�����������ʹ�˿͵ȴ���
		{
			if (buffer[0].dir != STILL)
			{
				buffer[0].LastDir = buffer[0].dir;
				buffer[0].dir = STILL;
			}
		}
		else	//����˿ͳ��������������Ӧ�İ���ڶ��У�
		{
			int num = ProperPointID();
			CheckPointEnqueue(&buffer[0], num);
			fprintf(out, "��%.2f��:%d�ų˿ͳ�������,ѡ��%d�Ű����\n", CurrentTime, buffer[0].ID,num);
			buffer.erase(buffer.begin());
		}
	}
}

void MoveInDir(Passenger * cus)	//���ݵ�ǰ�˶�������ٶȸı�˿͵�����
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

void TransPosInBuffer()	//�ı�˿��ڻ������ڵ�λ��;
{
	for (int i = 0; i < buffer.size(); i++)
	{
		switch (buffer[i].dir)	//�߽�ת����
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
		if (i > 0 && buffer[i - 1].dir == STILL)	//�ƶ�ת��ֹ�жϴ���
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
		if (buffer[i].dir == STILL)	//��ֹת���ƶ��жϴ���
		{
			if (i == 0 && !IsInExit(&buffer[i]))	//����Ԫ�����⴦��
				buffer[i].dir = buffer[i].LastDir;
			if (i > 0 && buffer[i].LastDir == UP && !(abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y - 10 - PASSENGER_HEIGHT < buffer[i - 1].y))
				buffer[i].dir = UP;
			if (i > 0 && buffer[i].LastDir == DOWN && !(abs(buffer[i].x - buffer[i - 1].x) < PASSENGER_WIDTH && buffer[i].y + 10 + PASSENGER_HEIGHT > buffer[i - 1].y))
				buffer[i].dir = DOWN;
			if (i > 0 && buffer[i].LastDir == RIGHT && !(abs(buffer[i].y - buffer[i - 1].y) < PASSENGER_HEIGHT && buffer[i].x + 10 + PASSENGER_WIDTH > buffer[i - 1].x))
				buffer[i].dir = RIGHT;
		}
		MoveInDir(&buffer[i]);	//�����Ѹı���˶�����ı�˿͵����ꣻ
	}
}

void TransPosInCheckPoint()	//�ı�˿��ڰ���ڶ������λ�ã�
{
	for (int i = 0; i < checkpoint[0].CheckQueue.size(); i++)	//���ƽ�������ڳ˿��ƶ�����������ڳ˿͵�����Ϊ��ͨ�˿͵�������
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
		if (abs(checkpoint[0].CheckQueue[i].x - checkpoint[0].x) < 2 * Speed)	//������С������Speed��λ,��ֱ�Ӷ���
			checkpoint[0].CheckQueue[i].x = checkpoint[0].x;
		if (abs(checkpoint[0].CheckQueue[i].y - CHECK_LINE_Y) < 2 * Speed)
			checkpoint[0].CheckQueue[i].y = CHECK_LINE_Y;
	}
	for (int i = 1; i <= 8; i++)	//������ͨ����ڳ˿��ƶ�
	{
		for (int j = 0; j < checkpoint[i].CheckQueue.size(); j++)
		{
			if (checkpoint[i].CheckQueue[j].y > 370)		//δ��ת��������������ƶ�
				checkpoint[i].CheckQueue[j].y -= Speed;
			else if (abs(checkpoint[i].CheckQueue[j].x - checkpoint[i].x) >= Speed)	//δ������ڶ�Ӧ����������������ƶ�
				checkpoint[i].CheckQueue[j].x += (checkpoint[i].CheckQueue[j].x < checkpoint[i].x ? 1 : -1) * Speed;	//���ݳ˿ͼ������x��������λ��ʹ�˿ͺ����ƶ�
			else if (j == 0 && checkpoint[i].CheckQueue[j].y > CHECK_LINE_Y)	//���Ϊ���׳˿�������λ��δ���ﰲ���,�������ƶ�
				checkpoint[i].CheckQueue[j].y -= Speed;
			else if(j > 0 && checkpoint[i].CheckQueue[j].y - Speed - PASSENGER_HEIGHT >= checkpoint[i].CheckQueue[j-1].y)	//����Ƕ��׳˿������ϲ��ᷢ����ײ,�������ƶ�
				checkpoint[i].CheckQueue[j].y -= Speed;

			if (abs(checkpoint[i].CheckQueue[j].x - checkpoint[i].x) < Speed)	//������С��һ��Speed��λ,��ֱ�Ӷ���
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

void TransCheckPointState()	//��������ı䰲��ڵ�״̬
{
	if (SystemState == BEONDUTY && (CheckPointNum() == 0 || buffer.size() / CheckPointNum() >= MaxSeqLen))	//����������̬����һ������ڣ�
	{
		for (int i = 1; i <= 8; i++)
		{
			if (checkpoint[i].state == NOTOPEN)
			{
				fprintf(out, "��%.2f��:��������������,����%d�Ű����\n", CurrentTime, i);
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
			checkpoint[i].RestTime -= 1.0 / 60;		//��Ϣʱ�����һ֡����Ӧʱ����
			if (checkpoint[i].RestTime <= 0)		//�����Ϣ��ϣ�תΪ������״̬
			{
				checkpoint[i].state = NOTOPEN;
				fprintf(out, "��%.2f��:%d�Ű������Ϣ���,תΪ������״̬", CurrentTime, i);
			}
		}
		if (checkpoint[i].state == WAITING && checkpoint[i].CheckQueue.empty())
		{
			checkpoint[i].state = RESTING;
			checkpoint[i].RestTime = (MinRestTimeLen + MaxRestTimeLen) / 2 + gaussrand() * 2;	//������̬�ֲ������ģ��ÿ������ڵ���Ϣʱ�䣻
			if (checkpoint[i].RestTime < MinRestTimeLen)	checkpoint[i].RestTime = MinRestTimeLen;	//��Χ����
			if (checkpoint[i].RestTime > MaxRestTimeLen)	checkpoint[i].RestTime = MaxRestTimeLen;
			if(SystemState == BEONDUTY)
				fprintf(out, "��%.2f��:%d�Ű����תΪ��Ϣ״̬,��Ϣʱ��Ϊ%.2f��\n",CurrentTime, i, checkpoint[i].RestTime);
		}
	}
	if (SystemState != BEONDUTY && buffer.empty())	//����ǹ���״̬�һ����������
	{
		for (int i = 1; i <= 8; i++)
		{
			if (checkpoint[i].state == SPARE || checkpoint[i].state == RESTING || checkpoint[i].state == NOTOPEN)
			{
				checkpoint[i].state = CLOSED;
				fprintf(out, "��%.2f��:%d�Ű����תΪ���°�״̬\n",CurrentTime, i, checkpoint[i].RestTime);
			}
		}
	}
}

void CheckPointDequeue()
{
	for (int i = 0; i <= 8; i++)
	{
		if (!checkpoint[i].CheckQueue.empty()) //�������ڶ��зǿգ�
		{
			if (abs(checkpoint[i].CheckQueue[0].y == CHECK_LINE_Y))	//������׳˿��ѵ��ﰲ���
				checkpoint[i].CheckQueue[0].CheckTime -= 1.0 / 60;	//����ڰ���ʱ�����һ֡����Ӧ��ʱ��
			if (checkpoint[i].CheckQueue[0].CheckTime <= 0)	//����ʱ�䵽�����
			{
				if(i == 0)
					fprintf(out, "��%.2f��:��������ڶ���%d�ų˿Ͱ������\n", CurrentTime, checkpoint[i].CheckQueue[0].ID);
				else
					fprintf(out, "��%.2f��:%d�Ű���ڶ���%d�ų˿Ͱ������\n", CurrentTime, i, checkpoint[i].CheckQueue[0].ID);
				delimage(checkpoint[i].CheckQueue[0].pic);
				checkpoint[i].CheckQueue.erase(checkpoint[i].CheckQueue.begin());
			}
		}

	}
}

void NewOpenAdjust()	//ʵ�������������ʱ�ٽ�����ڳ˿Ͷ��еĶ�̬����
{
	if (checkpoint[1].state == SPARE)	//���߽簲����¿�ʱ��ֻ������Ҳల��ڶ���
	{
		for (int i = 2; i < checkpoint[2].CheckQueue.size(); i++)
		{
			fprintf(out, "��%.2f��:2�Ű����%d�ų˿Ͷ�̬������1�Ű����\n", CurrentTime, checkpoint[2].CheckQueue[i].ID);
			checkpoint[1].CheckQueue.push_back(checkpoint[2].CheckQueue[i]);
		}
		for (int i = checkpoint[2].CheckQueue.size() - 1; i >= 2; i--)
			checkpoint[2].CheckQueue.erase(checkpoint[2].CheckQueue.begin() + i);
	}
	if (checkpoint[8].state == SPARE)	//�Ҳ�߽簲����¿�ʱ��ֻ�������ల��ڶ���
	{
		for (int i = 2; i < checkpoint[7].CheckQueue.size(); i++)
		{
			fprintf(out, "��%.2f��:7�Ű����%d�ų˿Ͷ�̬������8�Ű����\n", CurrentTime, checkpoint[7].CheckQueue[i].ID);
			checkpoint[8].CheckQueue.push_back(checkpoint[7].CheckQueue[i]);
		}
		for (int i = checkpoint[7].CheckQueue.size() - 1; i >= 2; i--)
			checkpoint[7].CheckQueue.erase(checkpoint[7].CheckQueue.begin() + i);
	}
	for (int i = 2; i <= 7; i++)	//������༰�Ҳల��ڶ���
	{
		if (checkpoint[i].state == SPARE)
		{
			int todel1[5] = { 0 }, todel2[5] = { 0 };	//���ڼ�¼��Ҫɾ�����ڽ�����ڵĳ˿�
			int idx1 = 0, idx2 = 0;
			for (int j =  2; j < max(checkpoint[i - 1].CheckQueue.size(), checkpoint[i + 1].CheckQueue.size()); j++)	//���ల��ڶ�Ӧ�˿ͽ����¿������
			{
				int ID1 = j < checkpoint[i - 1].CheckQueue.size() ? checkpoint[i - 1].CheckQueue[j].ID : INT_MAX;
				int ID2 = j < checkpoint[i + 1].CheckQueue.size() ? checkpoint[i + 1].CheckQueue[j].ID : INT_MAX;
				if (ID1 < ID2)
				{
					fprintf(out, "��%.2f��:%d�Ű����%d�ų˿Ͷ�̬������%d�Ű����\n", CurrentTime,i-1, checkpoint[i-1].CheckQueue[j].ID,i);
					checkpoint[i].CheckQueue.push_back(checkpoint[i - 1].CheckQueue[j]);
					todel1[idx1++] = j;
				}
				else
				{
					fprintf(out, "��%.2f��:%d�Ű����%d�ų˿Ͷ�̬������%d�Ű����\n", CurrentTime, i+1, checkpoint[i+1].CheckQueue[j].ID, i);
					checkpoint[i].CheckQueue.push_back(checkpoint[i + 1].CheckQueue[j]);
					todel2[idx2++] = j;
				}
			}
			for (int j = idx1 - 1; j >= 0; j--)		//ɾ�����ల��ڶ��ж�Ӧ�˿�
				checkpoint[i - 1].CheckQueue.erase(checkpoint[i - 1].CheckQueue.begin() + todel1[j]);
			for (int j = idx2 - 1; j >= 0; j--)
				checkpoint[i + 1].CheckQueue.erase(checkpoint[i + 1].CheckQueue.begin() + todel2[j]);
		}
	}

}

void BufferControl()	//ʵ�ֶԻ��������������
{
	if (BufferFull() && BufferLineNum < MaxLine)
	{
		fprintf(out, "��%.2f��:����������,���ζ��ж�̬����\n", CurrentTime);
		BufferLineNum += 2;
	}
	BufferDequeue();	//�жϴ�����������Ԫ�س��Ӳ�����
	if (IsAutoGenerate && SystemState == BEONDUTY)	//�Զ�ģʽ�ҹ���״̬���²����³˿ͽ��뻺������
	{
		if (randomf() < 0.05 + gaussrand() / 100)	//������̬�ֲ�ģ��ÿһ֡�˿ͳ��ֵĸ��ʣ�ƽ��ÿ����������˿ͣ�
			NewCusEnqueue();
	}
	TransPosInBuffer();	//�ı�˿����꣬ʵ�ֶ���Ч����
}

void CheckPointControl()	//ʵ�ֶ԰���ڵ��������
{
	CheckPointDequeue();
	TransCheckPointState();
	NewOpenAdjust();
	TransPosInCheckPoint();
}

void SystemControl()	//��ϵͳ��������ƣ���ʵ���������ļ������;
{
	CurrentTime = fclock();	//����һ֡����Ӧ��ʱ�䣻
	CheckPointControl();
	BufferControl();
	if ((int)(CurrentTime + 1.0 / 60) > (int)CurrentTime)	//ÿ�����µ�һ�룬���������عؼ�������
	{
		fprintf(out, "\n�������е�%d��:\n", (int)(CurrentTime + 1.0 / 60) );
		fprintf(out, "��������ǰ����: %d\n", buffer.size());
		if (BufferFull() && BufferLineNum == MaxLine)
			fprintf(out, "����������\n");
		if (checkpoint[0].CheckQueue.size() > 0)
			fprintf(out, "��������ڹ�����,��ǰ��������: %d,����%d�ų˿�ʣ�ల��ʱ��%.2fs\n", checkpoint[0].CheckQueue.size(), checkpoint[0].CheckQueue[0].ID, checkpoint[0].CheckQueue[0].CheckTime);
		else
			fprintf(out, "��������ڿ�����\n");
		for (int i = 1; i <= 8; i++)
		{
			fprintf(out, "%d�Ű���ڵ�ǰ״̬: ", i);
			switch (checkpoint[i].state)
			{
			case SPARE: fprintf(out, "������\n");	break;
			case WORKING: fprintf(out, "������,��ǰ��������: %d,����%d�ų˿�ʣ�ల��ʱ��%.2fs\n", checkpoint[i].CheckQueue.size(), checkpoint[i].CheckQueue[0].ID, checkpoint[i].CheckQueue[0].CheckTime);	break;
			case WAITING: fprintf(out, "׼����Ϣ,��ǰ��������: %d,����%d�ų˿�ʣ�ల��ʱ��%.2fs\n", checkpoint[i].CheckQueue.size(), checkpoint[i].CheckQueue[0].ID, checkpoint[i].CheckQueue[0].CheckTime);	break;
			case RESTING: fprintf(out, "��Ϣ��,ʣ����Ϣʱ��%.2fs\n", checkpoint[i].RestTime);	break;
			case NOTOPEN: fprintf(out, "δ����\n");	break;
			case CLOSED: fprintf(out, "���°�\n");	break;
			}
		}
	}
	if (CurrentTime >= OffdutyTime && SystemState == BEONDUTY)	//��������ı�ϵͳ״̬������ ���°� ���°ࣻ
	{
		SystemState = TOOFFDUTY;
		fprintf(out, "��%.2f��:�°�ʱ���ѵ�,׼���°�\n",CurrentTime);
	}
	if (SystemState == TOOFFDUTY && buffer.empty() && CheckPointEmpty())
	{
		SystemState = BEOFFDUTY;
		fprintf(out, "��%.2f��:�˿ʹ������,�������°�\n",CurrentTime);

	}
}