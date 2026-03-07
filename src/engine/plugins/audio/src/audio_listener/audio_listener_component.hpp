#pragma once

class AudioListenerComponent final {
    // Marker component - entities with this component act as the audio listener
    // Position and orientation are taken from the entity's Transform component
public:
    AudioListenerComponent() = default;
};
