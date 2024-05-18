#include <iostream>
#include <string>
#include <boost/json.hpp>
#include <bserv/common.hpp>

std::vector<boost::json::object> utils_select(
    std::shared_ptr<bserv::db_connection> conn,
    std::string query_str,
    bserv::db_relation_to_object orm)
{
    bserv::db_transaction tx{conn};
    bserv::db_result db_res = tx.exec(query_str);
    tx.commit();
    // lginfo << db_res.query();
    return orm.convert_to_vector(db_res);
}

template <typename T = boost::json::array>
boost::json::object utils_res(T &json_res, bserv::response_type &response)
{
    response.result(200);
    response.set("Access-Control-Allow-Origin", "*");
    response.set("Access-Control-Allow-Headers", "*");
    response.set("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    response.set("Cache-Control", "no-store");
    return {
        {"code", "1"},
        {"msg", "操作成功"},
        {"result", json_res}};
}

const std::string SECRET_KEY = "SpyFamily";

// CREATE TABLE "category" (
//   "id" text PRIMARY KEY,
//   "name" text,
//   "picture" text,
//   "children" json,
//   "goods" json
// );
bserv::db_relation_to_object orm_category{
    bserv::make_db_field<std::string>("id"),       // 映射 id 字段
    bserv::make_db_field<std::string>("name"),     // 映射 name 字段
    bserv::make_db_field<std::string>("picture"),  // 映射 picture 字段
    bserv::make_db_field<std::string>("children"), // 映射 children 字段 json
    bserv::make_db_field<std::string>("goods")     // 映射 goods 字段 json
};

// CREATE TABLE "banner" (
//   "id" text,
//   "imgUrl" text,
//   "hrefUrl" text,
//   "type" text
// );
bserv::db_relation_to_object orm_banner{
    bserv::make_db_field<std::string>("id"),      // 映射 id 字段
    bserv::make_db_field<std::string>("imgUrl"),  // 映射 imgUrl 字段
    bserv::make_db_field<std::string>("hrefUrl"), // 映射 hrefUrl 字段
    bserv::make_db_field<std::string>("type")     // 映射 type 字段
};

// CREATE TABLE "hot" (
//   "id" text,
//   "picture" text,
//   "title" text,
//   "alt" text
// );
bserv::db_relation_to_object orm_hot{
    bserv::make_db_field<std::string>("id"),      // 映射 id 字段
    bserv::make_db_field<std::string>("picture"), // 映射 picture 字段
    bserv::make_db_field<std::string>("title"),   // 映射 title 字段
    bserv::make_db_field<std::string>("alt")      // 映射 alt 字段
};

// CREATE TABLE "new" (
//   "id" text,
//   "name" text,
//   "desc" text,
//   "price" text,
//   "picture" text,
//   "discount" text NULL,
//   "orderNum" bigint
// );
bserv::db_relation_to_object orm_new{
    bserv::make_db_field<std::string>("id"),                      // 映射 id 字段
    bserv::make_db_field<std::string>("name"),                    // 映射 name 字段
    bserv::make_db_field<std::string>("desc"),                    // 映射 desc 字段
    bserv::make_db_field<std::string>("price"),                   // 映射 price 字段
    bserv::make_db_field<std::string>("picture"),                 // 映射 picture 字段
    bserv::make_db_field<std::optional<std::string>>("discount"), // 映射 discount 字段
    bserv::make_db_field<int64_t>("orderNum")                     // 映射 orderNum 字段
};

// CREATE TABLE "categoods" (
//   "id" text,
//   "name" text,
//   "picture" text,
//   "saleInfo" text,
//   "children" json,
//   "goods" json
// );
bserv::db_relation_to_object orm_categoods{
    bserv::make_db_field<std::string>("id"),       // 映射 id 字段
    bserv::make_db_field<std::string>("name"),     // 映射 name 字段
    bserv::make_db_field<std::string>("picture"),  // 映射 picture 字段
    bserv::make_db_field<std::string>("saleInfo"), // 映射 saleInfo 字段
    bserv::make_db_field<std::string>("children"), // 映射 children 字段 json
    bserv::make_db_field<std::string>("goods")     // 映射 goods 字段 json
};

// CREATE TABLE "goods" (
//   "id" text PRIMARY KEY,
//   "name" text,
//   "desc" text,
//   "price" text,
//   "oldPrice" text,
//   "discount" real,
//   "inventory" bigint,
//   "brand" json NULL,
//   "salesCount" bigint,
//   "commentCount" bigint,
//   "collectCount" bigint,
//   "mainPictures" json,
//   "specs" json,
//   "skus" json,
//   "categories_1" text,
//   "categories_2" text,
//   "categories" json,
//   "details.pictures" json,
//   "details.properties" json
// );
bserv::db_relation_to_object orm_goods{
    bserv::make_db_field<std::string>("id"),                   // 映射 id 字段
    bserv::make_db_field<std::string>("name"),                 // 映射 name 字段
    bserv::make_db_field<std::string>("desc"),                 // 映射 desc 字段
    bserv::make_db_field<std::string>("price"),                // 映射 price 字段
    bserv::make_db_field<std::string>("oldPrice"),             // 映射 oldPrice 字段
    bserv::make_db_field<double>("discount"),                  // 映射 discount 字段
    bserv::make_db_field<int64_t>("inventory"),                // 映射 inventory 字段
    bserv::make_db_field<std::optional<std::string>>("brand"), // 映射 brand 字段 json
    bserv::make_db_field<int64_t>("salesCount"),               // 映射 salesCount 字段
    bserv::make_db_field<int64_t>("commentCount"),             // 映射 commentCount 字段
    bserv::make_db_field<int64_t>("collectCount"),             // 映射 collectCount 字段
    bserv::make_db_field<std::string>("mainPictures"),         // 映射 mainPictures 字段 json
    bserv::make_db_field<std::string>("specs"),                // 映射 specs 字段 json
    bserv::make_db_field<std::string>("skus"),                 // 映射 skus 字段 json
    bserv::make_db_field<std::string>("categories_1"),         // 映射 categories_1 字段
    bserv::make_db_field<std::string>("categories_2"),         // 映射 categories_2 字段
    bserv::make_db_field<std::string>("categories"),           // 映射 categories 字段 json
    bserv::make_db_field<std::string>("details.pictures"),     // 映射 details.pictures 字段 json
    bserv::make_db_field<std::string>("details.properties")    // 映射 details.properties 字段 json
};

// CREATE TABLE "user" (
//   "id" SERIAL PRIMARY KEY,
//   "account" text,
//   "password" text,
//   "mobile" text NULL,
//   "email" text NULL,
//   "nickname" text NULL,
//   "avatar" text NULL,
//   "gender" text NULL,
//   "birthday" date NULL
// );
bserv::db_relation_to_object orm_user{
    bserv::make_db_field<int64_t>("id"),                          // 映射 id 字段
    bserv::make_db_field<std::string>("account"),                 // 映射 account 字段
    bserv::make_db_field<std::string>("password"),                // 映射 password 字段
    bserv::make_db_field<std::optional<std::string>>("mobile"),   // 映射 mobile 字段
    bserv::make_db_field<std::optional<std::string>>("email"),    // 映射 email 字段
    bserv::make_db_field<std::optional<std::string>>("nickname"), // 映射 nickname 字段
    bserv::make_db_field<std::optional<std::string>>("avatar"),   // 映射 avatar 字段
    bserv::make_db_field<std::optional<std::string>>("gender"),   // 映射 gender 字段
    bserv::make_db_field<std::optional<std::string>>("birthday")  // 映射 birthday 字段
};

// CREATE TABLE "cart" (
//   "id" SERIAL PRIMARY KEY,
//   "account" TEXT,
//   "goodsId" TEXT,
//   "name" text,
//   "picture" text,
//   "price" text,
//   "count" bigint,
//   "skuId" text,
//   "attrsText" text NULL,
//   "selected" boolean,
//   "specs" json NULL,
//   "postFee" real NULL
// );
bserv::db_relation_to_object orm_cart{
    bserv::make_db_field<int64_t>("id"),                           // 映射 id 字段
    bserv::make_db_field<std::string>("account"),                  // 映射 account 字段
    bserv::make_db_field<std::string>("goodsId"),                  // 映射 goodsId 字段
    bserv::make_db_field<std::string>("name"),                     // 映射 name 字段
    bserv::make_db_field<std::string>("picture"),                  // 映射 picture 字段
    bserv::make_db_field<std::string>("price"),                    // 映射 price 字段
    bserv::make_db_field<int64_t>("count"),                        // 映射 count 字段
    bserv::make_db_field<std::string>("skuId"),                    // 映射 skuId 字段
    bserv::make_db_field<std::optional<std::string>>("attrsText"), // 映射 attrsText 字段
    bserv::make_db_field<bool>("selected"),                        // 映射 selected 字段
    bserv::make_db_field<std::optional<std::string>>("specs"),     // 映射 specs 字段 json
    bserv::make_db_field<std::optional<double>>("postFee")         // 映射 postFee 字段
};


// CREATE TABLE "userAddress" (
//   "id" SERIAL PRIMARY KEY,
//   "account" text,
//   "receiver" text,
//   "contact" text,
//   "fullLocation" text,
//   "address" text,
//   "isDefault" int
// );
bserv::db_relation_to_object orm_userAddress{
    bserv::make_db_field<int64_t>("id"),                           // 映射 id 字段
    bserv::make_db_field<std::string>("account"),                  // 映射 account 字段
    bserv::make_db_field<std::string>("receiver"),                 // 映射 receiver 字段
    bserv::make_db_field<std::string>("contact"),                  // 映射 contact 字段
    bserv::make_db_field<std::string>("fullLocation"),             // 映射 fullLocation 字段
    bserv::make_db_field<std::string>("address"),                  // 映射 address 字段
    bserv::make_db_field<int>("isDefault")                         // 映射 isDefault 字段
};