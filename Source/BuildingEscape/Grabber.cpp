#include "Grabber.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h "
#include "PhysicsEngine/PhysicsHandleComponent.h"

UGrabber::UGrabber() {
	PrimaryComponentTick.bCanEverTick = true;
}

void 
UGrabber::BeginPlay() {
	Super::BeginPlay();
	InitGrabber();
}

void 
UGrabber::TickComponent(float aDeltaTime, 
						ELevelTick aTickType, 
						FActorComponentTickFunction* aThisTickFunction) {
	Super::TickComponent(aDeltaTime, 
						 aTickType, 
						 aThisTickFunction);

	UpdateGrabbedComponent();
}

void
UGrabber::DrawLineFromPlayerPositionToPlayerViewDirection() {
	FVector playerPosition;
	FVector lineEnd;
	ComputePlayerViewLineTrace(playerPosition,
							   lineEnd);
	DrawDebugLine(GetWorld(),
				  playerPosition,
				  lineEnd,
				  FColor(255, 0, 255),
				  false,
				  0.0f,
				  0.0f,
				  3.0f);
}

FHitResult
UGrabber::ComputePlayerViewHitResult() const {
	FVector playerPosition;
	FVector lineEnd;
	ComputePlayerViewLineTrace(playerPosition,
							   lineEnd);
	const FCollisionQueryParams collisionQueryParams(TEXT(""),
													 false,
													 GetOwner()); // actor to ignore in line trace
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByObjectType(hitResult,
											playerPosition,
											lineEnd,
											FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
											collisionQueryParams);

	return hitResult;
}

void 
UGrabber::LogPlayerViewHitActorName() const {
	const FHitResult hitResult = ComputePlayerViewHitResult();

	if (hitResult.Actor != nullptr) {
		UE_LOG(LogTemp,
			   Warning,
			   TEXT("Hit actor: %s"),
			   *hitResult.Actor->GetName());
	}
}

void 
UGrabber::InitGrabber() {
	InitPhysicsHandle();
	InitInputComponent();
}

void 
UGrabber::InitPhysicsHandle() {
	AActor* ownerActor = GetOwner();
	check(ownerActor);

	mPhysicsHandle = ownerActor->FindComponentByClass<UPhysicsHandleComponent>();
	if (mPhysicsHandle == nullptr) {
		UE_LOG(LogTemp,
			   Error,
			   TEXT("No physics handle component found on %s"),
			   *ownerActor->GetName());
	}
}

void 
UGrabber::InitInputComponent() {
	check(mInputComponent == nullptr);

	AActor* ownerActor = GetOwner();
	check(ownerActor);
	mInputComponent = ownerActor->FindComponentByClass<UInputComponent>();
	if (mInputComponent == nullptr) {
		UE_LOG(LogTemp,
			   Error,
			   TEXT("No input component found on %s"),
			   *ownerActor->GetName());
	} else {
		mInputComponent->BindAction("Grab",
									IE_Pressed,
									this,
									&UGrabber::Grab);

		mInputComponent->BindAction("Grab",
									IE_Released,
									this,
									&UGrabber::Release);
	}
}

void 
UGrabber::Grab() {
	if (mPhysicsHandle == nullptr) {
		return;
	}

	const FHitResult hitResult = ComputePlayerViewHitResult();
	if (hitResult.GetActor() != nullptr) {
		UPrimitiveComponent* componentToGrab = hitResult.GetComponent();
		check(componentToGrab != nullptr);

		const FVector lineEnd = ComputePlayerViewLineTraceEnd();
		mPhysicsHandle->GrabComponentAtLocation(componentToGrab,
												NAME_None,
												lineEnd);
	}
}

void
UGrabber::Release() {
	if (mPhysicsHandle == nullptr) {
		return;
	}

	mPhysicsHandle->ReleaseComponent();
}

void
UGrabber::ComputePlayerViewLineTrace(FVector& aLineStart,
									 FVector& aLineEnd) const {
	FRotator playerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(aLineStart,
															   playerRotation);

	aLineEnd = aLineStart + playerRotation.Vector() * mLineScale;
}

FVector
UGrabber::ComputePlayerViewLineTraceEnd() const {
	FVector playerPosition;
	FVector lineEnd;
	ComputePlayerViewLineTrace(playerPosition,
							   lineEnd);

	return lineEnd;
}

void 
UGrabber::UpdateGrabbedComponent() {
	if (mPhysicsHandle && mPhysicsHandle->GrabbedComponent != nullptr) {
		const FVector lineEnd = ComputePlayerViewLineTraceEnd();
		mPhysicsHandle->SetTargetLocation(lineEnd);
	}
}