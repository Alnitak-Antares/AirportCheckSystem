#include "Definition.h"
#include "Output.h"
#include "SystemControl.h"
#include "Init.h"
#include "Input.h"
using namespace std;


int MaxCustSingleLine = 7;	//���������������˿�����
int MaxLine;	//���λ���������ж����� 1 ~ 8��
int MaxSeqLen;	//�������ȴ����� 1 ~ 6��
int MaxCustCheck;	//����ڶ�����󳤶� 1 ~ 6��
int MinCheckTime;	//һ�ΰ������ʱ�� 1 ~ Max��     
int MaxCheckTime;  //һ�ΰ������ʱ�� MinCheckTime ~ Max��
int MinRestTimeLen;	 //�����һ����Ϣ���ʱ�� 1 ~ Max��
int MaxRestTimeLen;	//�����һ����Ϣ�ʱ�� MinRestTimeLen ~ Max��
int OffdutyTime;	//�°�ʱ�� 1 ~ Max��
bool IsAutoGenerate; //�Ƿ��Զ������˿� true or false��
int Speed;	//�˿��ƶ��ٶ� 1~5��
int SystemState = BEONDUTY;	//��¼��ǰϵͳ����״̬��
double CurrentTime = 0;	//��¼��ǰʱ�䣻


vector<Passenger> buffer;	//����vector��̬����ģ�⻺�������У�
CheckPoint checkpoint[9];	//��������飬���Ÿ���0��Ϊ��������ڣ�

int main()
{
	Init();	//�����ʼ��
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