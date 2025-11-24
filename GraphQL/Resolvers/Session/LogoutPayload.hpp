#pragma once

#include "Generated/LogoutPayloadObject.h"
#include "Resolvers/Session/LogoutReasonResolver.hpp"
#include "Models/LogoutPayload.hpp"
#include "Enums/LogoutReason.hpp"

class LogoutPayloadResolver
{
public:
    explicit LogoutPayloadResolver(std::shared_ptr<omnicore::model::LogoutPayload> model)
        : m_model(std::move(model)) {};

    graphql::service::AwaitableScalar<std::string> getSessionUUID(graphql::service::FieldParams &&) const
    {
        co_return m_model->SessionUUID;
    }

    graphql::service::AwaitableScalar<std::string> getStartDate(graphql::service::FieldParams &&) const
    {
        co_return m_model->StartDate;
    }

    graphql::service::AwaitableScalar<std::string> getEndDate(graphql::service::FieldParams &&) const
    {
        co_return m_model->EndDate;
    }

    graphql::service::AwaitableScalar<int> getDuration(graphql::service::FieldParams &&) const
    {
        co_return m_model->Duration;
    }

    graphql::service::AwaitableScalar<graphql::omnisphere::LogoutReason> getReason(graphql::service::FieldParams &&) const
    {
        co_return toGraphQLLogoutReason(m_model->Reason);
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getMessage(graphql::service::FieldParams &&) const
    {
        co_return m_model->Message;
    }

    void beginSelectionSet(const graphql::service::SelectionSetParams &) const {}
    void endSelectionSet(const graphql::service::SelectionSetParams &) const {}

private:
    std::shared_ptr<omnicore::model::LogoutPayload> m_model;
};
