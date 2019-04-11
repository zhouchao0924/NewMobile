
#include "ajdr.h"
#include "CameraMod.h"
#include "DREditorViewportClient.h"

FCameraMod::FCameraMod()
	:ViewportClient(nullptr)
{
}

void FCameraMod::OnEnter()
{
	if (ViewportClient)
	{
		if (SavedState.bSaved)
		{
			ViewportClient->CameraMgr.SetViewType(SavedState.bProjView);
			ViewportClient->CameraMgr.SetViewLocation(SavedState.ViewLoc);
			ViewportClient->CameraMgr.SetViewRotation(SavedState.ViewRotation);
			ViewportClient->CameraMgr.SetOrthoScale(SavedState.OrthoScale);
		}
		else
		{
			Reset();
		}
	}
}

void FCameraMod::OnLeave()
{
	if (ViewportClient)
	{
		SavedState.bSaved = true;
		SavedState.bProjView = ViewportClient->CameraMgr.IsProjView();
		ViewportClient->CameraMgr.GetViewPoint(SavedState.ViewLoc, SavedState.ViewRotation);
		SavedState.OrthoScale = ViewportClient->CameraMgr.GetOrthoScale();
	}
}

void FCameraMod::SetViewportClient(FDREditorViewportClient *InViewportClient)
{
	ViewportClient = InViewportClient;
}


