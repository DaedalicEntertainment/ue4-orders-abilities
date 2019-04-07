#include "Orders/RTSMoveOrder.h"

#include "NumericLimits.h"
#include "TransformCalculus2D.h"
#include "UnrealMathUtility.h"
#include "GameFramework/Actor.h"

#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSOrderTargetData.h"


URTSMoveOrder::URTSMoveOrder()
{
    TargetType = ERTSTargetType::LOCATION;
    bIsCreatingIndividualTargetLocations = true;

    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Immobilized());
    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Constructing());
}

void URTSMoveOrder::CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
                                                    const FRTSOrderTargetData& TargetData,
                                                    TArray<FVector2D>& OutTargetLocations) const
{
    const FVector2D& TargetLocation = TargetData.Location;

    // Resize the output array upfront.
    OutTargetLocations.AddUninitialized(OrderedActors.Num());

    TArray<AActor*> ActorsToMove = OrderedActors;

    // Sort all actors by their formation rank.
    ActorsToMove.Sort([&](const AActor& First, const AActor& Second) {
        return GetFormationRank(&First) > GetFormationRank(&Second);
    });

    const FVector2D StartingCenterOfFormation = GetCenterOfGroup(ActorsToMove);
    const FVector2D TargetCenterOfFormation(TargetLocation.X, TargetLocation.Y);

    TArray<FVector2D> TargetLocations;
    CalculateFormation(ActorsToMove.Num(), TargetCenterOfFormation - StartingCenterOfFormation, TargetCenterOfFormation,
                       TargetLocations);

    int32 MovedUnits = 0;
    while (ActorsToMove.Num() > 0)
    {
        int32 FormationRank = GetFormationRank(ActorsToMove[0]);
        int32 UnitsWithCurrentRank = 0;
        for (int32 i = 0; i < ActorsToMove.Num(); ++i)
        {
            if (FormationRank != GetFormationRank(ActorsToMove[i]))
            {
                break;
            }

            UnitsWithCurrentRank++;
        }

        int32 MovedActors = 0;
        for (int32 i = MovedUnits; i < MovedUnits + UnitsWithCurrentRank; ++i)
        {
            const FVector2D CurrentFormationLocation = TargetLocations[i];
            AActor* ActorToMove = nullptr;

            // The lower the better.
            float Quality = TNumericLimits<float>::Max();

            for (int32 j = 0; j < UnitsWithCurrentRank - MovedActors; ++j)
            {
                FVector2D UnitStartLocation(ActorsToMove[j]->GetActorLocation().X,
                                            ActorsToMove[j]->GetActorLocation().Y);

                const FVector2D Delta = (UnitStartLocation - StartingCenterOfFormation) -
                                        (CurrentFormationLocation - TargetCenterOfFormation);

                const float NewQuality = FVector2D::DotProduct(Delta, Delta);
                if (NewQuality < Quality)
                {
                    ActorToMove = ActorsToMove[j];
                    Quality = NewQuality;
                }
            }

            // Remove the chosen actor so that it cannot choose another location.
            ActorsToMove.Remove(ActorToMove);
            MovedActors++;

            // Store the location in the output array.
            OutTargetLocations[OrderedActors.IndexOfByKey(ActorToMove)] = CurrentFormationLocation;
        }

        MovedUnits += UnitsWithCurrentRank;
    }
}

void URTSMoveOrder::CalculateFormation(int32 UnitCount, const FVector2D Direction, const FVector2D TargetLocation,
                                       TArray<FVector2D>& OutLocations) const
{
    if (UnitCount <= 1)
    {
        OutLocations.Add(TargetLocation);
        return;
    }

    // Create a formation grid of an appropriate size.
    //

    // The formation is a square and this is its edge length in units.
    const int32 EdgeLengthX = FMath::CeilToInt(FMath::Sqrt(UnitCount));
    const int32 EdgeLengthY = FMath::CeilToInt(UnitCount / static_cast<float>(EdgeLengthX));

    // TODO: Remove this magic numbers.
    // The space between two units in cm.
    const float UnitSpacingX = 300;
    const float UnitSpacingY = 300;

    float FormationOffsetX = (EdgeLengthX / 2) * UnitSpacingX;
    if (EdgeLengthX % 2 == 0)
    {
        // Patch the offset if the edge length is even.
        FormationOffsetX -= UnitSpacingX / 2.0f;
    }

    float FormationOffsetY = (EdgeLengthY / 2) * UnitSpacingY;
    if (EdgeLengthY % 2 == 0)
    {
        // Patch the offset if the edge length is even.
        FormationOffsetY -= UnitSpacingY / 2.0f;
    }

    for (int32 i = 0; i < UnitCount; ++i)
    {
        const int32 X = i % EdgeLengthX;
        const int32 Y = i / EdgeLengthX;

        const FVector2D GridLocation(X * UnitSpacingX - FormationOffsetX, Y * UnitSpacingY - FormationOffsetY);
        OutLocations.Add(GridLocation);
    }

    // Patch up the last row. If it is not full the units must be centered.
    const int32 UnitsInLastRow = UnitCount % EdgeLengthX;
    if (UnitsInLastRow != 0)
    {
        const float LastRowUnitLocationOffset = (UnitSpacingX * (EdgeLengthX - UnitsInLastRow)) / 2.0f;
        for (int32 i = UnitCount - 1; i >= UnitCount - UnitsInLastRow; --i)
        {
            OutLocations[i].X += LastRowUnitLocationOffset;
        }
    }

    // Rotate and translate the local grid location to the target locations.
    //

    // Calculate the polar angle of delta (in radians).
    const float Angle = FMath::Atan2(Direction.Y, Direction.X) + HALF_PI;

    // Create a complex number to rotate the grid position.
    const FQuat2D Rotator(Angle);

    for (int32 i = 0; i < UnitCount; ++i)
    {
        // Rotate the grid positions around (0,0) and than add the target location.
        OutLocations[i] = Rotator.TransformPoint(OutLocations[i]) + TargetLocation;
    }
}

FVector2D URTSMoveOrder::GetCenterOfGroup(const TArray<AActor*>& Actors) const
{
    FVector2D Locations = FVector2D::ZeroVector;
    int32 NumActors = Actors.Num();

    for (auto Actor : Actors)
    {
        if (!IsValid(Actor))
        {
            --NumActors;
            continue;
        }

        FVector ActorLocation = Actor->GetActorLocation();
        Locations.X += ActorLocation.X;
        Locations.Y += ActorLocation.Y;
    }

    return Locations / NumActors;
}

int32 URTSMoveOrder::GetFormationRank(const AActor* Unit) const
{
    if (!IsValid(Unit))
    {
        return 0;
    }

    // NOTE(np): In A Year Of Rain, units have specific formation ranks when moving.
    //const URTSFormationRankComponent* FormationRankComponent = Unit->FindComponentByClass<URTSFormationRankComponent>();
    //if (FormationRankComponent != nullptr)
    //{
    //    return FormationRankComponent->Rank;
    //}

    return 0;
}
