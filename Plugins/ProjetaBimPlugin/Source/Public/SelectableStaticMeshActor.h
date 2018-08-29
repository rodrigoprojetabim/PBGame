// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "ProjetaBimTypes.h"
#include "Engine/StaticMeshActor.h"
#include "SelectionInterface.h"
#include "SelectableStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJETABIMPLUGIN_API ASelectableStaticMeshActor : public AStaticMeshActor, public ISelectionInterface
{
	GENERATED_BODY()

public:
	/* ISelectionInterface */
	virtual void Select_Implementation(int32 Index) override;
	virtual void Deselect_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) const override;
	FString GetDiscipline_Implementation() const override;
	FString GetUniqueIdentifier_Implementation(int32 Index) const override;
	FObjectIdentifier GetObjectIdentifier_Implementation(int32 Index) const override;
	/* end ISelectionInterface */
	
	UPROPERTY(EditAnywhere, Category="ProjetaBIM")
	FObjectIdentifier ObjectIdentifier;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM", meta=(Keywords="json link"))
	void SetObjectIdentifier(const FObjectIdentifier& NewObjectIdentifier);
};
