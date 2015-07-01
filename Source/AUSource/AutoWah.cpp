#include "AutoWah.h"


#pragma mark ____Construction_Initialization

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// AutoWah::AutoWah
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AutoWah::AutoWah(AudioUnit component)
: AUEffectBase(component)
{
	SetParameter(kAutoWahParam_Speed,		kDefaultSpeed);
	SetParameter(kAutoWahParam_Resonance,	kDefaultResonance);
	SetParameter(kAutoWahParam_Frequancy,	kDefaultFrequancy);
	SetParameter(kAutoWahParam_Range,		kDefaultRange);
	
	SetParamHasSampleRateDependency(false);
}


#pragma mark ____Parameters

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWah::GetParameterInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AutoWah::GetParameterInfo(	AudioUnitScope			inScope,
									AudioUnitParameterID	inParameterID,
									AudioUnitParameterInfo	&outParameterInfo )
{
	OSStatus result = noErr;
	
	outParameterInfo.flags =
		kAudioUnitParameterFlag_IsWritable + kAudioUnitParameterFlag_IsReadable;
	
	if (inScope == kAudioUnitScope_Global)
	{
		switch(inParameterID)
		{
			case kAutoWahParam_Speed:
				AUBase::FillInParameterName (outParameterInfo, kSpeed_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
				outParameterInfo.minValue = kMinSpeed;
				outParameterInfo.maxValue = kMaxSpeed;
				outParameterInfo.defaultValue = kDefaultSpeed;
				outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
				break;
				
			case kAutoWahParam_Resonance:
				AUBase::FillInParameterName (outParameterInfo, kResonance_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Decibels;
				outParameterInfo.minValue = kMinResonance;
				outParameterInfo.maxValue = kMaxResonance;
				outParameterInfo.defaultValue = kDefaultResonance;
				outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
				break;
				
			case kAutoWahParam_Frequancy:
				AUBase::FillInParameterName (outParameterInfo, kFrequancy_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Hertz;
				outParameterInfo.minValue = kMinFrequancy;
				outParameterInfo.maxValue = kMaxFrequancy;
				outParameterInfo.defaultValue = kDefaultFrequancy;
				break;
				
			case kAutoWahParam_Range:
				AUBase::FillInParameterName (outParameterInfo, kRange_Name, false);
				outParameterInfo.unit = kAudioUnitParameterUnit_Hertz;
				outParameterInfo.minValue = kMinRange;
				outParameterInfo.maxValue = kMaxRange;
				outParameterInfo.defaultValue = kDefaultRange;
				break;
				
			default:
				result = kAudioUnitErr_InvalidParameter;
				break;
		}
	}
	
	else
	{
		result = kAudioUnitErr_InvalidParameter;
	}
	
	return result;
}


#pragma mark ____Properties

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWah::GetPropertyInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AutoWah::GetPropertyInfo(	AudioUnitPropertyID		inID,
									AudioUnitScope			inScope,
									AudioUnitElement		inElement,
									UInt32					&outDataSize,
									Boolean					&outWritable)

{
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWah::GetProperty
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AutoWah::GetProperty(		AudioUnitPropertyID		inID,
									AudioUnitScope			inScope,
									AudioUnitElement		inElement,
									void					*outData)
{
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}


#pragma mark ____Presets

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWah::GetPresets
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AutoWah::GetPresets(CFArrayRef *outData) const
{
	if (outData == NULL) return noErr;
	
	CFMutableArrayRef theArray = CFArrayCreateMutable (NULL, kNumberPresets, NULL);
	for (int i = 0; i < kNumberPresets; ++i) {
		CFArrayAppendValue (theArray, &kPresets[i]);
	}
	
	*outData = (CFArrayRef)theArray;
	return noErr;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWah::NewFactoryPresetSet
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AutoWah::NewFactoryPresetSet(const AUPreset &inNewFactoryPreset)
{
	SInt32 chosenPreset = inNewFactoryPreset.presetNumber;
	
	for(int i = 0; i < kNumberPresets; ++i)
	{
		if(chosenPreset == kPresets[i].presetNumber)
		{
			switch(chosenPreset)
			{
				case kPreset_One:
					SetParameter(kAutoWahParam_Speed, 100);
					SetParameter(kAutoWahParam_Resonance, 30);
					break;
				case kPreset_Two:
					SetParameter(kAutoWahParam_Speed, 10);
					SetParameter(kAutoWahParam_Resonance, 40);
					break;
			}
			
			SetAFactoryPresetAsCurrent (kPresets[i]);
			return noErr;
		}
	}
	
	return kAudioUnitErr_InvalidPropertyValue;
}

#pragma mark ____AutoWahKernel

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWahKernel::AutoWahKernel()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AutoWahKernel::AutoWahKernel(AUEffectBase *inAudioUnit)
: AUKernelBase(inAudioUnit)
{
	Reset();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWahKernel::~AutoWahKernel()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AutoWahKernel::~AutoWahKernel( )
{
	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWahKernel::Reset()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AutoWahKernel::Reset()
{
	mX1 = 0.0;
	mX2 = 0.0;
	mY1 = 0.0;
	mY2 = 0.0;
	
	mLastCutoff = 2.0 * 300.0 / GetSampleRate();
	CalculateLopassParams(mLastCutoff, kDefaultResonance);
	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	AutoWahKernel::GetFrequencyResponse()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AutoWahKernel::CalculateLopassParams(double inFreq, double inResonance)
{
	double r = pow(10.0, 0.05 * -inResonance);		// convert from decibels to linear
	
	double k = 0.5 * r * sin(M_PI * inFreq);
	double c1 = 0.5 * (1.0 - k) / (1.0 + k);
	double c2 = (0.5 + c1) * cos(M_PI * inFreq);
	double c3 = (0.5 + c1 - c2) * 0.25;
	
	mA0 = 2.0 *   c3;
	mA1 = 2.0 *   2.0 * c3;
	mA2 = 2.0 *   c3;
	mB1 = 2.0 *   -c2;
	mB2 = 2.0 *   c1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  AutoWahKernel::Process(int inFramesToProcess)
//
//		We process one non-interleaved stream at a time
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AutoWahKernel::Process( const	Float32		*inSourceP,
									Float32		*inDestP,
									UInt32 		inFramesToProcess,
									UInt32		inNumChannels, // for version 2 AudioUnits inNumChannels is always 1
									bool		&ioSilence)
{
	
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	int n = inFramesToProcess;
	
	double cutoff = 0;
	float srate = GetSampleRate();
	double resonance = GetParameter(kAutoWahParam_Resonance);
	double speed = GetParameter(kAutoWahParam_Speed);
	double freq = GetParameter(kAutoWahParam_Frequancy);
	double range = GetParameter(kAutoWahParam_Range);
	
	
	
	// debug
#ifdef DEBUG_PATH
	std::ofstream debug(DEBUG_PATH, std::ios::out | std::ios::app);
#endif
	
	while(n--)
	{
		float input = *sourceP++;
		float output = mA0*input + mA1*mX1 + mA2*mX2 - mB1*mY1 - mB2*mY2;
		
		mX2 = mX1;
		mX1 = input;
		mY2 = mY1;
		mY1 = output;
		
		cutoff += input * input;
		*destP++ = output;
	}

	// calculate RMS and convert cutoff frequancy
	cutoff = sqrt(cutoff / inFramesToProcess);
	cutoff = 20 * log(cutoff) + 450;
	if(cutoff < 0.0) cutoff = 0; // underflow
	cutoff *= 10;
	
	// update cutoff frequancy
	cutoff = 2.0 * cutoff / srate;
	if(cutoff > mLastCutoff){
		mLastCutoff += (2.0 * speed / srate);
		if(cutoff < mLastCutoff) mLastCutoff = cutoff;
	}
	else if(cutoff < mLastCutoff){
		mLastCutoff -= (2.0 * speed / srate);
		if(cutoff > mLastCutoff) mLastCutoff = cutoff;
	}
	
	CalculateLopassParams(mLastCutoff, resonance);
	
#ifdef DEBUG_PATH
	debug << "fre: " << cutoff << "     range: " << mLastCutoff << std::endl;
#endif
	
}