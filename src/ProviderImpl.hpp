/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_PROVIDER_IMPL_H
#define __MOBJECT_PROVIDER_IMPL_H

#include "mobject/Backend.hpp"
#include "mobject/UUID.hpp"

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <tuple>

#define FIND_SEQUENCER(__var__) \
        std::shared_ptr<Backend> __var__;\
        do {\
            std::lock_guard<tl::mutex> lock(m_backends_mtx);\
            auto it = m_backends.find(sequencer_id);\
            if(it == m_backends.end()) {\
                result.success() = false;\
                result.error() = "Sequencer with UUID "s + sequencer_id.to_string() + " not found";\
                req.respond(result);\
                spdlog::error("[provider:{}] Sequencer {} not found", id(), sequencer_id.to_string());\
                return;\
            }\
            __var__ = it->second;\
        }while(0)

namespace mobject {

using namespace std::string_literals;
namespace tl = thallium;

class ProviderImpl : public tl::provider<ProviderImpl> {

    auto id() const { return get_provider_id(); } // for convenience

    using json = nlohmann::json;

    public:

    std::string          m_token;
    tl::pool             m_pool;
    // Admin RPC
    tl::remote_procedure m_create_sequencer;
    tl::remote_procedure m_open_sequencer;
    tl::remote_procedure m_close_sequencer;
    tl::remote_procedure m_destroy_sequencer;
    // Client RPC
    tl::remote_procedure m_check_sequencer;
    tl::remote_procedure m_say_hello;
    tl::remote_procedure m_compute_sum;
    // Backends
    std::unordered_map<UUID, std::shared_ptr<Backend>> m_backends;
    tl::mutex m_backends_mtx;

    ProviderImpl(const tl::engine& engine, uint16_t provider_id, const tl::pool& pool)
    : tl::provider<ProviderImpl>(engine, provider_id)
    , m_pool(pool)
    , m_create_sequencer(define("mobject_create_sequencer", &ProviderImpl::createSequencer, pool))
    , m_open_sequencer(define("mobject_open_sequencer", &ProviderImpl::openSequencer, pool))
    , m_close_sequencer(define("mobject_close_sequencer", &ProviderImpl::closeSequencer, pool))
    , m_destroy_sequencer(define("mobject_destroy_sequencer", &ProviderImpl::destroySequencer, pool))
    , m_check_sequencer(define("mobject_check_sequencer", &ProviderImpl::checkSequencer, pool))
    , m_say_hello(define("mobject_say_hello", &ProviderImpl::sayHello, pool))
    , m_compute_sum(define("mobject_compute_sum",  &ProviderImpl::computeSum, pool))
    {
        spdlog::trace("[provider:{0}] Registered provider with id {0}", id());
    }

    ~ProviderImpl() {
        spdlog::trace("[provider:{}] Deregistering provider", id());
        m_create_sequencer.deregister();
        m_open_sequencer.deregister();
        m_close_sequencer.deregister();
        m_destroy_sequencer.deregister();
        m_check_sequencer.deregister();
        m_say_hello.deregister();
        m_compute_sum.deregister();
        spdlog::trace("[provider:{}]    => done!", id());
    }

    void createSequencer(const tl::request& req,
                        const std::string& token,
                        const std::string& sequencer_type,
                        const std::string& sequencer_config) {

        spdlog::trace("[provider:{}] Received createSequencer request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), sequencer_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), sequencer_config);

        auto sequencer_id = UUID::generate();
        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        json json_config;
        try {
            json_config = json::parse(sequencer_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse sequencer configuration for sequencer {}",
                    id(), sequencer_id.to_string());
            req.respond(result);
            return;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = SequencerFactory::createSequencer(sequencer_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when creating sequencer {} of type {}:",
                    id(), sequencer_id.to_string(), sequencer_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            req.respond(result);
            return;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown sequencer type "s + sequencer_type;
            spdlog::error("[provider:{}] Unknown sequencer type {} for sequencer {}",
                    id(), sequencer_type, sequencer_id.to_string());
            req.respond(result);
            return;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[sequencer_id] = std::move(backend);
            result.value() = sequencer_id;
        }
        
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully created sequencer {} of type {}",
                id(), sequencer_id.to_string(), sequencer_type);
    }

    void openSequencer(const tl::request& req,
                      const std::string& token,
                      const std::string& sequencer_type,
                      const std::string& sequencer_config) {

        spdlog::trace("[provider:{}] Received openSequencer request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), sequencer_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), sequencer_config);

        auto sequencer_id = UUID::generate();
        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        json json_config;
        try {
            json_config = json::parse(sequencer_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse sequencer configuration for sequencer {}",
                    id(), sequencer_id.to_string());
            req.respond(result);
            return;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = SequencerFactory::openSequencer(sequencer_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when opening sequencer {} of type {}:",
                    id(), sequencer_id.to_string(), sequencer_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            req.respond(result);
            return;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown sequencer type "s + sequencer_type;
            spdlog::error("[provider:{}] Unknown sequencer type {} for sequencer {}",
                    id(), sequencer_type, sequencer_id.to_string());
            req.respond(result);
            return;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[sequencer_id] = std::move(backend);
            result.value() = sequencer_id;
        }
        
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully created sequencer {} of type {}",
                id(), sequencer_id.to_string(), sequencer_type);
    }

    void closeSequencer(const tl::request& req,
                        const std::string& token,
                        const UUID& sequencer_id) {
        spdlog::trace("[provider:{}] Received closeSequencer request for sequencer {}",
                id(), sequencer_id.to_string());

        RequestResult<bool> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(sequencer_id) == 0) {
                result.success() = false;
                result.error() = "Sequencer "s + sequencer_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Sequencer {} not found", id(), sequencer_id.to_string());
                return;
            }

            m_backends.erase(sequencer_id);
        }
        req.respond(result);
        spdlog::trace("[provider:{}] Sequencer {} successfully closed", id(), sequencer_id.to_string());
    }
    
    void destroySequencer(const tl::request& req,
                         const std::string& token,
                         const UUID& sequencer_id) {
        RequestResult<bool> result;
        spdlog::trace("[provider:{}] Received destroySequencer request for sequencer {}", id(), sequencer_id.to_string());

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(sequencer_id) == 0) {
                result.success() = false;
                result.error() = "Sequencer "s + sequencer_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Sequencer {} not found", id(), sequencer_id.to_string());
                return;
            }

            result = m_backends[sequencer_id]->destroy();
            m_backends.erase(sequencer_id);
        }

        req.respond(result);
        spdlog::trace("[provider:{}] Sequencer {} successfully destroyed", id(), sequencer_id.to_string());
    }

    void checkSequencer(const tl::request& req,
                       const UUID& sequencer_id) {
        spdlog::trace("[provider:{}] Received checkSequencer request for sequencer {}", id(), sequencer_id.to_string());
        RequestResult<bool> result;
        FIND_SEQUENCER(sequencer);
        result.success() = true;
        req.respond(result);
        spdlog::trace("[provider:{}] Code successfully executed on sequencer {}", id(), sequencer_id.to_string());
    }

    void sayHello(const tl::request& req,
                  const UUID& sequencer_id) {
        spdlog::trace("[provider:{}] Received sayHello request for sequencer {}", id(), sequencer_id.to_string());
        RequestResult<bool> result;
        FIND_SEQUENCER(sequencer);
        sequencer->sayHello();
        spdlog::trace("[provider:{}] Successfully executed sayHello on sequencer {}", id(), sequencer_id.to_string());
    }

    void computeSum(const tl::request& req,
                    const UUID& sequencer_id,
                    int32_t x, int32_t y) {
        spdlog::trace("[provider:{}] Received sayHello request for sequencer {}", id(), sequencer_id.to_string());
        RequestResult<int32_t> result;
        FIND_SEQUENCER(sequencer);
        result = sequencer->computeSum(x, y);
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully executed computeSum on sequencer {}", id(), sequencer_id.to_string());
    }

};

}

#endif
