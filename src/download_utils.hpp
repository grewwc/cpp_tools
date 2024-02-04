#pragma once

#include "curl/curl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <string>

namespace wwc {
    bool download_url(const char* url, std::string& chunk);
}