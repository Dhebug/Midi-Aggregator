
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
#include "AkaiMPKMidiDevice.h"


class AkaiSysExHeader
{
public:
	bool IsValid() const
	{
		if (m_ExclusiveStatus != 0xf0)
		{
			return false;
		}
		if (m_AkaiID != 0x47)
		{
			return false;
		}
		if (m_ZeroByte != 0)
		{
			return false;
		}
		if (m_Device != 0x26)
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

	/* Sent: { 0xF0, 0x47, 0x00, 0x26, 0x66, 0x00, 0x01, 0x01, 0xF7 };
	f0 47 00 26 67 00 6d 01 09 00 04 00 00 00 00 00 00 03 01 00 00 00 00 00 01 01 01 2c 00 01 01 2d 01 02 01 2e 02 03
	01 2f 03 04 01 30 04 05 01 31 05 06 01 32 06 07 01 33 07 08 01 20 08 09 01 21 09 0a 01 22 0a 0b 01 23 0b 0c 01 24
	0c 0d 01 25 0d 0e 01 26 0e 0f 01 27 0f 10 01 01 00 7f 02 00 7f
	*/

public:
	unsigned char	m_ExclusiveStatus;  // 0xF0
	unsigned char	m_AkaiID;           // 0x47
	unsigned char	m_ZeroByte;         // 0x00
	unsigned char	m_Device;           // 0x26
	unsigned char	m_GroupNumberHigh;  // 0x7C
	unsigned char	m_GroupNumberLow;   // 0x1C
	unsigned char	m_ByteCountHigh;    // 0x   - Byte Count shows the size of data in blocks from Model ID onward (up to but not including the checksum).
	unsigned char	m_ByteCountLow;     // 0x
	unsigned char   m_ModelID;          // 0x06
	unsigned char	m_AddressHigh;      // 0x
	unsigned char	m_AddressMid;       // 0x
	unsigned char	m_AddressLow;       // 0x
};




void AkaikMpkMidiDevice::SendDumpRequest()
{
	assert(m_MidiOutput);
	// http://practicalusage.com/akai-mpk261-mpk2-series-controlling-the-controller-with-sysex/
	// https://github.com/gljubojevic/akai-mpk-mini-editor/blob/master/README.md
	// MPK261:     F0   47   00   25   30   00   04   01   00   01   xx   F7
	// MPK Mini: 0xF0,0x47,0x7F,0x7C,0x63,0x00,0x01,0x01,0xF7   Get Preset 1
	// MPK Mini: 0xF0,0x47,0x7F,0x7C,0x63,0x00,0x01,0x02,0xF7   Get Preset 2
	// MPK Mini: 0xF0,0x47,0x7F,0x7C,0x63,0x00,0x01,0x03,0xF7   Get Preset 3
	// MPK Mini: 0xF0,0x47,0x7F,0x7C,0x63,0x00,0x01,0x04,0xF7   Get Preset 4

	// "F0 7E 7F 06 01 F7"
	// "F0 47 7F 28 60 00 04 41 09 01 06 F7"
	// This is all on an Akai (0x47|71) MIDI device type 0x26|38.

	// https://cycling74.com/forums/akai-mpk-mini-send-get-signals-to-light-buttons
	//std::vector<unsigned char> message = { 0xF0,0x47,0x7F,0x7C,0x63,0x00,0x01,0x01,0xF7 };
	//std::vector<unsigned char> message = { 0xF0,0x47,0x71,0x7C,0x63,0x00,0x01,0x01,0xF7 };

	// Byte 0 = 144, Byte 1 = 59, Byte 2 = 127, stamp = 1.822 device = Akai MPKmini mkII
	//std::vector<unsigned char> message = { 144,59,127 };  // Prog Change/On/OFF

	//std::vector<unsigned char> message = { 144,28,127 };  // Prog Change/On/OFF
	//std::vector<unsigned char> message = { 144,45,127 };  // Prog Change/On/OFF

	// Using PocketMidi, MPK Tool "Get":
	// F0 47 00 26 67 00 6D 01 09 00 04 00 00 00 00 00 00 03 01 00 00 00 
	// 00 00 01 01 01 2C 00 01 01 2D 01 02 01 2E 02 03 01 2F 03 04 01 30 
	// 04 05 01 31 05 06 01 32 06 07 01 33 07 08 01 20 08 09 01 21 09 0A 
	// 01 22 0A 0B 01 23 0B 0C 01 24 0C 0D 01 25 0D 0E 01 26 0E 0F 01 27 
	// 0F 10 01 01 00 7F 02 00 7F 03 00 7F 04 00 7F 05 00 7F 06 00 7F 07 
	// 00 7F 08 00 7F 0C F7

	// Using DebugView++ with loopMidi:
	/*
29	11.932685	22888	loopMIDI.exe	02b3b65e [loopMIDI Port]: from device
30	11.932685	22888	loopMIDI.exe	: F0 47 00 26 66 00 01 01 F7
32	11.932715	22888	loopMIDI.exe	: F0 47 00 26 66 00 01 01 F7
34	11.932744	22888	loopMIDI.exe	: F0 47 00 26 66 00 01 01 F7
36	11.932771	22888	loopMIDI.exe	: F0 47 00 26 66 00 01 01 F7
37	17.944765	10856	<system>	COsInterface::SetApp: set Instance = 0x6dc4 (28100), AppID = 0, Name = DebugView++()

		*/
		//std::vector<unsigned char> message = { 0xF0, 0x47, 0x00, 0x26, 0x66, 0x00, 0x01, 0x01, 0xF7 };
		//                                                                              +----+          0=RAM 1/2/3/4=Programs
	std::vector<unsigned char> message = { 0xF0, 0x47, 0x00, 0x26, 0x66, 0x00, 0x01, 0x00, 0xF7 };
	SendMessage(message);
}




void AkaikMpkMidiDevice::ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp)
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
	if (nBytes >= sizeof(AkaiSysExHeader))
	{
		/* Sent: { 0xF0, 0x47, 0x00, 0x26, 0x66, 0x00, 0x01, 0x01, 0xF7 };
		f0 47 00 26 67 00 6d 01 09 00 04 00 00 00 00 00 00 03 01 00 00 00 00 00 01 01 01 2c 00 01 01 2d 01 02 01 2e 02 03
		01 2f 03 04 01 30 04 05 01 31 05 06 01 32 06 07 01 33 07 08 01 20 08 09 01 21 09 0a 01 22 0a 0b 01 23 0b 0c 01 24
		0c 0d 01 25 0d 0e 01 26 0e 0f 01 27 0f 10 01 01 00 7f 02 00 7f
		*/
		const unsigned char* ptrStart = message.data();
		const AkaiSysExHeader& header(*(AkaiSysExHeader*)message.data());
		if (header.IsValid())
		{
			if (header.CheckIsValidData())
			{
				int byteCount = header.GetByteCount();
				int address = header.GetAddress();
				const unsigned char* ptrData = header.GetDataStart();
			}
		}
	}
}


void AkaikMpkMidiDevice::Draw(sf::RenderWindow& window)
{
	// Draw the main device
	VisualMidiDevice::Draw(window);

	sf::Transform transform = getTransform();

	// Draw the knobs
	for (int knob = 0; knob < 8; knob++)
	{
		int xOffset = knob & 3;
		int yOffset = knob >> 2;

		// https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php
		sf::Transform knobTransform(transform);
		knobTransform.translate(566.f + 84 * xOffset, 41.f + 79 * yOffset);

		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(26, 8));
		rectangle.setFillColor(sf::Color::White);
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(2);
		rectangle.setOrigin(3.f, 5.f);
		rectangle.rotate(127 + GetControlChange(1 + knob) * 2.3f);
		window.draw(rectangle, knobTransform);
	}

	DrawKeyboard(window,
		48, 72,
		31.f, 243.f,
		54.f,
		50.f, 213.f,
		25.f, 129.f
	);
}
