#include <fstream>

#include <libreborn/util/util.h>

#include "config.h"

// Path
std::string ConfigFile::get_path() const {
    const std::string &dir = home_get() + "/cerberus";
    ensure_directory(dir.c_str());
    return dir + '/' + get_file();
}

// Load
void ConfigFile::load() {
    clear();
    // Read Data
    std::ifstream file(get_path(), std::ios::in);
    if (file) {
        do_load(file);
    }
    file.close();
    // Check If Successful
    if (!check_load()) {
        ERR("Unable To Load %s", get_name());
    }
    // Save Newly Loaded Data
    save();
}
bool ConfigFile::check_load() const {
    // Ignore Error
    return true;
}


// Save
void ConfigFile::save() const {
    // Check If The File Needs To Be Saved
    if (!can_save()) {
        return;
    }
    // Write Data
    std::ofstream file(get_path(), std::ios::out);
    if (file) {
        do_save(file);
    }
    file.close();
    // Check If Successful
    if (!file) {
        ERR("Unable To Save %s", get_name());
    }
}
void ConfigFile::do_save(std::ofstream &) const {
    IMPOSSIBLE();
}