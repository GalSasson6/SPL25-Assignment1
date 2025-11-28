#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    if (cache.contains(track.get_title())){
        cache.get(track.get_title());
        std::cout << "[Cache HIT] Track found in cache: " << track.get_title() << std::endl;
        return 1;
    }
    std::cout << "[Cache MISS] Cloning track into cache: " << track.get_title() << std::endl;
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if (!cloned_track) { 
        std::cout << "[Error]: Track " << track.get_title() << " failed to clone" << std::endl;
        return -1;
    }
    cloned_track->load();
    cloned_track-> analyze_beatgrid();
    
    bool results = cache.put(std::move(cloned_track));
    std::cout << "[Cache INSERT] Added '" << track.get_title() << "' to cache." << std::endl;
    if (results) { 
        return -1;
    }
    return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    return cache.get(track_title);    
}
