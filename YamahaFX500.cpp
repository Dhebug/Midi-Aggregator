
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
#include "YamahaFX500.h"


namespace fx500
{
	const int None = 0;
	const int Comp = 1 << 0;
	const int Dist = 1 << 1;
	const int Eq   = 1 << 2;
	const int Mod  = 1 << 3;
	const int Rev  = 1 << 4;
	const int Reversed = 1 << 5;

	struct EffectData
	{
		const char* effectName;
		int         effectFlag;
	};

	const EffectData g_EffectData[60] =
	{
		{"Broad Dist.", Comp| Dist| Eq| Mod| Rev},
		{"Warm Strings", Comp | Eq | Mod | Rev},
		{"Standard Jazz",Comp | Eq | Rev},
		{"Soft Echo", Dist | Eq | Mod | Rev},
		{"Power Pan",Dist | Mod | Rev | Reversed},
		{"Trad. Dist.",Comp | Dist | Rev},
		{"Dark Dist.",Comp | Dist | Eq | Mod | Rev},
		{"Ring Dist.",Comp | Dist | Eq | Rev},
		{"Metal Overdrive",Comp | Dist | Eq | Rev},

		{"Echo Dist.",Comp | Dist | Eq | Mod | Rev},
		{"Tight Dist.", Dist | Eq | Rev},
		{"Blue Dist.", Dist | Eq | Rev},
		{"Fuzz", Dist | Eq |  Rev},
		{"Slap Dist.", Dist | Eq |  Rev},
		{"Power Leads",Comp | Dist | Eq | Mod | Rev},
		{"Chasing Leads", Dist | Eq | Mod | Rev},
		{"Power Stack",Comp | Dist | Eq | Mod | Rev},
		{"Symphonic Dist.",Comp | Dist | Eq | Mod | Rev},
		{"Turbo Drive",Comp | Dist | Eq | Mod | Rev},

		{"Chasing Rhythm",Comp | Dist | Eq | Rev},
		{"Stereo Dist.",Comp | Dist | Eq |  Rev},
		{"Fusion Dist.", Dist | Eq | Mod | Rev},
		{"Boogie Room",Comp | Dist | Eq | Mod | Rev},
		{"Buzz Backer",Comp | Dist | Eq |  Rev},
		{"Liquid Dist.",Comp | Dist | Eq | Mod | Rev},
		{"Electronic Chords",Comp | Dist | Eq | Mod | Rev},
		{"Clean Repeat",Comp | Eq | Mod | Rev},
		{"Sweet Swirl",Comp | Eq | Mod | Rev},
		{"Mild Motion",Comp | Eq | Mod | Rev},

		{"Pearly Chords",Comp | Eq | Rev},
		{"Crisp Chords",Comp |  Eq | Mod | Rev},
		{"Sharp Chords",Comp | Eq | Mod | Rev},
		{"Tremolo", Dist | Eq | Mod | Rev},
		{"Sweet Flange",Comp | Eq | Mod | Rev},
		{"Chord-Clouds",Comp |  Eq | Mod | Rev | Reversed},
		{"Light Symphonic",Comp |  Eq | Mod | Rev},
		{"Clean Acoustic",Comp | Eq | Mod | Rev},
		{"Acoustic Solo",Comp |  Eq | Mod | Rev},
		{"Lush Strings",Comp | Mod | Rev},

		{"Soft Focus",Comp | Eq | Mod | Rev | Reversed},
		{"Brass Room",Comp |  Eq |  Rev},
		{"Brass Burst",Comp | Dist | Eq | Rev},
		{"Trumpet Flange",Comp | Dist | Eq | Mod | Rev},
		{"Brass Energizer",Comp | Eq |  Rev},
		{"Echo Rhythm",Comp | Eq | Mod | Rev},
		{"Symphonic Hall",Comp | Eq | Mod | Rev},
		{"Horror House", Dist | Eq | Mod | Rev | Reversed},
		{"Sitar",Comp | Dist | Eq | Mod | Rev},
		{"Staccato Vibe",Comp | Eq | Mod | Rev | Reversed},

		{"Sweep Gate",Comp | Dist | Mod | Rev | Reversed},
		{"Monk Akka!", Dist | Eq | Mod | Rev | Reversed},
		{"Straight Bass",Comp | Eq | Mod | Rev},
		{"Slap Bass",Comp | Eq | Mod },
		{"Fretless Bass",Comp | Dist | Eq | Mod | Rev},
		{"Trad. Bass",Comp | Eq | Mod | Rev},
		{"Sax Solo",Comp | Eq | Rev},
		{"Vocal Reverb",Comp | Eq | Rev},
		{"Drum Gate/Rev",Comp |  Eq | Rev},
		{"Tight Snare",Comp | Eq | Rev},
		{"Rock Drums",Comp | Eq | Rev}
	};
}




std::string YamahaFX500MidiDevice::GetProgramName(int program) const
{
	if ((program < 0) || (program > 59))
	{
		return "";
	}
	return fx500::g_EffectData[program].effectName;
}


void YamahaFX500MidiDevice::SetSelectedProgram(int selectedProgram)
{
	// Send the message
	VisualMidiDevice::SetSelectedProgram(selectedProgram);

	m_EffectFlags = fx500::g_EffectData[m_SelectedProgram].effectFlag;
}


void YamahaFX500MidiDevice::ToggleFlag(int flag)
{
	/*
	MFC06

	- Effect:
	1 / Comp  B0 74 00
	2 / Dist  B0 75 00
	3 / Eq    B0 76 00
	4 / Mod   B0 77 00
	5 / Rev   B0 78 00

	- Pgm:
	1 / Comp  C0 00
	2 / Dist  C0 01
	3 / Eq    C0 02
	4 / Mod   C0 03
	5 / Rev   C0 04
	*/

	if (flag < 5)
	{
		m_EffectFlags ^= (1 << flag);

		static unsigned char value = 0;
		std::vector<unsigned char> message;
		message.push_back(0xB0);
		message.push_back(0x74+flag);
		message.push_back(value);
		SendMessage(message);
	}
}

void YamahaFX500MidiDevice::SendControlChange(unsigned char controlNumber, unsigned char controlValue)
{
	std::vector<unsigned char> message;
	message.push_back(0xB0);
	message.push_back(controlNumber);
	message.push_back(controlValue);
	SendMessage(message);
}

void YamahaFX500MidiDevice::SendProgramChange(unsigned char programNumber)
{
	std::vector<unsigned char> message;
	message.push_back(0xC0);
	message.push_back(programNumber);
	SendMessage(message);
}

void YamahaFX500MidiDevice::SendChannelPressure(unsigned char pressure)
{
	std::vector<unsigned char> message;
	message.push_back(0xD0);
	message.push_back(pressure);
	SendMessage(message);
}




void YamahaFX500MidiDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	sf::Transform transform = getTransform();

	using namespace fx500;
	int flags = m_EffectFlags;

	// Draw the status leds
	int intFlags = flags;
	for (int led = 0; led < 5; led++)
	{
		if (intFlags & 1)
		{
			int xOffset = led;
			int yOffset = 0;

			if ((led >= 3) && (flags & Reversed))
			{
				// MOD and REV are moved to the second line and reversed
				yOffset++;
			}

			// https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php
			sf::Transform ledTransform(transform);
			ledTransform.translate(409.f + 47 * xOffset, 15.f + 33 * yOffset);

			sf::CircleShape circle;
			circle.setRadius(5.f);
			circle.setFillColor(sf::Color::Red);
			circle.setOutlineColor(sf::Color::Black);
			circle.setOutlineThickness(1.f);
			circle.setOrigin(5.f, 5.f);
			window.draw(circle, ledTransform);
		}
		intFlags >>= 1;
	}
}
