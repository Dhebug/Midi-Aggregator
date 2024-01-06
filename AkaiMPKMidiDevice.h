
#pragma once

#include <SFML/Graphics.hpp>
#include "RtMidi.h"
#include <assert.h>

#include "VisualMidiDevice.h"



class AkaikMpkMidiDevice : public VisualMidiDevice
{
public:
	AkaikMpkMidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual void SendDumpRequest() override;
	virtual void ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp) override;
	virtual void Draw(sf::RenderWindow& window) override;

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(-1,-1,-1,-1); }


private:
};

