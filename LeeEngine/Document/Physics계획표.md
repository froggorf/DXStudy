## 주 역할
- 기본적인 물리 
- 오브젝트간 overlap, hit 이벤트를 처리 해야함
- 특정 오브젝트간에만 충돌체크가 가능하도록 Collision Type 도 지정해야함


## 계획표
[x] 0. convex hull을 만들기 위해서 static mesh의 vertices를 얻어오는 정보를 만들어야함


[x] 1. 그럼 일단 UShapeComponent 를 만들고 얘가 PxRigidActor* 을 관리하게 하고

[ ] 1-1. 이때 PxRigidActor*는 userData에다가 UShapeComponent를 건네줌으로써 만약 overlap 이나 hit 이벤트같은걸 처리하고싶을 때 처리할 수 있게 구현

[ ] 1-2. 이 때 UPrimitiveComponent가 렌더링쓰레드로 연결되는데, UPrimitiveComponent 단위로 UShapeComponent를 가지도록 구현

[ ] 1-3. 그리고 디버깅 드로우같은 경우에는 UShapeComponent 내에 bool값 bDebugDraw 를 통해 관리되는데,
렌더링 쓰레드에서 해당 shape를 받아와서 가장 마지막에 드로우 하도록 할 예정

[ ] 2. UShapeComponent를 상속받아서 UBoxComponent / USphereComponent / ... 등을 만들고 각각 Shape를 해당 모양으로 지정해준다

[ ] 3. UShapeComponent 에는 bool bIsDynamic 을 통해 PxRigidDynamic을 만들지 PxRigidStatic을 만들지가 정해짐


여기까지가 일단 RigidBody를 관리하는 방법이고,

[ ] 4. PhysX 라이브러리에 필요한 애들
에를들어 PxDefaultAllocator, PxDefaultErrorCallBack / PxFoundation , PxPhysics, PxScene 은 
UPhysicsEngine 이라는 클래스로 관리하고 gPhysicsEngine 이라는 이름으로 전역변수로 선언할 것

그래서 Engine::Tick 에서 1/60초마다 업데이트를 진행하게 하고,