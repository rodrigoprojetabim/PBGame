// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjetaBimTypes.h"
#include "Engine/EngineTypes.h"
#include "ProjetaBimPluginBPLibrary.generated.h"

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
	static void GetMaterialDataFromDatasmith(const FString& DatasmithPath, UPARAM(ref) TArray<FString>& GlassTags, TArray<FString>& OutMaterialNames, TArray<bool>& OutIsOpaque, TArray<FLinearColor>& OutColors);

	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "pie editor"))
	static void SetOverridenLightmapResolution(class UStaticMeshComponent* SMC, bool bNewOverrideLightmapRes, int32 OverridenLightmapRes);

	/** saves a screenshot of the viewport to disk. Path is relative to /Content. Do NOT provide the filename suffix. */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "viewport"))
	static void SaveScreenshot(const FString& FilePath, bool bShowUI);

	/** opens a system popup window with the error Message, then exits the game */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "dialog log warning assert debug create fatal"))
	static void OpenSystemPopupErrorWindow(const FString& Message);

	/** opens a system popup window with the warning Message */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "dialog log warning assert debug create"))
	static void OpenSystemPopupWarningWindow(const FString& Message);

	/** clears the log file (Log.txt) */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "warning assert debug"))
	static void EmptyLogFile();

	/** adds Message to the Log.txt file */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "warning assert debug"))
	static void AddLogEntry(const FString& Message);

	/** returns true if map exists, false otherwise */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (Keywords = "test check level", WorldContext = "WorldContextObject") )
	static bool DoesMapExists(const UObject* WorldContextObject, FName LevelName);

	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (Keywords = "closest nearest"))
	static float GetDistanceToCollision(UPrimitiveComponent* CollisionComponent, const FVector& Point, FVector& ClosestPointOnCollision);
	
	/** Get the object's unique identifier (such as LinkID_JsonID) */
	UFUNCTION(BlueprintPure, Category="Object Identifier")
	static FString GetUniqueIdentifier(const FObjectIdentifier& ObjectIdentifier);
	
	/** Gets opacity level's value */
	UFUNCTION(BlueprintPure, Category="Object Identifier")
	static float GetOpacityLevelValue(EOpacityLevel OpacityLevel);
	
	/** sets object mobility */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "warning assert debug"))
	static void SetMobility(class USceneComponent* Component, TEnumAsByte<EComponentMobility::Type> NewMobility);
	
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "warning assert debug"))
	static void SetDistanceFieldIndirectShadow(class UStaticMeshComponent* Component, bool bNewUseDFShadows, float MinVisibility = 0.1f);
	
	/** set distance field resolution scale */
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (Keywords = "warning assert debug"))
	static void SetDistanceFieldResolutionScale(class UStaticMeshComponent* SMComponent, float NewDFResolutionScale);

};
