// (C) 2018 - ProjetaBIM

#include "InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"

#define LIMBO FTransform(FVector(WORLD_MAX-10.f, WORLD_MAX-10.f, WORLD_MAX-10.f))

bool AInstancedStaticMeshActor::IsAnyInstanceTransparent() const
{
	for (auto Op : OpacityLevels)
	{
		if (Op == EOpacityLevel::Transparent)
		{
			return true;
		}
	}
	return false;
}

AInstancedStaticMeshActor::AInstancedStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC"));
	ISMC_Selected = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC_Selected"));
	ISMC_Transparent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMC_Transparent"));
	
	RootComponent = ISMC;
	ISMC_Selected->SetupAttachment(ISMC);
	ISMC_Transparent->SetupAttachment(ISMC);

	ISMC_Selected->SetVisibility(false);
	ISMC_Transparent->SetVisibility(false);

	ISMC_Selected->SetRenderCustomDepth(true);

	ISMC->SetMobility(EComponentMobility::Static);
	ISMC_Selected->SetMobility(EComponentMobility::Static);
	ISMC_Transparent->SetMobility(EComponentMobility::Static);
}

void AInstancedStaticMeshActor::SetMaterialAndMesh_Implementation(AStaticMeshActor * SourceSMA)
{
	if (SourceSMA == nullptr)
	{
		return;
	}
	const UStaticMeshComponent* SMC = SourceSMA->GetStaticMeshComponent();
	
	ISMC->SetStaticMesh(SMC->GetStaticMesh());
	ISMC_Transparent->SetStaticMesh(SMC->GetStaticMesh());
	ISMC_Selected->SetStaticMesh(SMC->GetStaticMesh());

	const int32 NumMaterials = SMC->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		ISMC->SetMaterial(i, SMC->GetMaterial(i));
		ISMC_Transparent->SetMaterial(i, SMC->GetMaterial(i));
		ISMC_Selected->SetMaterial(i, SMC->GetMaterial(i));
	}
}

void AInstancedStaticMeshActor::AddInstancePB_Implementation(const FTransform& WorldTransform, const FObjectIdentifier& ObjectIdentifier)
{
	const int32 Index = ISMC->AddInstanceWorldSpace(WorldTransform);
	IDMap.Add(Index, ObjectIdentifier);
	ISMC_Selected->AddInstanceWorldSpace(LIMBO);
	ISMC_Transparent->AddInstanceWorldSpace(LIMBO);

	OpacityLevels.Add(EOpacityLevel::Opaque);
	OriginalWorldTransforms.Add(WorldTransform);
	NumberOfInstances = NumberOfInstances + 1;
}

int32 AInstancedStaticMeshActor::ObjectIdentifierToInstanceIndex(const FObjectIdentifier& ObjectID) const
{
	TArray<int32> Keys;
	IDMap.GetKeys(Keys);
	for (int32 i : Keys)
	{
		if (*IDMap.Find(i) == ObjectID)
		{
			return i;
		}
	}
	return -1;
}

void AInstancedStaticMeshActor::Highlight_Implementation(int32 Index)
{
	if (OpacityLevels.IsValidIndex(Index) && OpacityLevels[Index] == EOpacityLevel::Opaque)
	{
		HighlightedIndexes.AddUnique(Index);

		ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
		ISMC_Selected->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
		ISMC_Selected->SetVisibility(true);
	}
}

void AInstancedStaticMeshActor::RemoveHighlight_Implementation(int32 Index)
{
	if (!HighlightedIndexes.Contains(Index) || (OpacityLevels.IsValidIndex(Index) && OpacityLevels[Index] != EOpacityLevel::Opaque) )
	{
		return;
	}
	HighlightedIndexes.Remove(Index);

	ISMC->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
	ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);

	if (HighlightedIndexes.Num() == 0)
	{
		ISMC_Selected->SetVisibility(false);
	}
}

FObjectIdentifier AInstancedStaticMeshActor::GetObjectIdentifier_Implementation(int32 Index) const
{
	return *IDMap.Find(Index);
}

EOpacityLevel AInstancedStaticMeshActor::GetObjectOpacity_Implementation(int32 Index) const
{
	if (OpacityLevels.IsValidIndex(Index))
	{
		return OpacityLevels[Index];
	}
	return EOpacityLevel::Opaque;	
}

void AInstancedStaticMeshActor::SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	if (Index != -1)
	{
		SetObjectOpacity_Internal(Index, NewOpacityLevel);
	}
	else
	{
		for (int i = 0; i < NumberOfInstances; i++)
		{
			SetObjectOpacity_Internal(i, NewOpacityLevel);
		}
	}
}

void AInstancedStaticMeshActor::SetObjectOpacity_Internal(int32 Index, EOpacityLevel NewOpacityLevel)
{
	if (OpacityLevels.IsValidIndex(Index))
	{
		OpacityLevels[Index] = NewOpacityLevel;

		switch (NewOpacityLevel)
		{
			case EOpacityLevel::Opaque:
			{
				ISMC->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
				ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				ISMC_Transparent->UpdateInstanceTransform(Index, LIMBO, true, true, true);
				if (HighlightedIndexes.Find(Index) != INDEX_NONE)
				{
					Highlight(Index);
				}
			} break;
			case EOpacityLevel::Transparent:
			{
					ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
					ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
					ISMC_Transparent->UpdateInstanceTransform(Index, OriginalWorldTransforms[Index], true, true, true);
					ISMC_Transparent->SetVisibility(true);
			} break;
			case EOpacityLevel::Invisible:
			{
					ISMC->UpdateInstanceTransform(Index, LIMBO, true, true, true);
					ISMC_Selected->UpdateInstanceTransform(Index, LIMBO, true, true, true);
					ISMC_Transparent->UpdateInstanceTransform(Index, LIMBO, true, true, true);
			} break;
		}

		if (!IsAnyInstanceTransparent())
		{
			ISMC_Transparent->SetVisibility(false);
		}
	}
}

FString AInstancedStaticMeshActor::GetJsonIdentifier_Implementation(int32 Index) const
{
	const FObjectIdentifier Obj = *IDMap.Find(Index);
	return Obj.JsonIdentifier;
}

FString AInstancedStaticMeshActor::GetDiscipline_Implementation() const
{
	TArray<int32> Keys;
	IDMap.GetKeys(Keys);
	if (Keys.Num() > 0)
	{
		return (*IDMap.Find(Keys[0])).DisciplineCode;
	}
	return TEXT("");
}

FString AInstancedStaticMeshActor::GetUniqueIdentifier_Implementation(int32 Index) const
{
	const FObjectIdentifier Obj = *IDMap.Find(Index);
	return Obj.GetUniqueIdentifier();
}

void AInstancedStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic* MatInstSelected = ISMC_Selected->CreateAndSetMaterialInstanceDynamic(0);
	if (MatInstSelected != nullptr)
	{
		MatInstSelected->SetScalarParameterValue(FName("Highlight"), 1.0f);
	}

	UMaterialInstanceDynamic* MatInstTransparent = ISMC_Transparent->CreateAndSetMaterialInstanceDynamic(0);
	if (MatInstTransparent != nullptr)
	{
		MatInstTransparent->SetScalarParameterValue(FName("UserSetOpacity"), 0.5f);
	}
}

