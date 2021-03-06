#pragma once
#include <map>

enum Actions {
	MoveForward,
	MoveBackWard,
	MoveLeft,
	MoveRight,
	BeatIt,
	VoidShell,
	Shoot,
	Dash,
	DebugMode,
};

class InputManager
{
protected:
	InputManager() {}
	~InputManager() {}
	/*implicit methods exist for the copy constructor and operator= and we want to forbid calling them.*/
	InputManager(const InputManager &) {}
	InputManager& operator =(const InputManager &) {}

public:
	static void CreateInstance()
	{
		if (ms_pInstance == NULL)
			ms_pInstance = new InputManager;
	}
	static InputManager * GetInstance()
	{
		return ms_pInstance;
	}
	static void DestroyInstance()
	{
		if (ms_pInstance)
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}

	void Init();
	void LoadDefault();
	void keyUpdate(unsigned char key, bool bIsPressed);
	bool ActionCheck(Actions action);
	void MouseDown(float x, float y);
	void MouseUp(float x, float y);
	void RightMouseDown(float x, float y);
	void RightMouseUp(float x, float y);
	void MousePosition(float x, float y);
	bool isMouseDown = false;
	bool isRightMouseDown = FALSE;
	float mX, mY;
	float mouseX, mouseY;

protected:
	static InputManager * ms_pInstance;

public:
	int keyMap = 0;
	map<unsigned char, Actions> keyBind;
};