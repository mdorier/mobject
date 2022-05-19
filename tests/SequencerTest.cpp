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

class SequencerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( SequencerTest );
    CPPUNIT_TEST( testMakeSequencerHandle );
    CPPUNIT_TEST( testSayHello );
    CPPUNIT_TEST( testComputeSum );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* sequencer_config = "{ \"path\" : \"mydb\" }";
    mobject::UUID sequencer_id;

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

    void testMakeSequencerHandle() {
        mobject::Client client(engine);
        std::string addr = engine.self();

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeSequencerHandle should not throw for valid id.",
                client.makeSequencerHandle(addr, 0, sequencer_id));

        auto bad_id = mobject::UUID::generate();
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.makeSequencerHandle should throw for invalid id.",
                client.makeSequencerHandle(addr, 0, bad_id),
                mobject::Exception);
        
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.makeSequencerHandle should throw for invalid provider.",
                client.makeSequencerHandle(addr, 1, sequencer_id),
                std::exception);
        
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeSequencerHandle should not throw for invalid id when check=false.",
                client.makeSequencerHandle(addr, 0, bad_id, false));

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeSequencerHandle should not throw for invalid provider when check=false.",
                client.makeSequencerHandle(addr, 1, sequencer_id, false));
    }

    void testSayHello() {
        mobject::Client client(engine);
        std::string addr = engine.self();
        
        mobject::SequencerHandle my_sequencer = client.makeSequencerHandle(addr, 0, sequencer_id);

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_sequencer.sayHello() should not throw.",
                my_sequencer.sayHello());
    }

    void testComputeSum() {
        mobject::Client client(engine);
        std::string addr = engine.self();
        
        mobject::SequencerHandle my_sequencer = client.makeSequencerHandle(addr, 0, sequencer_id);

        int32_t result = 0;
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_sequencer.computeSum() should not throw.",
                my_sequencer.computeSum(42, 51, &result));

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "42 + 51 should be 93",
                93, result);

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_sequencer.computeSum() should not throw when passed NULL.",
                my_sequencer.computeSum(42, 51, nullptr));

        mobject::AsyncRequest request;
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_sequencer.computeSum() should not throw when called asynchronously.",
                my_sequencer.computeSum(42, 51, &result, &request));

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "request.wait() should not throw.",
                request.wait());
    }

};
CPPUNIT_TEST_SUITE_REGISTRATION( SequencerTest );
