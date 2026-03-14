#ifndef SUN_COUNTER_HPP
#define SUN_COUNTER_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

class SunCounter : public Util::GameObject {
public:
    SunCounter() {
        auto text = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/Geo-Regular.ttf", 20,
            "50", Util::Color(0, 0, 0));
        SetDrawable(text);
        SetZIndex(80);
        m_Transform.translation = {-570.0f, 277.0f};
    }

    void SetAmount(int amount) {
        auto text = std::dynamic_pointer_cast<Util::Text>(m_Drawable);
        text->SetText(std::to_string(amount));
    }
};

#endif // SUN_COUNTER_HPP
