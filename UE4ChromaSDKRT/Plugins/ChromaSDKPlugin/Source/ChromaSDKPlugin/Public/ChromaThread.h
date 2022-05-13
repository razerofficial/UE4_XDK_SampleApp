#pragma once

#include "AnimationBase.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
#include "Windows/AllowWindowsPlatformTypes.h" 
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "Windows/HideWindowsPlatformTypes.h"

namespace ChromaSDK
{
	class ChromaThread
	{
	public:
		static ChromaThread* Instance();
		static void Init();
		void Start();
		void Stop();
		void AddAnimation(AnimationBase* animation);
		void RemoveAnimation(AnimationBase* animation);
		void DeleteAnimation(AnimationBase* animation);
		int GetAnimationCount();
		int GetAnimationId(int index);
		void UseIdleAnimation(EChromaSDKDeviceEnum::Type device, bool flag);
		void SetIdleAnimationName(const char* name);
	private:
		ChromaThread();
		void ProcessAnimations(float deltaTime);
		void ChromaWorker();
		static ChromaThread* _sInstance;
		static std::mutex _sMutex;
		static bool _sWaitForExit;
		static std::thread* _sThread;
		static std::vector<AnimationBase*> _sAnimations;
		static std::vector<bool> _sUseIdleAnimation;
		static std::vector<std::string> _sIdleAnimation;
	};
}

#endif
