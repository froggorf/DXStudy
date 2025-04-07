// 04.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"


class UAnimNotify : public UObject
{
	MY_GENERATED_BODY(UAnimNotify)
public:
	UAnimNotify() = default;
	~UAnimNotify() override = default;


	virtual void Notify();
protected:
private:
public:
protected:
private:
};

class UAnimNotify_PlaySound : public UAnimNotify
{
	MY_GENERATED_BODY(UAnimNotify_PlaySound)
public:
	UAnimNotify_PlaySound() = default;
	~UAnimNotify_PlaySound() override = default;

	void Notify() override;
};

class UAnimNotify_PlaySound_WalkLeft : public UAnimNotify_PlaySound
{
	MY_GENERATED_BODY(UAnimNotify_PlaySound_WalkLeft)
public:
	UAnimNotify_PlaySound_WalkLeft() = default;
	~UAnimNotify_PlaySound_WalkLeft() override = default;

	void Notify() override;
};

class UAnimNotify_PlaySound_WalkRight : public UAnimNotify_PlaySound
{
	MY_GENERATED_BODY(UAnimNotify_PlaySound_WalkRight)
public:
	UAnimNotify_PlaySound_WalkRight() = default;
	~UAnimNotify_PlaySound_WalkRight() override = default;

	void Notify() override;
};


class UAnimNotify_PlaySound_RunLeft : public UAnimNotify_PlaySound
{
	MY_GENERATED_BODY(UAnimNotify_PlaySound_RunLeft)
public:
	UAnimNotify_PlaySound_RunLeft() = default;
	~UAnimNotify_PlaySound_RunLeft() override = default;

	void Notify() override;
};


class UAnimNotify_PlaySound_RunRight : public UAnimNotify_PlaySound
{
	MY_GENERATED_BODY(UAnimNotify_PlaySound_RunRight)
public:
	UAnimNotify_PlaySound_RunRight() = default;
	~UAnimNotify_PlaySound_RunRight() override = default;

	void Notify() override;
};
