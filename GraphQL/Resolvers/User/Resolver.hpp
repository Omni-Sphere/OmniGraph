#pragma once

#include "GraphQL/Generated/UserObject.h"
#include "Models/User.hpp"

class UserResolver {
public:
    explicit UserResolver(std::shared_ptr<omnisphere::omnicore::models::User> model)
        : m_model(std::move(model))
    {}

    graphql::service::AwaitableScalar<int> getEntry(graphql::service::FieldParams&&) const {
        co_return m_model->Entry;
    }

    graphql::service::AwaitableScalar<std::string> getCode(graphql::service::FieldParams&&) const {
        co_return m_model->Code;
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getName(graphql::service::FieldParams&&) const {
        co_return m_model->Name;
    }
    
    graphql::service::AwaitableScalar<std::optional<std::string>> getEmail(graphql::service::FieldParams&&) const {
        co_return m_model->Email;
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getPhone(graphql::service::FieldParams&&) const {
        co_return m_model->Phone;
    }

    graphql::service::AwaitableScalar<std::optional<int>> getEmployee(graphql::service::FieldParams&&) const {
        co_return m_model->Employee;
    }

    graphql::service::AwaitableScalar<bool> getSuperUser(graphql::service::FieldParams&&) const {
        co_return m_model->SuperUser;
    }

    graphql::service::AwaitableScalar<bool> getIsLocked(graphql::service::FieldParams&&) const {
        co_return m_model->IsLocked;
    }

    graphql::service::AwaitableScalar<bool> getIsActive(graphql::service::FieldParams&&) const {
        co_return m_model->IsActive;
    }

    graphql::service::AwaitableScalar<bool> getChangePasswordNextLogin(graphql::service::FieldParams&&) const {
        co_return m_model->ChangePasswordNextLogin;
    }

    graphql::service::AwaitableScalar<bool> getPasswordNeverExpires(graphql::service::FieldParams&&) const {
        co_return m_model->PasswordNeverExpires;
    }

    graphql::service::AwaitableScalar<int> getCreatedBy(graphql::service::FieldParams&&) const {
        co_return m_model->CreatedBy;
    }

    graphql::service::AwaitableScalar<std::optional<int>> getLastUpdatedBy(graphql::service::FieldParams&&) const {
        co_return m_model->LastUpdatedBy;
    }

    graphql::service::AwaitableScalar<std::string> getCreateDate(graphql::service::FieldParams&&) const {
        co_return m_model->CreateDate;
    }
    
    graphql::service::AwaitableScalar<std::optional<std::string>> getUpdateDate(graphql::service::FieldParams&&) const {
        co_return m_model->UpdateDate;
    }
    
    void beginSelectionSet(const graphql::service::SelectionSetParams&) const {}
    void endSelectionSet(const graphql::service::SelectionSetParams&) const {}

private:
    std::shared_ptr<omnisphere::omnicore::models::User> m_model;
};
