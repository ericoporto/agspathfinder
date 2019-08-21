// dllmain.cpp : Defines the entry point for the DLL application.
#pragma region Defines_and_Includes

#include "core/platform.h"

#define MIN_EDITOR_VERSION 1
#define MIN_ENGINE_VERSION 3

#define DEFAULT_RGB_R_SHIFT_32  16
#define DEFAULT_RGB_G_SHIFT_32  8
#define DEFAULT_RGB_B_SHIFT_32  0
#define DEFAULT_RGB_A_SHIFT_32  24

#if AGS_PLATFORM_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <cstring>

#if !defined(BUILTIN_PLUGINS)
#define THIS_IS_THE_PLUGIN
#endif

#include "plugin/agsplugin.h"
#include "route_finder_jps.inl"

#if defined(BUILTIN_PLUGINS)
namespace agspathfinder {
#endif

#if !AGS_PLATFORM_OS_WINDOWS
#define min(x,y) (((x) < (y)) ? (x) : (y))
#define max(x,y) (((x) > (y)) ? (x) : (y))
#endif

#if AGS_PLATFORM_OS_WINDOWS
	// The standard Windows DLL entry point

	BOOL APIENTRY DllMain(HANDLE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved) {

		switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
	}
#endif

#pragma region Color_Functions


	int getr32(int c)
	{
		return ((c >> DEFAULT_RGB_R_SHIFT_32) & 0xFF);
	}


	int getg32(int c)
	{
		return ((c >> DEFAULT_RGB_G_SHIFT_32) & 0xFF);
	}


	int getb32(int c)
	{
		return ((c >> DEFAULT_RGB_B_SHIFT_32) & 0xFF);
	}


	int geta32(int c)
	{
		return ((c >> DEFAULT_RGB_A_SHIFT_32) & 0xFF);
	}


	int makeacol32(int r, int g, int b, int a)
	{
		return ((r << DEFAULT_RGB_R_SHIFT_32) |
			(g << DEFAULT_RGB_G_SHIFT_32) |
			(b << DEFAULT_RGB_B_SHIFT_32) |
			(a << DEFAULT_RGB_A_SHIFT_32));
	}

#pragma endregion


//define engine
IAGSEngine* engine;

#include "PathNode.h"

class ManagedPathNodeInterface : public IAGSScriptManagedObject {
public:
	ManagedPathNodeInterface() {};

	virtual int Dispose(const char* address, bool force) {
		delete address;
		return 1;
	}
	virtual const char* GetType() {
		return typeName;
	}
	virtual int Serialize(const char* address, char* buffer, int bufsize) {
		memcpy(buffer, address, sizeof(PathNode));
		return sizeof(PathNode);
	}

	static const char* typeName;
};

const char* ManagedPathNodeInterface::typeName = "PathNode";
ManagedPathNodeInterface gManagedPathNodeInterface;

class ManagedPathNodeReader : public IAGSManagedObjectReader {
public:
	ManagedPathNodeReader() {}

	virtual void Unserialize(int key, const char* serializedData, int dataSize) {
		PathNode* newPathNode = new PathNode(0, 0);
		memcpy(newPathNode, serializedData, sizeof(PathNode));
		engine->RegisterUnserializedObject(key, newPathNode, &gManagedPathNodeInterface);
	}
};

ManagedPathNodeReader gManagedPathNodeReader;

void PathNode_set_X(PathNode* obj, int val) {
	obj->X = val;
}

int PathNode_get_X(PathNode* obj) {
	return obj->X;
}

void PathNode_set_Y(PathNode* obj, int val) {
	obj->Y = val;
}

int PathNode_get_Y(PathNode* obj) {
	return obj->Y;
}

PathNode* CreatePathNode(int X, int Y) {
	PathNode* newPathNode = new PathNode(X, Y);
	engine->RegisterManagedObject(newPathNode, &gManagedPathNodeInterface);
	return newPathNode;
}

// -- do PathNodeArray stuff
#include "PathNodeArray.h"


//------------------------------------------------------------------------------

PathNodeArray* PathNodeArray_Create(int32 count, PathNode* pathNode)
{
	PathNodeArray* arr;

	if (count < 1)
		arr = new PathNodeArray();
	else if (pathNode == NULL)
		arr = new PathNodeArray(count, CreatePathNode(-1,-1));
	else
		arr = new PathNodeArray(count, pathNode);

	engine->RegisterManagedObject(arr, &PathNodeArray_Interface);

	return (arr);
}

//------------------------------------------------------------------------------

PathNodeArray* PathNodeArray_Copy(PathNodeArray* source)
{
	PathNodeArray* arr;

	if (source == NULL)
		arr = new PathNodeArray();
	else
		arr = new PathNodeArray(source);

	engine->RegisterManagedObject(arr, &PathNodeArray_Interface);

	return (arr);
}

//------------------------------------------------------------------------------

void PathNodeArray_Swap(PathNodeArray* a, PathNodeArray* b)
{
	PathNodeArray::swap(a, b);
}

//------------------------------------------------------------------------------

void PathNodeArray_Clear(PathNodeArray* arr)
{
	arr->clear();
}

//------------------------------------------------------------------------------

int32 PathNodeArray_Empty(PathNodeArray* arr)
{
	return arr->empty();
}

//------------------------------------------------------------------------------

void PathNodeArray_Erase(PathNodeArray* arr, int32 pos, int32 number)
{
	if (pos < 0)
		pos += arr->size();

	if (pos >= arr->size())
		pos = arr->size() - 1;

	if (number < 2)
		arr->erase(pos);
	else
	{
		number += pos;
		if (number < 0)
			number = 0;
		else if (number > arr->size())
			number = arr->size();

		arr->erase(pos, number);
	}
}

//------------------------------------------------------------------------------

void PathNodeArray_Insert(PathNodeArray* arr, int32 pos, PathNode* pathNode)
{
	if (pos < 0)
		pos += arr->size();

	if (pos > arr->size())
		pos = arr->size();

	if (pathNode == NULL)
		arr->insert(pos, CreatePathNode(-1,-1));
	else
		arr->insert(pos, pathNode);
}

//------------------------------------------------------------------------------

void PathNodeArray_InsertPathNodeArray(PathNodeArray* arr, int32 pos, PathNodeArray* source)
{
	if (pos < 0)
		pos += arr->size();

	if (pos > arr->size())
		pos = arr->size();

	arr->insert(pos, source);
}

//------------------------------------------------------------------------------

PathNode* PathNodeArray_GetItems(PathNodeArray* arr, int32 i)
{
	if ((i < 0) || (i > arr->size()))
		return  CreatePathNode(-1,-1);

	PathNode * pathNode = (*arr).data[i];
	engine->RegisterManagedObject(pathNode, &gManagedPathNodeInterface);
	return pathNode;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PathNodeArray_SetItems(PathNodeArray* arr, int32 i, PathNode* pathNode)
{
	if ((i < 0) || (i > arr->size()))
		return;

	(*arr)[i] = *pathNode;
}

//------------------------------------------------------------------------------

PathNode* PathNodeArray_Pop(PathNodeArray* arr)
{
	if (!arr->empty()) {
		PathNode* pathNode = arr->pop();
		engine->RegisterManagedObject(pathNode, &gManagedPathNodeInterface);
		return pathNode;
	}
	else {
		return (0);
	}
}

//------------------------------------------------------------------------------

void PathNodeArray_Push(PathNodeArray* arr, PathNode* pathNode)
{
	arr->push(pathNode == NULL ? CreatePathNode(0,0) : pathNode);
}

//------------------------------------------------------------------------------

int32 PathNodeArray_GetSize(PathNodeArray* arr)
{
	return arr->size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PathNodeArray_SetSize(PathNodeArray* arr, int32 size)
{
	arr->resize(size);
}

//------------------------------------------------------------------------------

void PathNodeArray_Reserve(PathNodeArray* arr, int32 number)
{
	arr->reserve(number);
}

// -- do pathfinding
#define MAKE_INTCOORD(x,y) (((unsigned short)x << 16) | ((unsigned short)y))

const int MAXNAVPOINTS = 100;
int navpoints[MAXNAVPOINTS];
int num_navpoints;

int lastcx;
int lastcy;

Navigation nav;

bool can_see_from(int x1, int y1, int x2, int y2)
{
	lastcx = x1;
	lastcy = y1;

	if ((x1 == x2) && (y1 == y2))
		return 1;

	return !nav.TraceLine(x1, y1, x2, y2, lastcx, lastcy);
}

// new routing using JPS
PathNodeArray* find_route_jps(int fromx, int fromy, int destx, int desty)
{
	static std::vector<int> path, cpath;
	path.clear();
	cpath.clear();

	if (nav.NavigateRefined(fromx, fromy, destx, desty, path, cpath) == Navigation::NAV_UNREACHABLE)
		return 0;

	num_navpoints = 0;

	// new behavior: cut path if too complex rather than abort with error message
	int count = std::min<int>((int)cpath.size(), MAXNAVPOINTS);

	PathNodeArray* pathNodeArr = new PathNodeArray();

	for (int i = 0; i < count; i++)
	{
		int x, y;
		nav.UnpackSquare(cpath[i], x, y);

		pathNodeArr->push(new PathNode(x, y));

	}

	return pathNodeArr;
}

void AgsPathfinder_SetGridFromSprite(int sprite) {
	BITMAP* sprBitmap = engine->GetSpriteGraphic(sprite);
	int sprWidth, sprHeight, colorDepth;

	engine->GetBitmapDimensions(sprBitmap, &sprWidth, &sprHeight, &colorDepth);

	while (!nav.map.empty()) {
		unsigned char* aRow = nav.map.back();
		nav.map.pop_back();
		delete aRow;
	}

	unsigned char** sprCharBuffer = engine->GetRawBitmapSurface(sprBitmap);
	unsigned int** sprLongBuffer = (unsigned int**)sprCharBuffer;

	nav.Resize(sprWidth, sprHeight);

	for (int y = 0; y < sprHeight; y++) {
		nav.map[y] = new unsigned char[sprWidth];
		for (int x = 0; x < sprWidth; x++) {
			if (colorDepth == 32) {
				nav.map[y][x] = (unsigned char)getr32(sprLongBuffer[y][x]);
			}
		}
	}

	engine->ReleaseBitmapSurface(sprBitmap);
}

PathNodeArray* AgsPathfinder_GetPathFromTo(int origin_x, int origin_y, int destination_x, int destination_y) {
	num_navpoints = 0;
	int srcx = origin_x;
	int srcy = origin_y;
	int xx = destination_x;
	int yy = destination_y;

	PathNodeArray* arr = new PathNodeArray();

	if (xx < 0 || xx >= nav.mapWidth || yy < 0 || yy >= nav.mapHeight || !nav.Walkable(xx, yy) || !nav.Walkable(srcx, srcy)) {

	}  else 	if (can_see_from(srcx, srcy, xx, yy))
	{
		arr->push(new PathNode(srcx, srcy));
		arr->push(new PathNode(xx, yy));
	}
	else {
		arr->insert(0,find_route_jps(srcx, srcy, xx, yy));
	}

	engine->RegisterManagedObject(arr, &PathNodeArray_Interface);

	return (arr);
}

#if AGS_PLATFORM_OS_WINDOWS

	//==============================================================================

	// ***** Design time *****

	IAGSEditor* editor; // Editor interface

	const char* ourScriptHeader =
		"  \r\n"
		"managed struct PathNode{ \r\n"
		"  \r\n"
		"  /// Creates a PathNode from X and Y points \r\n"
		"  import static PathNode* Create(int X, int Y);\r\n"
		"  import attribute int X;\r\n"
		"  import attribute int Y;\r\n"
		"}; \r\n"
		"managed struct PathNodeArray {\r\n"
		"  readonly int Capacity;\r\n"
		"  /// Creates a new array.\r\n"
		"  import static PathNodeArray *Create (int count = 0, PathNode* pathNode = 0); // $AUTOCOMPLETESTATICONLY$\r\n"
		"  /// Copies an existing array.\r\n"
		"  import static PathNodeArray *Copy (PathNodeArray *source); // $AUTOCOMPLETESTATICONLY$\r\n"
		"  /// Swaps the values of two arrays.\r\n"
		"  import static void Swap (PathNodeArray *a, PathNodeArray *b); // $AUTOCOMPLETESTATICONLY$\r\n"
		"  /// Clears all values in the array.\r\n"
		"  import void Clear ();\r\n"
		"  /// Returns true when the array is empty.\r\n"
		"  import bool Empty ();\r\n"
		"  /// Removes specified value(s) from the array.\r\n"
		"  import void Erase (int pos, int number = 1);\r\n"
		"  /// Inserts a value at a specified place in the array.\r\n"
		"  import void Insert (int pos, PathNode* pathNode);\r\n"
		"  /// Inserts all values of the given array a the specified place.\r\n"
		"  import void InsertPathNodeArray (int pos, PathNodeArray *source);\r\n"
		"  import attribute PathNode* Items[];\r\n"
		"  /// Removes the last item of the array and returns it.\r\n"
		"  import PathNode* Pop ();\r\n"
		"  /// Adds the specified value to the end of the array.\r\n"
		"  import void Push (PathNode* pathNode);\r\n"
		"  import attribute int Size;\r\n"
		"  /// Increases the capacity of the array. (Note: you don't need to do this manually)\r\n"
		"  import void Reserve (int number);\r\n"
		"};\r\n"
		"  \r\n"
		"struct AgsPathfinder { \r\n"
		"  \r\n"
		"  /// Pass a sprite identifier and the bottom threshold for walls color\r\n"
		"  import static void SetGridFromSprite(int sprite);\r\n"
		"  \r\n"
		"  /// Get nodes for set origin and destination \r\n"
		"  import static PathNodeArray* GetPathFromTo(int origin_x, int origin_y, int destination_x, int destination_y);\r\n"
		"}; \r\n";


	//------------------------------------------------------------------------------

	LPCSTR AGS_GetPluginName()
	{
		return ("agspathfinder");
	}

	//------------------------------------------------------------------------------

	int AGS_EditorStartup(IAGSEditor* lpEditor)
	{
		// User has checked the plugin to use it in their game

		// If it's an earlier version than what we need, abort.
		if (lpEditor->version < MIN_EDITOR_VERSION)
			return (-1);

		editor = lpEditor;
		editor->RegisterScriptHeader(ourScriptHeader);

		// Return 0 to indicate success
		return (0);
	}

	//------------------------------------------------------------------------------

	void AGS_EditorShutdown()
	{
		// User has un-checked the plugin from their game
		editor->UnregisterScriptHeader(ourScriptHeader);
	}

	//------------------------------------------------------------------------------

	void AGS_EditorProperties(HWND parent)                        //*** optional ***
	{
		// User has chosen to view the Properties of the plugin
		// We could load up an options dialog or something here instead
	/*	MessageBox(parent,
				 L"agsfastwfc v1.0 By Calin Leafshade",
				 L"About",
			 MB_OK | MB_ICONINFORMATION);
	 */
	}

	//------------------------------------------------------------------------------

	int AGS_EditorSaveGame(char* buffer, int bufsize)             //*** optional ***
	{
		// Called by the editor when the current game is saved to disk.
		// Plugin configuration can be stored in [buffer] (max [bufsize] bytes)
		// Return the amount of bytes written in the buffer
		return (0);
	}

	//------------------------------------------------------------------------------

	void AGS_EditorLoadGame(char* buffer, int bufsize)            //*** optional ***
	{
		// Called by the editor when a game is loaded from disk
		// Previous written data can be read from [buffer] (size [bufsize]).
		// Make a copy of the data, the buffer is freed after this function call.
	}

	//==============================================================================

#endif

// ***** Run time *****

// Engine interface

//------------------------------------------------------------------------------



#define REGISTER(x) engine->RegisterScriptFunction(#x, (void *) (x));
#define REG_CLASS(c,x,a) engine->RegisterScriptFunction(#c "::" #x "^" #a, (void *) (c ## _ ## x));
#define REG_ATTR(c,x) engine->RegisterScriptFunction(#c "::get_" #x, (void *) (c ## _Get ## x)); \
                      engine->RegisterScriptFunction(#c "::set_" #x, (void *) (c ## _Set ## x));
#define REG_ARR(c,x) engine->RegisterScriptFunction(#c "::geti_" #x, (void *) (c ## _Get ## x)); \
                     engine->RegisterScriptFunction(#c "::seti_" #x, (void *) (c ## _Set ## x));

#define STRINGIFY(s) STRINGIFY_X(s)
#define STRINGIFY_X(s) #s

	void AGS_EngineStartup(IAGSEngine* lpEngine)
	{
		engine = lpEngine;

		// Make sure it's got the version with the features we need
		if (engine->version < MIN_ENGINE_VERSION)
			engine->AbortGame("Plugin needs engine version " STRINGIFY(MIN_ENGINE_VERSION) " or newer.");

		//register functions
		engine->AddManagedObjectReader(ManagedPathNodeInterface::typeName, &gManagedPathNodeReader);

		engine->RegisterScriptFunction("PathNode::Create^2", (void *) CreatePathNode);
		engine->RegisterScriptFunction("PathNode::set_X", (void *) PathNode_set_X);
		engine->RegisterScriptFunction("PathNode::get_X", (void *) PathNode_get_X);
		engine->RegisterScriptFunction("PathNode::set_Y", (void *) PathNode_set_Y);
		engine->RegisterScriptFunction("PathNode::get_Y", (void *) PathNode_get_Y);

		engine->AddManagedObjectReader(PathNodeArrayInterface::name, &PathNodeArray_Reader);

		REG_CLASS(PathNodeArray, Create, 2)
		REG_CLASS(PathNodeArray, Copy, 1)
		REG_CLASS(PathNodeArray, Swap, 2)
		REG_CLASS(PathNodeArray, Clear, 0)
		REG_CLASS(PathNodeArray, Empty, 0)
		REG_CLASS(PathNodeArray, Erase, 2)
		REG_CLASS(PathNodeArray, Insert, 2)
		REG_CLASS(PathNodeArray, InsertPathNodeArray, 2)
		REG_ARR(PathNodeArray, Items)
		REG_CLASS(PathNodeArray, Pop, 0)
		REG_CLASS(PathNodeArray, Push, 1)
		REG_ATTR(PathNodeArray, Size)
		REG_CLASS(PathNodeArray, Reserve, 1)

		engine->RegisterScriptFunction("AgsPathfinder::SetGridFromSprite^2", (void *) AgsPathfinder_SetGridFromSprite);
		engine->RegisterScriptFunction("AgsPathfinder::GetPathFromTo^5", (void *) AgsPathfinder_GetPathFromTo);

	}

	//------------------------------------------------------------------------------

	void AGS_EngineShutdown()
	{
		// Called by the game engine just before it exits.
		// This gives you a chance to free any memory and do any cleanup
		// that you need to do before the engine shuts down.
	}

	//------------------------------------------------------------------------------

	int AGS_EngineOnEvent(int event, int data)                    //*** optional ***
	{
		switch (event)
		{
			/*
				case AGSE_KEYPRESS:
				case AGSE_MOUSECLICK:
				case AGSE_POSTSCREENDRAW:
				case AGSE_PRESCREENDRAW:
				case AGSE_SAVEGAME:
				case AGSE_RESTOREGAME:
				case AGSE_PREGUIDRAW:
				case AGSE_LEAVEROOM:
				case AGSE_ENTERROOM:
				case AGSE_TRANSITIONIN:
				case AGSE_TRANSITIONOUT:
				case AGSE_FINALSCREENDRAW:
				case AGSE_TRANSLATETEXT:
				case AGSE_SCRIPTDEBUG:
				case AGSE_SPRITELOAD:
				case AGSE_PRERENDER:
				case AGSE_PRESAVEGAME:
				case AGSE_POSTRESTOREGAME:
			 */
		default:
			break;
		}

		// Return 1 to stop event from processing further (when needed)
		return (0);
	}

	//------------------------------------------------------------------------------

	int AGS_EngineDebugHook(const char* scriptName,
		int lineNum, int reserved)            //*** optional ***
	{
		// Can be used to debug scripts, see documentation
		return 0;
	}

	//------------------------------------------------------------------------------

	void AGS_EngineInitGfx(const char* driverID, void* data)      //*** optional ***
	{
		// This allows you to make changes to how the graphics driver starts up.
		// See documentation
	}

	//..............................................................................


#if defined(BUILTIN_PLUGINS)
}
#endif
