// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ImagePub.h"

// important sort
#include "OpenCVHelper.h"
#include "PreOpenCVHeaders.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PostOpenCVHeaders.h"

#include "CamCV.generated.h"

UCLASS()
class UTILPLUGIN_API ACamCV : public AActor
{
	GENERATED_BODY()

private:
	// UTexture2D* TexFromCvMat(cv::Mat& Mat, UTexture2D* InTexture);
	TArray<FColor> ColorDataFront;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ACamCV();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SC_Front;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* RT_Front;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* MaterialInstanceDynamic;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bSaveImage = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D VideoSize = FVector2D(1920, 1080);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* TextureCV = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UImagePub* ImagePub;
};
