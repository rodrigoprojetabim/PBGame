// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjetaBimPluginBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UProjetaBimPluginBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** @param bAbsolute if true, path is absolute, otherwise, it's relative to /Content */
	UFUNCTION(BlueprintPure, Category = "Image", meta = (Keywords = "image png jpg jpeg bmp ico icon bitmap exr icns"))
	static UTexture2D* LoadTexture2DFromFile(const FString& FilePath, bool& IsValid, int32& Width, int32& Height, bool bAbsolute = true);
	
	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", meta = (Keywords = "order"))
	static void SortStringArray(UPARAM(ref) TArray<FString>& StringArray, TArray<FString>& StringArrayRef);
	
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static void SaveStringToFile(FString Filename, FString StringToSave);

	UFUNCTION(BlueprintPure, Category = "String")
	static FString GetCleanFileName(const FString& Filepath);

	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (Keywords = "folder"))
	static bool DoesDirectoryExists(const FString& Path);

	UFUNCTION(BlueprintPure, Category = "Image", meta = (Keywords = "directory"))
	static TArray<FString> GetImagesInFolder(const FString& Path);

	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (Keywords = "pie editor"))
	static bool IsPIE(AActor* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "pie editor"))
	static void RenameActor(AActor* Actor, const FString& NewName);

	UFUNCTION(BlueprintPure, Category = "Utilities")
	static FString GetActorsStreamingLevelName(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Utilities")
	static int32 Return42(); 
};
