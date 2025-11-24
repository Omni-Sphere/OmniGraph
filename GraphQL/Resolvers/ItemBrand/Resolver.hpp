#pragma once 

#include "GraphQL/Generated/ItemBrandObject.h"
#include "Models/ItemBrand.hpp"

#include "Resolvers/BaseResolver.hpp"
#include "ServiceInyector.hpp"

class ItemBrandResolver : public BaseResolver<omnicore::model::ItemBrand>
{
public:
    using BaseResolver::getEntry;
    using BaseResolver::getCode;
    using BaseResolver::getName;
    using BaseResolver::getCreatedBy;
    using BaseResolver::getCreateDate;
    using BaseResolver::getLastUpdatedBy;
    using BaseResolver::getUpdateDate;

    explicit ItemBrandResolver(std::shared_ptr<omnicore::model::ItemBrand> model, std::shared_ptr<ServiceInyector> services)
        : BaseResolver(std::move(model), std::move(services))
    {}
};