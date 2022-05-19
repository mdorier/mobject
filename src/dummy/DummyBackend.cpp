/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "DummyBackend.hpp"
#include <iostream>

MOBJECT_REGISTER_BACKEND(dummy, DummySequencer);

void DummySequencer::sayHello() {
    std::cout << "Hello World" << std::endl;
}

mobject::RequestResult<int32_t> DummySequencer::computeSum(int32_t x, int32_t y) {
    mobject::RequestResult<int32_t> result;
    result.value() = x + y;
    return result;
}

mobject::RequestResult<bool> DummySequencer::destroy() {
    mobject::RequestResult<bool> result;
    result.value() = true;
    // or result.success() = true
    return result;
}

std::unique_ptr<mobject::Backend> DummySequencer::create(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<mobject::Backend>(new DummySequencer(config));
}

std::unique_ptr<mobject::Backend> DummySequencer::open(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<mobject::Backend>(new DummySequencer(config));
}
