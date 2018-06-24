#pragma once
#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

double gaussrand();
int CheckPointNum();
int ProperPointID();

bool CheckPointFull();
bool CheckPointEmpty();
bool BufferFull();
bool IsInExit(Passenger * cus);

void CheckPointEnqueue(Passenger * cus, int num);
void NewCusEnqueue();
void BufferDequeue();
void MoveInDir(Passenger * cus);
void TransPosInBuffer();
void TransPosInCheckPoint();
void TransCheckPointState();
void BufferControl();
void CheckPointDequeue();
void NewOpenAdjust();
void CheckPointControl();
void SystemControl();

#endif