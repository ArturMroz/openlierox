/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Sounds header file
// Created 29/7/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


sfxgame_t	sfxGame;
sfxgen_t	sfxGeneral;


bool InitSoundSystem(int rate, int channels, int buffers) {
	// HINT: other SDL stuff is already inited, we don't care here
	if( SDL_Init(SDL_INIT_AUDIO) != 0 ) {
		printf("InitSoundSystem: Unable to initialize SDL-sound: %s\n", SDL_GetError());
		return false;
	}
	
	if(Mix_OpenAudio(rate, AUDIO_S16, channels, buffers)) {
		printf("InitSoundSystem: Unable to open audio (SDL_mixer): %s\n", Mix_GetError());
    	return false;
	}
		
	return true;
}

bool StartSoundSystem() {
	// TODO start sound
	return true;
}

bool StopSoundSystem() {
	// TODO stop sound
	return true;
}

bool SetSoundVolume(int vol) {
	// TODO set volume
	return true;
}

bool QuitSoundSystem() {
	Mix_CloseAudio();
	return true;
}

SoundSample* LoadSoundSample(char* filename, int maxsimulplays) {
	Mix_Chunk* sample = Mix_LoadWAV(filename);
	if(!sample) {
		// TODO: only print this error, if file is existing
		//printf("LoadSoundSample: Error while loading %s: %s\n", filename, Mix_GetError());
		return NULL;
	}
		
	SoundSample* ret = new SoundSample;
	ret->sample = sample;
	ret->maxsimulplays = maxsimulplays;
	return ret;
}

bool FreeSoundSample(SoundSample* sample) {
	// no sample, so we are ready
	if(!sample) return true;
	
	if(!sample->sample) {
		Mix_FreeChunk(sample->sample);
		sample->sample = NULL;	
	}
	delete sample;
	return true;
}

bool PlaySoundSample(SoundSample* sample) {
	if(sample == NULL || sample->sample == NULL)
		return false;

	if(Mix_PlayChannel(-1, sample->sample, 0) != 0) {
		//printf("PlaySoundSample: Error: %s\n",Mix_GetError());
		return false;
	}
	
	return true;
}



///////////////////
// Load the sounds
int LoadSounds(void)
{
	sfxGame.smpNinja = LoadSample("data/sounds/throw.wav",4);
	sfxGame.smpPickup = LoadSample("data/sounds/pickup.wav",2);
	sfxGame.smpBump = LoadSample("data/sounds/bump.wav", 2);
	sfxGame.smpDeath[0] = LoadSample("data/sounds/death1.wav", 2);
	sfxGame.smpDeath[1] = LoadSample("data/sounds/death2.wav", 2);
	sfxGame.smpDeath[2] = LoadSample("data/sounds/death3.wav", 2);

	//sfxGeneral.smpChat = LoadSample("data/sounds/chat.wav",2);
	sfxGeneral.smpClick = LoadSample("data/sounds/click.wav",2);


	return true;
}


///////////////////
// Play a sound in the viewport
void StartSound(SoundSample* smp, CVec pos, int local, int volume, CWorm *me)
{
	int pan = 0;
	int maxhearing = 750;	// Maximum distance for hearing

	// If this wasn't a sound by me, setup the volume & pan based on position
	if(!local) {
		/*float side = pos.GetX() - me->getPos().GetX();
		float distance = CalculateDistance(pos,me->getPos());

		// To far
		if(distance > maxhearing)
			return;

		volume = (int)(100.0f*(1.0f-distance/maxhearing));
		pan = (int)(100*(side/maxhearing));*/

		
		// Check if it's in the viewport
		/*CViewport *v = me->getViewport();
		int wx = v->GetWorldX();
		int wy = v->GetWorldY();
		int l = v->GetLeft();
		int t = v->GetTop();

		// Are we inside the viewport?
		int x = (int)pos.GetX() - wx;
		int y = (int)pos.GetY() - wy;
		x*=2;
		y*=2;

		if(x+l+10 < l || x-10 > v->GetVirtW())
			return;
		if(y+t+10 < t || y-10 > v->GetVirtH())
			return;*/
	}

	// TODO: implement a PlayExSoundSample for this
	// this was the old call (using BASS_SamplePlayEx):
	//PlayExSampleSoundEx(smp,0,-1,volume,pan,-1);
	// we are using a workaround here
	PlaySoundSample(smp);
}
