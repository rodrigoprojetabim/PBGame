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
struct FStaticMeshArray
{
	GENERATED_USTRUCT_BODY()

	/** Discipline identifier (ARQ, EST, etc.) */
	UPROPERTY(BlueprintReadOnly, Category = "Default")
	TArray<AStaticMeshActor*> Meshes;
};

UCLASS()
class APBGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

	public:
	
	APBGameMode();

	UPROPERTY(BlueprintReadWrite, Category = "ProjetaBIM")
	TArray<FDiscipline> Disciplines;

	UPROPERTY(BlueprintReadOnly, Category = "ProjetaBIM")
		TMap<FString, FStaticMeshArray> StaticMeshCategoriesMap;
	
	UPROPERTY(BlueprintReadOnly, Category = "ProjetaBIM")
		TMap<FString, AStaticMeshActor*> StaticMeshMap;

};
