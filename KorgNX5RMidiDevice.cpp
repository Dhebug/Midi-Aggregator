
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
#include "KorgNX5RMidiDevice.h"



void KorgNX5RMidiDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	sf::Transform transform = getTransform();

	if (m_MidiOutSignal)
	{
		sf::Transform ledTransform(transform);
		ledTransform.translate(398.f, 74.f);

		sf::CircleShape circle;
		circle.setRadius(3.f);
		circle.setFillColor(sf::Color(m_MidiOutSignal*4,0,0));
		circle.setOutlineColor(sf::Color::Black);
		circle.setOutlineThickness(1.f);
		circle.setOrigin(3.f, 3.f);
		window.draw(circle, ledTransform);

		m_MidiOutSignal--;
	}
}
