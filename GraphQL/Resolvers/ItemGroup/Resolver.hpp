#pragma once
#include "GraphQL/Generated/ItemGroupObject.h"
#include "ItemGroup/Models/ItemGroup.hpp"
#include "Resolvers/BaseResolver.hpp"
#include "Resolvers/User/Resolver.hpp"
#include "ServiceInyector.hpp"


class ItemGroupResolver : public BaseResolver<omnisphere::omnierp::models::ItemGroup> {
public:
  using BaseResolver::getCode;
  using BaseResolver::getCreateDate;
  using BaseResolver::getCreatedBy;
  using BaseResolver::getEntry;
  using BaseResolver::getLastUpdatedBy;
  using BaseResolver::getName;
  using BaseResolver::getUpdateDate;

  explicit ItemGroupResolver(std::shared_ptr<omnisphere::omnierp::models::ItemGroup> model,
                             std::shared_ptr<ServiceInyector> _services)
      : BaseResolver(std::move(model), std::move(_services)) {}
};