// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimSequence.h"
#include "Engine/UObject/UObject.h"

class UAnimInstance : public UObject
{
	MY_GENERATED_BODY(UAnimInstance)
public:
	UAnimInstance();
	UAnimInstance(const std::shared_ptr<UAnimSequence>& InAnimation);
	~UAnimInstance() override {};
	void UpdateAnimation(float dt, float OwnerPrimitiveID);
	void PlayAnimation(const std::shared_ptr<UAnimSequence>& InAnimation);
	void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform);

	void SetAnimation(const std::shared_ptr<UAnimSequence>& InAnimation, float InBlendTime = 0.0f);

	std::vector<DirectX::XMMATRIX>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

protected:
private:
public:
protected:
private:
	std::vector<DirectX::XMMATRIX> m_FinalBoneMatrices;
	std::shared_ptr<UAnimSequence> m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};
