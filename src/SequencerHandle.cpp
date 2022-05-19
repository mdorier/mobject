/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "mobject/SequencerHandle.hpp"
#include "mobject/RequestResult.hpp"
#include "mobject/Exception.hpp"

#include "AsyncRequestImpl.hpp"
#include "ClientImpl.hpp"
#include "SequencerHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/pair.hpp>

namespace mobject {

SequencerHandle::SequencerHandle() = default;

SequencerHandle::SequencerHandle(const std::shared_ptr<SequencerHandleImpl>& impl)
: self(impl) {}

SequencerHandle::SequencerHandle(const SequencerHandle&) = default;

SequencerHandle::SequencerHandle(SequencerHandle&&) = default;

SequencerHandle& SequencerHandle::operator=(const SequencerHandle&) = default;

SequencerHandle& SequencerHandle::operator=(SequencerHandle&&) = default;

SequencerHandle::~SequencerHandle() = default;

SequencerHandle::operator bool() const {
    return static_cast<bool>(self);
}

Client SequencerHandle::client() const {
    return Client(self->m_client);
}

void SequencerHandle::sayHello() const {
    if(not self) throw Exception("Invalid mobject::SequencerHandle object");
    auto& rpc = self->m_client->m_say_hello;
    auto& ph  = self->m_ph;
    auto& sequencer_id = self->m_sequencer_id;
    rpc.on(ph)(sequencer_id);
}

void SequencerHandle::computeSum(
        int32_t x, int32_t y,
        int32_t* result,
        AsyncRequest* req) const
{
    if(not self) throw Exception("Invalid mobject::SequencerHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto& sequencer_id = self->m_sequencer_id;
    if(req == nullptr) { // synchronous call
        RequestResult<int32_t> response = rpc.on(ph)(sequencer_id, x, y);
        if(response.success()) {
            if(result) *result = response.value();
        } else {
            throw Exception(response.error());
        }
    } else { // asynchronous call
        auto async_response = rpc.on(ph).async(sequencer_id, x, y);
        auto async_request_impl =
            std::make_shared<AsyncRequestImpl>(std::move(async_response));
        async_request_impl->m_wait_callback =
            [result](AsyncRequestImpl& async_request_impl) {
                RequestResult<int32_t> response =
                    async_request_impl.m_async_response.wait();
                    if(response.success()) {
                        if(result) *result = response.value();
                    } else {
                        throw Exception(response.error());
                    }
            };
        *req = AsyncRequest(std::move(async_request_impl));
    }
}

}
