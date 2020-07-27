#include <windows.h>
#include <dshow.h>
#include <iostream>
#include "Mmdeviceapi.h"
#include <Functiondiscoverykeys_devpkey.h>

#pragma comment(lib, "strmiids")

HRESULT EnumerateSoundDevices(GUID clsidDeviceType) {
    // Create the System Device Enumerator.
    HRESULT hr;
    ICreateDevEnum* pSysDevEnum = NULL; //Create system Device Enumerator
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum, (void**)&pSysDevEnum); //Create uninitialized object of System Device Enumerator
    if (FAILED(hr))
    {
        printf("failed");
        return hr;
    }

    // Obtain a class enumerator for the category you wish to Enumerate for.
    IEnumMoniker* pEnumCat = NULL; //Create a pointer of type IEnumMoniker for Enumerating Monikers of system devices Represented by Component Object Model
    hr = pSysDevEnum->CreateClassEnumerator(clsidDeviceType, &pEnumCat, 0); //Create Class Enumerator and 

    if (hr == S_OK)
    {
        // Enumerate the monikers.
        IMoniker* pMoniker = NULL; //Create a pointer to store each moniker as we enumerate
        ULONG cFetched;
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) //While Enumerating the next Moniker make sure HRESULT returns ok
        {
            IPropertyBag* pPropBag; //Create a pointer of type IPropertyBag to store information about the device
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                (void**)&pPropBag); //Imoniker with IID_PropertyBag ReferenceID and store it in the pPropBag pointer address
            if (SUCCEEDED(hr))
            {
                // To retrieve the filter's friendly name, do the following:
                VARIANT varName; //Create a Variable of type Variant so that we can read the information stored at pPropBag Pointer 
                VariantInit(&varName);
                hr = pPropBag->Read(L"FriendlyName", &varName, 0); //parse pPropBag for L"Friendly Name" and store in address of Variant varName
                if (SUCCEEDED(hr))
                {
                    // Display the name in your UI somehow.
                    printf("Successfully Enumerated Device!");//Print device "Friendly" name
                    std::cout << " " << std::endl;
                    BSTR byteString = varName.bstrVal;
                    printf("Device Name: %S", varName.bstrVal); // here we use %S to Stringify the bytestring and we can decode the bytes into an actual device name
                    std::cout << " " << std::endl;
                }

                hr = pPropBag->Write(L"FriendlyName", &varName);
                VariantClear(&varName);

                // WaveInID applies only to audio capture devices.

                hr = pPropBag->Read(L"WaveInID", &varName, 0);
                if (SUCCEEDED(hr))
                {
                    printf("WaveIn ID: %d\n", varName.lVal);
                    VariantClear(&varName);
                }

                hr = pPropBag->Read(L"DevicePath", &varName, 0);
                if (SUCCEEDED(hr))
                {
                    // The device path is not intended for display.
                    printf("Device path: %S\n", varName.bstrVal);
                    VariantClear(&varName);
                }
                else {
                    std::cout << "could not read device path!" << std::endl;
                }

                VariantClear(&varName); //Release Variant from memory

                // To create an instance of the filter, do the following:
                /*IBaseFilter* pFilter;
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                    (void**)&pFilter);*/
                    // Now add the filter to the graph. 
                    // Remember to release pFilter later.
                pPropBag->Release();
            }
            pMoniker->Release();
        }
        pEnumCat->Release();
    }
    pSysDevEnum->Release();
}

IMMDevice* getDefaultInputAudioDevice() {
    HRESULT hr = S_OK; //In this case we indicate S_OK which is represented as a return value of 0 to verify the functions we ran were successful
    IMMDeviceEnumerator* pEnumerator = NULL; //Create a pointer of type IIMDeviceEnumerator where we can store our enumerated devices
    IMMDevice* pEndpoint = NULL; //Create our endpoint pointer indicating where we have hit an audio endpoint device
    IPropertyStore* pProps = NULL; //Create a pointer of type IPropertyStore where we can store information about our device
    LPWSTR pwszID = NULL;

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator,
        NULL,
        CLSCTX_ALL,
        IID_IMMDeviceEnumerator,
        (void**)&pEnumerator);

    PROPVARIANT varName;

    hr = pEnumerator->GetDefaultAudioEndpoint(
        eCapture,
        eCommunications,
        &pEndpoint
    );

    hr = pEndpoint->GetId(&pwszID);

    hr = pEndpoint->OpenPropertyStore(
        STGM_READ, &pProps);

    // Initialize container for property value.
    PropVariantInit(&varName);

    // Get the endpoint's friendly-name property.
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);

    printf("Default Endpoint for mic: \"%S\" (%S)\n", varName.pwszVal, pwszID);
    return pEndpoint;
}


//HRESULT EnumerateDevices(REFGUID category, IEnumMoniker** ppEnum)
//{
//    // Create the System Device Enumerator.
//    ICreateDevEnum* pDevEnum;
//    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
//        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
//
//    if (SUCCEEDED(hr))
//    {
//        // Create an enumerator for the category.
//        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
//        if (hr == S_FALSE)
//        {
//            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
//        }
//        pDevEnum->Release();
//    }
//    return hr;
//}