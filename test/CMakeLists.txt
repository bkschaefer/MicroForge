juce_add_console_app(MicroForgeTests)

target_sources(MicroForgeTests PRIVATE
    main.cpp
    ../src/CircularBuffer.cpp
    ../src/granular-processor/GrainEnvelope.cpp
    ../src/granular-processor/Grain.cpp
    ../src/granular-processor/GrainScheduler.cpp
)

target_compile_definitions(MicroForgeTests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_include_directories(MicroForgeTests PRIVATE ../src)

target_link_libraries(MicroForgeTests PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
