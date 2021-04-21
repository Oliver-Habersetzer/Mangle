/*
  ==============================================================================

	Utils.h
	Created: 20 Apr 2021 2:39:15am
	Author:  WurstBroHD

  ==============================================================================
*/

#pragma once

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}