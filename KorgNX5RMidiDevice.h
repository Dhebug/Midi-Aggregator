
#pragma once


#include "VisualMidiDevice.h"



class KorgNX5RMidiDevice : public VisualMidiDevice
{

public:
	KorgNX5RMidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(103, 32, 247, 75); }
	virtual void Draw(sf::RenderWindow& window) override;

};


