/*
  LinearTimecode.h - Library for decoding linear timecodes send over audio.
*/

#include "LinearTimecode.h"

// constructor (frameType)
LinearTimecode::LinearTimecode(frameTypes frameType) {

    // initialize framerate and frametype
    this -> _framerate = 0.0;
    
    switch (frameType) {
        
        // 23.976 FPS
        case FRAME_23_976:
        
            this -> _framerate = 23.976;
            break;
        
        // 24 FPS
        case FRAME_24:
        
            this -> _framerate = 24.0;
            break;
        
        // 25 FPS
        default:
        case FRAME_25:
        
            this -> _framerate = 25;
            break;
        
        // 29.97 No Drop Frame
        case NO_DROP_FRAME_29_97:
        
            this -> _framerate = 29.97;
            break;
        
        // 30 FPS
        case FRAME_30:
        
            this -> _framerate = 30.0;
            break;
    }
    
    // calculate the bitlength of one bit
    // 1000 / framerate = ms
    // ms / 80Bit = 0.µs per bit
    // 0.µs per bit * 1000 = µs.0 as int
    this -> _bitLength = int(((1000 / this -> _framerate) / 80) * 1000);
    
    // initialize
    this -> _init();
}

// constructor (frameDuration)
LinearTimecode::LinearTimecode(const unsigned int frameDuration) {
    
    // calculate the bitlength of one bit
    // frameDuration / 80Bit = 0.µs per bit
    // 0.µs per bit * 1000 = µs.0 as int
    this -> _bitLength = int((frameDuration / 80) * 1000);
    
    // initialize
    this -> _init();
}

// initialize
void LinearTimecode::_init() {

    // initialize frame pattern
    this -> _frameSyncPattern = 0b1011111111111100;
    
    // initialize timecode digits (H, H, m, m, s, s, f, f)
    this -> _digits[8] = {0};
    
    // initialize frame buffers
    this -> _frameBuffers[10] = {0b00000000};
    
    // initialize timing
    this -> _timeLast = 0;
    this -> _timeDifference = 0;
    
    // set if half edge is detected
    this -> _halfEdge = false;
    
    // short edge duration
    this -> _shortEdgeDurationMin = 0;
    this -> _shortEdgeDurationMax = 0;
    
    // long edge duration
    this -> _longEdgeDurationMin = 0;
    this -> _longEdgeDurationMax = 0;
    
    // initialize time
    this -> _timeHours = 0;
    this -> _timeMinutes = 0;
    this -> _timeSeconds = 0;
    this -> _timeFrames = 0;
    
    // calculate the bit length of one third bit
    const int thirdBitLength = int(this -> _bitLength / 3);
    
    // set short edge duration
    this -> _shortEdgeDurationMin = thirdBitLength * 1;
    this -> _shortEdgeDurationMax = thirdBitLength * 2;
    
    // set long edge duration (+ 1 so they don't overlap)
    this -> _longEdgeDurationMin = thirdBitLength * 2 + 1;
    this -> _longEdgeDurationMin = thirdBitLength * 3;
}

// shift each bit in frameBuffers one to the right
void LinearTimecode::_shiftFrameBuffersRight(volatile uint8_t frameBuffers[10]) {

    // loop through all buffers
    for (int bufferIndex = 9; bufferIndex > 0; bufferIndex--) {
    
        // shift current bit one to left
        frameBuffers[bufferIndex] = frameBuffers[bufferIndex] >> 1;
        
        // copy bit from previous bit
        bitWrite(frameBuffers[bufferIndex], 7, bitRead(frameBuffers[bufferIndex - 1], 0));
    }
    
    // shift bit in first buffer
    frameBuffers[0] = frameBuffers[0] >> 1;
}

// call on interrupt change
void LinearTimecode::onEdgeChange() {

    // calculate time difference
    this -> _timeDifference = micros() - this -> _timeLast;
    this -> _timeLast = micros();
    
    // 1 bit
    if ((this -> _timeDifference < this -> _shortEdgeDurationMax) && (this -> _timeDifference > this -> _shortEdgeDurationMin)) {
        
        // check for half edge
        if (this -> _halfEdge) {
        
            // shift frame buffer to right
            this -> _shiftFrameBuffersRight(this -> _frameBuffers);
            bitSet(this -> _frameBuffers[0], 7);
            
            // set half edge to false
            this -> _halfEdge = false;
            
        } else {
        
            // set half edge to true
            this -> _halfEdge = true;
        }
        
    // 0 bit
    } else if ((this -> _timeDifference < this -> _longEdgeDurationMax) && (this -> _timeDifference > this -> _longEdgeDurationMin)) {
    
        // shift frame buffer to right
        this -> _shiftFrameBuffersRight(this -> _frameBuffers);
    }
    
    // check for sync pattern
    if (word(this -> _frameBuffers[0], this -> _frameBuffers[1]) == this -> _frameSyncPattern) {
    
        // loop though all bytes
        for (int byteIndex = 0; byteIndex < 8; byteIndex++) {
    
          // convert binary to decimal
          this -> _digits[byteIndex] = int(this -> _frameBuffers[byteIndex + 2]);
        }
        
        // callback on sync
        this -> _onSync();
    }
}

// get the current byte buffer
uint8_t LinearTimecode::getByteBuffer(const unsigned int index) {
    return this -> _frameBuffers[index];
}

// get the framerate as float
float LinearTimecode::getFramerate() {
    return this -> _framerate;
};

// get the bit length as int in us
const int LinearTimecode::getBitLength() {
    return this -> _bitLength;
};

// get timecode as string with a format
String LinearTimecode::getTimecode(LinearTimecode::formats format) {
    // digita array
    volatile int *d = this -> _digits;
    
    switch (format) {
      
      // HH.mm.ss.ff
      case FORMAT_DOT:
        return (String) d[0] + d[1] + "." + d[2] + d[3] + "." + d[4] + d[5] + "." + d[6] + d[7];
      
      // HH:mm:ss:ff
      case FORMAT_COLON:
        return (String) d[0] + d[1] + ":" + d[2] + d[3] + ":" + d[4] + d[5] + ":" + d[6] + d[7];
      
      // HH:mm:ss.ff
      default:  
      case FORMAT_DOT_COLON:   
        return (String) d[0] + d[1] + ":" + d[2] + d[3] + ":" + d[4] + d[5] + "." + d[6] + d[7];
        
      // HH mm ss ff
      case FORMAT_SPACE:
        return (String) d[0] + d[1] + " " + d[2] + d[3] + " " + d[4] + d[5] + " " + d[6] + d[7];
        
    }
}

// get the frames
const int LinearTimecode::getFrames() {
    return this -> _timeFrames;
}

// get the seconds
const int LinearTimecode::getSeconds() {
    return this -> _timeSeconds;
}

// get the minutes
const int LinearTimecode::getMinutes() {
    return this -> _timeMinutes;
}

// get the hours
const int LinearTimecode::getHours() {
    return this -> _timeHours;
}

// set the sync pattern
void LinearTimecode::setSyncPattern(const word pattern) {
    this -> _frameSyncPattern = pattern;
}

// set short edge duration
void LinearTimecode::setShortEdgeDuration(const unsigned int min, const unsigned int max) {
    this -> _shortEdgeDurationMin = min;
    this -> _shortEdgeDurationMax = max;
}

// set long edge duration
void LinearTimecode::setLongEdgeDuration(const unsigned int min, const unsigned int max) {
    this -> _longEdgeDurationMin = min;
    this -> _longEdgeDurationMax = max;
}

// gets called on synced pattern
void LinearTimecode::onSync(void (*callback)(void)) {
    this -> _onSync = callback;
}


