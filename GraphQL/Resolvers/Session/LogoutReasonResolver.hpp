#pragma once
#include <stdexcept>
#include "Generated/OmnisSchema.h"
#include "Enums/LogoutReason.hpp"

inline omnisphere::omnicore::enums::LogoutReason toLocalLogoutReason(graphql::omnisphere::LogoutReason gqlReason)
{
    switch (gqlReason)
    {
        case graphql::omnisphere::LogoutReason::USER_LOGOUT:
            return omnisphere::omnicore::enums::LogoutReason::USER_LOGOUT;
        case graphql::omnisphere::LogoutReason::SESSION_TIMEOUT:
            return omnisphere::omnicore::enums::LogoutReason::SESSION_TIMEOUT;
        case graphql::omnisphere::LogoutReason::TOKEN_EXPIRED:
            return omnisphere::omnicore::enums::LogoutReason::TOKEN_EXPIRED;
        case graphql::omnisphere::LogoutReason::FORCE_LOGOUT:
            return omnisphere::omnicore::enums::LogoutReason::FORCE_LOGOUT;
        case graphql::omnisphere::LogoutReason::DEVICE_DISCONNECTED:
            return omnisphere::omnicore::enums::LogoutReason::DEVICE_DISCONNECTED;
        case graphql::omnisphere::LogoutReason::SECURITY_EVENT:
            return omnisphere::omnicore::enums::LogoutReason::SECURITY_EVENT;
        default:
            throw std::runtime_error("Invalid GraphQL LogoutReason");
    }
}

inline graphql::omnisphere::LogoutReason toGraphQLLogoutReason(omnisphere::omnicore::enums::LogoutReason localReason)
{
    switch (localReason)
    {
        case omnisphere::omnicore::enums::LogoutReason::USER_LOGOUT:
            return graphql::omnisphere::LogoutReason::USER_LOGOUT;
        case omnisphere::omnicore::enums::LogoutReason::SESSION_TIMEOUT:
            return graphql::omnisphere::LogoutReason::SESSION_TIMEOUT;
        case omnisphere::omnicore::enums::LogoutReason::TOKEN_EXPIRED:
            return graphql::omnisphere::LogoutReason::TOKEN_EXPIRED;
        case omnisphere::omnicore::enums::LogoutReason::FORCE_LOGOUT:
            return graphql::omnisphere::LogoutReason::FORCE_LOGOUT;
        case omnisphere::omnicore::enums::LogoutReason::DEVICE_DISCONNECTED:
            return graphql::omnisphere::LogoutReason::DEVICE_DISCONNECTED;
        case omnisphere::omnicore::enums::LogoutReason::SECURITY_EVENT:
            return graphql::omnisphere::LogoutReason::SECURITY_EVENT;
        default:
            throw std::runtime_error("Invalid local LogoutReason");
    }
}

