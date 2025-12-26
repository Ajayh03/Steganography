# Description
The art and science of hiding information by embedding messages within other, seemingly harmless messages. Bits of unused data are replaced by bits of valuable information using LSB mechanism. Sender and receiver will have individual key / secret based on which they will be able to extract the actual data from the image. This project also gives basic level understanding of image processing methodologies.

# Steganography
This project demonstrates the implementation of image steganography using the C programming language. The goal of the project is to hide secret text data inside a BMP image file and later retrieve it without visibly altering the image.
The encoding process works by modifying the least significant bits (LSB) of the image data to store the secret message. The decoding process reverses this operation to extract the hidden information accurately.

# Purpose

This project was developed to strengthen understanding of low-level programming concepts and data representation in C, particularly how information can be securely stored and extracted from binary files.

# Features

-> Hides text data inside a BMP image

-> Retrieves the hidden data without loss

-> Preserves the original image appearance

-> Modular and well-structured C code

# Concepts Used

-> File handling in C

-> Bitwise operations

-> Structures and pointers

-> Modular programming


