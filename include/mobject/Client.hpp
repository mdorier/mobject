/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_CLIENT_HPP
#define __MOBJECT_CLIENT_HPP

#include <mobject/SequencerHandle.hpp>
#include <mobject/UUID.hpp>
#include <thallium.hpp>
#include <memory>

namespace mobject {

class ClientImpl;
class SequencerHandle;

/**
 * @brief The Client object is the main object used to establish
 * a connection with a Mobject service.
 */
class Client {

    friend class SequencerHandle;

    public:

    /**
     * @brief Default constructor.
     */
    Client();

    /**
     * @brief Constructor using a margo instance id.
     *
     * @param mid Margo instance id.
     */
    Client(margo_instance_id mid);

    /**
     * @brief Constructor.
     *
     * @param engine Thallium engine.
     */
    Client(const thallium::engine& engine);

    /**
     * @brief Copy constructor.
     */
    Client(const Client&);

    /**
     * @brief Move constructor.
     */
    Client(Client&&);

    /**
     * @brief Copy-assignment operator.
     */
    Client& operator=(const Client&);

    /**
     * @brief Move-assignment operator.
     */
    Client& operator=(Client&&);

    /**
     * @brief Destructor.
     */
    ~Client();

    /**
     * @brief Returns the thallium engine used by the client.
     */
    const thallium::engine& engine() const;

    /**
     * @brief Creates a handle to a remote sequencer and returns.
     * You may set "check" to false if you know for sure that the
     * corresponding sequencer exists, which will avoid one RPC.
     *
     * @param address Address of the provider holding the database.
     * @param provider_id Provider id.
     * @param sequencer_id Sequencer UUID.
     * @param check Checks if the Database exists by issuing an RPC.
     *
     * @return a SequencerHandle instance.
     */
    SequencerHandle makeSequencerHandle(const std::string& address,
                                      uint16_t provider_id,
                                      const UUID& sequencer_id,
                                      bool check = true) const;

    /**
     * @brief Checks that the Client instance is valid.
     */
    operator bool() const;

    /**
     * @brief Get internal configuration as a JSON-formatted string.
     *
     * @return configuration string.
     */
    std::string getConfig() const;

    private:

    Client(const std::shared_ptr<ClientImpl>& impl);

    std::shared_ptr<ClientImpl> self;
};

}

#endif
