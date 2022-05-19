/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "mobject/Exception.hpp"
#include "mobject/Client.hpp"
#include "mobject/SequencerHandle.hpp"
#include "mobject/RequestResult.hpp"

#include "ClientImpl.hpp"
#include "SequencerHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;

namespace mobject {

Client::Client() = default;

Client::Client(const tl::engine& engine)
: self(std::make_shared<ClientImpl>(engine)) {}

Client::Client(margo_instance_id mid)
: self(std::make_shared<ClientImpl>(mid)) {}

Client::Client(const std::shared_ptr<ClientImpl>& impl)
: self(impl) {}

Client::Client(Client&& other) = default;

Client& Client::operator=(Client&& other) = default;

Client::Client(const Client& other) = default;

Client& Client::operator=(const Client& other) = default;


Client::~Client() = default;

const tl::engine& Client::engine() const {
    return self->m_engine;
}

Client::operator bool() const {
    return static_cast<bool>(self);
}

SequencerHandle Client::makeSequencerHandle(
        const std::string& address,
        uint16_t provider_id,
        const UUID& sequencer_id,
        bool check) const {
    auto endpoint  = self->m_engine.lookup(address);
    auto ph        = tl::provider_handle(endpoint, provider_id);
    RequestResult<bool> result;
    result.success() = true;
    if(check) {
        result = self->m_check_sequencer.on(ph)(sequencer_id);
    }
    if(result.success()) {
        auto sequencer_impl = std::make_shared<SequencerHandleImpl>(self, std::move(ph), sequencer_id);
        return SequencerHandle(sequencer_impl);
    } else {
        throw Exception(result.error());
        return SequencerHandle(nullptr);
    }
}

std::string Client::getConfig() const {
    return "{}";
}

}
