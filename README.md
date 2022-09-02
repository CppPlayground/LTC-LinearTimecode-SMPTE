# LTC LinearTimecode (SMPTE)
A LinearTimecode library for [Arduino.cc](https://www.arduino.cc)

Use this library to decode linear timecodes. You can generate such timecodes for example on the [elteesee](https://elteesee.pehrhovey.net) website made by [pehrhovey](http://pehrhovey.net/blog/about/). You can find more references in [this wikipedia](https://en.wikipedia.org/wiki/Linear_timecode) article.

> Skip to [How to use the library](#how-to-use-the-library)

----

### What is LTC?

"LTC" stands for "Linear Timecode" and is used to encode/decode timecode data in an audio signal. Basically you have an audio signal that runs synchronously with the music or the film and in which the timecode is stored as a bit sequence. You can store the time and date in a timecode, but I'll just cover the time for now.

----

### How does LTC work?

The time code consists of 8 bytes and a fixed sync word at the end. So 10 bytes in total. The fixed sync word at the end is used to tell the decoder that the sequence has ended.
> A long bit period corresponds to a 0 bit <br> A double short bit period corresponds to a 1 bit

####  Example for one frame

![signwave](https://user-images.githubusercontent.com/62719703/187992274-4ab05553-c9b8-472d-beda-67c769e59c40.svg)

#### Which byte does what

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

----

### How does the library decode the timecode?

#### Standards

First, there are different timecode standards, which differ in the number of frames or the length of a frame as you can see on the [elteesee](https://elteesee.pehrhovey.net) website.
- 23.976 F <br>
- 24 F <br>
- 25 F <br>
- 29.97ndf (no drop frame) <br>
- 30 F

#### Calculate one bit lenght

To calculate how long one bit lasts, we need to do a bit of math:

```ino
// convert framerate to ms
float framerate = 25.00;
float ms = 1000 / framerate;

// now calculate how long one bit is
// a frame consists of 80 bits
// this gives us back how long one bit is in ms
float oneBit = ms / 80;

// convert ms to µs and cast to int and we have the bit length in µs
const int bitLength = int(oneBit * 1000);
```

Now the library just waits for an interrupt. An interrupt occurs when the sine wave crosses 0. Then the library checks the time difference between the two interrupts. A short time between two interrupts means that a 1 bit has arrived, and a long time between two interrupts means that a 0 bit has arrived.

----

### How to use the library?

#### Syntax

###### constructor

```ino
LinearTimecode::LinearTimecode(LinearTimecode::frameTypes frameType);
```
`enum frameType` : { FRAME_23_976, FRAME_24, FRAME_25, NO_DROP_FRAME_29_97, FRAME_30 }

```ino
LinearTimecode::LinearTimecode(const unsigned int frameDuration);
```
`const unsigned int frameDuration` : duration

<br>

###### methodes

```ino
const unsigned int LinearTimecode::getBitLength();
```
```ino
uint8_t LinearTimecode::getByteBuffer(const unsigned int index);
```
```ino
float LinearTimecode::getFramerate();
```
```ino
int LinearTimecode::getFrames();
```
```ino
int LinearTimecode::getSeconds();
```
```ino
int LinearTimecode::getMinutes();
```
```ino
int LinearTimecode::getHours();
```
```ino
String getTimecode(LinearTimecode::formats format);
```
```ino
void LinearTimecode::onEdgeChange();
```
```ino
void LinearTimecode::onSync(void (*callback)());
```
```ino
void setShortEdgeDuration(const unsigned int min, const unsigned int max);
```
```ino
void setLongEdgeDuration(const unsigned int min, const unsigned int max);
```
```ino
void LinearTimecode::setSyncPattern(word pattern);
```

#### Example

```ino
// create a new LinearTimecode class instance (I'll call it "ltc") with a frame rate of 25 frames
LinearTimecode ltc(ltc.FRAME_25);
```








