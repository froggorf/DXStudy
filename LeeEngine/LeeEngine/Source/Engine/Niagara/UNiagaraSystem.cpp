#include "CoreMinimal.h"
#include "UNiagaraSystem.h"


std::shared_ptr<FTickParticleCS> FNiagaraRendererProperty::TickParticleCS;

void FNiagaraRendererProperty::ExecuteCS()
{
	if(TickParticleCS == nullptr)
	{
		TickParticleCS = std::reinterpret_pointer_cast<FTickParticleCS>(FShader::GetShader("FTickParticleCS"));
	}

	TickParticleCS->SetParticleBuffer(ParticleBuffer);
	TickParticleCS->Execute();
	
	
}

void FNiagaraRendererProperty::Render()
{
	ParticleBuffer->Binding(ParticleRegister);
}

std::shared_ptr<UStaticMesh> FNiagaraRendererSprites::Mesh;
void FNiagaraRendererSprites::Render()
{
	FNiagaraRendererProperty::Render();
	
	
	
	auto RenderData = Mesh->GetStaticMeshRenderData();
	UINT MeshCount = RenderData->MeshCount;
	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	for (int MeshIndex = 0; MeshIndex < MeshCount; ++MeshIndex)
	{
		// 셰이더 설정
		UINT stride = sizeof(MyVertexData);
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
		DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC indexBufferDesc;
		RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
		UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
		//DeviceContext->DrawIndexedInstanced(indexSize,MaxParticleCount, 0, 0, 0);
		DeviceContext->DrawIndexed(indexSize, 0, 0);

	}
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	

}

// =======================================

void UNiagaraSystem::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);


	if(AssetData.contains("Emitters"))
	{
		auto EmitterData = AssetData["Emitters"];
		for (const auto& Emitter : EmitterData)
		{
			std::shared_ptr<FNiagaraEmitter> NewEmitter = std::make_shared<FNiagaraEmitter>();
			int RenderType = Emitter["RenderType"];
			switch(RenderType)
			{
			case 0:
				// 스프라이트 렌더러
				NewEmitter->RenderProperty = std::make_shared<FNiagaraRendererSprites>();
				break;
			case 1:
				// 메쉬 렌더러
				break;
			case 2:
				// 리본 렌더러
				break;
			case 3:
				// 빔 렌더러
				break;
			default:
				assert(0 && "잘못된 RenderType");
			}


			std::string_view MaterialName = Emitter["Material"];
			NewEmitter->RenderProperty->SetMaterial(UMaterial::GetMaterialCache(MaterialName.data()));
			Emitters.emplace_back(NewEmitter);
		}
	}
	GetNiagaraSystemCache()[GetName()] = shared_from_this();
}