
#ifndef AutoWah_AutoWah_h
#define AutoWah_AutoWah_h
#endif

#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "AutoWahVersion.h"
#include <math.h>

//If you want to debug, please enter the path of the debug between ""
//#define DEBUG_PATH ""

#ifdef DEBUG_PATH
#include <fstream>
#endif

#pragma mark ____AutoWahKernel

class AutoWahKernel : public AUKernelBase
{
public:
	AutoWahKernel(AUEffectBase *inAudioUnit);
	virtual ~AutoWahKernel();
	
	virtual void Process(	const Float32   *inSourceP,
							Float32         *inDestP,
							UInt32          inFramesToProcess,
							UInt32          inNumChannels,
							bool            &ioSilence );
	
	void CalculateLopassParams(	double inFreq,
								double inResonance );
	
	virtual void Reset();
	
private:
	// filter coefficients
	double	mA0;
	double	mA1;
	double	mA2;
	double	mB1;
	double	mB2;
	
	// filter state
	double	mX1;
	double	mX2;
	double	mY1;
	double	mY2;
	
	double	mLastCutoff;
	double	buf;
	int		flag;
};


#pragma mark ____AutoWah

class AutoWah : public AUEffectBase
{
public:
	AutoWah(AudioUnit component);
	
	virtual OSStatus Version() { return kAutoWahVersion; }
	
	virtual AUKernelBase *NewKernel() { return new AutoWahKernel(this); }
	
	virtual OSStatus GetPropertyInfo(	AudioUnitPropertyID		inID,
										AudioUnitScope			inScope,
										AudioUnitElement		inElement,
										UInt32                  &outDataSize,
										Boolean                 &outWritable );
	
	virtual OSStatus GetProperty(		AudioUnitPropertyID 	inID,
										AudioUnitScope 			inScope,
										AudioUnitElement 		inElement,
										void 					*outData );
	
	virtual OSStatus GetParameterInfo(  AudioUnitScope			inScope,
										AudioUnitParameterID	inParameterID,
										AudioUnitParameterInfo	&outParameterInfo );
	
	virtual OSStatus GetPresets(CFArrayRef *outData) const;
	virtual OSStatus NewFactoryPresetSet (const AUPreset &inNewFactoryPreset);
	
protected:
};


AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, AutoWah)


enum
{
	kAutoWahParam_Speed			= 0,
	kAutoWahParam_Resonance		= 1,
	kAutoWahParam_Frequancy		= 2,
	kAutoWahParam_Range			= 3
};


static CFStringRef kSpeed_Name		= CFSTR("Speed");
static CFStringRef kResonance_Name	= CFSTR("Q");
static CFStringRef kFrequancy_Name	= CFSTR("Freq");
static CFStringRef kRange_Name		= CFSTR("Range");


const long kMinSpeed			= 10;
const long kMaxSpeed			= 200;
const long kDefaultSpeed		= 100;

const long kMinResonance		= 0;
const long kMaxResonance		= 40;
const long kDefaultResonance	= 20;

const long kMinFrequancy		= 100;
const long kMaxFrequancy		= 500;
const long kDefaultFrequancy	= 200;

const long kMinRange			= 100;
const long kMaxRange			= 2000;
const long kDefaultRange		= 1000;



// Factory presets
static const int kPreset_One	= 0;
static const int kPreset_Two	= 1;
static const int kNumberPresets	= 2;

static AUPreset kPresets[kNumberPresets] =
{
	{ kPreset_One, CFSTR("Quacky Wah") },
	{ kPreset_Two, CFSTR("Spacey Wah") }
};

static const int kPresetDefault = kPreset_One;
static const int kPresetDefaultIndex = 0;
