# Add the files we want to link against
set(config_source_files
    Config.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(Config ${config_source_files})
