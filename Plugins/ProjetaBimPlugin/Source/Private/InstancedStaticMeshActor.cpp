// (C) 2018 - ProjetaBIM

#include "InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"

#define PLUS_LIMBO_OFFSET FVector(0.f, 0.f, WORLD_MAX*0.5f)
#define MINUS_LIMBO_OFFSET FVector(0.f, 0.f, -WORLD_MAX*0.5f)

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

void AInstancedStaticMeshActor::AddInstancePB_Implementation(const FTransform & WorldTransform, const FString & JsonID)
{
	const int32 Index = ISMC->AddInstanceWorldSpace(WorldTransform);
	IDMap.Add(Index, FCString::Atoi(*JsonID));

	FTransform Limbo = WorldTransform;
	Limbo.AddToTranslation(MINUS_LIMBO_OFFSET);
	ISMC_Selected->AddInstanceWorldSpace(Limbo);
	ISMC_Transparent->AddInstanceWorldSpace(Limbo);
}

void AInstancedStaticMeshActor::Select_Implementation(int32 Index)
{
	if (SelectedIndexes.Contains(Index))
	{
		return;
	}
	SelectedIndexes.Add(Index);

	FTransform Limbo;
	ISMC->GetInstanceTransform(Index, Limbo);
	Limbo.AddToTranslation(MINUS_LIMBO_OFFSET);
	ISMC->UpdateInstanceTransform(Index, Limbo, false, false, true);

	ISMC_Selected->GetInstanceTransform(Index, Limbo);
	Limbo.AddToTranslation(PLUS_LIMBO_OFFSET);
	ISMC_Selected->UpdateInstanceTransform(Index, Limbo, false, true, true);
	ISMC_Selected->SetVisibility(true);
}

void AInstancedStaticMeshActor::Deselect_Implementation(int32 Index)
{
	if (!SelectedIndexes.Contains(Index))
	{
		return;
	}
	SelectedIndexes.Remove(Index);

	FTransform Limbo;
	ISMC->GetInstanceTransform(Index, Limbo);
	Limbo.AddToTranslation(PLUS_LIMBO_OFFSET);
	ISMC->UpdateInstanceTransform(Index, Limbo, false, true, true);

	ISMC_Selected->GetInstanceTransform(Index, Limbo);
	Limbo.AddToTranslation(MINUS_LIMBO_OFFSET);
	ISMC_Selected->UpdateInstanceTransform(Index, Limbo, false, false, true);

	if (SelectedIndexes.Num() == 0)
	{
		ISMC_Selected->SetVisibility(false);
	}
}

int32 AInstancedStaticMeshActor::ObjectIDToInstanceIndex(int32 ObjectID) const
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

