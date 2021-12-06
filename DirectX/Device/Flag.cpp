#include "Flag.h"

Flag::Flag(unsigned flags)
    : mFlags(flags)
{
}

Flag::~Flag() = default;

void Flag::set(unsigned value) {
    mFlags |= value;
}

void Flag::reset(unsigned value) {
    mFlags &= ~value;
}

bool Flag::check(unsigned value) const {
    return ((mFlags & value) != 0);
}

unsigned Flag::get() const {
    return mFlags;
}

void Flag::clear() {
    mFlags = 0;
}
