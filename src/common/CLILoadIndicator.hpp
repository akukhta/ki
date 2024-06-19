#pragma once
#include <string>
#include <fmt/color.h>
#include <iostream>

namespace ki {
    namespace UI {
        class CLILoadIndicator {
        public:
            explicit CLILoadIndicator(std::string label, size_t maxValue = 100, size_t currentValue = 0,
                                      size_t barWidth = 20)
                    : maxValue(maxValue), currentValue(currentValue), barWidth(barWidth), label(std::move(label)) {
                step = 100 / barWidth;
            }

            ~CLILoadIndicator() {
                std::cout << "\n";
                std::cout.flush();
            }

            void setValue(int value) {
                currentValue = value;
            }

            void draw() {
                std::cout << "\r" << label << "\t[";
                int percent = std::min(
                        static_cast<int>(((static_cast<float>(currentValue) / static_cast<float>(maxValue)) * 100)),
                        100);
                size_t filled = percent / step;

                fmt::color barColor;

                if (percent <= 33) {
                    barColor = fmt::color::red;
                } else if (percent <= 66) {
                    barColor = fmt::color::yellow;
                } else {
                    barColor = fmt::color::green;
                }

                for (size_t i = 0; i < filled; i++) {
                    std::cout << fmt::format(fmt::fg(barColor), "*");
                }

                for (size_t i = 0; i < barWidth - filled; i++) {
                    std::cout << "-";
                }

                std::cout << fmt::format("]\t{}/{}", std::min(currentValue, maxValue), maxValue);

                std::cout.flush();
            }

        private:
            size_t maxValue = 100;
            size_t currentValue = 0;
            size_t barWidth = 20;
            size_t step = 5;
            std::string label;
        };
    }
}