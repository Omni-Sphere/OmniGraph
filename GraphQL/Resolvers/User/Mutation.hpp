#pragma once

#include <graphqlservice/GraphQLService.h>

#include <memory>
#include <optional>
#include <string>
#include "Resolvers/User/Resolver.hpp"
#include "Generated/UserMutationObject.h"
#include "User/User.hpp"
#include "DTOs/CreateUser.hpp"
#include "DTOs/ChangePassword.hpp"
#include "Models/User.hpp"
#include "Enums/UserFilter.hpp"

class UserMutation
{
private:
    std::shared_ptr<omnisphere::omnicore::services::User> m_userService;

public:
    explicit UserMutation(std::shared_ptr<omnisphere::omnicore::services::User> user) noexcept
        : m_userService(std::move(user))
    {
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getCreateUser(graphql::service::FieldParams &&params, const graphql::omnisphere::CreateUserInput &_newUser) const
    {
        try
        {
            omnisphere::omnicore::dtos::CreateUser newUser(
                _newUser.Code,
                _newUser.Name,
                _newUser.Email,
                _newUser.Phone,
                _newUser.Employee,
                _newUser.Password,
                _newUser.SuperUser,
                _newUser.ChangePasswordNextLogin,
                _newUser.PasswordNeverExpires,
                _newUser.CreatedBy,
                _newUser.CreateDate
            );   

            if(!m_userService->Add(newUser))
                throw;

            omnisphere::omnicore::models::User userModel = m_userService->Get(omnisphere::omnicore::enums::UserFilter::Code, newUser.Code);

            std::shared_ptr<UserResolver> resolver = std::make_shared<UserResolver>(std::make_shared<omnisphere::omnicore::models::User>(userModel));

            co_return std::make_shared<graphql::omnisphere::object::User>(std::move(resolver));
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "CreateUser"}}});
        }
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getUpdateUser(graphql::service::FieldParams &&params, const graphql::omnisphere::UpdateUserInput &userChanges) const
    {
        try
        {
            omnisphere::omnicore::dtos::UpdateUser _user;

            _user.Where.Entry = userChanges.Where.Entry;
            _user.Where.Code = userChanges.Where.Code;
            _user.Data.Email = userChanges.Data.Email;
            _user.Data.Employee = userChanges.Data.Employee;
            _user.Data.Phone = userChanges.Data.Phone;
            _user.UpdateDate = userChanges.UpdateDate;
            _user.UpdatedBy = userChanges.UpdatedBy;
            
            omnisphere::omnicore::models::User userModel = m_userService->Modify(_user);

            std::shared_ptr<UserResolver> resolver = std::make_shared<UserResolver>(std::make_shared<omnisphere::omnicore::models::User>(userModel));

            co_return std::make_shared<graphql::omnisphere::object::User>(std::move(resolver));
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "UpdateUser"}}});
        }
    }

    graphql::service::AwaitableScalar<bool> getChangePassword(graphql::service::FieldParams &&params, const graphql::omnisphere::ChangePasswordInput &userParams) const
    {
        try
        {   
            omnisphere::omnicore::dtos::ChangePassword _user(
                userParams.Entry,
                userParams.Code,
                userParams.OldPassword,
                userParams.NewPassword,
                userParams.UpdateDate,
                userParams.UpdatedBy
            );

            if(!m_userService->ModifyPassword(_user))
                throw std::runtime_error("Error changing password");

            return true;
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "ModifyPassword"}}});
        }
    };

    graphql::service::AwaitableScalar<bool> getUpdateUserPolicies(graphql::service::FieldParams &&params, const graphql::omnisphere::UpdateUserPoliciesInput &userParams) const
    {
        try
        {
            return false;
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "ModifyPolicies"}}});
        }
    };
};