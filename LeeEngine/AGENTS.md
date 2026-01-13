# Repository Guidelines

## Project Structure & Module Organization
This repository is a Visual Studio solution rooted at `LeeEngine.sln` with two primary projects. Engine code lives under `LeeEngine/Source` (for example `DirectX/`, `Engine/`, and `ThirdParty/`). Game-specific logic is in `MyGame/Source/MyGame` with feature folders such as `Actor/`, `Component/`, and `Core/`. Runtime assets are stored in `MyGame/Content`, shaders in `MyGame/Shader`, and configuration in `MyGame/Config`. Build artifacts and intermediates appear in `Executable/` and alongside `MyGame/*.exe/.pdb`. Planning notes live in `Document/`.

## Build, Test, and Development Commands
Use Visual Studio 2022 (v143 toolset) for day-to-day work: open `LeeEngine.sln`, select `Debug|x64` or `Release|x64`, and build the `LeeEngine` and `MyGame` projects. For command-line builds, use MSBuild from a VS Developer PowerShell:

```powershell
msbuild LeeEngine.sln /p:Configuration=Debug /p:Platform=x64
```

Run the game after a successful build:

```powershell
MyGame\MyGame.exe
```

## Coding Style & Naming Conventions
`MyGame/.editorconfig` is the source of truth for formatting. Use tabs with width 4, keep braces on the same line, insert spaces around operators, and align pointers/references to the left. Naming follows Unreal-style prefixes in the codebase (for example `A*` for actors and `U*` for engine-style objects). Keep feature folders grouped by gameplay area under `MyGame/Source/MyGame`.

## Testing Guidelines
No dedicated unit test framework is present. Validate changes by running the game and exercising relevant levels and systems (for example load test assets such as `TestLevel` under `MyGame/Content/4Level`). When adding temporary test actors or assets, follow the existing `Test*` naming (for example `ATestCube`). For shader or rendering changes, verify in a representative scene and capture a quick note on what was checked.

## Commit & Pull Request Guidelines
Commit messages in the history are short, descriptive phrases without prefixes; keep that style and focus on the user-visible change. For pull requests, include a concise summary, link related issues/tasks when available, list the build configuration used, and describe how you validated changes. Attach screenshots or short clips when modifying assets, shaders, UI, or levels.

## Configuration & Third-Party Assets
Third-party binaries (for example Assimp, FMOD, PhysX DLLs) live alongside `MyGame` outputs and should not be hand-edited. Keep `MyGame/Config` changes minimal and scoped to the feature you are working on.

내가 지시한 내용을 나한테 한번 더 물어봐서 너가 잘 이해했는지 확인받아줘

파일 수정, 삭제, 추가 등 변경사항이 있으면 어떤 파일을 어떻게 수정하거나 삭제할건지 코드도 자세히 알려줘

내가 지시한 내용 이외의 일은 하지 말아줘

C:\Project\DXStudy\LeeEngine