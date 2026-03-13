#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Background : public Util::GameObject {
public:
    Background()
        : GameObject(
              std::make_shared<Util::Image>(RESOURCE_DIR "/Background/land1.jpg"),
              -10) {
        m_Transform.scale = {1.2f, 1.2f};
    }
};

#endif // BACKGROUND_HPP
