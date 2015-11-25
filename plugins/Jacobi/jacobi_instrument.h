/*
 * jacobi_Instrument.h - software-synthesizer
 *
 * Copyright (c) 2015 Simon Jackson <jackokring/at/gmail.com>
 * 
 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#ifndef _JACOBI_H
#define _JACOBI_H

#include <QObject>
#include "Instrument.h"
#include "InstrumentView.h"
#include "Knob.h"


class jacobiInstrumentView;
class NotePlayHandle;
class automatableButtonGroup;
class PixmapButton; //randomize button?

class voiceObject : public Model
{
	Q_OBJECT
	MM_OPERATORS
public:
	voiceObject( Model * _parent, int _idx );
	virtual ~voiceObject();


private:
	FloatModel m_pulseWidthModel;
	FloatModel m_attackModel;
	FloatModel m_decayModel;
	FloatModel m_sustainModel;
	FloatModel m_releaseModel;
	FloatModel m_coarseModel;
	IntModel m_waveFormModel;
	BoolModel m_syncModel;
	BoolModel m_ringModModel;
	BoolModel m_filteredModel;
	BoolModel m_testModel;

	friend class jacobiInstrument;
	friend class jacobiInstrumentView;
} ;

class jacobiInstrument : public Instrument
{
	Q_OBJECT
public:
	jacobiInstrument( InstrumentTrack * _instrument_track );
	virtual ~jacobiInstrument();

	virtual void playNote( NotePlayHandle * _n, sampleFrame * _working_buffer );
	virtual void deleteNotePluginData( NotePlayHandle * _n );

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );

	virtual QString nodeName() const;

	virtual f_cnt_t desiredReleaseFrames() const;

	virtual PluginView * instantiateView( QWidget * _parent );

private:
	// voices
	voiceObject * m_voice[3];

	// filter	
	FloatModel m_filterFCModel;
	FloatModel m_filterResonanceModel;
	IntModel m_filterModeModel;
	
	// misc
	BoolModel m_voice3OffModel;
	FloatModel m_volumeModel;

	IntModel m_chipModel;

	friend class jacobiInstrumentView;

} ;



class jacobiInstrumentView : public InstrumentView
{
	Q_OBJECT
public:
	jacobiInstrumentView( Instrument * _instrument, QWidget * _parent );
	virtual ~jacobiInstrumentView();

private:
	virtual void modelChanged();
	
	automatableButtonGroup * m_passBtnGrp;
	automatableButtonGroup * m_jacobiTypeBtnGrp;

	struct voiceKnobs
	{
		voiceKnobs( Knob * a,
					Knob * d,
					Knob * s,
					Knob * r,
					Knob * pw,
					Knob * crs,
					automatableButtonGroup * wfbg,
					PixmapButton * syncb,
					PixmapButton * ringb,
					PixmapButton * filterb,
					PixmapButton * testb ) :
			m_attKnob( a ),
			m_decKnob( d ),
			m_sustKnob( s ),
			m_relKnob( r ),
			m_pwKnob( pw ),
			m_crsKnob( crs ),
			m_waveFormBtnGrp( wfbg ),
			m_syncButton( syncb ),
			m_ringModButton( ringb ),
			m_filterButton( filterb ),
			m_testButton( testb )
		{
		}
		voiceKnobs()
		{
		}
		Knob * m_attKnob;
		Knob * m_decKnob;
		Knob * m_sustKnob;
		Knob * m_relKnob;
		Knob * m_pwKnob;
		Knob * m_crsKnob;
		automatableButtonGroup * m_waveFormBtnGrp;
		PixmapButton * m_syncButton;
		PixmapButton * m_ringModButton;
		PixmapButton * m_filterButton;
		PixmapButton * m_testButton;
	} ;

	voiceKnobs m_voiceKnobs[3];

	Knob * m_volKnob;
	Knob * m_resKnob;
	Knob * m_cutKnob;
	PixmapButton * m_offButton;

protected slots:
	void updateKnobHint();
	void updateKnobToolTip();
} ;


#endif
