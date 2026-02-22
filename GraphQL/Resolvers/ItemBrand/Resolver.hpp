#pragma once

#include "GraphQL/Generated/ItemBrandObject.h"
#include "ItemBrand/Models/ItemBrand.hpp"

#include "Resolvers/BaseResolver.hpp"
#include "ServiceInyector.hpp"

class ItemBrandResolver : public BaseResolver<omnisphere::omnierp::models::ItemBrand> {
public:
  using BaseResolver::getCode;
  using BaseResolver::getCreateDate;
  using BaseResolver::getCreatedBy;
  using BaseResolver::getEntry;
  using BaseResolver::getLastUpdatedBy;
  using BaseResolver::getName;
  using BaseResolver::getUpdateDate;

  explicit ItemBrandResolver(std::shared_ptr<omnisphere::omnierp::models::ItemBrand> model,
                             std::shared_ptr<ServiceInyector> services)
      : BaseResolver(std::move(model), std::move(services)) {}
};