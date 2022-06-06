# youtube-drive-cpp

A collection of tools for converting arbitrary files into images and videos and vice-versa. The main tools are written in C++, and the rest are bash scripts which invoke ImageMagick and ffmpeg.

### Dependencies
- a C++ compiler (for file-to-bmp and bmp-to-file tools)
- a shell such as Bash (for running the scripts and calling everything else)
- ImageMagick (for converting between lossless images formats)
- ffmpeg (to convert to videos and back)

### Getting Started
At the moment, the functionality is split across the main conversion executeables (file-to-bmp, bmp-to-file) and the scripts (split-file-to-bitmaps, images-to-video, video-to-images). 

To get a rough idea of how things work, change `INPUT_FILENAME` in `split-file-to-bitmaps` to whatever file you want to test. I'm using a 1.3MB NASA image so that it splits into more than one image, but not too many images. I recommend using something very similar in size for the time being. Then, run `test-split-to-video-and-back` to see the result!

This should work on any Windows/Mac/Linux system as long as you have the dependencies satisfied.

### Known Bugs
Here are some limits in functionality or issues that I'm sure need to be addressed
- Parsing files loads as much as it can into the buffer, but should ideally load chunks at a time.
- Scripts are mostly hardcoded, but should be changed so they accept input arguments instead. Overall, it just needs to be combined into a more succinct interface with ideally a single master script.
- With the current configuration, the compression from video to back is indeed barely lossy, meaning there is no hope for uploading to youtube and redownloading yet. More research needs to be done to see if there is a video encoding that is bit-for-bit reversible.

