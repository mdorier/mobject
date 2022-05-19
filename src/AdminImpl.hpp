/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_ADMIN_IMPL_H
#define __MOBJECT_ADMIN_IMPL_H

#include <thallium.hpp>

namespace mobject {

namespace tl = thallium;

class AdminImpl {

    public:

    tl::engine           m_engine;
    tl::remote_procedure m_create_sequencer;
    tl::remote_procedure m_open_sequencer;
    tl::remote_procedure m_close_sequencer;
    tl::remote_procedure m_destroy_sequencer;

    AdminImpl(const tl::engine& engine)
    : m_engine(engine)
    , m_create_sequencer(m_engine.define("mobject_create_sequencer"))
    , m_open_sequencer(m_engine.define("mobject_open_sequencer"))
    , m_close_sequencer(m_engine.define("mobject_close_sequencer"))
    , m_destroy_sequencer(m_engine.define("mobject_destroy_sequencer"))
    {}

    AdminImpl(margo_instance_id mid)
    : AdminImpl(tl::engine(mid)) {
    }

    ~AdminImpl() {}
};

}

#endif
