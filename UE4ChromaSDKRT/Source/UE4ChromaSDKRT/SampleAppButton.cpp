// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "SampleAppButton.h" //___HACK_UE4_VERSION_4_16_OR_GREATER
#include "UE4ChromaSDKRT.h"
//#include "SampleAppButton.h" //___HACK_UE4_VERSION_4_15_OR_LESS
#include "ChromaSDKPluginBPLibrary.h"
#include "SampleAppChromaBP.h"

using namespace std;

mutex USampleAppButton::_sMutex;

//USampleAppButton::USampleAppButton(const class FPostConstructInitializeProperties& PCIP) //___HACK_UE4_VERSION_4_8_OR_LESS
//	: Super(PCIP) //___HACK_UE4_VERSION_4_8_OR_LESS
USampleAppButton::USampleAppButton(const FObjectInitializer& ObjectInitializer) //___HACK_UE4_VERSION_4_9_OR_GREATER
	: Super(ObjectInitializer) //___HACK_UE4_VERSION_4_9_OR_GREATER
{
}

void USampleAppButton::HandleClick()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	if (!UChromaSDKPluginBPLibrary::IsInitialized())
	{
		UE_LOG(LogTemp, Error, TEXT("Chroma is not initialized!"));
		return;
	}

	lock_guard<mutex> guard(_sMutex);

	if (Name.Compare("Button_Effect1") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect1();
		USampleAppChromaBP::SampleAppShowEffect1ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect1Headset();
		USampleAppChromaBP::SampleAppShowEffect1Mousepad();
		USampleAppChromaBP::SampleAppShowEffect1Mouse();
	}
	else if (Name.Compare("Button_Effect2") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect2();
		USampleAppChromaBP::SampleAppShowEffect2ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect2Headset();
		USampleAppChromaBP::SampleAppShowEffect2Mousepad();
		USampleAppChromaBP::SampleAppShowEffect2Mouse();
	}
	else if (Name.Compare("Button_Effect3") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect3();
		USampleAppChromaBP::SampleAppShowEffect3ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect3Headset();
		USampleAppChromaBP::SampleAppShowEffect3Mousepad();
		USampleAppChromaBP::SampleAppShowEffect3Mouse();
	}
	else if (Name.Compare("Button_Effect4") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect4();
		USampleAppChromaBP::SampleAppShowEffect4ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect4Headset();
		USampleAppChromaBP::SampleAppShowEffect4Mousepad();
		USampleAppChromaBP::SampleAppShowEffect4Mouse();
	}
	else if (Name.Compare("Button_Effect5") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect5();
		USampleAppChromaBP::SampleAppShowEffect5ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect5Headset();
		USampleAppChromaBP::SampleAppShowEffect5Mousepad();
		USampleAppChromaBP::SampleAppShowEffect5Mouse();
	}
	else if (Name.Compare("Button_Effect6") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect6();
		USampleAppChromaBP::SampleAppShowEffect6ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect6Headset();
		USampleAppChromaBP::SampleAppShowEffect6Mousepad();
		USampleAppChromaBP::SampleAppShowEffect6Mouse();
	}
	else if (Name.Compare("Button_Effect7") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect7();
		USampleAppChromaBP::SampleAppShowEffect7ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect7Headset();
		USampleAppChromaBP::SampleAppShowEffect7Mousepad();
		USampleAppChromaBP::SampleAppShowEffect7Mouse();
	}
	else if (Name.Compare("Button_Effect8") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect8();
		USampleAppChromaBP::SampleAppShowEffect8ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect8Headset();
		USampleAppChromaBP::SampleAppShowEffect8Mousepad();
		USampleAppChromaBP::SampleAppShowEffect8Mouse();
	}
	else if (Name.Compare("Button_Effect9") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect9();
		USampleAppChromaBP::SampleAppShowEffect9ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect9Headset();
		USampleAppChromaBP::SampleAppShowEffect9Mousepad();
		USampleAppChromaBP::SampleAppShowEffect9Mouse();
	}
	else if (Name.Compare("Button_Effect10") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect10();
		USampleAppChromaBP::SampleAppShowEffect10ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect10Headset();
		USampleAppChromaBP::SampleAppShowEffect10Mousepad();
		USampleAppChromaBP::SampleAppShowEffect10Mouse();
	}
	else if (Name.Compare("Button_Effect11") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect11();
		USampleAppChromaBP::SampleAppShowEffect11ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect11Headset();
		USampleAppChromaBP::SampleAppShowEffect11Mousepad();
		USampleAppChromaBP::SampleAppShowEffect11Mouse();
	}
	else if (Name.Compare("Button_Effect12") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect12();
		USampleAppChromaBP::SampleAppShowEffect12ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect12Headset();
		USampleAppChromaBP::SampleAppShowEffect12Mousepad();
		USampleAppChromaBP::SampleAppShowEffect12Mouse();
	}
	else if (Name.Compare("Button_Effect13") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect13();
		USampleAppChromaBP::SampleAppShowEffect13ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect13Headset();
		USampleAppChromaBP::SampleAppShowEffect13Mousepad();
		USampleAppChromaBP::SampleAppShowEffect13Mouse();
	}
	else if (Name.Compare("Button_Effect14") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect14();
		USampleAppChromaBP::SampleAppShowEffect14ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect14Headset();
		USampleAppChromaBP::SampleAppShowEffect14Mousepad();
		USampleAppChromaBP::SampleAppShowEffect14Mouse();
	}
	else if (Name.Compare("Button_Effect15") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect15();
		USampleAppChromaBP::SampleAppShowEffect15ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect15Headset();
		USampleAppChromaBP::SampleAppShowEffect15Mousepad();
		USampleAppChromaBP::SampleAppShowEffect15Mouse();
	}
	else if (Name.Compare("Button_Effect16") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect16();
		USampleAppChromaBP::SampleAppShowEffect16ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect16Headset();
		USampleAppChromaBP::SampleAppShowEffect16Mousepad();
		USampleAppChromaBP::SampleAppShowEffect16Mouse();
	}
	else if (Name.Compare("Button_Effect17") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect17();
		USampleAppChromaBP::SampleAppShowEffect17ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect17Headset();
		USampleAppChromaBP::SampleAppShowEffect17Mousepad();
		USampleAppChromaBP::SampleAppShowEffect17Mouse();
	}
	else if (Name.Compare("Button_Effect18") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect18();
		USampleAppChromaBP::SampleAppShowEffect18ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect18Headset();
		USampleAppChromaBP::SampleAppShowEffect18Mousepad();
		USampleAppChromaBP::SampleAppShowEffect18Mouse();
	}
	else if (Name.Compare("Button_Effect19") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect19();
		USampleAppChromaBP::SampleAppShowEffect19ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect19Headset();
		USampleAppChromaBP::SampleAppShowEffect19Mousepad();
		USampleAppChromaBP::SampleAppShowEffect19Mouse();
	}
	else if (Name.Compare("Button_Effect20") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect20();
		USampleAppChromaBP::SampleAppShowEffect20ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect20Headset();
		USampleAppChromaBP::SampleAppShowEffect20Mousepad();
		USampleAppChromaBP::SampleAppShowEffect20Mouse();
	}
	else if (Name.Compare("Button_Effect21") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect21();
		USampleAppChromaBP::SampleAppShowEffect21ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect21Headset();
		USampleAppChromaBP::SampleAppShowEffect21Mousepad();
		USampleAppChromaBP::SampleAppShowEffect21Mouse();
	}
	else if (Name.Compare("Button_Effect22") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect22();
		USampleAppChromaBP::SampleAppShowEffect22ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect22Headset();
		USampleAppChromaBP::SampleAppShowEffect22Mousepad();
		USampleAppChromaBP::SampleAppShowEffect22Mouse();
	}
	else if (Name.Compare("Button_Effect23") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect23();
		USampleAppChromaBP::SampleAppShowEffect23ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect23Headset();
		USampleAppChromaBP::SampleAppShowEffect23Mousepad();
		USampleAppChromaBP::SampleAppShowEffect23Mouse();
	}
	else if (Name.Compare("Button_Effect24") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect24();
		USampleAppChromaBP::SampleAppShowEffect24ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect24Headset();
		USampleAppChromaBP::SampleAppShowEffect24Mousepad();
		USampleAppChromaBP::SampleAppShowEffect24Mouse();
	}
	else if (Name.Compare("Button_Effect25") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect25();
		USampleAppChromaBP::SampleAppShowEffect25ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect25Headset();
		USampleAppChromaBP::SampleAppShowEffect25Mousepad();
		USampleAppChromaBP::SampleAppShowEffect25Mouse();
	}
	else if (Name.Compare("Button_Effect26") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect26();
		USampleAppChromaBP::SampleAppShowEffect26ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect26Headset();
		USampleAppChromaBP::SampleAppShowEffect26Mousepad();
		USampleAppChromaBP::SampleAppShowEffect26Mouse();
	}
	else if (Name.Compare("Button_Effect27") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect27();
		USampleAppChromaBP::SampleAppShowEffect27ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect27Headset();
		USampleAppChromaBP::SampleAppShowEffect27Mousepad();
		USampleAppChromaBP::SampleAppShowEffect27Mouse();
	}
	else if (Name.Compare("Button_Effect28") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect28();
		USampleAppChromaBP::SampleAppShowEffect28ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect28Headset();
		USampleAppChromaBP::SampleAppShowEffect28Mousepad();
		USampleAppChromaBP::SampleAppShowEffect28Mouse();
	}
	else if (Name.Compare("Button_Effect29") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect29();
		USampleAppChromaBP::SampleAppShowEffect29ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect29Headset();
		USampleAppChromaBP::SampleAppShowEffect29Mousepad();
		USampleAppChromaBP::SampleAppShowEffect29Mouse();
	}
	else if (Name.Compare("Button_Effect30") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect30();
		USampleAppChromaBP::SampleAppShowEffect30ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect30Headset();
		USampleAppChromaBP::SampleAppShowEffect30Mousepad();
		USampleAppChromaBP::SampleAppShowEffect30Mouse();
	}
	else if (Name.Compare("Button_Effect31") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect31();
		USampleAppChromaBP::SampleAppShowEffect31ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect31Headset();
		USampleAppChromaBP::SampleAppShowEffect31Mousepad();
		USampleAppChromaBP::SampleAppShowEffect31Mouse();
	}
	else if (Name.Compare("Button_Effect32") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect32();
		USampleAppChromaBP::SampleAppShowEffect32ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect32Headset();
		USampleAppChromaBP::SampleAppShowEffect32Mousepad();
		USampleAppChromaBP::SampleAppShowEffect32Mouse();
	}
	else if (Name.Compare("Button_Effect33") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect33();
		USampleAppChromaBP::SampleAppShowEffect33ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect33Headset();
		USampleAppChromaBP::SampleAppShowEffect33Mousepad();
		USampleAppChromaBP::SampleAppShowEffect33Mouse();
	}
	else if (Name.Compare("Button_Effect34") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect34();
		USampleAppChromaBP::SampleAppShowEffect34ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect34Headset();
		USampleAppChromaBP::SampleAppShowEffect34Mousepad();
		USampleAppChromaBP::SampleAppShowEffect34Mouse();
	}
	else if (Name.Compare("Button_Effect35") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect35();
		USampleAppChromaBP::SampleAppShowEffect35ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect35Headset();
		USampleAppChromaBP::SampleAppShowEffect35Mousepad();
		USampleAppChromaBP::SampleAppShowEffect35Mouse();
	}
	else if (Name.Compare("Button_Effect36") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect36();
		USampleAppChromaBP::SampleAppShowEffect36ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect36Headset();
		USampleAppChromaBP::SampleAppShowEffect36Mousepad();
		USampleAppChromaBP::SampleAppShowEffect36Mouse();
	}
	else if (Name.Compare("Button_Effect37") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect37();
		USampleAppChromaBP::SampleAppShowEffect37ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect37Headset();
		USampleAppChromaBP::SampleAppShowEffect37Mousepad();
		USampleAppChromaBP::SampleAppShowEffect37Mouse();
	}
	else if (Name.Compare("Button_Effect38") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect38();
		USampleAppChromaBP::SampleAppShowEffect38ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect38Headset();
		USampleAppChromaBP::SampleAppShowEffect38Mousepad();
		USampleAppChromaBP::SampleAppShowEffect38Mouse();
	}
	else if (Name.Compare("Button_Effect39") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect39();
		USampleAppChromaBP::SampleAppShowEffect39ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect39Headset();
		USampleAppChromaBP::SampleAppShowEffect39Mousepad();
		USampleAppChromaBP::SampleAppShowEffect39Mouse();
	}
	else if (Name.Compare("Button_Effect40") == 0)
	{
		USampleAppChromaBP::SampleAppShowEffect40();
		USampleAppChromaBP::SampleAppShowEffect40ChromaLink();
		USampleAppChromaBP::SampleAppShowEffect40Headset();
		USampleAppChromaBP::SampleAppShowEffect40Mousepad();
		USampleAppChromaBP::SampleAppShowEffect40Mouse();
	}
#endif
}
