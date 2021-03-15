// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HttpModule.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "JsonReader.h"
#include "JsonObject.h"
#include "JsonSerializer.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPPERAPP_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

private:

	int mode = 0;

	bool bUseRdp = false;


	UEditableTextBox *loginAccountTb;
	UEditableTextBox *loginPwdTb;
	UEditableTextBox *loginIpTb;

	UEditableTextBox *regAccountTb;
	UEditableTextBox *regPwdTb;
	UEditableTextBox *regIpTb;

	UButton *manualIpBtn;
	UTextBlock *foundIpTb;

	bool Initialize() override;

	void LoginRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess);

	void Login2RdpRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess);

	void Login2Rdp();

	void RegisterRequestComplete(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess);

	FString HashStringWithSHA1(const FString &inString);

	FString EncodeString(const FString &inString);

	FString makeUrl(const FString &ip, const FString &port, const FString &route);

	void SearchLocalServer();

	void OnPingCompleted(FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool bIsSuccess);
	
public:
	UFUNCTION(BlueprintCallable, Category = "clickFunction")
		void handleSwitchRegisterEvent();

	UFUNCTION(BlueprintCallable, Category = "clickFunction")
		void handleSwitchLoginEvent();

	UFUNCTION(BlueprintCallable, Category = "clickFunction")
		void handleLoginClickedEvent();

	UFUNCTION(BlueprintCallable, Category = "clickFunction")
		void handleRegisterClickedEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "clickFunction")
		void OnLoginRequestComplete();
	UPROPERTY(EditAnywhere)
		bool searchLocalServer;
};
