/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __DUMMY_BACKEND_HPP
#define __DUMMY_BACKEND_HPP

#include <mobject/Backend.hpp>

using json = nlohmann::json;

/**
 * Dummy implementation of an mobject Backend.
 */
class DummySequencer : public mobject::Backend {
   
    json m_config;

    public:

    /**
     * @brief Constructor.
     */
    DummySequencer(const json& config)
    : m_config(config) {}

    /**
     * @brief Move-constructor.
     */
    DummySequencer(DummySequencer&&) = default;

    /**
     * @brief Copy-constructor.
     */
    DummySequencer(const DummySequencer&) = default;

    /**
     * @brief Move-assignment operator.
     */
    DummySequencer& operator=(DummySequencer&&) = default;

    /**
     * @brief Copy-assignment operator.
     */
    DummySequencer& operator=(const DummySequencer&) = default;

    /**
     * @brief Destructor.
     */
    virtual ~DummySequencer() = default;

    /**
     * @brief Prints Hello World.
     */
    void sayHello() override;

    /**
     * @brief Compute the sum of two integers.
     *
     * @param x first integer
     * @param y second integer
     *
     * @return a RequestResult containing the result.
     */
    mobject::RequestResult<int32_t> computeSum(int32_t x, int32_t y) override;

    /**
     * @brief Destroys the underlying sequencer.
     *
     * @return a RequestResult<bool> instance indicating
     * whether the database was successfully destroyed.
     */
    mobject::RequestResult<bool> destroy() override;

    /**
     * @brief Static factory function used by the SequencerFactory to
     * create a DummySequencer.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the sequencer
     *
     * @return a unique_ptr to a sequencer
     */
    static std::unique_ptr<mobject::Backend> create(const thallium::engine& engine, const json& config);

    /**
     * @brief Static factory function used by the SequencerFactory to
     * open a DummySequencer.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the sequencer
     *
     * @return a unique_ptr to a sequencer
     */
    static std::unique_ptr<mobject::Backend> open(const thallium::engine& engine, const json& config);
};

#endif
