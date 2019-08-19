#pragma once
/*
  code based on  AGS Array plugin -- copyright (c) 2011 Ferry Timmers.  All rights reserved.
  the AGS Array plugin is provided under the Artistic License 2.0
*/


#ifndef _PATHNODEARRAY_H
#define _PATHNODEARRAY_H

#include <vector>
#include <string>

#include "plugin/agsplugin.h"
#include "PathNode.h"

 //------------------------------------------------------------------------------

 // note: All datatypes in AGS are 32 bits (except maybe char).
 // int32 is defined in agsplugin.h as long

struct PathNodeArray
{
	int32 capacity;
	std::vector<PathNode*> data;

	// call it everytime the array length changes
	void _c() { capacity = data.capacity(); }

	PathNodeArray() : data() { _c(); }
	PathNodeArray(PathNodeArray* a) : data(a->data) { _c(); }
	PathNodeArray(int32 num, PathNode* pathnode) : data(num, pathnode) { _c(); }

	void clear() { data.clear(); _c(); }

	int32 empty() const { return data.empty(); }

	void erase(int32 pos) { data.erase(data.begin() + pos); _c(); }

	void erase(int32 first, int32 last) {
		data.erase(data.begin() + first,
			data.begin() + last);
		_c();
	}

	void insert(int32 pos, PathNode* pathnode) {
		data.insert(data.begin() + pos,
			pathnode); _c();
	}

	void insert(int32 pos, const PathNodeArray* src) {
		data.insert(data.begin() + pos,
			src->data.begin(),
			src->data.end());
		_c();
	}

	PathNode operator[](int32 pos) { return *data[pos]; _c(); }

	PathNode* pop()
	{
		PathNode* pathnode = *data.rbegin();
		data.pop_back();
		_c();
		return pathnode;
	}

	void push(PathNode* pathnode) { data.push_back(pathnode); _c(); }

	int32 size() const { return data.size(); }

	void resize(int32 num) { data.resize(num); _c(); }

	void reserve(int32 num) { data.reserve(num); _c(); }

	// Excuse to use static
	static void swap(PathNodeArray* a, PathNodeArray* b)
	{
		a->data.swap(b->data);
		a->_c();
		b->_c();
	}
};

//------------------------------------------------------------------------------
// AGS interface instances

class PathNodeArrayInterface : public IAGSScriptManagedObject
{
public:
	static const char* name;

	PathNodeArrayInterface() {};

	virtual int Dispose(const char* address, bool force);
	virtual const char* GetType() { return (name); }
	virtual int Serialize(const char* address, char* buffer, int bufsize);

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class PathNodeArrayReader : public IAGSManagedObjectReader
{
public:

	PathNodeArrayReader() {}

	virtual void Unserialize(int key, const char* serializedData, int dataSize);

};

//------------------------------------------------------------------------------

extern PathNodeArrayInterface PathNodeArray_Interface;
extern PathNodeArrayReader PathNodeArray_Reader;

//------------------------------------------------------------------------------

#endif /* _PATHNODEARRAY_H */

//..............................................................................