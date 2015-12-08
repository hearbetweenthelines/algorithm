# Hear Between the Lines - Algorithms
C programs that provide the backend support of this project.

## Original MP3 Encoder Used
* [Shine](https://github.com/toots/shine)

## Libraries
* `libsndfile` C library for reading and writing sound files containing sampled audio data.
* `zlib` A massively spiffy yet delicately unobtrusive compression library.
* `tiny-AES128-C` A small and portable C implementation of the AES128 ECB and CBC encryption algorithms

## Tools
* [ffmpeg](https://github.com/FFmpeg/FFmpeg) is a command line toolbox to
  manipulate, convert and stream multimedia content.

## Sample Usage
Encoding using wavstego `./wavstego -e <pin> -m <message_file> -a <input_audio> -o <output_file>`

Decoding using wavstego `./wavstego -d <pin> -a <input_audio>` 

Encoding using mp3stego `./mp3stego -b <bitrate> -e <message_file> -p <pin> <input_audio> <output_file>`

Decoding using mp3stego `./mp3stego -p <pin> <input_audio>` 
