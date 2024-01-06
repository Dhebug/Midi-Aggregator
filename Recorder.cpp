
#define SFML_STATIC 


#include <iostream>
#include <cstdlib>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#pragma warning (disable:4996)

#include "VisualMidiDevice.h"
#include "Recorder.h"


extern sf::Font* g_Font;   // Hack


bool RecorderDevice::SetSoundRecorder(const std::string& deviceName)
{
	std::vector<std::string> devices = sf::SoundRecorder::getAvailableDevices();

	if (!m_SoundBufferRecorder.setDevice(deviceName))
	{
		throw std::runtime_error("Can't setDevice " + deviceName);
	}

	// "Microphone (AT2020USB+)"
    // "Line (Xonar U7)"

	return false;
}


void RecorderDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	//CheckTick();

	const sf::SoundBuffer& buffer = m_SoundBufferRecorder.getBuffer();

	sf::String message = "Track: " + std::to_string(m_Track) ;
	message += "\r\nVolume: " + std::to_string(int(m_Volume));
	message += "\r\nPlaying: " + std::to_string(int(m_IsPlaying));
	message += "\r\nRecording: " + std::to_string(int(m_IsRecording));
	message += "\r\nPaused: " + std::to_string(int(m_IsPaused));
	message += "\r\nLooping: " + std::to_string(int(m_Islooping));
	message += "\r\nSize: " + std::to_string(buffer.getDuration().asSeconds());

	


	sf::FloatRect displayBox = GetDisplayBox();
	if (displayBox.left >= 0)
	{
		sf::Text text;
		text.setFont(*g_Font); // font is a sf::Font
		text.setString(message);
		text.setCharacterSize(12); // in pixels, not points!
		text.setFillColor(sf::Color::White);
		//text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		text.setPosition(displayBox.left + 1, displayBox.top + 1);
		window.draw(text, getTransform());   // inside the main loop, between window.clear() and window.display()

		text.setFillColor(sf::Color::Green);
		text.setPosition(displayBox.left, displayBox.top);
		window.draw(text, getTransform());   // inside the main loop, between window.clear() and window.display()
	}
}



void RecorderDevice::SetVolume(float volume)
{
	m_Volume = volume;
}



bool RecorderDevice::CheckTick()
{
	return false;
}



void RecorderDevice::Record()
{
	m_IsRecording = true;
	m_IsPlaying   = false;
	m_SoundBufferRecorder.start();
}

void RecorderDevice::Stop()
{
	m_IsPlaying   = false;
	m_IsRecording = false;
	m_SoundBufferRecorder.stop();
	m_SoundPlayBack.stop();
}

void RecorderDevice::Pause()
{
	m_IsPaused = !m_IsPaused;
	//m_SoundBufferRecorder.
}

void RecorderDevice::ToggleLooping()
{
	m_Islooping = !m_Islooping;
}



void RecorderDevice::Playback() 
{
	m_IsPlaying   = true;
	if (m_IsRecording)
	{
		Stop();
	}
	const sf::SoundBuffer& buffer = m_SoundBufferRecorder.getBuffer();
	m_SoundPlayBack.setBuffer(buffer);
	m_SoundPlayBack.setLoop(m_Islooping);
	m_SoundPlayBack.play();
}

