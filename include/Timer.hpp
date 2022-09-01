#ifndef JACOBI_SPM_TIMER_HPP
#define JACOBI_SPM_TIMER_HPP

#include <chrono>
#include <iostream>
#include <type_traits>
#include <string_view>
#include <fmt/core.h>

namespace spm {

    /***
     * Class used to measure times of computation in a C++ block, using
     * the RAII pattern.
     * @tparam TimeType
     */
    template<typename TimeType = std::chrono::microseconds>
    class Timer {

        std::ostream& m_outFile;

        std::string_view m_message{};
        std::chrono::system_clock::time_point m_start{};
        std::chrono::system_clock::time_point m_stop{};

    private:

        long *elapsedTime = nullptr;

        std::string_view getTimeLabel() {

            using namespace std::string_view_literals;

            if constexpr(std::is_same<TimeType, std::chrono::microseconds>()) {
                return "microseconds"sv;
            } else if constexpr(std::is_same<TimeType, std::chrono::milliseconds>()) {
                return "milliseconds"sv;
            } else if constexpr(std::is_same<TimeType, std::chrono::seconds>()) {
                return "seconds"sv;
            } else if constexpr(std::is_same<TimeType, std::chrono::minutes>()) {
                return "minutes"sv;
            } else if constexpr(std::is_same<TimeType, std::chrono::hours>()) {
                return "hours"sv;
            }

            return "unknown"sv;
        }

        void stopTimer() {

            m_stop = std::chrono::system_clock::now();

            auto elapsed = m_stop - m_start;
            auto duration = std::chrono::duration_cast<TimeType>(elapsed).count();

            if (elapsedTime != nullptr) {
                (*elapsedTime) = duration;
            }

            printMessage(duration);
        }

        void printMessage(auto duration) {
            m_outFile << fmt::format("[info][Timer] :: {}, lasts {} {}\n", m_message, std::to_string(duration), getTimeLabel());
        }

    public:

        explicit Timer(std::string_view m, std::ostream& out = std::cout) : m_message(m), m_outFile(out), elapsedTime(nullptr) {
            m_start = std::chrono::system_clock::now();
        }

        Timer(std::string_view m, long *us, std::ostream& out = std::cout) : m_message(m), m_outFile(out), elapsedTime(us) {
            m_start = std::chrono::system_clock::now();
        }

        ~Timer() {
            stopTimer();
        }

    };
}

#endif //JACOBI_SPM_TIMER_HPP
