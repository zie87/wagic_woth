//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------
#include "DebugRoutines.h"

#define FSOUND_FREE 0

#include "../../include/JSoundSystem.h"
#include "../../include/JFileSystem.h"

//////////////////////////////////////////////////////////////////////////
JMusic::JMusic()
#ifdef USE_PHONON
  : mOutput(0), mMediaObject(0)
#endif
{
}

void JMusic::Update(){

}

int JMusic::getPlayTime(){
  return 0;
}

JMusic::~JMusic()
{
#ifdef USE_PHONON
  if(mOutput)
    delete mOutput;
  if(mMediaObject)
    delete mMediaObject;
#endif
}

#ifdef USE_PHONON
void JMusic::seekAtTheBegining()
{
  mMediaObject->seek(0);
}
#endif

//////////////////////////////////////////////////////////////////////////
JSample::JSample()
#ifdef USE_PHONON
  : mOutput(0), mMediaObject(0)
#endif
{

}

JSample::~JSample()
{
#ifdef USE_PHONON
  if(mOutput)
    delete mOutput;
  if(mMediaObject)
    delete mMediaObject;
#endif
}

unsigned long JSample::fileSize()
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////
JSoundSystem* JSoundSystem::mInstance = NULL;

JSoundSystem* JSoundSystem::GetInstance()
{
  if (mInstance == NULL)
    {
      mInstance = new JSoundSystem();
      mInstance->InitSoundSystem();
    }
  return mInstance;
}


void JSoundSystem::Destroy()
{
  if (mInstance)
    {
      mInstance->DestroySoundSystem();
      delete mInstance;
      mInstance = NULL;
    }
}


JSoundSystem::JSoundSystem()
{
  mVolume = 0;
  mSampleVolume = 0;
}

JSoundSystem::~JSoundSystem()
{
}

void JSoundSystem::InitSoundSystem()
{
}


void JSoundSystem::DestroySoundSystem()
{
}


JMusic *JSoundSystem::LoadMusic(const char *fileName)
{
#ifdef USE_PHONON
  JMusic* music = new JMusic();
  if (music)
  {
    music->mOutput = new Phonon::AudioOutput(Phonon::GameCategory, 0);
    music->mMediaObject = new Phonon::MediaObject(0);
    string fullpath = JFileSystem::GetInstance()->GetResourceFile(fileName);
    music->mMediaObject->setCurrentSource(QString(fullpath.c_str()));
    Phonon::Path mediapath = Phonon::createPath(music->mMediaObject, music->mOutput);
    Q_ASSERT(mediapath.isValid());
  }
  return music;
#else
  return NULL;
#endif
}


void JSoundSystem::PlayMusic(JMusic *music, bool looping)
{
#ifdef USE_PHONON
  if (music && music->mMediaObject && music->mOutput)
  {
    if(looping)
    {
      music->mMediaObject->connect(music->mMediaObject, SIGNAL(aboutToFinish()), music, SLOT(seekAtTheBegining()));
    }
    music->mOutput->setVolume((qreal)mVolume*0.01);
    music->mMediaObject->play();

  }
#endif
}


void JSoundSystem::StopMusic(JMusic *music)
{
#ifdef USE_PHONON
  if (music && music->mMediaObject && music->mOutput)
  {
    music->mMediaObject->stop();
  }
#endif
}


void JSoundSystem::PauseMusic(JMusic *music)
{
    StopMusic(music);
}


void JSoundSystem::ResumeMusic(JMusic *music)
{
    PlayMusic(music);
}


void JSoundSystem::SetVolume(int volume)
{
  SetMusicVolume(volume);
  SetSfxVolume(volume);
}

void JSoundSystem::SetMusicVolume(int volume)
{
  mVolume = volume;
}

void JSoundSystem::SetSfxVolume(int volume){
  mSampleVolume = volume;
  SetMusicVolume(mVolume);
}

JSample *JSoundSystem::LoadSample(const char *fileName)
{
#if (defined USE_PHONON)
  JSample* sample = new JSample();
  if (sample)
  {
    sample->mOutput = new Phonon::AudioOutput(Phonon::GameCategory, 0);
    sample->mMediaObject = new Phonon::MediaObject(0);
    string fullpath = JFileSystem::GetInstance()->GetResourceFile(fileName);
    sample->mMediaObject->setCurrentSource(QString(fullpath.c_str()));
    Phonon::Path mediapath = Phonon::createPath(sample->mMediaObject, sample->mOutput);
    Q_ASSERT(mediapath.isValid());
  }
  return sample;
#else
  return NULL;
#endif
}


void JSoundSystem::PlaySample(JSample *sample)
{
#ifdef USE_PHONON
  if (sample && sample->mMediaObject && sample->mOutput)
  {
    sample->mOutput->setVolume((qreal)mSampleVolume*0.01);
    sample->mMediaObject->play();
  }
#endif
}

