

#include "ajdr.h"
#include "Operation_Select.h"

UOperation_Select::UOperation_Select(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UOperation_Select::InputKey(FViewport *Viewport, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton)
	{		
		FDREditorViewportClient *ViewportClient = GetViewportClient();	
		if (ViewportClient)
		{
			int32 X = Viewport->GetMouseX();
			int32 Y = Viewport->GetMouseY();
			ViewportClient->HitTestByCursor(X, Y);
		}
	}
}

void UOperation_Select::InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime)
{
}


