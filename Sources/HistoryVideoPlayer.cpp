#include "HistoryVideoPlayer.h"
#include "Debugger.h" 

HistoryVideoPlayer::HistoryVideoPlayer()
{
}

HistoryVideoPlayer::HistoryVideoPlayer(VideoManager* manager, byte videoID, DateTime startTime, DateTime endTime)
{
}

HistoryVideoPlayer::~HistoryVideoPlayer()
{
}

void HistoryVideoPlayer::run()
{
	PRINT(ALWAYS_PRINT, "HistoryVideoPlayer", __FUNCTION__, __LINE__);
}

void HistoryVideoPlayer::SetPlaySpeed(byte playSpeed)
{
	PRINT(ALWAYS_PRINT, "HistoryVideoPlayer", __FUNCTION__, __LINE__, "playSpeed = %d", playSpeed);
}
