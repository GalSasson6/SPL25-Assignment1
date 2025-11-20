#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist) {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    
    //Todo: Implement buildLibrary method
    for (const auto& track_info : library_tracks) {
        if (track_info.type == "MP3") {
        
            bool has_tags = (track_info.extra_param2!=0);

            AudioTrack* track = new MP3Track(track_info.title,track_info.artists,track_info.duration_seconds,track_info.bpm,track_info.extra_param1,has_tags);
            std::cout << "MP3: MP3Track created: "<< track_info.title << " " << track_info.extra_param1 << " kbps" << std::endl; 
            library.push_back(track);    
        }

        if (track_info.type == "WAV") {
            
            AudioTrack* track = new WAVTrack(track_info.title,track_info.artists,track_info.duration_seconds,track_info.bpm,track_info.extra_param1,track_info.extra_param2);
            library.push_back(track);    
            std::cout << "WAV: WAVTrack created: "<< track_info.title << " " << track_info.extra_param1 << " hz/ " <<track_info.extra_param2<<"bit"<< std::endl; 


        }
    }



    std::cout << "[INFO] Track library built:"<< library_tracks.size() << "loaded.\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    return nullptr; // Placeholder
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    // For now, add a placeholder to fix the linker error
        std::cout << "[INFO]: Loading playlist:" << playlist_name << std::endl;

        playlist = Playlist(playlist_name);
        for (int index : track_indices) {
            int actual_index = index -1;
            if (actual_index<library.size() && actual_index>-1) {
                AudioTrack* og_track = library[actual_index];
                PointerWrapper<AudioTrack> track_clone = og_track -> clone();
                AudioTrack* rawptr = track_clone.release();

                if (!rawptr) {
                    std::cout << "[WARNING]: nullptr :" << index << std::endl;
                    continue;
                }

                rawptr ->load();
                rawptr ->analyze_beatgrid();
                playlist.add_track(rawptr);
                std::cout << "[INFO] Added " <<  og_track->get_title() <<" to Playlist"<< std::endl;



            }
            else {
            std::cout << "[WARNING]: Invalid track index:" << index << std::endl;

             }

        
        }
        std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << playlist.get_track_count() << " tracks)" << std::endl;}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    return std::vector<std::string>(); // Placeholder
}
