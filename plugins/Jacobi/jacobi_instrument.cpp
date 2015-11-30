/*
 * jacobi_instrument.cpp - software-synthesizer
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


#include <QPainter>
#include <QDomElement>

#include <cstdio>

#include "jacobi_instrument.h"
#include "Engine.h"
#include "InstrumentTrack.h"
#include "Knob.h"
#include "NotePlayHandle.h"
#include "PixmapButton.h"
#include "ToolTip.h"

#include "embed.cpp"


extern "C"
{
Plugin::Descriptor PLUGIN_EXPORT jacobi_plugin_descriptor =
{
	STRINGIFY( PLUGIN_NAME ),
	"JACOBI",
	QT_TRANSLATE_NOOP( "pluginBrowser", "An PM subtractive wave shaping hybrid." ),
	"Simon Jackson <jackokring/at/gmail.com>",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader( "logo" ),
	NULL,
	NULL
} ;

}

voiceObject::voiceObject( Model * _parent, int _idx ) :
	Model( _parent ),
	m_bnk( 2048.0f, 0.0f, 4095.0f, 1.0f, this,
					tr( "Row %1 bank select" ).arg( _idx+1 ) ) 
{
	for(int i = 0; i < BNK; ++i) {
		m_sld[i]( 8.0f, 0.0f, 15.0f, 1.0f, this,
						tr( "Row %1 parameter slide" ).arg( _idx+1 ) );
		m_rto_drv[i]( 8.0f, 0.0f, 15.0f, 1.0f, this,
						tr( "Row %1 frequency ratio or drive" ).arg( _idx+1 ) );
		m_mod_gen[i]( 15.0f, 0.0f, 15.0f, 1.0f, this,
						tr( "Row %1 modulation depth or generation" ).arg( _idx+1 ) );
		m_fhc_wso_trt[i]( 8.0f, 0.0f, 15.0f, 1.0f, this,
						tr( "Row %1 effect A" ).arg( _idx+1 ) );
		m_gnm_rdf_sso_pq[i]( 0.0f, -24.0, 24.0, 1.0f, this,
						tr( "Row %1 effect B" ).arg( _idx+1 ) );
		m_cut[i]( 0.0f, -24.0, 24.0, 1.0f, this,
						tr( "Row %1 filter cutoff" ).arg( _idx+1 ) );
		m_rez[i]( 0.0f, -24.0, 24.0, 1.0f, this,
						tr( "Row %1 LPF to BPF resonnance" ).arg( _idx+1 ) );
	}
	
}

voiceObject::~voiceObject()
{
}

jacobiInstrument::jacobiInstrument( InstrumentTrack * _instrument_track ) :
	Instrument( _instrument_track, &jacobi_plugin_descriptor )
{
	for( int i = 0; i < 4; ++i )
	{
		m_voice[i] = new voiceObject( this, i );
	}
}

jacobiInstrument::~jacobiInstrument()
{
	for( int i = 0; i < 4; ++i )
	{
		//avoid memory leaks
		delete m_voice[i];
	}
}

void jacobiInstrument::saveSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	// voices
	for( int i = 0; i < 4; ++i )
	{
		const QString isb = QString::number( i );
		m_voice[i]->m_bnk.saveSettings(_doc, _this, "bnk" + isb );
		for( int j = 0; j < BNK; ++j )
		{
			const QString is = isb + "_" + QString::number( j );
			m_voice[i]->m_sld[j].saveSettings(_doc, _this, "sld" + is );
			m_voice[i]->m_rto_drv[j].saveSettings(_doc, _this, "rto" + is );
			m_voice[i]->m_mod_gen[j].saveSettings(_doc, _this, "mod" + is );
			m_voice[i]->m_fhc_wso_trt[j].saveSettings(_doc, _this, "fhc" + is );
			m_voice[i]->m_gnm_rdf_sso_pq[j].saveSettings(_doc, _this, "gnm" + is );
			m_voice[i]->m_cut[j].saveSettings(_doc, _this, "cut" + is );
			m_voice[i]->m_rez[j].saveSettings(_doc, _this,"rez" + is );
		}
	}
}

void jacobiInstrument::loadSettings( const QDomElement & _this )
{
	// voices
	for( int i = 0; i < 4; ++i )
	{
		const QString isb = QString::number( i );
		m_voice[i]->m_bnk.loadSettings(_this, "bnk" + isb );
		for( int j = 0; j < BNK; ++j )
		{
			const QString is = isb + "_" + QString::number( j );
			m_voice[i]->m_sld[j].loadSettings(_this, "sld" + is );
			m_voice[i]->m_rto_drv[j].loadSettings(_this, "rto" + is );
			m_voice[i]->m_mod_gen[j].loadSettings(_this, "mod" + is );
			m_voice[i]->m_fhc_wso_trt[j].loadSettings(_this, "fhc" + is );
			m_voice[i]->m_gnm_rdf_sso_pq[j].loadSettings(_this, "gnm" + is );
			m_voice[i]->m_cut[j].loadSettings(_this, "cut" + is );
			m_voice[i]->m_rez[j].loadSettings(_this,"rez" + is );
		}
	}
}

QString jacobiInstrument::nodeName() const
{
	return( jacobi_plugin_descriptor.name );
}

void jacobiInstrument::playNote( NotePlayHandle * _n,
						sampleFrame * _working_buffer )
{
	//TODO
	instrumentTrack()->processAudioBuffer( _working_buffer, frames + offset, _n );
}

void jacobiInstrument::deleteNotePluginData( NotePlayHandle * _n )
{
	delete static_cast<cjacobi *>( _n->m_pluginData );
}

PluginView * jacobiInstrument::instantiateView( QWidget * _parent )
{
	return( new jacobiInstrumentView( this, _parent ) );
}

class jacobiKnob : public Knob
{
public:
	jacobiKnob( QWidget * _parent ) :
			Knob( knobStyled, _parent )
	{
		setFixedSize( 16, 16 );
		setCenterPointX( 7.5 );
		setCenterPointY( 7.5 );
		setInnerRadius( 2 );
		setOuterRadius( 8 );
		setTotalAngle( 270.0 );
		setLineWidth( 2 );
	}
};

jacobiInstrumentView::jacobiInstrumentView( Instrument * _instrument,
							QWidget * _parent ) :
	InstrumentView( _instrument, _parent )
{

	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );

	m_volKnob = new jacobiKnob( this );
	m_volKnob->setHintText( tr( "Volume:" ), "" );
	m_volKnob->move( 7, 64 );

	m_resKnob = new jacobiKnob( this );
	m_resKnob->setHintText( tr( "Resonance:" ), "" );
	m_resKnob->move( 7 + 28, 64 );

	m_cutKnob = new jacobiKnob( this );
	m_cutKnob->setHintText( tr( "Cutoff frequency:" ), "Hz" );
	m_cutKnob->move( 7 + 2*28, 64 );

	PixmapButton * hp_btn = new PixmapButton( this, NULL );
	hp_btn->move( 140, 77 );
	hp_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "hpred" ) );
	hp_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "hp" ) );
	ToolTip::add( hp_btn, tr( "High-Pass filter ") );

	PixmapButton * bp_btn = new PixmapButton( this, NULL );
	bp_btn->move( 164, 77 );
	bp_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "bpred" ) );
	bp_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "bp" ) );
	ToolTip::add( bp_btn, tr( "Band-Pass filter ") );

	PixmapButton * lp_btn = new PixmapButton( this, NULL );
	lp_btn->move( 185, 77 );
	lp_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "lpred" ) );
	lp_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "lp" ) );
	ToolTip::add( lp_btn, tr( "Low-Pass filter ") );

	m_passBtnGrp = new automatableButtonGroup( this );
	m_passBtnGrp->addButton( hp_btn );
	m_passBtnGrp->addButton( bp_btn );
	m_passBtnGrp->addButton( lp_btn );

	m_offButton = new PixmapButton( this, NULL );
	m_offButton->setCheckable( true );
	m_offButton->move( 207, 77 );
	m_offButton->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "3offred" ) );
	m_offButton->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "3off" ) );
	ToolTip::add( m_offButton, tr( "Voice3 Off ") );

	PixmapButton * mos6581_btn = new PixmapButton( this, NULL );
	mos6581_btn->move( 170, 59 );
	mos6581_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "6581red" ) );
	mos6581_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "6581" ) );
	ToolTip::add( mos6581_btn, tr( "MOS6581 jacobi ") );

	PixmapButton * mos8580_btn = new PixmapButton( this, NULL );
	mos8580_btn->move( 207, 59 );
	mos8580_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap( "8580red" ) );
	mos8580_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( "8580" ) );
	ToolTip::add( mos8580_btn, tr( "MOS8580 jacobi ") );

	m_jacobiTypeBtnGrp = new automatableButtonGroup( this );
	m_jacobiTypeBtnGrp->addButton( mos6581_btn );
	m_jacobiTypeBtnGrp->addButton( mos8580_btn );

	for( int i = 0; i < 3; i++ ) 
	{
		Knob *ak = new jacobiKnob( this );
		ak->setHintText( tr("Attack:"), "" );
		ak->move( 7, 114 + i*50 );
		ak->setWhatsThis( tr ( "Attack rate determines how rapidly the output "
				"of Voice %1 rises from zero to peak amplitude." ).arg( i+1 ) );

		Knob *dk = new jacobiKnob( this );
		dk->setHintText( tr("Decay:") , "" );
		dk->move( 7 + 28, 114 + i*50 );
		dk->setWhatsThis( tr ( "Decay rate determines how rapidly the output "
				"falls from the peak amplitude to the selected Sustain level." ) );

		Knob *sk = new jacobiKnob( this );
		sk->setHintText( tr("Sustain:"), "" );
		sk->move( 7 + 2*28, 114 + i*50 );
		sk->setWhatsThis( tr ( "Output of Voice %1 will remain at the selected "
				"Sustain amplitude as long as the note is held." ).arg( i+1 ) );

		Knob *rk = new jacobiKnob( this );
		rk->setHintText( tr("Release:"), "" );
		rk->move( 7 + 3*28, 114 + i*50 );
		rk->setWhatsThis( tr ( "The output of of Voice %1 will fall from "
				"Sustain amplitude to zero amplitude at the selected Release "
				"rate." ).arg( i+1 ) );

		Knob *pwk = new jacobiKnob( this );
		pwk->setHintText( tr("Pulse Width:"), "" );
		pwk->move( 7 + 4*28, 114 + i*50 );
		pwk->setWhatsThis( tr ( "The Pulse Width resolution allows the width "
				"to be smoothly swept with no discernable stepping. The Pulse "
				"waveform on Oscillator %1 must be selected to have any audible"
				" effect." ).arg( i+1 ) );

		Knob *crsk = new jacobiKnob( this );
		crsk->setHintText( tr("Coarse:"), " semitones" );
		crsk->move( 147, 114 + i*50 );
		crsk->setWhatsThis( tr ( "The Coarse detuning allows to detune Voice "
				"%1 one octave up or down." ).arg( i+1 ) );

		PixmapButton * pulse_btn = new PixmapButton( this, NULL );
		pulse_btn->move( 187, 101 + i*50 );
		pulse_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "pulsered" ) );
		pulse_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "pulse" ) );
		ToolTip::add( pulse_btn, tr( "Pulse Wave" ) );

		PixmapButton * triangle_btn = new PixmapButton( this, NULL );
		triangle_btn->move( 168, 101 + i*50 );
		triangle_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "trianglered" ) );
		triangle_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "triangle" ) );
		ToolTip::add( triangle_btn, tr( "Triangle Wave" ) );

		PixmapButton * saw_btn = new PixmapButton( this, NULL );
		saw_btn->move( 207, 101 + i*50 );
		saw_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "sawred" ) );
		saw_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "saw" ) );
		ToolTip::add( saw_btn, tr( "SawTooth" ) );

		PixmapButton * noise_btn = new PixmapButton( this, NULL );
		noise_btn->move( 226, 101 + i*50 );
		noise_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "noisered" ) );
		noise_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "noise" ) );
		ToolTip::add( noise_btn, tr( "Noise" ) );

		automatableButtonGroup * wfbg =
			new automatableButtonGroup( this );

		wfbg->addButton( pulse_btn );
		wfbg->addButton( triangle_btn );
		wfbg->addButton( saw_btn );
		wfbg->addButton( noise_btn );

		int syncRingWidth[] = { 3, 1, 2 };

		PixmapButton * sync_btn = new PixmapButton( this, NULL );
		sync_btn->setCheckable( true );
		sync_btn->move( 207, 134 + i*50 );
		sync_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "syncred" ) );
		sync_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "sync" ) );
		ToolTip::add( sync_btn, tr( "Sync" ) );
		sync_btn->setWhatsThis( tr ( "Sync synchronizes the fundamental "
			"frequency of Oscillator %1 with the fundamental frequency of "
			"Oscillator %2 producing \"Hard Sync\" effects." ).arg( i+1 )
			.arg( syncRingWidth[i] ) );

		PixmapButton * ringMod_btn = new PixmapButton( this, NULL );
		ringMod_btn->setCheckable( true );
		ringMod_btn->move( 170, 116 + i*50 );
		ringMod_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "ringred" ) );
		ringMod_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "ring" ) );
		ToolTip::add( ringMod_btn, tr( "Ring-Mod" ) );
		ringMod_btn->setWhatsThis( tr ( "Ring-mod replaces the Triangle "
			"Waveform output of Oscillator %1 with a \"Ring Modulated\" "
			"combination of Oscillators %1 and %2." ).arg( i+1 )
			.arg( syncRingWidth[i] ) );

		PixmapButton * filter_btn = new PixmapButton( this, NULL );
		filter_btn->setCheckable( true );
		filter_btn->move( 207, 116 + i*50 );
		filter_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "filterred" ) );
		filter_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "filter" ) );
		ToolTip::add( filter_btn, tr( "Filtered" ) );
		filter_btn->setWhatsThis( tr ( "When Filtered is on, Voice %1 will be "
			"processed through the Filter. When Filtered is off, Voice %1 "
			"appears directly at the output, and the Filter has no effect on "
			"it." ).arg( i+1 ) );

		PixmapButton * test_btn = new PixmapButton( this, NULL );
		test_btn->setCheckable( true );
		test_btn->move( 170, 134 + i*50 );
		test_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "testred" ) );
		test_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "test" ) );
		ToolTip::add( test_btn, tr( "Test" ) );
		test_btn->setWhatsThis( tr ( "Test, when set, resets and locks "
			"Oscillator %1 at zero until Test is turned off." ).arg( i+1 ) );

		m_voiceKnobs[i] = voiceKnobs( ak, dk, sk, rk, pwk, crsk, wfbg,
								sync_btn, ringMod_btn, filter_btn, test_btn );
	}
}


jacobiInstrumentView::~jacobiInstrumentView()
{
}

void jacobiInstrumentView::updateKnobHint()
{
	jacobiInstrument * k = castModel<jacobiInstrument>();

	for( int i = 0; i < 3; ++i )
	{
		m_voiceKnobs[i].m_attKnob->setHintText( tr( "Attack:" ) + " ", " (" +
				QString::fromLatin1( attackTime[(int)k->m_voice[i]->
				m_attackModel.value()] ) + ")" );
		ToolTip::add( m_voiceKnobs[i].m_attKnob,
						attackTime[(int)k->m_voice[i]->m_attackModel.value()] );

		m_voiceKnobs[i].m_decKnob->setHintText( tr( "Decay:" ) + " ", " (" +
				QString::fromLatin1( decRelTime[(int)k->m_voice[i]->
				m_decayModel.value()] ) + ")" );
		ToolTip::add( m_voiceKnobs[i].m_decKnob,
						decRelTime[(int)k->m_voice[i]->m_decayModel.value()] );

		m_voiceKnobs[i].m_relKnob->setHintText( tr( "Release:" ) + " ", " (" +
				QString::fromLatin1( decRelTime[(int)k->m_voice[i]->
				m_releaseModel.value()] )  + ")" );
		ToolTip::add( m_voiceKnobs[i].m_relKnob,
						decRelTime[(int)k->m_voice[i]->m_releaseModel.value()]);
	
		m_voiceKnobs[i].m_pwKnob->setHintText( tr( "Pulse Width:" )+ " ", " (" +
				QString::number(  (double)k->m_voice[i]->
				m_pulseWidthModel.value() / 40.95 ) + "%)" );
		ToolTip::add( m_voiceKnobs[i].m_pwKnob,
				QString::number( (double)k->m_voice[i]->
				m_pulseWidthModel.value() / 40.95 ) + "%" );
	}
	m_cutKnob->setHintText( tr( "Cutoff frequency:" ) + " ", " (" +
				QString::number ( (int) ( 9970.0 / 2047.0 *
				(double)k->m_filterFCModel.value() + 30.0 ) ) + "Hz)" );
	ToolTip::add( m_cutKnob, QString::number( (int) ( 9970.0 / 2047.0 *
					 (double)k->m_filterFCModel.value() + 30.0 ) ) + "Hz" );
}




void jacobiInstrumentView::updateKnobToolTip()
{
	jacobiInstrument * k = castModel<jacobiInstrument>();
	for( int i = 0; i < 3; ++i )
	{
		ToolTip::add( m_voiceKnobs[i].m_sustKnob,
				QString::number( (int)k->m_voice[i]->m_sustainModel.value() ) );
		ToolTip::add( m_voiceKnobs[i].m_crsKnob,
				QString::number( (int)k->m_voice[i]->m_coarseModel.value() ) +
				" semitones" );
	}
	ToolTip::add( m_volKnob,
					QString::number( (int)k->m_volumeModel.value() ) );
	ToolTip::add( m_resKnob,
					QString::number( (int)k->m_filterResonanceModel.value() ) );
}

void jacobiInstrumentView::modelChanged()
{
	jacobiInstrument * k = castModel<jacobiInstrument>();

	m_volKnob->setModel( &k->m_volumeModel );
	m_resKnob->setModel( &k->m_filterResonanceModel );
	m_cutKnob->setModel( &k->m_filterFCModel );
	m_passBtnGrp->setModel( &k->m_filterModeModel );
	m_offButton->setModel(  &k->m_voice3OffModel );
	m_jacobiTypeBtnGrp->setModel(  &k->m_chipModel );

	for( int i = 0; i < 3; ++i )
	{
		m_voiceKnobs[i].m_attKnob->setModel(
					&k->m_voice[i]->m_attackModel );
		m_voiceKnobs[i].m_decKnob->setModel(
					&k->m_voice[i]->m_decayModel );
		m_voiceKnobs[i].m_sustKnob->setModel(
					&k->m_voice[i]->m_sustainModel );
		m_voiceKnobs[i].m_relKnob->setModel(
					&k->m_voice[i]->m_releaseModel );
		m_voiceKnobs[i].m_pwKnob->setModel(
					&k->m_voice[i]->m_pulseWidthModel );
		m_voiceKnobs[i].m_crsKnob->setModel(
					&k->m_voice[i]->m_coarseModel );
		m_voiceKnobs[i].m_waveFormBtnGrp->setModel(
					&k->m_voice[i]->m_waveFormModel );
		m_voiceKnobs[i].m_syncButton->setModel(
					&k->m_voice[i]->m_syncModel );
		m_voiceKnobs[i].m_ringModButton->setModel(
					&k->m_voice[i]->m_ringModModel );
		m_voiceKnobs[i].m_filterButton->setModel(
					&k->m_voice[i]->m_filteredModel );
		m_voiceKnobs[i].m_testButton->setModel(
					&k->m_voice[i]->m_testModel );
	}

	for( int i = 0; i < 3; ++i )
	{
		connect( &k->m_voice[i]->m_attackModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobHint() ) );
		connect( &k->m_voice[i]->m_decayModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobHint() ) );
		connect( &k->m_voice[i]->m_releaseModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobHint() ) );
		connect( &k->m_voice[i]->m_pulseWidthModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobHint() ) );
		connect( &k->m_voice[i]->m_sustainModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobToolTip() ) );
		connect( &k->m_voice[i]->m_coarseModel, SIGNAL( dataChanged() ),
			this, SLOT( updateKnobToolTip() ) );
	}
	
	connect( &k->m_volumeModel, SIGNAL( dataChanged() ),
		this, SLOT( updateKnobToolTip() ) );
	connect( &k->m_filterResonanceModel, SIGNAL( dataChanged() ),
		this, SLOT( updateKnobToolTip() ) );
	connect( &k->m_filterFCModel, SIGNAL( dataChanged() ),
		this, SLOT( updateKnobHint() ) );

	updateKnobHint();
	updateKnobToolTip();
}

extern "C"
{

// necessary for getting instance out of shared lib
Plugin * PLUGIN_EXPORT lmms_plugin_main( Model *, void * _data )
{
	return( new jacobiInstrument(
				static_cast<InstrumentTrack *>( _data ) ) );
}


}




