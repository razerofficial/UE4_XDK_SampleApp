// Copyright 1998-2014 Epic SampleApps, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "UMG.h"
#include "SampleAppButton.h"
#include "SampleAppChromaBP.generated.h"

UCLASS()
class USampleAppChromaBP : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

    static int min(int a, int b);
	static int max(int a, int b);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppSetupButtonsEffects", Keywords = "Setup an array of button widgets"), Category = "Sample")
	static void SampleAppSetupButtonsEffects(const TArray<UButton*>& buttons);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppSampleStart", Keywords = "Init at the start of the application"), Category = "Sample")
	static void SampleAppSampleStart();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppSampleEnd", Keywords = "Uninit at the end of the application"), Category = "Sample")
	static void SampleAppSampleEnd();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect1", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect1();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect1ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect1ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect1Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect1Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect1Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect1Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect1Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect1Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect2", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect2();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect2ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect2ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect2Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect2Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect2Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect2Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect2Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect2Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect3", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect3();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect3ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect3ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect3Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect3Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect3Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect3Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect3Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect3Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect4", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect4();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect4ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect4ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect4Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect4Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect4Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect4Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect4Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect4Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect5", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect5();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect5ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect5ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect5Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect5Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect5Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect5Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect5Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect5Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect6", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect6();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect6ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect6ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect6Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect6Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect6Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect6Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect6Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect6Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect7", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect7();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect7ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect7ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect7Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect7Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect7Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect7Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect7Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect7Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect8", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect8();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect8ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect8ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect8Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect8Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect8Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect8Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect8Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect8Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect9", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect9();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect9ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect9ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect9Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect9Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect9Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect9Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect9Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect9Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect10", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect10();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect10ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect10ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect10Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect10Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect10Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect10Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect10Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect10Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect11", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect11();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect11ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect11ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect11Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect11Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect11Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect11Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect11Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect11Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect12", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect12();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect12ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect12ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect12Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect12Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect12Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect12Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect12Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect12Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect13", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect13();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect13ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect13ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect13Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect13Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect13Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect13Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect13Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect13Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect14", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect14();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect14ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect14ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect14Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect14Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect14Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect14Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect14Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect14Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect15", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect15();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect15ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect15ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect15Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect15Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect15Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect15Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect15Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect15Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect16", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect16();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect16ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect16ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect16Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect16Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect16Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect16Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect16Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect16Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect17", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect17();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect17ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect17ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect17Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect17Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect17Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect17Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect17Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect17Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect18", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect18();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect18ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect18ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect18Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect18Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect18Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect18Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect18Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect18Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect19", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect19();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect19ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect19ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect19Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect19Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect19Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect19Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect19Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect19Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect20", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect20();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect20ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect20ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect20Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect20Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect20Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect20Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect20Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect20Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect21", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect21();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect21ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect21ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect21Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect21Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect21Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect21Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect21Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect21Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect22", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect22();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect22ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect22ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect22Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect22Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect22Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect22Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect22Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect22Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect23", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect23();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect23ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect23ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect23Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect23Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect23Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect23Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect23Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect23Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect24", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect24();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect24ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect24ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect24Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect24Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect24Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect24Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect24Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect24Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect25", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect25();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect25ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect25ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect25Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect25Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect25Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect25Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect25Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect25Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect26", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect26();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect26ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect26ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect26Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect26Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect26Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect26Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect26Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect26Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect27", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect27();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect27ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect27ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect27Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect27Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect27Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect27Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect27Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect27Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect28", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect28();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect28ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect28ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect28Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect28Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect28Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect28Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect28Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect28Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect29", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect29();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect29ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect29ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect29Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect29Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect29Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect29Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect29Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect29Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect30", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect30();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect30ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect30ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect30Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect30Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect30Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect30Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect30Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect30Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect31", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect31();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect31ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect31ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect31Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect31Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect31Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect31Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect31Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect31Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect32", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect32();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect32ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect32ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect32Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect32Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect32Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect32Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect32Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect32Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect33", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect33();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect33ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect33ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect33Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect33Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect33Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect33Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect33Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect33Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect34", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect34();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect34ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect34ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect34Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect34Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect34Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect34Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect34Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect34Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect35", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect35();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect35ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect35ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect35Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect35Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect35Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect35Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect35Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect35Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect36", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect36();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect36ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect36ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect36Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect36Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect36Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect36Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect36Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect36Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect37", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect37();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect37ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect37ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect37Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect37Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect37Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect37Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect37Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect37Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect38", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect38();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect38ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect38ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect38Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect38Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect38Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect38Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect38Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect38Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect39", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect39();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect39ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect39ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect39Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect39Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect39Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect39Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect39Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect39Mouse();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect40", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect40();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect40ChromaLink", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect40ChromaLink();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect40Headset", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect40Headset();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect40Mousepad", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect40Mousepad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SampleAppShowEffect40Mouse", Keywords = "Example"), Category = "Sample")
	static void SampleAppShowEffect40Mouse();

private:
	static TArray<USampleAppButton*> _sSampleButtons;
};
