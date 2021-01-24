// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SampleGameLoopChromaBP.h" //___HACK_UE4_VERSION_4_16_OR_GREATER
#include "UE4ChromaSDKRT.h"
//#include "SampleGameLoopChromaBP.h" //___HACK_UE4_VERSION_4_15_OR_LESS
#include "ChromaSDKPluginBPLibrary.h"

//USampleGameLoopChromaBP::USampleGameLoopChromaBP(const FPostConstructInitializeProperties& PCIP) //___HACK_UE4_VERSION_4_8_OR_LESS
//	: Super(PCIP) //___HACK_UE4_VERSION_4_8_OR_LESS
USampleGameLoopChromaBP::USampleGameLoopChromaBP(const FObjectInitializer& ObjectInitializer) //___HACK_UE4_VERSION_4_9_OR_GREATER
	: Super(ObjectInitializer) //___HACK_UE4_VERSION_4_9_OR_GREATER
{
}

void USampleGameLoopChromaBP::SampleGameLoopSampleStart()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	if (!UChromaSDKPluginBPLibrary::IsInitialized())
	{
		FChromaSDKAppInfoType appInfo;
		appInfo.Title = "UE4 Chroma Sample Game Loop Application";
		appInfo.Description = "A sample application using Razer Chroma SDK";
		appInfo.Author_Name = "Razer";
		appInfo.Author_Contact = "https://github.com/razerofficial/UE4_XDK_SampleApp";

		//appInfo.SupportedDevice = 
		//    0x01 | // Keyboards
		//    0x02 | // Mice
		//    0x04 | // Headset
		//    0x08 | // Mousepads
		//    0x10 | // Keypads
		//    0x20   // ChromaLink devices
		//    ;
		appInfo.SupportedDevice = (0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);
		appInfo.Category = 1;

		int32 result = UChromaSDKPluginBPLibrary::ChromaSDKInitSDK(appInfo);
		switch (result)
		{
		case RZRESULT_DLL_NOT_FOUND:
			UE_LOG(LogTemp, Error, TEXT("Chroma DLL is not found!"));
			break;
		case RZRESULT_DLL_INVALID_SIGNATURE:
			UE_LOG(LogTemp, Error, TEXT("Chroma DLL has an invalid signature!"));
			break;
		case RZRESULT_SUCCESS:
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize Chroma!"));
			break;
		}
	}
#endif
}

void USampleGameLoopChromaBP::SampleGameLoopSampleEnd()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	if (UChromaSDKPluginBPLibrary::IsInitialized())
	{
		UChromaSDKPluginBPLibrary::ChromaSDKUnInit();
	}
#endif
}

void USampleGameLoopChromaBP::SampleGameLoopUpdate(float deltaSeconds, FChromaSDKScene& scene, bool toggleHotkeys, bool toggleAmmo)
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	if (UChromaSDKPluginBPLibrary::IsInitialized())
	{
		UChromaSDKPluginBPLibrary::ChromaSDKUnInit();
	}
#endif
}
