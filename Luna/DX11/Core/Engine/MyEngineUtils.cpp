#include "MyEngineUtils.h"
//
//XMFLOAT3 FTransform::CalculateEulerRotationFromQuaternion(const XMVECTOR& Quaternion)
//{
//	//02.19 임시 코드
//	// 쿼터니언 성분 추출
//    XMFLOAT4 q;
//    XMStoreFloat4(&q, Quaternion);
//
//    float x = q.x;
//    float y = q.y;
//    float z = q.z;
//    float w = q.w;
//
//    XMFLOAT3 Euler;
//
//    // Pitch (X축 회전)
//    float sinPitch = 2.0f * (w * x - y * z);
//    if (std::abs(sinPitch) >= 1.0f) {
//        // Gimbal lock: pitch가 ±90도에 가까울 때
//        Euler.x = std::copysign(DirectX::XM_PIDIV2, sinPitch); // ±π/2
//    } else {
//        Euler.x = std::asin(sinPitch);
//    }
//
//    // Yaw (Y축 회전)
//    Euler.y= std::atan2(2.0f * (w * y + z * x), 1.0f - 2.0f * (x * x + y * y));
//
//    // Roll (Z축 회전)
//    Euler.z= std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
//
//    Euler.x = XMConvertToDegrees(Euler.x);
//    Euler.y = XMConvertToDegrees(Euler.y);
//    Euler.z = XMConvertToDegrees(Euler.z);
//    return Euler;
//}
