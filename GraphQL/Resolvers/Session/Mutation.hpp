#pragma once

#include <memory>
#include <optional>
#include <string>
#include <graphqlservice/GraphQLService.h>
#include "GraphQL/Generated/SessionMutationObject.h"
#include "GraphQL/Resolvers/Session/AuthPayLoad.hpp"
#include "GraphQL/Resolvers/Session/LogoutPayload.hpp"
#include "GraphQL/Resolvers/Session/LogoutReasonResolver.hpp"
#include "Models/SessionData.hpp"
#include "Models/AuthPayload.hpp"
#include "DTOs/Login.hpp"
#include "DTOs/Logout.hpp"
#include "User.hpp"
#include "Session.hpp"

class SessionMutation 
{
private:
    std::shared_ptr<omnicore::service::Session> m_session;
    

public:
    explicit SessionMutation(std::shared_ptr<omnicore::service::Session> _session) noexcept
        : m_session(std::move(_session)) {}

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::AuthPayLoad>> getLogin(graphql::service::FieldParams &&params, const graphql::omnisphere::SessionParams &sessionParam) const
    {
        try
        {
            omnicore::dto::Login login(
                sessionParam.Code,
                sessionParam.Email,
                sessionParam.Phone,
                sessionParam.StartDate,
                sessionParam.DeviceIP,
                sessionParam.HostName,
                sessionParam.Password
            );

            omnicore::model::AuthPayload authPayload = m_session->Login(login);
            std::shared_ptr<AuthPayloadResolver> resolver = std::make_shared<AuthPayloadResolver>(std::make_shared<omnicore::model::AuthPayload>(authPayload));

            co_return std::make_shared<graphql::omnisphere::object::AuthPayLoad>(resolver);
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Session", "Login"}}});
        }
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::LogoutPayload>> getLogout(graphql::service::FieldParams &&params, const graphql::omnisphere::LogoutParams &dto) const
    {
        try
        {
            omnicore::dto::Logout logout(
                dto.SessionUUID,
                dto.EndDate,
                toLocalLogoutReason(dto.Reason),
                dto.Message
            );

            std::shared_ptr<omnicore::model::LogoutPayload> model = std::make_shared<omnicore::model::LogoutPayload>(m_session->Logout(logout));

            std::shared_ptr<LogoutPayloadResolver> resolver = std::make_shared<LogoutPayloadResolver>(model);

            co_return std::make_shared<graphql::omnisphere::object::LogoutPayload>(resolver);
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"SessionMutation", "logout"}}});
        }
    };
};
