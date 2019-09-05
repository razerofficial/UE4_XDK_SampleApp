// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE4ChromaSDKRTTarget : TargetRules
{
//	public UE4ChromaSDKRTTarget(TargetInfo Target) //___HACK_UE4_VERSION_4_15_OR_LESS
    public UE4ChromaSDKRTTarget(TargetInfo Target) : base(Target) //___HACK_UE4_VERSION_4_16_OR_GREATER
    {
		Type = TargetType.Game;

        ExtraModuleNames.AddRange(new string[] { "UE4ChromaSDKRT" }); //___HACK_UE4_VERSION_4_16_OR_GREATER
    }

	//
	// TargetRules interface.
	//

//	public override void SetupBinaries( //___HACK_UE4_VERSION_4_15_OR_LESS
//        TargetInfo Target, //___HACK_UE4_VERSION_4_15_OR_LESS
//        ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations, //___HACK_UE4_VERSION_4_15_OR_LESS
//        ref List<string> OutExtraModuleNames //___HACK_UE4_VERSION_4_15_OR_LESS
//        ) //___HACK_UE4_VERSION_4_15_OR_LESS
//    { //___HACK_UE4_VERSION_4_15_OR_LESS
//        OutExtraModuleNames.AddRange( new string[] { "UE4ChromaSDKRT" } ); //___HACK_UE4_VERSION_4_15_OR_LESS
//    } //___HACK_UE4_VERSION_4_15_OR_LESS
}
