
#pragma once


#include "VisualMidiDevice.h"



class YamahaMU100MidiDevice : public VisualMidiDevice
{

public:
	YamahaMU100MidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(160, 24, 260, 78); }

};


