## Table of contents

* [See Also](#see-also)
* [Overview](#overview)
* [Tutorials](#tutorials)
* [Supported versions](#supported-versions)
* [Dependencies](#dependencies)
* [Plugin Structure](#plugin-structure)
* [Samples](#samples)
* [API](#api)

## See Also

**Apps:**

- [ChromaClientForDiscord](https://github.com/tgraupmann/ChromaDiscordApp) - Add Chroma lighting to the Discord App events

- [ChromaClientForMixer](https://github.com/tgraupmann/ChromaClientForMixer) - Add Chroma lighting to the Mixer streaming experience

- [ChromaClientForTwitch](https://github.com/tgraupmann/ChromaTwitchExtension) - Add Chroma lighting to the Twitch streaming experience

**Plugins:**

- [CChromaEditor](https://github.com/RazerOfficial/CChromaEditor) - C++ native MFC library for playing and editing Chroma animations

- [GameMakerChromaExtension](https://github.com/RazerOfficial/GameMakerChromaExtension) - GameMaker extension to control lighting for Razer Chroma

- [HTML5ChromaSDK](https://github.com/RazerOfficial/HTML5ChromaSDK) - JavaScript library for playing Chroma animations

- [UE4_XDK_SampleApp](https://github.com/RazerOfficial/UE4_XDK_SampleApp) - UE4 runtime module with Blueprint library for the ChromaSDK

- [UnityNativeChromaSDK](https://github.com/RazerOfficial/UnityNativeChromaSDK) - Unity native library for the ChromaSDK

## Overview

`UE4ChromaSDKRT` provides a runtime module for using the `ChromaSDK` (requires UE 4.5.1 or better). The runtime module provides a blueprint library and C++ methods for playing Chroma animations. See the [Chroma Guide](http://chroma.razer.com/ChromaGuide/) for details on how to make visually interesting Chroma animations using the plugin API.

## Tutorials

### Videos


**Chroma Design Guide**

<a target="_blank" href="https://www.youtube.com/watch?v=-demSB_xLTc"><img src="https://img.youtube.com/vi/-demSB_xLTc/0.jpg"/></a>

**UE4ChromaSDKRT Plugin Setup** for your specific version of UE4.

<a target="_blank" href="https://www.youtube.com/watch?v=7buO9YCXjtY"><img src="https://img.youtube.com/vi/7buO9YCXjtY/0.jpg"/></a>

**UE4ChromaSDKRT Overview**

<a target="_blank" href="https://www.youtube.com/watch?v=-Q6Dn15m1Jg"><img src="https://img.youtube.com/vi/-Q6Dn15m1Jg/0.jpg"/></a>

**Simplify UI Blueprints**

<a target="_blank" href="https://www.youtube.com/watch?v=Rz-mOXvd2a0"><img src="https://img.youtube.com/vi/Rz-mOXvd2a0/0.jpg"/></a>

## Supported versions

This project is checked in with engine 4.21 and supports later versions. The scripts at [UE4ChromaSDKRT_Versions](https://github.com/razerofficial/UE4ChromaSDKRT_Versions) can convert to earlier versions.

## Plugin Structure

```
UE4ChromaSDKRT/Plugins/
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/ChromaSDKPlugin.uplugin
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Resources
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Resources/Icon128.png
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/ChromaSDKPlugin.Build.cs
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/Animation1D.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/Animation2D.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/AnimationBase.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/ChromaSDKPlugin.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/UE4ChromaSDKRT/ChromaSDKPluginAnimation1DObject.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/UE4ChromaSDKRT/ChromaSDKPluginAnimation2DObject.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/ChromaSDKPluginBPLibrary.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/ChromaSDKPluginPrivatePCH.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/ChromaSDKPluginTypes.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Private/ChromaThread.cpp
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/Animation1D.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/Animation2D.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/AnimationBase.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/ChromaSDKPluginAnimation1DObject.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/ChromaSDKPluginAnimation2DObject.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/ChromaSDKPluginBPLibrary.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/ChromaSDKPluginTypes.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/ChromaThread.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/IChromaSDKPlugin.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/RzChromaSDKDefines.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/RzChromaSDKTypes.h
UE4ChromaSDKRT/Plugins/ChromaSDKPlugin/Source/ChromaSDKPlugin/Public/RzErrors.h
```

## Plugin appears in Window->Plugins
![image_1](images/image_2.png)


## Samples

**Animation Sample**

* The animation sample showcases invoking the ChromaSDK `Blueprint Library` from the C++ [Sample BP Library](Source/UE4ChromaSDKRT/SampleBPLibrary.cpp).

![image_3](images/image_3.png)

* Event `BeginPlay` invokes the setup for UI and buttons, and finishes invoking the `SampleStart` BP function.

![image_4](images/image_4.png)

* Event `EndPlay` ends the application by invoking the `SampleEnd` BP function.

![image_5](images/image_5.png)

* The `SetupUI` custom event loads the sample BP Widget and adds the stored widget var to the viewport.

![image_6](images/image_6.png)

* The `AnimationWidget_BP` UI Widget defines the all the button variable names.

![image_8](images/image_8.png)

* The `SetupButtons` custom event manually uses the `Widget` variable to add a click handler for each UI Widget button accessor which invokes the corresponding BP sample function.

![image_7](images/image_7.png)

* The animation sample `Chroma` files are found in the project's content folder. There's a set of `Blank`, `Fire`, and `Random` Chroma animation files. After building the Windows standalone application, the `Chroma` files should be copied to the compiled application content folder.

## API

`UE4ChromaSDKRT` is a Blueprint API library with more methods than the older [UE4ChromaSDK](https://github.com/RazerOfficial/UE4ChromaSDK#api) Blueprint library with some exceptions to be compatible with earlier versions of UE4.

* Enum syntax - Enums are namespaced and types use the `EChromaSDKKeyboardKey::Type` syntax to avoid collisions.

* No const enum types or passing enums by reference in function parameters - Avoid use of const enum types because that seems to crash in UE 4.5.

* [CopyKeyColorName](#CopyKeyColorName)
* [CopyKeysColorName](#CopyKeysColorName)
* [CopyKeysColorAllFramesName](#CopyKeysColorAllFramesName)
* [CopyAllKeysAllFramesName](#CopyAllKeysAllFramesName)
* [CopyNonZeroKeyColorName](#CopyNonZeroKeyColorName)
* [CopyNonZeroKeysColorName](#CopyNonZeroKeysColorName)
* [CopyNonZeroKeysColorAllFramesName](#CopyNonZeroKeysColorAllFramesName)
* [CopyNonZeroAllKeysAllFramesName](#CopyNonZeroAllKeysAllFramesName)
* [GetKeyColorName](#GetKeyColorName)
* [FillColorName](#FillColorName)
* [FillColorAllFramesName](#FillColorAllFramesName)
* [FillNonZeroColorName](#FillNonZeroColorName)
* [FillNonZeroColorAllFramesName](#FillNonZeroColorAllFramesName)
* [MultiplyIntensityName](#MultiplyIntensityName)
* [MultiplyIntensityAllFramesName](#MultiplyIntensityAllFramesName)
* [OffsetColorsName](#OffsetColorsName)
* [OffsetColorsAllFramesName](#OffsetColorsAllFramesName)
* [OffsetNonZeroColorsName](#OffsetNonZeroColorsName)
* [OffsetNonZeroColorsAllFramesName](#OffsetNonZeroColorsAllFramesName)
* [SetKeyColorName](#SetKeyColorName)
* [SetKeysColorName](#SetKeysColorName)
* [SetKeyColorAllFramesName](#SetKeyColorAllFramesName)
* [SetKeysColorAllFramesName](#SetKeysColorAllFramesName)
* [SetKeyNonZeroColorName](#SetKeyNonZeroColorName)
* [SetKeysNonZeroColorName](#SetKeysNonZeroColorName)
* [SetKeysNonZeroColorAllFramesName](#SetKeysNonZeroColorAllFramesName)

---
<a name="CopyKeyColorName"></a>
**CopyKeyColorName**

Copy color from a source animation to a target animation for a key
```c++
void UChromaSDKPluginBPLibrary::CopyKeyColorName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const int frameIndex, EChromaSDKKeyboardKey::Type key)
```
---
<a name="CopyKeysColorName"></a>
**CopyKeysColorName**

Copy color from a source animation to a target animation for a set of keys
```c++
void UChromaSDKPluginBPLibrary::CopyKeysColorName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const int frameIndex, const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys)
```
---
<a name="CopyKeysColorAllFramesName"></a>
**CopyKeysColorAllFramesName**

Copy color from a source animation to a target animation for a set of keys for all frames
```c++
void UChromaSDKPluginBPLibrary::CopyKeysColorAllFramesName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys)
```
---
<a name="CopyAllKeysAllFramesName"></a>
**CopyAllKeysAllFramesName**

Copy colors from a source animation to a target animation for all keys and all frames
```c++
void UChromaSDKPluginBPLibrary::CopyAllKeysAllFramesName(const FString& sourceAnimationName,
  const FString& targetAnimationName)
```
---
<a name="CopyNonZeroAllKeysAllFramesName"></a>
**CopyNonZeroAllKeysAllFramesName**

Copy nonzero colors from a source animation to a target animation for all keys and all frames
```c++
void UChromaSDKPluginBPLibrary::CopyNonZeroAllKeysAllFramesName(const FString& sourceAnimationName,
  const FString& targetAnimationName)
```
---
<a name="CopyNonZeroKeyColorName"></a>
**CopyNonZeroKeyColorName**

Copy nonzero color from a source animation to a target animation for a key
```c++
void UChromaSDKPluginBPLibrary::CopyNonZeroKeyColorName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const int frameIndex, EChromaSDKKeyboardKey::Type key)
```
---
<a name="CopyNonZeroKeysColorName"></a>
**CopyNonZeroKeysColorName**

Copy nonzero color from a source animation to a target animation for a set of keys
```c++
void UChromaSDKPluginBPLibrary::CopyNonZeroKeysColorName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const int frameIndex, const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys)
```
---
<a name="CopyNonZeroKeysColorAllFramesName"></a>
**CopyNonZeroKeysColorAllFramesName**

Copy nonzero color from a source animation to a target animation for a set of keys for all frames
```c++
void UChromaSDKPluginBPLibrary::CopyNonZeroKeysColorAllFramesName(const FString& sourceAnimationName,
  const FString& targetAnimationName,
  const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys)
```
---
<a name="GetKeyColorName"></a>
**GetKeyColorName**

Get the color from the animation frame's key

```c++
FLinearColor UChromaSDKPluginBPLibrary::GetKeyColorName(const FString& animationName,
  const int frameIndex, EChromaSDKKeyboardKey::Type key)
```
---
<a name="FillColorName"></a>
**FillColorName**

Set the RGB value for all colors in the specified frame
```c++
void UChromaSDKPluginBPLibrary::FillColorName(const FString& animationName,
  int frameId, int red, int green, int blue)
```
---
<a name="FillColorAllFramesName"></a>
**FillColorAllFramesName**

Set the RGB value for all colors for all frames
```c++
void UChromaSDKPluginBPLibrary::FillColorAllFramesName(const FString& animationName,
  int red, int green, int blue)
```
---
<a name="FillNonZeroColorName"></a>
**FillNonZeroColorName**

Set the RGB value for all colors in the specified frame if the destination color is not black
```c++
void UChromaSDKPluginBPLibrary::FillNonZeroColorName(const FString& animationName,
  int frameId, int red, int green, int blue)
```
---
<a name="FillNonZeroColorAllFramesName"></a>
**FillNonZeroColorAllFramesName**

Set the RGB value for all colors for all frames if the destination color is not black
```c++
void UChromaSDKPluginBPLibrary::FillNonZeroColorAllFramesName(const FString& animationName,
  int red, int green, int blue)
```
---
<a name="MultiplyIntensityName"></a>
**MultiplyIntensityName**

Multiply all the colors in the frame by the intensity value
```c++
void UChromaSDKPluginBPLibrary::MultiplyIntensityName(const FString& animationName,
  int frameId, float intensity)

```
---
<a name="MultiplyIntensityAllFramesName"></a>
**MultiplyIntensityAllFramesName**

Multiply all the colors for all frames by the intensity value
```c++
void UChromaSDKPluginBPLibrary::MultiplyIntensityAllFramesName(const FString& animationName,
  float intensity)

```
---
<a name="OffsetColorsName"></a>
**OffsetColorsName**

Offset all colors in the frame using the RGB offset
```c++
void UChromaSDKPluginBPLibrary::OffsetColorsName(const FString& animationName,
  int frameId, int red, int green, int blue)

```
---
<a name="OffsetColorsAllFramesName"></a>
**OffsetColorsAllFramesName**

Offset all colors for all frames using the RGB offset
```c++
void UChromaSDKPluginBPLibrary::OffsetColorsAllFramesName(const FString& animationName,
  int red, int green, int blue)

```
---
<a name="OffsetNonZeroColorsName"></a>
**OffsetNonZeroColorsName**

Offset all colors in the frame using the RGB offset if the destination color is not black
```c++
void UChromaSDKPluginBPLibrary::OffsetNonZeroColorsName(const FString& animationName,
  int frameId, int red, int green, int blue)
```
---
<a name="OffsetNonZeroColorsAllFramesName"></a>
**OffsetNonZeroColorsAllFramesName**

Offset all colors for all frames using the RGB offset if the destination color is not black
```c++
void UChromaSDKPluginBPLibrary::OffsetNonZeroColorsAllFramesName(const FString& animationName,
  int red, int green, int blue)
```
---
<a name="SetKeyColorName"></a>
**SetKeyColorName**

Set the animation frame's key to the supplied color

```c++
void UChromaSDKPluginBPLibrary::SetKeyColorName(const FString& animationName,
    const int frameIndex, const EChromaSDKKeyboardKey& key, const FLinearColor& color);
```
---
<a name="SetKeysColorName"></a>
**SetKeysColorName**

Set the animation frame to the supplied color for a set of keys
```c++
void UChromaSDKPluginBPLibrary::SetKeysColorName(const FString& animationName,
    const int frameIndex, const TArray<EChromaSDKKeyboardKey>& keys, const FLinearColor& color);
```
---
<a name="SetKeyColorAllFramesName"></a>
**SetKeyColorAllFramesName**

Set the key to the supplied color for all animation frames
```c++
void UChromaSDKPluginBPLibrary::SetKeyColorAllFramesName(const FString& animationName,
    const EChromaSDKKeyboardKey& key, const FLinearColor& color);
```
---
<a name="SetKeysColorAllFramesName"></a>
**SetKeysColorAllFramesName**

Set the keys to the supplied color for all animation frames
```c++
void UChromaSDKPluginBPLibrary::SetKeysColorAllFramesName(const FString& animationName,
    const TArray<EChromaSDKKeyboardKey>& keys, const FLinearColor& color);
```
---
<a name="SetKeyNonZeroColorName"></a>
**SetKeyNonZeroColorName**

Set the animation frame's key to the supplied color if the key is not already black
```c++
void UChromaSDKPluginBPLibrary::SetKeyNonZeroColorName(const FString& animationName,
  const int frameIndex, EChromaSDKKeyboardKey::Type key, const FLinearColor& colorParam)
```
---
<a name="SetKeysNonZeroColorName"></a>
**SetKeysNonZeroColorName**

Set the animation frame to the supplied color for a set of keys if the key is not already black
```c++
void UChromaSDKPluginBPLibrary::SetKeysNonZeroColorName(const FString& animationName,
  const int frameIndex,
  const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys,
  const FLinearColor& colorParam)
```
---
<a name="SetKeysNonZeroColorAllFramesName"></a>
**SetKeysNonZeroColorAllFramesName**

Set the keys to the supplied color for all animation frames if the key is not already black
```c++
void UChromaSDKPluginBPLibrary::SetKeysNonZeroColorAllFramesName(const FString& animationName,
  const TArray<TEnumAsByte<EChromaSDKKeyboardKey::Type>>& keys, const FLinearColor& colorParam)
```
---
