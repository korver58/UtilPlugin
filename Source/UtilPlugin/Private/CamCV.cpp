#include "CamCV.h"

ACamCV::ACamCV()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
		RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	ImagePub = CreateDefaultSubobject<UImagePub>(TEXT("ImagePub"));
	// TFPub = CreateDefaultSubobject<UTFPub>(TEXT("TFPub"));
	PosePub = CreateDefaultSubobject<UPosePub>(TEXT("PosePub"));
}

void ACamCV::BeginPlay()
{
	Super::BeginPlay();

	RT_Front = NewObject<UTextureRenderTarget2D>(this);
	int w = VideoSize.X;
	int h = VideoSize.Y;
	RT_Front->InitCustomFormat(w, h, EPixelFormat::PF_B8G8R8A8, false);
	RT_Front->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RT_Front->bGPUSharedFlag = true;
	RT_Front->UpdateResource();

	SC_Front = NewObject<USceneCaptureComponent2D>(this);
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	SC_Front->AttachToComponent(RootComponent, rules);
	SC_Front->RegisterComponent();
	SC_Front->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SC_Front->bCaptureEveryFrame = true;
	SC_Front->TextureTarget = RT_Front;
	SC_Front->UpdateContent();

	if (!TextureCV) {
		TextureCV = UTexture2D::CreateTransient(w, h, EPixelFormat::PF_B8G8R8A8, FName("RuntimeTextureCV"));
	}
#if WITH_EDITORONLY_DATA
	TextureCV->MipGenSettings = TMGS_NoMipmaps;
#endif
	TextureCV->SRGB = RT_Front->SRGB;
}

void ACamCV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float RefreshTimer = 0.0f;
	static int FrameCount = 0;
	float RefreshRate = 30.0f;

	RefreshTimer += DeltaTime;
	if (RefreshTimer >= 1.0f / RefreshRate) {
		RefreshTimer -= 1.0f / RefreshRate;

		FTextureRenderTargetResource* rtFront = RT_Front->GameThread_GetRenderTargetResource();
		int w = rtFront->GetSizeXY().X;
		int h = rtFront->GetSizeXY().Y;
		rtFront->ReadPixels(ColorDataFront);

		cv::Mat front = cv::Mat(h, w, CV_8UC4, ColorDataFront.GetData());

		if (bSaveImage) {
			FString path = FPaths::ProjectSavedDir() + "/" + FString::FromInt(FrameCount) + ".png";
			cv::imwrite(TCHAR_TO_UTF8(*path), front);
		}

		if (bRosPublish) {
			ImagePub->Publish(VideoSize.X, VideoSize.Y, front.data);
			// TFPub->Publish();
			PosePub->Publish();
		}

		// UE_5.2\Engine\Plugins\Runtime\OpenCV\Source\OpenCVHelper\Private\OpenCVHelper.cpp
		// FOpenCVHelper::TextureFromCvMat(front, TextureCV);
		FTexture2DMipMap& Mip0 = TextureCV->GetPlatformData()->Mips[0];
		void* TextureData = Mip0.BulkData.Lock(LOCK_READ_WRITE);
		const int32 PixelStride = front.channels();
		FMemory::Memcpy(TextureData, front.data, front.cols * front.rows * SIZE_T(PixelStride));
		Mip0.BulkData.Unlock();
		TextureCV->UpdateResource();

		FrameCount += 1;
	}
}