// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "renderingthread.h"

std::shared_ptr<FScene> FRenderCommandExecutor::CurrentSceneData = nullptr;



std::unordered_map<std::string,std::function<void()>> FScene::ImGuiRenderFunctions;
std::unordered_map<std::string,std::function<void()>> FScene::ImGuizmoRenderFunctions;


// 디버깅 콘솔
std::vector<DebugText> FScene::DebugConsoleText;
std::vector<DebugText> FScene::PendingAddDebugConsoleText;
std::vector<DebugText> FScene::SearchingDebugConsoleText;
std::string FScene::DebugConsoleSearchText;