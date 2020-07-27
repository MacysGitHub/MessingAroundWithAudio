#pragma once

HRESULT EnumerateSoundDevices(GUID clsidDeviceType);
IMMDevice* getDefaultInputAudioDevice();