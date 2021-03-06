// Fill out your copyright notice in the Description page of Project Settings.

#include "ThreeLanes.h"
#include "Grid.h"
#include "GridCell.h"


// Sets default values
AGrid::AGrid()
{
	DimensionX = DimensionY = 0;
	CellSize = FVector2D(1, 1);

	RootComponent = SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
}

void AGrid::InitializeGrid(int32 X, int32 Y)
{
	if (Cells.Num() != 0)
	{
		UE_LOG(LogGrid, Error, TEXT("Attempted to initialize a grid that has already been initialized!"));
		return;
	}
	DimensionX = X;
	DimensionY = Y;

	// We know exactly how much cells we need.
	Cells.Reserve(X * Y);

	for (int32 y = 0; y < Y; y++)
	{
		for (int32 x = 0; x < X; x++)
		{
			AGridCell* NewCell = (AGridCell*)GetWorld()->SpawnActor(AGridCell::StaticClass());
			NewCell->Initialize(this, x, y);
			Cells.Push(NewCell);
			// UE_LOG(LogGrid, Log, TEXT("Initialized cell at (%d, %d)"), x, y);
		}
	}
}

AGridCell* AGrid::GetCell(int32 X, int32 Y)
{
	if (X >= DimensionX || X < 0 || Y >= DimensionY || Y < 0)
	{
		return nullptr;
	}

	return Cells[X + Y*DimensionX];
}

#if WITH_EDITOR
void AGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AGrid, bDrawDebug))
	{
		UE_LOG(LogGrid, Log, TEXT("bDrawDebug got toggled!"));
		for (AGridCell* Cell : Cells)
		{
			Cell->SetDebugDraw(bDrawDebug);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AGridTest::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid(X, Y);

	for (int32 x = 0; x < X; x++)
	{
		for (int32 y = 0; y < Y; y++)
		{
			AGridCell* Cell = GetCell(x, y);
			if (!Cell->IsValidLowLevel())
			{
				UE_LOG(LogGrid, Error, TEXT("GetCell returned no cell at all for coordinates (%d, %d)!"), x, y);
			}
			else if (Cell->X != x || Cell->Y != y)
			{
				UE_LOG(LogGrid, Error, TEXT("GetCell returned incorrect cell. Requested: (%d,%d), Returned: (%d,%d)"), x, y, Cell->X, Cell->Y);
			}
		}
	}
}
