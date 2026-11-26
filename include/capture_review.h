#ifndef CAPTURE_REVIEW_H
#define CAPTURE_REVIEW_H

#include "script.h"
#include "pokemon_storage_system.h"

void LONG_CALL CaptureReview_Record(PCStorage *storage, int box, int slot, u32 pid);
u16 LONG_CALL CaptureReview_Apply(SCRIPTCONTEXT *ctx, u16 action);
BOOL LONG_CALL CaptureReview_ShowSummary(SCRIPTCONTEXT *ctx);

#endif
