#pragma once
#include <opencv2/opencv.hpp>

// The Fibonacci recurrence relation describes a sequence F
// where F(n) is { n = 0, a
//               { n = 1, b
//               { n > 1, F(n-2) + F(n-1)
// for some initial integral values a and b.
// If the sequence is initialized F(0) = 1, F(1) = 1,
// then this relation produces the well-known Fibonacci
// sequence: 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.
extern "C" void CaptureLibinit();

// Produce the next value in the sequence.
// Returns true on success and updates current value and index;
// false on overflow, leaves current value and index unchanged.
//extern "C"  bool getFrame(int id);

extern "C" bool setFrame(cv::Mat frame);

// Get the current value in the sequence.
/*extern "C" CAPTURELIB_API unsigned long long fibonacci_current();

// Get the position of the current value in the sequence.
extern "C" CAPTURELIB_API unsigned fibonacci_index();*/