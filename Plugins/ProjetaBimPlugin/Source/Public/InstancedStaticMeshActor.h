// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionInterface.h"
#include "ProjetaBimTypes.h"
#include "InstancedStaticMeshActor.generated.h"

UCLASS(Blueprintable)
class PROJETABIMPLUGIN_API AInstancedStaticMeshActor : public AActor, public ISelectionInterface
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC_Selected;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Instanced Static Mesh Actor")
	class UInstancedStaticMeshComponent* ISMC_Transparent;
	
	UPROPERTY()
	TArray<EOpacityLevel> ObjectsOpacity;
	
	UPROPERTY()
	TArray<EOpacityLevel> SetSelectionsOpacity;
	
	UPROPERTY()
	TArray<FTransform> OriginalWorldTransforms;
	
	/** index of currently highlighted instances */
	TArray<int32> HighlightedIndexes;

	bool IsAnyInstanceTransparent() const;
	
	void SetObjectOpacity_Internal(int32 Index);
	
	UPROPERTY()
	int32 NumberOfInstances;
	
	TArray<class UMaterialInstanceDynamic*> MatInstancesSelected;
	TArray<class UMaterialInstanceDynamic*> MatInstancesTransparent;

public:
	AInstancedStaticMeshActor();
	
	/** maps ISM index to object (Revit) identifier */
	UPROPERTY(EditAnywhere, Category = "ProjetaBIM")
	TMap<int32, FObjectIdentifier> IDMap;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Instanced Static Mesh Actor")
	void SetMaterialAndMesh(class AStaticMeshActor* SourceSMA);
	void SetMaterialAndMesh_Implementation(class AStaticMeshActor* SourceSMA);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Instanced Static Mesh Actor")
	void AddInstancePB(const FTransform& WorldTransform, FObjectIdentifier ObjectIdentifier);
	void AddInstancePB_Implementation(const FTransform& WorldTransform, FObjectIdentifier ObjectIdentifier);

	UFUNCTION(BlueprintPure, Category = "Instanced Static Mesh Actor", meta=(Keywords="json link"))
	int32 ObjectIdentifierToInstanceIndex(const FObjectIdentifier& ObjectID) const;
	
	UFUNCTION(BlueprintCallable, Category="ProjetaBIM", meta=(Keywords="json link"))
	void SetObjectIdentifier(int32 Index, const FObjectIdentifier& NewObjectIdentifier);

	/* ISelectionInterface */
	virtual void Highlight_Implementation(int32 Index) override;
	virtual void RemoveHighlight_Implementation(int32 Index) override;
	FString GetJsonIdentifier_Implementation(int32 Index) const override;
	FString GetDiscipline_Implementation() const override;
	FString GetUniqueIdentifier_Implementation(int32 Index) const override;
	FObjectIdentifier GetObjectIdentifier_Implementation(int32 Index) const override;
	EOpacityLevel GetObjectOpacity_Implementation(int32 Index) const override;
	void SetObjectOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel) override;
	int32 Length_Implementation() const override;
	void SetCollisionProfileName_Implementation(FName NewCollisionProfile) override;
	void GetObjectBounds_Implementation(int32 Index, bool bOnlyCollidingComponents, FVector& OutOrigin, FVector& OutExtent) const override;
	EOpacityLevel GetSetSelectionOpacity_Implementation(int32 Index) const override;
	void SetSetSelectionOpacity_Implementation(int32 Index, EOpacityLevel NewOpacityLevel) override;
	bool IsFullyOpaque_Implementation(int32 Index) const override;
	/* end ISelectionInterface */
};
