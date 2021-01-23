// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include "SampleGameButton.generated.h"

UCLASS()
class USampleGameButton : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FString Name;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "HandleClick", Keywords = "Dynamic function to handle button widget clicks"), Category = "Sample")
	void HandleClick();

private:
	static std::mutex _sMutex;
};
