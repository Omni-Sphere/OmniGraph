#pragma once

#include <graphqlservice/GraphQLService.h>
#include "Generated/UserQueryObject.h"
#include "Generated/UserObject.h"
#include "Resolvers/User/Resolver.hpp"
#include "User.hpp"
#include "Enums/UserFilter.hpp"

#include <memory>
#include <vector>

class UserQuery
{
public:
    explicit UserQuery(std::shared_ptr<omnicore::service::User> userService)
        : m_userService(std::move(userService)) {}

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::User>> getGetUser(graphql::service::FieldParams &&params, const graphql::omnisphere::GetUserInput &userInput) const
    {
        try
        {
            const int sentParams = userInput.Entry.has_value() + userInput.Code.has_value() + userInput.Name.has_value() + userInput.Email.has_value() + userInput.Phone.has_value() + userInput.Employee.has_value();

            omnicore::model::User userModel;

            if(userInput.Code.has_value())
                userModel = m_userService->Get(omnicore::enums::UserFilter::Code, userInput.Code.value());

            if(userInput.Name.has_value())

            if(userInput.Email.has_value())
                userModel = m_userService->Get(omnicore::enums::UserFilter::Email, userInput.Email.value());

            if(userInput.Employee.has_value())
                userModel = m_userService->Get(omnicore::enums::UserFilter::Employee, std::to_string(userInput.Employee.value()));

            std::shared_ptr<UserResolver> resolver = std::make_shared<UserResolver>(std::make_shared<omnicore::model::User>(userModel));

            co_return std::make_shared<graphql::omnisphere::object::User>(std::move(resolver));
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "GetUser"}}});
        }
    }

    graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::User>>> getSearchUsers(graphql::service::FieldParams &&params, const graphql::omnisphere::SearchUsersInput &filter) const
    {
        try
        {
            
            std::vector<std::shared_ptr<graphql::omnisphere::object::User>> Users;

            omnicore::dto::SearchUsers filter;

            std::vector<omnicore::model::User> model = m_userService->Search(filter);

            for (const omnicore::model::User &user : model)
            {
                std::shared_ptr<UserResolver> resolver = std::make_shared<UserResolver>(std::make_shared<omnicore::model::User>(user));
                Users.emplace_back(std::make_shared<graphql::omnisphere::object::User>(std::move(resolver)));
            }

            co_return Users;
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"User", "GetUsers"}}});
        }
    }

private:
    std::shared_ptr<omnicore::service::User> m_userService;
};
