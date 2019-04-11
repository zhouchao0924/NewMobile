
#include "ajdr.h"
#include "Protocal.h"
#include "DRGameMode.h"
#include "AsyncTask/ResAsyncTaskMgr.h"

UProtocalImpl::UProtocalImpl(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UProtocalImpl *UProtocalImpl::GetProtocal(UObject *Obj)
{
// 	UWorld *MyWorld = Obj ? Obj->GetWorld() : NULL;
// 	UCEditorGameInstance *MyGame = Cast<UCEditorGameInstance>(MyWorld ? MyWorld->GetWorld() : NULL);
// 	if (MyGame)
// 	{
// 		return MyGame->Context.Protocal;
// 	}
	return NULL;
}

void UProtocalImpl::CallProtocal(UVaRestJsonObject * JsonObj, FProtocalDelegate Delegate)
{
	FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
}

