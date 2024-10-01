#include "PlattformUtils.hpp"
#include <iostream>
namespace VectorVertex
{


    std::string FileDialog::ExecuteZenity(const std::string &command)
    {
        std::array<char, 128> buffer;
        std::string result;

        // Open a pipe to the zenity command
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }

        // Read the output from the command
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }

        // Remove trailing newline
        if (!result.empty() && result.back() == '\n')
        {
            result.pop_back();
        }

        return result;
    }

    std::string FileDialog::BuildFilterString(const std::vector<std::string> &filters)
    {
        std::string filterString;
        for (const auto &filter : filters)
        {
            if (!filterString.empty())
            {
                filterString += " | ";
            }
            filterString += filter;
        }
        return filterString;
    }

    std::string FileDialog::OpenFile(const std::string &title, const std::vector<std::string> &filters, const std::string &defaultPath)
    {
        std::string command = "zenity --file-selection --title=\"" + title + "\"";
        if (!defaultPath.empty())
        {
            command += " --filename=\"" + defaultPath + "\"";
        }
        if (!filters.empty())
        {
            command += " --file-filter=\"" + filters[0] + "\""; // Add the first filter as the default
            for (int i = 1; i < filters.size(); ++i)
            {
                command += " --file-filter=\"" + filters[i] + "\""; // Add remaining filters
            }
        }
        return ExecuteZenity(command);
    }

    std::string FileDialog::SaveFile(const std::string &title, const std::vector<std::string> &filters, const std::string &defaultPath)
    {
        std::string command = "zenity --file-selection --save --title=\"" + title + "\"";
        if (!defaultPath.empty())
        {
            command += " --filename=\"" + defaultPath + "\"";
        }
        if (!filters.empty())
        {
            command += " --file-filter=\"" + filters[0] + "\""; // Add the first filter as the default
            for (int i = 1; i < filters.size(); ++i)
            {
                command += " --file-filter=\"" + filters[i] + "\""; // Add remaining filters
            }
        }
        return ExecuteZenity(command);
    }
}