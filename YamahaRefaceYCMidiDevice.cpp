
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
#include "YamahaRefaceYCMidiDevice.h"



// http://www.muzines.co.uk/articles/everything-you-ever-wanted-to-know-about-system-exclusive/5722
class YamahaSysExHeader
{
public:
	bool IsValid() const
	{
		if (m_ExclusiveStatus != 0xf0)
		{
			return false;
		}
		if (m_YamahaID != 0x43)
		{
			return false;
		}
		return true;
	}

	int GetByteCount() const
	{
		int byteCount = (((int)m_ByteCountHigh) << 8) + m_ByteCountLow;
		return byteCount;
	}

	int GetAddress() const
	{
		int address = (((int)m_AddressHigh) << 16) + (((int)m_AddressMid) << 8) + m_AddressLow;
		return address;
	}

	// http://www.muzines.co.uk/articles/everything-you-ever-wanted-to-know-about-system-exclusive/5722
	bool CheckIsValidData() const
	{
		// CALCULATING CHECKSUMS
		const unsigned char* ptrData = (const unsigned char*)(&m_ModelID);
		int byteCount = GetByteCount();
		unsigned char calculatedCheckSum = 0;
		while (byteCount--)
		{
			unsigned char data = *ptrData++;
			calculatedCheckSum += data;
		}
		unsigned char checkSum = *ptrData++;
		if (*ptrData++ != 0xf7)
		{
			return false;
		}
		calculatedCheckSum = (~calculatedCheckSum & 127) + 1;
		if (checkSum != calculatedCheckSum)
		{
			return false;
		}
		return true;
	}

	const unsigned char* GetDataStart() const
	{
		return (const unsigned char*)(this + 1);
	}

public:
	unsigned char	m_ExclusiveStatus;  // 0xF0
	unsigned char	m_YamahaID;         // 0x43
	unsigned char	m_DeviceNumber;     // 0x0n
	unsigned char	m_GroupNumberHigh;  // 0x7C
	unsigned char	m_GroupNumberLow;   // 0x1C
	unsigned char	m_ByteCountHigh;    // 0x   - Byte Count shows the size of data in blocks from Model ID onward (up to but not including the checksum).
	unsigned char	m_ByteCountLow;     // 0x
	unsigned char   m_ModelID;          // 0x06
	unsigned char	m_AddressHigh;      // 0x
	unsigned char	m_AddressMid;       // 0x
	unsigned char	m_AddressLow;       // 0x
};




int YamahaRefaceYCMidiDevice::SysExDumpToControlChange(int sysExValue)
{
	ControlChange values[] =
	{
		Volume,
		Reserved,
		//RotarySpeed,
		OrganVoiceType ,
		Footage_1 ,
		Footage_2 ,
		Footage_3 ,
		Footage_4 ,
		Footage_5 ,
		Footage_6 ,
		Footage_7 ,
		Footage_8 ,
		Footage_9 ,
		VibratoChorusSelect ,
		VibratoChorusDepth ,
		PercussionOnOff ,
		PercussionType ,
		DistortionDrive ,
		ReverbDepth
	};
	if (sysExValue < sizeof(values) / sizeof(*values))
	{
		return values[sysExValue];
	}
	return -1;
}


void YamahaRefaceYCMidiDevice::SendDumpRequest()
{
	assert(m_MidiOutput);
	/*
	std::vector<unsigned char> message;
	message.push_back(0xF0); // Exclusive status
	message.push_back(0x43); // YAMAHA ID
	message.push_back(0x20); // Device Number
	message.push_back(0x7F); // Group Number High
	message.push_back(0x1C); // Group Number Low
	message.push_back(0x06); // Model ID
	message.push_back(0x0E); // Address High  -- MIDI PARAMETER CHANGE TABLE (Tone Generator)
	message.push_back(0x0F); // Address Mid
	message.push_back(0x00); // Address Low
	message.push_back(0xF7);
	*/
	// https://yamahasynth.com/learn/2010s/sequencing-reface-cp-cs-dx-yc
	std::vector<unsigned char> message = { 0xF0,0x43,0x20,0x7F,0x1C,0x06,0x0E,0x0F,0x00,0xF7 };
	SendMessage(message);
}





void YamahaRefaceYCMidiDevice::ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp)
{
	/*
	(3 - 4 - 3) BULK DUMP
	11110000 F0H Exclusive status
	01000011 43H YAMAHA ID

	0000nnnn 0nH Device Number
	01111111 7FH Group Number High
	00011100 1CH Group Number Low
	0bbbbbbb bbbbbbb Byte Count
	0bbbbbbb bbbbbbb Byte Count
	00000110 06H Model ID
	0aaaaaaa aaaaaaa Address High
	0aaaaaaa aaaaaaa Address Mid
	0aaaaaaa aaaaaaa Address Low
	0 0 Data
	l l
	0ccccccc ccccccc Check - sum
	11110111 F7H End of Exclusive
	See the following BULK DUMP Table for Addressand Byte Count.
	Byte Count shows the size of data in blocks from Model ID onward(up to but not including the checksum).
	The Check sum is the value that results in a value of 0 for the lower 7 bits when the Model ID, Start Address,
	Dataand Check sum itself are added
	*/
	auto nBytes = message.size();
	if (nBytes >= sizeof(YamahaSysExHeader))
	{
		const YamahaSysExHeader& header(*(YamahaSysExHeader*)message.data());
		if (header.IsValid())
		{
			if (header.CheckIsValidData())
			{
				int byteCount = header.GetByteCount();
				int address = header.GetAddress();
				const unsigned char* ptrData = header.GetDataStart();

				switch (address)
				{
				case 0x000000:  // System - Common
					break;

				case 0x0e0f00:  // TG - BulkHeader
					break;

				case 0x300000:  // TG - Common
				{
					ControlChange values[] =
					{
						Volume,
							Reserved,
						OrganVoiceType ,
						Footage_1 ,
						Footage_2 ,
						Footage_3 ,
						Footage_4 ,
						Footage_5 ,
						Footage_6 ,
						Footage_7 ,
						Footage_8 ,
						Footage_9 ,
						VibratoChorusSelect ,
						VibratoChorusDepth ,
						PercussionOnOff ,
						PercussionType ,
						PercussionLength, 
						RotarySpeed,
						DistortionDrive ,
						ReverbDepth,
							Reserved,
							Reserved
							/*,
							Reserved,
							Reserved,
							Reserved,
							Reserved,
							*/
					};
					ControlChange* ptrControls = values;

					byteCount -= 4;
					while (byteCount--)
					{
						ControlChange controller = *ptrControls++;
						unsigned char value = *ptrData++;

						switch (controller)
						{
						case OrganVoiceType:
							{
								char array[] = { 0, 32,64,95,127 };
								value = array[value];
							}
							//value = ((char*){ 0, 32,64,95,127 })[value];
							break;

						case Footage_1:
						case Footage_2:
						case Footage_3:
						case Footage_4:
						case Footage_5:
						case Footage_6:
						case Footage_7:
						case Footage_8:
						case Footage_9:
							value = (value * 127) / 6;
							break;
						}

						m_ControlChange[controller] = value;

						//ControlChange control = SysExDumpToControlChange(sysExValue);
					}
				}
				break;

				case 0x0f0f00:  // TB - Bulk
					break;
				}

			}
		}
		/*
		auto it = message.begin();
		if ((it[0] == 0xf0) && (it[1] == 0x43))
		{
			it += 2; // SysEx/Yamaha
			it += 5; // Misc
			if (it[0] == 0x06)
			{
				it += 4; // model id + address
				while (it != message.end() && it[0] != 0xf7)
				{
					int sysExValue = *it++;
					ControlChange control = SysExDumpToControlChange(sysExValue);


				}

			}

		}
		*/
	}

}






void YamahaRefaceYCMidiDevice::DrawFootage(sf::RenderWindow& window)
{
	sf::Transform transform = getTransform();

	// Draw the knobs
	for (int slider = 0; slider < 9; slider++)
	{
		int value = GetControlChange(102 + slider);

		sf::Transform knobTransform(transform);
		knobTransform.translate(486.f + slider * 53.f, 65.f + value * 54 / 127);  // 72 tall

		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(46, 16));
		rectangle.setFillColor(sf::Color::Black);
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setOutlineThickness(2);
		rectangle.setOrigin(23.f, 8.f);
		//rectangle.rotate(127 + GetControlChange(1 + knob) * 2.3f);
		window.draw(rectangle, knobTransform);
	}
}


void YamahaRefaceYCMidiDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	DrawFootage(window);

	DrawKeyboard(window,
		48, 84,
		146.f, 219.f,
		52.f,
		51.f, 239.f,
		24.f, 150.f
	);
}




//std::cout << "bulk!" << std::endl;
//deviceMPK.SendDumpRequest();

// JV10 - F0 41 10 6A 11 01 00 00 00 01 0F 17 01 57 F7

//std::vector<unsigned char> message;
// https://www.yamahasynth.com/ask-a-question/reface-cs-button-to-dump-sysex-out
// http://www.muzines.co.uk/articles/everything-you-ever-wanted-to-know-about-system-exclusive/5722
// Send it a Bulk Request:
// F0 43 20 7F 1C 03 0E 0F 00 F7

// Dump Request:
// See the following DUMP REQUEST Table for Address and Byte Count.
/*
message.push_back(0xF0); // Exclusive status
message.push_back(0x43); // YAMAHA ID
message.push_back(0x20); // Device Number
message.push_back(0x7F); // Group Number High
message.push_back(0x1C); // Group Number Low
message.push_back(0x06); // Model ID
message.push_back(0x00); // Address High  -- MIDI PARAMETER CHANGE TABLE (SYSTEM)
message.push_back(0x00); // Address Mid
message.push_back(0x00); // Address Low
message.push_back(0xF7);
*/
/*
message.push_back(0xF0); // Exclusive status
message.push_back(0x43); // YAMAHA ID
message.push_back(0x20); // Device Number
message.push_back(0x7F); // Group Number High
message.push_back(0x1C); // Group Number Low
message.push_back(0x06); // Model ID
message.push_back(0x0E); // Address High  -- MIDI PARAMETER CHANGE TABLE (Tone Generator)
message.push_back(0x0F); // Address Mid
message.push_back(0x00); // Address Low
message.push_back(0xF7);
*/

/*
// --- Identity Request ---
message.push_back(0xF0);
message.push_back(0x7E);
message.push_back(0x00);
message.push_back(0x06);
message.push_back(0x01);
message.push_back(0xF7); // Answers: Answer: f0 7e 7f 06 02 43 00 41 54 06 00 00 00 7f f7
*/

// Controls:
// Rotary=19
// Wave=80
// Effect Reverb=91
/*
// --- Parameter Request ---
message.push_back(0xF0); // Exclusive status
message.push_back(0x43); // YAMAHA ID
message.push_back(0x30); // Device Number
message.push_back(0x7F); // Group Number High
message.push_back(0x1C); // Group Number Low
message.push_back(0x06); // Model ID
message.push_back(0x30); // Address High
message.push_back(0x00); // Address Mid
message.push_back(0x13); // Address Low
message.push_back(0xF7); // Answers: Answer:
Byte 0 = 240 F0
Byte 1 = 67  43
Byte 2 = 16  10
Byte 3 = 127 7F
Byte 4 = 28  1C
Byte 5 = 6   06

Byte 6 = 48  30 // Address High
Byte 7 = 0   00 // Address Mid
Byte 8 = 19  13 // Address Low

Byte 9 = 0   00     <- Value from 0 to 127
Byte 10 = 247 F7
stamp = 0.049 device = Yamaha Reface YC
*/
//deviceReface.Out()->sendMessage