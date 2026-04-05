#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Background : public Util::GameObject {
public:
    explicit Background(int lanes = 5)
        : GameObject(
              std::make_shared<Util::Image>(GetImagePath(lanes)),
              -10) {
        m_Transform.scale = {1.2f, 1.2f};
    }

private:
    static std::string GetImagePath(int lanes) {
        return std::string(RESOURCE_DIR "/Background/land")
               + std::to_string(lanes) + ".jpg";
    }
};

#endif // BACKGROUND_HPP
