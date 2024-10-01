#pragma once

#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <vector>

namespace VectorVertex
{
    class FileDialog
    {
    public:
        static std::string OpenFile(const std::string &title = "Open File", const std::vector<std::string> &filters = {}, const std::string &defaultPath = "./");
        static std::string SaveFile(const std::string &title = "Save File", const std::vector<std::string> &filters = {}, const std::string &defaultPath = "./");

    private:
        static std::string ExecuteZenity(const std::string &command);
        static std::string BuildFilterString(const std::vector<std::string> &filters);
    };
}
#endif // FILE_DIALOG_H
