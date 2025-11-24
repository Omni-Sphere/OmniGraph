#pragma once
#include <memory>
#include "Database.hpp"
#include "User.hpp"
#include "Item.hpp"
#include "Session.hpp"
#include "ItemGroup.hpp"
#include "ItemBrand.hpp"

class ServiceInyector {
private:
    std::shared_ptr<omnicore::service::Database> _db;

public:
    std::shared_ptr<omnicore::service::User> User;
    std::shared_ptr<omnicore::service::Item> Item;
    std::shared_ptr<omnicore::service::Session> Session;
    std::shared_ptr<omnicore::service::ItemGroup> ItemGroup;
    std::shared_ptr<omnicore::service::ItemBrand> ItemBrand;

    explicit ServiceInyector(std::shared_ptr<omnicore::service::Database> db)
        : _db(std::move(db))
    {
        User = std::make_shared<omnicore::service::User>(_db);
        Item = std::make_shared<omnicore::service::Item>(_db);
        Session = std::make_shared<omnicore::service::Session>(_db);
        ItemGroup = std::make_shared<omnicore::service::ItemGroup>(_db);
        ItemBrand = std::make_shared<omnicore::service::ItemBrand>(_db);
    }
};
