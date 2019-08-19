/*
  code based on  AGS Array plugin -- copyright (c) 2011 Ferry Timmers.  All rights reserved.
  the AGS Array plugin is provided under the Artistic License 2.0
*/

/***************************************************************
 * STL vector wrapper                                          *
 ***************************************************************/

#include "PathNodeArray.h"

 //------------------------------------------------------------------------------

extern IAGSEngine* engine;

PathNodeArrayInterface PathNodeArray_Interface;
PathNodeArrayReader PathNodeArray_Reader;

const char* PathNodeArrayInterface::name = "PathNodeArray";

//------------------------------------------------------------------------------

int PathNodeArrayInterface::Dispose(const char* address, bool force)
{
	delete ((PathNodeArray*)address);
	return (1);
}

//------------------------------------------------------------------------------

int PathNodeArrayInterface::Serialize(const char* address, char* buffer, int bufsize)
{
	PathNodeArray* arr = (PathNodeArray*)address;
	char* ptr = buffer;

	std::vector<PathNode*>::iterator it;
	for (it = arr->data.begin(); it != arr->data.end(); ++it)
	{
		size_t length = sizeof(char)*8 + 1;

		if (ptr - buffer + length > bufsize)
			break;

		char* packedPathNode = new char[8];
		packedPathNode[0] = (const char)(0x000000ff & (*it)->X);
		packedPathNode[1] = (const char)((0x0000ff00 & (*it)->X) >> 8);
		packedPathNode[2] = (const char)((0x00ff0000 & (*it)->X) >> 16);
		packedPathNode[3] = (const char)((0xff000000 & (*it)->X) >> 24);
		packedPathNode[4] = (const char)(0x000000ff & (*it)->Y);
		packedPathNode[5] = (const char)((0x0000ff00 & (*it)->Y) >> 8);
		packedPathNode[6] = (const char)((0x00ff0000 & (*it)->Y) >> 16);
		packedPathNode[7] = (const char)((0xff000000 & (*it)->Y) >> 24);
		
		memcpy(ptr, packedPathNode, length);
		ptr += length;
	}

	return (ptr - buffer);
}

//------------------------------------------------------------------------------

void PathNodeArrayReader::Unserialize(int key, const char* serializedData, int dataSize)
{
	PathNodeArray* arr = new PathNodeArray();

	const char* ptr = serializedData;
	while (ptr - serializedData < dataSize)
	{
		int* unpackedPathNode = new int[8];
		unpackedPathNode[0] = 0x000000ff & ( *ptr );
		unpackedPathNode[1] = 0x0000ff00 & ( *(ptr+1) <<8 );
		unpackedPathNode[2] = 0x00ff0000 & ( *(ptr+2) << 16 );
		unpackedPathNode[3] = 0xff000000 & ( *(ptr+3) << 24 );
		unpackedPathNode[4] = 0x000000ff & (*(ptr+4));
		unpackedPathNode[5] = 0x0000ff00 & (*(ptr+5) << 8);
		unpackedPathNode[6] = 0x00ff0000 & (*(ptr+6) << 16);
		unpackedPathNode[7] = 0xff000000 & (*(ptr+7) << 24);

		PathNode* aPathNode = new PathNode(unpackedPathNode[0] + unpackedPathNode[1] + unpackedPathNode[2] + unpackedPathNode[3],
			unpackedPathNode[4] + unpackedPathNode[5] + unpackedPathNode[6] + unpackedPathNode[7]);

		arr->push(aPathNode);
		ptr += 8 + 1;
	}

	engine->RegisterUnserializedObject(key, arr, &PathNodeArray_Interface);
}

//..............................................................................