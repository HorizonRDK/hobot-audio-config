#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

#define MAX_LINE_LENGTH 256

typedef struct
{
    char module[50];
    char device[50];
    int mmap;
    int tsched;
    int fragments;
    int fragment_size;
    int rate;
    int channels;
    int rewind_safeguard;
} AudioConfig;

void parseLine(char *line, AudioConfig *config)
{
    sscanf(line, "load-module %s device=%s mmap=%d tsched=%d fragments=%d fragment_size=%d rate=%d channels=%d rewind_safeguard=%d",
           config->module, config->device, &config->mmap, &config->tsched, &config->fragments, &config->fragment_size,
           &config->rate, &config->channels, &config->rewind_safeguard);
}

void openPlaybackDevice(AudioConfig *config, snd_pcm_t **playback_handle)
{
    int err;
    snd_pcm_hw_params_t *params;

    printf("Opening playback device: %s\n", config->device);

    err = snd_pcm_open(playback_handle, config->device, SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
    {
        fprintf(stderr, "Error opening playback device: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Allocate hardware parameters structure
    snd_pcm_hw_params_alloca(&params);

    // Fill it in with default values
    snd_pcm_hw_params_any(*playback_handle, params);

    // Set the desired hardware parameters
    snd_pcm_hw_params_set_access(*playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*playback_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*playback_handle, params, config->channels);
    snd_pcm_hw_params_set_rate_near(*playback_handle, params, &config->rate, 0);

    // Write the parameters to the driver
    err = snd_pcm_hw_params(*playback_handle, params);
    if (err < 0)
    {
        fprintf(stderr, "Error setting hardware parameters: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void playBlankAudio(snd_pcm_t *playback_handle)
{
    // Sample data: play a silent sound
    const size_t frames = 48000; // Adjust based on your requirements
    short buffer[frames * 2];    // 2 channels

    printf("Playing blank audio...\n");

    // // Write the silent sound to the PCM device
    // if (snd_pcm_writei(playback_handle, buffer, frames) < 0)
    // {
    //     fprintf(stderr, "Error playing audio\n");
    // }
}

void closePlaybackDevice(snd_pcm_t *playback_handle)
{
    printf("Closing playback device...\n");
    snd_pcm_close(playback_handle);
}

void openRecordingDevice(AudioConfig *config, snd_pcm_t **capture_handle)
{
    int err;
    snd_pcm_hw_params_t *params;

    printf("Opening recording device: %s\n", config->device);

    err = snd_pcm_open(capture_handle, config->device, SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0)
    {
        fprintf(stderr, "Error opening recording device: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Allocate hardware parameters structure
    snd_pcm_hw_params_alloca(&params);

    // Fill it in with default values
    snd_pcm_hw_params_any(*capture_handle, params);

    // Set the desired hardware parameters
    snd_pcm_hw_params_set_access(*capture_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*capture_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*capture_handle, params, config->channels);
    snd_pcm_hw_params_set_rate_near(*capture_handle, params, &config->rate, 0);

    // Write the parameters to the driver
    err = snd_pcm_hw_params(*capture_handle, params);
    if (err < 0)
    {
        fprintf(stderr, "Error setting hardware parameters: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void recordAudio(snd_pcm_t *capture_handle)
{
    // Sample data: record audio but don't save it
    const size_t frames = 1; // Adjust based on your requirements
    short buffer[frames];    // 2 channels

    printf("Recording audio (not saving)...\n");

    // Read audio from the PCM device
    // if (snd_pcm_readi(capture_handle, buffer, frames) < 0)
    // {
    //     fprintf(stderr, "Error recording audio\n");
    // }
}

void closeRecordingDevice(snd_pcm_t *capture_handle)
{
    printf("Closing recording device...\n");
    snd_pcm_close(capture_handle);
}

int main()
{
    FILE *file = fopen("/etc/pulse/default.pa", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    AudioConfig playbackConfig, recordingConfig;
    snd_pcm_t *playback_handle = NULL;
    snd_pcm_t *capture_handle = NULL;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#')
        {
            // Skip lines starting with #
            continue;
        }
        if (strstr(line, "module-alsa-sink") != NULL)
        {
            parseLine(line, &playbackConfig);
            openPlaybackDevice(&playbackConfig, &playback_handle);
            playBlankAudio(playback_handle);
            closePlaybackDevice(playback_handle);
        }
        if (strstr(line, "module-alsa-source") != NULL)
        {
            parseLine(line, &recordingConfig);
            openRecordingDevice(&recordingConfig, &capture_handle);
            recordAudio(capture_handle);
            closeRecordingDevice(capture_handle);
        }
    }

    fclose(file);

    return 0;
}
