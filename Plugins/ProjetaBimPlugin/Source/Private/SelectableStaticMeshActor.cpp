// (C) 2018 - ProjetaBIM

#include "SelectableStaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ASelectableStaticMeshActor::ASelectableStaticMeshActor()
{
	CurrentOpacity = EOpacityLevel::Opaque;
}

void ASelectableStaticMeshActor::Highlight_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(true);

	for (int i = 0; i < GetStaticMeshComponent()->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = GetStaticMeshComponent()->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(FName(TEXT("Highlight")), 1.0f);
		}
	}
}

void ASelectableStaticMeshActor::RemoveHighlight_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(false);

	for (int i = 0; i < GetStaticMeshComponent()->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = GetStaticMeshComponent()->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(FName(TEXT("Highlight")), 0.0f);
		}
	}
}

FObjectIdentifier ASelectableStaticMeshActor::GetObjectIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier;
}

EOpacityLevel ASelectableStaticMeshActor::GetObjectOpacity_Implementation(int32 Index) const
{
	return CurrentOpacity;
}

void ASelectableStaticMeshActor::SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	UStaticMeshComponent* SMC = GetStaticMeshComponent();
	
	float NewOpacity;	
	switch (NewOpacityLevel)
	{
		case EOpacityLevel::Opaque:
		{
			NewOpacity = 1.0f;
			SetActorEnableCollision(true);
		} break;
		case EOpacityLevel::Transparent:
		{
			NewOpacity = 0.5f;
			SetActorEnableCollision(false);
		} break;
		case EOpacityLevel::Invisible:
		{
			NewOpacity = 0.0f;
			SetActorEnableCollision(false);
		} break;
	}

	for (int i = 0; i < SMC->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = SMC->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(FName(TEXT("UserSetOpacity")), NewOpacity);
		}
	}
}

FString ASelectableStaticMeshActor::GetJsonIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier.JsonIdentifier;
}

FString ASelectableStaticMeshActor::GetDiscipline_Implementation() const
{
	return ObjectIdentifier.DisciplineCode;
}

FString ASelectableStaticMeshActor::GetUniqueIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier.GetUniqueIdentifier();
}

void ASelectableStaticMeshActor::SetObjectIdentifier(const FObjectIdentifier& NewObjectIdentifier)
{
	ObjectIdentifier = NewObjectIdentifier;
}
