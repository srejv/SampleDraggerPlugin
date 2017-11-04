#pragma once

#include "JuceHeader.h"
#include "../Models/MySample.h"
#include "SamplePool.h"

class AudioFileLoader {
public:
	AudioFileLoader();
	AudioThumbnail* createThumbnail();
	MySample* loadAudioFile();

	int loadAudioFileIntoPool(SamplePool& pool) {
		FileChooser chooser("Select a Wave file to play...", File::nonexistent, "*.wav");
		if (chooser.browseForFileToOpen())
		{
			const File file(chooser.getResult());
			return pool.loadFile(file);
		}
		return -1;
	}

private:
	AudioThumbnailCache cache;
	AudioFormatManager formatManager;
};
