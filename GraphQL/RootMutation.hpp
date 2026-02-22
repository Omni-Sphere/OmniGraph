#pragma once
#include <graphqlservice/GraphQLService.h>
#include <map>
#include <memory>
#include <typeinfo>


#include "MutationObject.h"
#include "Resolvers/Item/Mutation.hpp"
#include "Resolvers/ItemBrand/Mutation.hpp"
#include "Resolvers/ItemGroup/Mutation.hpp"
#include "Resolvers/Session/Mutation.hpp"
#include "Resolvers/User/Mutation.hpp"
#include "ServiceInyector.hpp"


#include "Item/Item.hpp"
#include "ItemBrand/ItemBrand.hpp"
#include "ItemGroup/ItemGroup.hpp"
#include "Session/Session.hpp"
#include "User/User.hpp"


namespace graphql::resolvers {
class RootMutationResolver {
private:
  std::shared_ptr<ServiceInyector> _services;

public:
  explicit RootMutationResolver(std::shared_ptr<ServiceInyector> services)
      : _services(std::move(services)) {}

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::UserMutation>>
  applyUser(graphql::service::FieldParams &&) const {
    std::shared_ptr<UserMutation> userM =
        std::make_shared<UserMutation>(_services->User);
    co_return std::make_shared<graphql::omnisphere::object::UserMutation>(
        userM);
  }

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::SessionMutation>>
  applySession(graphql::service::FieldParams &&) const {
    std::shared_ptr<SessionMutation> sessionM =
        std::make_shared<SessionMutation>(_services->Session);
    co_return std::make_shared<graphql::omnisphere::object::SessionMutation>(
        sessionM);
  }

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::ItemMutation>>
  applyItem(graphql::service::FieldParams &&) const {
    std::shared_ptr<ItemMutation> itemM =
        std::make_shared<ItemMutation>(_services);
    co_return std::make_shared<graphql::omnisphere::object::ItemMutation>(
        itemM);
  }

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::ItemGroupMutation>>
  applyItemGroup(graphql::service::FieldParams &&) const {
    std::shared_ptr<ItemGroupMutation> itemGroupM =
        std::make_shared<ItemGroupMutation>(_services);
    co_return std::make_shared<graphql::omnisphere::object::ItemGroupMutation>(
        itemGroupM);
  }

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::ItemBrandMutation>>
  applyItemBrand(graphql::service::FieldParams &&) const {
    std::shared_ptr<ItemBrandMutation> itemBrandM =
        std::make_shared<ItemBrandMutation>(_services);
    co_return std::make_shared<graphql::omnisphere::object::ItemBrandMutation>(
        itemBrandM);
  }

  void beginSelectionSet(const graphql::service::SelectionSetParams &) const {}
  void endSelectionSet(const graphql::service::SelectionSetParams &) const {}
};

} // namespace graphql::resolvers
