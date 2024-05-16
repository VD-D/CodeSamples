// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "Types/AutobattlerStructs.h"

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FIntPair& Pair)
{
    return FCrc::MemCrc32(&Pair, sizeof(FIntPair));
}
#endif
