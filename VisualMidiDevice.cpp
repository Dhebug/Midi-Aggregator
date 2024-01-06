
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



VisualMidiDevice*           VisualMidiDevice::s_SelectedInputDevice = nullptr;
std::set<VisualMidiDevice*>	VisualMidiDevice::s_InputDevices;

VisualMidiDevice*           VisualMidiDevice::s_SelectedOutputDevice = nullptr;
std::set<VisualMidiDevice*>	VisualMidiDevice::s_OutputDevices;



VisualMidiDevice::VisualMidiDevice(const std::string& deviceName) :
	m_DeviceName(deviceName)
{
}


VisualMidiDevice::~VisualMidiDevice()
{
	if (m_MidiInput)
	{
		s_InputDevices.erase(this);
		delete m_MidiInput;
		m_MidiInput = nullptr;
	}
	if (m_MidiOutput)
	{
		s_OutputDevices.erase(this);
		delete m_MidiOutput;
		m_MidiOutput = nullptr;
	}
}


void VisualMidiDevice::SetAsSelectedInput()
{
	assert(m_MidiInput != nullptr);
	s_SelectedInputDevice = this;
}


void VisualMidiDevice::SetAsSelectedOutput()
{
	assert(m_MidiOutput!=nullptr);
	s_SelectedOutputDevice = this;
}


RtMidiIn* VisualMidiDevice::SetMidiInput(const std::string& deviceName)
{
	if (m_MidiInput)
	{
		s_InputDevices.erase(this);
		delete m_MidiInput;
		m_MidiInput = nullptr;
	}
	m_MidiInput = new RtMidiIn();
	std::string portName;
	unsigned int nPorts = m_MidiInput->getPortCount();
	for (unsigned int i = 0; i < nPorts; i++)
	{
		portName = m_MidiInput->getPortName(i);
		if (deviceName == portName)
		{
			m_MidiInput->openPort(i);
			m_MidiInput->ignoreTypes(false, false, false);		// Don't ignore sysex, timing, or active sensing messages.
			s_InputDevices.insert(this);
			if (s_SelectedInputDevice == nullptr)
			{
				SetAsSelectedInput();
			}
			return m_MidiInput;
		}
	}
	std::cout << "Warning: Device " << deviceName << " is not detected " << std::endl;
	return nullptr;
}


RtMidiOut* VisualMidiDevice::SetMidiOutput(const std::string& deviceName)
{
	if (m_MidiOutput)
	{
		s_OutputDevices.erase(this);
		delete m_MidiOutput;
		m_MidiOutput = nullptr;
	}
	m_MidiOutput = new RtMidiOut();
	std::string portName;
	unsigned int nPorts = m_MidiOutput->getPortCount();
	for (unsigned int i = 0; i < nPorts; i++)
	{
		portName = m_MidiOutput->getPortName(i);
		if (deviceName == portName)
		{
			m_MidiOutput->openPort(i);
			s_OutputDevices.insert(this);
			if (s_SelectedOutputDevice == nullptr)
			{
				SetAsSelectedOutput();
			}
			SetSelectedProgram(m_SelectedProgram);
			return m_MidiOutput;
		}
	}
	std::cout << "Warning: Device " << deviceName << " is not detected " << std::endl;
	return nullptr;
}




void VisualMidiDevice::PrintMessages(const std::vector<unsigned char>& message, double stamp)
{
	auto nBytes = message.size();
	for (int i = 0; i < nBytes; i++)
	{
		std::cout << "Byte " 
#if 0
			<< std::hex 
#endif
			<< i << " = " << (int)message[i] << ", ";
	}
	if (nBytes > 0)
	{
		std::cout << "stamp = " << stamp << " device = " << m_DeviceName << std::endl;
	}
}

void VisualMidiDevice::ProcessInput(VisualMidiDevice* output)
{
	std::vector<unsigned char> message;
    double stamp = m_MidiInput->getMessage(&message);
    auto nBytes = message.size();

	bool shouldForward = false;
	bool shouldPrint = false;
    if (nBytes)
    {
        // https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
        switch (message[0])
        {
        case 254:
            // Active Sensing. This message is intended to be sent repeatedly to tell the receiver that a connection is alive. 
            // Use of this message is optional. When initially received, the receiver will expect to receive another Active Sensing message each 300ms (max), and if it does not then it will assume that the connection has been terminated. 
            // At termination, the receiver will turn off all voices and return to normal (non- active sensing) operation. 
            // -> Sent by the Refact
            break;

		case 240:
			// System Exclusive messages
			ProcessSysExMessage(message, stamp);
			shouldPrint = true;
			break;

		case 176:
			// Control Changes
			// AKAI MPK 176 / 1-8 / 0-127  <- K1 to K8 control knobs
			if (nBytes > 2)
			{
				m_ControlChange[message[1]] = message[2];
			}
			shouldForward = true;
			shouldPrint = true;
			break;

		case 144: // Note ON
			if (nBytes > 2)
			{
				// Note ON : 144 / 48 / 128 - 144 / 72 / 128
				m_Notes[message[1]] = message[2];
			}
			shouldForward = true;
			shouldPrint = true;
			break;

		case 128: // Note OFF
			if (nBytes > 2)
			{
				// Note OFF : 128 / 48 / 0 - 128 / 72 / 0
				m_Notes[message[1]] = message[2];
			}
			shouldForward = true;
			shouldPrint = true;
			break;

		default:
			shouldForward = true;
			shouldPrint = true;
            break;
        }
		if (shouldForward && output)
		{
			output->SendMessage(message);
		}
		if (shouldPrint)
		{
			PrintMessages(message,stamp);
		}
	}
}



//
// SysExStuff
//
int VisualMidiDevice::SysExDumpToControlChange(int sysExValue)
{
	return -1;
}

void VisualMidiDevice::SendDumpRequest()
{
}

void VisualMidiDevice::ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp)
{
}




void VisualMidiDevice::LoadImage(const std::string& fullPath)
{
	if (!m_Texture.loadFromFile(fullPath))
	{
		throw std::runtime_error("Could not load " + fullPath);
	}
	m_Texture.setSmooth(false);
	setTexture(m_Texture);
	setOrigin(sf::Vector2f(m_Texture.getSize()) / 2.0f);
}


sf::FloatRect VisualMidiDevice::GetDisplayBox() const
{
	return sf::FloatRect(10, 10, 300, 100);
	//Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight);
}

extern sf::Font* g_Font;   // Hack

void VisualMidiDevice::Draw(sf::RenderWindow& window)
{
	sf::FloatRect boundingBox = getGlobalBounds();
	if (s_SelectedInputDevice == this)
	{
		sf::RectangleShape rectangle;
		rectangle.setPosition(boundingBox.getPosition());
		rectangle.setSize(boundingBox.getSize());
		rectangle.setFillColor(sf::Color::Transparent);
		rectangle.setOutlineColor(sf::Color::Blue);
		rectangle.setOutlineThickness(2);
		window.draw(rectangle);
	}
	if (s_SelectedOutputDevice == this)
	{
		sf::RectangleShape rectangle;
		rectangle.setPosition(boundingBox.getPosition());
		rectangle.setSize(boundingBox.getSize());
		rectangle.setFillColor(sf::Color::Transparent);
		rectangle.setOutlineColor(sf::Color(0,128,0));
		rectangle.setOutlineThickness(2);
		window.draw(rectangle);
	}

	window.draw(*this);

	// Add some data
	if (m_MidiOutput)
	{
		sf::FloatRect displayBox = GetDisplayBox();
		if (displayBox.left >= 0)
		{
			sf::String message="Program: "+std::to_string(m_SelectedProgram+1);
			message += "\r\n" + GetProgramName(m_SelectedProgram);

			sf::Text text;
			text.setFont(*g_Font); // font is a sf::Font
			text.setString(message);
			text.setCharacterSize(16); // in pixels, not points!
			text.setFillColor(sf::Color::White);
			//text.setStyle(sf::Text::Bold | sf::Text::Underlined);
			text.setPosition(displayBox.left+1, displayBox.top+1);
			window.draw(text,getTransform());   // inside the main loop, between window.clear() and window.display()
			text.setFillColor(sf::Color::Black);
			text.setPosition(displayBox.left, displayBox.top);
			window.draw(text, getTransform());   // inside the main loop, between window.clear() and window.display()
		}
	}

}


bool VisualMidiDevice::SelectItem(int xPos, int yPos)
{
	for (auto input : s_OutputDevices)
	{
		sf::FloatRect boundingBox = input->getGlobalBounds();
		if (boundingBox.contains((float)xPos, (float)yPos))
		{
			input->SetAsSelectedOutput();
			return true;
		}
	}

	for (auto input : s_InputDevices)
	{
		sf::FloatRect boundingBox = input->getGlobalBounds();
		if (boundingBox.contains((float)xPos, (float)yPos))
		{
			input->SetAsSelectedInput();
			return true;
		}
	}
	return false;
}


//
// Custom drawing routines
//
void VisualMidiDevice::DrawKeyboard(
	sf::RenderWindow& window,
	int firstKey, int lastKey,
	float xOffset, float yOffset,
	float xSpacing,
	float whiteWidth, float whiteHeight,
	float blackWidth, float blackHeight)
{
	sf::Transform transform = getTransform();

	// Draw the keys
	int octave[] = { 0,1,0,1,0,0,1,0,1,0,1,0 }; // 0=white 1=black

	for (int phase = 0; phase < 2; phase++)
	{
		int offset = 0;
		for (int key = 0; key <= (lastKey - firstKey); key++)
		{
			int value = m_Notes[firstKey + key] * 2;  // Convert 0-127 to 0-254
			int color = octave[key % 12];

			if (phase == color)
			{
				// https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php
				sf::Transform keyTransform(transform);
				keyTransform.translate(xOffset + (xSpacing * offset) - (blackWidth / 2) * color, yOffset);

				sf::RectangleShape rectangle;

				if (color == 0)
				{
					// White 51x239
					rectangle.setSize(sf::Vector2f(whiteWidth, whiteHeight));
					rectangle.setFillColor(sf::Color(255, 255 - value, 255 - value));
				}
				else
				{
					// Black 24x150
					rectangle.setSize(sf::Vector2f(blackWidth, blackHeight));
					rectangle.setFillColor(sf::Color(0 + value, 0, 0));
				}

				rectangle.setOutlineColor(sf::Color::Black);
				rectangle.setOutlineThickness(2);
				rectangle.setOrigin(3.f, 5.f);
				rectangle.rotate(0);
				window.draw(rectangle, keyTransform);
			}

			if (color == 0)
			{
				offset += 1;
			}
		}
	}
}



std::string VisualMidiDevice::GetProgramName(int program) const
{
	if ((program < 0) || (program > 127))
	{
		return "";
	}
	const char* programNames[128] =
	{
"Acoustic Grand Piano",
"Bright Acoustic Piano",
"Electric Grand Piano",
"Honky-tonk Piano",
"Electric Piano 1",
"Electric Piano 2",
"Harpsichord",
"Clavi",
"Celesta",
"Glockenspiel",
"Music Box",
"Vibraphone",
"Marimba",
"Xylophone",
"Tubular Bells",
"Dulcimer",
"Drawbar Organ",
"Percussive Organ",
"Rock Organ",
"Church Organ",
"Reed Organ",
"Accordion",
"Harmonica",
"Tango Accordion",
"Acoustic Guitar (nylon)",
"Acoustic Guitar (steel)",
"Electric Guitar (jazz)",
"Electric Guitar (clean)",
"Electric Guitar (muted)",
"Overdriven Guitar",
"Distortion Guitar",
"Guitar Harmonics",
"Acoustic Bass",
"Electric Bass (finger)",
"Electric Bass (pick)",
"Fretless Bass",
"Slap Bass 1",
"Slap Bass 2",
"Synth Bass 1",
"Synth Bass 2",
"Violin",
"Viola",
"Cello",
"Contrabass",
"Tremolo Strings",
"Pizzicato Strings",
"Orchestral Harp",
"Timpani",
"String Ensemble 1",
"String Ensemble 2",
"Synth Strings 1",
"Synth Strings 2",
"Choir Aahs",
"Voice Oohs",
"Synth Voice",
"Orchestra Hit",
"Trumpet",
"Trombone",
"Tuba",
"Muted Trumpet",
"French Horn",
"Brass Section",
"Synth Brass 1",
"Synth Brass 2",
"Soprano Sax",
"Alto Sax",
"Tenor Sax",
"Baritone Sax",
"Oboe",
"English Horn",
"Bassoon",
"Clarinet",
"Piccolo",
"Flute",
"Recorder",
"Pan Flute",
"Blown bottle",
"Shakuhachi",
"Whistle",
"Ocarina",
"Lead 1 (square)",
"Lead 2 (sawtooth)",
"Lead 3 (calliope)",
"Lead 4 (chiff)",
"Lead 5 (charang)",
"Lead 6 (voice)",
"Lead 7 (fifths)",
"Lead 8 (bass + lead)",
"Pad 1 (new age)",
"Pad 2 (warm)",
"Pad 3 (polysynth)",
"Pad 4 (choir)",
"Pad 5 (bowed)",
"Pad 6 (metallic)",
"Pad 7 (halo)",
"Pad 8 (sweep)",
"FX 1 (rain)",
"FX 2 (soundtrack)",
"FX 3 (crystal)",
"FX 4 (atmosphere)",
"FX 5 (brightness)",
"FX 6 (goblins)",
"FX 7 (echoes)",
"FX 8 (sci-fi)",
"Sitar",
"Banjo",
"Shamisen",
"Koto",
"Kalimba",
"Bag pipe",
"Fiddle",
"Shanai",
"Tinkle Bell",
"Agogô",
"Steel Drums",
"Woodblock",
"Taiko Drum",
"Melodic Tom",
"Synth Drum",
"Reverse Cymbal",
"Guitar Fret Noise",
"Breath Noise",
"Seashore",
"Bird Tweet",
"Telephone Ring",
"Helicopter",
"Applause",
"Gunshot"
	};
	return programNames[program];
}


