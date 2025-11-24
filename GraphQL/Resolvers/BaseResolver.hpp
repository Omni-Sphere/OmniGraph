#pragma once
#include "ServiceInyector.hpp"
#include "Resolvers/User/Resolver.hpp"

template <typename TModel>
class BaseResolver
{
public:
    BaseResolver(std::shared_ptr<TModel> model, std::shared_ptr<ServiceInyector> services)
        : m_model(std::move(model)), m_services(std::move(services)) {}

protected:
    graphql::service::AwaitableScalar<int> getEntry(graphql::service::FieldParams&&) const {
        co_return m_model->Entry;
    }

    graphql::service::AwaitableScalar<std::string> getCode(graphql::service::FieldParams&&) const {
        co_return m_model->Code;
    }

    graphql::service::AwaitableScalar<std::string> getName(graphql::service::FieldParams&&) const {
        co_return m_model->Name;
    }

    graphql::service::AwaitableScalar<std::string> getCreateDate(graphql::service::FieldParams&&) const {
        co_return m_model->CreateDate;
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getLastUpdatedBy(graphql::service::FieldParams &&) const
    {
        if(!m_model->LastUpdatedBy.has_value()) 
            co_return nullptr;

        co_return co_await ResolveUser(m_model->LastUpdatedBy.value()); 
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getUpdateDate(graphql::service::FieldParams&&) const {
        co_return m_model->UpdateDate;
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getCreatedBy(graphql::service::FieldParams&& params) const
    {
        co_return co_await ResolveUser(m_model->CreatedBy);
    }

protected:
    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> ResolveUser(int entry) const
    {
        auto userModel = m_services->User->Get(
            omnicore::enums::UserFilter::Entry,
            std::to_string(entry)
        );

        auto resolver = std::make_shared<UserResolver>(
            std::make_shared<omnicore::model::User>(userModel)
        );

        co_return std::make_shared<graphql::omnisphere::object::User>(resolver);
    }

    std::shared_ptr<TModel> m_model;
    std::shared_ptr<ServiceInyector> m_services;
};
