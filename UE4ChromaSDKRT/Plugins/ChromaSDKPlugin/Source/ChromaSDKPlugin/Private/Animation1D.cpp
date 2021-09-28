#include "Animation1D.h" //___HACK_UE4_VERSION_4_16_OR_GREATER
#include "ChromaSDKPluginPrivatePCH.h"
//#include "Animation1D.h" //___HACK_UE4_VERSION_4_15_OR_LESS
#include "ChromaSDKPluginBPLibrary.h"
#include "ChromaThread.h"

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE || PLATFORM_SWITCH

#include "Windows/AllowWindowsPlatformTypes.h" 

typedef unsigned char byte;
#define ANIMATION_VERSION 1

using namespace ChromaSDK;
using namespace std;

Animation1D::Animation1D()
{
	//default device
	_mDevice = EChromaSDKDevice1DEnum::DE_ChromaLink;
	Reset();
}

Animation1D::~Animation1D()
{

}

void Animation1D::Reset()
{
	_mFrames.clear();
	FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
	frame.Colors = UChromaSDKPluginBPLibrary::CreateColors1D(_mDevice);
	_mFrames.push_back(frame);

	_mIsPlaying = false;
	_mIsLoaded = false;
	_mTime = 0.0f;
	_mCurrentFrame = 0;
	_mLoop = false;
}

EChromaSDKDeviceTypeEnum::Type Animation1D::GetDeviceType()
{
	return EChromaSDKDeviceTypeEnum::DE_1D;
}

EChromaSDKDevice1DEnum::Type Animation1D::GetDevice()
{
	return _mDevice;
}

bool Animation1D::SetDevice(EChromaSDKDevice1DEnum::Type device)
{
	if (_mDevice != device)
	{
		_mDevice = device;
		Reset();
		return true;
	}
	else
	{
		return false;
	}
}

int Animation1D::GetDeviceId()
{
	return (int)GetDevice();
}

vector<FChromaSDKColorFrame1D>& Animation1D::GetFrames()
{
	return _mFrames;
}

int Animation1D::GetFrameCount()
{
	return _mFrames.size();
}

float Animation1D::GetDuration(unsigned int index)
{
	if (index < _mFrames.size())
	{
		FChromaSDKColorFrame1D& frame = _mFrames[index];
		return frame.Duration;
	}
	return 0.0f;
}

void Animation1D::Load()
{
	if (_mIsLoaded)
	{
		return;
	}

	for (unsigned int i = 0; i < _mFrames.size(); ++i)
	{
		FChromaSDKColorFrame1D& frame = _mFrames[i];
		FChromaSDKEffectResult effect = UChromaSDKPluginBPLibrary::ChromaSDKCreateEffectCustom1D(_mDevice, frame.Colors);
		if (effect.Result != 0)
		{
			fprintf(stderr, "Load: Failed to create effect!\r\n");
		}
		_mEffects.push_back(effect);
	}

	_mIsLoaded = true;
}

void Animation1D::Unload()
{
	if (!_mIsLoaded)
	{
		return;
	}

	for (unsigned int i = 0; i < _mEffects.size(); ++i)
	{
		FChromaSDKEffectResult& effect = _mEffects[i];
		int result = UChromaSDKPluginBPLibrary::ChromaSDKDeleteEffect(effect.EffectId);
		if (result != 0)
		{
			fprintf(stderr, "Unload: Failed to delete effect!\r\n");
		}
	}
	_mEffects.clear();
	_mIsLoaded = false;
}

void Animation1D::Play(bool loop)
{
	_mTime = 0.0f;
	_mCurrentFrame = -1;
	_mIsPlaying = true;
	_mLoop = loop;
	_mIsPaused = false;

	if (ChromaThread::Instance())
	{
		ChromaThread::Instance()->AddAnimation(this);
	}
}

void Animation1D::Pause()
{
	_mIsPaused = true;
}

void Animation1D::Resume(bool loop)
{
	_mIsPaused = false;
	_mLoop = loop;
}

void Animation1D::Stop()
{
	_mIsPlaying = false;

	if (ChromaThread::Instance())
	{
		ChromaThread::Instance()->RemoveAnimation(this);
	}
}

void Animation1D::Update(float deltaTime)
{
	if (_mIsPaused)
	{
		return;
	}

	if (!_mIsPlaying)
	{
		return;
	}

	InternalUpdate(deltaTime);
}

void Animation1D::InternalShowFrame()
{
	if (_mUsePreloading)
	{
		// use cached effects
		if (_mCurrentFrame >= 0 && _mCurrentFrame < (int)_mEffects.size())
		{
			FChromaSDKEffectResult& effect = _mEffects[_mCurrentFrame];
			int result = UChromaSDKPluginBPLibrary::ChromaSDKSetEffect(effect.EffectId);
			if (result != 0)
			{
				fprintf(stderr, "InternalShowFrame: Failed to set effect!\r\n");
			}
		}
	}
	else
	{
		//immediate mode
		if (_mCurrentFrame >= 0 && _mCurrentFrame < (int)_mFrames.size())
		{
			FChromaSDKColorFrame1D& frame = _mFrames[_mCurrentFrame];
			FChromaSDKEffectResult effect = UChromaSDKPluginBPLibrary::ChromaSDKCreateEffectCustom1D(_mDevice, frame.Colors);
			if (effect.Result == RZRESULT_SUCCESS)
			{
				UChromaSDKPluginBPLibrary::ChromaSDKSetEffect(effect.EffectId);
				UChromaSDKPluginBPLibrary::ChromaSDKDeleteEffect(effect.EffectId);
			}
		}
	}
}

void Animation1D::InternalUpdate(float deltaTime)
{
	if (_mCurrentFrame == -1)
	{
		_mCurrentFrame = 0;
		InternalShowFrame();
	}
	else
	{
		_mTime += deltaTime;
		float nextTime = GetDuration(_mCurrentFrame);
		if (nextTime < _mTime)
		{
			_mTime = 0.0f;
			if ((_mCurrentFrame + 1) < (int)_mFrames.size())
			{
				++_mCurrentFrame;
				InternalShowFrame();
			}
			else
			{
				if (_mLoop)
				{
					_mTime = 0.0f;
					_mCurrentFrame = -1;
				}
				else
				{
					//fprintf(stdout, "Update: Animation Complete.\r\n");
					_mIsPlaying = false;
					_mTime = 0.0f;
					_mCurrentFrame = 0;
				}
			}
		}
	}
}

void Animation1D::ResetFrames()
{
	_mCurrentFrame = 0;
	while (_mFrames.size() > 0)
	{
		auto it = _mFrames.begin();
		_mFrames.erase(it);
	}
	FChromaSDKColorFrame1D frame = FChromaSDKColorFrame1D();
	frame.Colors = UChromaSDKPluginBPLibrary::CreateColors1D(_mDevice);
	frame.Duration = 1;
	_mFrames.push_back(frame);
}

int Animation1D::Save(const char* path)
{
	FILE* stream;
	int result = fopen_s(&stream, path, "wb");
	if (result == 13)
	{
		fprintf(stderr, "Save: Permission denied! %s\r\n", path);
		return -1;
	}
	else if (0 == result &&
		stream)
	{
		long write = 0;
		long expectedWrite = 1;
		long expectedSize = 0;

		int version = ANIMATION_VERSION;
		expectedSize = sizeof(int);
		write = fwrite(&version, expectedSize, 1, stream);
		if (expectedWrite != write)
		{
			fprintf(stderr, "Save: Failed to write version!\r\n");
			std::fclose(stream);
			return -1;
		}

		//device type
		byte deviceType = (byte)EChromaSDKDeviceTypeEnum::DE_1D;
		expectedSize = sizeof(byte);
		fwrite(&deviceType, expectedSize, 1, stream);

		switch ((EChromaSDKDeviceTypeEnum::Type)deviceType)
		{
		case EChromaSDKDeviceTypeEnum::DE_1D:
			UE_LOG(LogTemp, Log, TEXT("Save: DeviceType: 1D\r\n"));
			break;
		case EChromaSDKDeviceTypeEnum::DE_2D:
			UE_LOG(LogTemp, Log, TEXT("Save: DeviceType: 2D\r\n"));
			break;
		}

		//device
		byte device = (byte)_mDevice;
		fwrite(&device, expectedSize, 1, stream);

		switch ((EChromaSDKDevice1DEnum::Type)device)
		{
		case EChromaSDKDevice1DEnum::DE_ChromaLink:
			UE_LOG(LogTemp, Log, TEXT("Save: Device: DE_ChromaLink\r\n"));
			break;
		case EChromaSDKDevice1DEnum::DE_Headset:
			UE_LOG(LogTemp, Log, TEXT("Save: Device: DE_Headset\r\n"));
			break;
		case EChromaSDKDevice1DEnum::DE_Mousepad:
			UE_LOG(LogTemp, Log, TEXT("Save: Device: DE_Mousepad\r\n"));
			break;
		}

		//frame count
		unsigned int frameCount = GetFrameCount();
		expectedSize = sizeof(unsigned int);
		fwrite(&frameCount, expectedSize, 1, stream);

		UE_LOG(LogTemp, Log, TEXT("Save: FrameCount: %d\r\n"), frameCount);

		//frames
		float duration = 0.0f;
		COLORREF color = RGB(0, 0, 0);
		for (unsigned int index = 0; index < frameCount; ++index)
		{
			//duration
			duration = GetDuration(index);
			expectedSize = sizeof(float);
			fwrite(&duration, expectedSize, 1, stream);

			//colors
			if (index < _mFrames.size())
			{
				FChromaSDKColorFrame1D& frame = _mFrames[index];
				for (int i = 0; i < frame.Colors.Num(); ++i)
				{
					//color
					color = IChromaSDKPlugin::ToBGR(frame.Colors[i]);
					expectedSize = sizeof(int);
					fwrite(&color, expectedSize, 1, stream);
				}
			}
		}

		fflush(stream);
		std::fclose(stream);

		return 0;
	}

	return -1;
}

#include "Windows/HideWindowsPlatformTypes.h"

#endif
