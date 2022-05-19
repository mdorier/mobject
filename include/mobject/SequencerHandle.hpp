/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_SEQUENCER_HANDLE_HPP
#define __MOBJECT_SEQUENCER_HANDLE_HPP

#include <thallium.hpp>
#include <memory>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <mobject/Client.hpp>
#include <mobject/Exception.hpp>
#include <mobject/AsyncRequest.hpp>

namespace mobject {

namespace tl = thallium;

class Client;
class SequencerHandleImpl;

/**
 * @brief A SequencerHandle object is a handle for a remote sequencer
 * on a server. It enables invoking the sequencer's functionalities.
 */
class SequencerHandle {

    friend class Client;

    public:

    /**
     * @brief Constructor. The resulting SequencerHandle handle will be invalid.
     */
    SequencerHandle();

    /**
     * @brief Copy-constructor.
     */
    SequencerHandle(const SequencerHandle&);

    /**
     * @brief Move-constructor.
     */
    SequencerHandle(SequencerHandle&&);

    /**
     * @brief Copy-assignment operator.
     */
    SequencerHandle& operator=(const SequencerHandle&);

    /**
     * @brief Move-assignment operator.
     */
    SequencerHandle& operator=(SequencerHandle&&);

    /**
     * @brief Destructor.
     */
    ~SequencerHandle();

    /**
     * @brief Returns the client this database has been opened with.
     */
    Client client() const;


    /**
     * @brief Checks if the SequencerHandle instance is valid.
     */
    operator bool() const;

    /**
     * @brief Sends an RPC to the sequencer to make it print a hello message.
     */
    void sayHello() const;

    /**
     * @brief Requests the target sequencer to compute the sum of two numbers.
     * If result is null, it will be ignored. If req is not null, this call
     * will be non-blocking and the caller is responsible for waiting on
     * the request.
     *
     * @param[in] x first integer
     * @param[in] y second integer
     * @param[out] result result
     * @param[out] req request for a non-blocking operation
     */
    void computeSum(int32_t x, int32_t y,
                    int32_t* result = nullptr,
                    AsyncRequest* req = nullptr) const;

    private:

    /**
     * @brief Constructor is private. Use a Client object
     * to create a SequencerHandle instance.
     *
     * @param impl Pointer to implementation.
     */
    SequencerHandle(const std::shared_ptr<SequencerHandleImpl>& impl);

    std::shared_ptr<SequencerHandleImpl> self;
};

}

#endif
