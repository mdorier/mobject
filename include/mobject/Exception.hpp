/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __MOBJECT_EXCEPTION_HPP
#define __MOBJECT_EXCEPTION_HPP

#include <exception>
#include <string>

namespace mobject {

class Exception : public std::exception {

    std::string m_error;

    public:

    template<typename ... Args>
    Exception(Args&&... args)
    : m_error(std::forward<Args>(args)...) {}

    virtual const char* what() const noexcept override {
        return m_error.c_str();
    }
    
};

}

#endif
