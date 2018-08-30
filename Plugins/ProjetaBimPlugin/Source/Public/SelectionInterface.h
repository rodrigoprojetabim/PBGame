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
	/** Highlight this object (updates its appearance) 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	void Highlight(int32 Index);

	/** Remove highlight from this object (updates its appearance) 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	void RemoveHighlight(int32 Index);
	
	/** Remove highlight from this object (updates its appearance) 
		@param Index used only for instanced static mesh actors */
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection")
	//void MakeOpaque(int32 Index);

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
	
	/** get this object's opacity 
		@param Index used only for instanced static mesh actors */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection", meta=(Keywords="transparent invisible opaque translucent"))
	EOpacityLevel GetObjectOpacity(int32 Index) const;
	
	/** set this object's opacity 
		@param Index used only for instanced static mesh actors; -1 means all instances */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection", meta=(Keywords="transparent invisible opaque translucent"))
	void SetObjectOpacity(int32 Index, EOpacityLevel NewOpacityLevel);
};
