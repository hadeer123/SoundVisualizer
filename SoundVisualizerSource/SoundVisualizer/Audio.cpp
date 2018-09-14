#include "Audio.h"
/**
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Handles all audio oprations
 */

Audio::Audio(void)
{
	//...
}


Audio::~Audio(void)
{
	//...
}

// Initializing the FMOD
void Audio::initializeAudioSystem(void)
{
	// creates system object
	result = FMOD::System_Create(&system);
    ERRCHECK(result);

	// get and checking FMOD version
    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    }

	// init FMOD system
    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
}

// Check the result object for error
void Audio::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

// Clear sound system
void Audio::clearAudioSystem(void)
{
	result = sound1->release();
    ERRCHECK(result);
    result = sound2->release();
    ERRCHECK(result);
    result = sound3->release();
	ERRCHECK(result);
    result = sound4->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);
}

// Loading sound track
int Audio::loadingSound(const char* path, unsigned int slot)
{
	if(slot > 3) return 0;

	if(slot == 1)
	{
		result = system->createSound(path, FMOD_HARDWARE, 0, &sound1);
		ERRCHECK(result);

		result = sound1->setMode(FMOD_LOOP_OFF);   
		ERRCHECK(result);
	}
	else if(slot == 2)
	{
		result = system->createSound(path, FMOD_HARDWARE, 0, &sound2);
		ERRCHECK(result);

		result = sound2->setMode(FMOD_LOOP_OFF);   
		ERRCHECK(result);
	}
	else if(slot == 3)
	{
		result = system->createSound(path, FMOD_HARDWARE, 0, &sound3);
		ERRCHECK(result);

		result = sound3->setMode(FMOD_LOOP_OFF);   
		ERRCHECK(result);
	}
	else if(slot == 4)
	{
		result = system->createSound(path, FMOD_HARDWARE, 0, &sound4);
		ERRCHECK(result);

		result = sound4->setMode(FMOD_LOOP_OFF);   
		ERRCHECK(result);
	}
	
	std::cout << "sound loaded: \"" << path << "\"" << endl;
	return 1;
}

// Play sound track
int Audio::playSound(unsigned int slot, float volume)
{
	if(slot > 4) return 0;

	channel->setVolume(0);

	if(slot == 1)
	{
		result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
		ERRCHECK(result);
	}
	else if(slot == 2)
	{
		result = system->playSound(FMOD_CHANNEL_FREE, sound2, false, &channel);
		ERRCHECK(result);
	}
	else if(slot == 3)
	{
		result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel);
		ERRCHECK(result);
	}
	else if(slot == 4)
	{
		result = system->playSound(FMOD_CHANNEL_FREE, sound4, false, &channel);
		ERRCHECK(result);
	}

	channel->setVolume(volume);

	system->update();

	return 1;
}

// Stop sound track
int Audio::stopSound(unsigned int slot)
{
	channel->setVolume(0);
	return 1;
}

FMOD::Channel* Audio::getChannel(unsigned int chan)
{
	return channel;
}
