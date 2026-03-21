#pragma once

#include "GraphQL/Generated/AreaObject.h"
#include "GraphQL/Generated/FloorObject.h"
#include "Area/Models/Area.hpp"
#include "Resolvers/BaseResolver.hpp"
#include "Resolvers/Floor/Resolver.hpp"
#include "ServiceInyector.hpp"

class AreaResolver : public BaseResolver<omnisphere::models::Area> {
public:
  using BaseResolver::getCode;
  using BaseResolver::getCreateDate;
  using BaseResolver::getCreatedBy;
  using BaseResolver::getEntry;
  using BaseResolver::getLastUpdatedBy;
  using BaseResolver::getName;
  using BaseResolver::getUpdateDate;

  explicit AreaResolver(std::shared_ptr<omnisphere::models::Area> model,
                        std::shared_ptr<ServiceInyector> services)
      : BaseResolver(std::move(model), services), m_services(services) {}

  graphql::service::AwaitableScalar<std::optional<std::string>>
  getColor(graphql::service::FieldParams &&) const {
    co_return m_model->Color;
  }

  graphql::service::AwaitableScalar<std::optional<int>>
  getCapacity(graphql::service::FieldParams &&) const {
    co_return m_model->Capacity;
  }

  graphql::service::AwaitableScalar<int>
  getFloor(graphql::service::FieldParams &&) const {
    co_return m_model->FloorEntry;
  }

private:
  std::shared_ptr<ServiceInyector> m_services;
};
