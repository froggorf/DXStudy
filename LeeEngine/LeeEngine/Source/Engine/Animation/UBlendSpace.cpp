#include "CoreMinimal.h"
#include "UBlendSpace.h"

#include "Engine/Misc/QueuedThreadPool.h"
#include "Engine/RenderCore/EditorScene.h"
XMFLOAT2 UBlendSpace::Gap = {5.0f,5.0f};
// ========================================================================
bool operator==(const XMFLOAT2& A, const XMFLOAT2& B)
{
	return A.x == B.x&& A.y == B.y;
}
float CrossProduct2D(const XMVECTOR& P, const XMVECTOR& Q, const XMVECTOR& R) {
	XMVECTOR PQ = XMVectorSubtract(Q, P);
	XMVECTOR PR = XMVectorSubtract(R, P);

	XMVECTOR cross = XMVector3Cross(PQ, PR);

	// z 좌표 반환 (2D 외적의 크기)
	return XMVectorGetZ(cross);
}

bool FAnimClipPoint::IsPointOnSegment( const XMFLOAT2& P1, const XMFLOAT2& P2) const
{
	XMVECTOR P = XMVectorSubtract(XMVectorSet(P1.x,P1.y,0.0f,0.0f), XMVectorSet(P2.x,P2.y,0.0f,0.0f));
	XMVECTOR AP1 = XMVectorSubtract(XMVectorSet(Position.x,Position.y,0.0f,0.0f),XMVectorSet(P1.x,P1.y,0.0f,0.0f));

	// 선위에 없음
	if(std::abs(XMVectorGetZ(XMVector3Cross(P,AP1))) > FLT_EPSILON)
	{
		return false;
	}

	// 2. 점 A가 선분 BC의 범위 내에 있는지 확인
	if ((Position.x <= std::min(P1.x, P2.x) || Position.x >= max(P1.x, P2.x)) && (Position.y <= std::min(P1.y, P2.y) || Position.y >= max(P1.y, P2.y))) return false;


	return true;
}

bool FAnimClipEdge::IsPointOnSegment(const XMFLOAT2& TargetPoint)
{
	XMVECTOR P = XMVectorSubtract(XMVectorSet(StartPoint->Position.x,StartPoint->Position.y,0.0f,0.0f), XMVectorSet(EndPoint->Position.x,EndPoint->Position.y,0.0f,0.0f));
	XMVECTOR AP1 = XMVectorSubtract(XMVectorSet(TargetPoint.x,TargetPoint.y,0.0f,0.0f),XMVectorSet(StartPoint->Position.x,StartPoint->Position.y,0.0f,0.0f));

	// 선위에 없음
	if(std::abs(XMVectorGetZ(XMVector3Cross(P,AP1))) > FLT_EPSILON)
	{
		return false;
	}

	// 2. 점 A가 선분 BC의 범위 내에 있는지 확인
	if ((TargetPoint.x <= std::min(StartPoint->Position.x, EndPoint->Position.x) || TargetPoint.x >= max(StartPoint->Position.x, EndPoint->Position.x)) && (TargetPoint.y <= std::min(StartPoint->Position.y, EndPoint->Position.y) || TargetPoint.y >= max(StartPoint->Position.y, EndPoint->Position.y))) return false;


	return true;
}

EPointAndTriangleState FAnimClipTriangle::FindPointAndTriangleState(const XMFLOAT2& TargetPoint)
{
	// 삼각형의 세 변에 대한 외적 계산
	float cross1 = CrossProduct2D(XMVectorSet(Points[0]->Position.x,Points[0]->Position.y,0.0f,0.0f),XMVectorSet( Points[1]->Position.x,Points[1]->Position.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));
	float cross2 = CrossProduct2D(XMVectorSet(Points[1]->Position.x,Points[1]->Position.y,0.0f,0.0f),XMVectorSet( Points[2]->Position.x,Points[2]->Position.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));
	float cross3 = CrossProduct2D(XMVectorSet(Points[2]->Position.x,Points[2]->Position.y,0.0f,0.0f),XMVectorSet( Points[0]->Position.x,Points[0]->Position.y,0.0f,0.0f), XMVectorSet( TargetPoint.x,TargetPoint.y,0.0f,0.0f));

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

// ========================================================================

void UBlendSpace::GetAnimationBoneMatrices(const XMFLOAT2& AnimValue, float CurrentAnimTime, std::vector<XMMATRIX>& OutMatrices)
{

	for(int TriangleIndex = 0; TriangleIndex < CurrentTriangles.size(); ++TriangleIndex)
	{
	
		EPointAndTriangleState State = CurrentTriangles[TriangleIndex]->FindPointAndTriangleState(AnimValue);

		if(State == EPointAndTriangleState::PATS_Inside)
		{
			TriangleInterpolation(AnimValue,CurrentTriangles[TriangleIndex], CurrentAnimTime,OutMatrices);
			return ;
		}
		if(State == EPointAndTriangleState::PATS_InEdge)
		{
			const auto& Edges = CurrentTriangles[TriangleIndex]->Edges;
			for(int EdgeIndex = 0; EdgeIndex < Edges.size(); ++EdgeIndex)
			{
				const std::shared_ptr<FAnimClipEdge>& Edge = Edges[EdgeIndex];

				if(Edge->IsPointOnSegment(AnimValue))
				{
					LinearInterpolation(AnimValue, Edge, CurrentAnimTime,OutMatrices);
					return ;
				}

			}
		}	
	}
	//// 아무것도 없었을 경우 간선 바로 위에 있었는지 확인,
	//if(CurrentTriangles.size() == 0)
	//{
	//	const auto& Edges = CurrentEdge;
	//	for(int EdgeIndex = 0; EdgeIndex < Edges.size(); ++EdgeIndex)
	//	{
	//		const std::shared_ptr<FAnimClipEdge>& Edge = Edges[EdgeIndex];
	//		if(Edge->IsPointOnSegment(AnimValue) )
	//		{
	//			LinearInterpolation(AnimValue, Edge,CurrentAnimTime, OutMatrices);
	//			return;
	//		}
	//	}
	//}

	// 전부다 삼각형 / 엣지의 밖에 있었다면
	// 아무것도 없다면 가장 가까운 점을 그리기
	std::shared_ptr<FAnimClipPoint> NearestPoint;
	float DistanceSq = 10000;
	XMVECTOR CurrentValueVec = XMVectorSet(AnimValue.x,AnimValue.y,0.0f,0.0f);
	for(int PointIndex= 0; PointIndex < CurrentPoints.size();++PointIndex)
	{
		const std::shared_ptr<FAnimClipPoint>& TargetPoint = CurrentPoints[PointIndex];
		float DistanceToPoint =XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(CurrentValueVec, XMVectorSet(TargetPoint->Position.x,TargetPoint->Position.y,0.0f,0.0f))));
		if(DistanceToPoint < DistanceSq)
		{
			DistanceSq = DistanceToPoint;
			NearestPoint = TargetPoint;
		}
	}
	if(NearestPoint)
	{
		CalculateOneAnimation(NearestPoint, CurrentAnimTime,OutMatrices);
		return;
	}
}

void UBlendSpace::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimationAsset::LoadDataFromFileData(AssetData);


	std::vector<float> Horizontal = AssetData["HorizontalValue"];
	HorizontalValue = {Horizontal[0] - Gap.x,Horizontal[1] +Gap.x};
	std::vector<float> Vertical = AssetData["VerticalValue"];
	VerticalValue = {Vertical[0] - Gap.y,Vertical[1] +Gap.y};

	std::vector<nlohmann::json> Points = AssetData["Points"];
	for(const auto& Point : Points)
	{
		std::vector<float> Position = Point["Position"];
		std::string AnimClipName = Point["AnimClipName"];
		if(std::shared_ptr<UAnimSequence> AnimSequence = UAnimSequence::GetAnimationAsset(AnimClipName))
		{
			CurrentPoints.emplace_back(std::make_shared<FAnimClipPoint>(
				XMFLOAT2{Position[0], Position[1]},
				AnimSequence
				));	
		}
		else
		{
			// Non valid AnimClipName
			assert(nullptr, "NULL AnimClip's name");
		}
		
	}
	
	

	CreateEdgeAndTriangle();
}

void UBlendSpace::CreateEdgeAndTriangle()
{
	CurrentEdge.clear();
	CurrentTriangles.clear();
	CreateEdge();
	CreateTriangle();
}

void UBlendSpace::CreateEdge()
{

	for(int i =0; i < CurrentPoints.size()-1; ++i)
	{
		for(int j = i+1; j < CurrentPoints.size(); ++j)
		{
			// 1. n(n-1) 돌면서 간선을 만든다.
			std::shared_ptr<FAnimClipEdge> NewEdge = std::make_shared<FAnimClipEdge>(CurrentPoints[i],CurrentPoints[j]);




			// 2. 만약 그 간선 사이에 점이 있다면 그 간선은 못만든다(n)
			bool bIsOnLine = false;
			for(int k =0; k < CurrentPoints.size(); ++k)
			{
				if(CurrentPoints[k]->IsPointOnSegment(CurrentPoints[i]->Position,CurrentPoints[j]->Position))
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
						bool bIsXNotInterSect = (max(AStart.x, AEnd.x) <= std::min(BStart.x, BEnd.x) || max(BStart.x, BEnd.x) <= std::min(AStart.x, AEnd.x));
						bool bIsYNotInterSect = (max(AStart.y, AEnd.y) <= std::min(BStart.y, BEnd.y) || max(BStart.y, BEnd.y) <= std::min(AStart.y, AEnd.y));
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
}

void UBlendSpace::CreateTriangle()
{
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

void UBlendSpace::TriangleInterpolation(const XMFLOAT2& AnimValue,
	const std::shared_ptr<FAnimClipTriangle>& Triangle, float AnimTime, std::vector<XMMATRIX>& OutMatrices)
{

	{
		const std::shared_ptr<FAnimClipPoint>& P1 = Triangle->Points[0];
		const std::shared_ptr<FAnimClipPoint>& P2 = Triangle->Points[1];
		const std::shared_ptr<FAnimClipPoint>& P3 = Triangle->Points[2];

		XMVECTOR CurrentValueVector = XMVectorSet(AnimValue.x,AnimValue.y,0.0f,0.0f);
		XMVECTOR P1Vector = XMVectorSet(P1->Position.x,P1->Position.y,0.0f,0.0f);
		XMVECTOR P2Vector = XMVectorSet(P2->Position.x,P2->Position.y,0.0f,0.0f);
		XMVECTOR P3Vector = XMVectorSet(P3->Position.x,P3->Position.y,0.0f,0.0f);

		float P1AnimDuration = P1->AnimSequence->GetDuration();
		float P2AnimDuration = P2->AnimSequence->GetDuration();
		float P3AnimDuration = P3->AnimSequence->GetDuration();

		float WantedAnimDuration = 30.0f;
		float AnimTimeNormalized = fmod(AnimTime, WantedAnimDuration);
		float WantedTimeRate = AnimTimeNormalized / WantedAnimDuration;

		std::vector<XMMATRIX> P1AnimMatrices(MAX_BONES,XMMatrixIdentity());

		std::atomic<bool> bAnim1Updated = false;
		auto Anim1Work = [P1, P1AnimDuration, WantedTimeRate, &P1AnimMatrices]()
		{
			P1->AnimSequence->GetBoneTransform(P1AnimDuration*WantedTimeRate, (P1AnimMatrices));
		};
		GThreadPool->AddTask(
			FTask{
				2,
				Anim1Work,
				&bAnim1Updated}
				);


		std::atomic<bool> bAnim2Updated = false;
	
		std::vector<XMMATRIX> P2AnimMatrices(MAX_BONES,XMMatrixIdentity());
		auto Anim2Work = [P2, P2AnimDuration, WantedTimeRate, &P2AnimMatrices]()
		{
			P2->AnimSequence->GetBoneTransform(P2AnimDuration*WantedTimeRate, (P2AnimMatrices));
		};
		GThreadPool->AddTask(
			FTask{
				2,
				Anim2Work,
			&bAnim2Updated}
		);

		std::atomic<bool> bAnim3Updated = false;
		std::vector<XMMATRIX> P3AnimMatrices(MAX_BONES,XMMatrixIdentity());
		auto Anim3Work = 		[P3, P3AnimDuration, WantedTimeRate, &P3AnimMatrices]()
		{ P3->AnimSequence->GetBoneTransform(P3AnimDuration*WantedTimeRate, (P3AnimMatrices)); };
		GThreadPool->AddTask(
			FTask{
				2,
			Anim3Work,
			&bAnim3Updated}
		);

		// 점의 거리를 기반으로 가중치 계산
		float P1LengthSq = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(P1Vector, CurrentValueVector)));
		float P2LengthSq = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(P2Vector, CurrentValueVector)));
		float P3LengthSq = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(P3Vector, CurrentValueVector)));
		float P1Weight = 1.0f / std::powf((P1LengthSq),4.0f);
		float P2Weight = 1.0f / std::powf((P2LengthSq),4.0f); // P2LengthEst/(P1LengthEst+P2LengthEst+P3LengthEst);
		float P3Weight = 1.0f / std::powf((P3LengthSq),4.0f); //P1Weight-P2Weight;
		float WeightSumBeforeNormalize = P1Weight+P2Weight+P3Weight;
		P1Weight = P1Weight/WeightSumBeforeNormalize;
		P2Weight = P2Weight/WeightSumBeforeNormalize;
		P3Weight = 1- P1Weight - P2Weight;

		while (!bAnim1Updated || !bAnim2Updated || !bAnim3Updated)
		{
			std::this_thread::yield();
		}

		
		for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
		{
			for(int VectorIndex = 0; VectorIndex < 4; ++VectorIndex)
			{
				XMVECTOR WeightedP1 = XMVectorScale(P1AnimMatrices[BoneIndex].r[VectorIndex], P1Weight);
				XMVECTOR WeightedP2 = XMVectorScale(P2AnimMatrices[BoneIndex].r[VectorIndex], P2Weight);
				XMVECTOR WeightedP3 = XMVectorScale(P3AnimMatrices[BoneIndex].r[VectorIndex], P3Weight);
				OutMatrices[BoneIndex].r[VectorIndex]= XMVectorAdd(XMVectorAdd(WeightedP1,WeightedP2),WeightedP3);
			}
		}
	}

	
}

void UBlendSpace::LinearInterpolation(const XMFLOAT2& CurrentValue,
	const std::shared_ptr<FAnimClipEdge>& Edge, float AnimTime, std::vector<XMMATRIX>& OutMatrices)
{
	const std::shared_ptr<FAnimClipPoint>& P1 = Edge->StartPoint;
	const std::shared_ptr<FAnimClipPoint>& P2 = Edge->EndPoint;
	XMVECTOR CurrentValueVector = XMVectorSet(CurrentValue.x,CurrentValue.y,0.0f,0.0f);
	XMVECTOR P1Vector = XMVectorSet(P1->Position.x,P1->Position.y,0.0f,0.0f);
	XMVECTOR P2Vector = XMVectorSet(P2->Position.x,P2->Position.y,0.0f,0.0f);

	float P1AnimDuration = P1->AnimSequence->GetDuration();
	float P2AnimDuration = P2->AnimSequence->GetDuration();

	//float WantedAnimDuration = (P1AnimDuration+P2AnimDuration)/2;
	float WantedAnimDuration = 30.0f;
	float AnimTimeNormalized = fmod(AnimTime, WantedAnimDuration);
	float WantedTimeRate = AnimTimeNormalized / WantedAnimDuration;

	std::vector<XMMATRIX> P1AnimMatrices(MAX_BONES,XMMatrixIdentity());

	std::atomic<bool> bAnim1Updated = false;
	auto Anim1Work = [P1, P1AnimDuration, WantedTimeRate, &P1AnimMatrices]()
		{
			P1->AnimSequence->GetBoneTransform(P1AnimDuration*WantedTimeRate, (P1AnimMatrices));
		};
	GThreadPool->AddTask(
		FTask{
			2,
			Anim1Work,
			&bAnim1Updated}
			);


	std::atomic<bool> bAnim2Updated = false;

	std::vector<XMMATRIX> P2AnimMatrices(MAX_BONES,XMMatrixIdentity());
	auto Anim2Work = [P2, P2AnimDuration, WantedTimeRate, &P2AnimMatrices]()
		{
			P2->AnimSequence->GetBoneTransform(P2AnimDuration*WantedTimeRate, (P2AnimMatrices));
		};
	GThreadPool->AddTask(
		FTask{
			2,
			Anim2Work,
			&bAnim2Updated}
			);

	float P1LengthEst = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(P1Vector, CurrentValueVector)));
	float P2LengthEst = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract(P2Vector, CurrentValueVector)));
	float P1Weight = P1LengthEst/(P1LengthEst+P2LengthEst);

	while (!bAnim1Updated || !bAnim2Updated )
	{
		std::this_thread::yield();
	}


	for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
	{
		XMMatrixLerp(P1AnimMatrices[BoneIndex], P2AnimMatrices[BoneIndex], P1Weight, OutMatrices[BoneIndex]);
	}

	
}

void UBlendSpace::CalculateOneAnimation(const std::shared_ptr<FAnimClipPoint>& Point,
	float AnimTime, std::vector<XMMATRIX>& OutMatrices)
{
	Point->AnimSequence->GetBoneTransform(fmod(AnimTime,Point->AnimSequence->GetDuration()), OutMatrices);
}
