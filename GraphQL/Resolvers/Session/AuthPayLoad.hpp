#pragma once

#include "Generated/AuthPayLoadObject.h"
#include "Resolvers/User/Resolver.hpp"
#include "Models/AuthPayload.hpp"

class AuthPayloadResolver {
public:
    explicit AuthPayloadResolver(std::shared_ptr<omnisphere::omnicore::models::AuthPayload> model)
        : m_model(std::move(model)) {}

    graphql::service::AwaitableScalar<std::string> getAccessToken(graphql::service::FieldParams&&) const 
    {
        co_return m_model->AccessToken;
    }
    
    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getUser(graphql::service::FieldParams&& params) 
    {
        co_return std::make_shared<graphql::omnisphere::object::User>(std::move(std::make_shared<UserResolver>(m_model->User)));
    }

    void beginSelectionSet(const graphql::service::SelectionSetParams&) const {}
    void endSelectionSet(const graphql::service::SelectionSetParams&) const {}

private:
    std::shared_ptr<omnisphere::omnicore::models::AuthPayload> m_model;
};
