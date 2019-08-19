#include "PathNode.h"

PathNode::PathNode(int x, int y)
{
	X = x;
	Y = y;
}

void PathNode::Set(int x, int y)
{
	X = x;
	Y = y;
}

void PathNode::SetSize(int size)
{
	Size = size;
}

PathNode::~PathNode(void)
{
}