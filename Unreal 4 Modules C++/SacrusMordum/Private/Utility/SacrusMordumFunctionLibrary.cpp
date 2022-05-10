// Copyright Robert Zygmunt Uszynski 2020-2021

#include "Utility/SacrusMordumFunctionLibrary.h"

AFixedCameraDelegator * USacrusMordumFunctionLibrary::GetCameraDelegatorByName(UObject * WorldContextObject, FName CameraDelegatorName)
{
	/* Do nothing if there is no world context object or engine pointer is null. */
	if (WorldContextObject == nullptr || GEngine == nullptr) return nullptr;

	/* Test to see if object has a world.*/
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) return nullptr;

	/* For all fixed camera delegators... */
	for (TObjectIterator<AFixedCameraDelegator> Itr; Itr; ++Itr)
	{
		AFixedCameraDelegator* CurrentFixedCameraDelegator = *Itr;

		/* ... if it is not valid or its world is not the same as the world context object's world, continue. */
		if (!IsValid(CurrentFixedCameraDelegator) || CurrentFixedCameraDelegator->GetWorld() != World) continue;

		/* ... if its name is the same as the CameraDelegatorName, return it. */
		if (CurrentFixedCameraDelegator->CameraDelegatorName.IsEqual(CameraDelegatorName)) return CurrentFixedCameraDelegator;
	}

	/* ... return nullptr if the fixed camera delegator could not be found.*/
	return nullptr;
}