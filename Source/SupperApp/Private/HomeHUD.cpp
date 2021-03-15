// Fill out your copyright notice in the Description page of Project Settings.


#include "HomeHUD.h"
#include "EventDelegates.h"

void AHomeHUD::DrawHUD() {
	Super::DrawHUD();

	if (!bAdded) {
		UEventDelegates::OnLoginResultDelegate.AddUObject(this, &ThisClass::OnLoginResult);
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
	OnLoginSuccessfulWithUrl(url);
}
