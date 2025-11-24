#pragma once
#include "GraphQL/Generated/ItemGroupObject.h"
#include "Models/ItemGroup.hpp"
#include "ServiceInyector.hpp"
#include "Resolvers/User/Resolver.hpp"
#include "Resolvers/BaseResolver.hpp"

class ItemGroupResolver : public BaseResolver<omnicore::model::ItemGroup>
{
public:
    using BaseResolver::getEntry;
    using BaseResolver::getCode;
    using BaseResolver::getName;
    using BaseResolver::getCreatedBy;
    using BaseResolver::getCreateDate;
    using BaseResolver::getLastUpdatedBy;
    using BaseResolver::getUpdateDate;
    
    explicit ItemGroupResolver(std::shared_ptr<omnicore::model::ItemGroup> model, std::shared_ptr<ServiceInyector> _services)
        : BaseResolver(std::move(model), std::move(_services))
    {}

};