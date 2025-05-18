// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UPrimitiveComponent.h"

#include "Engine/RenderCore/RenderingThread.h"

UINT PrimitiveIDCount = 0;

UPrimitiveComponent::UPrimitiveComponent()
{
	PrimitiveID  = PrimitiveIDCount++;
	bIsPrimitive = true;
}

UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::Register()
{
	USceneComponent::Register();

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> PrimitiveSceneProxies = CreateSceneProxy();
	for (int i = 0; i < PrimitiveSceneProxies.size(); ++i)
	{
		FScene::AddPrimitive_GameThread(PrimitiveID, PrimitiveSceneProxies[i], GetComponentTransform());
	}
}

void UPrimitiveComponent::SetScalarParam(UINT MeshIndex, const std::string& ParamName, float Value) const
{
	FScene::SetMaterialScalarParam_GameThread(PrimitiveID, MeshIndex, ParamName, Value);
}

void UPrimitiveComponent::SetTextureParam(UINT                           MeshIndex, UINT TextureSlot,
										const std::shared_ptr<UTexture>& Texture) const
{
	FScene::SetTextureParam_GameThread(PrimitiveID, MeshIndex, TextureSlot, Texture);
}

void UPrimitiveComponent::DrawDetailPanel(UINT ComponentDepth)
{
	{
		if (ComponentDepth == 0)
		{
			static float NewSpeedX = 0.0f, NewSpeedY = 0.0f;
			ImGui::Text("SetScalarParam");
			if (ImGui::SliderFloat("NewSpeedX", &NewSpeedX, -1.0f, 1.0f))
			{
				SetScalarParam(0, "WaterSpeedX", NewSpeedX);
			}
			if (ImGui::SliderFloat("NewSpeedY", &NewSpeedY, -1.0f, 1.0f))
			{
				SetScalarParam(0, "WaterSpeedY", NewSpeedY);
			}

			ImGui::Text("SetTextureParam");
			if (ImGui::Button("Water"))
			{
				SetTextureParam(0, 0, UTexture::GetTextureCache("T_TranslucentCube"));
			}
			if (ImGui::Button("Grass"))
			{
				SetTextureParam(0, 0, UTexture::GetTextureCache("T_Cube2"));
			}

			if (ImGui::Button("TestTexture"))
			{
				SetTextureParam(0, 0, UTexture::GetTextureCache("TestTexture"));
			}

			static float colors[3] = {0.0f, 1.0f, 1.0f};
			ImGui::SliderFloat3("CS_SetColor", colors, 0.0f, 1.0f);
			if (ImGui::Button("ComputeShader_SetColor"))
			{
				std::shared_ptr<FSetColorCS> SetColorCS = std::reinterpret_pointer_cast<FSetColorCS>(
					FShader::GetShader("FSetColorCS"));
				std::shared_ptr<UTexture> Texture = UTexture::GetTextureCache("TestTexture");
				SetColorCS->SetTargetTexture(Texture);
				SetColorCS->SetClearColor(XMFLOAT4{colors[0], colors[1], colors[2], 1.0f});
				SetColorCS->Execute_Enqueue();
			}
		}
	}
}
