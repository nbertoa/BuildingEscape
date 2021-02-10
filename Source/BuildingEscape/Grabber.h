#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class UPhysicsHandleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGrabber();

protected:
	void BeginPlay() override;

public:	
	void TickComponent(float aDeltaTime, 
					   ELevelTick aTickType, 
					   FActorComponentTickFunction* aThisTickFunction) override;

private:
	void DrawLineFromPlayerPositionToPlayerViewDirection();
	FHitResult ComputePlayerViewHitResult() const;
	void LogPlayerViewHitActorName() const;
	void InitGrabber();
	void InitPhysicsHandle();
	void InitInputComponent();
	void Grab();
	void Release();

	// aLineStart will be the position of the player in world space.
	// aLineEnd will be located in the direction of the 
	// player view point, scaled with mLineScale.
	void ComputePlayerViewLineTrace(FVector& aLineStart,
									FVector& aLineEnd) const;
	FVector ComputePlayerViewLineTraceEnd() const;

	void UpdateGrabbedComponent();	

	UPhysicsHandleComponent* mPhysicsHandle = nullptr;
	UInputComponent* mInputComponent = nullptr;

	float mLineScale = 100.0f;
};
