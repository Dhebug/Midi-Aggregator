
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
#include "Metronome.h"


extern sf::Font* g_Font;   // Hack


void MetronomeDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	CheckTick();

	sf::String message = "Speed: " + std::to_string(m_BPM) + "bpm";
	message += "\r\nStep: " + std::to_string(m_Position% m_Length) + "/" + std::to_string(m_Length);
	message += "\r\nVolume: " + std::to_string(int(m_Volume)
	);

	

	sf::FloatRect displayBox = GetDisplayBox();
	if (displayBox.left >= 0)
	{
		sf::Text text;
		text.setFont(*g_Font); // font is a sf::Font
		text.setString(message);
		text.setCharacterSize(16); // in pixels, not points!
		if (m_IsOn)
		{
			text.setFillColor(sf::Color::White);
		}
		else
		{
			text.setFillColor(sf::Color::Red);
		}

		//text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		text.setPosition(displayBox.left + 1, displayBox.top + 1);
		window.draw(text, getTransform());   // inside the main loop, between window.clear() and window.display()

		if (m_TickSignal)
		{
			text.setFillColor(sf::Color(m_TickSignal * 4, m_TickSignal * 4, 0));
			m_TickSignal--;
		}
		else
		{
			text.setFillColor(sf::Color::Black);
		}
		text.setPosition(displayBox.left, displayBox.top);
		window.draw(text, getTransform());   // inside the main loop, between window.clear() and window.display()
	}

}



void MetronomeDevice::LoadSound(const std::string& fullPath)
{
	if (!m_TicSound.loadFromFile(fullPath))
	{
		throw std::runtime_error("Could not load " + fullPath);
	}
}


void MetronomeDevice::StartStop()
{
	m_IsOn = !m_IsOn;

	if (m_IsOn)
	{
		m_Sound.setVolume(m_Volume);
		m_Sound.setBuffer(m_TicSound);
		m_Sound.play();
		m_MetronomeClock.restart();
		m_Elapsed_time = sf::milliseconds(0);
		m_Position = 0;
	}
	else
	{
		m_Sound.stop();
	}
}


void MetronomeDevice::SetBpm(int bpm)
{
	m_BPM = bpm;
	m_Delta_time = sf::milliseconds( (1000*60) / m_BPM);
}

void MetronomeDevice::SetVolume(float volume)
{
	m_Volume = volume;
}



bool MetronomeDevice::CheckTick()
{
	if (m_IsOn)
	{
		m_Elapsed_time += m_MetronomeClock.restart();

		// Have we passed the delay we wanted?
		if (m_Elapsed_time >= m_Delta_time)
		{
			// Subtract the time consumed (can be larger if we were paused in the debugger for example)
			while (m_Elapsed_time >= m_Delta_time)
			{
				m_Elapsed_time -= m_Delta_time;
			}
				
			m_Position++;
			m_Sound.setBuffer(m_TicSound);

			if ((m_Position % m_Length)==0)
			{
				m_Sound.setVolume(m_Volume);
				std::cout << "-- TICK --" << std::endl;
			}
			else
			{
				m_Sound.setVolume(m_Volume/4);
			}

			m_Sound.play();

			m_TickSignal = 63;
			return true;
		}
	}
	return false;
}

