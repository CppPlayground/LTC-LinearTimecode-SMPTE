# LTC LinearTimecode (SMPTE)
A LinearTimecode decode library for [Arduino.cc](https://www.arduino.cc)

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

> This example shows the timecode `00:00:00.00`

![signwave](https://github.com/Mqxx/LTC-LinearTimecode-SMPTE/blob/main/assets/signwave.svg "Signwave")

> [Raw image](https://raw.githubusercontent.com/Mqxx/LTC-LinearTimecode-SMPTE/main/assets/signwave.svg)

#### Which byte does what?

Byte    | Bit       | Visuel        | Meaning                    
:---:   |:---:      |:---:          |:---                        
A       | `0` - `3` | `--:--:--.-X` | Frame number units (0–9)   
B       | `0` - `3` | `--:--:--.X-` | Frame number tens (0-2)    
C       | `0` - `3` | `--:--:-X.--` | Seconds number units (0–9) 
D       | `0` - `3` | `--:--:X-.--` | Seconds number tens (0–5)  
E       | `0` - `3` | `--:-X:--.--` | Minutes number units (0-9) 
F       | `0` - `3` | `--:X-:--.--` | Minutes number tens (0-5)  
G       | `0` - `3` | `-X:--:--.--` | Hours number units (0-9)   
H       | `0` - `3` | `X-:--:--.--` | Hours number tens (0-2)    
I       | `0` - `7` |               | Sync word                  
J       | `0` - `7` |               | Sync Word                  

> Binary decimal numbers [wiki](https://en.wikipedia.org/wiki/Binary_number)

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

To calculate how long one bit lasts, we need to do a bit of math. Got it? A *bit* of math? Nahh lets go on 😅:

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
`enum frameType` : `{ FRAME_23_976, FRAME_24, FRAME_25, NO_DROP_FRAME_29_97, FRAME_30 }`

<br>

```ino
LinearTimecode::LinearTimecode(const unsigned int frameDuration);
```
`const unsigned int frameDuration` : `duration`

<br>

###### methodes

```ino
const int LinearTimecode::getBitLength();
```
> Returns the bit length of one bit.

<br>

```ino
uint8_t LinearTimecode::getByteBuffer(const unsigned int index);
```
`const unsigned int index` : `index`
> Returns the byte buffer at the specified index.

<br>

```ino
float LinearTimecode::getFramerate();
```
> Returns the set frame rate as a floating point number

<br>

```ino
const int LinearTimecode::getFrames();
```
> Returns the current frame

<br>

```ino
const int LinearTimecode::getSeconds();
```
> Returns the current second

<br>

```ino
const int LinearTimecode::getMinutes();
```
> Returns the current minute

<br>

```ino
const int LinearTimecode::getHours();
```
> Returns the current hour

<br>

```ino
String LinearTimecode::getTimecode(LinearTimecode::formats format);
```
`enum format` : `{ FORMAT_DOT, FORMAT_COLON, FORMAT_DOT_COLON, FORMAT_SPACE }`
> Returns the timecode in a string format<br>
> Format              | Visual
> :---                |:---:
> `FORMAT_DOT`        | `HH.MM.SS.FF`
> `FORMAT_COLON`      | `HH:MM:SS:FF`
> `FORMAT_DOT_COLON`  | `HH:MM:SS.FF`
> `FORMAT_SPACE`      | `HH MM SS FF`

<br>

```ino
void LinearTimecode::onEdgeChange();
```
> Needs to be executed on an interrupt change on the audio pin

<br>

```ino
void LinearTimecode::onSync(void (*callback)());
```
`void (*callback)()` : `function to execute`
> Gets called when the sync pattern is detected

<br>

```ino
void LinearTimecode::setShortEdgeDuration(const unsigned int min, const unsigned int max);
```
`const unsigned int min` : `the minimum short edge duration`<br>
`const unsigned int max` : `the maximum short edge duration`
> Set the minimum and maximum duration of the short edge

<br>

```ino
void LinearTimecode::setLongEdgeDuration(const unsigned int min, const unsigned int max);
```
`const unsigned int min` : `the minimum long edge duration`<br>
`const unsigned int max` : `the maximum long edge duration`
> Set the minimum and maximum duration of the long edge

<br>

```ino
void LinearTimecode::setSyncPattern(word pattern);
```
`word pattern` : `sync pattern`
> Set the sync pattern

<br>

#### Example

###### code

```ino
// include
// include the libary
#include <LinearTimecode.h>

// define
// the audio in pin needs to be an change intrrupt pin!
#define AUDIO_IN 2

// Create a new LinearTimecode class instance (I'll call it "ltc") with a frame rate of 25 frames
LinearTimecode ltc(ltc.FRAME_30);

// the setup function
void setup() {
  // begin serial
  Serial.begin(115200);

  // attach the interrupt pin to pin 2
  attachInterrupt(2, [](){
    
    // call the onEdgeChange() method of your class instance
    // this needs to be in a lambda function!
    ltc.onEdgeChange();
  }, CHANGE);
  
  // call the onSync() method of your class instance
  ltc.onSync([](){
    
    // print the result in the console
    Serial.println(ltc.getTimecode(ltc.FORMAT_DOT_COLON));
  });
}

void loop() {

}

```

###### output

```
10:03:12.13
```
> Example timecode





