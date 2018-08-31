// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "ProjetaBimTypes.h"
#include "PBGameMode.generated.h"

UCLASS()
class APBGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

private:

	FName SetSelectionDefinedTag;
	FName AddedToSMMapTag;

	class ULevelStreaming* GetStreamingLevelFromName(const FString& LevelName) const;
	
protected:

	/** returns the current level's name */
	FString GetPersistentLevelName() const;

	/** removes the discipline from LevelName, e.g. TESTMAP_ARQ becomes TESTMAP */
	FString GetLevelNameWithoutDisciplineSuffix(const FString& LevelName) const;

public:
	
	/** disciplines that need to be processed into the SetSelection map */
	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FString> DisciplinesToProcess;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM")
	void InitializeSetSelectionMap();

	/** note: returns a copy! */
	UFUNCTION(BlueprintPure, Category = "ProjetaBIM")
	bool GetSetSelection(const FString& SetIdentifier, FSetSelection& SetSelection) const;

	/** adds object to SetIdentifier, returns true if success, false if SetIdentifier doesn't exists */
	bool AddObjectToSetSelection(const FString& InSetIdentifier, FObjectIdentifier InObject);

	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FDiscipline> Disciplines;

	/** maps unique identifiers to object data */
	UPROPERTY(BlueprintReadOnly, Category = "ProjetaBIM")
	TMap<FString, FObjectIdentifier> ObjectsMap;
	
};
