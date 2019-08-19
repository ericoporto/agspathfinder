// dllmain.cpp : Defines the entry point for the DLL application.
#pragma region Defines_and_Includes

#include "core/platform.h"

#define MIN_EDITOR_VERSION 1
#define MIN_ENGINE_VERSION 3

#if AGS_PLATFORM_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#if !defined(BUILTIN_PLUGINS)
#define THIS_IS_THE_PLUGIN
#endif

#include "plugin/agsplugin.h"
#include "jps.h"

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

void PathNode_set_Size(PathNode* obj, int val) {
	obj->Size = val;
}

int PathNode_get_Size(PathNode* obj) {
	return obj->Size;
}

PathNode* CreatePathNode(int X, int Y) {
	PathNode* newPathNode = new PathNode(X, Y);
	engine->RegisterManagedObject(newPathNode, &gManagedPathNodeInterface);
	return newPathNode;
}

// -- do pathfinding

void AgsPathfinder_SetGridFromSprite(int sprite, int wall_color_threshold) {

}

PathNode** AgsPathfinder_GetPathFromTo(int origin_x, int origin_y, int destination_x, int destination_y) {
	PathNode** pathNodes = new PathNode* [3];
	pathNodes[0] = CreatePathNode(1, 2);
	pathNodes[0]->Size = 3;
	pathNodes[1] = CreatePathNode(3, 2);
	pathNodes[1]->Size = 2;
	pathNodes[2] = CreatePathNode(5, 6);
	pathNodes[2]->Size = 1;
	return pathNodes;
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
		"  import attribute int Size;\r\n"
		"}; \r\n"
		"  \r\n"
		"struct AgsPathfinder { \r\n"
		"  \r\n"
		"  /// Pass a sprite identifier and the bottom threshold for walls color\r\n"
		"  import static void SetGridFromSprite(int sprite, int wall_color_threshold = 4);\r\n"
		"  \r\n"
		"  /// Get nodes for set origin and destination \r\n"
		"  import static PathNode*[] GetPathFromTo(int origin_x, int origin_y, int destination_x, int destination_y);\r\n"
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

		engine->RegisterScriptFunction("PathNode::Create^2", CreatePathNode);
		engine->RegisterScriptFunction("PathNode::set_X", PathNode_set_X);
		engine->RegisterScriptFunction("PathNode::get_X", PathNode_get_X);
		engine->RegisterScriptFunction("PathNode::set_Y", PathNode_set_Y);
		engine->RegisterScriptFunction("PathNode::get_Y", PathNode_get_Y);
		engine->RegisterScriptFunction("PathNode::set_Size", PathNode_set_Size);
		engine->RegisterScriptFunction("PathNode::get_Size", PathNode_get_Size);

		engine->RegisterScriptFunction("AgsPathfinder::SetGridFromSprite^2", AgsPathfinder_SetGridFromSprite);
		engine->RegisterScriptFunction("AgsPathfinder::GetPathFromTo^4", AgsPathfinder_GetPathFromTo);

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
