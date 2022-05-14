// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "IChromaSDKPlugin.h"
#include "ChromaSDKPluginPrivatePCH.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

#include "Windows/AllowWindowsPlatformTypes.h" 

#include "ChromaAnimationAPI.h"

using namespace ChromaSDK;

DEFINE_LOG_CATEGORY(LogChromaPlugin);

class FChromaSDKPlugin : public IChromaSDKPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FChromaSDKPlugin, ChromaSDKPlugin )


void FChromaSDKPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	ChromaAnimationAPI::InitAPI();
#endif
}


void FChromaSDKPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

	if (ChromaAnimationAPI::GetIsInitializedAPI())
	{
		ChromaAnimationAPI::StopAll();
		ChromaAnimationAPI::CloseAll();
		RZRESULT result = ChromaAnimationAPI::Uninit();
		if (result != RZRESULT_SUCCESS)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("Failed to uninitialize Chroma!"));
		}
		ChromaAnimationAPI::UninitAPI();
	}
	
#endif
}

#pragma region Color Operations

int IChromaSDKPlugin::min(const int a, const int b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}
int IChromaSDKPlugin::max(const int a, const int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

int IChromaSDKPlugin::GetRGB(const int red, const int green, const int blue)
{
	return (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
}

int IChromaSDKPlugin::ToBGR(const FLinearColor& color)
{
	int red = max(0, min(255, color.R * 255));
	int green = max(0, min(255, color.G * 255));
	int blue = max(0, min(255, color.B * 255));
	int customFlag = max(0, min(255, color.A * 255));
	int bgrColor = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16) | (customFlag << 24);
	return bgrColor;
}

FLinearColor IChromaSDKPlugin::ToLinearColor(const int color)
{
	float red = (color & 0xFF) / 255.0f;
	float green = ((color & 0xFF00) >> 8) / 255.0f;
	float blue = ((color & 0xFF0000) >> 16) / 255.0f;
	float alpha = ((color & 0xFF000000) >> 24) / 255.0f;
	return FLinearColor(red, green, blue, alpha);
}

#pragma endregion


#include "Windows/HideWindowsPlatformTypes.h"

#endif
