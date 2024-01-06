
#pragma once


#include "VisualMidiDevice.h"



class RolandSC88vlMidiDevice : public VisualMidiDevice
{

public:
	RolandSC88vlMidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(165, 25, 226, 87); }

};


