// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

//expose HMODULE
#include "RzChromaSDKDefines.h"
#include "RzChromaSDKTypes.h"
#include "RzErrors.h"
#include <map>
#include <string>
#include "ChromaSDKPluginTypes.h"

DECLARE_LOG_CATEGORY_EXTERN(LogChromaPlugin, Log, All);

typedef RZRESULT(*CHROMA_SDK_INIT)(void);
typedef RZRESULT(*CHROMA_SDK_INIT_SDK)(ChromaSDK::APPINFOTYPE* appInfo);
typedef RZRESULT(*CHROMA_SDK_UNINIT)(void);
typedef RZRESULT(*CHROMA_SDK_CREATE_EFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_CHROMA_LINK_EFFECT)(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_KEYBOARD_EFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_KEYPAD_EFFECT)(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_HEADSET_EFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_MOUSE_EFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_CREATE_MOUSEPAD_EFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CHROMA_SDK_SET_EFFECT)(RZEFFECTID EffectId);
typedef RZRESULT(*CHROMA_SDK_DELETE_EFFECT)(RZEFFECTID EffectId);

namespace ChromaSDK
{
	class AnimationBase;
}

#endif

/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules 
 * within this plugin.
 */
class IChromaSDKPlugin : public IModuleInterface
{

public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IChromaSDKPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked< IChromaSDKPlugin >( "ChromaSDKPlugin" );
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "ChromaSDKPlugin" );
	}

	static IChromaSDKPlugin* GetChromaSDKPlugin();
    
    static int GetRGB(const int red, const int green, const int blue);
    static int ToBGR(const FLinearColor& color);
    static FLinearColor ToLinearColor(const int color);
    
protected:
	static int min(const int a, const int b);
	static int max(const int a, const int b);

public:
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	// SDK Methods
	RZRESULT ChromaSDKInit();
	RZRESULT ChromaSDKInitSDK(ChromaSDK::APPINFOTYPE* appInfo);
	RZRESULT ChromaSDKUnInit();
	bool IsInitialized();
	RZRESULT ChromaSDKCreateEffect(RZDEVICEID deviceId, ChromaSDK::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateChromaLinkEffect(ChromaSDK::ChromaLink::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateHeadsetEffect(ChromaSDK::Headset::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateKeyboardEffect(ChromaSDK::Keyboard::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateKeypadEffect(ChromaSDK::Keypad::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateMouseEffect(ChromaSDK::Mouse::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKCreateMousepadEffect(ChromaSDK::Mousepad::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId);
	RZRESULT ChromaSDKSetEffect(RZEFFECTID effectId);
	RZRESULT ChromaSDKDeleteEffect(RZEFFECTID effectId);

	static int GetMaxLeds(EChromaSDKDevice1DEnum::Type device);
	static int GetMaxRow(EChromaSDKDevice2DEnum::Type device);
	static int GetMaxColumn(EChromaSDKDevice2DEnum::Type device);
	
	void CloseAll();
	int CloseAnimation(const int animationId);
	int CloseAnimationName(const char* path);
	int GetAnimation(const char* path);
	int GetAnimationIdFromInstance(ChromaSDK::AnimationBase* animation);
	ChromaSDK::AnimationBase* GetAnimationInstance(const int animationId);
	ChromaSDK::AnimationBase* GetAnimationInstanceName(const char* path);
	ChromaSDK::AnimationBase* GetAnimationInstanceIfOpenName(const char* path);
	const char* GetAnimationName(const int animationId);
	int GetAnimationCount();
	int GetAnimationId(const int index);
	int GetPlayingAnimationCount();
	int GetPlayingAnimationId(const int index);
	void PlayAnimation(const int animationId, const bool loop);
	void PlayAnimationName(const char* path, const bool loop);
	void StopAnimation(const int animationId);
	void StopAnimationName(const char* path);
	void StopAnimationType(const int deviceType, const int device);
	bool IsAnimationPlaying(const int animationId);
	bool IsAnimationPlayingName(const char* path);
	bool IsAnimationPlayingType(const int deviceType, const int device);
	int GetAnimationFrameCount(const int animationId);
	int GetAnimationFrameCountName(const char* path);

	// CREATE ANIMATION IN MEMORY
	int CreateAnimationInMemory(const int deviceType, const int device);

	// LERP
	float Lerp(const float start, const float end, const float amt);

	// LERP COLOR
	int LerpColor(const int from, const int to, const float t);


	// COPY ANIMATION

	int CopyAnimation(const int sourceAnimationId, const char* targetAnimation);
	void CopyAnimationName(const char* sourceAnimation, const char* targetAnimation);


	// APPEND ALL FRAMES

	void AppendAllFrames(const int sourceAnimationId, const int targetAnimationId);
	void AppendAllFramesName(const char* sourceAnimation, const char* targetAnimation);


	// INVERT ALL COLORS

	void InvertColors(const int animationId, const int frameId);
	void InvertColorsName(const char* path, const int frameId);

	// ALL FRAMES

	void InvertColorsAllFrames(const int animationId);
	void InvertColorsAllFramesName(const char* path);


	// SET KEY COLOR

	void SetKeyColor(const int animationId, const int frameId, const int rzkey, const COLORREF color);
	void SetKeyColorName(const char* path, const int frameId, const int rzkey, const COLORREF color);

	// NONZERO

	void SetKeyNonZeroColor(const int animationId, const int frameId, const int rzkey, const COLORREF color);
	void SetKeyNonZeroColorName(const char* path, const int frameId, const int rzkey, const COLORREF color);

	// GET KEY COLOR
	COLORREF GetKeyColor(const int animationId, const int frameId, const int rzkey);
	COLORREF GetKeyColorName(const char* path, const int frameId, const int rzkey);

	// COPY KEY COLOR
	void CopyKeyColor(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int rzkey);
	void CopyKeyColorName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int rzkey);

	// NONZERO
	void CopyNonZeroKeyColor(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int rzkey);
	void CopyNonZeroKeyColorName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int rzkey);

	// COPY ALL KEYS
	void CopyAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId);
	void CopyAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId);

	// NONZERO
	void CopyNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId);
	void CopyNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId);

	// NONZERO ADD
	void AddNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId);
	void AddNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId);

	// NONZERO SUBTRACT
	void SubtractNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId);
	void SubtractNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId);

	// NONZERO OFFSET
	void CopyNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset);
	void CopyNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset);

	// NONZERO OFFSET ADD
	void AddNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset);
	void AddNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset);

	// NONZERO OFFSET SUBTRACT
	void SubtractNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset);
	void SubtractNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset);

	// COPY NONZERO TARGET
	void CopyNonZeroTargetAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId);
	void CopyNonZeroTargetAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId);

	// ALL FRAMES
	void CopyNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId);
	void CopyNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation);

	// ALL FRAMES
	void CopyZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId);
	void CopyZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation);

	// ADD NONZERO TARGET
	void AddNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId);
	void AddNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation);

	// SUBTRACT NONZERO TARGET ALL FRAMES
	void SubtractNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId);
	void SubtractNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation);

	// COPY NONZERO TARGET ALL FRAMES
	void CopyNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset);
	void CopyNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset);

	// ADD NONZERO TARGET ALL FRAMES OFFSET
	void AddNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset);
	void AddNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset);

	// SUBTRACT NONZERO TARGET ALL FRAMES OFFSET
	void SubtractNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset);
	void SubtractNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset);

	// MULTIPLY COLOR LERP ALL FRAMES
	void MultiplyColorLerpAllFrames(const int animationId, const int color1, const int color2);
	void MultiplyColorLerpAllFramesName(const char* path, const int color1, const int color2);

	// MULTIPLY TARGET COLOR LERP ALL FRAMES
	void MultiplyTargetColorLerpAllFrames(const int animationId, const int color1, const int color2);
	void MultiplyTargetColorLerpAllFramesName(const char* path, const int color1, const int color2);

	// FILL COLOR
	
	void FillColor(const int animationId, const int frameId, const int color);
	void FillColorName(const char* path, const int frameId, const int color);

	void FillColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue);
	void FillColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue);

	// NONZERO COLOR

	void FillNonZeroColor(const int animationId, const int frameId, const int color);
	void FillNonZeroColorName(const char* path, const int frameId, const int color);

	void FillNonZeroColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue);
	void FillNonZeroColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue);

	
	// ZERO COLOR

	void FillZeroColor(const int animationId, const int frameId, const int color);
	void FillZeroColorName(const char* path, const int frameId, const int color);

	void FillZeroColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue);
	void FillZeroColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue);


	// FILL THRESHOLD COLORS

	void FillThresholdColors(const int animationId, const int frameId, const int threshold, const int color);
	void FillThresholdColorsName(const char* path, const int frameId, const int threshold, const int color);

	void FillThresholdColorsRGB(const int animationId, const int frameId, const int threshold, const int red, const int green, const int blue);
	void FillThresholdColorsRGBName(const char* path, const int frameId, const int threshold, const int red, const int green, const int blue);


	// FILL THRESHOLD COLORS ALL FRAMES
	void FillThresholdColorsAllFrames(const int animationId, const int threshold, const int color);
	void FillThresholdColorsAllFramesName(const char* path, const int threshold, const int color);

	// RGB
	void FillThresholdColorsAllFramesRGB(const int animationId, const int threshold, const int red, const int green, const int blue);
	void FillThresholdColorsAllFramesRGBName(const char* path, const int threshold, const int red, const int green, const int blue);


	// MIN MAX
	void FillThresholdColorsMinMaxRGB(const int animationId, const int frameId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue);
	void FillThresholdColorsMinMaxRGBName(const char* path, const int frameId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue);


	// ALL FRAMES
	void FillThresholdColorsMinMaxAllFramesRGB(const int animationId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue);
	void FillThresholdColorsMinMaxAllFramesRGBName(const char* path, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue);


	// FILL THRESHOLD RGB COLORS RGB
	void FillThresholdRGBColorsRGB(const int animationId, const int frameId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue);
	void FillThresholdRGBColorsRGBName(const char* path, const int frameId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue);


	// FILL THRESHOLD RGB COLORS ALL FRAMES RGB
	void FillThresholdRGBColorsAllFramesRGB(const int animationId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue);
	void FillThresholdRGBColorsAllFramesRGBName(const char* path, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue);


	// FILL COLOR ALL FRAMES

	void FillColorAllFrames(const int animationId, const int color);
	void FillColorAllFramesName(const char* path, const int color);

	void FillColorAllFramesRGB(const int animationId, const int red, const int green, const int blue);
	void FillColorAllFramesRGBName(const char* path, const int red, const int green, const int blue);

	// NONZERO COLOR

	void FillNonZeroColorAllFrames(const int animationId, const int color);
	void FillNonZeroColorAllFramesName(const char* path, const int color);

	void FillNonZeroColorAllFramesRGB(const int animationId, const int red, const int green, const int blue);
	void FillNonZeroColorAllFramesRGBName(const char* path, const int red, const int green, const int blue);

	// ZERO COLOR

	void FillZeroColorAllFrames(const int animationId, const int color);
	void FillZeroColorAllFramesName(const char* path, const int color);

	void FillZeroColorAllFramesRGB(const int animationId, const int red, const int green, const int blue);
	void FillZeroColorAllFramesRGBName(const char* path, const int red, const int green, const int blue);

	// FILL RANDOM COLORS
	void FillRandomColors(const int animationId, const int frameId);
	void FillRandomColorsName(const char* path, const int frameId);

	void FillRandomColorsAllFrames(const int animationId);
	void FillRandomColorsAllFramesName(const char* path);

	// FILL RANDOM BLACK AND WHITE
	void FillRandomColorsBlackAndWhite(const int animationId, const int frameId);
	void FillRandomColorsBlackAndWhiteName(const char* path, const int frameId);

	void FillRandomColorsBlackAndWhiteAllFrames(const int animationId);
	void FillRandomColorsBlackAndWhiteAllFramesName(const char* path);

	// OFFSET COLORS

	void OffsetColors(const int animationId, const int frameId, const int red, const int green, const int blue);
	void OffsetColorsName(const char* path, const int frameId, const int red, const int green, const int blue);

	void OffsetColorsAllFrames(const int animationId, const int red, const int green, const int blue);
	void OffsetColorsAllFramesName(const char* path, const int red, const int green, const int blue);

	void OffsetNonZeroColors(const int animationId, const int frameId, const int red, const int green, const int blue);
	void OffsetNonZeroColorsName(const char* path, const int frameId, const int red, const int green, const int blue);

	void OffsetNonZeroColorsAllFrames(const int animationId, const int red, const int green, const int blue);
	void OffsetNonZeroColorsAllFramesName(const char* path, const int red, const int green, const int blue);


	// MULTIPLY INTENSITY

	void MultiplyIntensity(const int animationId, const int frameId, const float intensity);
	void MultiplyIntensityName(const char* path, const int frameId, const float intensity);

	void MultiplyIntensityColor(const int animationId, const int frameId, const int color);
	void MultiplyIntensityColorName(const char* path, const int frameId, const int color);

	void MultiplyIntensityRGB(const int animationId, const int frameId, const int red, const int green, const int blue);
	void MultiplyIntensityRGBName(const char* path, const int frameId, const int red, const int green, const int blue);

	void MultiplyIntensityAllFrames(const int animationId, const float intensity);
	void MultiplyIntensityAllFramesName(const char* path, const float intensity);

	void MultiplyIntensityColorAllFrames(const int animationId, const int color);
	void MultiplyIntensityColorAllFramesName(const char* path, const int color);

	void MultiplyIntensityAllFramesRGB(const int animationId, const int red, const int green, const int blue);
	void MultiplyIntensityAllFramesRGBName(const char* path, const int red, const int green, const int blue);


	// MULTIPLY NONZERO TARGET COLOR LERP
	void MultiplyTargetColorLerp(const int animationId, const int frameId, const int color1, const int color2);
	void MultiplyNonZeroTargetColorLerp(const int animationId, const int frameId, const int color1, const int color2);
	
	// ALL FRAMES
	void MultiplyNonZeroTargetColorLerpAllFrames(const int animationId, const int color1, const int color2);
	void MultiplyNonZeroTargetColorLerpAllFramesName(const char* path, const int color1, const int color2);


	void LoadAnimation(const int animationId);
	void LoadAnimationName(const char* path);
	void UnloadAnimation(const int animationId);
	void UnloadAnimationName(const char* path);

	void SetChromaCustomFlag(const int animationId, const bool flag);
	void SetChromaCustomFlagName(const char* path, const bool flag);

	void SetChromaCustomColorAllFrames(const int animationId);
	void SetChromaCustomColorAllFramesName(const char* path);

	int PreviewFrame(const int animationId, const int frameId);
	void PreviewFrameName(const char* path, const int frameId);

	int OverrideFrameDuration(const int animationId, const float duration);
	void OverrideFrameDurationName(const char* path, const float duration);

	// MAKE FRAMES
	void MakeBlankFrames(const int animationId, const int frameCount, const float duration, const int color);
	void MakeBlankFramesName(const char* path, const int frameCount, const float duration, const int color);

	void MakeBlankFramesRGB(const int animationId, const int frameCount, const float duration, const int red, const int green, const int blue);
	void MakeBlankFramesRGBName(const char* path, const int frameCount, const float duration, const int red, const int green, const int blue);

	// RANDOM
	void MakeBlankFramesRandom(const int animationId, const int frameCount, const float duration);
	void MakeBlankFramesRandomName(const char* path, const int frameCount, const float duration);

	// RANDOM BLACK AND WHITE
	void MakeBlankFramesRandomBlackAndWhite(const int animationId, const int frameCount, const float duration);
	void MakeBlankFramesRandomBlackAndWhiteName(const char* path, const int frameCount, const float duration);

	// REVERSE ALL FRAMES
	void ReverseAllFrames(const int animationId);
	void ReverseAllFramesName(const char* path);

	// DUPLICATE FRAMES
	void DuplicateFrames(const int animationId);
	void DuplicateFramesName(const char* path);

	// DUPLICATE FIRST FRAME
	void DuplicateFirstFrame(const int animationId, const int frameCount);
	void DuplicateFirstFrameName(const char* path, const int frameCount);

	// DUPLICATE MIRROR FRAMES
	void DuplicateMirrorFrames(const int animationId);
	void DuplicateMirrorFramesName(const char* path);

	// INSERT FRAME
	void InsertFrame(const int animationId, const int sourceFrame, const int targetFrame);
	void InsertFrameName(const char* path, const int sourceFrame, const int targetFrame);

	// INSERT DELAY
	void InsertDelay(const int animationId, const int frameId, const int delay);
	void InsertDelayName(const char* path, const int frameId, const int delay);

	// REDUCE FRAMES
	void ReduceFrames(const int animationId, const int n);
	void ReduceFramesName(const char* path, const int n);

	// TRIM FRAME
	void TrimFrame(const int animationId, const int frameId);
	void TrimFrameName(const char* path, const int frameId);

	// TRIM START FRAMES
	void TrimStartFrames(const int animationId, const int numberOfFrames);
	void TrimStartFramesName(const char* path, const int numberOfFrames);

	// TRIM END FRAMES
	void TrimEndFrames(const int animationId, const int lastFrameId);
	void TrimEndFramesName(const char* path, const int lastFrameId);

	// FADE START FRAMES
	void FadeStartFrames(const int animationId, const int fade);
	void FadeStartFramesName(const char* path, const int fade);

	// FADE END FRAMES
	void FadeEndFrames(const int animationId, const int fade);
	void FadeEndFramesName(const char* path, const int fade);

	// OPEN ANIMATION

	int OpenAnimation(const char* path);
	ChromaSDK::AnimationBase* OpenAnimationFromMemory(const unsigned char* data);
	int OpenAnimationFromMemory(const unsigned char* data, const char* name);

	// IDLE ANIMATION

	void UseIdleAnimation(EChromaSDKDeviceEnum::Type device, const bool flag);
	void UseIdleAnimations(const bool flag);
	void SetIdleAnimationName(const char* path);

	/*
		Set preloading animation flag, which is set to true by default. Reference
		animation by id.
	*/
	void UsePreloading(const int animationId, const bool flag);
	/*
		Set preloading animation flag, which is set to true by default. Reference
		animation by name.
	*/
	void UsePreloadingName(const char* path, const bool flag);

	int GetCurrentFrame(const int animationId);
	int GetCurrentFrameName(const char* path);

	void SetCurrentFrame(const int animationId, const int frameId);
	void SetCurrentFrameName(const char* path, const int frameId);

protected:

	bool ValidateGetProcAddress(bool condition, FString methodName);

	static IChromaSDKPlugin* _sInstance;

	bool _mInitialized;

	HMODULE _mLibraryChroma;
	static bool _sLibraryMissing;
	static bool _sInvalidSignature;

	CHROMA_SDK_INIT _mMethodInit;
	CHROMA_SDK_INIT_SDK _mMethodInitSDK;
	CHROMA_SDK_UNINIT _mMethodUnInit;
	CHROMA_SDK_CREATE_EFFECT _mMethodCreateEffect;
	CHROMA_SDK_CREATE_CHROMA_LINK_EFFECT _mMethodCreateChromaLinkEffect;
	CHROMA_SDK_CREATE_HEADSET_EFFECT _mMethodCreateHeadsetEffect;
	CHROMA_SDK_CREATE_KEYBOARD_EFFECT _mMethodCreateKeyboardEffect;
	CHROMA_SDK_CREATE_KEYPAD_EFFECT _mMethodCreateKeypadEffect;
	CHROMA_SDK_CREATE_MOUSE_EFFECT _mMethodCreateMouseEffect;
	CHROMA_SDK_CREATE_MOUSEPAD_EFFECT _mMethodCreateMousepadEffect;
	CHROMA_SDK_SET_EFFECT _mMethodSetEffect;
	CHROMA_SDK_DELETE_EFFECT _mMethodDeleteEffect;

	int _mAnimationId;
	std::map<std::string, int> _mAnimationMapID;
	std::map<int, ChromaSDK::AnimationBase*> _mAnimations;
	std::map<EChromaSDKDevice1DEnum::Type, int> _mPlayMap1D;
	std::map<EChromaSDKDevice2DEnum::Type, int> _mPlayMap2D;
#endif
};
