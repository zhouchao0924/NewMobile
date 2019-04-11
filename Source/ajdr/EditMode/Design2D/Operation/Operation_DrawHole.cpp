

#include "ajdr.h"
#include "Operation_DrawHole.h"

UOperation_DrawDoor::UOperation_DrawDoor(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOperation_DrawDoor::InputKey(FViewport *Viewport, FKey Key, EInputEvent Event)
{
}

void UOperation_DrawDoor::InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

UOperation_DrawWindow::UOperation_DrawWindow(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UOperation_DrawWindow::InputKey(FViewport *Viewport, FKey Key, EInputEvent Event)
{
}

void UOperation_DrawWindow::InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime)
{
}


