// Pre Compiled Header
#pragma once

// 기본 Windows 헤더
#include <crtdbg.h>
#include <windows.h>
#include <tchar.h>

// DirectX 11 헤더
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

// STL 헤더 (자주 사용되는 표준 라이브러리)
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>
#include <queue>
#include <memory>

// 기타 유틸리티 헤더
#include <thread>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <cassert>
#include <threads.h>

// 게임 엔진 개발 관련 공통 헤더 (예: 유틸리티 함수)
#include "DirectX/d3dUtil.h"		// DirectX 초기화 및 설정 관련 유틸리티
#include "Engine/MyEngineUtils.h"	// 엔진 관련 공통 유틸리티

#include "Engine/DirectX/Device.h"	// DirectX 디바이스 클래스

// ThirdParty
// ImGui & ImGuiGizmo
#include "backends/imgui_impl_win32.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "imgui_internal.h"
#include "imgui.h"
#include "ImGuizmo.h"

// Assimp
#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// DirectXTex
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// nlohmann/json
#include "nlohmann/json.hpp"

// FMOD
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>
#include <FMOD/fmod_common.h>
#include <FMOD/fmod_dsp.h>
#include <FMOD/fmod_dsp_effects.h>
#include <FMOD/fmod_errors.h>
#include <FMOD/fmod_output.h>

// PhysX
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/cooking/PxCooking.h>
#include <PhysX/cooking/PxConvexMeshDesc.h>