#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"

class ULoginWidget : public UUserWidget
{
	MY_GENERATE_BODY(ULoginWidget)
	ULoginWidget() = default;
	~ULoginWidget() override = default;

	void NativeConstruct() override;
	void Tick(float DeltaSeconds) override;

private:

	void SetLoginButtonText();
	void SetRegisterButtonText();
	void OnLoginButtonClicked();
	void OnRegisterButtonClicked();
	void OnUsernameCommitted(const std::wstring& Text);
	void OnPasswordCommitted(const std::wstring& Text);

private:
	// 배경 이미지
	std::shared_ptr<FImageWidget> BackgroundImage;

	// 입력 필드들
	std::shared_ptr<FEditableTextWidget> UsernameInput;
	std::shared_ptr<FEditableTextWidget> PasswordInput;

	// 로그인 버튼
	std::shared_ptr<FButtonWidget> LoginButton;
	std::shared_ptr<FTextWidget> LoginButtonText;

	// 회원등록 버튼
	std::shared_ptr<FButtonWidget> RegisterButton;
	std::shared_ptr<FTextWidget> RegisterButtonText;

	std::shared_ptr<FTextWidget> WarningText;

	FTimerHandle LoginButtonTimerHandle;
	FTimerHandle RegisterButtonTimerHandle;
	
};