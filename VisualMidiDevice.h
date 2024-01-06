
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "RtMidi.h"
#include <assert.h>
#include <set>

class VisualMidiDevice : public sf::Sprite
{
public:
	VisualMidiDevice(const std::string& deviceName);
	virtual ~VisualMidiDevice();

	//
	// Midi stuff
	//
	RtMidiIn* SetMidiInput(const std::string& deviceName);
	RtMidiOut* SetMidiOutput(const std::string& deviceName);

	void SetAsSelectedInput();
	void SetAsSelectedOutput();
	static VisualMidiDevice* GetSelectedInput()  { return s_SelectedInputDevice; }
	static VisualMidiDevice* GetSelectedOutput() { return s_SelectedOutputDevice; }


	void SetSubDevice(VisualMidiDevice* subDevice)  { m_SubDevice= subDevice; }   // Should probably use smart pointers there
	VisualMidiDevice* GetSubDevice() const          { return m_SubDevice; }


	RtMidiIn* In()		{ return m_MidiInput; }
	RtMidiOut* Out()	{ return m_MidiOutput; }


	void SendMessage(const std::vector<unsigned char>& message)
	{
		assert(m_MidiOutput);
		m_MidiOutput->sendMessage(&message);
		m_MidiOutSignal = 63;
	}

	void ProcessInput(VisualMidiDevice* output);
	void PrintMessages(const std::vector<unsigned char>& message, double stamp);

	int GetControlChange(int index)
	{
		if ((index >= 0) && (index < 128))
		{
			return m_ControlChange[index];
		}
		else
		{
			return 0;
		}
	}

	//
	// SysEx stuff
	//
	virtual int SysExDumpToControlChange(int sysExValue);
	virtual void SendDumpRequest();
	virtual void ProcessSysExMessage(const std::vector<unsigned char>& message, double stamp);


	//
	// Visuals
	//
	void LoadImage(const std::string& fullPath);

	virtual void Draw(sf::RenderWindow& window);

	virtual sf::FloatRect GetDisplayBox() const;

	//
	// Custom drawing routines
	//
	void DrawKeyboard(
		sf::RenderWindow& window,
		int firstKey, int lastKey,
		float xOffset, float yOffset,
		float xSpacing,
		float whiteWidth, float whiteHeight,
		float blackWidth, float blackHeight);

	//
	// Mouse interactions
	//
	static bool SelectItem(int xPos, int yPos);


	virtual int GetMaxPrograms() const
	{
		return 128;
	}

	int GetSelectedProgram() const
	{
		return m_SelectedProgram;
	}

	virtual void SetSelectedProgram(int selectedProgram) 
	{
		while (selectedProgram < 0)
		{
			selectedProgram += 128;
		}
		selectedProgram %= GetMaxPrograms();

		m_SelectedProgram = selectedProgram;
		std::vector<unsigned char> message;
		message.push_back(0xc0);
		message.push_back(m_SelectedProgram);
		SendMessage(message);
	}

	virtual std::string GetProgramName(int program) const;



protected:


private:
	VisualMidiDevice();   // N/A

public:
	sf::Texture		  m_Texture;
	std::string		  m_DeviceName;
	RtMidiIn*		  m_MidiInput = nullptr;
	RtMidiOut*		  m_MidiOutput = nullptr;
	VisualMidiDevice* m_SubDevice = nullptr;

	int             m_ControlChange[128] = {};
	std::map<int, int> m_Notes;

	int             m_SelectedProgram = 0;
	int             m_MidiOutSignal = 0;         ///< Used to make things like "activity leds" blinking


	static VisualMidiDevice*            s_SelectedInputDevice;
	static std::set<VisualMidiDevice*>	s_InputDevices;

	static VisualMidiDevice*            s_SelectedOutputDevice;
	static std::set<VisualMidiDevice*>	s_OutputDevices;
};


