#pragma once
/**
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Handles all audio oprations
 */

#include <iostream>

#include <FMOD\fmod.hpp>
#include <FMOD\fmod_errors.h>

using namespace std;

class Audio
{
public:
	Audio(void);
	~Audio(void);
	// Initializing the FMOD
	void initializeAudioSystem(void);
	// Clear sound system
	void clearAudioSystem(void);
	// Loading sound track
	int loadingSound(const char* path, unsigned int slot);
	// Play sound track
	int playSound(unsigned int slot, float volume);
	// Stop sound track
	int stopSound(unsigned int slot);
	// Get channel
	FMOD::Channel* getChannel(unsigned int channel);
private:
	// The sound system
	FMOD::System *system;
	// Vars for saving sound tracks
    FMOD::Sound *sound1, *sound2, *sound3, *sound4;
	// Sound channels
    FMOD::Channel *channel;
	// The FMOD result object
    FMOD_RESULT result;

    int key;
    unsigned int version;

	// Check the result object for error
	void ERRCHECK(FMOD_RESULT result);
};

