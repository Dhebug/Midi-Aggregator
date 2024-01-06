
#pragma once


#include "VisualMidiDevice.h"



class MetronomeDevice : public VisualMidiDevice
{

public:
	MetronomeDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{
		SetBpm(100);
	}

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(11,226, 125, 60); }
	virtual void Draw(sf::RenderWindow& window) override;


	void LoadSound(const std::string& fullPath);

	void StartStop();
	void SetBpm(int bpm);
	void SetVolume(float volume);

protected:
	bool CheckTick();

protected:
	sf::SoundBuffer		m_TicSound;
	sf::Sound			m_Sound;
	sf::Clock			m_MetronomeClock;
	sf::Time            m_Elapsed_time;
	sf::Time			m_Delta_time;
	bool                m_IsOn = false;
	int                 m_BPM = 100;
	int					m_TickSignal = 0;
	int                 m_Length = 8;
	int                 m_Position = 0;
	float               m_Volume = 50.0f;
};

// 1 beat per minute
// 1 beat per 60 seconds

