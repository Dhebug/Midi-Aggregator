
#pragma once


#include "VisualMidiDevice.h"



class RecorderDevice : public VisualMidiDevice
{
public:
	RecorderDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{
	}

	bool SetSoundRecorder(const std::string& deviceName);


	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(5,40, 80, 172); }
	virtual void Draw(sf::RenderWindow& window) override;

	void SetVolume(float volume);
	void ToggleLooping();

	void Record();
	void Stop();
	void Pause();
	void Playback();

protected:
	bool CheckTick();

protected:
	sf::SoundBufferRecorder    m_SoundBufferRecorder;
	sf::Sound                  m_SoundPlayBack;

	float               m_Volume = 100.0f;

	int                 m_Track = 0;

	bool                m_IsPlaying = false;
	bool                m_IsRecording = false;
	bool                m_IsPaused = false;
	bool                m_Islooping = false;

};

// 1 beat per minute
// 1 beat per 60 seconds

