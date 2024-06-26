//
// Created by pc on 2023/8/17.
//

#include "FIleUtils.h"

#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <filesystem>

namespace FileUtils {

    std::vector<uint8_t> read_binary_file(const std::string& filename, const uint32_t count) {
        std::vector<uint8_t> data;

        std::ifstream file;

        file.open(filename, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        uint64_t read_count = count;
        if (count == 0) {
            file.seekg(0, std::ios::end);
            read_count = static_cast<uint64_t>(file.tellg());
            file.seekg(0, std::ios::beg);
        }

        data.resize(static_cast<size_t>(read_count));
        file.read(reinterpret_cast<char*>(data.data()), read_count);
        file.close();

        return data;
    }

    std::string getFileTimeStr(const std::string& path, const std::string& format) {
        const auto fileTime   = std::filesystem::last_write_time(path);
        const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
        const auto time       = std::chrono::system_clock::to_time_t(systemTime);

        struct tm stime;
        localtime_s(&stime, &time);

        char tmp[32] = {NULL};
        strftime(tmp, sizeof(tmp), format.c_str(), &stime);

        return tmp;
    }

    std::vector<uint8_t> readShaderBinary(const std::string filename) {
        return read_binary_file(filename, 0);
    }

    std::string getFileExt(const std::string& filepath) {
        return filepath.substr(filepath.find('.') + 1);
    }

    std::string getResourcePath() {
#ifdef VK_RESOURCES_DIR
        return VK_RESOURCES_DIR;
#else
        return "./../resources";
#endif
    }

    std::string getResourcePath(const std::string& path) {
        return getResourcePath() + path;
    }

    std::string getShaderPath(const std::string& path) {
        return getShaderPath() + path;
    }

    std::string getShaderPath() {
#ifdef VK_SHADERS_DIR
        return VK_SHADERS_DIR;
#else
        return "./../shaders";
#endif
    }

}// namespace FileUtils
