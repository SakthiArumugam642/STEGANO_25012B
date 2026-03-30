**Project Title**
Image Steganography using LSB (Least Significant Bit)

**Overview**
This project implements steganography, a technique used to hide secret data inside an image without visibly altering it.
The system uses the LSB (Least Significant Bit) method to embed a secret text message into a BMP image, creating a stego image. The hidden message can later be extracted using the decoding process.

**Features**
Encode secret message into an image
Decode hidden message from stego image
Uses lossless BMP format for accuracy
Minimal visual distortion
Simple command-line interface

**Working Principle**
Each pixel in an image is made of bytes
The least significant bit (LSB) of each byte is modified
These bits are used to store secret message data
**Since only the last bit is changed, the image looks visually unchanged.**

**Requirements**
GCC Compiler
BMP Image (uncompressed)

**Limitations**
Works only with BMP images
No encryption (only hiding data)
Limited by image size

**Future Improvements**
Add encryption (AES) before encoding
Support PNG/JPEG formats
GUI-based interface
Password protection

**Key Concepts Used**
File handling in C
Bit manipulation
LSB encoding
Data hiding techniques

**Applications**
Secure communication
Digital watermarking
Data hiding in multimedia
