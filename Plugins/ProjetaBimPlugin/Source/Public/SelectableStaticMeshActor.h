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

protected:
	EOpacityLevel CurrentOpacity;

public:
	/* ISelectionInterface */
	virtual void Highlight_Implementation(int32 Index) override;
	virtual void RemoveHighlight_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) const override;
	FString GetDiscipline_Implementation() const override;
	FString GetUniqueIdentifier_Implementation(int32 Index) const override;
	FObjectIdentifier GetObjectIdentifier_Implementation(int32 Index) const override;
	EOpacityLevel GetObjectOpacity_Implementation(int32 Index) const override;
	void SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel) override;
	/* end ISelectionInterface */
	
	ASelectableStaticMeshActor();

	UPROPERTY(EditAnywhere, Category="ProjetaBIM")
	FObjectIdentifier ObjectIdentifier;

	UFUNCTION(BlueprintCallable, Category="ProjetaBIM", meta=(Keywords="json link"))
	void SetObjectIdentifier(const FObjectIdentifier& NewObjectIdentifier);
};
