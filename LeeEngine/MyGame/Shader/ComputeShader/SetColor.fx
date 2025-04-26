#ifndef _SET_COLOR
#define _SET_COLOR

cbuffer Data : register(b5)
{
    float4 ClearColor;
    int TextureWidth;
    int TextureHeight;
    float2 pad;
}

// t 레지스터, ReadOnly

// u 레지스터, UnorderedAccess, Read, Write
// 컴퓨트쉐이더 전용 레지스터

// ==========
// SetColorCS
// ==========
// SV_GroupThreadID     : 호출된 스레드가 본인이 속한 그룹안에서의 좌표
// SV_GroupIndex        : SV_GroupThreadID 를 1차원 인덱스로 변경
// SV_GroupID           : 호출된 스레드가 속한 그룹 ID 
// SV_DispatchThreadID  : 호출된 스레드의 고유 ID, 모든 그룹을 통틀어서 계산된 스레드의 좌표

RWTexture2D<float4> g_Target : register(u0);

// HLSL 5.0 기준 스레드 개수는 1024개 제한
[numthreads(32, 32, 1)]
void CS_SetColor( int3 _ID              : SV_DispatchThreadID
    , int3 _GroupThreadID   : SV_GroupThreadID
    , int  _GroupIndex      : SV_GroupIndex
    , int3 _GroupID        : SV_GroupID)
{    
   

    g_Target[_ID.xy] = float4(0.0, 1.0,1.0,1.0);
}

#endif
