#include <Windows.h>
#include <AudioClient.h>
#include <iostream>

class MyAudioSource
{
public:
	HRESULT SetFormat(WAVEFORMATEX* pWF);
	HRESULT LoadData(UINT32 bufferFrameCount, BYTE *pData, DWORD flags);
};

HRESULT SetFormat(WAVEFORMAT* pWF) {
	std::cout << "set sound data format to wav..." << std::endl;
	return S_OK;
}

HRESULT MyAudioSource::LoadData(UINT32 bufferFrameCount, BYTE* pData, DWORD flags) //devise a function to load the audio data and send to and endpoint device for output
{
	static int CallCount = 0;
	//cout << "CallCount = " << CallCount++ << endl ;
	// For the time being, just pretend to record for 15 seconds.
	//byte buffer[(int)NumFrames];//create a buffer array the size of NumFrames
	while (flags == AUDCLNT_BUFFERFLAGS_SILENT) {

		if (flags != AUDCLNT_BUFFERFLAGS_SILENT) {
			//when buffer flags are silent data is done being loaded and we can safely exit loop
		}
	}
	

	return S_OK;
}