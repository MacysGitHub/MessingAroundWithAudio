#include "windows.h"
#include "Mmdeviceapi.h"
#include <iostream>
#include <combaseapi.h>
#include <objidl.h>
#include <dshow.h>
#include <AudioClient.h>
#include <time.h>

#include <cstdio>
#include <mmdeviceapi.h>
#include "DeviceEnumerator.h"
#include "MyAudioSink.h"

#include <Functiondiscoverykeys_devpkey.h>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const GUID CLSID_AudioDevs = CLSID_AudioInputDeviceCategory;
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
//MyAudioSink* mAS; still need to define the audio sink functions within the AudioSink class so that we can officially use it d:

constexpr auto REFTIMES_PER_SEC = (10000000 * 25);
constexpr auto REFTIMES_PER_MILLISEC = 10000;

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

void ListEndpoints();
void RecordFromMic();

int main()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//ListEndpoints();
	//getDefaultAudioDevice();
	//EnumerateSoundDevices(CLSID_AudioDevs);
	RecordFromMic();
}

void RecordFromMic() {
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	UINT32 numFramesPadding;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioCaptureClient* pCaptureClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE* pData;
	DWORD flags;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		pDevice = getDefaultInputAudioDevice();

	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr)

	std::cout << "speakerDevice: " << pDevice << std::endl;

	std::cout << "pAudioClient " << pAudioClient << std::endl;

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr)

	//hr = speakerAudioClient->GetMixFormat(&pwfx);

	std::cout << "Wave Format Samples per second: " << pwfx->nSamplesPerSec << std::endl;

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	EXIT_ON_ERROR(hr)

		// Get the size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

	std::cout << "Realtek microphone data buffer size: " << bufferFrameCount << std::endl;

	hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr)

	std::cout << "Capture Client Successfully Initialized: " << pCaptureClient << std::endl;

	// Notify the audio sink which format to use.
	/*hr = pMySink->SetFormat(pwfx);
EXIT_ON_ERROR(hr)*/

// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	std::cout << "Duration of allocated Buffer: " << hnsActualDuration << std::endl;

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr)


		while (bDone == false) {

			hr = pCaptureClient->GetNextPacketSize(&packetLength); //get size of incoming audio data packet
			EXIT_ON_ERROR(hr)

				/*std::cout << "Incoming Packet Length: " << packetLength << std::endl;*/

				while (packetLength != 0) //When we start receiving packets that are not of 0 value begin our loop of receiving incoming audio data
				{
					hr = pCaptureClient->GetBuffer(
						&pData,
						&numFramesAvailable,
						&flags, NULL, NULL); //Create a buffer based on the audio capture client's specifications (since every audio device is different in the way it provides audio data)
					EXIT_ON_ERROR(hr)

						std::cout << "Flags used: " << flags << std::endl;;

						std::cout << "Number Frames Available: " << numFramesAvailable << std::endl;

					std::cout << pData << std::endl;

					if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
					{
						pData = NULL;  // Tell CopyData to write silence.
					}

					hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
					EXIT_ON_ERROR(hr)

						hr = pCaptureClient->GetNextPacketSize(&packetLength);
					EXIT_ON_ERROR(hr)

				}
		}

	// Copy the available capture data to the audio sink.
	/*hr = pMySink->CopyData(
		pData, numFramesAvailable, &bDone);
	EXIT_ON_ERROR(hr)*/

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)

		Exit:
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pCaptureClient)
}

//-----------------------------------------------------------
// This function enumerates all active (plugged in) audio
// rendering endpoint devices. It prints the friendly name
// and endpoint ID string of each endpoint device.
//-----------------------------------------------------------


void ListEndpoints() {
	HRESULT hr = S_OK; //In this case we indicate S_OK which is represented as a return value of 0
	IMMDeviceEnumerator* pEnumerator = NULL; //Create a pointer of type IIMDeviceEnumerator where we can store our enumerated devices
	IMMDeviceCollection* pCollection = NULL; //Create pointer of type IMMDeviceCollection where store our collection of enumerated devices
	IMMDevice* pEndpoint = NULL; //Create our endpoint pointer indicating where we have hit an audio endpoint device
	IPropertyStore* pProps = NULL; //create pointer to property store
	LPWSTR pwszID = NULL;

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator,
		NULL,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		(void**)&pEnumerator); //Creates a single uninitialized object of the class associated with a specified CLSID, in this case, CLSID_MMEnumerator with a reference identifier: IID_IMMDeviceEnumerator
	EXIT_ON_ERROR(hr);

	std::cout << "CoCreateInstance pointer that we can interface with through memory: " << pEnumerator << std::endl;

	hr = pEnumerator->EnumAudioEndpoints(eCapture,
		DEVICE_STATE_ACTIVE,
		&pCollection);
	EXIT_ON_ERROR(hr);

	std::cout << "Enum Endpoints to pointer memory address &pCollection: " << pCollection << std::endl;

	UINT count;
	hr = pCollection->GetCount(&count); // Get the number of enumerated devices
	EXIT_ON_ERROR(hr);

	std::cout << "Device Collection Pointer Address: " << pCollection << std::endl << "Our Device Iterator Counter: " << count << std::endl;

	if (count == 0)
	{
		printf("No endpoints found.\n");
	}

	// Each iteration prints the name of an endpoint device.
	PROPVARIANT varName;
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = pCollection->Item(i, &pEndpoint);
		EXIT_ON_ERROR(hr);

		std::cout << "Collection Pointer: " << pCollection << std::endl;

		// Get the endpoint ID string.
		hr = pEndpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr);

		std::cout << "Endpoint pointer " << i << ":" << pEndpoint << std::endl;

		hr = pEndpoint->OpenPropertyStore(
			STGM_READ, &pProps);
		EXIT_ON_ERROR(hr);

		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr);

		// Print endpoint friendly name and endpoint ID.
		printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, pwszID);

	}

Exit:
	CoTaskMemFree(pwszID);
	pwszID = NULL;
	PropVariantClear(&varName);
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);
	SAFE_RELEASE(pEndpoint);
	SAFE_RELEASE(pProps);
}