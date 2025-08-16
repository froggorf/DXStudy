#include "CoreMinimal.h"
#include "ChildWidget.h"

void IPanelContainer::AddChild(const std::shared_ptr<FChildWidget>& NewChild)
{
	AttachChildren.emplace_back(NewChild);
}

UINT WidgetCount = 0;
FChildWidget::FChildWidget()
{
	WidgetID = WidgetCount++;
}

void FImageWidget::Draw()
{
	// LeftTopRightBottom 받아서
	float Left = GetSlot()->GetLeft();
	float Right = GetSlot()->GetRight();
	float Top = GetSlot()->GetTop();
	float Bottom = GetSlot()->GetBottom();


	// 해당위치에 Imgui로 그리기
	if (Brush.Image)
	{
		ImVec2 Size = {Right - Left, Bottom - Top};
		std::string ID = "##Image" + std::to_string(WidgetID);
		ImGui::SetNextWindowPos(ImVec2{Left,Top});
		ImGui::Begin(ID.c_str(), nullptr);
		ImGui::Image(reinterpret_cast<ImTextureID>(Brush.Image->GetSRV().Get()), Size);
		ImGui::End();
	}
	
}
