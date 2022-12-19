// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <string>

#include "Argon2/Constants.h"

namespace Config
{
    class Config
    {
      public:
        Config() {};

        Constants::OptimizationMethod optimizationMethod;
    };

    extern Config config;
}
