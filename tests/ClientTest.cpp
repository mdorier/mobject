/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <mobject/Client.hpp>
#include <mobject/Admin.hpp>

extern thallium::engine engine;
extern std::string sequencer_type;

class ClientTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ClientTest );
    CPPUNIT_TEST( testOpenSequencer );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* sequencer_config = "{ \"path\" : \"mydb\" }";
    UUID sequencer_id;

    public:

    void setUp() {
        mobject::Admin admin(engine);
        std::string addr = engine.self();
        sequencer_id = admin.createSequencer(addr, 0, sequencer_type, sequencer_config);
    }

    void tearDown() {
        mobject::Admin admin(engine);
        std::string addr = engine.self();
        admin.destroySequencer(addr, 0, sequencer_id);
    }

    void testOpenSequencer() {
        mobject::Client client(engine);
        std::string addr = engine.self();
        
        Sequencer my_sequencer = client.open(addr, 0, sequencer_id);
        CPPUNIT_ASSERT_MESSAGE(
                "Sequencer should be valid",
                static_cast<bool>(my_sequencer));

        auto bad_id = UUID::generate();
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.open should fail on non-existing sequencer",
                client.open(addr, 0, bad_id);
    }
};
CPPUNIT_TEST_SUITE_REGISTRATION( ClientTest );
