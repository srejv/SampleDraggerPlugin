#pragma once

#include "JuceHeader.h"
#include "DataModel.h"

class AudioFileLoader {
public:
	AudioFileLoader();
	AudioThumbnail* createThumbnail();
	MySample* loadAudioFile();

private:
	AudioThumbnailCache cache;
	AudioFormatManager formatManager;
};
