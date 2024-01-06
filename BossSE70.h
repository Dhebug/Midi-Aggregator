
#pragma once


#include "VisualMidiDevice.h"



class BossSE70MidiDevice : public VisualMidiDevice
{

public:
	BossSE70MidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual sf::FloatRect GetDisplayBox() const override { return sf::FloatRect(152, 45, 177, 39); }
	virtual void Draw(sf::RenderWindow& window) override;

	virtual std::string GetProgramName(int program) const override;

	/*
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
	*/
protected:
	int     m_EffectFlags = 0;
};


