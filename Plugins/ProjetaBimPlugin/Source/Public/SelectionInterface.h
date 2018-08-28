// (C) 2018 - ProjetaBIM

#pragma once

#include "CoreMinimal.h"
#include "ProjetaBimTypes.h"
#include "UObject/Interface.h"
#include "SelectionInterface.generated.h"

UINTERFACE(BlueprintType)
class USelectionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJETABIMPLUGIN_API ISelectionInterface
{
	GENERATED_BODY()

public:
	/** Select this object (updates its appearance) 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	void Select(int32 Index);

	/** Deselect this object (updates its appearance) 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	void Deselect(int32 Index);

	/** get this object's json identifier 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	FString GetJsonIdentifier(int32 Index) const;
	
	/** get this object's discipline */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection", meta=(Keywords="json identifier"))
	FString GetDiscipline() const;
	
	/** get this object's unique identifier (LinkID_JsonID)
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection", meta=(Keywords="json link"))
	FString GetUniqueIdentifier(int32 Index) const;
	
	/** get this object's identifier (LinkID, JsonID and Discipline)
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection", meta=(Keywords="json link"))
	FObjectIdentifier GetObjectIdentifier(int32 Index) const;
};
