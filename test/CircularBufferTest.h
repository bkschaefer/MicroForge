//
//  CircularBufferTest.h
//  MicroForge
//
//  Created by Ben on 26.02.26.
//

#pragma once

#include "../src/CircularBuffer.h"
#include <juce_core/juce_core.h>

class CircularBufferTest : public juce::UnitTest
{
public:
    CircularBufferTest()
        : juce::UnitTest("CircularBuffer", "DSP")
    {
    }

    void runTest() override
    {
        beginTest("write and read back a sample");
        {
            CircularBuffer buf(1, 8);
            buf.writeSample(0, 0.5f);
            expectWithinAbsoluteError(buf.getSample(0, 0.0f), 0.5f, 0.0001f);
        }

        beginTest("interpolation between two samples");
        {
            CircularBuffer buf(1, 8);
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 1.0f);
            // read at 0.5 — halfway between index 0 (0.0) and index 1 (1.0)
            expectWithinAbsoluteError(buf.getSample(0, 0.5f), 0.5f, 0.0001f);
        }

        beginTest("write position wraps at buffer end");
        {
            CircularBuffer buf(1, 4);
            buf.advanceWritePosition(); // 1
            buf.advanceWritePosition(); // 2
            buf.advanceWritePosition(); // 3
            buf.advanceWritePosition(); // should wrap back to 0
            expectEquals(buf.getWritePosition(), 0);
        }

        beginTest("read position wraps when negative");
        {
            CircularBuffer buf(1, 8);
            // advance write position to index 7, write a known value there
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.0f);
            buf.advanceWritePosition();
            buf.writeSample(0, 0.75f); // index 7
            // -1 should wrap to index 7 (bufferSize - 1)
            expectWithinAbsoluteError(buf.getSample(0, -1.0f), 0.75f, 0.0001f);
        }

        beginTest("read position wraps when past buffer size");
        {
            CircularBuffer buf(1, 8);
            buf.writeSample(0, 0.9f); // index 0
            // position 8 should wrap back to index 0
            expectWithinAbsoluteError(buf.getSample(0, 8.0f), 0.9f, 0.0001f);
        }

        beginTest("channels don't interfere with each other");
        {
            CircularBuffer buf(2, 8);
            buf.writeSample(0, 0.1f);
            buf.writeSample(1, 0.9f);
            expectWithinAbsoluteError(buf.getSample(0, 0.0f), 0.1f, 0.0001f);
            expectWithinAbsoluteError(buf.getSample(1, 0.0f), 0.9f, 0.0001f);
        }

        beginTest("write position increments correctly");
        {
            CircularBuffer buf(1, 8);
            expectEquals(buf.getWritePosition(), 0);
            buf.advanceWritePosition();
            expectEquals(buf.getWritePosition(), 1);
            buf.advanceWritePosition();
            expectEquals(buf.getWritePosition(), 2);
        }

        beginTest("default constructor doesn't crash");
        {
            CircularBuffer buf;
            buf.writeSample(0, 1.0f);
            buf.advanceWritePosition();
            // uninitialized buffer should be a no-op, position stays 0
            expectEquals(buf.getWritePosition(), 0);
        }
    }
};

static CircularBufferTest circularBufferTest;
