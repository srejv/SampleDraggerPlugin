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
      const double duration = reader->lengthInSamples / reader->sampleRate;

      if (duration < 20)
      {
        mySample = new MySample();
        mySample->fileName = file.getFileNameWithoutExtension();
        mySample->fileBuffer.setSize(reader->numChannels, reader->lengthInSamples);
        reader->read(&mySample->fileBuffer,
          0,
          reader->lengthInSamples,
          0,
          true,
          true);
        mySample->position = 0;
        mySample->numChannels = reader->numChannels;
        mySample->thumbnail = createThumbnail();
        mySample->thumbnail->setSource(new FileInputSource(file));
        return mySample.release();
      }
      else
      {
        // handle the error that the file is 2 seconds or longer..
      }
    }
  }
  return nullptr;
}
