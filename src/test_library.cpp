#include <iostream>
#include <vector>
#include <cassert>
#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"

// ANSI Color codes for pretty printing
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

void runTest() {
    std::cout << "=== STARTING DJLIBRARYSERVICE TEST ===\n" << std::endl;

    // 1. SETUP DUMMY DATA
    // We mimic what the SessionFileParser would provide
    std::vector<SessionConfig::TrackInfo> dummy_config;

    // Track 1 (Index 1): MP3
    SessionConfig::TrackInfo t1;
    t1.type = "MP3";
    t1.title = "Song A";
    t1.artists = {"Artist A"};
    t1.duration_seconds = 120;
    t1.bpm = 128;
    t1.extra_param1 = 320; // Bitrate
    t1.extra_param2 = 1;   // Has tags (1 = true)
    dummy_config.push_back(t1);

    // Track 2 (Index 2): WAV
    SessionConfig::TrackInfo t2;
    t2.type = "WAV";
    t2.title = "Song B";
    t2.artists = {"Artist B"};
    t2.duration_seconds = 200;
    t2.bpm = 140;
    t2.extra_param1 = 44100; // Sample Rate
    t2.extra_param2 = 16;    // Bit Depth
    dummy_config.push_back(t2);

    // 2. INSTANTIATE SERVICE
    DJLibraryService service;

    // 3. TEST buildLibrary
    std::cout << "[TEST] Running buildLibrary()..." << std::endl;
    service.buildLibrary(dummy_config);
    
    // We can't easily check private 'library' size directly, 
    // but we can verify it works via the next steps.
    std::cout << GREEN << " [PASS] buildLibrary executed without crash." << RESET << "\n" << std::endl;


    // 4. TEST loadPlaylistFromIndices
    // Scenario: 
    // Index 1 -> Valid (Song A)
    // Index 50 -> Invalid (Should be skipped)
    // Index 2 -> Valid (Song B)
    // Index 0 -> Invalid (Too low, should be skipped)
    
    std::vector<int> indices = {1, 50, 2, 0}; 
    std::string playlistName = "Test Mix";

    std::cout << "[TEST] Running loadPlaylistFromIndices(1, 50, 2, 0)..." << std::endl;
    service.loadPlaylistFromIndices(playlistName, indices);

    // Verify Playlist Size (Should be 2, because 50 and 0 are invalid)
    int count = service.getPlaylist().get_track_count();
    if (count == 2) {
        std::cout << GREEN << " [PASS] Playlist count is 2 (Invalid indices skipped)." << RESET << std::endl;
    } else {
        std::cout << RED << " [FAIL] Playlist count is " << count << ". Expected 2." << RESET << std::endl;
    }


    // 5. TEST getTrackTitles
    std::cout << "\n[TEST] Running getTrackTitles()..." << std::endl;
    std::vector<std::string> titles = service.getTrackTitles();
    
    bool hasSongA = false, hasSongB = false;
    for (const auto& t : titles) {
        if (t == "Song A") hasSongA = true;
        if (t == "Song B") hasSongB = true;
    }

    if (hasSongA && hasSongB) {
        std::cout << GREEN << " [PASS] All titles found in playlist." << RESET << std::endl;
    } else {
        std::cout << RED << " [FAIL] Missing titles. Got: ";
        for(const auto& t : titles) std::cout << t << " ";
        std::cout << RESET << std::endl;
    }


    // 6. TEST findTrack
    std::cout << "\n[TEST] Running findTrack()..." << std::endl;
    AudioTrack* found = service.findTrack("Song A");
    if (found && found->get_title() == "Song A") {
        std::cout << GREEN << " [PASS] Found 'Song A' correctly." << RESET << std::endl;
    } else {
        std::cout << RED << " [FAIL] Could not find 'Song A'." << RESET << std::endl;
    }

    AudioTrack* notFound = service.findTrack("Ghost Song");
    if (notFound == nullptr) {
        std::cout << GREEN << " [PASS] Correctly returned nullptr for missing track." << RESET << std::endl;
    } else {
        std::cout << RED << " [FAIL] Returned non-null for missing track." << RESET << std::endl;
    }
    
    std::cout << "\n=== TEST COMPLETE (Check valgrind for memory leaks!) ===" << std::endl;
}

int main() {
    runTest();
    return 0;
}