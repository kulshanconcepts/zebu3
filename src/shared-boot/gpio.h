// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

enum {
	// The GPIO registers base address.
	GPIO_BASE = 0x00200000,

	// The offsets for reach register.

	// Controls actuation of pull up/down to ALL GPIO pins.
	GPPUD = (GPIO_BASE + 0x94),

	// Controls actuation of pull up/down for specific GPIO pin.
	GPPUDCLK0 = (GPIO_BASE + 0x98),
};
