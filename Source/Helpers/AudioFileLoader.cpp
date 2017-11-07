#include "AudioFileLoader.h"

AudioFileLoader::AudioFileLoader() : cache(20) {
  formatManager.registerBasicFormats();
}

AudioThumbnail* AudioFileLoader::createThumbnail() {
  return new AudioThumbnail(512, formatManager, cache);
}

MySample* AudioFileLoader::loadAudioFile() {
  ScopedPointer<MySample> mySample;
  FileChooser chooser("Select a Wave file to play...",
    File::nonexistent,
    "*.wav");
  if (chooser.browseForFileToOpen())
  {
    const File file(chooser.getResult());
    ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
      mySample = new MySample();
      mySample->fileName = file.getFileNameWithoutExtension();
      mySample->fileBuffer.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
      reader->read(&mySample->fileBuffer,
                   0,
                   static_cast<int>(reader->lengthInSamples),
                   0,
                   true,
                   true);
      mySample->position = 0;
      my/Users/od/DocumentsSample->numChannels = reader->numChannels;
      mySample->thumbnail = createThumbnail();
      mySample->thumbnail->addBlock(0, mySample->fileBuffer, 0, mySample->fileBuffer.getNumSamples());
      return mySample.release();
    }
    else
    {
      // handle the error that the file is 2 seconds or longer..
    }
  }
  return nullptr;
}
