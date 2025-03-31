#include "UTestComponent.h"

#include <Engine/RenderCore/EditorScene.h>
bool operator==(const XMFLOAT2& A, const XMFLOAT2& B)
{
	return A.x == B.x&& A.y == B.y;
}

UTestComponent::UTestComponent()
{
	TestValue = {{0.0f,0.0f}, {100.0f,0.0f}, {50.0f,-50.0f}, {0.0f,-100.0f}, {0.0f,100.0f}, {-100.0f,0.0f}};
	for(int i =0; i < TestValue.size()-1; ++i)
	{
		for(int j = i+1; j < TestValue.size(); ++j)
		{
			// 1. n(n-1) 돌면서 간선을 만든다.
			Edge NewEdge;
			NewEdge.StartPos = TestValue[i];
			NewEdge.EndPos = TestValue[j];

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
				if(IsPointOnSegment(TestValue[k], TestValue[i],TestValue[j]))
				{
					bIsOnLine = true;
					break;
				}	
			}
			if(bIsOnLine)
			{
				continue;
			}

			NewEdge.LengthSq = XMVectorGetX( XMVector2LengthSq(XMVectorSubtract(XMLoadFloat2(&NewEdge.StartPos), XMLoadFloat2(&NewEdge.EndPos))));

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
				if(IsSegmentsIntersect(NewEdge.StartPos,NewEdge.EndPos, CurrentEdge[EdgeIndex].StartPos,CurrentEdge[EdgeIndex].EndPos))
				{
					bIsIntersecting = true;
					if(NewEdge.LengthSq < CurrentEdge[EdgeIndex].LengthSq)
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


	std::vector<Edge> ToBeTriangleEdge{CurrentEdge.begin(),CurrentEdge.end()};
	for(int TargetEdgeIndex = 0; TargetEdgeIndex < ToBeTriangleEdge.size(); ++TargetEdgeIndex)
	{

		for(int NextEdgeIndex = TargetEdgeIndex +1; NextEdgeIndex < ToBeTriangleEdge.size(); ++NextEdgeIndex)
		{
			// 시작점 or 끝점이 같은 것이 있는지 확인
			bool bIsSameStart = ToBeTriangleEdge[TargetEdgeIndex].StartPos == ToBeTriangleEdge[NextEdgeIndex].StartPos;
			bool bIsSameEnd = ToBeTriangleEdge[TargetEdgeIndex].EndPos == ToBeTriangleEdge[NextEdgeIndex].EndPos;
			bool bIsSameAStartAndBEnd = (ToBeTriangleEdge[TargetEdgeIndex].StartPos == ToBeTriangleEdge[NextEdgeIndex].EndPos);
			bool bIsSameAEndAndBStart = (ToBeTriangleEdge[TargetEdgeIndex].EndPos== ToBeTriangleEdge[NextEdgeIndex].StartPos);

			// 같은 것이 있다면 (이어지는 엣지가 있다면)
			if(bIsSameStart || bIsSameEnd || bIsSameAStartAndBEnd || bIsSameAEndAndBStart)
			{
				// 찾으려는 간선의 위치
				XMFLOAT2 FindEdgeFirstPos, FindEdgeSecondPos;
				if(bIsSameStart)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex].EndPos;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex].EndPos;
				}
				if(bIsSameEnd)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex].StartPos;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex].StartPos;
				}
				if(bIsSameAStartAndBEnd)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex].EndPos;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex].StartPos;	
				}
				if(bIsSameAEndAndBStart)
				{
					FindEdgeFirstPos = ToBeTriangleEdge[TargetEdgeIndex].StartPos;
					FindEdgeSecondPos = ToBeTriangleEdge[NextEdgeIndex].EndPos;	
				}

				for(int FoundEdgeIndex = NextEdgeIndex + 1 ; FoundEdgeIndex < ToBeTriangleEdge.size(); ++FoundEdgeIndex)
				{
					// 해당하는 간선을 찾음
					if(ToBeTriangleEdge[FoundEdgeIndex].StartPos == FindEdgeFirstPos && ToBeTriangleEdge[FoundEdgeIndex].EndPos == FindEdgeSecondPos
						|| ToBeTriangleEdge[FoundEdgeIndex].EndPos == FindEdgeFirstPos && ToBeTriangleEdge[FoundEdgeIndex].StartPos == FindEdgeSecondPos)
					{
						Triangle NewTriangle;
						NewTriangle.Edges.push_back(ToBeTriangleEdge[TargetEdgeIndex]);
						NewTriangle.Edges.push_back(ToBeTriangleEdge[NextEdgeIndex]);
						NewTriangle.Edges.push_back(ToBeTriangleEdge[FoundEdgeIndex]);

						NewTriangle.Points.push_back(ToBeTriangleEdge[FoundEdgeIndex].StartPos);
						NewTriangle.Points.push_back(ToBeTriangleEdge[FoundEdgeIndex].EndPos);

						XMFLOAT2 OtherPoint;
						if(ToBeTriangleEdge[TargetEdgeIndex].StartPos == ToBeTriangleEdge[FoundEdgeIndex].StartPos
							|| ToBeTriangleEdge[TargetEdgeIndex].StartPos == ToBeTriangleEdge[FoundEdgeIndex].EndPos)
						{
							OtherPoint = ToBeTriangleEdge[TargetEdgeIndex].EndPos;
						}
						else if(ToBeTriangleEdge[TargetEdgeIndex].EndPos == ToBeTriangleEdge[FoundEdgeIndex].StartPos
							|| ToBeTriangleEdge[TargetEdgeIndex].EndPos == ToBeTriangleEdge[FoundEdgeIndex].EndPos)
						{
							OtherPoint = ToBeTriangleEdge[TargetEdgeIndex].StartPos;
						}
						NewTriangle.Points.push_back(OtherPoint);

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

void UTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	UActorComponent::DrawDetailPanel(ComponentDepth);

	if(ImGui::SliderFloat("TestValue", &TestValue1, 0.0f,600.0f))
	{
		
	}


	
	auto d2 = ImGui::GetWindowDrawList();

	ImVec2 CurPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawRectPos = CurPos + ImVec2{10.0f,10.0f};
	ImVec2 DrawSize = ImVec2{400.0f,300.0f};
	d2->AddRectFilled(DrawRectPos, DrawRectPos+ DrawSize, IM_COL32(50,50,50,255));


	XMFLOAT2 HorizontalValue = {-100.0f,100.0f};
	XMFLOAT2 VerticalValue = {-100.0f, 100.0f};
	XMFLOAT2 Gap = {10.0f,10.0f};

	ImVec2 RectLeftBottom = DrawRectPos; RectLeftBottom.y += DrawSize.y;

	std::function<XMFLOAT2(const XMFLOAT2& WantPos, const XMFLOAT2& CurrentSize, const XMFLOAT2& WantSize)> AdjustSize = [](const XMFLOAT2& WantPos, const XMFLOAT2& CurrentSize, const XMFLOAT2& WantSize)->XMFLOAT2
	{
		XMFLOAT2 AdjustSize;
		AdjustSize.x = WantSize.x/CurrentSize.x;
		AdjustSize.y = WantSize.y/CurrentSize.y;
		return XMFLOAT2{WantPos.x*AdjustSize.x, WantPos.y*AdjustSize.y};
	};

	XMFLOAT2 CurrentTestValueSize = {HorizontalValue.y - HorizontalValue.x ,VerticalValue.y - VerticalValue.x};
	for(int i = 0 ; i < TestValue.size(); ++i)
	{
		XMFLOAT2 AdjustedTestValue = AdjustSize(XMFLOAT2{TestValue[i].x - HorizontalValue.x, TestValue[i].y - HorizontalValue.x}, CurrentTestValueSize, {DrawSize.x,DrawSize.y});
		d2->AddCircleFilled(ImVec2{RectLeftBottom.x + AdjustedTestValue.x, RectLeftBottom.y-AdjustedTestValue.y}, 5, IM_COL32(255,255,255,255));
	}

	
}
