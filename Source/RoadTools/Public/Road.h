// Copyright (c) 2014 Judd Cohen
//
// Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
// or copy at http://opensource.org/licenses/MIT)

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Road.generated.h"


/**
 * Represents one segment - all meshes between two points on a spline
 */
USTRUCT(BlueprintType)
struct FRoadSegment
{
	GENERATED_USTRUCT_BODY()

	/** Mesh override for this segment. If left empty, will use the default mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Segment)
	UStaticMesh* Mesh;

	/** Number of SplineMeshComponents to make that will follow this segment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Segment)
	int32 NumSubDivisions;

	/** The scale value for the starting spline point of this segment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Segment)
	FVector2D Scale;

	/** The roll value for the starting spline point of this segment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Segment)
	float Roll;

	FRoadSegment()
	{
		NumSubDivisions = 4;
		Scale = FVector2D(1.f, 1.f);
		Roll = 0;
	}
};


/**
* A repeating mesh or meshes along a spline
*/
UCLASS(HideCategories=(Activation, Spline, Input))
class ARoad : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TSubobjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Road)
	TSubobjectPtr<USplineComponent> Spline;

	/** The default mesh to use when a segment does not define one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Road)
	UStaticMesh* DefaultMesh;

	/** Segment data for the whole road */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Road)
	TArray<FRoadSegment> Segments;

	/** The scale value for the last spline point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Road)
	FVector2D EndScale;

	/** The roll value for the last spline point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Road)
	float EndRoll;

	/** rebuild SplineMeshComponents on construction */
	virtual void OnConstruction(const FTransform& transform) override;

#if WITH_EDITOR
	/** force OnConstruction to run again when the spline is edited */
	virtual void PostEditMove(bool bFinished) override;
#endif

	FVector GetLocalTangentAtDistanceAlongSpline(float Distance);

	/** rebuild SplineMeshComponents for a particular segment */
	void UpdateSplineSegment(int32 SegmentIndex, int32 SplineStartIndex, int32 SplineEndIndex);

};

