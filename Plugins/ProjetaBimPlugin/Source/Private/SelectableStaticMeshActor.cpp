// (C) 2018 - ProjetaBIM

#include "SelectableStaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "ProjetaBimPluginBPLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

ASelectableStaticMeshActor::ASelectableStaticMeshActor()
{
	ObjectOpacity = EOpacityLevel::Opaque;
	SetSelectionOpacity = EOpacityLevel::Opaque;
}

void ASelectableStaticMeshActor::SetScalarParameter(FName ParameterName, float Value)
{
	UStaticMeshComponent* SMC = GetStaticMeshComponent();
	
	for (int i = 0; i < SMC->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = SMC->CreateDynamicMaterialInstance(i);
		if (MID != nullptr)
		{
			MID->SetScalarParameterValue(ParameterName, Value);
		}
	}
}

void ASelectableStaticMeshActor::UpdateCollisionAfterOpacityChange()
{
	const bool bIsOpaque = ObjectOpacity == EOpacityLevel::Opaque && SetSelectionOpacity == EOpacityLevel::Opaque;
	SetActorEnableCollision(bIsOpaque);
}

void ASelectableStaticMeshActor::Highlight_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(true);
	SetScalarParameter(FName(TEXT("Highlight")), 1.0f);
}

void ASelectableStaticMeshActor::RemoveHighlight_Implementation(int32 Index)
{
	GetStaticMeshComponent()->SetRenderCustomDepth(false);
	SetScalarParameter(FName(TEXT("Highlight")), 0.0f);
}

FObjectIdentifier ASelectableStaticMeshActor::GetObjectIdentifier_Implementation(int32 Index) const
{
	return ObjectIdentifier;
}

EOpacityLevel ASelectableStaticMeshActor::GetObjectOpacity_Implementation(int32 Index) const
{
	return ObjectOpacity;
}

void ASelectableStaticMeshActor::SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	ObjectOpacity = NewOpacityLevel;
	UpdateCollisionAfterOpacityChange();
	const float NewOpacity = UProjetaBimPluginBPLibrary::GetOpacityLevelValue(NewOpacityLevel);
	SetScalarParameter(FName(TEXT("UserSetOpacity")), NewOpacity);
}

int32 ASelectableStaticMeshActor::Length_Implementation() const
{
	return 1;
}

void ASelectableStaticMeshActor::SetCollisionProfileName_Implementation(FName NewCollisionProfile)
{
	GetStaticMeshComponent()->SetCollisionProfileName(NewCollisionProfile);
}

void ASelectableStaticMeshActor::GetObjectBounds_Implementation(int32 Index, bool bOnlyCollidingComponents, FVector & OutOrigin, FVector & OutExtent) const
{
	GetActorBounds(bOnlyCollidingComponents, OutOrigin, OutExtent);
}

EOpacityLevel ASelectableStaticMeshActor::GetSetSelectionOpacity_Implementation(int32 Index) const
{
	return SetSelectionOpacity;
}

void ASelectableStaticMeshActor::SetSetSelectionOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel)
{
	SetSelectionOpacity = NewOpacityLevel;
	UpdateCollisionAfterOpacityChange();
	const float NewOpacity =  UProjetaBimPluginBPLibrary::GetOpacityLevelValue(NewOpacityLevel);
	SetScalarParameter(FName(TEXT("SetSelectionOpacity")), NewOpacity);	
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
