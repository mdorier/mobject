/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_SEQUENCER_HANDLE_IMPL_H
#define __MOBJECT_SEQUENCER_HANDLE_IMPL_H

#include <mobject/UUID.hpp>

namespace mobject {

class SequencerHandleImpl {

    public:

    UUID                        m_sequencer_id;
    std::shared_ptr<ClientImpl> m_client;
    tl::provider_handle         m_ph;

    SequencerHandleImpl() = default;
    
    SequencerHandleImpl(const std::shared_ptr<ClientImpl>& client, 
                       tl::provider_handle&& ph,
                       const UUID& sequencer_id)
    : m_sequencer_id(sequencer_id)
    , m_client(client)
    , m_ph(std::move(ph)) {}
};

}

#endif
