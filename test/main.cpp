//
//  main.cpp
//  MicoForge
//
//  Created by Ben on 20.02.26.
//

#include <juce_core/juce_core.h>

#include "CircularBufferTest.h"
#include "GrainEnvelopeTest.h"
#include "GrainTest.h"
#include "GrainSchedulerTest.h"

int main()
{
    juce::UnitTestRunner runner;
    runner.runAllTests();

    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        auto* result = runner.getResult(i);
        juce::Logger::outputDebugString(result->unitTestName + ": "
            + juce::String(result->failures) + " failures");
    }

    return 0;
}
