// TODO: 임시로 txt 파일로 작성, 추후 마크다운 변경 예정

// 02.15 구현
게임 내 오브젝트로 사용할 데이터의 파일 타입은 .myasset 으로 관리
예시)
SM_Cube.myasset
{// 내부 텍스트
0
Model/Cube.fbx
Texture/CubeTexture.png
}
0 -> StaticMesh에 대한 데이터임을 명시
Model/Cube.fbx -> 모델 데이터 경로를 명시
Texture/CubeTexture.png -> 텍스쳐 데이터 경로를 명시