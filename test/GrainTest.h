//
//  GrainTest.h
//  MicoForge
//
//  Created by Ben on 26.02.26.
//

#pragma once

#include "../src/granular-processor/Grain.h"
#include <juce_core/juce_core.h>

class GrainTest : public juce::UnitTest
{
public:
    GrainTest()
        : juce::UnitTest("Grain", "DSP")
    {
    }

    void runTest() override
    {
        beginTest("inactive by default");
        {
            Grain grain;
            expect(!grain.isActive());
        }

        beginTest("activate with valid params sets active");
        {
            GaussianEnvelope env;
            GrainActivationParams params;
            params.startPosition = 0.0f;
            params.duration = 100.0f;
            params.playbackRate = 1.0f;
            params.amplitude = 1.0f;
            params.envelope = &env;

            Grain grain;
            grain.activate(params);
            expect(grain.isActive());
        }

        beginTest("activate with duration zero stays inactive");
        {
            GaussianEnvelope env;
            GrainActivationParams params;
            params.startPosition = 0.0f;
            params.duration = 0.0f;
            params.playbackRate = 1.0f;
            params.amplitude = 1.0f;
            params.envelope = &env;

            Grain grain;
            grain.activate(params);
            expect(!grain.isActive());
        }

        beginTest("activate with null envelope stays inactive");
        {
            GrainActivationParams params;
            params.startPosition = 0.0f;
            params.duration = 100.0f;
            params.playbackRate = 1.0f;
            params.amplitude = 1.0f;
            params.envelope = nullptr;

            Grain grain;
            grain.activate(params);
            expect(!grain.isActive());
        }

        beginTest("getSampleForChannel returns 0 when inactive");
        {
            CircularBuffer buf(1, 8);
            Grain grain;
            expectWithinAbsoluteError(grain.getSampleForChannel(buf, 0), 0.0f, 0.0001f);
        }

        beginTest("grain deactivates after duration is reached");
        {
            GaussianEnvelope env;
            GrainActivationParams params;
            params.startPosition = 0.0f;
            params.duration = 3.0f;
            params.playbackRate = 1.0f;
            params.amplitude = 1.0f;
            params.envelope = &env;

            Grain grain;
            grain.activate(params);
            grain.advance();
            grain.advance();
            grain.advance(); // third advance crosses duration=3, grain should deactivate
            expect(!grain.isActive());
        }

        beginTest("read position advances by playback rate each step");
        {
            GaussianEnvelope env;
            GrainActivationParams params;
            params.startPosition = 0.0f;
            params.duration = 100.0f;
            params.playbackRate = 1.0f;
            params.amplitude = 1.0f;
            params.envelope = &env;

            Grain grain;
            grain.activate(params);
            grain.advance();
            expectWithinAbsoluteError(grain.getCurrentReadPosition(), 1.0f, 0.0001f);
        }
    }
};

static GrainTest grainTest;
