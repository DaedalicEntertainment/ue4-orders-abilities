// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class OrdersAbilities : ModuleRules
{
	public OrdersAbilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",

                "AIModule",
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks"
            });
	}
}
