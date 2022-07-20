#ifndef JACOBI_SPM_TIMER_HPP
#define JACOBI_SPM_TIMER_HPP

#include <chrono>
#include <iostream>
#include <type_traits>
#include <string_view>

namespace spm {
    template<typename TimeType = std::chrono::microseconds>
    class Timer {

        std::ostream& outFile;

        std::string_view message;
        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point stop;

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

            stop = std::chrono::system_clock::now();

            auto elapsed = stop - start;
            auto duration = std::chrono::duration_cast<TimeType>(elapsed).count();

            if (elapsedTime != nullptr) {
                (*elapsedTime) = duration;
            }


            printMessage(duration);
        }

        void printMessage(auto duration) {
            outFile << "[Timer] :: " << message << ", duration: " << std::to_string(duration) << " " << getTimeLabel() << "\n";
        }

    public:

        Timer(std::string_view m, std::ostream& out = std::cout) : message(m), outFile(out), elapsedTime(nullptr) {
            start = std::chrono::system_clock::now();
        }

        Timer(std::string_view m, long *us, std::ostream& out = std::cout) : message(m), outFile(out), elapsedTime(us) {
            start = std::chrono::system_clock::now();
        }

        ~Timer() {
            stopTimer();
        }

    };
}

#endif //JACOBI_SPM_TIMER_HPP
