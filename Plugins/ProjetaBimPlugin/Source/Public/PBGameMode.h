// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "PBGameMode.generated.h"

USTRUCT(BlueprintType)
struct FSetSelection
{
	GENERATED_USTRUCT_BODY()

	/** identifier, as in json */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Identifier;

	/** text that appears in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FText UIText;

	/** current opacity for this Set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float CurrentOpacity;

	/** Meshes in this SetSelection */
	UPROPERTY(BlueprintReadOnly, Category = "Default")
	TArray<AStaticMeshActor*> Meshes;

	FSetSelection()
	{
		CurrentOpacity = 1.0f;
	}
};

USTRUCT(BlueprintType)
struct FDiscipline
{
	GENERATED_USTRUCT_BODY()

	/** Discipline identifier (ARQ, EST, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString Identifier;

	/** Discipline user-friendly name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FText Name;

	/** SetSelections for this discipline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FSetSelection> Sets;
};

USTRUCT(BlueprintType)
struct FSetSelectionMap
{
	GENERATED_USTRUCT_BODY()

	/** SetSelection identifier */
	UPROPERTY(BlueprintReadOnly, Category = "Default")
	FString Identifier;

	/** Meshes in this SetSelection */
	UPROPERTY(BlueprintReadOnly, Category = "Default")
	TArray<AStaticMeshActor*> Meshes;
};

UCLASS()
class APBGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

private:

	FName SetSelectionDefinedTag;
	FName AddedToSMMapTag;

	class ULevelStreaming* GetStreamingLevelFromName(const FString& LevelName);
	
protected:

	/** returns Mesh's discipline by looking at its streaming level name suffix. Returns MOB if level has no suffix. */
	FString GetMeshDiscipline(const class AStaticMeshActor* Mesh);

	/** returns the current level's name */
	FString GetPersistentLevelName();

	/** removes the discipline from LevelName, e.g. TESTMAP_ARQ becomes TESTMAP */
	FString GetLevelNameWithoutDisciplineSuffix(const FString& LevelName);

public:
	
	APBGameMode();

	/** disciplines that need to be processed into the SetSelection map */
	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FString> DisciplinesToProcess;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM")
	void InitializeSetSelectionMap();

	/** note: returns a copy! */
	UFUNCTION(BlueprintPure, Category = "ProjetaBIM")
	bool GetSetSelection(const FString& SetIdentifier, FSetSelection& SetSelection);

	/** adds Mesh to SetIdentifier, returns true if success, false if SetIdentifier doesn't exists */
	bool AddStaticMeshToSetSelection(const FString& SetIdentifier, class AStaticMeshActor* Mesh);

	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FDiscipline> Disciplines;

	UPROPERTY(BlueprintReadOnly, Category = "ProjetaBIM")
	TMap<FString, AStaticMeshActor*> StaticMeshMap;
	
};
