#pragma once

#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
public:
    // Call once at startup
    static void Init(const char* filename) {
        std::lock_guard<std::mutex> lock(GetMutex());
        if (GetFile()) fclose(GetFile());
        GetFile() = fopen(filename, "w");
    }

    // Call once at shutdown
    static void Shutdown() {
        std::lock_guard<std::mutex> lock(GetMutex());
        if (GetFile()) {
            fclose(GetFile());
            GetFile() = nullptr;
        }
    }

    static void Log(const char* fmt, ...) {
        std::lock_guard<std::mutex> lock(GetMutex());
        if (!GetFile()) return;

        // Timestamp (wall time, not monotonic)
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* ptm = std::localtime(&now_c);
        char timebuf[32];
        std::strftime(timebuf, 32, "%H:%M:%S", ptm);

        // Print timestamp
        fprintf(GetFile(), "[%s] ", timebuf);

        va_list args;
        va_start(args, fmt);
        vfprintf(GetFile(), fmt, args);
        va_end(args);

        fprintf(GetFile(), "\n");
        fflush(GetFile());
    }

private:
    static FILE*& GetFile() {
        static FILE* file = nullptr;
        return file;
    }

    static std::mutex& GetMutex() {
        static std::mutex mtx;
        return mtx;
    }
};