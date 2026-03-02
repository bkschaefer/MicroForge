//
//  GrainEnvelopeTest.h
//  MicoForge
//
//  Created by Ben on 26.02.26.
//

#pragma once

#include "../src/granular-processor/GrainEnvelope.h"
#include <juce_core/juce_core.h>

class GrainEnvelopeTest : public juce::UnitTest
{
public:
    GrainEnvelopeTest()
        : juce::UnitTest("GrainEnvelope", "DSP")
    {
    }

    void runTest() override
    {
        beginTest("Gaussian peaks at center");
        {
            GaussianEnvelope env;
            expectWithinAbsoluteError(env.processEnvelope(0.5f), 1.0f, 0.01f);
        }

        beginTest("Gaussian fades out at edges");
        {
            GaussianEnvelope env;
            // grain boundary — both edges should be near zero, not exactly zero
            expect(env.processEnvelope(0.0f) < 0.01f);
            expect(env.processEnvelope(1.0f) < 0.01f);
        }

        beginTest("Gaussian stays in range 0 to 1");
        {
            GaussianEnvelope env;
            for (int i = 0; i <= 100; ++i)
            {
                float val = env.processEnvelope(i / 100.0f);
                expect(val >= 0.0f && val <= 1.0f);
            }
        }

        beginTest("Parabolic peaks at center");
        {
            ParabolicEnvelope env;
            // formula is 4 * t * (1 - t), so at t=0.5: 4 * 0.5 * 0.5 = 1.0 exactly
            expectWithinAbsoluteError(env.processEnvelope(0.5f), 1.0f, 0.0001f);
        }

        beginTest("Parabolic is zero at edges");
        {
            ParabolicEnvelope env;
            expectWithinAbsoluteError(env.processEnvelope(0.0f), 0.0f, 0.0001f);
            expectWithinAbsoluteError(env.processEnvelope(1.0f), 0.0f, 0.0001f);
        }

        beginTest("Parabolic stays in range 0 to 1");
        {
            ParabolicEnvelope env;
            for (int i = 0; i <= 100; ++i)
            {
                float val = env.processEnvelope(i / 100.0f);
                expect(val >= 0.0f && val <= 1.0f);
            }
        }
    }
};

static GrainEnvelopeTest grainEnvelopeTest;
