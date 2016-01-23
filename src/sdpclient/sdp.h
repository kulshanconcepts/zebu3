// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "serial.h"

class SdpClient {
private:
    Serial& serial;

public:
    SdpClient(Serial& serial);
    ~SdpClient();
};
