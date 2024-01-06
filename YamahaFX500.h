
#pragma once


#include "VisualMidiDevice.h"



class YamahaFX500MidiDevice : public VisualMidiDevice
{

public:
	YamahaFX500MidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const override { return sf::FloatRect(204, 44, 260, 78); }
	virtual std::string GetProgramName(int program) const override;
	virtual void Draw(sf::RenderWindow& window) override;
	virtual void SetSelectedProgram(int selectedProgram) override;
	virtual int GetMaxPrograms() const override
	{
		return 60;
	}

	//
	// Device specific
	//
	void ToggleFlag(int flag);

	void SendControlChange(unsigned char controlNumber,unsigned char controlValue);
	void SendProgramChange(unsigned char programNumber);
	void SendChannelPressure(unsigned char pressure);

protected:
	int     m_EffectFlags = 0;
};


