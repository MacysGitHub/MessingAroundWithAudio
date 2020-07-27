/*Create a sink for audio data to flow into*/
#include <Windows.h>
#include <time.h>

class MyAudioSink
{
public:
	HRESULT SetFormat(WAVEFORMATEX* pWF);
	HRESULT CopyData(BYTE* pData, UINT32 NumFrames, BOOL* pDone);
};


HRESULT MyAudioSink::SetFormat(WAVEFORMATEX* pWF) //set the format in which to buff audio as
{

	// For the time being, just return OK.
	return (S_OK);
}

HRESULT MyAudioSink::CopyData(BYTE* pData, UINT32 NumFrames, BOOL* pDone) //devise a function to copy the audio data
{
	static int CallCount = 0;
	//cout << "CallCount = " << CallCount++ << endl ;
	// For the time being, just pretend to record for 15 seconds.
	//byte buffer[(int)NumFrames];//create a buffer array the size of NumFrames
	
	if (clock() > 15 * CLOCKS_PER_SEC) *pDone = true; //when the clock cycles per second are greater than 15 set the boolean pointer to true in which case we can then exit the loop
	else *pDone = false;

	return S_OK;
}