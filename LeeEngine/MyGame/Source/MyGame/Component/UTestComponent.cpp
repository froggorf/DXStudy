
#include "UTestComponent.h"
#include <Engine/RenderCore/EditorScene.h>

#include "Engine/Mesh/USkeletalMesh.h"

bool operator==(const XMFLOAT2& A, const XMFLOAT2& B)
{
	return A.x == B.x&& A.y == B.y;
}

UTestComponent::UTestComponent()
{
	std::shared_ptr<USkeletalMesh> PaladinSkeleton = USkeletalMesh::GetSkeletalMesh("SK_Paladin");
	if(!PaladinSkeleton)
	{
		return;
	}
	std::shared_ptr<UAnimSequence> IdleAnim = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_Idle.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);

	std::shared_ptr<UAnimSequence> WalkForward= std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_WalkForward.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> WalkRight = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_WalkRight.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> WalkBack = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_WalkBack.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> WalkLeft = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_WalkLeft.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);

	std::shared_ptr<UAnimSequence> RunForward= std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_RunForward.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> RunRight = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_RunRight.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> RunBack = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_RunBack.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);
	std::shared_ptr<UAnimSequence> RunLeft = std::make_shared<UAnimSequence>(GEngine->GetDirectoryPath() + "/Content/Resource/Animation/Paladin/Paladin_RunLeft.fbx", PaladinSkeleton->GetSkeletalMeshRenderData()->ModelBoneInfoMap);

	HorizontalValue = {-180.0f - Gap.x,180.0f + Gap.x};
	VerticalValue = {0.0f-Gap.y, 600.0f+Gap.y};

	// IDLE
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{0.0f,0.0f}, IdleAnim));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-90.0f,0.0f}, IdleAnim));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-180.0f,0.0f}, IdleAnim));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{90.0f,0.0f}, IdleAnim));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{180.0f,0.0f}, IdleAnim));

	// WALK
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{0.0f,150.0f}, WalkForward));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-90.0f,150.0f}, WalkLeft));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{90.0f,150.0f}, WalkRight));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-180.0f,150.0f}, WalkBack));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{180.0f,150.0f}, WalkBack));

	// Run
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{0.0f,600.0f}, RunForward));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-90.0f,600.0f}, RunLeft));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{90.0f,600.0f}, RunRight));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{-180.0f,600.0f}, RunBack));
	TestValue.emplace_back(std::make_shared<FAnimClipPoint>(XMFLOAT2{180.0f,600.0f}, RunBack));

	for(int i =0; i < TestValue.size()-1; ++i)
	{
		for(int j = i+1; j < TestValue.size(); ++j)
		{
			// 1. n(n-1) 돌면서 간선을 만든다.
			std::shared_ptr<FAnimClipEdge> NewEdge = std::make_shared<FAnimClipEdge>(TestValue[i],TestValue[j]);


			std::function<bool(const XMFLOAT2& CheckPoint, const XMFLOAT2& FirstPoint, const XMFLOAT2& SecondPoint)> IsPointOnSegment = [](const XMFLOAT2& A, const XMFLOAT2& B, const XMFLOAT2& C)->bool
				{
					float crossProduct = (C.x - B.x) * (A.y - B.y) - (C.y - B.y) * (A.x - B.x);
					if (std::abs(crossProduct) > FLT_EPSILON) { // 0이 아닌 경우 (부동소수점 비교)
						return false;
					}

					// 2. 점 A가 선분 BC의 범위 내에 있는지 확인
					if ((A.x <= min(B.x, C.x) || A.x >= max(B.x, C.x)) && (A.y <= min(B.y, C.y) || A.y >= max(B.y, C.y))) return false;


					return true;
				};

			// 2. 만약 그 간선 사이에 점이 있다면 그 간선은 못만든다(n)
			bool bIsOnLine = false;
			for(int k =0; k < TestValue.size(); ++k)
			{
				if(IsPointOnSegment(TestValue[k]->Position, TestValue[i]->Position,TestValue[j]->Position))
				{
					bIsOnLine = true;
					break;
				}	
			}
			if(bIsOnLine)
			{
				continue;
			}

			NewEdge->LengthSq = XMVectorGetX( XMVector2LengthSq(XMVectorSubtract(XMLoadFloat2(&NewEdge->StartPoint->Position), XMLoadFloat2(&NewEdge->EndPoint->Position))));

			// 3. 만약 그 간선이 다른 간선들과 교차할 경우, 길이가 긴 간선이 이긴다 (nlogn)
			std::function<float(const XMFLOAT2& P, const XMFLOAT2& Q, const XMFLOAT2& R)> CrossProduct = [](const XMFLOAT2& P, const XMFLOAT2& Q, const XMFLOAT2& R)->float
				{
					return (Q.x - P.x) * (R.y - P.y) - (Q.y - P.y) * (R.x - P.x);
				};
			std::function<bool(const XMFLOAT2& AStart, const XMFLOAT2& AEnd, const XMFLOAT2& BStart, const XMFLOAT2& BEnd)> IsSegmentsIntersect = [CrossProduct](const XMFLOAT2& AStart, const XMFLOAT2& AEnd, const XMFLOAT2& BStart, const XMFLOAT2& BEnd)->bool
				{
					float cross1 = CrossProduct(BStart,BEnd,AStart);
					float cross2 = CrossProduct(BStart,BEnd,AEnd);
					float cross3 = CrossProduct(AStart,AEnd,BStart);
					float cross4 = CrossProduct(AStart,AEnd,BEnd);

					bool bIsIntersecting = (cross1*cross2 <= 0) && (cross3*cross4 <= 0);
					if(bIsIntersecting)
					{
						// 직선이 겹칠 경우 선분도 겹치는지 확인
						bool bIsXNotInterSect = (max(AStart.x, AEnd.x) <= min(BStart.x, BEnd.x) || max(BStart.x, BEnd.x) <= min(AStart.x, AEnd.x));
						bool bIsYNotInterSect = (max(AStart.y, AEnd.y) <= min(BStart.y, BEnd.y) || max(BStart.y, BEnd.y) <= min(AStart.y, AEnd.y));
						if ( bIsXNotInterSect || bIsYNotInterSect) 
						{
							return false;
						}

					}
					return bIsIntersecting;
				};

			// 선분이 겹칠 경우, 길이가 짧은 것을 넣어주므로
			// 새로 넣는 간선이 길이가 짧을 경우 추가를 위해 index가 -1이 아니지만
			// 길이가 더 길경우 -1로 나오게 되면서 추가되지 않고 스킵됨.
			bool bIsIntersecting = false;
			int RemoveEdgeIndexForAddNewEdge = -1;
			for(int EdgeIndex = 0; EdgeIndex < CurrentEdge.size(); ++EdgeIndex)
			{
				if(IsSegmentsIntersect(NewEdge->StartPoint->Position,NewEdge->EndPoint->Position, CurrentEdge[EdgeIndex]->StartPoint->Position,CurrentEdge[EdgeIndex]->EndPoint->Position))
				{
					bIsIntersecting = true;
					if(NewEdge->LengthSq < CurrentEdge[EdgeIndex]->LengthSq)
					{
						RemoveEdgeIndexForAddNewEdge = EdgeIndex;
						break;
					}
				}
			}
			if(!bIsIntersecting)
			{
				CurrentEdge.push_back(NewEdge);
			}
			else
			{
				if(RemoveEdgeIndexForAddNewEdge != -1 )
				{
					CurrentEdge.erase(CurrentEdge.begin()+RemoveEdgeIndexForAddNewEdge);
					CurrentEdge.push_back(NewEdge);
				}	
			}



		}
	}


	std::vector<std::shared_ptr<FAnimClipEdge>> ToBeTriangleEdge{CurrentEdge.begin(),CurrentEdge.end()};
	for(int TargetEdgeIndex = 0; TargetEdgeIndex < ToBeTriangleEdge.size(); ++TargetEdgeIndex)
	{

		for(int NextEdgeIndex = TargetEdgeIndex +1; NextEdgeIndex < ToBeTriangleEdge.size(); ++NextEdgeIndex)
		{
			// 시작점 or 끝점이 같은 것이 있는지 확인
			bool bIsSameStart = ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position == ToBeTriangleEdge[NextEdgeIndex]->StartPoint->Position;
			bool bIsSameEnd = ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position == ToBeTriangleEdge[NextEdgeIndex]->EndPoint->Position;
			bool bIsSameAStartAndBEnd = (ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position == ToBeTriangleEdge[NextEdgeIndex]->EndPoint->Position);
			bool bIsSameAEndAndBStart = (ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position== ToBeTriangleEdge[NextEdgeIndex]->StartPoint->Position);

			// 같은 것이 있다면 (이어지는 엣지가 있다면)
			if(bIsSameStart || bIsSameEnd || bIsSameAStartAndBEnd || bIsSameAEndAndBStart)
			{
				// 찾으려는 간선의 위치
				XMFLOAT2 FindEdgeFirstPos, FindEdgeSecondPos;
				if(bIsSameStart)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex]->EndPoint->Position;
				}
				if(bIsSameEnd)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex]->StartPoint->Position;
				}
				if(bIsSameAStartAndBEnd)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex]->StartPoint->Position;	
				}
				if(bIsSameAEndAndBStart)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex]->EndPoint->Position;	
				}

				for(int FoundEdgeIndex = NextEdgeIndex + 1 ; FoundEdgeIndex < ToBeTriangleEdge.size(); ++FoundEdgeIndex)
				{
					// 해당하는 간선을 찾음
					if(ToBeTriangleEdge[FoundEdgeIndex]->StartPoint->Position == FindEdgeFirstPos && ToBeTriangleEdge[FoundEdgeIndex]->EndPoint->Position == FindEdgeSecondPos
						|| ToBeTriangleEdge[FoundEdgeIndex]->EndPoint->Position == FindEdgeFirstPos && ToBeTriangleEdge[FoundEdgeIndex]->StartPoint->Position == FindEdgeSecondPos)
					{
						std::shared_ptr<FAnimClipTriangle> NewTriangle = std::make_shared<FAnimClipTriangle>();
						
						NewTriangle->Edges.push_back(ToBeTriangleEdge[TargetEdgeIndex]);
						NewTriangle->Edges.push_back(ToBeTriangleEdge[NextEdgeIndex]);
						NewTriangle->Edges.push_back(ToBeTriangleEdge[FoundEdgeIndex]);

						NewTriangle->Points.push_back(ToBeTriangleEdge[FoundEdgeIndex]->StartPoint);
						NewTriangle->Points.push_back(ToBeTriangleEdge[FoundEdgeIndex]->EndPoint);

						std::shared_ptr<FAnimClipPoint> OtherPoint;
						if(ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position == ToBeTriangleEdge[FoundEdgeIndex]->StartPoint->Position
							|| ToBeTriangleEdge[TargetEdgeIndex]->StartPoint->Position == ToBeTriangleEdge[FoundEdgeIndex]->EndPoint->Position)
						{
							OtherPoint = ToBeTriangleEdge[TargetEdgeIndex]->EndPoint;
						}
						else if(ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position == ToBeTriangleEdge[FoundEdgeIndex]->StartPoint->Position
							|| ToBeTriangleEdge[TargetEdgeIndex]->EndPoint->Position == ToBeTriangleEdge[FoundEdgeIndex]->EndPoint->Position)
						{
							OtherPoint = ToBeTriangleEdge[TargetEdgeIndex]->StartPoint;
						}
						NewTriangle->Points.push_back(OtherPoint);

						CurrentTriangles.push_back(NewTriangle);
					}


				}

			}
		}
		// 현재의 탐색하는 간선에 대해서 탐색이 끝나면 해당 간선은 삭제해준다.
		ToBeTriangleEdge.erase(ToBeTriangleEdge.begin());
		--TargetEdgeIndex;
	}
}

enum class EPointAndTriangleState
{
	PATS_Inside,
	PATS_InEdge,
	PATS_Outside,
};

float CrossProduct2D(const XMVECTOR& P, const XMVECTOR& Q, const XMVECTOR& R) {
	XMVECTOR PQ = XMVectorSubtract(Q, P);
	XMVECTOR PR = XMVectorSubtract(R, P);

	XMVECTOR cross = XMVector3Cross(PQ, PR);

	// z 좌표 반환 (2D 외적의 크기)
	return XMVectorGetZ(cross);
}

EPointAndTriangleState FindPointAndTriangleState(const XMFLOAT2& TargetPoint, const XMFLOAT2& T1, const XMFLOAT2& T2, const XMFLOAT2& T3)
{
	// 삼각형의 세 변에 대한 외적 계산
	float cross1 = CrossProduct2D(XMVectorSet(T1.x,T1.y,0.0f,0.0f),XMVectorSet( T2.x,T2.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));
	float cross2 = CrossProduct2D(XMVectorSet(T2.x,T2.y,0.0f,0.0f),XMVectorSet( T3.x,T3.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));
	float cross3 = CrossProduct2D(XMVectorSet(T3.x,T3.y,0.0f,0.0f),XMVectorSet( T1.x,T1.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));

	// 1. 점이 삼각형 내부에 있는지 확인 (모든 외적의 부호가 같아야 함)
	bool isInside = (cross1 > 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0);

	// 2. 점이 삼각형의 변 위에 있는지 확인 (외적이 0이고 범위 내에 있어야 함)
	bool isOnEdge = 
		(std::abs(cross1) < FLT_EPSILON) || 
		(std::abs(cross2) < FLT_EPSILON) || 
		(std::abs(cross3) < FLT_EPSILON);
	// 결과 반환
	if (isOnEdge) 
	{
		return EPointAndTriangleState::PATS_InEdge;
	} else if (isInside) 
	{
		return EPointAndTriangleState::PATS_Inside;
	}
	else 
	{
		return EPointAndTriangleState::PATS_Outside;
	}
}

XMFLOAT2 GetDrawPanelPosFromLocal(const XMFLOAT2& LocalPos, const XMFLOAT2& LocalHorizontalSize, const XMFLOAT2 LocalVerticalSize, const XMFLOAT2& DrawPanelSize, bool bFlipY = true)
{
	// LocalPos를 DrawPanelSize로 정규화
	// ex) [-100,100], [0,600] 같은 Local Size를
	//		[0, 400] , [0,300] 같은 DrawPanel Size로 조정
	// 특히, 윈도우 포지션은 좌상단이 0,0 이지만,
	// DrawPanel은 좌 하단이므로, 해당 값에 맞게 반환하기 위해 bFlipY 값을 통해 반영
	XMFLOAT2 ReturnValue{LocalPos};

	ReturnValue.x = ReturnValue.x - min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y - min(LocalVerticalSize.x,LocalVerticalSize.y);

	float Width = max(LocalHorizontalSize.x,LocalHorizontalSize.y) - min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x,LocalVerticalSize.y) - min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (DrawPanelSize.x / Width);
	ReturnValue.y = ReturnValue.y * (DrawPanelSize.y / Height);

	if(bFlipY)
	{
		ReturnValue.y = DrawPanelSize.y-ReturnValue.y;
	}
	return ReturnValue;
}

XMFLOAT2 GetLocalPosFromDrawPanel(const XMFLOAT2& DrawPanelPos, const XMFLOAT2& LocalHorizontalSize, const XMFLOAT2 LocalVerticalSize, const XMFLOAT2& DrawPanelSize)
{
	XMFLOAT2 ReturnValue{DrawPanelPos};


	float Width = max(LocalHorizontalSize.x,LocalHorizontalSize.y) - min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x,LocalVerticalSize.y) - min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (Width / DrawPanelSize.x);
	ReturnValue.y = ReturnValue.y * (Height / DrawPanelSize.y);

	ReturnValue.x = ReturnValue.x + min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y + min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.y = LocalVerticalSize.y - ReturnValue.y -5.0f; // Gap;

	return ReturnValue;
}



void UTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	UActorComponent::DrawDetailPanel(ComponentDepth);

	if(ImGui::SliderFloat("TestValue", &TestValue1, 0.0f,600.0f))
	{
		
	}

	
	
	auto d2 = ImGui::GetWindowDrawList();

	ImVec2 CurPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawRectPos = CurPos;
	
	d2->PushClipRect(DrawRectPos, DrawRectPos+DrawSize);
	d2->AddRectFilled(DrawRectPos, DrawRectPos+ DrawSize, IM_COL32(50,50,50,255));


	for(int i = 0; i < CurrentEdge.size(); ++i)
	{
		XMFLOAT2 FirstPos = CurrentEdge[i]->StartPoint->Position;
		XMFLOAT2 SecondPos = CurrentEdge[i]->EndPoint->Position;
		XMFLOAT2 AdjustedFirstPos = GetDrawPanelPosFromLocal(FirstPos,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
		XMFLOAT2 AdjustedSecondPos= GetDrawPanelPosFromLocal(SecondPos, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});

		d2->AddLine(ImVec2{DrawRectPos.x+ AdjustedFirstPos.x,DrawRectPos.y+AdjustedFirstPos.y}, ImVec2{DrawRectPos.x+AdjustedSecondPos.x,DrawRectPos.y+AdjustedSecondPos.y}, IM_COL32(150,150,150,255),1);
	}

	for(int i = 0 ; i < TestValue.size(); ++i)
	{
		XMFLOAT2 AdjustedTestValue = GetDrawPanelPosFromLocal(TestValue[i]->Position, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
		d2->AddCircleFilled(ImVec2{DrawRectPos.x + AdjustedTestValue.x, DrawRectPos.y + AdjustedTestValue.y}, 5, IM_COL32(255,255,255,255));
	}

	ImGuiIO& IO = ImGui::GetIO();
	static ImVec2 CurTestPos;
	if(IO.KeyCtrl)
	{
		CurTestPos= IO.MousePos - ImVec2{10.0f,-10.0f};
	}
	XMFLOAT2 LeftTopBorderPos = GetDrawPanelPosFromLocal({HorizontalValue.x+Gap.x, VerticalValue.x + Gap.y}, HorizontalValue,VerticalValue,{DrawSize.x, DrawSize.y},false);
	XMFLOAT2 RightBottomBorderPos = GetDrawPanelPosFromLocal({HorizontalValue.y - Gap.x, VerticalValue.y-Gap.y}, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y},false);
	CurTestPos.x = std::clamp(CurTestPos.x,LeftTopBorderPos.x+DrawRectPos.x, RightBottomBorderPos.x+DrawRectPos.x);
	CurTestPos.y = std::clamp(CurTestPos.y,LeftTopBorderPos.y+DrawRectPos.y, RightBottomBorderPos.y+DrawRectPos.y);



	// 마우스 위치에서 Rect시작 위치를 뺴줌으로써 x축은 0부터 시작하지만, y축은 반대의 좌표계를 가지므로,
	// y축의 height에서 해당 값을 빼주어야 해당 렉트의 위치로 적용된다.
	XMFLOAT2 CurTestPosToRectSize {CurTestPos.x - DrawRectPos.x,(CurTestPos.y - DrawRectPos.y)};
	CurrentValue = GetLocalPosFromDrawPanel(CurTestPosToRectSize, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	
	for(int i = 0; i < CurrentTriangles.size(); ++i)
	{
		XMFLOAT2 AdjustedT1 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[0]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
		XMFLOAT2 AdjustedT2 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[1]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
		XMFLOAT2 AdjustedT3 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[2]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});

		EPointAndTriangleState State = FindPointAndTriangleState(
			CurTestPosToRectSize,
			AdjustedT1,
			AdjustedT2,
			AdjustedT3
		);
		if(State== EPointAndTriangleState::PATS_Inside)
		{
			d2->AddTriangleFilled(
				{DrawRectPos.x + AdjustedT1.x, DrawRectPos.y + AdjustedT1.y},
				{DrawRectPos.x + AdjustedT2.x, DrawRectPos.y + AdjustedT2.y},
				{DrawRectPos.x + AdjustedT3.x, DrawRectPos.y + AdjustedT3.y},
				IM_COL32(200,200,200,255)
			);

			

			break;
		}
		else if(State == EPointAndTriangleState::PATS_InEdge)
		{
			for(int EdgeIndex = 0; EdgeIndex < CurrentEdge.size();++EdgeIndex)
			{
				// TODO: 엣지에 있을 때에 대한 처리...
				
			}
		}
	}


	d2->AddLine(ImVec2{ CurTestPos.x-5.0f, CurTestPos.y}, ImVec2{CurTestPos.x+5.0f,CurTestPos.y},IM_COL32(0,255,0,255),2);
	d2->AddLine(ImVec2{ CurTestPos.x, CurTestPos.y-5.0f}, ImVec2{CurTestPos.x,CurTestPos.y+5.0f},IM_COL32(0,255,0,255),2);

	d2->PopClipRect();
	
}

void UTestComponent::UpdateAnimation(float CurrentAnimTime)
{
}
