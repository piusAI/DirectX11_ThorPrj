#pragma once

#include <queue>
using namespace std;

struct POS
{
	float X = 0;
	float Y = 0;
	float Z = 0;
};

struct MoveQueue
{
	//static MoveQueue* MoveQueueInstance;
	//MoveQueue();


	// initialize Position
	static queue<POS> MoveQ;

};
