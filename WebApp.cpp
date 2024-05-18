#include <iostream>
#include <string>

#include <boost/json.hpp>
#include <bserv/common.hpp>
#include <jwt-cpp/jwt.h>

#include "orm.h"

using std::cerr;
using std::cout;
using std::endl;

boost::json::object hello()
{
    return {{"msg", "hello, world!"}};
}
boost::json::object echo(
    boost::json::object &&params)
{
    return params;
}
boost::json::object greet(
    const std::string &id_str)
{
    return {{"hello", id_str}};
}

// ----------------------------------------------

// /home/category/head
boost::json::object getCategoryAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response)
{
    auto res = utils_select(conn, "SELECT * FROM category", orm_category);
    boost::json::array json_res;
    for (auto &category : res)
    {
        boost::json::object json_category;
        json_category["id"] = category["id"];
        json_category["name"] = category["name"];
        json_category["picture"] = category["picture"];
        json_category["children"] = boost::json::parse(category["children"].as_string());
        json_category["goods"] = boost::json::parse(category["goods"].as_string());
        json_res.push_back(json_category);
    }
    return utils_res(json_res, response);
}

// -------------------------------
// /home/banner

boost::json::object getBannerAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response)
{
    auto res = utils_select(conn, "SELECT * FROM banner", orm_banner);
    boost::json::array json_res;
    for (auto &category : res)
    {
        boost::json::object json_category;
        json_category["id"] = category["id"];
        json_category["imgUrl"] = category["imgUrl"];
        json_category["hrefUrl"] = category["hrefUrl"];
        json_category["type"] = category["type"];
        json_res.push_back(json_category);
    }
    return utils_res(json_res, response);
}

// /home/hot
boost::json::object getHotAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response)
{
    auto res = utils_select(conn, "SELECT * FROM hot", orm_hot);
    boost::json::array json_res;
    for (auto &category : res)
    {
        boost::json::object json_category;
        json_category["id"] = category["id"];
        json_category["picture"] = category["picture"];
        json_category["title"] = category["title"];
        json_category["alt"] = category["alt"];
        json_res.push_back(json_category);
    }
    return utils_res(json_res, response);
}

// /home/new
boost::json::object getNewAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response)
{
    auto res = utils_select(conn, "SELECT * FROM new", orm_new);
    boost::json::array json_res;
    for (auto &category : res)
    {
        boost::json::object json_category;
        json_category["id"] = category["id"];
        json_category["name"] = category["name"];
        json_category["desc"] = category["desc"];
        json_category["price"] = category["price"];
        json_category["picture"] = category["picture"];
        json_category["discount"] = category["discount"];
        json_category["orderNum"] = category["orderNum"];
        json_res.push_back(json_category);
    }
    return utils_res(json_res, response);
}

// /home/goods
boost::json::object getGoodsAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response)
{
    auto res = utils_select(conn, "SELECT * FROM categoods", orm_categoods);
    boost::json::array json_res;
    for (auto &category : res)
    {
        boost::json::object json_category;
        json_category["id"] = category["id"];
        json_category["name"] = category["name"];
        json_category["picture"] = category["picture"];
        json_category["saleInfo"] = category["saleInfo"];
        json_category["children"] = boost::json::parse(category["children"].as_string());
        json_category["goods"] = boost::json::parse(category["goods"].as_string());
        json_res.push_back(json_category);
    }
    return utils_res(json_res, response);
}

// /category?id=1005000
boost::json::object getTopCategoryAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{

    std::string query_str = "SELECT * FROM category WHERE id='";
    query_str += params["id"].as_string();
    query_str += "'";

    bserv::db_transaction tx{conn};
    bserv::db_result db_res = tx.exec(query_str);
    auto res = orm_category.convert_to_vector(db_res);

    boost::json::object json_res = res[0];
    boost::json::array children = boost::json::parse(json_res["children"].as_string()).as_array();

    json_res.erase("children");
    json_res.erase("goods");
    json_res["children"] = bserv::json::array{};
    for (auto &child : children)
    {
        auto subItem = child.as_object();
        bserv::json::object subData{};
        subData["id"] = subItem["id"];
        subData["name"] = subItem["name"];
        subData["picture"] = subItem["picture"];
        subData["goods"] = boost::json::array();

        std::string query_str = R"(SELECT "id", "name", "desc", "price", "mainPictures" FROM goods WHERE categories_2=')";
        query_str += subData["id"].as_string();
        query_str += "'";

        bserv::db_relation_to_object orm{
            bserv::make_db_field<std::string>("id"),                         // 映射 id 字段
            bserv::make_db_field<std::string>("name"),                       // 映射 name 字段
            bserv::make_db_field<std::string>("desc"),                       // 映射 desc 字段
            bserv::make_db_field<std::string>("price"),                      // 映射 price 字段
            bserv::make_db_field<std::optional<std::string>>("mainPictures") // 映射 mainPictures 字段 json
        };
        bserv::db_result db_res = tx.exec(query_str);
        auto res = orm.convert_to_vector(db_res);
        for (auto &good : res)
        {
            good["picture"] = bserv::json::parse(good["mainPictures"].as_string()).as_array()[0];
            good.erase("mainPictures");
            subData["goods"].as_array().push_back(good);
        }
        json_res["children"].as_array().push_back(subData);
    }
    return utils_res(json_res, response);
}

// details.js/getDetail(id)
boost::json::object getDetail(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{

    std::string query_str = "SELECT * FROM goods WHERE id='";
    query_str += params["id"].as_string();
    query_str += "'";

    auto res = utils_select(conn, query_str, orm_goods)[0];
    boost::json::object json_res;
    json_res["id"] = res["id"];
    json_res["name"] = res["name"];
    json_res["desc"] = res["desc"];
    json_res["price"] = res["price"];
    json_res["oldPrice"] = res["oldPrice"];
    json_res["discount"] = res["discount"];
    json_res["inventory"] = res["inventory"];
    json_res["brand"] = boost::json::parse(res["brand"].as_string());
    json_res["salesCount"] = res["salesCount"];
    json_res["commentCount"] = res["commentCount"];
    json_res["collectCount"] = res["collectCount"];
    json_res["mainPictures"] = boost::json::parse(res["mainPictures"].as_string());
    json_res["specs"] = boost::json::parse(res["specs"].as_string());
    json_res["skus"] = boost::json::parse(res["skus"].as_string());
    json_res["categories_1"] = res["categories_1"];
    json_res["categories_2"] = res["categories_2"];
    json_res["categories"] = boost::json::parse(res["categories"].as_string());
    json_res["details.pictures"] = boost::json::parse(res["details.pictures"].as_string());
    json_res["details.properties"] = boost::json::parse(res["details.properties"].as_string());
    return utils_res(json_res, response);
}

// user.js/loginAPI(user)
boost::json::object loginAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{
    std::string query_str = "SELECT * FROM \"user\" WHERE account='";
    query_str += params["account"].as_string();
    query_str += "'";

    auto res = utils_select(conn, query_str, orm_user);
    if (res.empty())
        return {{"message", "用户不存在"},
                {"code", "17001"}};
    if (res[0]["password"].as_string() != params["password"].as_string())
        return {{"message", "密码错误"},
                {"code", "17002"}};

    auto token = jwt::create()
                     .set_payload_claim("account", jwt::claim(std::string(params["account"].as_string())))
                     .sign(jwt::algorithm::hs256{SECRET_KEY});
    boost::json::object json_res = res[0];
    json_res["token"] = token;
    return utils_res(json_res, response);
}

// /member/cart
// cart.js/findNewCartListAPI() token
boost::json::object findNewCartListAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{
    std::string account = jwt::decode(std::string(params["token"].as_string())).get_payload_claim("account").as_string();
    std::string query_str = "SELECT * FROM cart WHERE account='" + account + "'";
    auto res = utils_select(conn, query_str, orm_cart);
    boost::json::array json_res;
    for (auto &good : res)
    {
        json_res.push_back(good);
    }
    return utils_res(json_res, response);
}

// /member/cart/add
// cart.js/insertCartAPI(goods) token
boost::json::object insertCartAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{

    std::string account = jwt::decode(std::string(params["token"].as_string())).get_payload_claim("account").as_string();
    std::string query_str = "SELECT * FROM cart WHERE account='" + account + "' AND \"skuId\"='";
    query_str += params["skuId"].as_string();
    query_str += "'";

    auto res = utils_select(conn, query_str, orm_cart);
    bserv::db_transaction tx{conn};
    if (res.size() > 0)
    {
        query_str = "UPDATE cart SET count = count + ? WHERE account=? AND \"skuId\"=?";
        auto res = tx.exec(query_str, params["count"].as_string(), account, params["skuId"].as_string());
        tx.commit();
        lginfo << res.query();

        response.result(200);
        response.set("Access-Control-Allow-Origin", "*");
        response.set("Access-Control-Allow-Headers", "*");
        response.set("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
        response.set("Cache-Control", "no-store");
        return {{"code", "1"},
                {"msg", "操作成功"}};
    }

    auto goodsId = params["goodsId"].as_string();
    auto name = params["name"].as_string();
    auto picture = params["picture"].as_string();
    auto price = params["price"].as_string();
    auto count = params["count"].as_string();
    auto attrsText = params["attrsText"].as_string();
    auto selected = params["selected"].as_string();
    query_str = "INSERT INTO cart (\"account\", \"goodsId\", \"name\", \"picture\", \"price\", \"count\", \"skuId\", \"attrsText\", \"selected\") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    bserv::db_result r = tx.exec(query_str, account, goodsId, name, picture, price, count, params["skuId"].as_string(), attrsText, selected);
    lginfo << r.query();
    tx.commit();

    response.result(200);
    response.set("Access-Control-Allow-Origin", "*");
    response.set("Access-Control-Allow-Headers", "*");
    response.set("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    response.set("Cache-Control", "no-store");
    return {{"code", "1"},
            {"msg", "操作成功"}};
}

// /member/cart/delete
// cart.js/delCartAPI(ids) token
boost::json::object delCartAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{
    cerr << params << endl;
    std::string account = jwt::decode(std::string(params["token"].as_string())).get_payload_claim("account").as_string();
    std::string query_str = "DELETE FROM cart WHERE account=? AND \"skuId\"=?";
    bserv::db_transaction tx{conn};
    auto res = tx.exec(query_str, account, params["skuId"].as_string());
    tx.commit();
    lginfo << res.query();

    response.result(200);
    response.set("Access-Control-Allow-Origin", "*");
    response.set("Access-Control-Allow-Headers", "*");
    response.set("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    response.set("Cache-Control", "no-store");
    return {{"code", "1"},
            {"msg", "操作成功"}};
}

// cart.js/updateCartAPI(skuId, data:{ count, selected }) token
boost::json::object updateCartAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params,
    const std::string &skuId)
{
    std::string account = jwt::decode(std::string(params["token"].as_string())).get_payload_claim("account").as_string();
    std::string query_str = "UPDATE cart SET count = ?, selected = ? WHERE account=? AND \"skuId\"=?";
    bserv::db_transaction tx{conn};
    auto res = tx.exec(query_str, params["count"].as_string(), params["selected"].as_string(), account, skuId);
    tx.commit();
    lginfo << res.query();

    response.result(200);
    response.set("Access-Control-Allow-Origin", "*");
    response.set("Access-Control-Allow-Headers", "*");
    response.set("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    response.set("Cache-Control", "no-store");
    return {{"code", "1"},
            {"msg", "操作成功"}};
}

// member/order/pre
// checkout.js/getCheckInfoAPI() token
boost::json::object getCheckInfoAPI(
    std::shared_ptr<bserv::db_connection> conn,
    bserv::response_type &response,
    boost::json::object &&params)
{
    cerr << params << endl;
    std::string account = jwt::decode(std::string(params["token"].as_string())).get_payload_claim("account").as_string();
    std::string query_str = "SELECT * FROM \"userAddress\" WHERE account='";
    query_str += account;
    query_str += "'";
    auto res = utils_select(conn, query_str, orm_userAddress);
    boost::json::object json_res;
    json_res["userAddresses"] = bserv::json::array{};
    for (auto &address : res)
    {
        json_res["userAddresses"].as_array().push_back(address);
    }

    query_str = "SELECT * FROM cart WHERE account='" + account + "' AND selected=true";
    auto res2 = utils_select(conn, query_str, orm_cart);
    boost::json::array goods;
    for (auto &good : res2)
    {
        cerr << good << endl;
        auto totalPrice = std::stod(std::string(good["price"].as_string())) * good["count"].as_int64();
        good["totalPrice"] = std::to_string(totalPrice);
        goods.push_back(good);
    }
    json_res["goods"] = goods;

    int goodsCount = goods.size();
    int totalPrice = 0;
    for (auto &good : goods)
    {
        totalPrice += std::stoi(std::string(good.as_object()["totalPrice"].as_string()));
    }
    int postFee = 8;
    int totalPayPrice = totalPrice + postFee;
    json_res["summary"] = {
        {"goodsCount", goodsCount},
        {"totalPrice", totalPrice},
        {"postFee", postFee},
        {"totalPayPrice", totalPayPrice}};

    return utils_res(json_res, response);
}

// -------------------------------------------------

int main(int argc, char *argv[])
{
    bserv::server_config config;

    config.set_port(3000);
    config.set_num_threads(1);
    config.set_num_db_conn(8);
    config.set_db_conn_str("postgresql://ethan:123456@127.0.0.1:5432/ethan");
    std::cout << config.get_name() << " config:"
              << "\nport: " << config.get_port()
              << "\nthreads: " << config.get_num_threads()
              << "\nrotation: " << config.get_log_rotation_size() / 1024 / 1024
              << "\nlog path: " << config.get_log_path()
              << "\ndb-conn: " << config.get_num_db_conn()
              << "\nconn-str: " << config.get_db_conn_str() << std::endl;

    bserv::server{config, {

                              bserv::make_path("/hello", &hello),
                              bserv::make_path("/echo", &echo, bserv::placeholders::json_params),
                              bserv::make_path("/greet/<str>", &greet, bserv::placeholders::_1),

                              bserv::make_path("/home/category/head", &getCategoryAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response),
                              bserv::make_path("/home/banner", &getBannerAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response),
                              bserv::make_path("/home/hot", &getHotAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response),
                              bserv::make_path("/home/new", &getNewAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response),
                              bserv::make_path("/home/goods", &getGoodsAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response),

                              bserv::make_path("/category", &getTopCategoryAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                              bserv::make_path("/goods", &getDetail, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),

                              bserv::make_path("/login_form", &loginAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                              bserv::make_path("/member/cart", &findNewCartListAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                              bserv::make_path("/member/cart/add", &insertCartAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                              bserv::make_path("/member/cart/delete", &delCartAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                              bserv::make_path("/member/cart/put/<str>", &updateCartAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params, bserv::placeholders::_1),

                              bserv::make_path("/member/order/pre", &getCheckInfoAPI, bserv::placeholders::db_connection_ptr, bserv::placeholders::response, bserv::placeholders::json_params),
                          }};
}
