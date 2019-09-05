// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ChromaSDKPluginPrivatePCH.h"
#include "AnimationBase.h"
#include "Animation1D.h"
#include "Animation2D.h"
#include "ChromaSDKPluginBPLibrary.h"
#include "ChromaThread.h"
#include "VerifyLibrarySignature.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

#include "Windows/AllowWindowsPlatformTypes.h" 

typedef unsigned char byte;
#define ANIMATION_VERSION 1

using namespace ChromaSDK;
using namespace ChromaSDK::ChromaLink;
using namespace ChromaSDK::Headset;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Keypad;
using namespace ChromaSDK::Mouse;
using namespace ChromaSDK::Mousepad;
using namespace std;

DEFINE_LOG_CATEGORY(LogChromaPlugin);

#if PLATFORM_XBOXONE
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#elif PLATFORM_WINDOWS
#if _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif
#endif

#endif


#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
bool IChromaSDKPlugin::_sLibraryMissing = false;
bool IChromaSDKPlugin::_sInvalidSignature = false;
#endif

int g_seed = 0;
inline int fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}

class FChromaSDKPlugin : public IChromaSDKPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FChromaSDKPlugin, ChromaSDKPlugin )


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


void FChromaSDKPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	_mInitialized = false;
	_mAnimationId = 0;
	_mAnimationMapID.clear();
	_mAnimations.clear();
	_mPlayMap1D.clear();
	_mPlayMap2D.clear();

	_mMethodInit = nullptr;
	_mMethodUnInit = nullptr;
	_mMethodCreateEffect = nullptr;
	_mMethodCreateChromaLinkEffect = nullptr;
	_mMethodCreateHeadsetEffect = nullptr;
	_mMethodCreateKeyboardEffect = nullptr;
	_mMethodCreateKeypadEffect = nullptr;
	_mMethodCreateMouseEffect = nullptr;
	_mMethodCreateMousepadEffect = nullptr;
	_mMethodSetEffect = nullptr;
	_mMethodDeleteEffect = nullptr;

	// abort load if an invalid signature was detected
	if (_sInvalidSignature)
	{
		return;
	}

	if (_sLibraryMissing)
	{
		return;
	}

#if PLATFORM_XBOXONE
	_mLibraryChroma = LoadLibrary(*FPaths::Combine(*FPaths::LaunchDir(), CHROMASDKDLL));
#else
	_mLibraryChroma = LoadLibrary(CHROMASDKDLL);
#endif
	if (_mLibraryChroma == NULL)
	{
		UE_LOG(LogChromaPlugin, Warning, TEXT("ChromaSDKPlugin failed to load!"));
		_sLibraryMissing = true;
		return;
	}
	//UE_LOG(LogChromaPlugin, Log, TEXT("ChromaSDKPlugin loaded."));

#if PLATFORM_WINDOWS
	// verify the library has a valid signature
	_sInvalidSignature = !ChromaSDK::VerifyLibrarySignature::VerifyModule(_mLibraryChroma);
	if (_sInvalidSignature)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("Failed to load Chroma library with invalid signature!"));

		// unload the library
		FreeLibrary(_mLibraryChroma);
		_mLibraryChroma = NULL;

		return;
	}
#endif

	// GetProcAddress will throw 4191 because it's an unsafe type cast
#pragma warning(disable: 4191)
	_mMethodInit = (CHROMA_SDK_INIT)GetProcAddress(_mLibraryChroma, "Init");
	if (ValidateGetProcAddress(_mMethodInit == nullptr, FString("Init")))
	{
		return;
	}
	_mMethodUnInit = (CHROMA_SDK_UNINIT)GetProcAddress(_mLibraryChroma, "UnInit");
	if (ValidateGetProcAddress(_mMethodUnInit == nullptr, FString("UnInit")))
	{
		return;
	}

	_mMethodCreateChromaLinkEffect = (CHROMA_SDK_CREATE_CHROMA_LINK_EFFECT)GetProcAddress(_mLibraryChroma, "CreateChromaLinkEffect");
	if (ValidateGetProcAddress(_mMethodCreateChromaLinkEffect == nullptr, FString("CreateChromaLinkEffect")))
	{
		return;
	}
	_mMethodCreateHeadsetEffect = (CHROMA_SDK_CREATE_HEADSET_EFFECT)GetProcAddress(_mLibraryChroma, "CreateHeadsetEffect");
	if (ValidateGetProcAddress(_mMethodCreateHeadsetEffect == nullptr, FString("CreateHeadsetEffect")))
	{
		return;
	}
	_mMethodCreateKeyboardEffect = (CHROMA_SDK_CREATE_KEYBOARD_EFFECT)GetProcAddress(_mLibraryChroma, "CreateKeyboardEffect");
	if (ValidateGetProcAddress(_mMethodCreateKeyboardEffect == nullptr, FString("CreateKeyboardEffect")))
	{
		return;
	}
	_mMethodCreateMouseEffect = (CHROMA_SDK_CREATE_MOUSE_EFFECT)GetProcAddress(_mLibraryChroma, "CreateMouseEffect");
	if (ValidateGetProcAddress(_mMethodCreateMouseEffect == nullptr, FString("CreateMouseEffect")))
	{
		return;
	}
	_mMethodCreateMousepadEffect = (CHROMA_SDK_CREATE_MOUSEPAD_EFFECT)GetProcAddress(_mLibraryChroma, "CreateMousepadEffect");
	if (ValidateGetProcAddress(_mMethodCreateMousepadEffect == nullptr, FString("CreateMousepadEffect")))
	{
		return;
	}
	_mMethodCreateKeypadEffect = (CHROMA_SDK_CREATE_KEYPAD_EFFECT)GetProcAddress(_mLibraryChroma, "CreateKeypadEffect");
	if (ValidateGetProcAddress(_mMethodCreateKeypadEffect == nullptr, FString("CreateKeypadEffect")))
	{
		return;
	}

	_mMethodCreateEffect = (CHROMA_SDK_CREATE_EFFECT)GetProcAddress(_mLibraryChroma, "CreateEffect");
	if (ValidateGetProcAddress(_mMethodCreateEffect == nullptr, FString("CreateEffect")))
	{
		return;
	}
	_mMethodSetEffect = (CHROMA_SDK_SET_EFFECT)GetProcAddress(_mLibraryChroma, "SetEffect");
	if (ValidateGetProcAddress(_mMethodSetEffect == nullptr, FString("SetEffect")))
	{
		return;
	}
	_mMethodDeleteEffect = (CHROMA_SDK_DELETE_EFFECT)GetProcAddress(_mLibraryChroma, "DeleteEffect");
	if (ValidateGetProcAddress(_mMethodDeleteEffect == nullptr, FString("DeleteEffect")))
	{
		return;
	}

#pragma warning(default: 4191)

	if (!UChromaSDKPluginBPLibrary::IsInitialized())
	{
		UChromaSDKPluginBPLibrary::ChromaSDKInit();
	}
#endif
}


void FChromaSDKPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

	if (UChromaSDKPluginBPLibrary::IsInitialized())
	{
		UChromaSDKPluginBPLibrary::ChromaSDKUnInit();
	}

	if (_mLibraryChroma)
	{
		FreeLibrary(_mLibraryChroma);
		_mLibraryChroma = nullptr;
	}

	_mMethodInit = nullptr;
	_mMethodUnInit = nullptr;
	_mMethodCreateEffect = nullptr;
	_mMethodCreateChromaLinkEffect = nullptr;
	_mMethodCreateHeadsetEffect = nullptr;
	_mMethodCreateKeyboardEffect = nullptr;
	_mMethodCreateKeypadEffect = nullptr;
	_mMethodCreateMouseEffect = nullptr;
	_mMethodCreateMousepadEffect = nullptr;
	_mMethodSetEffect = nullptr;
	_mMethodDeleteEffect = nullptr;

	_sInstance = nullptr;
#endif
}

IChromaSDKPlugin* IChromaSDKPlugin::GetChromaSDKPlugin()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	if (_sInstance == nullptr)
	{
		IChromaSDKPlugin& instance = Get();
		_sInstance = &instance;
	}
	return _sInstance;
#else
    return nullptr;
#endif
}

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE

RZRESULT IChromaSDKPlugin::ChromaSDKInit()
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mInitialized)
	{
		//UE_LOG(LogChromaPlugin, Error, TEXT("ChromaSDKPlugin is already initialized!"));
		return RZRESULT_INVALID;
	}

	if (ChromaThread::Instance() == nullptr)
	{
		ChromaThread::Init();
	}
	ChromaThread::Instance()->Start();

	if (_mMethodInit == nullptr)
	{
		return RZRESULT_INVALID;
	}

	RZRESULT result = _mMethodInit();
	if (result == RZRESULT_SUCCESS)
	{
		_mInitialized = true;
	}
	else
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("ChromaSDKPlugin [Init] result=%d"), result);
	}
	return result;
}

RZRESULT IChromaSDKPlugin::ChromaSDKUnInit()
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (!_mInitialized)
	{
		//UE_LOG(LogChromaPlugin, Error, TEXT("ChromaSDKPlugin is not initialized!"));
		return RZRESULT_INVALID;
	}

	if (ChromaThread::Instance())
	{
		ChromaThread::Instance()->Stop();
	}

	CloseAll();

	if (_mMethodUnInit == nullptr)
	{
		return RZRESULT_INVALID;
	}

	RZRESULT result = _mMethodUnInit();
	_mInitialized = false;
	_mAnimationId = 0;
	_mAnimationMapID.clear();
	_mAnimations.clear();
	_mPlayMap1D.clear();
	_mPlayMap2D.clear();
	if (result != RZRESULT_SUCCESS)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("ChromaSDKPlugin [Uninit] result=%d"), result);
	}
	return result;
}

bool IChromaSDKPlugin::IsInitialized()
{
	return _mInitialized;
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateEffect(RZDEVICEID deviceId, ChromaSDK::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateEffect(deviceId, effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateChromaLinkEffect(ChromaSDK::ChromaLink::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateChromaLinkEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateChromaLinkEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateHeadsetEffect(ChromaSDK::Headset::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateHeadsetEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateHeadsetEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateKeyboardEffect(ChromaSDK::Keyboard::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateKeyboardEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateKeyboardEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateKeypadEffect(ChromaSDK::Keypad::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateKeypadEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateKeypadEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateMouseEffect(ChromaSDK::Mouse::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateMouseEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateMouseEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKCreateMousepadEffect(ChromaSDK::Mousepad::EFFECT_TYPE effect, PRZPARAM pParam, RZEFFECTID* pEffectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodCreateMousepadEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodCreateMousepadEffect(effect, pParam, pEffectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKSetEffect(RZEFFECTID effectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodSetEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodSetEffect(effectId);
}

RZRESULT IChromaSDKPlugin::ChromaSDKDeleteEffect(RZEFFECTID effectId)
{
	if (_sLibraryMissing)
	{
		return RZRESULT_DLL_NOT_FOUND;
	}

	if (_sInvalidSignature)
	{
		return RZRESULT_DLL_INVALID_SIGNATURE;
	}

	if (_mMethodDeleteEffect == nullptr)
	{
		return RZRESULT_INVALID;
	}

	return _mMethodDeleteEffect(effectId);
}

int IChromaSDKPlugin::GetMaxLeds(EChromaSDKDevice1DEnum::Type device)
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	switch (device)
	{
	case EChromaSDKDevice1DEnum::DE_ChromaLink:
		return ChromaSDK::ChromaLink::MAX_LEDS;
	case EChromaSDKDevice1DEnum::DE_Headset:
		return ChromaSDK::Headset::MAX_LEDS;
	case EChromaSDKDevice1DEnum::DE_Mousepad:
		return ChromaSDK::Mousepad::MAX_LEDS;
	}
#endif
	return 0;
}

int IChromaSDKPlugin::GetMaxRow(EChromaSDKDevice2DEnum::Type device)
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	switch (device)
	{
	case EChromaSDKDevice2DEnum::DE_Keyboard:
		return ChromaSDK::Keyboard::MAX_ROW;
	case EChromaSDKDevice2DEnum::DE_Keypad:
		return ChromaSDK::Keypad::MAX_ROW;
	case EChromaSDKDevice2DEnum::DE_Mouse:
		return ChromaSDK::Mouse::MAX_ROW;
	}
#endif
	return 0;
}

int IChromaSDKPlugin::GetMaxColumn(EChromaSDKDevice2DEnum::Type device)
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	switch (device)
	{
	case EChromaSDKDevice2DEnum::DE_Keyboard:
		return ChromaSDK::Keyboard::MAX_COLUMN;
	case EChromaSDKDevice2DEnum::DE_Keypad:
		return ChromaSDK::Keypad::MAX_COLUMN;
	case EChromaSDKDevice2DEnum::DE_Mouse:
		return ChromaSDK::Mouse::MAX_COLUMN;
	}
#endif
	return 0;
}

void IChromaSDKPlugin::CloseAll()
{
	while (_mAnimations.size() > 0)
	{
		auto it = _mAnimations.begin();
		if (it != _mAnimations.end())
		{
			CloseAnimation(it->first);
		}
	}

	_mAnimationId = 0;
}

int IChromaSDKPlugin::CloseAnimation(const int animationId)
{
	auto itAnimation = _mAnimations.find(animationId);
	if (itAnimation != _mAnimations.end())
	{
		AnimationBase* animation = itAnimation->second;
		_mAnimations.erase(itAnimation);
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("CloseAnimation: Animation is null! id=%d"), animationId);
			return -1;
		}
		animation->Stop();
		// protect animation from load/unload while playing
		mutex& animationMutex = animation->GetMutex();
		lock_guard<mutex> animationGuard(animationMutex);
		// do operation
		// unload the animation if loaded
		animation->Unload();
		string animationName = animation->GetName();
		auto itID = _mAnimationMapID.find(animationName);
		if (itID != _mAnimationMapID.end())
		{
			_mAnimationMapID.erase(itID);
		}
		if (ChromaThread::Instance())
		{
			// safely delete playing animations
			ChromaThread::Instance()->DeleteAnimation(animation);
		}
		else
		{
			//delete animation safely
			delete animation;
		}
		return animationId;
	}
	return -1;
}

int IChromaSDKPlugin::CloseAnimationName(const char* path)
{
	vector<int> removeList = vector<int>();
	for (auto it = _mAnimations.begin(); it != _mAnimations.end(); ++it)
	{
		AnimationBase* animation = it->second;
		if (animation != nullptr)
		{
			if (animation->GetName().compare(path) == 0)
			{
				removeList.push_back(it->first);
			}
		}
	}
	while (removeList.size() > 0)
	{
		auto it = removeList.begin();
		if (it != removeList.end())
		{
			int animationId = *it;
			removeList.erase(it);
			CloseAnimation(animationId);
		}
	}
	/*
	else
	{
		//UE_LOG(LogChromaPlugin, Error, TEXT("CloseAnimationName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
	}
	*/
	return -1;
}

int IChromaSDKPlugin::GetAnimation(const char* path)
{
	auto it = _mAnimationMapID.find(path);
	if (it != _mAnimationMapID.end())
	{
		return it->second;
	}
	return OpenAnimation(path);
}

int IChromaSDKPlugin::GetAnimationIdFromInstance(AnimationBase* animation)
{
	if (animation == nullptr)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("GetAnimationIdFromInstance: Invalid animation!"));
		return -1;
	}
	for (auto iter = _mAnimations.begin(); iter != _mAnimations.end(); ++iter)
	{
		if (iter->second == animation)
		{
			return iter->first;
		}
	}
	return -1;
}

AnimationBase* IChromaSDKPlugin::GetAnimationInstance(const int animationId)
{
	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		return it->second;
	}
	return nullptr;
}

AnimationBase* IChromaSDKPlugin::GetAnimationInstanceName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		//LogError("GetAnimationInstanceName: Animation not found! %s\r\n", path);
		return nullptr;
	}
	return GetAnimationInstance(animationId);
}

AnimationBase* IChromaSDKPlugin::GetAnimationInstanceIfOpenName(const char* path)
{
	auto it = _mAnimationMapID.find(path);
	if (it != _mAnimationMapID.end())
	{
		return GetAnimationInstance(it->second);
	}
	//LogError("GetAnimationInstanceIfOpenName: Animation not found! %s\r\n", path);
	return nullptr;
}

const char* IChromaSDKPlugin::GetAnimationName(const int animationId)
{
	if (animationId < 0)
	{
		return "";
	}
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (animation == nullptr)
	{
		return "";
	}
	return animation->GetName().c_str();
}

int IChromaSDKPlugin::GetAnimationCount()
{
	return _mAnimationMapID.size();
}

int IChromaSDKPlugin::GetAnimationId(const int index)
{
	int i = 0;
	for (std::map<string, int>::iterator it = _mAnimationMapID.begin(); it != _mAnimationMapID.end(); ++it)
	{
		if (index == i)
		{
			return (*it).second;
		}
		++i;
	}
	return -1;
}

int IChromaSDKPlugin::GetPlayingAnimationCount()
{
	if (ChromaThread::Instance() == nullptr)
	{
		return 0;
	}
	return ChromaThread::Instance()->GetAnimationCount();
}

int IChromaSDKPlugin::GetPlayingAnimationId(const int index)
{
	if (ChromaThread::Instance() == nullptr)
	{
		return -1;
	}
	return ChromaThread::Instance()->GetAnimationId(index);
}

void IChromaSDKPlugin::PlayAnimation(const int animationId, const bool loop)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("PlayAnimation: Animation is null! id=%d"), animationId);
			return;
		}
		StopAnimationType(animation->GetDeviceTypeId(), animation->GetDeviceId());
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
			_mPlayMap1D[(EChromaSDKDevice1DEnum::Type)animation->GetDeviceId()] = animationId;
			break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
			_mPlayMap2D[(EChromaSDKDevice2DEnum::Type)animation->GetDeviceId()] = animationId;
			break;
		}
		//UE_LOG(LogChromaPlugin, Log, TEXT("PlayAnimation: %s"), *FString(UTF8_TO_TCHAR(animation->GetName().c_str())));
		// protect animation from load/unload while playing
		mutex& animationMutex = animation->GetMutex();
		lock_guard<mutex> animationGuard(animationMutex);
		// do operation
		if (animation->HasUsePreloading())
		{
			animation->Load();
		}
		animation->Play(loop);
	}
}

void IChromaSDKPlugin::PlayAnimationName(const char* path, const bool loop)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("PlayAnimationName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	PlayAnimation(animationId, loop);
}

void IChromaSDKPlugin::StopAnimation(const int animationId)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("StopAnimation: Animation is null! id=%d"), animationId);
			return;
		}
		//UE_LOG(LogChromaPlugin, Log, TEXT("StopAnimation: %s"), *FString(UTF8_TO_TCHAR(animation->GetName().c_str())));
		animation->Stop();
	}
}

void IChromaSDKPlugin::StopAnimationName(const char* path)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("StopAnimationName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	StopAnimation(animationId);
}

void IChromaSDKPlugin::StopAnimationType(const int deviceType, const int device)
{
	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		if (_mPlayMap1D.find((EChromaSDKDevice1DEnum::Type)device) != _mPlayMap1D.end())
		{
			int prevAnimation = _mPlayMap1D[(EChromaSDKDevice1DEnum::Type)device];
			if (prevAnimation != -1)
			{
				StopAnimation(prevAnimation);
				_mPlayMap1D[(EChromaSDKDevice1DEnum::Type)device] = -1;
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		if (_mPlayMap2D.find((EChromaSDKDevice2DEnum::Type)device) != _mPlayMap2D.end())
		{
			int prevAnimation = _mPlayMap2D[(EChromaSDKDevice2DEnum::Type)device];
			if (prevAnimation != -1)
			{
				StopAnimation(prevAnimation);
				_mPlayMap2D[(EChromaSDKDevice2DEnum::Type)device] = -1;
			}
		}
	}
	break;
	}
}

bool IChromaSDKPlugin::IsAnimationPlaying(const int animationId)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("IsAnimationPlaying: Animation is null! id=%d"), animationId);
			return false;
		}
		return animation->IsPlaying();
	}
	return false;
}

bool IChromaSDKPlugin::IsAnimationPlayingName(const char* path)
{
	if (!IsInitialized())
	{
		ChromaSDKInit();
	}
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("IsAnimationPlayingName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return false;
	}
	return IsAnimationPlaying(animationId);
}

bool IChromaSDKPlugin::IsAnimationPlayingType(const int deviceType, const int device)
{
	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		if (_mPlayMap1D.find((EChromaSDKDevice1DEnum::Type)device) != _mPlayMap1D.end())
		{
			int prevAnimation = _mPlayMap1D[(EChromaSDKDevice1DEnum::Type)device];
			if (prevAnimation != -1)
			{
				return IsAnimationPlaying(prevAnimation);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		if (_mPlayMap2D.find((EChromaSDKDevice2DEnum::Type)device) != _mPlayMap2D.end())
		{
			int prevAnimation = _mPlayMap2D[(EChromaSDKDevice2DEnum::Type)device];
			if (prevAnimation != -1)
			{
				return IsAnimationPlaying(prevAnimation);
			}
		}
	}
	break;
	}
	return false;
}

int IChromaSDKPlugin::GetAnimationFrameCount(const int animationId)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return -1;
	}
	return animation->GetFrameCount();
}

int IChromaSDKPlugin::GetAnimationFrameCountName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("GetFrameCountName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return -1;
	}
	return GetAnimationFrameCount(animationId);
}


void IChromaSDKPlugin::SetKeyColor(const int animationId, const int frameId, const int rzkey, const COLORREF color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D &&
		animation->GetDeviceId() == (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			frame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)] = ToLinearColor(color);
		}
	}
}

void IChromaSDKPlugin::SetKeyColorName(const char* path, const int frameId, const int rzkey, const COLORREF color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SetKeyColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	SetKeyColor(animationId, frameId, rzkey, color);
}


void IChromaSDKPlugin::SetKeyNonZeroColor(const int animationId, const int frameId, const int rzkey, const COLORREF color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D &&
		animation->GetDeviceId() == (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			if (frame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)] != ToLinearColor(RGB(0,0,0)))
			{
				frame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)] = ToLinearColor(color);
			}
		}
	}
}

void IChromaSDKPlugin::SetKeyNonZeroColorName(const char* path, const int frameId, const int rzkey, const COLORREF color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SetKeyNonZeroColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	SetKeyNonZeroColor(animationId, frameId, rzkey, color);
}


COLORREF IChromaSDKPlugin::GetKeyColor(const int animationId, const int frameId, const int rzkey)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return 0;
	}
	if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D &&
		animation->GetDeviceId() == (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			FLinearColor& color = frame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)];
			return ToBGR(color);
		}
	}
	return 0;
}

COLORREF IChromaSDKPlugin::GetKeyColorName(const char* path, const int frameId, const int rzkey)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("GetKeyColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return 0;
	}
	return GetKeyColor(animationId, frameId, rzkey);
}

void IChromaSDKPlugin::CopyKeyColor(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int rzkey)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		targetFrame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)] = sourceFrame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)];
	}
}

void IChromaSDKPlugin::CopyKeyColorName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int rzkey)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyKeyColorName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyKeyColorName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyKeyColor(sourceAnimationId, targetAnimationId, frameId, rzkey);
}


void IChromaSDKPlugin::CopyNonZeroKeyColor(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int rzkey)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		FLinearColor& color = sourceFrame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)];
		if (ToBGR(color) != 0)
		{
			targetFrame.Colors[HIBYTE(rzkey)].Colors[LOBYTE(rzkey)] = color;
		}
	}
}

void IChromaSDKPlugin::CopyNonZeroKeyColorName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int rzkey)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroKeyColorName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroKeyColorName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroKeyColor(sourceAnimationId, targetAnimationId, frameId, rzkey);
}


void IChromaSDKPlugin::CopyAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& color = sourceFrame.Colors[i].Colors[j];
				targetFrame.Colors[i].Colors[j] = color;
			}
		}
	}
}

void IChromaSDKPlugin::CopyAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyAllKeysName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyAllKeysName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyAllKeys(sourceAnimationId, targetAnimationId, frameId);
}


// NONZERO

void IChromaSDKPlugin::CopyNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& color = sourceFrame.Colors[i].Colors[j];
				if (ToBGR(color) != 0)
				{
					targetFrame.Colors[i].Colors[j] = color;
				}
			}
		}
	}
}

void IChromaSDKPlugin::CopyNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroAllKeysName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroAllKeysName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroAllKeys(sourceAnimationId, targetAnimationId, frameId);
}

// NONZERO ADD

void IChromaSDKPlugin::AddNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& linearColor = sourceFrame.Colors[i].Colors[j];
				int color = ToBGR(linearColor);
				if (color != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
				}
			}
		}
	}
}

void IChromaSDKPlugin::AddNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroAllKeysName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroAllKeysName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	AddNonZeroAllKeys(sourceAnimationId, targetAnimationId, frameId);
}

// NONZERO SUBTRACT

void IChromaSDKPlugin::SubtractNonZeroAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& linearColor = sourceFrame.Colors[i].Colors[j];
				int color = ToBGR(linearColor);
				if (color != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
				}
			}
		}
	}
}

void IChromaSDKPlugin::SubtractNonZeroAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroAllKeysName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroAllKeysName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	SubtractNonZeroAllKeys(sourceAnimationId, targetAnimationId, frameId);
}


// NONZERO OFFSET

void IChromaSDKPlugin::CopyNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)sourceFrames.size() && (frameId + offset) < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId+offset];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& color = sourceFrame.Colors[i].Colors[j];
				if (ToBGR(color) != 0)
				{
					targetFrame.Colors[i].Colors[j] = color;
				}
			}
		}
	}
}

void IChromaSDKPlugin::CopyNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroAllKeysOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroAllKeysOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroAllKeysOffset(sourceAnimationId, targetAnimationId, frameId, offset);
}


// NONZERO OFFSET ADD

void IChromaSDKPlugin::AddNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
	FChromaSDKColorFrame2D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
	for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
	{
		for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
		{
			FLinearColor& linearColor = sourceFrame.Colors[i].Colors[j];
			int color = ToBGR(linearColor);
			if (color != 0)
			{
				int sourceRed = color & 0xFF;
				int sourceGreen = (color & 0xFF00) >> 8;
				int sourceBlue = (color & 0xFF0000) >> 16;

				int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
				int oldRed = oldColor & 0xFF;
				int oldGreen = (oldColor & 0xFF00) >> 8;
				int oldBlue = (oldColor & 0xFF0000) >> 16;

				int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
				int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
				int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
				int newColor = red | (green << 8) | (blue << 16);

				targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
			}
		}
	}
}

void IChromaSDKPlugin::AddNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroAllKeysOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroAllKeysOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	AddNonZeroAllKeysOffset(sourceAnimationId, targetAnimationId, frameId, offset);
}


// NONZERO OFFSET SUBTRACT

void IChromaSDKPlugin::SubtractNonZeroAllKeysOffset(const int sourceAnimationId, const int targetAnimationId, const int frameId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		sourceAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (targetAnimation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D ||
		targetAnimation->GetDeviceId() != (int)EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	if (frameId < 0)
	{
		return;
	}
	Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
	Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
	vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
	vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
	if (sourceFrames.size() == 0)
	{
		return;
	}
	if (targetFrames.size() == 0)
	{
		return;
	}
	if (frameId < (int)sourceFrames.size() && (frameId + offset) < (int)targetFrames.size())
	{
		FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
		FChromaSDKColorFrame2D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
		for (int i = 0; i < sourceFrame.Colors.Num(); ++i)
		{
			for (int j = 0; j < sourceFrame.Colors[i].Colors.Num(); ++j)
			{
				FLinearColor& linearColor = sourceFrame.Colors[i].Colors[j];
				int color = ToBGR(linearColor);
				int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
				if (color != 0 &&
					oldColor != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
				}
			}
		}
	}
}

void IChromaSDKPlugin::SubtractNonZeroAllKeysOffsetName(const char* sourceAnimation, const char* targetAnimation, const int frameId, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroAllKeysOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroAllKeysOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	SubtractNonZeroAllKeysOffset(sourceAnimationId, targetAnimationId, frameId, offset);
}


// FILL COLOR


void IChromaSDKPlugin::FillColor(const int animationId, const int frameId, const int color)
{
	//clamp values
	FLinearColor newColor = ToLinearColor(color & 0xFFFFFFFF);

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				colors[i] = newColor;
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					row.Colors[j] = newColor;
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillColorName(const char* path, const int frameId, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillColor(animationId, frameId, color);
}


void IChromaSDKPlugin::FillColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue)
{
	int color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				colors[i] = ToLinearColor(color);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					row.Colors[j] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillColorRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillColorRGB(animationId, frameId, red, green, blue);
}


// NONZERO COLOR

void IChromaSDKPlugin::FillNonZeroColor(const int animationId, const int frameId, const int color)
{
	//clamp values
	FLinearColor newColor = ToLinearColor(color & 0xFFFFFFFF);

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				if (ToBGR(colors[i]) != 0)
				{
					colors[i] = newColor;
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					if (ToBGR(row.Colors[j]) != 0)
					{
						row.Colors[j] = newColor;
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillNonZeroColorName(const char* path, const int frameId, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillNonZeroColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillNonZeroColor(animationId, frameId, color);
}


void IChromaSDKPlugin::FillNonZeroColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue)
{
	int color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				if (ToBGR(colors[i]) != 0)
				{
					colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					if (ToBGR(row.Colors[j]) != 0)
					{
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillNonZeroColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillNonZeroColorRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillNonZeroColorRGB(animationId, frameId, red, green, blue);
}


// ZERO COLOR

void IChromaSDKPlugin::FillZeroColor(const int animationId, const int frameId, const int color)
{
	//clamp values
	FLinearColor newColor = ToLinearColor(color & 0xFFFFFFFF);

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				if (ToBGR(colors[i]) == 0)
				{
					colors[i] = newColor;
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					if (ToBGR(row.Colors[j]) == 0)
					{
						row.Colors[j] = newColor;
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillZeroColorName(const char* path, const int frameId, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillZeroColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillZeroColor(animationId, frameId, color);
}


void IChromaSDKPlugin::FillZeroColorRGB(const int animationId, const int frameId, const int red, const int green, const int blue)
{
	//clamp values
	int r = max(0, min(255, red));
	int g = max(0, min(255, green));
	int b = max(0, min(255, blue));
	FLinearColor newColor = ToLinearColor(GetRGB(r, g, b));

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				if (ToBGR(colors[i]) == 0)
				{
					colors[i] = newColor;
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					if (ToBGR(row.Colors[j]) == 0)
					{
						row.Colors[j] = newColor;
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillZeroColorRGBName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillZeroColorRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillZeroColorRGB(animationId, frameId, red, green, blue);
}


// FILL THRESHOLD COLORS

void IChromaSDKPlugin::FillThresholdColors(const int animationId, const int frameId, const int threshold, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				FLinearColor& linearColor = colors[i];
				int oldColor = ToBGR(linearColor);
				int red = oldColor & 0xFF;
				int green = (oldColor & 0xFF00) >> 8;
				int blue = (oldColor & 0xFF0000) >> 16;
				if (red != 0 &&
					green != 0 &&
					blue != 0 &&
					red <= threshold &&
					green <= threshold &&
					blue <= threshold) {
					colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					FLinearColor& linearColor = row.Colors[j];
					int oldColor = ToBGR(linearColor);
					int red = oldColor & 0xFF;
					int green = (oldColor & 0xFF00) >> 8;
					int blue = (oldColor & 0xFF0000) >> 16;
					if (red != 0 &&
						green != 0 &&
						blue != 0 &&
						red <= threshold &&
						green <= threshold &&
						blue <= threshold) {
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillThresholdColorsName(const char* path, const int frameId, const int threshold, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillThresholdColors(animationId, frameId, threshold, color);
}

void IChromaSDKPlugin::FillThresholdColorsRGB(const int animationId, const int frameId, const int threshold, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	int color = GetRGB(red, green, blue);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				FLinearColor& linearColor = colors[i];
				int oldColor = ToBGR(linearColor);
				int r = oldColor & 0xFF;
				int g = (oldColor & 0xFF00) >> 8;
				int b = (oldColor & 0xFF0000) >> 16;
				if (r != 0 &&
					g != 0 &&
					b != 0 &&
					r <= threshold &&
					g <= threshold &&
					b <= threshold) {
					colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					FLinearColor& linearColor = row.Colors[j];
					int oldColor = ToBGR(linearColor);
					int r = oldColor & 0xFF;
					int g = (oldColor & 0xFF00) >> 8;
					int b = (oldColor & 0xFF0000) >> 16;
					if (r != 0 &&
						g != 0 &&
						b != 0 &&
						r <= threshold &&
						g <= threshold &&
						b <= threshold) {
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillThresholdColorsRGBName(const char* path, const int frameId, const int threshold, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillThresholdColorsRGB(animationId, frameId, threshold, red, green, blue);
}


// FILL THRESHOLD COLORS ALL FRAMES

void IChromaSDKPlugin::FillThresholdColorsAllFrames(const int animationId, const int threshold, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillThresholdColors(animationId, frameId, threshold, color);
	}
}

void IChromaSDKPlugin::FillThresholdColorsAllFramesName(const char* path, const int threshold, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillThresholdColorsAllFrames(animationId, threshold, color);
}

void IChromaSDKPlugin::FillThresholdColorsAllFramesRGB(const int animationId, const int threshold, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillThresholdColorsRGB(animationId, frameId, threshold, red, green, blue);
	}
}

void IChromaSDKPlugin::FillThresholdColorsAllFramesRGBName(const char* path, const int threshold, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillThresholdColorsAllFramesRGB(animationId, threshold, red, green, blue);
}


// MIN MAX
void IChromaSDKPlugin::FillThresholdColorsMinMaxRGB(const int animationId, const int frameId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue)
{
	StopAnimation(animationId);
	int minColor = GetRGB(minRed, minGreen, minBlue);
	int maxColor = GetRGB(maxRed, maxGreen, maxBlue);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int oldColor = ToBGR(colors[i]);
				int red = oldColor & 0xFF;
				int green = (oldColor & 0xFF00) >> 8;
				int blue = (oldColor & 0xFF0000) >> 16;
				if (red != 0 ||
					green != 0 ||
					blue != 0)
				{
					if (red <= minThreshold &&
						green <= minThreshold &&
						blue <= minThreshold) {
						colors[i] = ToLinearColor(minColor);
					}
					else if (red >= maxThreshold ||
						green >= maxThreshold ||
						blue >= maxThreshold) {
						colors[i] = ToLinearColor(maxColor);
					}
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int oldColor = ToBGR(row.Colors[j]);
					int red = oldColor & 0xFF;
					int green = (oldColor & 0xFF00) >> 8;
					int blue = (oldColor & 0xFF0000) >> 16;
					if (red != 0 ||
						green != 0 ||
						blue != 0)
					{
						if (red <= minThreshold &&
							green <= minThreshold &&
							blue <= minThreshold) {
							row.Colors[j] = ToLinearColor(minColor);
						}
						else if (red >= maxThreshold ||
							green >= maxThreshold ||
							blue >= maxThreshold) {
							row.Colors[j] = ToLinearColor(maxColor);
						}
					}
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::FillThresholdColorsMinMaxRGBName(const char* path, const int frameId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsMinMaxRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillThresholdColorsMinMaxRGB(animationId, frameId, minThreshold, minRed, minGreen, minBlue, maxThreshold, maxRed, maxGreen, maxBlue);
}

// ALL FRAMES
void IChromaSDKPlugin::FillThresholdColorsMinMaxAllFramesRGB(const int animationId, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillThresholdColorsMinMaxRGB(animationId, frameId, minThreshold, minRed, minGreen, minBlue, maxThreshold, maxRed, maxGreen, maxBlue);
	}
}
void IChromaSDKPlugin::FillThresholdColorsMinMaxAllFramesRGBName(const char* path, const int minThreshold, const int minRed, const int minGreen, const int minBlue, const int maxThreshold, const int maxRed, const int maxGreen, const int maxBlue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdColorsMinMaxAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillThresholdColorsMinMaxAllFramesRGB(animationId, minThreshold, minRed, minGreen, minBlue, maxThreshold, maxRed, maxGreen, maxBlue);
}


// FILL COLOR ALL FRAMES


void IChromaSDKPlugin::FillColorAllFrames(const int animationId, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillColor(animationId, frameId, color);
	}
}

void IChromaSDKPlugin::FillColorAllFramesName(const char* path, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillColorAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillColorAllFrames(animationId, color);
}


void IChromaSDKPlugin::FillColorAllFramesRGB(const int animationId, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillColorRGB(animationId, frameId, red, green, blue);
	}
}

void IChromaSDKPlugin::FillColorAllFramesRGBName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillColorAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillColorAllFramesRGB(animationId, red, green, blue);
}


// NONZERO COLOR


void IChromaSDKPlugin::FillNonZeroColorAllFrames(const int animationId, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillNonZeroColor(animationId, frameId, color);
	}
}

void IChromaSDKPlugin::FillNonZeroColorAllFramesName(const char* path, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillNonZeroColorAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillNonZeroColorAllFrames(animationId, color);
}


void IChromaSDKPlugin::FillNonZeroColorAllFramesRGB(const int animationId, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillNonZeroColorRGB(animationId, frameId, red, green, blue);
	}
}

void IChromaSDKPlugin::FillNonZeroColorAllFramesRGBName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillNonZeroColorAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillNonZeroColorAllFramesRGB(animationId, red, green, blue);
}


// ZERO COLOR


void IChromaSDKPlugin::FillZeroColorAllFrames(const int animationId, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillZeroColor(animationId, frameId, color);
	}
}

void IChromaSDKPlugin::FillZeroColorAllFramesName(const char* path, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillZeroColorAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillZeroColorAllFrames(animationId, color);
}


void IChromaSDKPlugin::FillZeroColorAllFramesRGB(const int animationId, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillZeroColorRGB(animationId, frameId, red, green, blue);
	}
}

void IChromaSDKPlugin::FillZeroColorAllFramesRGBName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillZeroColorAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return FillZeroColorAllFramesRGB(animationId, red, green, blue);
}


// FILL RANDOM COLORS


void IChromaSDKPlugin::FillRandomColors(const int animationId, const int frameId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	if (frameId >= 0 && frameId < frameCount)
	{
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				frame.Colors = UChromaSDKPluginBPLibrary::CreateRandomColors1D(animation1D->GetDevice());
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				frame.Colors = UChromaSDKPluginBPLibrary::CreateRandomColors2D(animation2D->GetDevice());
			}
		}
		break;
		}
	}
}

void IChromaSDKPlugin::FillRandomColorsName(const char* path, const int frameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillRandomColorsName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillRandomColors(animationId, frameId);
}


void IChromaSDKPlugin::FillRandomColorsAllFrames(const int animationId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillRandomColors(animationId, frameId);
	}
}

void IChromaSDKPlugin::FillRandomColorsAllFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillRandomColorsAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillRandomColorsAllFrames(animationId);
}


// FILL RANDOM BLACK AND WHITE


void IChromaSDKPlugin::FillRandomColorsBlackAndWhite(const int animationId, const int frameId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	if (frameId >= 0 && frameId < frameCount)
	{
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				frame.Colors = UChromaSDKPluginBPLibrary::CreateRandomColorsBlackAndWhite1D(animation1D->GetDevice());
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				frame.Colors = UChromaSDKPluginBPLibrary::CreateRandomColorsBlackAndWhite2D(animation2D->GetDevice());
			}
		}
		break;
		}
	}
}

void IChromaSDKPlugin::FillRandomColorsBlackAndWhiteName(const char* path, const int frameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillRandomColorsBlackAndWhiteName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillRandomColorsBlackAndWhite(animationId, frameId);
}


void IChromaSDKPlugin::FillRandomColorsBlackAndWhiteAllFrames(const int animationId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillRandomColorsBlackAndWhite(animationId, frameId);
	}
}

void IChromaSDKPlugin::FillRandomColorsBlackAndWhiteAllFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillRandomColorsBlackAndWhiteAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillRandomColorsBlackAndWhiteAllFrames(animationId);
}


// OFFSET COLORS

void IChromaSDKPlugin::OffsetColors(const int animationId, const int frameId, const int offsetRed, const int offsetGreen, const int offsetBlue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(colors[i]);
				int red = (color & 0xFF);
				int green = (color & 0xFF00) >> 8;
				int blue = (color & 0xFF0000) >> 16;
				red = max(0, min(255, red + offsetRed));
				green = max(0, min(255, green + offsetGreen));
				blue = max(0, min(255, blue + offsetBlue));
				color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
				colors[i] = ToLinearColor(color);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(row.Colors[j]);
					int red = (color & 0xFF);
					int green = (color & 0xFF00) >> 8;
					int blue = (color & 0xFF0000) >> 16;
					red = max(0, min(255, red + offsetRed));
					green = max(0, min(255, green + offsetGreen));
					blue = max(0, min(255, blue + offsetBlue));
					color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
					row.Colors[j] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	}
}
void IChromaSDKPlugin::OffsetColorsName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OffsetColorsName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return OffsetColors(animationId, frameId, red, green, blue);
}


void IChromaSDKPlugin::OffsetColorsAllFrames(const int animationId, const int offsetRed, const int offsetGreen, const int offsetBlue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		OffsetColors(animationId, frameId, offsetRed, offsetGreen, offsetBlue);
	}
}
void IChromaSDKPlugin::OffsetColorsAllFramesName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OffsetColorsAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return OffsetColorsAllFrames(animationId, red, green, blue);
}


void IChromaSDKPlugin::OffsetNonZeroColors(const int animationId, const int frameId, const int offsetRed, const int offsetGreen, const int offsetBlue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(colors[i]);
				if (color != 0)
				{
					int red = (color & 0xFF);
					int green = (color & 0xFF00) >> 8;
					int blue = (color & 0xFF0000) >> 16;
					red = max(0, min(255, red + offsetRed));
					green = max(0, min(255, green + offsetGreen));
					blue = max(0, min(255, blue + offsetBlue));
					color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
					colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(row.Colors[j]);
					if (color != 0)
					{
						int red = (color & 0xFF);
						int green = (color & 0xFF00) >> 8;
						int blue = (color & 0xFF0000) >> 16;
						red = max(0, min(255, red + offsetRed));
						green = max(0, min(255, green + offsetGreen));
						blue = max(0, min(255, blue + offsetBlue));
						color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
	break;
	}
}
void IChromaSDKPlugin::OffsetNonZeroColorsName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OffsetNonZeroColorsName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return OffsetNonZeroColors(animationId, frameId, red, green, blue);
}


void IChromaSDKPlugin::OffsetNonZeroColorsAllFrames(const int animationId, const int offsetRed, const int offsetGreen, const int offsetBlue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		OffsetNonZeroColors(animationId, frameId, offsetRed, offsetGreen, offsetBlue);
	}
}
void IChromaSDKPlugin::OffsetNonZeroColorsAllFramesName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OffsetNonZeroColorsAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return OffsetNonZeroColorsAllFrames(animationId, red, green, blue);
}


// MULTIPLY INTENSITY

void IChromaSDKPlugin::MultiplyIntensity(const int animationId, const int frameId, const float intensity)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(colors[i]);
				int red = (color & 0xFF);
				int green = (color & 0xFF00) >> 8;
				int blue = (color & 0xFF0000) >> 16;
				red = max(0, min(255, red * intensity));
				green = max(0, min(255, green * intensity));
				blue = max(0, min(255, blue * intensity));
				color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
				colors[i] = ToLinearColor(color);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(row.Colors[j]);
					int red = (color & 0xFF);
					int green = (color & 0xFF00) >> 8;
					int blue = (color & 0xFF0000) >> 16;
					red = max(0, min(255, red * intensity));
					green = max(0, min(255, green * intensity));
					blue = max(0, min(255, blue * intensity));
					color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
					row.Colors[j] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::MultiplyIntensityName(const char* path, const int frameId, const float intensity)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensity(animationId, frameId, intensity);
}

// MULTIPLY INTENSITY COLOR

void IChromaSDKPlugin::MultiplyIntensityColor(const int animationId, const int frameId, const int color)
{
	StopAnimation(animationId);
	int red = (color & 0xFF);
	int green = (color & 0xFF00) >> 8;
	int blue = (color & 0xFF0000) >> 16;
	float redIntensity = red / 255.0f;
	float greenIntensity = green / 255.0f;
	float blueIntensity = blue / 255.0f;
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int oldColor = ToBGR(colors[i]);
				int r = (oldColor & 0xFF);
				int g = (oldColor & 0xFF00) >> 8;
				int b = (oldColor & 0xFF0000) >> 16;
				r = max(0, min(255, r * redIntensity));
				g = max(0, min(255, g * greenIntensity));
				b = max(0, min(255, b * blueIntensity));
				int newColor = (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
				colors[i] = ToLinearColor(newColor);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int oldColor = ToBGR(row.Colors[j]);
					int r = (oldColor & 0xFF);
					int g = (oldColor & 0xFF00) >> 8;
					int b = (oldColor & 0xFF0000) >> 16;
					r = max(0, min(255, r * redIntensity));
					g = max(0, min(255, g * greenIntensity));
					b = max(0, min(255, b * blueIntensity));
					int newColor = (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
					row.Colors[j] = ToLinearColor(newColor);
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::MultiplyIntensityColorName(const char* path, const int frameId, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityColorName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensityColor(animationId, frameId, color);
}


// MULTIPLY INTENSITY RGB

void IChromaSDKPlugin::MultiplyIntensityRGB(const int animationId, const int frameId, const int red, const int green, const int blue)
{
	float redIntensity = red / 255.0f;
	float greenIntensity = green / 255.0f;
	float blueIntensity = blue / 255.0f;

	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				FLinearColor& linearColor = colors[i];
				int color = ToBGR(linearColor);
				int r = (color & 0xFF);
				int g = (color & 0xFF00) >> 8;
				int b = (color & 0xFF0000) >> 16;
				r = max(0, min(255, r * redIntensity));
				g = max(0, min(255, g * greenIntensity));
				b = max(0, min(255, b * blueIntensity));
				color = (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
				colors[i] = ToLinearColor(color);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					FLinearColor& linearColor = row.Colors[j];
					int color = ToBGR(linearColor);
					int r = (color & 0xFF);
					int g = (color & 0xFF00) >> 8;
					int b = (color & 0xFF0000) >> 16;
					r = max(0, min(255, r * redIntensity));
					g = max(0, min(255, g * greenIntensity));
					b = max(0, min(255, b * blueIntensity));
					color = (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
					row.Colors[j] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	}
}

void IChromaSDKPlugin::MultiplyIntensityRGBName(const char* path, const int frameId, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensityRGB(animationId, frameId, red, green, blue);
}


// MULTIPLY INTENSITY ALL FRAMES

void IChromaSDKPlugin::MultiplyIntensityAllFrames(const int animationId, const float intensity)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		MultiplyIntensity(animationId, frameId, intensity);
	}
}

void IChromaSDKPlugin::MultiplyIntensityAllFramesName(const char* path, const float intensity)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensityAllFrames(animationId, intensity);
}


// MULTIPLY INTENSITY COLOR ALL FRAMES

void IChromaSDKPlugin::MultiplyIntensityColorAllFrames(const int animationId, const int color)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		MultiplyIntensityColor(animationId, frameId, color);
	}
}

void IChromaSDKPlugin::MultiplyIntensityColorAllFramesName(const char* path, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityColorAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensityColorAllFrames(animationId, color);
}


// MULTIPLY INTENSITY ALL FRAMES RGB

void IChromaSDKPlugin::MultiplyIntensityAllFramesRGB(const int animationId, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = GetAnimationFrameCount(animationId);
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		MultiplyIntensityRGB(animationId, frameId, red, green, blue);
	}
}

void IChromaSDKPlugin::MultiplyIntensityAllFramesRGBName(const char* path, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyIntensityAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	return MultiplyIntensityAllFramesRGB(animationId, red, green, blue);
}


void IChromaSDKPlugin::LoadAnimation(const int animationId)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	animation->Stop();
	// protect animation from load/unload while playing
	mutex& animationMutex = animation->GetMutex();
	lock_guard<mutex> animationGuard(animationMutex);
	// do operation
	animation->Load();
}

void IChromaSDKPlugin::LoadAnimationName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("LoadName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	LoadAnimation(animationId);
}

void IChromaSDKPlugin::UnloadAnimation(const int animationId)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	animation->Stop();
	// protect animation from load/unload while playing
	mutex& animationMutex = animation->GetMutex();
	lock_guard<mutex> animationGuard(animationMutex);
	// do operation
	animation->Unload();
}

void IChromaSDKPlugin::UnloadAnimationName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("UnloadAnimationName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	UnloadAnimation(animationId);
}

void IChromaSDKPlugin::SetChromaCustomFlag(const int animationId, const bool flag)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D)
	{
		return;
	}
	if (animation->GetDeviceId() != EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	Animation2D* animation2D = (Animation2D*)(animation);
	animation2D->SetChromaCustom(flag);
}

void IChromaSDKPlugin::SetChromaCustomFlagName(const char* path, const bool flag)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SetChromaCustomFlagName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	SetChromaCustomFlag(animationId, flag);
}

void IChromaSDKPlugin::SetChromaCustomColorAllFrames(const int animationId)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D)
	{
		return;
	}
	if (animation->GetDeviceId() != EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	Animation2D* animation2D = (Animation2D*)(animation);
	vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
	for (int frameId = 0; frameId < int(frames.size()); ++frameId)
	{
		FChromaSDKColorFrame2D& frame = frames[frameId];
		int maxRow = GetMaxRow(animation2D->GetDevice());
		int maxColumn = GetMaxColumn(animation2D->GetDevice());
		for (int i = 0; i < maxRow; ++i)
		{
			FChromaSDKColors& row = frame.Colors[i];
			for (int j = 0; j < maxColumn; ++j)
			{
				int color = ToBGR(row.Colors[j]);
				int customFlag = 0x1;
				int red = (color & 0xFF);
				int green = (color & 0xFF00) >> 8;
				int blue = (color & 0xFF0000) >> 16;
				color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16) | (customFlag << 24);
				row.Colors[j] = ToLinearColor(color);
			}
		}
	}
}

void IChromaSDKPlugin::SetChromaCustomColorAllFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SetChromaCustomColorAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	SetChromaCustomColorAllFrames(animationId);
}


int IChromaSDKPlugin::PreviewFrame(const int animationId, const int frameId)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			return -1;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				TArray<FLinearColor>& colors = frame.Colors;
				FChromaSDKEffectResult result = UChromaSDKPluginBPLibrary::ChromaSDKCreateEffectCustom1D(animation1D->GetDevice(), colors);
				if (result.Result == 0)
				{
					UChromaSDKPluginBPLibrary::ChromaSDKSetEffect(result.EffectId);
					UChromaSDKPluginBPLibrary::ChromaSDKDeleteEffect(result.EffectId);
				}
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			if (frameId >= 0 && frameId < (int)frames.size())
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				TArray<FChromaSDKColors>& colors = frame.Colors;
				FChromaSDKEffectResult result;
				if (animation2D->UseChromaCustom())
				{
					result = UChromaSDKPluginBPLibrary::ChromaSDKCreateEffectKeyboardCustom2D(colors);
				}
				else
				{
					result = UChromaSDKPluginBPLibrary::ChromaSDKCreateEffectCustom2D(animation2D->GetDevice(), colors);
				}
				if (result.Result == 0)
				{
					UChromaSDKPluginBPLibrary::ChromaSDKSetEffect(result.EffectId);
					UChromaSDKPluginBPLibrary::ChromaSDKDeleteEffect(result.EffectId);
				}
			}
		}
		break;
		}
		return animationId;
	}

	return -1;
}

void IChromaSDKPlugin::PreviewFrameName(const char* path, const int frameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("PreviewFrameName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	PreviewFrame(animationId, frameId);
}


int IChromaSDKPlugin::OverrideFrameDuration(const int animationId, const float duration)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			return -1;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			for (int i = 0; i < int(frames.size()); ++i)
			{
				FChromaSDKColorFrame1D& frame = frames[i];
				if (duration < 0.033f)
				{
					frame.Duration = 0.033f;
				}
				else
				{
					frame.Duration = duration;
				}
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			for (int i = 0; i < int(frames.size()); ++i)
			{
				FChromaSDKColorFrame2D& frame = frames[i];
				if (duration < 0.033f)
				{
					frame.Duration = 0.033f;
				}
				else
				{
					frame.Duration = duration;
				}
			}
		}
		break;
		}
		return animationId;
	}

	return -1;
}

void IChromaSDKPlugin::OverrideFrameDurationName(const char* path, const float duration)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OverrideFrameDurationName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	OverrideFrameDuration(animationId, duration);
}


// MAKE FRAMES


void IChromaSDKPlugin::MakeBlankFrames(const int animationId, const int frameCount, const float duration, const int color)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	StopAnimation(animationId);
	if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* animation1D = (Animation1D*)animation;
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		frames.clear();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
			frame.Duration = duration;
			frame.Colors = UChromaSDKPluginBPLibrary::CreateColors1D(animation1D->GetDevice());
			int maxLeds = UChromaSDKPluginBPLibrary::GetMaxLeds(animation1D->GetDevice());
			for (int i = 0; i < maxLeds; ++i)
			{
				frame.Colors[i] = ToLinearColor(color);
			}
			frames.push_back(frame);
		}
	}
	else if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* animation2D = (Animation2D*)animation;
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		frames.clear();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
			frame.Duration = duration;
			frame.Colors = UChromaSDKPluginBPLibrary::CreateColors2D(animation2D->GetDevice());
			int maxRow = UChromaSDKPluginBPLibrary::GetMaxRow(animation2D->GetDevice());
			int maxColumn = UChromaSDKPluginBPLibrary::GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					row.Colors[j] = ToLinearColor(color);
				}
			}
			frames.push_back(frame);
		}
	}
}

void IChromaSDKPlugin::MakeBlankFramesName(const char* path, const int frameCount, const float duration, const int color)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MakeBlankFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MakeBlankFrames(animationId, frameCount, duration, color);
}


void IChromaSDKPlugin::MakeBlankFramesRGB(const int animationId, const int frameCount, const float duration, const int red, const int green, const int blue)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	StopAnimation(animationId);
	int color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
	if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		frames.clear();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
			frame.Duration = duration;
			frame.Colors = UChromaSDKPluginBPLibrary::CreateColors1D(animation1D->GetDevice());
			int maxLeds = UChromaSDKPluginBPLibrary::GetMaxLeds(animation1D->GetDevice());
			for (int i = 0; i < maxLeds; ++i)
			{
				frame.Colors[i] = ToLinearColor(color);
			}
			frames.push_back(frame);
		}
	}
	else if (animation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		frames.clear();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
			frame.Duration = duration;
			frame.Colors = UChromaSDKPluginBPLibrary::CreateColors2D(animation2D->GetDevice());
			int maxRow = UChromaSDKPluginBPLibrary::GetMaxRow(animation2D->GetDevice());
			int maxColumn = UChromaSDKPluginBPLibrary::GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					row.Colors[j] = ToLinearColor(color);
				}
			}
			frames.push_back(frame);
		}
	}
}

void IChromaSDKPlugin::MakeBlankFramesRGBName(const char* path, const int frameCount, const float duration, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MakeBlankFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MakeBlankFramesRGB(animationId, frameCount, duration, red, green, blue);
}


// RANDOM


void IChromaSDKPlugin::MakeBlankFramesRandom(const int animationId, const int frameCount, const float duration)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D)
	{
		return;
	}
	if (animation->GetDeviceId() != EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	StopAnimation(animationId);
	Animation2D* animation2D = (Animation2D*)(animation);
	vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
	frames.clear();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
		frame.Duration = duration;
		frame.Colors = UChromaSDKPluginBPLibrary::CreateColors2D(animation2D->GetDevice());
		int maxRow = UChromaSDKPluginBPLibrary::GetMaxRow(animation2D->GetDevice());
		int maxColumn = UChromaSDKPluginBPLibrary::GetMaxColumn(animation2D->GetDevice());
		for (int i = 0; i < maxRow; ++i)
		{
			FChromaSDKColors& row = frame.Colors[i];
			for (int j = 0; j < maxColumn; ++j)
			{
				int red = fastrand() % 256;
				int green = fastrand() % 256;
				int blue = fastrand() % 256;
				COLORREF color = GetRGB(red, green, blue);
				row.Colors[j] = ToLinearColor(color);
			}
		}
		frames.push_back(frame);
	}
}

void IChromaSDKPlugin::MakeBlankFramesRandomName(const char* path, int frameCount, const float duration)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MakeBlankFramesRandomName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MakeBlankFramesRandom(animationId, frameCount, duration);
}


// RANDOM BLACK AND WHITE


void IChromaSDKPlugin::MakeBlankFramesRandomBlackAndWhite(const int animationId, const int frameCount, const float duration)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	if (animation->GetDeviceType() != EChromaSDKDeviceTypeEnum::DE_2D)
	{
		return;
	}
	if (animation->GetDeviceId() != EChromaSDKDevice2DEnum::DE_Keyboard)
	{
		return;
	}
	StopAnimation(animationId);
	Animation2D* animation2D = (Animation2D*)(animation);
	vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
	frames.clear();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
		frame.Duration = duration;
		frame.Colors = UChromaSDKPluginBPLibrary::CreateColors2D(animation2D->GetDevice());
		int maxRow = UChromaSDKPluginBPLibrary::GetMaxRow(animation2D->GetDevice());
		int maxColumn = UChromaSDKPluginBPLibrary::GetMaxColumn(animation2D->GetDevice());
		for (int i = 0; i < maxRow; ++i)
		{
			FChromaSDKColors& row = frame.Colors[i];
			for (int j = 0; j < maxColumn; ++j)
			{
				int gray = fastrand() % 256;
				COLORREF color = RGB(gray, gray, gray);
				row.Colors[j] = ToLinearColor(color);
			}
		}
		frames.push_back(frame);
	}
}

void IChromaSDKPlugin::MakeBlankFramesRandomBlackAndWhiteName(const char* path, const int frameCount, const float duration)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MakeBlankFramesRandomBlackAndWhiteName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MakeBlankFramesRandomBlackAndWhite(animationId, frameCount, duration);
}


// REVERSE ALL FRAMES
void IChromaSDKPlugin::ReverseAllFrames(const int animationId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		reverse(frames.begin(), frames.end());
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		reverse(frames.begin(), frames.end());
	}
	break;
	}
}
void IChromaSDKPlugin::ReverseAllFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("ReverseAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	ReverseAllFrames(animationId);
}


// DUPLICATE FRAMES
void IChromaSDKPlugin::DuplicateFrames(const int animationId)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateFrames: Animation is null! id=%d"), animationId);
			return;
		}
		int frameCount = animation->GetFrameCount();
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				newFrames.push_back(frame);
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame1D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				newFrames.push_back(frame);
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame2D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::DuplicateFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	DuplicateFrames(animationId);
}

// DUPLICATE FIRST FRAME
void IChromaSDKPlugin::DuplicateFirstFrame(const int animationId, const int frameCount)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateFirstFrame: Animation is null! id=%d"), animationId);
			return;
		}
		if (animation->GetFrameCount() == 0)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("PluginDuplicateFirstFrame: Animation has no frames! id=%d"), animationId);
			return;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			FChromaSDKColorFrame1D firstFrame = frames[0];
			frames.clear();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				frames.push_back(firstFrame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			FChromaSDKColorFrame2D firstFrame = frames[0]; //copy
			frames.clear();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				frames.push_back(firstFrame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::DuplicateFirstFrameName(const char* path, const int frameCount)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateFirstFrameName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	DuplicateFirstFrame(animationId, frameCount);
}

// DUPLICATE MIRROR FRAMES
void IChromaSDKPlugin::DuplicateMirrorFrames(const int animationId)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateMirrorFrames: Animation is null! id=%d"), animationId);
			return;
		}
		int frameCount = animation->GetFrameCount();
		if (frameCount == 0)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateMirrorFrames: Animation has no frames! id=%d"), animationId);
			return;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			for (int frameId = frameCount - 1; frameId >= 0; --frameId)
			{
				FChromaSDKColorFrame1D frame = frames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			for (int frameId = frameCount - 1; frameId >= 0; --frameId)
			{
				FChromaSDKColorFrame2D frame = frames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::DuplicateMirrorFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("DuplicateMirrorFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	DuplicateMirrorFrames(animationId);
}

// INSERT FRAME
void IChromaSDKPlugin::InsertFrame(const int animationId, const int sourceFrame, const int targetFrame)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("InsertFrame: Animation is null! id=%d"), animationId);
			return;
		}
		int frameCount = animation->GetFrameCount();
		if (sourceFrame < 0 ||
			sourceFrame >= frameCount)
		{
			return;
		}
		if (targetFrame < 0 ||
			targetFrame >= frameCount)
		{
			return;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				if (targetFrame == frameId)
				{
					FChromaSDKColorFrame1D frame = frames[sourceFrame];
					newFrames.push_back(frame);
				}

				FChromaSDKColorFrame1D frame = frames[frameId];
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame1D frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				if (targetFrame == frameId)
				{
					FChromaSDKColorFrame2D frame = frames[sourceFrame];
					newFrames.push_back(frame);
				}

				FChromaSDKColorFrame2D frame = frames[frameId];
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame2D frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::InsertFrameName(const char* path, const int sourceFrame, const int targetFrame)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("InsertFrameName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	InsertFrame(animationId, sourceFrame, targetFrame);
}

// INSERT DELAY
void IChromaSDKPlugin::InsertDelay(const int animationId, const int frameId, const int delay)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("InsertDelay: Animation is null! id=%d"), animationId);
			return;
		}
		int frameCount = animation->GetFrameCount();
		if (frameId < 0 ||
			frameId >= frameCount)
		{
			return;
		}
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int index = 0; index < frameCount; ++index)
			{
				FChromaSDKColorFrame1D frame = frames[index];
				if (index == frameId)
				{
					for (int d = 0; d < delay; ++d)
					{
						newFrames.push_back(frame);
					}
				}
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int index = 0; index < (int)newFrames.size(); ++index)
			{
				FChromaSDKColorFrame1D frame = newFrames[index];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int index = 0; index < frameCount; ++index)
			{
				FChromaSDKColorFrame2D frame = frames[index];
				if (index == frameId)
				{
					for (int d = 0; d < delay; ++d)
					{
						newFrames.push_back(frame);
					}
				}
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int index = 0; index < (int)newFrames.size(); ++index)
			{
				FChromaSDKColorFrame2D frame = newFrames[index];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::InsertDelayName(const char* path, const int frameId, const int delay)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("InsertDelayName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	InsertDelay(animationId, frameId, delay);
}

// REDUCE FRAMES
void IChromaSDKPlugin::ReduceFrames(const int animationId, const int n)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("ReduceFrames: Animation is null! id=%d"), animationId);
			return;
		}
		if (n <= 0)
		{
			return;
		}
		int frameCount = animation->GetFrameCount();
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				if (frameId % n == 0)
				{
					FChromaSDKColorFrame1D& frame = frames[frameId];
					newFrames.push_back(frame);
				}
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame1D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int frameId = 0; frameId < frameCount; ++frameId)
			{
				if (frameId % n == 0)
				{
					FChromaSDKColorFrame2D& frame = frames[frameId];
					newFrames.push_back(frame);
				}
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame2D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::ReduceFramesName(const char* path, const int n)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("ReduceFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	ReduceFrames(animationId, n);
}

// TRIM FRAME
void IChromaSDKPlugin::TrimFrame(const int animationId, const int frameId)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("TrimFrame: Animation is null! id=%d"), animationId);
			return;
		}
		int frameCount = animation->GetFrameCount();
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int index = 0; index < frameCount; ++index)
			{
				if (index != frameId)
				{
					FChromaSDKColorFrame1D& frame = frames[frameId];
					newFrames.push_back(frame);
				}
			}
			frames.clear();
			for (int index = 0; index < (int)newFrames.size(); ++index)
			{
				FChromaSDKColorFrame1D& frame = newFrames[index];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int index = 0; index < frameCount; ++index)
			{
				if (index != frameId)
				{
					FChromaSDKColorFrame2D& frame = frames[frameId];
					newFrames.push_back(frame);
				}
			}
			frames.clear();
			for (int index = 0; index < (int)newFrames.size(); ++index)
			{
				FChromaSDKColorFrame2D& frame = newFrames[index];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::TrimFrameName(const char* path, const int frameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("TrimFrameName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	TrimFrame(animationId, frameId);
}

// TRIM START FRAMES
void IChromaSDKPlugin::TrimStartFrames(const int animationId, const int numberOfFrames)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("TrimStartFrames: Animation is null! id=%d"), animationId);
			return;
		}
		if (numberOfFrames < 0)
		{
			return;
		}
		int frameCount = animation->GetFrameCount();
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			vector<FChromaSDKColorFrame1D> newFrames = vector<FChromaSDKColorFrame1D>();
			for (int frameId = numberOfFrames; frameId < frameCount; ++frameId)
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame1D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			vector<FChromaSDKColorFrame2D> newFrames = vector<FChromaSDKColorFrame2D>();
			for (int frameId = numberOfFrames; frameId < frameCount; ++frameId)
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				newFrames.push_back(frame);
			}
			frames.clear();
			for (int frameId = 0; frameId < (int)newFrames.size(); ++frameId)
			{
				FChromaSDKColorFrame2D& frame = newFrames[frameId];
				frames.push_back(frame);
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::TrimStartFramesName(const char* path, const int numberOfFrames)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("TrimStartFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	TrimStartFrames(animationId, numberOfFrames);
}

// TRIM END FRAMES
void IChromaSDKPlugin::TrimEndFrames(const int animationId, const int lastFrameId)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("TrimEndFrames: Animation is null! id=%d"), animationId);
			return;
		}
		if (lastFrameId <= 0)
		{
			return;
		}
		int frameCount = animation->GetFrameCount();
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			while (lastFrameId < (int)frames.size())
			{
				frames.pop_back();
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			while (lastFrameId < (int)frames.size())
			{
				frames.pop_back();
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::TrimEndFramesName(const char* path, const int lastFrameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("TrimEndFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	TrimEndFrames(animationId, lastFrameId);
}

// FADE START FRAMES
void IChromaSDKPlugin::FadeStartFrames(const int animationId, const int fade)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("FadeStartFrames: Animation is null! id=%d"), animationId);
			return;
		}
		if (fade <= 0)
		{
			return;
		}
		int frameCount = animation->GetFrameCount();
		for (int frameId = 0; frameId < fade; ++frameId)
		{
			float t = (frameId + 1) / (float)fade;
			MultiplyIntensity(animationId, frameId, t);
		}
	}
}
void IChromaSDKPlugin::FadeStartFramesName(const char* path, const int fade)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FadeStartFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FadeStartFrames(animationId, fade);
}

// FADE END FRAMES
void IChromaSDKPlugin::FadeEndFrames(const int animationId, const int fade)
{
	StopAnimation(animationId);

	auto it = _mAnimations.find(animationId);
	if (it != _mAnimations.end())
	{
		AnimationBase* animation = it->second;
		if (animation == nullptr)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("FadeEndFrames: Animation is null! id=%d"), animationId);
			return;
		}
		if (fade <= 0)
		{
			return;
		}
		int frameCount = animation->GetFrameCount();
		for (int offset = 0; offset < fade; ++offset)
		{
			int frameId = frameCount - 1 - offset;
			float t = (offset + 1) / (float)fade;
			MultiplyIntensity(animationId, frameId, t);
		}
	}
}
void IChromaSDKPlugin::FadeEndFramesName(const char* path, const int fade)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FadeEndFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FadeEndFrames(animationId, fade);
}

// CREATE ANIMATION IN MEMORY
int IChromaSDKPlugin::CreateAnimationInMemory(const int deviceType, const int device)
{
	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = new Animation1D();
		animation1D->SetDevice((EChromaSDKDevice1DEnum::Type)device);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		frames.clear();
		FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
		frame.Colors = UChromaSDKPluginBPLibrary::CreateColors1D((EChromaSDKDevice1DEnum::Type)device);
		frames.push_back(frame);

		int id = _mAnimationId;
		_mAnimations[id] = animation1D;
		++_mAnimationId;
		return id;
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = new Animation2D();
		animation2D->SetDevice((EChromaSDKDevice2DEnum::Type)device);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		frames.clear();
		FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
		frame.Colors = UChromaSDKPluginBPLibrary::CreateColors2D((EChromaSDKDevice2DEnum::Type)device);
		frames.push_back(frame);

		int id = _mAnimationId;
		_mAnimations[id] = animation2D;
		++_mAnimationId;
		return id;
	}
	break;
	}
	return -1;
}

// LERP
float IChromaSDKPlugin::Lerp(const float start, const float end, const float amt)
{
	return (1 - amt)*start + amt * end;
}

// LERP COLOR
int IChromaSDKPlugin::LerpColor(const int from, const int to, const float t)
{
	int red = floor(Lerp((from & 0xFF), (to & 0xFF), t));
	int green = floor(Lerp((from & 0xFF00) >> 8, (to & 0xFF00) >> 8, t));
	int blue = floor(Lerp((from & 0xFF0000) >> 16, (to & 0xFF0000) >> 16, t));
	int color = red | (green << 8) | (blue << 16);
	return color;
}


// COPY ANIMATION
int IChromaSDKPlugin::CopyAnimation(const int sourceAnimationId, const char* targetAnimation)
{
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return -1;
	}
	CloseAnimationName(targetAnimation);
	int deviceType = sourceAnimation->GetDeviceTypeId();
	int device = sourceAnimation->GetDeviceId();
	int targetAnimationId = CreateAnimationInMemory(deviceType, device);
	if (targetAnimationId < 0)
	{
		return -1;
	}
	switch (sourceAnimation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* sourceAnimation1D = (Animation1D*)sourceAnimation;
		Animation1D* targetAnimation1D = new Animation1D();
		targetAnimation1D->SetDevice(sourceAnimation1D->GetDevice());
		targetAnimation1D->SetName(targetAnimation);
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		targetFrames.clear();
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		for (int frameId = 0; frameId < (int)sourceFrames.size(); ++frameId)
		{
			FChromaSDKColorFrame1D frame = sourceFrames[frameId];
			targetFrames.push_back(frame);
		}

		int id = _mAnimationId;
		_mAnimations[id] = targetAnimation1D;
		++_mAnimationId;
		_mAnimationMapID[targetAnimation] = id;
		return id;
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* sourceAnimation2D = (Animation2D*)sourceAnimation;
		Animation2D* targetAnimation2D = new Animation2D();
		targetAnimation2D->SetDevice(sourceAnimation2D->GetDevice());
		targetAnimation2D->SetName(targetAnimation);
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		targetFrames.clear();
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		for (int frameId = 0; frameId < (int)sourceFrames.size(); ++frameId)
		{
			FChromaSDKColorFrame2D frame = sourceFrames[frameId];
			targetFrames.push_back(frame);
		}

		int id = _mAnimationId;
		_mAnimations[id] = targetAnimation2D;
		++_mAnimationId;
		_mAnimationMapID[targetAnimation] = id;
		return id;
	}
	break;
	}
	return -1;
}
void IChromaSDKPlugin::CopyAnimationName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyAnimationName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	CopyAnimation(sourceAnimationId, targetAnimation);
}


// APPEND ALL FRAMES
void IChromaSDKPlugin::AppendAllFrames(const int sourceAnimationId, const int targetAnimationId)
{
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	StopAnimation(targetAnimationId);
	switch (sourceAnimation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* sourceAnimation1D = (Animation1D*)sourceAnimation;
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		Animation1D* targetAnimation1D = (Animation1D*)targetAnimation;
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		int frameCount = (int)sourceFrames.size();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame1D frame = sourceFrames[frameId];
			targetFrames.push_back(frame);
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* sourceAnimation2D = (Animation2D*)sourceAnimation;
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		Animation2D* targetAnimation2D = (Animation2D*)targetAnimation;
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		int frameCount = (int)sourceFrames.size();
		for (int frameId = 0; frameId < frameCount; ++frameId)
		{
			FChromaSDKColorFrame2D frame = sourceFrames[frameId];
			targetFrames.push_back(frame);
		}
	}
	break;
	}
}
void IChromaSDKPlugin::AppendAllFramesName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AppendAllFramesName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AppendAllFramesName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	AppendAllFrames(sourceAnimationId, targetAnimationId);
}


// INVERT ALL COLORS
void IChromaSDKPlugin::InvertColors(const int animationId, const int frameId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	if (frameId >= 0 && frameId < frameCount)
	{
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				int maxLeds = GetMaxLeds(animation1D->GetDevice());
				TArray<FLinearColor>& colors = frame.Colors;
				for (int i = 0; i < maxLeds; ++i)
				{
					int color = ToBGR(colors[i]);
					int red = 255 - (color & 0xFF);
					int green = 255 - ((color & 0xFF00) >> 8);
					int blue = 255 - ((color & 0xFF0000) >> 16);
					color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
					colors[i] = ToLinearColor(color);
				}
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				int maxRow = GetMaxRow(animation2D->GetDevice());
				int maxColumn = GetMaxColumn(animation2D->GetDevice());
				for (int i = 0; i < maxRow; ++i)
				{
					FChromaSDKColors& row = frame.Colors[i];
					for (int j = 0; j < maxColumn; ++j)
					{
						int color = ToBGR(row.Colors[j]);
						int red = 255 - (color & 0xFF);
						int green = 255 - ((color & 0xFF00) >> 8);
						int blue = 255 - ((color & 0xFF0000) >> 16);
						color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::InvertColorsName(const char* path, const int frameId)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("InvertColorsName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	InvertColors(animationId, frameId);
}

// ALL FRAMES
void IChromaSDKPlugin::InvertColorsAllFrames(const int animationId)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		switch (animation->GetDeviceType())
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
		{
			Animation1D* animation1D = (Animation1D*)(animation);
			vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame1D& frame = frames[frameId];
				int maxLeds = GetMaxLeds(animation1D->GetDevice());
				TArray<FLinearColor>& colors = frame.Colors;
				for (int i = 0; i < maxLeds; ++i)
				{
					int color = ToBGR(colors[i]);
					int red = 255 - (color & 0xFF);
					int green = 255 - ((color & 0xFF00) >> 8);
					int blue = 255 - ((color & 0xFF0000) >> 16);
					color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
					colors[i] = ToLinearColor(color);
				}
			}
		}
		break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
		{
			Animation2D* animation2D = (Animation2D*)(animation);
			vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
			if (frameId >= 0 &&
				frameId < (int)frames.size())
			{
				FChromaSDKColorFrame2D& frame = frames[frameId];
				int maxRow = GetMaxRow(animation2D->GetDevice());
				int maxColumn = GetMaxColumn(animation2D->GetDevice());
				for (int i = 0; i < maxRow; ++i)
				{
					FChromaSDKColors& row = frame.Colors[i];
					for (int j = 0; j < maxColumn; ++j)
					{
						int color = ToBGR(row.Colors[j]);
						int red = 255 - (color & 0xFF);
						int green = 255 - ((color & 0xFF00) >> 8);
						int blue = 255 - ((color & 0xFF0000) >> 16);
						color = (red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16);
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
		break;
		}
	}
}
void IChromaSDKPlugin::InvertColorsAllFramesName(const char* path)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("InvertColorsAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	InvertColorsAllFrames(animationId);
}


// COPY NONZERO TARGET
void IChromaSDKPlugin::CopyNonZeroTargetAllKeys(const int sourceAnimationId, const int targetAnimationId, const int frameId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		if (frameId >= 0 && frameId < int(targetFrames.size()))
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					targetFrame.Colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		if (frameId >= 0 && frameId < int(targetFrames.size()))
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						targetFrame.Colors[i].Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::CopyNonZeroTargetAllKeysName(const char* sourceAnimation, const char* targetAnimation, const int frameId)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroTargetAllKeys(sourceAnimationId, targetAnimationId, frameId);
}


// NONZERO ALL FRAMES
void IChromaSDKPlugin::CopyNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					targetFrame.Colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						targetFrame.Colors[i].Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::CopyNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysAllFramesName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysAllFramesName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroTargetAllKeysAllFrames(sourceAnimationId, targetAnimationId);
}

// ZERO ALL FRAMES
void IChromaSDKPlugin::CopyZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) == 0)
				{
					targetFrame.Colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) == 0)
					{
						targetFrame.Colors[i].Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::CopyZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyZeroTargetAllKeysAllFramesName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyZeroTargetAllKeysAllFramesName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyZeroTargetAllKeysAllFrames(sourceAnimationId, targetAnimationId);
}


// ADD NONZERO TARGET
void IChromaSDKPlugin::AddNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i] = ToLinearColor(newColor);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						int sourceRed = color & 0xFF;
						int sourceGreen = (color & 0xFF00) >> 8;
						int sourceBlue = (color & 0xFF0000) >> 16;

						int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
						int oldRed = oldColor & 0xFF;
						int oldGreen = (oldColor & 0xFF00) >> 8;
						int oldBlue = (oldColor & 0xFF0000) >> 16;

						int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
						int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
						int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
						int newColor = red | (green << 8) | (blue << 16);

						targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::AddNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroTargetAllKeysAllFramesName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroTargetAllKeysAllFramesName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	AddNonZeroTargetAllKeysAllFrames(sourceAnimationId, targetAnimationId);
}


// SUBTRACT NONZERO TARGET ALL FRAMES
void IChromaSDKPlugin::SubtractNonZeroTargetAllKeysAllFrames(const int sourceAnimationId, const int targetAnimationId)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i] = ToLinearColor(newColor);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(targetFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[frameId];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						int sourceRed = color & 0xFF;
						int sourceGreen = (color & 0xFF00) >> 8;
						int sourceBlue = (color & 0xFF0000) >> 16;

						int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
						int oldRed = oldColor & 0xFF;
						int oldGreen = (oldColor & 0xFF00) >> 8;
						int oldBlue = (oldColor & 0xFF0000) >> 16;

						int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
						int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
						int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
						int newColor = red | (green << 8) | (blue << 16);

						targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::SubtractNonZeroTargetAllKeysAllFramesName(const char* sourceAnimation, const char* targetAnimation)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroTargetAllKeysAllFramesName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroTargetAllKeysAllFramesName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	SubtractNonZeroTargetAllKeysAllFrames(sourceAnimationId, targetAnimationId);
}


// COPY NONZERO TARGET ALL FRAMES
void IChromaSDKPlugin::CopyNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					targetFrame.Colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						targetFrame.Colors[i].Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::CopyNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysAllFramesOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("CopyNonZeroTargetAllKeysAllFramesOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	CopyNonZeroTargetAllKeysAllFramesOffset(sourceAnimationId, targetAnimationId, offset);
}


// ADD NONZERO TARGET ALL FRAMES OFFSET
void IChromaSDKPlugin::AddNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i] = ToLinearColor(newColor);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						int sourceRed = color & 0xFF;
						int sourceGreen = (color & 0xFF00) >> 8;
						int sourceBlue = (color & 0xFF0000) >> 16;

						int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
						int oldRed = oldColor & 0xFF;
						int oldGreen = (oldColor & 0xFF00) >> 8;
						int oldBlue = (oldColor & 0xFF0000) >> 16;

						int red = min(255, max(0, oldRed + sourceRed)) & 0xFF;
						int green = min(255, max(0, oldGreen + sourceGreen)) & 0xFF;
						int blue = min(255, max(0, oldBlue + sourceBlue)) & 0xFF;
						int newColor = red | (green << 8) | (blue << 16);

						targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::AddNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroTargetAllKeysAllFramesOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("AddNonZeroTargetAllKeysAllFramesOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	AddNonZeroTargetAllKeysAllFramesOffset(sourceAnimationId, targetAnimationId, offset);
}


// SUBTRACT NONZERO TARGET ALL FRAMES OFFSET
void IChromaSDKPlugin::SubtractNonZeroTargetAllKeysAllFramesOffset(const int sourceAnimationId, const int targetAnimationId, const int offset)
{
	StopAnimation(targetAnimationId);
	AnimationBase* sourceAnimation = GetAnimationInstance(sourceAnimationId);
	if (nullptr == sourceAnimation)
	{
		return;
	}
	AnimationBase* targetAnimation = GetAnimationInstance(targetAnimationId);
	if (nullptr == targetAnimation)
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() != targetAnimation->GetDeviceType() ||
		sourceAnimation->GetDeviceId() != sourceAnimation->GetDeviceId())
	{
		return;
	}
	if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_1D)
	{
		Animation1D* sourceAnimation1D = (Animation1D*)(sourceAnimation);
		Animation1D* targetAnimation1D = (Animation1D*)(targetAnimation);
		vector<FChromaSDKColorFrame1D>& sourceFrames = sourceAnimation1D->GetFrames();
		vector<FChromaSDKColorFrame1D>& targetFrames = targetAnimation1D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxLeds = GetMaxLeds(sourceAnimation1D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame1D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame1D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(sourceFrame.Colors[i]);
				if (color != 0 &&
					ToBGR(targetFrame.Colors[i]) != 0)
				{
					int sourceRed = color & 0xFF;
					int sourceGreen = (color & 0xFF00) >> 8;
					int sourceBlue = (color & 0xFF0000) >> 16;

					int oldColor = ToBGR(targetFrame.Colors[i]);
					int oldRed = oldColor & 0xFF;
					int oldGreen = (oldColor & 0xFF00) >> 8;
					int oldBlue = (oldColor & 0xFF0000) >> 16;

					int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
					int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
					int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
					int newColor = red | (green << 8) | (blue << 16);

					targetFrame.Colors[i] = ToLinearColor(newColor);
				}
			}
		}
	}
	else if (sourceAnimation->GetDeviceType() == EChromaSDKDeviceTypeEnum::DE_2D)
	{
		Animation2D* sourceAnimation2D = (Animation2D*)(sourceAnimation);
		Animation2D* targetAnimation2D = (Animation2D*)(targetAnimation);
		vector<FChromaSDKColorFrame2D>& sourceFrames = sourceAnimation2D->GetFrames();
		vector<FChromaSDKColorFrame2D>& targetFrames = targetAnimation2D->GetFrames();
		if (sourceFrames.size() == 0)
		{
			return;
		}
		if (targetFrames.size() == 0)
		{
			return;
		}
		int maxRow = GetMaxRow(sourceAnimation2D->GetDevice());
		int maxColumn = GetMaxColumn(sourceAnimation2D->GetDevice());
		for (int frameId = 0; frameId < int(sourceFrames.size()); ++frameId)
		{
			FChromaSDKColorFrame2D& sourceFrame = sourceFrames[frameId % sourceFrames.size()];
			FChromaSDKColorFrame2D& targetFrame = targetFrames[(frameId + offset) % targetFrames.size()];
			for (int i = 0; i < maxRow; ++i)
			{
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(sourceFrame.Colors[i].Colors[j]);
					if (color != 0 &&
						ToBGR(targetFrame.Colors[i].Colors[j]) != 0)
					{
						int sourceRed = color & 0xFF;
						int sourceGreen = (color & 0xFF00) >> 8;
						int sourceBlue = (color & 0xFF0000) >> 16;

						int oldColor = ToBGR(targetFrame.Colors[i].Colors[j]);
						int oldRed = oldColor & 0xFF;
						int oldGreen = (oldColor & 0xFF00) >> 8;
						int oldBlue = (oldColor & 0xFF0000) >> 16;

						int red = min(255, max(0, oldRed - sourceRed)) & 0xFF;
						int green = min(255, max(0, oldGreen - sourceGreen)) & 0xFF;
						int blue = min(255, max(0, oldBlue - sourceBlue)) & 0xFF;
						int newColor = red | (green << 8) | (blue << 16);

						targetFrame.Colors[i].Colors[j] = ToLinearColor(newColor);
					}
				}
			}
		}
	}
}
void IChromaSDKPlugin::SubtractNonZeroTargetAllKeysAllFramesOffsetName(const char* sourceAnimation, const char* targetAnimation, const int offset)
{
	int sourceAnimationId = GetAnimation(sourceAnimation);
	if (sourceAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroTargetAllKeysAllFramesOffsetName: Source Animation not found! %s"), *FString(UTF8_TO_TCHAR(sourceAnimation)));
		return;
	}
	int targetAnimationId = GetAnimation(targetAnimation);
	if (targetAnimationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("SubtractNonZeroTargetAllKeysAllFramesOffsetName: Target Animation not found! %s"), *FString(UTF8_TO_TCHAR(targetAnimation)));
		return;
	}
	SubtractNonZeroTargetAllKeysAllFramesOffset(sourceAnimationId, targetAnimationId, offset);
}


// MULTIPLY COLOR LERP ALL FRAMES
void IChromaSDKPlugin::MultiplyColorLerpAllFrames(const int animationId, const int color1, const int color2)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		float t = (frameId + 1) / (float)frameCount;
		int color = LerpColor(color1, color2, t);
		MultiplyIntensityColor(animationId, frameId, color);
	}
}
void IChromaSDKPlugin::MultiplyColorLerpAllFramesName(const char* path, const int color1, const int color2)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyColorLerpAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MultiplyColorLerpAllFrames(animationId, color1, color2);
}


// MULTIPLY TARGET COLOR LERP ALL FRAMES
void IChromaSDKPlugin::MultiplyTargetColorLerpAllFrames(const int animationId, const int color1, const int color2)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		MultiplyTargetColorLerp(animationId, frameId, color1, color2);
	}
}
void IChromaSDKPlugin::MultiplyTargetColorLerpAllFramesName(const char* path, const int color1, const int color2)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyTargetColorLerpAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MultiplyTargetColorLerpAllFrames(animationId, color1, color2);
}


// FILL THRESHOLD RGB COLORS RGB
void IChromaSDKPlugin::FillThresholdRGBColorsRGB(const int animationId, const int frameId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	int color = GetRGB(red, green, blue);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int oldColor = ToBGR(colors[i]);
				int r = oldColor & 0xFF;
				int g = (oldColor & 0xFF00) >> 8;
				int b = (oldColor & 0xFF0000) >> 16;
				if (r != 0 &&
					g != 0 &&
					b != 0 &&
					r <= redThreshold &&
					g <= greenThreshold &&
					b <= blueThreshold) {
					colors[i] = ToLinearColor(color);
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int oldColor = ToBGR(row.Colors[j]);
					int r = oldColor & 0xFF;
					int g = (oldColor & 0xFF00) >> 8;
					int b = (oldColor & 0xFF0000) >> 16;
					if (r != 0 &&
						g != 0 &&
						b != 0 &&
						r <= redThreshold &&
						g <= greenThreshold &&
						b <= blueThreshold) {
						row.Colors[j] = ToLinearColor(color);
					}
				}
			}
		}
	}
	break;
	}
}
void IChromaSDKPlugin::FillThresholdRGBColorsRGBName(const char* path, const int frameId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdRGBColorsRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillThresholdRGBColorsRGB(animationId, frameId, redThreshold, greenThreshold, blueThreshold, red, green, blue);
}


// FILL THRESHOLD RGB COLORS ALL FRAMES RGB
void IChromaSDKPlugin::FillThresholdRGBColorsAllFramesRGB(const int animationId, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		FillThresholdRGBColorsRGB(animationId, frameId, redThreshold, greenThreshold, blueThreshold, red, green, blue);
	}
}
void IChromaSDKPlugin::FillThresholdRGBColorsAllFramesRGBName(const char* path, const int redThreshold, const int greenThreshold, const int blueThreshold, const int red, const int green, const int blue)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("FillThresholdRGBColorsAllFramesRGBName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	FillThresholdRGBColorsAllFramesRGB(animationId, redThreshold, greenThreshold, blueThreshold, red, green, blue);
}


// MULTIPLY TARGET COLOR LERP
void IChromaSDKPlugin::MultiplyTargetColorLerp(const int animationId, const int frameId, const int color1, const int color2)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(colors[i]);
				float red = (color & 0xFF) / 255.0f;
				float green = ((color & 0xFF00) >> 8) / 255.0f;
				float blue = ((color & 0xFF0000) >> 16) / 255.0f;
				float t = (red + green + blue) / 3.0f;
				colors[i] = ToLinearColor(LerpColor(color1, color2, t));
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(row.Colors[j]);
					float red = (color & 0xFF) / 255.0f;
					float green = ((color & 0xFF00) >> 8) / 255.0f;
					float blue = ((color & 0xFF0000) >> 16) / 255.0f;
					float t = (red + green + blue) / 3.0f;
					row.Colors[j] = ToLinearColor(LerpColor(color1, color2, t));
				}
			}
		}
	}
	break;
	}
}

// NONZERO
void IChromaSDKPlugin::MultiplyNonZeroTargetColorLerp(const int animationId, const int frameId, const int color1, const int color2)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	switch (animation->GetDeviceType())
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		Animation1D* animation1D = (Animation1D*)(animation);
		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame1D& frame = frames[frameId];
			int maxLeds = GetMaxLeds(animation1D->GetDevice());
			TArray<FLinearColor>& colors = frame.Colors;
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = ToBGR(colors[i]);
				if (color != 0)
				{
					float red = (color & 0xFF) / 255.0f;
					float green = ((color & 0xFF00) >> 8) / 255.0f;
					float blue = ((color & 0xFF0000) >> 16) / 255.0f;
					float t = (red + green + blue) / 3.0f;
					colors[i] = ToLinearColor(LerpColor(color1, color2, t));
				}
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		Animation2D* animation2D = (Animation2D*)(animation);
		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		if (frameId >= 0 &&
			frameId < (int)frames.size())
		{
			FChromaSDKColorFrame2D& frame = frames[frameId];
			int maxRow = GetMaxRow(animation2D->GetDevice());
			int maxColumn = GetMaxColumn(animation2D->GetDevice());
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors& row = frame.Colors[i];
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = ToBGR(row.Colors[j]);
					if (color != 0)
					{
						float red = (color & 0xFF) / 255.0f;
						float green = ((color & 0xFF00) >> 8) / 255.0f;
						float blue = ((color & 0xFF0000) >> 16) / 255.0f;
						float t = (red + green + blue) / 3.0f;
						row.Colors[j] = ToLinearColor(LerpColor(color1, color2, t));
					}
				}
			}
		}
	}
	break;
	}
}

// ALL FRAMES
void IChromaSDKPlugin::MultiplyNonZeroTargetColorLerpAllFrames(const int animationId, const int color1, const int color2)
{
	StopAnimation(animationId);
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		return;
	}
	int frameCount = animation->GetFrameCount();
	for (int frameId = 0; frameId < frameCount; ++frameId)
	{
		MultiplyNonZeroTargetColorLerp(animationId, frameId, color1, color2);
	}
}
void IChromaSDKPlugin::MultiplyNonZeroTargetColorLerpAllFramesName(const char* path, const int color1, const int color2)
{
	int animationId = GetAnimation(path);
	if (animationId < 0)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("MultiplyNonZeroTargetColorLerpAllFramesName: Animation not found! %s"), *FString(UTF8_TO_TCHAR(path)));
		return;
	}
	MultiplyNonZeroTargetColorLerpAllFrames(animationId, color1, color2);
}

int IChromaSDKPlugin::OpenAnimation(const char* path)
{
	AnimationBase* animation = nullptr;

	//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: %s"), path);

	FILE* stream = nullptr;
	if (0 != fopen_s(&stream, path, "rb") ||
		stream == nullptr)
	{
		//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Failed to open animation! %s"), *FString(UTF8_TO_TCHAR(path)));
		return -1;
	}

	//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Found animation! %s"), *FString(UTF8_TO_TCHAR(path)));

	long read = 0;
	long expectedRead = 1;
	long expectedSize = sizeof(byte);

	//version
	int version = 0;
	expectedSize = sizeof(int);
	read = fread(&version, expectedSize, 1, stream);
	if (read != expectedRead)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Failed to read version!"));
		std::fclose(stream);
		return -1;
	}
	if (version != ANIMATION_VERSION)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Unexpected Version!"));
		std::fclose(stream);
		return -1;
	}

	//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Version: %d"), version);

	//device
	byte device = 0;

	// device type
	byte deviceType = 0;
	expectedSize = sizeof(byte);
	read = fread(&deviceType, expectedSize, 1, stream);
	if (read != expectedRead)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Unexpected DeviceType!"));
		std::fclose(stream);
		return -1;
	}

	//device
	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
		//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: DeviceType: 1D"));
		break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
		//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: DeviceType: 2D"));
		break;
	default:
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Unexpected DeviceType!"));
		std::fclose(stream);
		return -1;
	}

	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
		read = fread(&device, expectedSize, 1, stream);
		if (read != expectedRead)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Unexpected Device!"));
			std::fclose(stream);
			return -1;
		}
		else
		{
			switch ((EChromaSDKDevice1DEnum::Type)device)
			{
			case EChromaSDKDevice1DEnum::DE_ChromaLink:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_ChromaLink"));
				break;
			case EChromaSDKDevice1DEnum::DE_Headset:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_Headset"));
				break;
			case EChromaSDKDevice1DEnum::DE_Mousepad:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_Mousepad"));
				break;
			}

			Animation1D* animation1D = new Animation1D();
			animation = animation1D;

			// device
			animation1D->SetDevice((EChromaSDKDevice1DEnum::Type)device);

			//frame count
			int frameCount;

			expectedSize = sizeof(int);
			read = fread(&frameCount, expectedSize, 1, stream);
			if (read != expectedRead)
			{
				UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading frame count!"));
				delete animation1D;
				std::fclose(stream);
				return -1;
			}
			else
			{
				vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
				for (int index = 0; index < frameCount; ++index)
				{
					FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
					int maxLeds = GetMaxLeds((EChromaSDKDevice1DEnum::Type)device);

					//duration
					float duration = 0.0f;
					expectedSize = sizeof(float);
					read = fread(&duration, expectedSize, 1, stream);
					if (read != expectedRead)
					{
						UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading duration!"));
						delete animation1D;
						std::fclose(stream);
						return -1;
					}
					else
					{
						if (duration < 0.033f)
						{
							duration = 0.033f;
						}
						frame.Duration = duration;

						// colors
						expectedSize = sizeof(int);
						for (int i = 0; i < maxLeds; ++i)
						{
							int color = 0;
							read = fread(&color, expectedSize, 1, stream);
							if (read != expectedRead)
							{
								UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading color!"));
								delete animation1D;
								std::fclose(stream);
								return -1;
							}
							else
							{
								frame.Colors.Add(ToLinearColor(color));
							}
						}
						if (index == 0)
						{
							frames[0] = frame;
						}
						else
						{
							frames.push_back(frame);
						}
					}
				}
			}
		}
		break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
		read = fread(&device, expectedSize, 1, stream);
		if (read != expectedRead)
		{
			UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Unexpected Device!"));
			std::fclose(stream);
			return -1;
		}
		else
		{
			switch ((EChromaSDKDevice2DEnum::Type)device)
			{
			case EChromaSDKDevice2DEnum::DE_Keyboard:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_Keyboard"));
				break;
			case EChromaSDKDevice2DEnum::DE_Keypad:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_Keypad"));
				break;
			case EChromaSDKDevice2DEnum::DE_Mouse:
				//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Device: DE_Mouse"));
				break;
			}

			Animation2D* animation2D = new Animation2D();
			animation = animation2D;

			//device
			animation2D->SetDevice((EChromaSDKDevice2DEnum::Type)device);

			//frame count
			int frameCount;

			expectedSize = sizeof(int);
			read = fread(&frameCount, expectedSize, 1, stream);
			if (read != expectedRead)
			{
				UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading frame count!"));
				delete animation2D;
				std::fclose(stream);
				return -1;
			}
			else
			{
				vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
				for (int index = 0; index < frameCount; ++index)
				{
					FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
					int maxRow = GetMaxRow((EChromaSDKDevice2DEnum::Type)device);
					int maxColumn = GetMaxColumn((EChromaSDKDevice2DEnum::Type)device);

					//duration
					float duration = 0.0f;
					expectedSize = sizeof(float);
					read = fread(&duration, expectedSize, 1, stream);
					if (read != expectedRead)
					{
						UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading duration!"));
						delete animation2D;
						std::fclose(stream);
						return -1;
					}
					else
					{
						if (duration < 0.033f)
						{
							duration = 0.033f;
						}
						frame.Duration = duration;

						// colors
						expectedSize = sizeof(int);
						for (int i = 0; i < maxRow; ++i)
						{
							FChromaSDKColors row = FChromaSDKColors();
							for (int j = 0; j < maxColumn; ++j)
							{
								int color = 0;
								read = fread(&color, expectedSize, 1, stream);
								if (read != expectedRead)
								{
									UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Error detected reading color!"));
									delete animation2D;
									std::fclose(stream);
									return -1;
								}
								else
								{
									row.Colors.Add(ToLinearColor(color));
								}
							}
							frame.Colors.Add(row);
						}
						if (index == 0)
						{
							frames[0] = frame;
						}
						else
						{
							frames.push_back(frame);
						}
					}
				}
			}
		}
		break;
	}

	std::fclose(stream);

	if (animation == nullptr)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimation: Animation is null! name=%s"), *FString(UTF8_TO_TCHAR(path)));
		return -1;
	}

	//UE_LOG(LogChromaPlugin, Log, TEXT("OpenAnimation: Loaded %s"), *FString(UTF8_TO_TCHAR(path)));
	animation->SetName(path);
	int id = _mAnimationId;
	_mAnimations[id] = animation;
	++_mAnimationId;
	_mAnimationMapID[path] = id;
	return id;
}

AnimationBase* IChromaSDKPlugin::OpenAnimationFromMemory(const byte* data)
{
	const byte* pointer = data;
	AnimationBase* animation = nullptr;

	if (0 == data)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimationFromMemory: Failed to open animation!"));
		return nullptr;
	}

	long read = 0;
	long expectedRead = 1;
	long expectedSize = sizeof(byte);

	//version
	int version = 0;
	expectedSize = sizeof(int);
	memcpy(&version, pointer, expectedSize);
	pointer += expectedSize;
	if (version != ANIMATION_VERSION)
	{
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimationFromMemory: Unexpected Version!"));
		return nullptr;
	}

	//LogDebug("OpenAnimationFromMemory: Version: %d\r\n", version);

	//device
	byte device = 0;

	// device type
	byte deviceType = 0;
	expectedSize = sizeof(byte);
	memcpy(&deviceType, pointer, expectedSize);
	pointer += expectedSize;

	//device
	switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
		//LogDebug("OpenAnimation: DeviceType: 1D\r\n");
		break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
		//LogDebug("OpenAnimation: DeviceType: 2D\r\n");
		break;
	default:
		UE_LOG(LogChromaPlugin, Error, TEXT("OpenAnimationFromMemory: Unexpected DeviceType!"));
		return nullptr;
	}

	switch (deviceType)
	{
	case EChromaSDKDeviceTypeEnum::DE_1D:
	{
		expectedSize = sizeof(byte);
		memcpy(&device, pointer, expectedSize);
		pointer += expectedSize;

		switch ((EChromaSDKDevice1DEnum::Type)device)
		{
		case EChromaSDKDevice1DEnum::DE_ChromaLink:
			//LogDebug("OpenAnimation: Device: DE_ChromaLink\r\n");
			break;
		case EChromaSDKDevice1DEnum::DE_Headset:
			//LogDebug("OpenAnimation: Device: DE_Headset\r\n");
			break;
		case EChromaSDKDevice1DEnum::DE_Mousepad:
			//LogDebug("OpenAnimation: Device: DE_Mousepad\r\n");
			break;
		}

		Animation1D* animation1D = new Animation1D();
		animation = animation1D;

		// device
		animation1D->SetDevice((EChromaSDKDevice1DEnum::Type)device);

		//frame count
		int frameCount;

		expectedSize = sizeof(int);
		memcpy(&frameCount, pointer, expectedSize);
		pointer += expectedSize;

		vector<FChromaSDKColorFrame1D>& frames = animation1D->GetFrames();
		for (int index = 0; index < frameCount; ++index)
		{
			FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
			int maxLeds = GetMaxLeds((EChromaSDKDevice1DEnum::Type)device);

			//duration
			float duration = 0.0f;
			expectedSize = sizeof(float);
			memcpy(&duration, pointer, expectedSize);
			pointer += expectedSize;

			if (duration >= 0.033f)
			{
				frame.Duration = duration;
			}
			else
			{
				frame.Duration = 0.033f;
			}

			// colors
			expectedSize = sizeof(int);
			for (int i = 0; i < maxLeds; ++i)
			{
				int color = 0;
				memcpy(&color, pointer, expectedSize);
				pointer += expectedSize;

				frame.Colors.Add(ToLinearColor(color));
			}
			if (index == 0)
			{
				frames[0] = frame;
			}
			else
			{
				frames.push_back(frame);
			}
		}
	}
	break;
	case EChromaSDKDeviceTypeEnum::DE_2D:
	{
		expectedSize = sizeof(byte);
		memcpy(&device, pointer, expectedSize);
		pointer += expectedSize;

		switch ((EChromaSDKDevice2DEnum::Type)device)
		{
		case EChromaSDKDevice2DEnum::DE_Keyboard:
			//LogDebug("OpenAnimation: Device: DE_Keyboard\r\n");
			break;
		case EChromaSDKDevice2DEnum::DE_Keypad:
			//LogDebug("OpenAnimation: Device: DE_Keypad\r\n");
			break;
		case EChromaSDKDevice2DEnum::DE_Mouse:
			//LogDebug("OpenAnimation: Device: DE_Mouse\r\n");
			break;
		}

		Animation2D* animation2D = new Animation2D();
		animation = animation2D;

		//device
		animation2D->SetDevice((EChromaSDKDevice2DEnum::Type)device);

		//frame count
		int frameCount;

		expectedSize = sizeof(int);
		memcpy(&frameCount, pointer, expectedSize);
		pointer += expectedSize;

		vector<FChromaSDKColorFrame2D>& frames = animation2D->GetFrames();
		for (int index = 0; index < frameCount; ++index)
		{
			FChromaSDKColorFrame2D frame = FChromaSDKColorFrame2D();
			int maxRow = GetMaxRow((EChromaSDKDevice2DEnum::Type)device);
			int maxColumn = GetMaxColumn((EChromaSDKDevice2DEnum::Type)device);

			//duration
			float duration = 0.0f;
			expectedSize = sizeof(float);
			memcpy(&duration, pointer, expectedSize);
			pointer += expectedSize;

			if (duration >= 0.033f)
			{
				frame.Duration = duration;
			}
			else
			{
				frame.Duration = 0.033f;
			}

			// colors
			expectedSize = sizeof(int);
			for (int i = 0; i < maxRow; ++i)
			{
				FChromaSDKColors row = FChromaSDKColors();
				for (int j = 0; j < maxColumn; ++j)
				{
					int color = 0;
					memcpy(&color, pointer, expectedSize);
					pointer += expectedSize;

					row.Colors.Add(ToLinearColor(color));
				}
				frame.Colors.Add(row);
			}
			if (index == 0)
			{
				frames[0] = frame;
			}
			else
			{
				frames.push_back(frame);
			}
		}
	}
	break;
	}

	//LogDebug("OpenAnimationFromMemory: Loaded %s\r\n", path.c_str());

	return animation;
}

void IChromaSDKPlugin::UseIdleAnimation(EChromaSDKDeviceEnum::Type device, const bool flag)
{
	if (ChromaThread::Instance())
	{
		ChromaThread::Instance()->UseIdleAnimation(device, flag);
	}
}

void IChromaSDKPlugin::UseIdleAnimationAll(const bool flag)
{
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_ChromaLink, flag);
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_Headset, flag);
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_Keyboard, flag);
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_Keypad, flag);
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_Mouse, flag);
	UseIdleAnimation(EChromaSDKDeviceEnum::DE_Mousepad, flag);
}

void IChromaSDKPlugin::SetIdleAnimationName(const char* path)
{
	LoadAnimationName(path);
	if (ChromaThread::Instance())
	{
		ChromaThread::Instance()->SetIdleAnimationName(path);
	}
}

void IChromaSDKPlugin::UsePreloading(const int animationId, const bool flag)
{
	AnimationBase* animation = GetAnimationInstance(animationId);
	if (nullptr == animation)
	{
		//LogError("UsePreloading: Animation is null! id=%d\r\n", animationId);
		return;
	}

	animation->UsePreloading(flag);
}

void IChromaSDKPlugin::UsePreloadingName(const char* path, const bool flag)
{
	AnimationBase* animation = GetAnimationInstanceName(path);
	if (nullptr == animation)
	{
		//LogError("UsePreloadingName: Animation is null! %s\r\n", path);
		return;
	}

	animation->UsePreloading(flag);
}


// VALIDATE DLL METHODS

bool IChromaSDKPlugin::ValidateGetProcAddress(bool condition, FString methodName)
{
	if (condition)
	{
		UE_LOG(LogChromaPlugin, Warning, TEXT("ChromaSDKPlugin failed to load %s!"), *methodName);
	}
	else
	{
		//UE_LOG(LogChromaPlugin, Log, TEXT("ChromaSDKPlugin loaded %s."), *methodName);
	}
	return condition;
}

#include "Windows/HideWindowsPlatformTypes.h"

#endif
