/**
 * @file test_phase4_part1.cpp
 * @brief Unit Tester for Assignment 7.3.1: LRUCache and DJControllerService
 * * Tests cover:
 * 1. LRU Cache Core Logic (Insertion, Eviction, Access updates)
 * 2. DJControllerService Logic (Load flows, Hit/Miss/Evict returns)
 * 3. Memory Management (via MockTrack tracking)
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "LRUCache.h"
#include "DJControllerService.h"
#include "AudioTrack.h"
#include "PointerWrapper.h"

// ==========================================
// 1. MOCK TRACK IMPLEMENTATION
// ==========================================
// We need a concrete class to test AudioTrack polymorphism
class MockTrack : public AudioTrack {
public:
    static int instance_count;
    
    MockTrack(std::string t) 
        : AudioTrack(t, {"Artist"}, 180, 120, 100) {
        instance_count++;
    }

    // Copy constructor
    MockTrack(const MockTrack& other) : AudioTrack(other) {
        instance_count++;
    }

    virtual ~MockTrack() {
        instance_count--;
    }

    void load() override {
        std::cout << "   [Mock] Loading " << title << std::endl;
    }

    void analyze_beatgrid() override {
        std::cout << "   [Mock] Analyzing " << title << std::endl;
    }

    double get_quality_score() const override { return 100.0; }

    // IMPORTANT: Phase 4 requires polymorphic cloning
    PointerWrapper<AudioTrack> clone() const override {
        return PointerWrapper<AudioTrack>(new MockTrack(*this));
    }
};

int MockTrack::instance_count = 0;

// ==========================================
// 2. TEST UTILITIES
// ==========================================
void print_header(const std::string& msg) {
    std::cout << "\n========================================\n";
    std::cout << "  " << msg << "\n";
    std::cout << "========================================\n";
}

void assert_test(bool condition, const std::string& name) {
    if (condition) {
        std::cout << " [PASS] " << name << std::endl;
    } else {
        std::cout << " [FAIL] " << name << std::endl;
        exit(1); // Fail fast
    }
}

// ==========================================
// 3. LRU CACHE TESTS (Section 7.3.1)
// ==========================================
void test_lru_logic() {
    print_header("TESTING LRU CACHE CORE LOGIC");

    // Scenario from PDF Page 21:
    // "Empty cache (size 2): put(A) -> no eviction"
    LRUCache cache(2);
    
    std::cout << "--- Step 1: Insert A into empty cache ---\n";
    MockTrack* tA = new MockTrack("Track A");
    bool evicted = cache.put(PointerWrapper<AudioTrack>(tA));
    assert_test(!evicted, "Put(A) should return false (no eviction)");
    assert_test(cache.size() == 1, "Cache size should be 1");

    // "Cache [A]: put(B) -> no eviction"
    std::cout << "--- Step 2: Insert B ---\n";
    MockTrack* tB = new MockTrack("Track B");
    evicted = cache.put(PointerWrapper<AudioTrack>(tB));
    assert_test(!evicted, "Put(B) should return false (no eviction)");
    assert_test(cache.size() == 2, "Cache size should be 2 (Full)");

    // Access A to make it MRU (Most Recently Used)
    // Cache State before: [A (older), B (newer)]
    // Access A -> State: [B (older), A (newer)]
    std::cout << "--- Step 3: Access A (Make MRU) ---\n";
    assert_test(cache.get("Track A") != nullptr, "Get(A) should return pointer");
    
    // "Cache [A,B]: put(C) -> evicts LRU"
    // Since A was just accessed, B is now LRU. C should replace B.
    std::cout << "--- Step 4: Insert C (Trigger Eviction) ---\n";
    MockTrack* tC = new MockTrack("Track C");
    evicted = cache.put(PointerWrapper<AudioTrack>(tC));
    
    assert_test(evicted, "Put(C) should return true (eviction occurred)");
    assert_test(cache.contains("Track A"), "Track A should still be in cache (was MRU)");
    assert_test(cache.contains("Track C"), "Track C should be in cache (just inserted)");
    assert_test(!cache.contains("Track B"), "Track B should be evicted (was LRU)");

    // "Cache [A,C]: put(A) -> updates A's access time, returns false"
    std::cout << "--- Step 5: Re-insert Existing A ---\n";
    // Note: put() usually consumes ownership. 
    // We need to be careful not to double-free or leak if we pass a new pointer to an existing key.
    // The spec says "Check if track ... exists ... update access time ... return false".
    MockTrack* tA_duplicate = new MockTrack("Track A");
    evicted = cache.put(PointerWrapper<AudioTrack>(tA_duplicate));
    assert_test(!evicted, "Put(A) again should return false");
    // Note: In a real implementation, tA_duplicate might be destroyed inside put if not used, 
    // or the user is responsible. For this test, we verify the logic return value.
}

// ==========================================
// 4. CONTROLLER SERVICE TESTS
// ==========================================
void test_controller_service() {
    print_header("TESTING DJ CONTROLLER SERVICE");

    DJControllerService controller(2); // Capacity 2
    MockTrack sourceA("Track A");
    MockTrack sourceB("Track B");
    MockTrack sourceC("Track C");

    // 1. Load A -> MISS (0)
    std::cout << "--- Load 'Track A' (Empty Cache) ---\n";
    int result = controller.loadTrackToCache(sourceA);
    assert_test(result == 0, "First load should return 0 (MISS, no eviction)");
    
    // 2. Load A again -> HIT (1)
    std::cout << "--- Load 'Track A' (Again) ---\n";
    result = controller.loadTrackToCache(sourceA);
    assert_test(result == 1, "Second load should return 1 (HIT)");

    // 3. Load B -> MISS (0)
    std::cout << "--- Load 'Track B' ---\n";
    result = controller.loadTrackToCache(sourceB);
    assert_test(result == 0, "Load B should return 0 (MISS, no eviction)");

    // Current Cache: [A, B] (Full). A was accessed last (HIT), so B is older? 
    // Wait, step 2 accessed A. So A is MRU. B is older?
    // Actually step 3 inserted B. Inserting usually makes it MRU.
    // Let's verify LRU order by accessing A again explicitly to be sure.
    controller.getTrackFromCache("Track A"); // A is now MRU. B is LRU.

    // 4. Load C -> MISS with Eviction (-1)
    // Should evict B.
    std::cout << "--- Load 'Track C' (Full Cache, Evict B) ---\n";
    result = controller.loadTrackToCache(sourceC);
    assert_test(result == -1, "Load C should return -1 (MISS with EVICTION)");
    
    assert_test(controller.getTrackFromCache("Track A") != nullptr, "A should remain");
    assert_test(controller.getTrackFromCache("Track B") == nullptr, "B should be gone");
    assert_test(controller.getTrackFromCache("Track C") != nullptr, "C should be present");
}

int main() {
    std::cout << "Starting Phase 4 (Part 1) Tester...\n";
    
    try {
        test_lru_logic();
        test_controller_service();
        
        std::cout << "\n------------------------------------------------\n";
        std::cout << "FINAL RESULT: All Section 7.3.1 Tests Passed!\n";
        std::cout << "------------------------------------------------\n";
        
        // Optional: Check for memory leaks using the instance counter
        // Note: This is rough; actual leak checking requires Valgrind/ASAN
        if (MockTrack::instance_count > 0) {
            std::cout << "[WARNING] MockTrack instance count is " << MockTrack::instance_count 
                      << ". Ensure your destructors and PointerWrappers are working.\n";
        } else {
             std::cout << "[INFO] MockTrack instance count is 0. Clean memory likely.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    
    return 0;
}