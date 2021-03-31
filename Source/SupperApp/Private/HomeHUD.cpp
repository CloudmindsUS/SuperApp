// Fill out your copyright notice in the Description page of Project Settings.


#include "HomeHUD.h"
#include "EventDelegates.h"

void AHomeHUD::DrawHUD() {
	Super::DrawHUD();

	if (!bAdded) {
		UEventDelegates::OnLoginResultDelegate.AddUObject(this, &ThisClass::OnLoginResult);
		UEventDelegates::OnRedirectCheckMeDelegate.AddUObject(this, &ThisClass::OnRedirectCheckMe);
		// Skip login menu
		UEventDelegates::OnHandleRedirectCheckMeDelegate.Broadcast();
		bAdded = true;
	}
	


}



void AHomeHUD::OnLoginResult(const FString &url) {

	if (HomeWidget != nullptr) {
		HomeWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Home widget is null"));
	}
	//MyWidget->SetVisibility(ESlateVisibility::Visible);
	ApplyLoginSuccessfulWithUrl(url);
}

void AHomeHUD::OnRedirectCheckMe() {
	UE_LOG(LogTemp, Warning, TEXT("OnRedirectCheckMe Called"));
	if (HomeWidget != nullptr) {
		HomeWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Home widget is null"));
	}
	ApplyRedirectCheckMe();
}