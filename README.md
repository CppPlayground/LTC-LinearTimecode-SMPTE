# LTC LinearTimecode
A LinearTimecode library for Arduino.cc

Use this library to decode linear timecodes. You can generate such timecodes for example on the [elteesee](https://elteesee.pehrhovey.net) website made by [pehrhovey](http://pehrhovey.net/blog/about/). More references under [this wikipedia](https://en.wikipedia.org/wiki/Linear_timecode) article.

### What is LTC?
"LTC" stands for "Linear Timecode" and is used to encode/decode timecode data in an audio signal. Basically you have an audio signal that runs synchronously with the music or the film and in which the timecode is stored as a bit sequence. You can store the time and date in a timecode, but I'll just cover the time for now.

### How does LTC work?
The time code consists of 8 bytes and a fixed sync word at the end. So 10 bytes in total. The fixed sync word at the end is used to tell the decoder that the sequence has ended.

This is an example for one frame in the timecode:

![signwave](https://user-images.githubusercontent.com/62719703/187992274-4ab05553-c9b8-472d-beda-67c769e59c40.svg)

Which byte does what?

Byte    | A | B | C | D | E | F | G | H | I | J
---     |---|---|---|---|---|---|---|---|---|---
Meaning |
