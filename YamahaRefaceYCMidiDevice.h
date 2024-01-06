
#pragma once


#include "VisualMidiDevice.h"



class YamahaRefaceYCMidiDevice : public VisualMidiDevice
{
public:
	enum ControlChange
	{
		Volume=0,             // Not usable
		Reserved=1,           // Not usable

		RotarySpeed = 19,
		OrganVoiceType = 80,
		Footage_1 = 102,
		Footage_2 = 103,
		Footage_3 = 104,
		Footage_4 = 105,
		Footage_5 = 106,
		Footage_6 = 107,
		Footage_7 = 108,
		Footage_8 = 109,
		Footage_9 = 110,
		VibratoChorusSelect = 79,
		VibratoChorusDepth = 77,
		PercussionOnOff = 111,
		PercussionType = 112,
		PercussionLength = 113,
		DistortionDrive = 18,
		ReverbDepth = 91
	};

public:
	YamahaRefaceYCMidiDevice(const std::string& deviceName) :
		VisualMidiDevice(deviceName)
	{}

	virtual int SysExDumpToControlChange(int sysExValue) override;
	virtual void SendDumpRequest() override;
	virtual void ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp) override;
	virtual void Draw(sf::RenderWindow& window) override;

	virtual sf::FloatRect GetDisplayBox() const { return sf::FloatRect(-1, -1, -1, -1); }

private:
	void DrawFootage(sf::RenderWindow& window);
};


