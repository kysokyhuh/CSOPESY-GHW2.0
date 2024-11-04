// initializer.cpp
#include "initializer.h"

Initializer::Initializer() : initialized(false) {}

void Initializer::setInitialized() {
    initialized = true;
}

bool Initializer::isInitialized() const {
    return initialized;
}
