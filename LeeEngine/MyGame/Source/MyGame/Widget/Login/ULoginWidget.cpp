#include "CoreMinimal.h"
#include "ULoginWidget.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Core/UMyGameInstance.h"

void ULoginWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	// ==================== 배경 이미지 ====================
	//BackgroundImage = std::make_shared<FImageWidget>(FImageBrush{
	//	UTexture::GetTextureCache("T_White"),  // 배경 텍스처
	//	{1.0f, 1.0f, 1.0f, 1.0f}
	//	});
	//BackgroundImage->AttachToWidget(MainCanvasWidget);
	//if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BackgroundImage->GetSlot()))
	//{
	//	CanvasSlot->Anchors = ECanvasAnchor::WrapAll;  // 전체 화면
	//	CanvasSlot->Position = {0, 0};
	//}
	//
	// ==================== Username 입력 필드 ====================
	UsernameInput = std::make_shared<FEditableTextWidget>();
	UsernameInput->SetHintText(L"ID");
	UsernameInput->SetMaxLength(20);
	UsernameInput->SetFontSize(30.0f);
	UsernameInput->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});  // 흰색 텍스트

	// 배경 스타일 (어두운 반투명)
	UsernameInput->SetBackgroundBrush(FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.5f, 0.5f, 0.5f, 0.8f}  // 어두운 반투명
		});
	UsernameInput->SetFocusedBrush(FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.1f, 0.1f, 0.1f, 0.9f}  // 포커스 시 조금 밝게
		});

	UsernameInput->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UsernameInput->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.0f};
		CanvasSlot->Position = {0, 0};
		CanvasSlot->Size = {450, 50};  // 입력 필드 크기
	}

	// ==================== Password 입력 필드 ====================
	PasswordInput = std::make_shared<FEditableTextWidget>();
	PasswordInput->SetHintText(L"PW");  // 비밀번호는 힌트 없음
	PasswordInput->SetIsPassword(true);  // 비밀번호 모드
	PasswordInput->SetMaxLength(30);
	PasswordInput->SetFontSize(30.0f);
	PasswordInput->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});

	PasswordInput->SetBackgroundBrush(FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.5f, 0.5f, 0.5f, 0.8f}
		});
	PasswordInput->SetFocusedBrush(FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.1f, 0.1f, 0.1f, 0.9f}
		});

	PasswordInput->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PasswordInput->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.0f};
		CanvasSlot->Position = {0, 60};  // Username 아래 10px 간격
		CanvasSlot->Size = {450, 50};
	}

	// ==================== 로그인 버튼 ====================
	LoginButton = std::make_shared<FButtonWidget>();

	// 버튼 스타일 (파란색 계열)
	LoginButton->SetStyle(EButtonType::Normal, FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.2f, 0.4f, 0.8f, 1.0f}  // 파란색
		});
	LoginButton->SetStyle(EButtonType::Hovered, FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.3f, 0.5f, 0.9f, 1.0f}  // 밝은 파란색
		});
	LoginButton->SetStyle(EButtonType::Pressed, FImageBrush{
		UTexture::GetTextureCache("T_White"),
		{0.1f, 0.3f, 0.7f, 1.0f}  // 어두운 파란색
		});

	LoginButton->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(LoginButton->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.0f};
		CanvasSlot->Position = {0, 130};  // Password 아래 20px 간격
		CanvasSlot->Size = {450, 55};     // 버튼 크기 (조금 더 크게)
	}

	// 로그인 버튼 텍스트
	LoginButtonText = std::make_shared<FTextWidget>();
	LoginButtonText->SetText(L"시작하기");
	LoginButtonText->SetFontSize(24.0f);
	LoginButtonText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});  // 흰색
	LoginButtonText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	LoginButtonText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	LoginButtonText->AttachToWidget(LoginButton);

	// ==================== 이벤트 바인딩 ====================

	// 로그인 버튼 클릭
	LoginButton->OnClicked.Add([this]()
		{
			OnLoginButtonClicked();
		});

	// Username에서 Enter 입력 시 Password로 포커스 이동
	UsernameInput->OnTextCommitted.Add([this](const std::wstring& Text)
		{
			OnUsernameCommitted(Text);
		});

	// Password에서 Enter 입력 시 로그인 시도
	PasswordInput->OnTextCommitted.Add([this](const std::wstring& Text)
		{
			OnPasswordCommitted(Text);
		});
}

void ULoginWidget::OnLoginButtonClicked()
{
	std::string Username = UsernameInput->GetText_String();
	std::string Password = PasswordInput->GetText_String();

	// 입력 검증
	if (Username.empty())
	{
		// TODO: 경고 메시지 표시
		MY_LOG("LOG", EDebugLogLevel::DLL_Warning, "Username is empty!");
		return;
	}

	if (Password.empty())
	{
		MY_LOG("LOG", EDebugLogLevel::DLL_Warning, "Password is empty!");
		return;
	}

	// 로그인 처리
	if (UMyGameInstance::GetInstance<UMyGameInstance>()->TryLogin(std::string{Username}, std::string{Password.data()}))
	{
		// 유저 ID 적용
		UMyGameInstance::GetInstance<UMyGameInstance>()->UserName = Username;
		// 모든 데이터 로드
		UMyGameInstance::GetInstance<UMyGameInstance>()->LoadInitialData();
		// 레벨 타운으로 옮기기
		GEngine->ChangeLevelByName("TownLevel");
	}
	

	// TODO: 실제 로그인 로직 구현
	// - 서버에 로그인 요청
	// - 성공 시 메인 화면으로 전환
	// - 실패 시 에러 메시지 표시
}

void ULoginWidget::OnUsernameCommitted(const std::wstring& Text)
{
	// Username에서 Enter 입력 시 Password로 포커스 이동
	if (!Text.empty())
	{
		PasswordInput->SetFocus(true);
	}
}

void ULoginWidget::OnPasswordCommitted(const std::wstring& Text)
{
	// Password에서 Enter 입력 시 로그인 시도
	if (!Text.empty())
	{
		OnLoginButtonClicked();
	}
}

void ULoginWidget::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);

	// ESC 키로 종료 (필요시)
	if (ImGui::IsKeyDown(ImGuiKey_Escape))
	{
		// TODO: 종료 확인 다이얼로그 표시
	}
}
