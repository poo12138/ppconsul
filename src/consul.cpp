//  Copyright (c) 2014 Andrey Upadyshev <oliora@gmail.com>
//
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "ppconsul/consul.h"
#include "ppconsul/helpers.h"

#include "http_client_impl.inl"


namespace ppconsul {

    namespace {
        // Creates base URL like "http://<addr>" if 'addr' has no scheme specified
        // or just "<addr>" if 'addr' contains any scheme.
        inline std::string makeAddr(const std::string& addr)
        {
            if (addr.find("://") != std::string::npos)
                return addr;
            else
                return "http://" + addr;
        }
    }

    const char *BadStatus::what() const PPCONSUL_NOEXCEPT
    {
        if (m_what.empty())
        {
            if (!m_message.empty())
            {
                m_what = helpers::format("%s [%03d %s]",
                    m_message.c_str(),
                    m_status.code(),
                    m_status.message().c_str());
            }
            else
            {
                m_what = helpers::format("%03d %s",
                    m_status.code(),
                    m_status.message().c_str());
            }
        }

        return m_what.c_str();
    }

    Consul::Consul(const std::string& defaultToken, const std::string& dataCenter, const std::string& addr)
    : m_client(new impl::HttpClient())
    , m_addr(makeAddr(addr))
    , m_dataCenter(dataCenter)
    , m_defaultToken(defaultToken)
    {}
    
    Consul::~Consul()
    {}

    Consul::Consul(Consul &&op)
    : m_client(std::move(op.m_client))
    , m_addr(std::move(op.m_addr))
    , m_dataCenter(std::move(op.m_dataCenter))
    , m_defaultToken(std::move(op.m_defaultToken))
    {}

    Consul& Consul::operator= (Consul &&op)
    {
        if (this != &op)
        {
            m_client = std::move(op.m_client);
            m_addr = std::move(op.m_addr);
            m_dataCenter = std::move(op.m_dataCenter);
            m_defaultToken = std::move(op.m_defaultToken);
        }
        
        return *this;
    }

    Response<std::string> Consul::get_impl(http::Status& status, const std::string& url)
    {
        Response<std::string> r;
        std::tie(status, r.headers(), r.value()) = m_client->get(url);
        return r;
    }

    std::string Consul::put_impl(http::Status& status, const std::string& url, const std::string& body)
    {
        std::string r;
        std::tie(status, r) = m_client->put(url, body);
        return r;
    }

    std::string Consul::del_impl(http::Status& status, const std::string& url)
    {
        std::string r;
        std::tie(status, r) = m_client->del(url);
        return r;
    }

}
