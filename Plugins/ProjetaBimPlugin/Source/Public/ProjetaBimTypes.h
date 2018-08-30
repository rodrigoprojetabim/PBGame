// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "ProjetaBimTypes.generated.h"

UENUM(BlueprintType)
enum class EOpacityLevel : uint8 
{
	Opaque,
	Transparent,
	Invisible,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FObjectIdentifier
{
	GENERATED_USTRUCT_BODY()

	/** Object json identifier*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Identifier")
	FString JsonIdentifier;

	/** Link id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Identifier")
	FString LinkIdentifier;
	
	/** Discipline code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Identifier")
	FString DisciplineCode;
	
	inline FString GetUniqueIdentifier() const
	{
		if (LinkIdentifier == TEXT("0"))
		{
			return JsonIdentifier;
		}
		return LinkIdentifier + TEXT("_") + JsonIdentifier;
	}

	inline bool operator==(const FObjectIdentifier &Other) const 
	{ 
		return (JsonIdentifier == Other.JsonIdentifier && LinkIdentifier == Other.LinkIdentifier && DisciplineCode == Other.DisciplineCode);
	}

	FObjectIdentifier()
	{
		JsonIdentifier = TEXT("0");
		LinkIdentifier = TEXT("0");
		DisciplineCode = TEXT("");
	}
};

USTRUCT(BlueprintType)
struct FSetSelection
{
	GENERATED_USTRUCT_BODY()

	/** identifier, as in json */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	FString Identifier;

	/** text that appears in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	FText UIText;

	/** current opacity for this Set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	float CurrentOpacity;

	/** Meshes in this SetSelection */
	UPROPERTY(BlueprintReadOnly, Category = "SetSelection")
	TArray<class AStaticMeshActor*> Meshes;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discipline")
	FString Identifier;

	/** Discipline user-friendly name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discipline")
	FText Name;

	/** SetSelections for this discipline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discipline")
	TArray<FSetSelection> Sets;
};

USTRUCT(BlueprintType)
struct FSetSelectionMap
{
	GENERATED_USTRUCT_BODY()

	/** SetSelection identifier */
	UPROPERTY(BlueprintReadOnly, Category = "SetSelectionMap")
	FString Identifier;

	/** Meshes in this SetSelection */
	UPROPERTY(BlueprintReadOnly, Category = "SetSelectionMap")
	TArray<AStaticMeshActor*> Meshes;
};
