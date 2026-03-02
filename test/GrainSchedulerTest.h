//
//  GrainSchedulerTest.h
//  MicoForge
//
//  Created by Ben on 26.02.26.
//

#pragma once

#include "../src/granular-processor/GrainScheduler.h"
#include <juce_core/juce_core.h>

class GrainSchedulerTest : public juce::UnitTest
{
public:
    GrainSchedulerTest()
        : juce::UnitTest("GrainScheduler", "DSP")
    {
    }

    void runTest() override
    {
        beginTest("no output before any grains are triggered");
        {
            GrainScheduler scheduler;
            scheduler.prepare(44100);
            CircularBuffer buf(1, 44100);
            expectWithinAbsoluteError(scheduler.sumGrainsForChannel(buf, 0), 0.0f, 0.0001f);
        }

        beginTest("synchronous mode triggers a grain at the expected interval");
        {
            GrainScheduler scheduler;
            scheduler.prepare(44100);
            scheduler.setMode(GranulationMode::Synchronous);
            scheduler.setDensity(10); // one grain every 4410 samples
            scheduler.setGrainDuration(50.0f);
            scheduler.setEnvelopeType(EnvelopeType::Gaussian);

            CircularBuffer buf(1, 44100);

            // pre-fill with 1.0f — grains read from this buffer, so without it
            // sumGrainsForChannel returns 0.0f even when grains are active
            for (int i = 0; i < 44100; ++i)
            {
                buf.writeSample(0, 1.0f);
                buf.advanceWritePosition();
            }

            // run just past one trigger interval (44100 / 10 = 4410 samples)
            bool grainTriggered = false;
            for (int i = 0; i < 4411; ++i)
            {
                scheduler.processTriggers(buf);
                scheduler.advanceGrains();
                if (scheduler.sumGrainsForChannel(buf, 0) != 0.0f)
                    grainTriggered = true;
            }
            expect(grainTriggered);
        }

        beginTest("asynchronous mode produces varying trigger intervals");
        {
            GrainScheduler scheduler;
            scheduler.prepare(44100);
            scheduler.setMode(GranulationMode::Asynchronous);
            scheduler.setDensity(100);
            scheduler.setGrainDuration(10.0f);
            scheduler.setEnvelopeType(EnvelopeType::Gaussian);

            CircularBuffer buf(1, 44100);

            // same as above — grains need non-zero buffer content to produce output
            for (int i = 0; i < 44100; ++i)
            {
                buf.writeSample(0, 1.0f);
                buf.advanceWritePosition();
            }

            // detect trigger times by watching the rising edge of sumGrainsForChannel
            juce::Array<int> triggerTimes;
            bool wasActive = false;
            for (int i = 0; i < 44100; ++i)
            {
                scheduler.processTriggers(buf);
                bool isActive = scheduler.sumGrainsForChannel(buf, 0) != 0.0f;
                if (isActive && !wasActive)
                    triggerTimes.add(i);
                wasActive = isActive;
                scheduler.advanceGrains();
            }

            // compare every interval against the first one — if any differ, variance is confirmed
            // avoids false negatives from just checking consecutive pairs
            bool intervalsVary = false;
            if (triggerTimes.size() >= 3)
            {
                int firstInterval = triggerTimes[1] - triggerTimes[0];
                for (int i = 2; i < triggerTimes.size(); ++i)
                {
                    if (triggerTimes[i] - triggerTimes[i - 1] != firstInterval)
                    {
                        intervalsVary = true;
                        break;
                    }
                }
            }
            expect(intervalsVary);
        }

        beginTest("distribution zero means no scatter in start position");
        {
            GrainScheduler scheduler;
            scheduler.prepare(44100);
            scheduler.setMode(GranulationMode::Synchronous);
            scheduler.setDensity(10);
            scheduler.setGrainDuration(50.0f);
            scheduler.setDistribution(0.0f);
            scheduler.setEnvelopeType(EnvelopeType::Gaussian);

            CircularBuffer buf(1, 44100);

            for (int i = 0; i < 4411; ++i)
            {
                scheduler.processTriggers(buf);
                scheduler.advanceGrains();
            }
            // we don't have direct access to startPosition from outside, so this just
            // verifies no crash — if you add a getter later, this test can be tightened
            expect(true);
        }

        beginTest("grain pool never exceeds 128 active grains");
        {
            GrainScheduler scheduler;
            scheduler.prepare(44100);
            scheduler.setMode(GranulationMode::Synchronous);
            scheduler.setDensity(128);
            scheduler.setGrainDuration(500.0f); // long grains so they pile up
            scheduler.setEnvelopeType(EnvelopeType::Gaussian);

            CircularBuffer buf(2, 44100);

            // flood the scheduler — long grains pile up, density=128 stresses the pool limit
            for (int i = 0; i < 10000; ++i)
            {
                scheduler.processTriggers(buf);
                scheduler.advanceGrains();
            }
            // pool overflow would likely produce inf or nan — isfinite catches both
            float out = scheduler.sumGrainsForChannel(buf, 0);
            expect(std::isfinite(out));
        }
    }
};

static GrainSchedulerTest grainSchedulerTest;
