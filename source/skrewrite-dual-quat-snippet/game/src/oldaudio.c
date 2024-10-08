


#if 0
#include "sakuren.h"

#define MINIAUDIO_IMPLMENTATION
#include <miniaudio.h>

/* Audio (currently miniaudio because dsound is deprecated, 
          XAudio2 isn't really supported in C because of COM, and
          WASAPI is really annoying to work with on C.
          Miniaudio is better because there it removes the component object model (COM) nonsense from WASAPI) */

#define MA_DEVICE_FORMAT      ma_format_f32
#define MA_DEVICE_CHANNELS    2
#define MA_DEVICE_SAMPLE_RATE 48000

static void
on_send_audio_data_to_device(ma_device *device, void *frames_out, const void *frames_input, ma_uint32 frame_count)
{
    // This function will be called when miniaudio needs more data.
    // Mix Audio and send to device:

    // NOTE: initialize output buffer to 0
    // zero_memory(frames_out, 0, frame_count * device->playback.channels * ma_get_bytes_per_sample(device->playback.format));

    ma_waveform* sinewave;

    MA_ASSERT(device->playback.channels == DEVICE_CHANNELS);
    sinwave = (ma_waveform*)device->pUserData;
    MA_ASSERT(sinwave != NULL);

    ma_waveform_read_pcm_frames(sinwave, output, framecount);
}

/* Init Audio (miniaudio) */
void audio_init(Platform* platform)
{
    // NOTE: Init audio context
    platform->context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS)
    {
        report_error("Miniaudio: Failed to initializing ");
        return;
    }

    platform->device_config  = ma_device_config_init(ma_Device_type_playback);
    device_config.playback.pDeviceID = NULL;
    device_config.playback.format   = MA_DEVICE_FORMAT;
    device_config.playback.channels = MA_DEVICE_CHANNELS;
    device_config.sampleRate        = MA_DEVICE_SAMPLE_RATE;
    device_config.dataCallback      = on_send_audio_data_to_device;
    device_config.pUserData         = NULL;  // NOTE: The device->pUserData in the data callback

    platform->device;
    if (ma_device_init(&context, &device_config, &device) != MA_SUCCESS)
    {
        report_error("Miniaudio: Failed to initialize the device");
        return;
    }

    // NOTE: Start playing the buffer
    ma_device_start(&device);
}
#endif