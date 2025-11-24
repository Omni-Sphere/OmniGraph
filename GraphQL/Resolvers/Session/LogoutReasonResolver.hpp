#pragma once
#include <stdexcept>
#include "Generated/OmnisSchema.h"
#include "Enums/LogoutReason.hpp"

inline omnicore::enums::LogoutReason toLocalLogoutReason(graphql::omnisphere::LogoutReason gqlReason)
{
    switch (gqlReason)
    {
        case graphql::omnisphere::LogoutReason::USER_LOGOUT:
            return omnicore::enums::LogoutReason::USER_LOGOUT;
        case graphql::omnisphere::LogoutReason::SESSION_TIMEOUT:
            return omnicore::enums::LogoutReason::SESSION_TIMEOUT;
        case graphql::omnisphere::LogoutReason::TOKEN_EXPIRED:
            return omnicore::enums::LogoutReason::TOKEN_EXPIRED;
        case graphql::omnisphere::LogoutReason::FORCE_LOGOUT:
            return omnicore::enums::LogoutReason::FORCE_LOGOUT;
        case graphql::omnisphere::LogoutReason::DEVICE_DISCONNECTED:
            return omnicore::enums::LogoutReason::DEVICE_DISCONNECTED;
        case graphql::omnisphere::LogoutReason::SECURITY_EVENT:
            return omnicore::enums::LogoutReason::SECURITY_EVENT;
        default:
            throw std::runtime_error("Invalid GraphQL LogoutReason");
    }
}

inline graphql::omnisphere::LogoutReason toGraphQLLogoutReason(omnicore::enums::LogoutReason localReason)
{
    switch (localReason)
    {
        case omnicore::enums::LogoutReason::USER_LOGOUT:
            return graphql::omnisphere::LogoutReason::USER_LOGOUT;
        case omnicore::enums::LogoutReason::SESSION_TIMEOUT:
            return graphql::omnisphere::LogoutReason::SESSION_TIMEOUT;
        case omnicore::enums::LogoutReason::TOKEN_EXPIRED:
            return graphql::omnisphere::LogoutReason::TOKEN_EXPIRED;
        case omnicore::enums::LogoutReason::FORCE_LOGOUT:
            return graphql::omnisphere::LogoutReason::FORCE_LOGOUT;
        case omnicore::enums::LogoutReason::DEVICE_DISCONNECTED:
            return graphql::omnisphere::LogoutReason::DEVICE_DISCONNECTED;
        case omnicore::enums::LogoutReason::SECURITY_EVENT:
            return graphql::omnisphere::LogoutReason::SECURITY_EVENT;
        default:
            throw std::runtime_error("Invalid local LogoutReason");
    }
}

