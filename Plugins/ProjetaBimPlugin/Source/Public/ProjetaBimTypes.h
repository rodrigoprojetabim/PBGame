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
	
	/** this object's actor */
	UPROPERTY(BlueprintReadOnly, Category = "Object Identifier")
	class AActor* Actor;
	
	/** its index (if instanced static mesh) */
	UPROPERTY(BlueprintReadOnly, Category = "Object Identifier")
	int32 Index;

	/** the level name it belongs to */
	UPROPERTY(BlueprintReadOnly, Category = "Object Identifier")
	FName LevelName;

	/** its SetSelection code (e.g. "ARQ_Outros", "ac", "portas"...) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Identifier")
	FString SetSelectionCode;

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

	inline bool HasJsonIdentifier() const
	{
		return JsonIdentifier != TEXT("0");
	}

	FObjectIdentifier()
	{
		JsonIdentifier = TEXT("0");
		LinkIdentifier = TEXT("0");
		DisciplineCode = TEXT("NONE");
		SetSelectionCode = TEXT("Indefinido");
	}
};


USTRUCT(BlueprintType)
struct FSetSelection
{
	GENERATED_USTRUCT_BODY()

	/** SetSelection identifier, as in json (e.g. "ac") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	FString Identifier;

	/** text that appears in the UI (e.g. "Ar Condicionado") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	FText UIText;

	/** current opacity for this Set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetSelection")
	EOpacityLevel CurrentOpacity;

	/** Objects in this SetSelection */
	UPROPERTY(BlueprintReadOnly, Category = "SetSelection")
	TArray<FObjectIdentifier> Objects;

	FSetSelection()
	{
		CurrentOpacity = EOpacityLevel::Opaque;
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
