#pragma once
class PathNode
{
public:
	PathNode(int x, int y);
	void Set(int x, int y);
	~PathNode(void);
	int X;
	int Y;
};