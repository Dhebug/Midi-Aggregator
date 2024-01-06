
#define SFML_STATIC 

#if _DEBUG
#pragma comment(lib, "sfml-system-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#pragma comment(lib, "sfml-graphics-s-d.lib")
#pragma comment(lib, "sfml-audio-s-d.lib")
#else
#pragma comment(lib, "sfml-system-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "sfml-audio-s.lib")
#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "flac.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "vorbisfile.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "freetype.lib")


#include <iostream>
#include <cstdlib>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#pragma warning (disable:4996)



#include "MidiAggregator.h"

#include "VisualMidiDevice.h"

#include "AkaiMPKMidiDevice.h"
#include "YamahaRefaceYCMidiDevice.h"
#include "YamahaMU100MidiDevice.h"
#include "KorgNX5RMidiDevice.h"
#include "RolandSC88vlMidiDevice.h"
#include "YamahaTX7MidiDevice.h"
#include "YamahaFX500.h"
#include "BossSE70.h"
#include "Metronome.h"
#include "Recorder.h"


bool LoadFile(const std::string& fileName, std::vector<unsigned char>& buffer)
{
	// get the size of the file
	struct stat file_info;

	if (stat(fileName.c_str(), &file_info) == -1)
	{
		return false;
	}

	// open the file
	int cBufferSize = file_info.st_size;
	int nHandle = open(fileName.c_str(), O_BINARY | O_RDONLY, 0);
	if (nHandle == -1)
	{
		return false;
	}

	// allocate some memory
	buffer.resize(cBufferSize);

	// read file content
	if (read(nHandle, buffer.data(), cBufferSize) != (int)cBufferSize)
	{
		close(nHandle);
		return false;
	}
	close(nHandle);
	return true;
}



sf::Font* g_Font = nullptr;

int main()
{
	try
	{
		sf::RenderWindow window(sf::VideoMode(1600, 1024), "MIDI Aggregator");


		sf::Font font;
		//if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
		if (!font.loadFromFile("C:\\Windows\\Fonts\\pixelmix.ttf"))
		{
			// error...                                                             
		}
		g_Font = &font;

		std::string dataFolder = "D:\\_music_\\Midi\\Sources\\MidiAggregator\\Data\\";

		// Input devices
		YamahaRefaceYCMidiDevice deviceReface("Yamaha Reface YC");
		deviceReface.LoadImage(dataFolder + "Yamaha-RefaceYC.png");
		deviceReface.SetMidiInput("reface YC");
		deviceReface.SetMidiOutput("reface YC");

		AkaikMpkMidiDevice deviceMPK("Akai MPKmini mkII");
		deviceMPK.LoadImage(dataFolder + "Akai-MPKmini.png");
		deviceMPK.SetMidiInput("MPKmini2");
		deviceMPK.SetMidiOutput("MPKmini2");

		// Synths
		YamahaTX7MidiDevice deviceTX7("Yamaha TX7");
		deviceTX7.LoadImage(dataFolder + "Yamaha-TX7.png");
		deviceTX7.SetMidiOutput("MIDIOUT7 (ESI M4U eX)");

		YamahaMU100MidiDevice deviceMU100("Yamaha MU100");
		deviceMU100.LoadImage(dataFolder + "Yamaha-MU100.png");
		deviceMU100.SetMidiOutput("MIDIOUT6 (ESI M4U eX)");

		KorgNX5RMidiDevice deviceKorgNX5R("Korg NX5R");
		deviceKorgNX5R.LoadImage(dataFolder + "Korg-NX5R.png");
		deviceKorgNX5R.SetMidiOutput("MIDIOUT5 (ESI M4U eX)");

		RolandSC88vlMidiDevice deviceRolandSC88vl("Roland SC88vl");
		deviceRolandSC88vl.LoadImage(dataFolder + "Roland-SC88vl.png");
		deviceRolandSC88vl.SetMidiOutput("MIDIOUT8 (ESI M4U eX)");
		//deviceRolandSC88vl.SetMidiOutput("ESI M4U eX");

		// Effect processors
		YamahaFX500MidiDevice deviceYamahaFX500("Yamaha FX500");
		deviceYamahaFX500.LoadImage(dataFolder + "Yamaha-FX500.png");
		deviceYamahaFX500.SetMidiOutput("MIDIOUT4 (ESI M4U eX)");

		BossSE70MidiDevice deviceBossSE70("Boss SE-70");
		deviceBossSE70.LoadImage(dataFolder + "Boss-SE-70.png");
		deviceBossSE70.SetMidiOutput("MIDIOUT3 (ESI M4U eX)");

		// Misc devices
		MetronomeDevice metronome("Metronome");
		metronome.LoadImage(dataFolder + "Metronome.png");
		metronome.LoadSound(dataFolder + "\\Sounds\\Metronome.wav");

		RecorderDevice recorder("Recorder");
		recorder.LoadImage(dataFolder + "Recorder.png");
		recorder.SetSoundRecorder("Line (Xonar U7)");
		//recorder.SetSoundRecorder("Microphone (AT2020USB+)");

		//VisualMidiDevice deviceDump("MIDIIN2 (ESI M4U eX)");
		//deviceDump.SetMidiInput("MIDIIN2 (ESI M4U eX)");




	
		deviceYamahaFX500.setPosition(sf::Vector2f(400, 100));
		deviceTX7.setPosition(sf::Vector2f(400, 100+140));
		deviceTX7.SetSubDevice(&deviceYamahaFX500);

		deviceBossSE70.setPosition(sf::Vector2f(1200,100+1));
		deviceRolandSC88vl.setPosition(sf::Vector2f(1200, 100+140));
		deviceRolandSC88vl.SetSubDevice(&deviceBossSE70);

		metronome.setPosition(sf::Vector2f(800, 250));


		deviceMU100.setPosition(sf::Vector2f(400, 450));
		deviceKorgNX5R.setPosition(sf::Vector2f(1200, 450));


		deviceMPK.setPosition(sf::Vector2f(300,800));
		deviceMPK.setScale(0.6f,0.6f);
		//deviceMPK.SendDumpRequest();  // WIP

		deviceReface.setPosition(sf::Vector2f(1100,800));
		deviceReface.setScale(0.6f,0.6f);
		deviceReface.SendDumpRequest();

		recorder.setPosition(sf::Vector2f(613, 800));

		// Select the TX7 as the default device
		deviceTX7.SetAsSelectedOutput();

		sf::Clock clock; // starts the clock
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::MouseButtonPressed:
					{
						if (event.mouseButton.button == sf::Mouse::Left)
						{
							VisualMidiDevice::SelectItem(event.mouseButton.x, event.mouseButton.y);
							/*
							std::cout << "the right button was pressed" << std::endl;
							std::cout << "mouse x: " << event.mouseButton.x << std::endl;
							std::cout << "mouse y: " << event.mouseButton.y << std::endl;
							*/
						}
					}
					break;

				case sf::Event::MouseWheelScrolled:
					{
						if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
						{
							std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;

							VisualMidiDevice* selectedOutputDevice = VisualMidiDevice::GetSelectedOutput();
							assert(selectedOutputDevice);
							if (selectedOutputDevice)
							{
								if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
									sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) )
								{
									// We are interested by the effect processor instead of the instrument
									VisualMidiDevice* subDevice = selectedOutputDevice->GetSubDevice();
									if (subDevice)
									{
										selectedOutputDevice = subDevice;
									}
								}


								int program = selectedOutputDevice->GetSelectedProgram();
								program += (int)event.mouseWheelScroll.delta;
								selectedOutputDevice->SetSelectedProgram(program);
							}
						}
					}
					break;

				case sf::Event::KeyPressed:
					{					
						switch (event.key.code)
						{
						case sf::Keyboard::Num1:
							std::cout << "1" << std::endl;
							deviceYamahaFX500.ToggleFlag(0);
							break;
						case sf::Keyboard::Num2:
							std::cout << "2" << std::endl;
							deviceYamahaFX500.ToggleFlag(1);
							break;
						case sf::Keyboard::Num3:
							std::cout << "3" << std::endl;
							deviceYamahaFX500.ToggleFlag(2);
							break;
						case sf::Keyboard::Num4:
							std::cout << "4" << std::endl;
							deviceYamahaFX500.ToggleFlag(3);
							break;
						case sf::Keyboard::Num5:
							std::cout << "5" << std::endl;
							deviceYamahaFX500.ToggleFlag(4);
							break;
						case sf::Keyboard::Num6:
							std::cout << "6" << std::endl;
							deviceYamahaFX500.SendControlChange(115,0);
							break;

						case sf::Keyboard::M:
							std::cout << "M" << std::endl;
							metronome.StartStop();
							break;

						case sf::Keyboard::Numpad1:
						case sf::Keyboard::Numpad2:
						case sf::Keyboard::Numpad3:
						case sf::Keyboard::Numpad4:
						case sf::Keyboard::Numpad5:
						case sf::Keyboard::Numpad6:
						case sf::Keyboard::Numpad7:
						case sf::Keyboard::Numpad8:
						case sf::Keyboard::Numpad9:
							{
								std::cout << "Num Pad" << std::endl;
								metronome.SetBpm(  200/(event.key.code-sf::Keyboard::Numpad1+1));
							}
							break;

						case sf::Keyboard::P:
							std::cout << "P" << std::endl;
							recorder.Playback();
							break;

						case sf::Keyboard::Space:
							std::cout << "Space" << std::endl;
							recorder.Pause();
							break;

						case sf::Keyboard::R:
							std::cout << "R" << std::endl;
							recorder.Record();
							break;

						case sf::Keyboard::S:
							std::cout << "S" << std::endl;
							recorder.Stop();
							break;

						case sf::Keyboard::L:
							std::cout << "L" << std::endl;
							recorder.ToggleLooping();
							break;
						}

						/*
						if (event.key.code == sf::Keyboard::Escape)
						{
							std::cout << "ESCAPE" << std::endl;
							std::vector<unsigned char> buffer;
							if (LoadFile("D:\\_music_\\Midi\\Dexed_cart_1.0\\Dexed_cart_1.0\\Jarrelook\\MJFDX7A.SYX", buffer))
							{
								deviceTX7.SendMessage(buffer);
							}
						}
						*/

#if 0
						if (event.key.code == sf::Keyboard::Escape)
						{
							std::cout << "ESCAPE" << std::endl;
							std::vector<unsigned char> buffer;
							if (LoadFile("D:\\_music_\\Midi\\Dexed_cart_1.0\\Dexed_cart_1.0\\Jarrelook\\MJFDX7A.SYX", buffer))
							{
								deviceTX7.SendMessage(buffer);
							}
						}
						else
							if (event.key.code == sf::Keyboard::Left)
						{
							std::cout << "LEFT" << std::endl;
							std::vector<unsigned char> buffer;
							if (LoadFile("D:\\_music_\\Midi\\Dexed_cart_1.0\\Dexed_cart_1.0\\Aminet\\001.syx", buffer))
							{
								deviceTX7.SendMessage(buffer);
							}
						}
#endif



					}
					break;
				}
			}

			window.clear();

			//
			// Process the MIDI signals
			//
			VisualMidiDevice* selectedOutputDevice = VisualMidiDevice::GetSelectedOutput();
			assert(selectedOutputDevice);

			deviceMPK.ProcessInput(selectedOutputDevice);
			deviceReface.ProcessInput(selectedOutputDevice);
			//deviceDump.ProcessInput(nullptr);

			//
			// Do some logic
			//
			{
				// Distortion/Reverb
				int distortionDriveSlider = deviceReface.GetControlChange(YamahaRefaceYCMidiDevice::DistortionDrive) * 5 / 127;
				int reverbSlider          = deviceReface.GetControlChange(YamahaRefaceYCMidiDevice::ReverbDepth) * 9 /127;
				int slider = distortionDriveSlider * 10 + reverbSlider;
				
				int currentEffect = deviceYamahaFX500.GetSelectedProgram();
				if ((currentEffect != slider) && (slider < 60))
				{
					deviceYamahaFX500.SetSelectedProgram(slider);
				}
			}

			//
			// Metronom stuff
			//
			{
				int knob = deviceMPK.GetControlChange(8); // 0 to 127
				int bpm = 60 + knob * 2;                  // 60 to 314
				metronome.SetBpm(bpm);

				int footage = deviceReface.GetControlChange(YamahaRefaceYCMidiDevice::Footage_9);
				float volume = 100.0f * (127 - footage) / 127.0f;
				metronome.SetVolume(volume);
			}

			//
			// Current device
			//
			{
				int organVoiceType = deviceReface.GetControlChange(YamahaRefaceYCMidiDevice::OrganVoiceType);
				switch (organVoiceType)
				{
				case 0:
					deviceTX7.SetAsSelectedOutput();
					break;
				case 32:
					deviceRolandSC88vl.SetAsSelectedOutput();
					break;
				case 64:
					deviceMU100.SetAsSelectedOutput();
					break;
				case 95:
					deviceKorgNX5R.SetAsSelectedOutput();
					break;
				case 127:
					// Nothing planned for these two
					break;
				}

			}


			//
			// Draw all the devices
			//
			deviceReface.Draw(window);
			deviceMPK.Draw(window);
			deviceTX7.Draw(window);
			deviceMU100.Draw(window);
			deviceKorgNX5R.Draw(window);
			deviceRolandSC88vl.Draw(window);
			deviceYamahaFX500.Draw(window);
			deviceBossSE70.Draw(window);
			metronome.Draw(window);
			recorder.Draw(window);

			sf::Time elapsed = clock.restart();
			window.display();
		}
	}

	catch (const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "oops";
	}


	return 0;
}



