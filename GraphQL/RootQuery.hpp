#pragma once
#include <graphqlservice/GraphQLService.h>
#include <memory>
#include "ServiceInyector.hpp"
#include "QueryObject.h"
#include "Resolvers/User/Query.hpp"
#include "Resolvers/Item/Query.hpp"
#include "Resolvers/ItemGroup/Query.hpp"
#include "Resolvers/ItemBrand/Query.hpp"

namespace graphql::resolvers
{

    class RootQueryResolver
    {
    private:
        std::shared_ptr<ServiceInyector> _services;

    public:
        explicit RootQueryResolver(std::shared_ptr<ServiceInyector> services)
            : _services(std::move(services))
        {
        }

        graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::UserQuery>> getUser() const
        {
            std::shared_ptr<UserQuery> userQ = std::make_shared<UserQuery>(_services->User);
            co_return std::make_shared<graphql::omnisphere::object::UserQuery>(userQ);
        }

        graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemQuery>> getItem() const
        {
            std::shared_ptr<ItemQuery> itemQ = std::make_shared<ItemQuery>(_services);
            co_return std::make_shared<graphql::omnisphere::object::ItemQuery>(itemQ);
        }

        graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemGroupQuery>> getItemGroup() const
        {
            std::shared_ptr<ItemGroupQuery> itemGroupQ = std::make_shared<ItemGroupQuery>(_services);
            co_return std::make_shared<graphql::omnisphere::object::ItemGroupQuery>(itemGroupQ);
        }

        graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrandQuery>> getItemBrand() const
        {
            std::shared_ptr<ItemBrandQuery> itemBrandQ = std::make_shared<ItemBrandQuery>(_services);
            co_return std::make_shared<graphql::omnisphere::object::ItemBrandQuery>(itemBrandQ);
        }

        void beginSelectionSet(const graphql::service::SelectionSetParams &) const {}
        void endSelectionSet(const graphql::service::SelectionSetParams &) const {}
    };

};
