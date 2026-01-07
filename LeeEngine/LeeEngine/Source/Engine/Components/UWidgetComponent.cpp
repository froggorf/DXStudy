#include "CoreMinimal.h"
#include "UWidgetComponent.h"

#include "Engine/Widget/ChildWidget.h"
#include "Engine/Widget/UUserWidget.h"
#include "Engine/World/UWorld.h"

void UWidgetComponent::Register()
{
	USceneComponent::Register();

	MainCanvas = std::make_shared<FCanvasWidget>();
	
}

void UWidgetComponent::SetWidget(const std::shared_ptr<UUserWidget>& WidgetInstance)
{
	Widget = WidgetInstance;
	SetDrawSize(Widget->GetMainCanvasWidget()->GetDesignResolution());
	Widget->AttachToPanel(MainCanvas);
}

void UWidgetComponent::TickComponent(float DeltaSeconds)
{
    USceneComponent::TickComponent(DeltaSeconds);

    if (!MainCanvas || !Widget)
    {
        return;
    }

    const XMFLOAT2& CurrentResolution = GDirectXDevice->GetResolution();
    MainCanvas->SetDesignResolution(CurrentResolution);
    SetDrawSize(Widget->GetMainCanvasWidget()->GetDesignResolution());

    const FTransform& Transform = GetComponentTransform();
    const FViewMatrices ViewMatrices = GetWorld()->GetCameraManager()->GetViewMatrices();
    const XMMATRIX ViewProjMatrix = ViewMatrices.GetViewMatrix() * ViewMatrices.GetProjectionMatrix();

    const XMVECTOR WorldPos = XMVectorSet(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z, 1.0f);
    const XMVECTOR ClipPos = XMVector4Transform(WorldPos, ViewProjMatrix);

    const float ClipW = XMVectorGetW(ClipPos);

    if (ClipW <= 0.0f)
    {
        return;
    }

    const XMVECTOR NDCPosVec = ClipPos / ClipW;
    const float NDC_X = XMVectorGetX(NDCPosVec);
    const float NDC_Y = XMVectorGetY(NDCPosVec);
    const float NDC_Z = XMVectorGetZ(NDCPosVec);

    if (NDC_Z < 0.0f || NDC_Z > 1.0f)
    {
        return;
    }

    const float ScreenX = (NDC_X + 1.0f) * 0.5f * CurrentResolution.x;
    const float ScreenY = (1.0f - (NDC_Y + 1.0f) * 0.5f) * CurrentResolution.y;
    const XMFLOAT2 NewPosition = { ScreenX, ScreenY };

    const float Left = NewPosition.x - DrawSize.x / 2;
    const float Right = NewPosition.x + DrawSize.x / 2;
    const float Up = NewPosition.y - DrawSize.y / 2;
    const float Down = NewPosition.y + DrawSize.y / 2;

    if ((Right < 0 || Left > CurrentResolution.x) ||
        (Down < 0 || Up > CurrentResolution.y))
    {
        return;
    }

    if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::static_pointer_cast<FCanvasSlot>(Widget->GetMainCanvasWidget()->GetSlot()))
    {
        CanvasSlot->Anchors = ECanvasAnchor::LeftTop;
        CanvasSlot->Position = NewPosition;
        CanvasSlot->Size = DrawSize;
        CanvasSlot->Alignment = {0.5f, 0.5f};
    }

    MainCanvas->Tick(DeltaSeconds);
}

