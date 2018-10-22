// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ProjetaBimPluginBPLibrary.h"
#include "ProjetaBimPlugin.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "HAL/PlatformFilemanager.h"
#include "Runtime/XmlParser/Public/XmlParser.h"
#include "Materials/MaterialInstance.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "PBGameMode.h"

#if PLATFORM_WINDOWS
#pragma warning(disable : 4668 4005)
#include <windows.h>
#endif

EImageFormat GetFormatFromFilename(const FString& FilePath)
{
	const FString Ext = FPaths::GetExtension(FilePath).ToLower();
	if (Ext == TEXT("jpg") || Ext == TEXT("jpeg"))
	{
		return EImageFormat::JPEG;
	}
	if (Ext == "png")
	{
		return EImageFormat::PNG;
	}
	if (Ext == "bmp")
	{
		return EImageFormat::BMP;
	}
	if (Ext == "exr")
	{
		return EImageFormat::EXR;
	}
	if (Ext == "ico")
	{
		return EImageFormat::ICO;
	}
	if (Ext == "icns")
	{
		return EImageFormat::ICNS;
	}
	const FString Msg = TEXT("ALERTA: Formato de imagem desconhecido para o arquivo ") + FilePath;
	UProjetaBimPluginBPLibrary::AddLogEntry(Msg);
	return EImageFormat::JPEG;
}

UProjetaBimPluginBPLibrary::UProjetaBimPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


UTexture2D* UProjetaBimPluginBPLibrary::LoadTexture2DFromFile(const FString& FilePath, bool& IsValid, int32& Width, int32& Height, bool bAbsolute)
{
	FString FullPath = FilePath;
	if (!bAbsolute)
	{
		FullPath = FPaths::ProjectContentDir() + FilePath;
	}
	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat ImageFormat = GetFormatFromFilename(FullPath);
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullPath)) return NULL;

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!LoadedT2D)
			{
				return NULL;
			}

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update
#define UpdateResource UpdateResource
			LoadedT2D->UpdateResource();
		}
	}
	IsValid = true;
	return LoadedT2D;
}

FString RemoveAccentFromFirstChar(FString A)
{
	//ANSI codes
	if (A[0] == 193)
		A[0] = 'A';
	else if (A[0] == 201)
		A[0] = 'E';
	else if (A[0] == 205)
		A[0] = 'I';
	else if (A[0] == 211)
		A[0] = 'O';
	else if (A[0] == 218)
		A[0] = 'U';

	else if (A[0] == 194)
		A[0] = 'A';
	else if (A[0] == 202)
		A[0] = 'E';
	else if (A[0] == 212)
		A[0] = 'O';

	else if (A[0] == 192)
		A[0] = 'A';

	return A;
}

//sorting predicate for FString that ignores accents
struct FSortString
{
	explicit FSortString()
	{
	}

	bool operator()(const FString A, const FString B) const
	{
		FString A_modified = RemoveAccentFromFirstChar(A);
		FString B_modified = RemoveAccentFromFirstChar(B);

		return B_modified > A_modified;
	}
};

void UProjetaBimPluginBPLibrary::SortStringArray(UPARAM(ref) TArray<FString>& StringArray, TArray<FString>& StringArrayRef)
{
	Algo::Sort(StringArray, FSortString());
	StringArrayRef = StringArray;
}

void UProjetaBimPluginBPLibrary::SaveStringToFile(FString Filename, FString StringToSave)
{
	const FString& FilePath = FPaths::GetPath(FPaths::ProjectContentDir()) + TEXT("/") + Filename;
	FFileHelper::SaveStringToFile(StringToSave, *FilePath);
}

FString UProjetaBimPluginBPLibrary::GetCleanFileName(const FString & Filepath)
{
	const FString Clean = FPaths::GetCleanFilename(Filepath);
	return Clean.Left(Clean.Len() - FPaths::GetExtension(Filepath).Len() - 1);
}

bool UProjetaBimPluginBPLibrary::DoesDirectoryExists(const FString & Path)
{
	const FString FullPath = FPaths::ProjectContentDir() + Path;
	return FPaths::DirectoryExists(FullPath);

}

TArray<FString> UProjetaBimPluginBPLibrary::GetImagesInFolder(const FString & Path)
{
	const FString FullPath = FPaths::ProjectContentDir() + Path;
	class FFindImagesVisitor : public IPlatformFile::FDirectoryVisitor
	{
	public:
		FFindImagesVisitor() {}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (!bIsDirectory)
			{
				const FString FullFilePath(FilenameOrDirectory);
				const FString Ext = FPaths::GetExtension(FullFilePath).ToLower();
				if (Ext == TEXT("jpg") || Ext == TEXT("jpeg") || Ext == TEXT("png") || Ext == TEXT("ico") || Ext == TEXT("exr") || Ext == TEXT("bmp") || Ext == TEXT("icns"))
				{
					FilesFound.Add(FullFilePath);
				}
			}
			return true;
		}
		TArray<FString> FilesFound;
	};

	FFindImagesVisitor Visitor;
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPath, Visitor);
	return Visitor.FilesFound;
}

bool UProjetaBimPluginBPLibrary::IsPIE(AActor * WorldContext)
{
	if (WorldContext)
	{
		return WorldContext->GetWorld()->WorldType == EWorldType::PIE;
	}
	return false;
}

void UProjetaBimPluginBPLibrary::RenameActor(AActor* Actor, const FString& NewName)
{
#if WITH_EDITOR
	if (Actor != nullptr)
	{
		for (TActorIterator<AActor> ActorItr(Actor->GetWorld()); ActorItr; ++ActorItr)
		{
			if (*ActorItr->GetActorLabel() == NewName)
			{
				return;
			}
		}
		Actor->Rename(*NewName);
		Actor->SetActorLabel(NewName);
	}
#endif
}


FString UProjetaBimPluginBPLibrary::GetActorsStreamingLevelName(AActor* Actor)
{
	if (Actor)
	{
		return Actor->GetLevel()->GetOuter()->GetName();
	}
	return FString();
}

void UProjetaBimPluginBPLibrary::GetMaterialDataFromDatasmith(const FString& DatasmithPath, UPARAM(ref) TArray<FString>& GlassTags, TArray<FString>& OutMaterialNames, TArray<bool>& OutIsOpaque, TArray<FLinearColor>& OutColors)
{
	const FXmlFile XmlFile(*DatasmithPath);

	if (XmlFile.IsValid())
	{
		const FXmlNode* RootNode = XmlFile.GetRootNode();

		if (RootNode != nullptr)
		{
			const TArray<FXmlNode*> AllNodes = RootNode->GetChildrenNodes();
			for (auto Node : AllNodes)
			{
				if (Node->GetTag() == TEXT("Material"))
				{
					const TArray<FXmlAttribute> Attrs = Node->GetAttributes();
					if (Attrs.Num() > 0)
					{
						//index 0 is always material name
						const FString& MatName = Attrs[0].GetValue();
						OutMaterialNames.Add(MatName);
						bool bIsOpaque = true;
						for (auto GlassTag : GlassTags)
						{
							if (MatName.Find(GlassTag) != INDEX_NONE)
							{
								bIsOpaque = false;
							}
						}
						OutIsOpaque.Add(bIsOpaque);

						const FXmlNode* ShaderNode = Node->GetFirstChildNode();
						const FXmlNode* ColorNode = ShaderNode->FindChildNode(TEXT("Diffusecolor"));
						if (ColorNode != nullptr)
						{
							const TArray<FXmlAttribute> Colors = ColorNode->GetAttributes();
							const float Red = FCString::Atof(*Colors[0].GetValue());
							const float Green = FCString::Atof(*Colors[1].GetValue());
							const float Blue = FCString::Atof(*Colors[2].GetValue());
							OutColors.Add(FLinearColor(Red, Green, Blue));
						}
						else
						{
							OutColors.Add(FLinearColor(0.7f, 0.7f, 0.7f));
						}
					}
				}
			}
		}
	}
}

void UProjetaBimPluginBPLibrary::SetOverridenLightmapResolution(UStaticMeshComponent * SMC, bool bNewOverrideLightmapRes, int32 OverridenLightmapRes)
{
	if (SMC != nullptr)
	{
		SMC->bOverrideLightMapRes = bNewOverrideLightmapRes;
		SMC->OverriddenLightMapRes = OverridenLightmapRes;
	}
}

void UProjetaBimPluginBPLibrary::SaveScreenshot(const FString & FilePath, bool bShowUI)
{
	const FString& FullPath = FPaths::ProjectContentDir() + FilePath;
	FScreenshotRequest::RequestScreenshot(FullPath, bShowUI, false);
}

void UProjetaBimPluginBPLibrary::OpenSystemPopupErrorWindow(const FString & Message)
{
	AddLogEntry(TEXT("ERRO: ") + Message);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *Message);

#if PLATFORM_WINDOWS
	MessageBox(
		NULL,
		(LPCWSTR)*Message,
		(LPCWSTR)L"Erro",
		MB_ICONERROR | MB_OK
	);
#endif
}

void UProjetaBimPluginBPLibrary::OpenSystemPopupWarningWindow(const FString & Message)
{
	AddLogEntry(TEXT("ALERTA: ") + Message);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *Message);

#if PLATFORM_WINDOWS
	MessageBox(
		NULL,
		(LPCWSTR)*Message,
		(LPCWSTR)L"Alerta",
		MB_ICONWARNING | MB_OK
	);
#endif
}

void UProjetaBimPluginBPLibrary::EmptyLogFile()
{
	const FString& FilePath = FPaths::GetPath(FPaths::GetProjectFilePath()) + TEXT("/Log.txt");
#define DeleteFile DeleteFile
	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FilePath);
}

void UProjetaBimPluginBPLibrary::AddLogEntry(const FString & Message)
{
	const FString& FilePath = FPaths::GetPath(FPaths::GetProjectFilePath()) + TEXT("/Log.txt");
	const FString LineToAdd = Message + TEXT("\n");
	FFileHelper::SaveStringToFile(*LineToAdd, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

bool UProjetaBimPluginBPLibrary::DoesMapExists(const UObject* WorldContextObject, FName LevelName)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return true;
	}

	const ETravelType TravelType = TRAVEL_Absolute;
	FWorldContext &WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
	FString Cmd = LevelName.ToString();
	FURL TestURL(&WorldContext.LastURL, *Cmd, TravelType);
	TestURL.Map = Cmd;
	return GEngine->MakeSureMapNameIsValid(TestURL.Map);
}

float UProjetaBimPluginBPLibrary::GetDistanceToCollision(UPrimitiveComponent * CollisionComponent, const FVector & Point, FVector & ClosestPointOnCollision)
{
	if (CollisionComponent == nullptr)
	{
		return -1;
	}
	return CollisionComponent->GetDistanceToCollision(Point, ClosestPointOnCollision);
}

FString UProjetaBimPluginBPLibrary::GetUniqueIdentifier(const FObjectIdentifier& ObjectIdentifier)
{
	if (ObjectIdentifier.LinkIdentifier == TEXT("0"))
	{
		return ObjectIdentifier.JsonIdentifier;
	}
	return ObjectIdentifier.LinkIdentifier + TEXT("_") + ObjectIdentifier.JsonIdentifier;
}

float UProjetaBimPluginBPLibrary::GetOpacityLevelValue(EOpacityLevel OpacityLevel)
{
	if (OpacityLevel == EOpacityLevel::Opaque)
	{
		return 1.0;
	} 
	if (OpacityLevel == EOpacityLevel::Transparent)
	{
		return 0.5f;
	}
	return 0.0f;
}

void UProjetaBimPluginBPLibrary::SetMobility(USceneComponent* Component, TEnumAsByte<EComponentMobility::Type> NewMobility)
{
#if WITH_EDITOR
	if (Component != nullptr)
	{
		Component->SetMobility(NewMobility);
	}
#endif
}

void UProjetaBimPluginBPLibrary::SetDistanceFieldIndirectShadow(UStaticMeshComponent * Component, bool bNewUseDFShadows, float MinVisibility)
{
#if WITH_EDITOR
	if (Component)
	{
		Component->bCastDistanceFieldIndirectShadow = bNewUseDFShadows;
		Component->DistanceFieldIndirectShadowMinVisibility = MinVisibility;
	}
#endif
}

void UProjetaBimPluginBPLibrary::SetDistanceFieldResolutionScale(UStaticMeshComponent * SMComponent, float NewDFResolutionScale)
{
#if WITH_EDITOR
	if (SMComponent == nullptr)
	{
		return;
	}
	UStaticMesh* Mesh = SMComponent->GetStaticMesh();
	if (Mesh && Mesh->SourceModels.Num() > 0)
	{
		bool bModified = false;
		for (int32 i=0; i < Mesh->SourceModels.Num(); i++)
		{
			FStaticMeshSourceModel& Model = Mesh->SourceModels[i];
			if (Model.BuildSettings.DistanceFieldResolutionScale != NewDFResolutionScale)
			{
				if (!bModified)
				{
					Mesh->Modify();
				}
				Model.BuildSettings.DistanceFieldResolutionScale = NewDFResolutionScale;
				bModified = true;
			}
		}
		if (bModified)
		{
			Mesh->Build();
		}
	}
#endif
}
