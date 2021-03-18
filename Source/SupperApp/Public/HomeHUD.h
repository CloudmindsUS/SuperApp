// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "HomeHUD.generated.h"

/**
 * 
 */
UCLASS()
class SUPPERAPP_API AHomeHUD : public AHUD
{
	GENERATED_BODY()

private:
	bool bAdded = false;

public:

	virtual void DrawHUD() override;

	void OnLoginResult(const FString &url);
	void OnRedirectCheckMe();

	UFUNCTION(BlueprintImplementableEvent, Category = "homehud")
		void OnLoginSuccessful();

	UFUNCTION(BlueprintImplementableEvent, Category = "homehud")
		void ApplyLoginSuccessfulWithUrl(const FString& url);

	UFUNCTION(BlueprintImplementableEvent, Category = "homehud")
		void ApplyRedirectCheckMe();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="homehud")
		class UUserWidget* HomeWidget;
};
