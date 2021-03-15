// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EventDelegates.generated.h"

/**
 * 
 */
UCLASS()
class SUPPERAPP_API UEventDelegates : public UObject
{
	GENERATED_BODY()
    
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(LoginResultDelegate, const FString&);

	static LoginResultDelegate OnLoginResultDelegate;

	
};
