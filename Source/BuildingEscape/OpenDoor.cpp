#include "OpenDoor.h"

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"

UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void 
UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitDoor();
}

void 
UOpenDoor::TickComponent(float aDeltaTime, 
						 ELevelTick aTickType,
						 FActorComponentTickFunction* aThisTickFunction) {
	Super::TickComponent(aDeltaTime, 
						 aTickType, 
						 aThisTickFunction); 

	if (GetTotalMassOfOverlappingActorsInTriggerVolume() > mTotalMassOfOverlappingActorsInTriggerVolume) {
		OpenDoor(aDeltaTime);
	} else if (HasTimePassedToCloseTheDoor()) {
		CloseDoor(aDeltaTime);
	}
}

void 
UOpenDoor::InitDoor() {
	mInitialYaw = GetOwner()->GetActorRotation().Yaw;
	mCurrentYaw = mInitialYaw;
	mTargetYaw += mInitialYaw;	
	mDoorLastOpenedTime = 0.0f;
	CheckOpenCloseTriggerVolume();
	InitAudioComponent();
}

void
UOpenDoor::CheckOpenCloseTriggerVolume() const {
	if (mOpenCloseTriggerVolume == nullptr) {
		UE_LOG(LogTemp,
			   Error,
			   TEXT("%s: The open close trigger volume component is invalid. Please, choose one."),
			   *GetOwner()->GetName());
	}
}

void
UOpenDoor::InitAudioComponent() {
	check(mAudioComponent == nullptr);

	AActor* ownerActor = GetOwner();
	check(ownerActor);
	mAudioComponent = ownerActor->FindComponentByClass<UAudioComponent>();
	if (mAudioComponent == nullptr) {
		UE_LOG(LogTemp,
			   Error,
			   TEXT("No audio component found on %s"),
			   *ownerActor->GetName());
	}
}

void  
UOpenDoor::OpenDoor(const float aDeltaTime) {
	AActor* ownerActor = GetOwner();
	check(ownerActor != nullptr);

	mCurrentYaw = FMath::FInterpTo(mCurrentYaw,
								   mTargetYaw,
								   aDeltaTime,
								   mOpeningSpeed);
	FRotator rotation = ownerActor->GetActorRotation();
	rotation.Yaw = mCurrentYaw;
	ownerActor->SetActorRotation(rotation);

	mDoorLastOpenedTime = GetWorld()->GetTimeSeconds();

	PlayOpenDoorSound();
}

void 
UOpenDoor::CloseDoor(const float aDeltaTime) {
	AActor* ownerActor = GetOwner();
	check(ownerActor != nullptr);

	mCurrentYaw = FMath::FInterpTo(mCurrentYaw,
								   mInitialYaw,
								   aDeltaTime,
								   mOpeningSpeed);
	FRotator rotation = ownerActor->GetActorRotation();
	rotation.Yaw = mCurrentYaw;
	ownerActor->SetActorRotation(rotation);

	PlayCloseDoorSound();
}

bool 
UOpenDoor::HasTimePassedToCloseTheDoor() const {
	return GetWorld()->GetTimeSeconds() - mDoorLastOpenedTime > mCloseDoorDelayTime;
}

float
UOpenDoor::GetTotalMassOfOverlappingActorsInTriggerVolume() const {
	float totalMass = 0.0f;

	if (mOpenCloseTriggerVolume == nullptr) {
		return totalMass;
	}

	TArray<AActor*> overlappingActors;
	mOpenCloseTriggerVolume->GetOverlappingActors(overlappingActors);

	for (AActor* overlappingActor : overlappingActors) {
		check(overlappingActor != nullptr);

		UPrimitiveComponent* primitiveComponent = overlappingActor->FindComponentByClass<UPrimitiveComponent>();
		check(primitiveComponent != nullptr);

		totalMass += primitiveComponent->GetMass();
	}

	return totalMass;
}

void 
UOpenDoor::PlayOpenDoorSound() {
	if (mAudioComponent == nullptr) {
		return;
	}
		
	mWasCloseDoorSoundPlayed = false;
	if (mWasOpenDoorSoundPlayed == false) {
		mAudioComponent->Play();
		mWasOpenDoorSoundPlayed = true;
	}
	
}

void 
UOpenDoor::PlayCloseDoorSound() {
	if (mAudioComponent == nullptr) {
		return;
	}
		
	mWasOpenDoorSoundPlayed = false;
	if (mWasCloseDoorSoundPlayed == false) {
		mAudioComponent->Play();
		mWasCloseDoorSoundPlayed = true;
	}
}
