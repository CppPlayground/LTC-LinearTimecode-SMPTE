# LTC LinearTimecode
A LinearTimecode library for [Arduino.cc](https://www.arduino.cc)

Use this library to decode linear timecodes. You can generate such timecodes for example on the [elteesee](https://elteesee.pehrhovey.net) website made by [pehrhovey](http://pehrhovey.net/blog/about/). More references under [this wikipedia](https://en.wikipedia.org/wiki/Linear_timecode) article.

### What is LTC?
"LTC" stands for "Linear Timecode" and is used to encode/decode timecode data in an audio signal. Basically you have an audio signal that runs synchronously with the music or the film and in which the timecode is stored as a bit sequence. You can store the time and date in a timecode, but I'll just cover the time for now.

### How does LTC work?
The time code consists of 8 bytes and a fixed sync word at the end. So 10 bytes in total. The fixed sync word at the end is used to tell the decoder that the sequence has ended.

- This is an example for one frame in the timecode:

![signwave](https://user-images.githubusercontent.com/62719703/187992274-4ab05553-c9b8-472d-beda-67c769e59c40.svg)

- Which byte does what?

Byte    | Bit         | Visuel        | Meaning                    
:---:   |:---:        |:---:          |:---                        
A       | `A0` - `A3` | `--:--:--.-X` | Frame number units (0–9)   
B       | `A0` - `A3` | `--:--:--.X-` | Frame number tens (0-2)    
C       | `A0` - `A3` | `--:--:-X.--` | Seconds number units (0–9) 
D       | `A0` - `A3` | `--:--:X-.--` | Seconds number tens (0–5)  
E       | `A0` - `A3` | `--:-X:--.--` | Minutes number units (0-9) 
F       | `A0` - `A3` | `--:X-:--.--` | Minutes number tens (0-5)  
G       | `A0` - `A3` | `-X:--:--.--` | Hours number units (0-9)   
H       | `A0` - `A3` | `X-:--:--.--` | Hours number tens (0-2)    
I       | `A0` - `A7` |               | Sync word                  
J       | `A0` - `A7` |               | Sync Word                  
