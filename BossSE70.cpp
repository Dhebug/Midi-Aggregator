
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
#include "BossSE70.h"


namespace se70
{
	const int None = 0;

	struct EffectData
	{
		const char* effectName="";
		int         effectFlag=None;
	};

	const EffectData g_EffectData[145] =
	{
		{"Heavy Metal",None},
		{"Deep Chorus",None},
		{"Bell Guitar",None},
		{"Overdub Echo",None},
		{"5th Drive",None},
		{"Vintage OD-1",None},
		{"Poundcake",None},
		{"Space Phase",None},
		{"Mellow Drive",None},
		{"Panning Wah",None},
		{"Tube Stack",None},
		{"Vibrato Gt",None},
		{"Reverse PS",None},
		{"Fuzz",None},
		{"Gt Synth?",None},
		{"'70s Rock",None},
		{"Tube Crunch",None},
		{"Sitar",None},
		{"Metal Flange",None},
		{"Beat Delay",None},
		{"Octave Drive",None},
		{"Mellow Tone",None},
		{"?ry Cat",None},
		{"Chorus Drive",None},
		{"Gate Drive",None},
		{"'70s Fusion",None},
		{"Rockabilly",None},
		{"Slow Attack",None},
		{"Jazz Tone",None},
		{"'60s Tremolo",None},
		{"Stereo Drive",None},
		{"Cutting",None},
		{"Rotary EXP",None},
		{"Arming EXP",None},
		{"CH1<->CH2 EXP",None},
		{"Wah Dist EXP",None},
		{"Feedback EXP",None},
		{"Funk Wah EXP",None},
		{"E.Acoustic 1",None},
		{"E.Acoustic 2",None},
		{"Synth Horn",None},
		{"Synth Lead",None},
		{"Fantasy Bell",None},
		{"Monster Cry",None},
		{"Synth+Guitar",None},
		{"Slap Bass",None},
		{"Mellow Bass",None},
		{"Metal Bass",None},
		{"Synth Bass 1",None},

		{"Heavy Metal",None},
		{"Deep Chorus",None},
		{"Standard Jazz",None},
		{"Bell Guitar",None},
		{"Overdub Echo",None},
		{"5th Drive",None},
		{"Vintage OD-1",None},
		{"Poundcake",None},
		{"Space Phase",None},

		{"Mellow Drive",None},
		{"Tight Dist.",None},
		{"Blue Dist.",None},
		{"Fuzz",None},
		{"Slap Dist.",None},
		{"Power Leads",None},
		{"Chasing Leads",None},
		{"Power Stack",None},
		{"Symphonic Dist.",None},
		{"Turbo Drive",None},

		{"Chasing Rhythm",None},
		{"Stereo Dist.",None},
		{"Fusion Dist.",None},
		{"Boogie Room",None},
		{"Buzz Backer",None},
		{"Liquid Dist.",None},
		{"Electronic Chords",None},
		{"Clean Repeat",None},
		{"Sweet Swirl",None},
		{"Mild Motion",None},

		{"Pearly Chords",None},
		{"Crisp Chords",None},
		{"Sharp Chords",None},
		{"Tremolo",None},
		{"Sweet Flange",None},
		{"Chord-Clouds",None},
		{"Light Symphonic",None},
		{"Clean Acoustic",None},
		{"Acoustic Solo",None},
		{"Lush Strings",None},

		{"Soft Focus",None},
		{"Brass Room",None},
		{"Brass Burst",None},
		{"Trumpet Flange",None},
		{"Brass Energizer",None},
		{"Echo Rhythm",None},
		{"Symphonic Hall",None},
		{"Horror House",None},
		{"Sitar",None},
		{"Staccato Vibe",None},

		{"Sweep Gate",None},
		{"Monk Akka!",None},
		{"Straight Bass",None},
		{"Slap Bass",None},
		{"Fretless Bass",None},
		{"Trad. Bass",None},
		{"Sax Solo",None},
		{"Vocal Reverb",None},
		{"Drum Gate/Rev",None},
		{"Tight Snare",None},
		{"Rock Drums",None}, };
}




std::string BossSE70MidiDevice::GetProgramName(int program) const
{
	if ((program < 0) || (program > 145))
	{
		return "";
	}
	return se70::g_EffectData[program].effectName;
}

/*
void BossSE70MidiDevice::SetSelectedProgram(int selectedProgram)
{
	// Send the message
	VisualMidiDevice::SetSelectedProgram(selectedProgram);

	m_EffectFlags = fx500::g_EffectData[m_SelectedProgram].effectFlag;
}


void BossSE70MidiDevice::ToggleFlag(int flag)
{

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

void BossSE70MidiDevice::SendControlChange(unsigned char controlNumber, unsigned char controlValue)
{
	std::vector<unsigned char> message;
	message.push_back(0xB0);
	message.push_back(controlNumber);
	message.push_back(controlValue);
	SendMessage(message);
}

void BossSE70MidiDevice::SendProgramChange(unsigned char programNumber)
{
	std::vector<unsigned char> message;
	message.push_back(0xC0);
	message.push_back(programNumber);
	SendMessage(message);
}

void BossSE70MidiDevice::SendChannelPressure(unsigned char pressure)
{
	std::vector<unsigned char> message;
	message.push_back(0xD0);
	message.push_back(pressure);
	SendMessage(message);
}
*/



void BossSE70MidiDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	sf::Transform transform = getTransform();

	/*
	using namespace se70;
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
	*/
}
