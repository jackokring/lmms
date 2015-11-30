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

#define BNK 16

#include <QObject>
#include "Instrument.h"
#include "InstrumentView.h"
#include "Knob.h"


class jacobiInstrumentView;
class NotePlayHandle;

class voiceObject : public Model
{
	Q_OBJECT
	MM_OPERATORS
public:
	voiceObject( Model * _parent, int _idx );
	virtual ~voiceObject();

private:
	IntModel m_bnk;
	FloatModel m_sld[BNK];
	FloatModel m_rto_drv[BNK];
	FloatModel m_mod_gen[BNK];
	FloatModel m_fhc_wso_trt[BNK];
	FloatModel m_gnm_rdf_sso_pq[BNK];
	FloatModel m_cut[BNK];
	FloatModel m_rez[BNK];

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

	virtual PluginView * instantiateView( QWidget * _parent );

private:
	// voices (well oscillators and shaper)
	voiceObject * m_voice[4];

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

	struct voiceKnobs
	{
		voiceKnobs( Knob * a,
				Knob * b,
				Knob * c,
				Knob * d,
				Knob * e,
				Knob * f,
				Knob * g,
				Knob * h ) :
			m_bnkKnob( a ),
			m_sldKnob( b ),
			m_rtoKnob( c ),
			m_modKnob( d ),
			m_fhcKnob( e ),
			m_gnmKnob( f ),
			m_cutKnob( g ),
			m_rezKnob( h )
		{
		}
		voiceKnobs()
		{
		}
		Knob * m_bnkKnob;
		Knob * m_sldKnob;
		Knob * m_rtoKnob;
		Knob * m_modKnob;
		Knob * m_fhcKnob;
		Knob * m_gnmKnob;
		Knob * m_cutKnob;
		Knob * m_rezKnob;
	} ;

	voiceKnobs m_voiceKnobs[4];

protected slots:
	void updateKnobHint();
	void updateKnobToolTip();
} ;


#endif
