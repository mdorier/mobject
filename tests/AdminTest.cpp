/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include <mobject/Admin.hpp>
#include <cppunit/extensions/HelperMacros.h>

extern thallium::engine engine;
extern std::string sequencer_type;

class AdminTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( AdminTest );
    CPPUNIT_TEST( testAdminCreateSequencer );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* sequencer_config = "{ \"path\" : \"mydb\" }";

    public:

    void setUp() {}
    void tearDown() {}

    void testAdminCreateSequencer() {
        mobject::Admin admin(engine);
        std::string addr = engine.self();

        mobject::UUID sequencer_id;
        // Create a valid Sequencer
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("admin.createSequencer should return a valid Sequencer",
                sequencer_id = admin.createSequencer(addr, 0, sequencer_type, sequencer_config));

        // Create a Sequencer with a wrong backend type
        mobject::UUID bad_id;
        CPPUNIT_ASSERT_THROW_MESSAGE("admin.createSequencer should throw an exception (wrong backend)",
                bad_id = admin.createSequencer(addr, 0, "blabla", sequencer_config),
                mobject::Exception);

        // Destroy the Sequencer
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("admin.destroySequencer should not throw on valid Sequencer",
            admin.destroySequencer(addr, 0, sequencer_id));

        // Destroy an invalid Sequencer
        CPPUNIT_ASSERT_THROW_MESSAGE("admin.destroySequencer should throw on invalid Sequencer",
            admin.destroySequencer(addr, 0, bad_id),
            mobject::Exception);
    }
};
CPPUNIT_TEST_SUITE_REGISTRATION( AdminTest );
