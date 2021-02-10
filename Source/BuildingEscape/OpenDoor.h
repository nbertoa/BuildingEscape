#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenDoor.generated.h"

class AActor;
class ATriggerVolume;
class UAudioComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOpenDoor();

protected:
	void BeginPlay() override;

public:	
	void TickComponent(float aDeltaTime, 
					   ELevelTick aTickType, 
					   FActorComponentTickFunction* aThisTickFunction) override;
private:
	void InitDoor();
	void CheckOpenCloseTriggerVolume() const;
	void InitAudioComponent();
	void OpenDoor(const float aDeltaTime);
	void CloseDoor(const float aDeltaTime);
	bool HasTimePassedToCloseTheDoor() const;
	float GetTotalMassOfOverlappingActorsInTriggerVolume() const;
	void PlayOpenDoorSound();
	void PlayCloseDoorSound();

	float mInitialYaw = 0.0f;
	float mCurrentYaw = 0.0f;

	UPROPERTY(EditAnywhere)
	float mTargetYaw = 90.0f;

	UPROPERTY(EditAnywhere)
	float mOpeningSpeed = 2.0f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* mOpenCloseTriggerVolume = nullptr;
		
	UPROPERTY(EditAnywhere)
	float mCloseDoorDelayTime = 2.0f;
	float mDoorLastOpenedTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float mTotalMassOfOverlappingActorsInTriggerVolume = 40.0f;

	UAudioComponent* mAudioComponent = nullptr;

	bool mWasOpenDoorSoundPlayed = false;
	bool mWasCloseDoorSoundPlayed = true;
};
