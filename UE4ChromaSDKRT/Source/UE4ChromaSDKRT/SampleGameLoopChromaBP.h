// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "ChromaSDKPluginTypes.h"
#include "SampleGameLoopChromaBP.generated.h"

UCLASS()
class USampleGameLoopChromaBP : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleGameLoopSampleStart", Keywords = "Init at the start of the application"), Category = "Sample")
	static void SampleGameLoopSampleStart();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleGameLoopSampleEnd", Keywords = "Uninit at the end of the application"), Category = "Sample")
	static void SampleGameLoopSampleEnd();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleGameLoopUpdate", Keywords = "Example"), Category = "Sample")
	static void SampleGameLoopUpdate(float deltaSeconds, UPARAM(ref) FChromaSDKScene& scene, bool toggleHotkeys, bool toggleAmmo);
};
