
#pragma once


#include "VisualMidiDevice.h"



class YamahaTX7MidiDevice : public VisualMidiDevice
{

public:
	YamahaTX7MidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(174, 85, 260, 78); }
	virtual std::string GetProgramName(int program) const override;
	virtual int GetMaxPrograms() const override
	{
		return 32;
	}

};


