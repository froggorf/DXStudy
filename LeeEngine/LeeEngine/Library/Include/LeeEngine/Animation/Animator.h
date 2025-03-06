// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#pragma once
#include <DirectXMath.h>
#include <vector>


struct AssimpNodeData;
class Animation;

class Animator
{
public:
	Animator(Animation* animation);
	void UpdateAnimation(float dt);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform);

	std::vector<DirectX::XMMATRIX>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

protected:
private:
public:
protected:
private:
	std::vector<DirectX::XMMATRIX> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};
