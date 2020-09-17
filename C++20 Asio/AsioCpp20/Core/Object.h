#pragma once
class Object
{
public:
	Object();
	virtual ~Object() = default;

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;

	bool DoPreInit();
	bool DoInit();
	bool DoPostInit();

	virtual bool PreInit() { return true; }
	virtual bool Init() { return true; }
	virtual bool PostInit() { return true; }

private:
	bool isPreInited{ false };
	bool isInited{ false };
	bool isPostInited{ false };
};

