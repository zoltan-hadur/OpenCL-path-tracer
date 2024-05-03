#pragma once

#include <memory>
#include <string>

#include "Component.h"

namespace OpenCL_PathTracer
{
    class Color;

    namespace GL_Stuff
    {
        class Font;

        class Text : public Component
        {
        private:
            std::shared_ptr<Font> _font;
            std::string _value;

        public:
            Text(std::shared_ptr<Font> font, std::string value, class Color color);

            std::string const& GetValue() const;
            void SetValue(std::string value);
        };
    }
}