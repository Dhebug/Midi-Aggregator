
#define SFML_STATIC 


#include <iostream>
#include <cstdlib>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#pragma warning (disable:4996)

#include "VisualMidiDevice.h"
#include "YamahaTX7MidiDevice.h"


namespace tx7
{

	struct FmSound
	{
		const char* effectName;
	};

	const FmSound g_EffectData[32] =
	{
		{"ACC. PIANO"},
		{"HI STRINGS"},
		{"TRUMPET A"},
		{"MALE CHOIR"},
		{"ELEC.PNO A"},
		{"E.ORGAN A"},
		{"POWERSYN A"},
		{"FATSYNTH A"},

		{"JAZZ GUITR"},
		{"CELLOS A"},
		{"A.MALLER A"},
		{"E.P & BR A"},
		{"PIPES A"},
		{"SYN-RISE A"},
		{"CLAV. A"},
		{"TINE E.PNO"},

		{"BC FLUTE"},
		{"HORN SEC.A"},
		{"DBL.HARP A"},
		{"E.GUITAR A"},
		{"E.BASS A"},
		{"HARPSI. A"},
		{"VIBES A"},
		{"SAX BC"},

		{"FM PIANO A"},
		{"TIMPANI NW"},
		{"TIMEWARP"},
		{"TUBERISE A"},
		{"VIOLINS A"},
		{"KARIMBA A"},
		{"HARMOSYNTH"},
		{"ORCHESTRAL"}
	};
}




std::string YamahaTX7MidiDevice::GetProgramName(int program) const
{
	if ((program < 0) || (program > 31))
	{
		return "";
	}
	return tx7::g_EffectData[program].effectName;
}


