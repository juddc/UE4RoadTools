// Copyright (c) 2014 Judd Cohen
//
// Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
// or copy at http://opensource.org/licenses/MIT)

#include "RoadToolsPrivatePCH.h"
#include "Road.h"


ARoad::ARoad(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	Root = PCIP.CreateAbstractDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	RootComponent = Root;

	Spline = PCIP.CreateDefaultSubobject<USplineComponent>(this, TEXT("Spline"));
	Spline->SetMobility(EComponentMobility::Static);
	Spline->AttachTo(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder
		(TEXT("StaticMesh'/RoadTools/Meshes/Road_Plain.Road_Plain'"));
	DefaultMesh = MeshFinder.Object;

	EndScale = FVector2D(1.f, 1.f);
	EndRoll = 0;
}


void ARoad::PostEditMove(bool bFinished)
{
	// this event is the only indication the spline component gives us that anything has changed,
	// so force a complete update here so that we can see mesh changes as the user edits the spline
	if (bFinished)
	{
		RerunConstructionScripts();
	}
	Super::PostEditMove(bFinished);
}


void ARoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// start at 1 so this runs once per segment instead of once per point
	for (int32 i = 1; i < Spline->GetNumSplinePoints(); i++)
	{
		// make some variables for readability
		int32 SegmentIndex = i - 1;
		int32 SplineStartIndex = i - 1;
		int32 SplineEndIndex = i;

		// bail early if there aren't enough segments created to cover the whole spline
		if (SplineStartIndex > Segments.Num() - 1)
		{
			break;
		}

		// clear out the old segments and force GC to prevent memory leaks
		GetWorld()->ForceGarbageCollection(true);

		// regenerate this segment
		UpdateSplineSegment(SegmentIndex, SplineStartIndex, SplineEndIndex);
	}
}


FVector ARoad::GetLocalTangentAtDistanceAlongSpline(float Distance)
{
	const float Param = Spline->SplineReparamTable.Eval(Distance, 0.f);
	const FVector Tangent = Spline->SplineInfo.EvalDerivative(Param, FVector::ZeroVector);
	return Tangent;
}


void ARoad::UpdateSplineSegment(int32 SegmentIndex, int32 SplineStartIndex, int32 SplineEndIndex)
{
	FRoadSegment& Segment = Segments[SegmentIndex];

	float distToStart = Spline->GetDistanceAlongSplineAtSplinePoint(SplineStartIndex);
	float distToEnd = Spline->GetDistanceAlongSplineAtSplinePoint(SplineEndIndex);

	float segmentDist = distToEnd - distToStart;
	float subdivDist = segmentDist / Segment.NumSubDivisions;

	FVector segmentStartTangent = GetLocalTangentAtDistanceAlongSpline(distToStart);
	FVector segmentEndTangent = GetLocalTangentAtDistanceAlongSpline(distToEnd);

	for (int32 i = 0; i < Segment.NumSubDivisions; i++)
	{
		float subdivStartDist = distToStart + (i * subdivDist);
		float subdivEndDist = distToStart + ((i + 1) * subdivDist);

		FVector startLoc = Spline->GetWorldLocationAtDistanceAlongSpline(subdivStartDist);
		startLoc = GetActorLocation() - Spline->ComponentToWorld.InverseTransformPosition(startLoc);

		FVector endLoc = Spline->GetWorldLocationAtDistanceAlongSpline(subdivEndDist);
		endLoc = GetActorLocation() - Spline->ComponentToWorld.InverseTransformPosition(endLoc);

		// make a new spline mesh
		USplineMeshComponent* comp = NewObject<USplineMeshComponent>(this);

		if (Segment.Mesh)
		{
			comp->SetStaticMesh(Segment.Mesh);
		}
		else if (DefaultMesh)
		{
			comp->SetStaticMesh(DefaultMesh);
		}

		comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		comp->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

		// update start and end positions
		comp->SplineParams.StartPos = GetActorLocation() - startLoc;
		comp->SplineParams.EndPos = GetActorLocation() - endLoc;

		FVector startTangent = Spline->GetWorldDirectionAtDistanceAlongSpline(subdivStartDist);
		startTangent = Spline->ComponentToWorld.InverseTransformVector(startTangent);
		// the tangent is currently normalized (because we used GetWorldDirection), so scale it up
		startTangent *= segmentStartTangent.Size() / Segment.NumSubDivisions;
		comp->SplineParams.StartTangent = startTangent;

		FVector endTangent = Spline->GetWorldDirectionAtDistanceAlongSpline(subdivEndDist);
		endTangent = Spline->ComponentToWorld.InverseTransformVector(endTangent);
		// the tangent is currently normalized (because we used GetWorldDirection), so scale it up
		endTangent *= segmentEndTangent.Size() / Segment.NumSubDivisions;
		comp->SplineParams.EndTangent = endTangent;

		// if this is the last segment, use the special EndRoll/EndScale variables.
		float nextRoll;
		FVector2D nextScale;
		if (SegmentIndex == Segments.Num() - 1)
		{
			nextRoll = EndRoll;
			nextScale = EndScale;
		}
		// otherwise just use the values for the next segment
		else
		{
			FRoadSegment& nextSegment = Segments[SegmentIndex + 1];
			nextRoll = nextSegment.Roll;
			nextScale = nextSegment.Scale;
		}

		// for multiple subdivisions in one segment, lerp between the beginning and end of the segment
		float startLerpAlpha = i * subdivDist / segmentDist;
		float endLerpAlpha = (i + 1) * subdivDist / segmentDist;

		float startRoll = FMath::Lerp(Segment.Roll, nextRoll, startLerpAlpha);
		comp->SplineParams.StartRoll = FMath::DegreesToRadians(startRoll);

		float endRoll = FMath::Lerp(Segment.Roll, nextRoll, endLerpAlpha);
		comp->SplineParams.EndRoll = FMath::DegreesToRadians(endRoll);

		comp->SplineParams.StartScale = FMath::Lerp(Segment.Scale, nextScale, startLerpAlpha);
		comp->SplineParams.EndScale = FMath::Lerp(Segment.Scale, nextScale, endLerpAlpha);

		// make sure the spline data updates correctly
		comp->MarkSplineParamsDirty();

		// finish creating and registering the component
		comp->AttachTo(Root);
		comp->AttachParent = Root;
		comp->SetMobility(EComponentMobility::Static);
		comp->RegisterComponent();
		comp->bCreatedByConstructionScript = true;
	}
}

