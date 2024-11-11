#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>  // For getenv and _dupenv_s
#include "root_directory.h"  // Includes the definition of logl_root

class FileSystem {
private:
    // Typedef for a function pointer that returns a std::string and takes a const std::string& as an argument
    typedef std::string(*Builder)(const std::string& path);

public:
    // Returns a path constructed using the pathBuilder function
    static std::string getPath(const std::string& path) {
        static std::string(*pathBuilder)(const std::string&) = getPathBuilder();
        return (*pathBuilder)(path);
    }

private:
    // Returns the root directory, either from an environment variable or the `logl_root` macro
    static const std::string& getRoot() {
        static std::string root = "";  // Static root string to store the root path
        static bool initialized = false;  // Flag to ensure initialization happens only once

        if (!initialized) {
            char* envRoot = nullptr;
            // Get the environment variable LOGL_ROOT_PATH securely using _dupenv_s
            if (_dupenv_s(&envRoot, nullptr, "LOGL_ROOT_PATH") == 0 && envRoot != nullptr) {
                root = envRoot;  // If environment variable is found, use it
                free(envRoot);    // Don't forget to free the memory allocated by _dupenv_s
            }
            else {
                root = logl_root;  // Fallback to default logl_root if environment variable is not set
            }
            initialized = true;  // Mark initialization as complete
        }

        return root;
    }

    // Chooses the appropriate path builder function based on the presence of a root path
    static Builder getPathBuilder() {
        if (getRoot() != "")
            return &FileSystem::getPathRelativeRoot;
        else
            return &FileSystem::getPathRelativeBinary;
    }

    // Constructs a path relative to the root directory
    static std::string getPathRelativeRoot(const std::string& path) {
        return getRoot() + "/" + path;
    }

    // Constructs a path relative to the binary's directory
    static std::string getPathRelativeBinary(const std::string& path) {
        return "../../../" + path;
    }
};

#endif  // FILESYSTEM_H
