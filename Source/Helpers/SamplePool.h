#pragma once

#include "JuceHeader.h"
#include "../Models/SampleData.h"

#include <vector>

class SamplePool {
public:
  SamplePool() {
    formatManager.registerBasicFormats();
  }

  SampleData& getSample(int index) {
    if(samplePool.size()  > index) {
      return samplePool[index];
    }
    return empty;
  }

  size_t size() {
	  return samplePool.size();
  }

  SampleData empty;

  int loadFile(const File& file) {
    ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr) return -1;

    const double duration = reader->lengthInSamples / reader->sampleRate;
    SampleData data;
    data.name = file.getFileNameWithoutExtension();
    data.buffer.setSize(reader->numChannels, reader->lengthInSamples);
    reader->read(&data.buffer, 0, reader->lengthInSamples, 0, true, true);

    samplePool.push_back(data);
    return samplePool.size() - 1;
  }

private:
  std::vector<SampleData> samplePool;
  AudioFormatManager formatManager;
};
